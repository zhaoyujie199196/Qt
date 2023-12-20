//
// Created by Yujie Zhao on 2023/2/24.
//

#include "qobject.h"
#include "qcoreevent.h"
#include "qobject_p.h"
#include "qmetaobject_p.h"
#include <mutex>
#include <QtCore/private/qthread_p.h>
#include <QtCore/qvariant.h>
#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qlogging.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/private/qorderedmutexlocker_p.h>
#include <QtCore/private/qtrace_p.h>
#include <QtCore/private/qhooks_p.h>

QT_BEGIN_NAMESPACE

static int DIRECT_CONNECTION_ONLY = 0;

static QBasicMutex _q_ObjectMutexPool[131];

Q_LOGGING_CATEGORY(lcConnect, "qt.core.qobject.connect")

/**
 * \internal
 * mutex to be locked when accessing the connection lists or the senders list
 */
static inline QBasicMutex *signalSlotLock(const QObject *o)
{
    return &_q_ObjectMutexPool[uint(quintptr(o)) % sizeof(_q_ObjectMutexPool) / sizeof(QBasicMutex)];
}

QBasicAtomicPointer<QSignalSpyCallbackSet> qt_signal_spy_callback_set = Q_BASIC_ATOMIC_INITIALIZER(nullptr);

void qt_register_signal_spy_callbacks(QSignalSpyCallbackSet *callback_set)
{
    qt_signal_spy_callback_set.storeRelease(callback_set);
}

void (*QAbstractDeclarativeData::destroyed)(QAbstractDeclarativeData *, QObject *) = nullptr;
void (*QAbstractDeclarativeData::signalEmitted)(QAbstractDeclarativeData *, QObject *, int, void **) = nullptr;
int  (*QAbstractDeclarativeData::receivers)(QAbstractDeclarativeData *, const QObject *, int) = nullptr;
bool (*QAbstractDeclarativeData::isSignalConnected)(QAbstractDeclarativeData *, const QObject *, int) = nullptr;
void (*QAbstractDeclarativeData::setWidgetParent)(QObject *, QObject *) = nullptr;

const char *qFlagLocation(const char *method)
{
    //zhaoyujie TODO
    return method;
}

//字符串的第一位添加了方法类型 (method / signal / slot)
static int extract_code(const char *member)
{
    return (((int)(*member) - '0') & 0x3);
}

static const char *extract_location(const char *member)
{
    if (QThreadData::current()->flaggedSignatures.contains(member)) {
        const char *location = member + qstrlen(member) + 1;
        if (*location != '\0') {
            return location;
        }
    }
    return nullptr;
}

//检测是否是信号  zhaoyujie TODO 这段代码什么意思？
static bool check_signal_macro(const QObject *sender, const char *signal, const char *func, const char *op) {
    int sigcode = extract_code(signal);
    if (sigcode != QSIGNAL_CODE) {
        if (sigcode == QSLOT_CODE) {
            //将SLOT当作了信号
            Q_ASSERT(false);
        }
        else {
            //没用使用SIGNAL包裹
            Q_ASSERT(false);
        }
        return false;
    }
    return true;
}

static bool check_method_code(int code, const QObject *object, const char *method, const char *func)
{
    if (code != QSLOT_CODE && code != QSIGNAL_CODE) {
        Q_ASSERT(false);
        return false;
    }
    return true;
}

static void err_method_notfound(const QObject *object, const char *method, const char *func)
{
    const char *type = "method";
    switch (extract_code(method)) {
        case QSLOT_CODE: type = "slot"; break;
        case QSIGNAL_CODE: type = "signal"; break;
    }
    const char *loc = extract_location(method);
    if (strchr(method, ')') == nullptr) {
        qCWarning(lcConnect, "QObject::%s: Parentheses expected, %s %s::%s%s%s", func, type,
                  object->metaObject()->className(), method + 1, loc ? " in " : "", loc ? loc : "");
    }
    else {
        qCWarning(lcConnect, "QObject::%s: No such %s %s::%s%s%s", func, type,
                  object->metaObject()->className(), method + 1, loc ? " in " : "", loc ? loc : "");
    }
}

void qt_qFindChildren_helper(const QObject *parent, const QString &name, const QMetaObject &mo, QList<void *> *list, Qt::FindChildOptions options)
{
    Q_ASSERT(parent);
    Q_ASSERT(list);
    const QObjectList &children = parent->children();
    QObject *obj;
    for (int i = 0; i < children.size(); ++i) {
        obj = children.at(i);
        if (mo.cast(obj)) {
            if (name.isNull() || obj->objectName() == name) {
                list->append(obj);
            }
        }
        if (options & Qt::FindChildrenRecursively) {
            qt_qFindChildren_helper(obj, name, mo, list, options);
        }
    }
}

static bool check_parent_thread(QObject *parent, QThreadData *parentThreadData, QThreadData *currentThreadData)
{
    if (parent && parentThreadData != currentThreadData) {
        QThread *parentThread = parentThreadData->thread.loadAcquire();
        QThread *currentThread = currentThreadData->thread.loadAcquire();
        qWarning("QObject: Cannot create children for a parent that is in a different thread.\n"
                 "(Parent is %s(%p), parent's thread is %s(%p), current thread is %s(%p)",
                 parent->metaObject()->className(),
                 parent,
                 parentThread ? parentThread->className().toStdString().c_str() : "QThread",
                 parentThread,
                 currentThread ? currentThread->className().toStdString().c_str() : "QThread",
                 currentThread);
        return false;
    }
    return true;
}

struct SlotObjectGuard {
    SlotObjectGuard() = default;
    Q_DISABLE_COPY_MOVE(SlotObjectGuard)
    explicit SlotObjectGuard(QtPrivate::QSlotObjectBase *slotObject)
            : m_slotObject(slotObject)
    {
        if (m_slotObject) {
            m_slotObject->ref();
        }
    }

    QtPrivate::QSlotObjectBase const *operator->() const
    {
        return m_slotObject;
    }

    QtPrivate::QSlotObjectBase *operator->()
    {
        return m_slotObject;
    }

    ~SlotObjectGuard() {
        if (m_slotObject) {
            m_slotObject->destroyIfLastRef();
        }
    }
private:
    QtPrivate::QSlotObjectBase *m_slotObject = nullptr;
};

QMetaObject *QObjectData::dynamicMetaObject() const
{
    return metaObject->toDynamicMetaObject(q_ptr);
}

QObjectPrivate::Connection *QMetaObjectPrivate::connect(const QObject *sender, int signal_index, const QMetaObject *smeta,
                                                        const QObject *receiver, int method_index, const QMetaObject *rmeta,
                                                        int type, int *types) {
    QObject *s = const_cast<QObject *>(sender);
    QObject *r = const_cast<QObject *>(receiver);

    int method_offset = rmeta ? rmeta->methodOffset() : 0;
    Q_ASSERT(!rmeta || QMetaObjectPrivate::get(rmeta)->revision >= 6);
    QObjectPrivate::StaticMetaCallFunction callFunction = rmeta ? rmeta->d.static_metacall : nullptr;

    QOrderedMutexLocker locker(signalSlotLock(sender), signalSlotLock(receiver));

    QObjectPrivate::ConnectionData *scd = QObjectPrivate::get(s)->connections.loadRelaxed();
    if (type & Qt::UniqueConnection && scd) {
#pragma message("UniqueConnection 未实现 重写")
        Q_ASSERT(false);
    }
    type &= ~Qt::UniqueConnection;
    const bool isSingleShot = type & Qt::SingleShotConnection;
    type &= ~Qt::SingleShotConnection;

    Q_ASSERT(type >= 0);
    Q_ASSERT(type <= 3);

    auto c = new QObjectPrivate::Connection();
    c->sender = s;
    c->signal_index = signal_index;
    c->receiver.storeRelaxed(r);

    QThreadData *td = r->d_func()->threadData;
    td->ref();
    c->receiverThreadData.storeRelaxed(td);
    c->method_relative = method_index;
    c->method_offset = method_offset;
    c->connectionType = type;
    c->isSlotObject = false;
    c->argumentTypes.storeRelaxed(types);
    if (types) {
        Q_ASSERT(false);
    }
    c->callFunction = callFunction;
    c->isSingleShot = isSingleShot;

    QObjectPrivate::get(s)->addConnection(signal_index, c);

    locker.unlock();
    QMetaMethod smethod = QMetaObjectPrivate::signal(smeta, signal_index);
    if (smethod.isValid()) {
        s->connectNotify(smethod);
    }
    return c;
}

bool QMetaObjectPrivate::disconnect(const QObject *sender, int signal_index, const QMetaObject *smeta,
                                    const QObject *receiver, int method_index, void **slot,
                                    DisconnectType disconnectType) {
    if (!sender) {
        return false;
    }
    QObject *s = const_cast<QObject *>(sender);
    QBasicMutex *senderMutex = signalSlotLock(sender);
    QBasicMutexLocker locker(senderMutex);

    QObjectPrivate::ConnectionData *scd = QObjectPrivate::get(s)->connections.loadRelaxed();
    if (!scd) {
        return false;
    }
    bool success = false;
    {
        QObjectPrivate::ConnectionDataPointer connections(scd);
        if (signal_index < 0) {
            for (int sig_index = -1; sig_index < scd->signalVectorCount(); ++sig_index) {
                if (disconnectHelper(connections.data(), sig_index, receiver, method_index, slot, senderMutex, disconnectType)) {
                    success = true;
                }
            }
        }
        else if (signal_index < scd->signalVectorCount()) {
            if (disconnectHelper(connections.data(), signal_index, receiver, method_index, slot, senderMutex, disconnectType)) {
                success = true;
            }
        }
    }

    locker.unlock();
    if (success) {
        scd->cleanOrphanedConnections(s);

        QMetaMethod smethod = QMetaObjectPrivate::signal(smeta, signal_index);
        if (smethod.isValid()) {
            s->disconnectNotify(smethod); //发送signal断开的通知
        }
    }
    return success;
}

bool QMetaObjectPrivate::disconnectHelper(QObjectPrivate::ConnectionData *connections, int signalIndex,
                                          const QObject *receiver, int method_index, void **slot,
                                          QBasicMutex *senderMutex, DisconnectType disconnectType)
{
    bool success = false;
    auto &connectionList = connections->connectionsForSignal(signalIndex);
    auto *c = connectionList.first.loadRelaxed();
    for (; c; c = c->nextConnectionList.loadRelaxed()) {
        QObject *r = c->receiver.loadRelaxed();

        auto needRemoveConnection =[&]()->bool {
            if (!r) {
                return false;
            }
            if (!receiver) {
                return true;
            }
            if (r != receiver) {
                return false;
            }
            //method_index或者slot二选一
            bool checkMethodIndex = (method_index < 0) || (!c->isSlotObject && c->method() == method_index);
            if (!checkMethodIndex) {
                return false;
            }
            bool checkSlotObject = (slot == nullptr) || (c->isSlotObject && c->slotObj->compare(slot));
            if (!checkSlotObject) {
                return false;
            }
            return true;
        };

        if (needRemoveConnection()) {
            bool needToUnlock = false;
            QBasicMutex *receiverMutex = nullptr;
            if (r) {
                receiverMutex = signalSlotLock(r);
                needToUnlock = QOrderedMutexLocker::relock(senderMutex, receiverMutex);
            }
            if (c->receiver.loadRelaxed()) {
                connections->removeConnection(c);
            }
            if (needToUnlock) {
                receiverMutex->unlock();
            }
            success = true;
            if (disconnectType == DisconnectOne) {
                return success;
            }
        }
    }
    return success;
}


QObjectPrivate::QObjectPrivate(int version)
    : threadData(nullptr), currentChildBeingDeleted(nullptr)
{
    q_ptr = nullptr;
    parent = nullptr;
    isWidget = false;
    blockSig = false;
    wasDeleted = false;
    isDeletingChildren = false;
    sendChildEvents = true;
    receiveChildEvents = true;
    postedEvents = 0;
    extraData = nullptr;
    metaObject = nullptr;
    isWindow = false;
    deleteLaterCalled = false;
}

QObjectPrivate::~QObjectPrivate()
{
    delete extraData;
}

void QObjectPrivate::addConnection(int signal, Connection *c)
{
    Q_ASSERT(c->sender == q_ptr);  //发送者得是q指针
    ensureConnectionData();
    ConnectionData *cd = connections.loadRelaxed();
    cd->resizeSignalVector(signal + 1);  //内部存取是使用signal的索引做的一个vector
    //每一个信号可以连接多个槽
    ConnectionList &connectionList = cd->connectionsForSignal(signal);
    if (connectionList.last.loadRelaxed()) {
        //connectionList中已经有对象了
        Q_ASSERT(connectionList.last.loadRelaxed()->receiver.loadRelaxed());
        //添加到末尾
        connectionList.last.loadRelaxed()->nextConnectionList.storeRelaxed(c);
    }
    else {
        connectionList.first.storeRelaxed(c);
    }
    c->id = ++cd->currentConnectionId;
    c->prevConnectionList = connectionList.last.loadRelaxed();
    connectionList.last.storeRelaxed(c);  //last存储为c，nextConnectionList和preConnectionList作为链表的串联

    QObjectPrivate *rd = QObjectPrivate::get(c->receiver.loadRelaxed());
    rd->ensureConnectionData();

    /*
     * prev指向sender的地址
     * next指向sender
     * prev指向自身
     * c->next的prev指向c中的c->next的地址
     * */
    c->prev = &(rd->connections.loadRelaxed()->senders);
    c->next = *c->prev;
    *c->prev = c;
    if (c->next) {
        c->next->prev = &c->next;
    }
}

void QObjectPrivate::setParent_helper(QObject *o)
{
    Q_Q(QObject);
    //检测将自身设置成了自己的父
    Q_ASSERT_X(q != o, Q_FUNC_INFO, "Cannot parent a QObject to itself");

    if (o == parent) {
        return;
    }

    if (parent) {  //从parent中删除
        QObjectPrivate *parentD = parent->d_func();
        //父亲正在删除这个子，不需要操作
        if (parentD->isDeletingChildren && wasDeleted && parentD->currentChildBeingDeleted == q) {

        }
        else {
            const int index = parentD->children.indexOf(q);
            if (index < 0) {
                //可能已经从父亲中删除掉了
            }
            else if (parentD->isDeletingChildren) {  //父正处在删除子的过程中，直接设置为nullptr，防止释放内存
                parentD->children[index] = nullptr;
            }
            else {
                parentD->children.removeAt(index);
                if (sendChildEvents && parentD->receiveChildEvents) {
                    //向原先的父发送删除了子的消息
                    QChildEvent e(QEvent::ChildRemoved, q);
                    QCoreApplication::sendEvent(parent, &e);
                }
            }
        }
    }
    parent = o;
    if (parent) {
        //检测线程，不同线程不能设置父子关系
        if (threadData != parent->d_func()->threadData) {
            qWarning("QObject::setParent: Cannot set parent, new parent is in a different thread");
            parent = nullptr;
            return;
        }
        parent->d_func()->children.append(q);
        if (sendChildEvents && parent->d_func()->receiveChildEvents) {
            if (!isWidget) {
                QChildEvent e(QEvent::ChildAdded, q);
                QCoreApplication::sendEvent(parent, &e);
            }
        }
    }
}

void QObjectPrivate::ConnectionData::removeConnection(Connection *c)
{
    Q_ASSERT(c->receiver.loadRelaxed());
    ConnectionList &connections = signalVector.loadRelaxed()->at(c->signal_index);
    c->receiver.storeRelaxed(nullptr);   //将receiver设置为空
    QThreadData *td = c->receiverThreadData.loadRelaxed();
    if (td) {  //connection要被删除了，里面的智能指针之类的deref下
        td->deref();
    }
    c->receiverThreadData.storeRelaxed(nullptr);

    auto foundConnectionFunc =[&connections, &c]()->bool {
        bool found = false;
        for (Connection *cc = connections.first.loadRelaxed(); cc; cc = cc->nextConnectionList.loadRelaxed()) {
            if (cc == c) {
                found = true;
                break;
            }
        }
        return found;
    };

    Q_ASSERT(foundConnectionFunc());

    /*
     * 从senders的链表中删除
     * c->prev为指向了前一个节点的next的指针，相当于前一个preNode->next = c->next
     * c->next->prev指向前一个节点的next的指针
     * */
    *c->prev = c->next;
    if (c->next) {
        c->next->prev = c->prev;
    }
    c->prev = nullptr;
    //如果是first或者last
    if (connections.first.loadRelaxed() == c) {
        connections.first.storeRelaxed(c->nextConnectionList.loadRelaxed());
    }
    if (connections.last.loadRelaxed() == c) {
        connections.last.storeRelaxed(c->prevConnectionList);
    }
    Q_ASSERT(signalVector.loadRelaxed()->at(c->signal_index).first.loadRelaxed() != c);
    Q_ASSERT(signalVector.loadRelaxed()->at(c->signal_index).last.loadRelaxed() != c);

    //不是first和last，从connection链表中删除
    Connection *n = c->nextConnectionList.loadRelaxed();
    if (n) {
        n->prevConnectionList = c->prevConnectionList;
    }
    if (c->prevConnectionList) {
        c->prevConnectionList->nextConnectionList.storeRelaxed(n);
    }
    c->prevConnectionList = nullptr;

    Q_ASSERT(c != orphaned.loadRelaxed());
    Connection *o = nullptr;
    //将c添加到孤儿列表中
    do {
        o = orphaned.loadRelaxed();
        c->nextInOrphanList = o;
    } while (!orphaned.testAndSetRelease(o, c));

    Q_ASSERT(!foundConnectionFunc());
}

void QObjectPrivate::clearBindingStorage()
{
    bindingStorage.clear();
}

void QObjectPrivate::deleteChildren()
{
    Q_ASSERT_X(!isDeletingChildren, "QObjectPrivate::deleteChildren()", "isDeletingChildren already set, did this function recurse?");  //isDeletingChildren 已经被调用过了，已经处于删除子的过程中了
    isDeletingChildren = true;
    for (int i = 0; i < children.count(); ++i) {
        currentChildBeingDeleted = children.at(i);
        children[i] = nullptr;
        delete currentChildBeingDeleted;
    }
    children.clear();
    currentChildBeingDeleted = nullptr;
    isDeletingChildren = false;
}

int QObjectPrivate::signalIndex(const char *signalName, const QMetaObject **meta) const
{
    Q_Q(const QObject);
    const QMetaObject *base = q->metaObject();
    Q_ASSERT(QMetaObjectPrivate::get(base)->revision >= 7);
    QArgumentTypeArray types;
    QByteArray name = QMetaObjectPrivate::decodeMethodSignature(signalName, types);
    int relative_index = QMetaObjectPrivate::indexOfSignalRelative(&base, name, types.size(), types.constData());
    if (relative_index < 0) {
        return relative_index;
    }
    relative_index = QMetaObjectPrivate::originalClone(base, relative_index);
    if (meta) {
        *meta = base;
    }
    return relative_index + QMetaObjectPrivate::signalOffset(base);
}

bool QObjectPrivate::isSignalConnected(uint signalIndex, bool checkDeclarative) const
{
    if (checkDeclarative && isDeclarativeSignalConnected(signalIndex)) {
        return true;
    }

    ConnectionData *cd = connections.loadRelaxed();
    if (!cd) {
        return false;
    }
    SignalVector *signalVector = cd->signalVector.loadRelaxed();
    if (!signalVector) {
        return false;
    }

    //zhaoyujie TODO 这里为什么又是-1？
    if (signalVector->at(-1).first.loadRelaxed()) {
        return true;
    }

    if (signalIndex < uint(cd->signalVectorCount())) {
        const QObjectPrivate::Connection *c = signalVector->at(signalIndex).first.loadRelaxed();
        while (c) {
            if (c->receiver.loadRelaxed()) {
                return true;
            }
            c = c->nextConnectionList.loadRelaxed();
        }
    }
    return false;
}

QObjectPrivate::Connection::~Connection() {
    if (ownArgumentTypes) {
        const int *v = argumentTypes.loadRelaxed();
        if (v != &DIRECT_CONNECTION_ONLY) {
            if (v) {
                Q_ASSERT(false);
            }
            delete[] v;
        }
    }
    if (isSlotObject) {
        slotObj->destroyIfLastRef();
    }
}

void QObjectPrivate::ConnectionData::cleanOrphanedConnectionsImpl(QObject *sender, LockPolicy lockPolicy)
{
    QBasicMutex *senderMutex = signalSlotLock(sender);
    ConnectionOrSignalVector *c = nullptr;
    {
        std::unique_lock<QBasicMutex> lock(*senderMutex, std::defer_lock_t{});
        if (lockPolicy == NeedToLock) {
            lock.lock();
        }
        if (ref.loadAcquire() > 1) {
            return;
        }
        c = orphaned.fetchAndStoreRelaxed(nullptr);
    }
    if (c) {
        if (lockPolicy == AlreadyLockedAndTemporarilyReleasingLock) {
            senderMutex->unlock();
            deleteOrphaned(c);
            senderMutex->unlock();
        }
        else {
            deleteOrphaned(c);
        }
    }
}

void QObjectPrivate::ConnectionData::deleteOrphaned(QObjectPrivate::ConnectionOrSignalVector *o)
{
    while (o) {
        QObjectPrivate::ConnectionOrSignalVector *next = nullptr;
        if (SignalVector *v = ConnectionOrSignalVector::asSignalVector(o)) {
            next = v->nextInOrphanList;
            free(v);
        }
        else {
            QObjectPrivate::Connection *c = static_cast<Connection *>(o);
            next = c->nextInOrphanList;
            Q_ASSERT(!c->receiver.loadRelaxed());
            Q_ASSERT(!c->prev);
            c->freeSlotObject();
            Q_ASSERT(c->ref_.loadAcquire() == 1);
            c->deref();   //这里应该会释放内存
        }
        o = next;
    }
}

bool QObjectPrivate::maybeSignalConnected(uint signalIndex) const
{
    ConnectionData *cd = connections.loadRelaxed();
    if (!cd) {
        return false;
    }
    SignalVector *signalVector = cd->signalVector.loadRelaxed();
    if (!signalVector) {
        return false;
    }
    if (signalVector->at(-1).first.loadAcquire()) {  //zhaoyujie TODO 这里为什么判断-1？
        Q_ASSERT(false);
        return true;
    }
    if (signalIndex < uint(cd->signalVectorCount())) {
        const QObjectPrivate::Connection *c = signalVector->at(signalIndex).first.loadAcquire();
        return c != nullptr;
    }
    return false;
}

bool QObjectPrivate::disconnect(QObjectPrivate::Connection *c)
{
    if (!c) {
        return false;
    }
    QObject *receiver = c->receiver.loadRelaxed();
    if (!receiver) {   //没有接受对象，连接的时候上层会将发送对象设置为接受0对象，用以管理生命周期
        return false;
    }

    QBasicMutex *senderMutex = signalSlotLock(c->sender);
    QBasicMutex *receiverMutex = signalSlotLock(receiver);

    QObjectPrivate::ConnectionData *connections;
    {
        QOrderedMutexLocker locker(senderMutex, receiverMutex);
        receiver = c->receiver.loadRelaxed();
        if (!receiver) {
            return false;
        }

        connections = QObjectPrivate::get(c->sender)->connections.loadRelaxed();
        Q_ASSERT(connections);
        connections->removeConnection(c);

        c->sender->disconnectNotify(QMetaObjectPrivate::signal(c->sender->metaObject(), c->signal_index));

        if (receiverMutex != senderMutex) {
            receiverMutex->unlock();
        }
        connections->cleanOrphanedConnectionsImpl(c->sender, ConnectionData::AlreadyLockedAndTemporarilyReleasingLock);
        senderMutex->unlock();
        locker.dismiss();
    }
    return true;
}

QObject::QObject(QObject *parent)
    : QObject(*new QObjectPrivate, parent)
{
}

QObject::QObject(QObjectPrivate &dd, QObject *parent)
    : d_ptr(&dd)
{
    Q_ASSERT(this != parent);
    Q_D(QObject);
    d_ptr->q_ptr = this;
    //和父的线程保持一致。如果没有父，使用当前线程
    auto threadData = (parent && !parent->thread()) ? parent->d_func()->threadData.loadRelaxed() : QThreadData::current();
    threadData->ref();
    d->threadData.storeRelaxed(threadData);
    if (parent) {
        //父与子得在一个线程
        if (!check_parent_thread(parent, parent ? parent->d_func()->threadData.loadRelaxed() : nullptr, threadData)) {
            parent = nullptr;
        }
        if (d->isWidget) {
            if (parent) {
                d->parent = parent;
                d->parent->d_func()->children.append(this);
            }
        }
        else {
            setParent(parent);
        }
    }
    //添加QObject的钩子
    if (Q_UNLIKELY(qtHookData[QHooks::AddQObject]))
        reinterpret_cast<QHooks::AddQObjectCallback>(qtHookData[QHooks::AddQObject])(this);
    Q_TRACE(QObject_ctor, this);

}

QObject::~QObject()
{
    Q_D(QObject);
    d->wasDeleted = true;
    d->blockSig = 0;  //zhaoyujie TODO blockSig设置为false。因此可以发送destroyed ?

    d->clearBindingStorage();  //zhaoyujie TODO

    //zhaoyujie TODO 这里段refFount是啥意思。。。
    QtSharedPointer::ExternalRefCountData *sharedRefcount = d->sharedRefCount.loadRelaxed();
    if (sharedRefcount) {
        Q_ASSERT(false);
        if (sharedRefcount->strongref.loadRelaxed() > 0) {
            Q_ASSERT(false); //仍然被什么对象持有？会导致野指针？
        }

        sharedRefcount->strongref.storeRelaxed(0);  //zhaoyujie TODO
        if (!sharedRefcount->weakref.deref()) {
            delete sharedRefcount;
        }
    }

    if (!d->isWidget && d->isSignalConnected(0)) {   //0是destroyed信号
        emit destroyed(this);
    }

    //zhaoyujie TODO 这一段又是什么意思？
    if (d->declarativeData && QAbstractDeclarativeData::destroyed) {
        Q_ASSERT(false);
        QAbstractDeclarativeData::destroyed(d->declarativeData, this);
    }

    //清理信号槽链接
    QObjectPrivate::ConnectionData *cd = d->connections.loadRelaxed();
    if (cd) {
        if (cd->currentSender) {
            Q_ASSERT(false);
            cd->currentSender->receiverDeleted();
            cd->currentSender = nullptr;
        }

        QBasicMutex *signalSlotMutex = signalSlotLock(this);
        QBasicMutexLocker locker(signalSlotMutex);

        //断开所有从这个object发送的的链接
        int receiverCount = cd->signalVectorCount();
        for (int signal = -1; signal < receiverCount; ++signal) {
            QObjectPrivate::ConnectionList &connectionList = cd->connectionsForSignal(signal);

            while (QObjectPrivate::Connection *c = connectionList.first.loadRelaxed()) {
                Q_ASSERT(c->receiver.loadAcquire());

                QBasicMutex *m = signalSlotLock(c->receiver.loadRelaxed());
                bool needToUnlock = QOrderedMutexLocker::relock(signalSlotMutex, m);
                if (c == connectionList.first.loadAcquire() && c->receiver.loadAcquire()) {
                    cd->removeConnection(c);
                    Q_ASSERT(connectionList.first.loadRelaxed() != c);
                }
                if (needToUnlock) {
                    m->unlock();
                }
            }
        }

        //所有以此object作为目标的链接也断开  zhaoyujie TODO
        while (QObjectPrivate::Connection *node = cd->senders) {
            Q_ASSERT(node->receiver.loadAcquire());
            QObject *sender = node->sender;
            sender->disconnectNotify(QMetaObjectPrivate::signal(sender->metaObject(), node->signal_index));
            QBasicMutex *m = signalSlotLock(sender);
            bool needToUnlock = QOrderedMutexLocker::relock(signalSlotMutex, m);
            if (node != cd->senders) {
                Q_ASSERT(needToUnlock);
                m->unlock();
                continue;
            }

            QObjectPrivate::ConnectionData *senderData = sender->d_func()->connections.loadRelaxed();
            Q_ASSERT(senderData);

            QtPrivate::QSlotObjectBase *slotObj = nullptr;
            if (node->isSlotObject) {  //函数指针形式链接的
                slotObj = node->slotObj;
                node->isSlotObject = false;
            }
            //从sender中清理connection
            senderData->removeConnection(node);
            //unlock之前需要先cleanOrphanedConnections
            const bool locksAreTheSame = signalSlotMutex == m;
            if (!locksAreTheSame) {
                locker.unlock();
            }
            senderData->cleanOrphanedConnections(sender,
                                                 QObjectPrivate::ConnectionData::AlreadyLockedAndTemporarilyReleasingLock
                                                 );
            if (needToUnlock) {
                m->unlock();
            }
            if (locksAreTheSame) {
                locker.unlock();
            }
            if (slotObj) {
                slotObj->destroyIfLastRef();
            }
            locker.relock();
        }
        cd->currentConnectionId.storeRelaxed(0);
    }

    if (cd && !cd->ref.deref()) { //cd在触发的时候，搞了个智能指针，进行了ref操作
        delete cd;
    }
    d->connections.storeRelaxed(nullptr);

    if (!d->children.isEmpty()) {
        d->deleteChildren();
    }

#if QT_VERSION < 0x60000
    qt_removeObject(this);
#endif

    //删除了QObject的回调
    if (Q_UNLIKELY(qtHookData[QHooks::RemoveQObject])) {
        reinterpret_cast<QHooks::RemoveQObjectCallback>(qtHookData[QHooks::RemoveQObject])(this);
    }

    Q_TRACE(QObject_dtor, this);

    if (d->parent) {  //从父中删除
        d->setParent_helper(nullptr);
    }

}


QString QObject::objectName() const
{
//    Q_D(const QObject);
//    if (!d->extraData && QtPrivate::isAnyBindingEvaluating()) {
//        QObjectPrivate *dd = const_cast<QObjectPrivate *>(d);
//        dd->extraData = new QObjectPrivate::ExtraData(dd);
//    }
//    return d->extraData ? d->extraData->objectName : QString();
    Q_ASSERT(false);
    return QString();
}

void QObject::setObjectName(const QString &name)
{
    Q_ASSERT(false);
//    Q_D(QObject);
//    d->ensureExtraData();
//    d->extraData->objectName.removeBindingUnlessInWrapper();
//    if (d->extraData->objectName != name) {
//        d->extraData->objectName.setValueBypassingBindings(name);
//        d->extraData->objectName.notify();
//    }
}

void QObject::deleteLater() {
    Q_ASSERT(false);
    //zhaoyujie TODO 这里的postEvent的特点是什么？
//    QCoreApplication::postEvent(this, new QDeferredDeleteEvent());
}

void qDeleteInEventHandler(QObject *o) {
    delete o;
}

bool QObject::event(QEvent *event)
{
    switch(event->type()) {
        case QEvent::Timer:
            timerEvent((QTimerEvent *)event);
            break;
        case QEvent::ChildAdded:
        case QEvent::ChildRemoved:
        case QEvent::ChildPolished:
            childEvent((QChildEvent *)event);
            break;
        case QEvent::DeferredDelete:  //推迟删除
            qDeleteInEventHandler(this);
            break;
        case QEvent::MetaCall:
            Q_ASSERT(false);
            break;
        case QEvent::ThreadChange:
            Q_ASSERT(false);
            break;
        default: {
            if (event->type() >= QEvent::User) {
                customEvent(event);
                break;
            }
            return false;
        }
    }
    return true;
}

bool QObject::eventFilter(QObject *, QEvent *)
{
    return false;
}


QVariant QObject::property(const char *name) const
{
    Q_D(const QObject);
    const QMetaObject *meta = metaObject();
    if (!name || !meta) {
        return QVariant();
    }
    int id = meta->indexOfProperty(name);
    if (id < 0) {  //没有通过Q_PROPERTY注册，查找动态属性
        if (!d->extraData) {
            return QVariant();
        }
        const int i = d->extraData->propertyNames.indexOf(name);
        return d->extraData->propertyValues.value(i);
    }
    else {
        QMetaProperty p = meta->property(id);
        if (!p.isReadable()) {
            Q_ASSERT(false);
        }
        return p.read(this);
    }
}

/*
 * setProperty先从metaObject中查找属性，如果找不到，就放在extraData（动态属性）中
 * 通过Q_PROPERTY注册的属性，设置成功返回true，否则都返回false
 * */
bool QObject::setProperty(const char *name, const QVariant &value)
{
    Q_D(QObject);
    const QMetaObject *meta = metaObject();
    if (!name || !meta) {
        return false;
    }
    int id = meta->indexOfProperty(name);
    if (id < 0) {
        if (!d->extraData) {
            d->extraData = new QObjectPrivate::ExtraData(d);
        }
        const int idx = d->extraData->propertyNames.indexOf(name);
        if (!value.isValid()) {  //value非法，从额外数据中移除
            if (idx == -1) {
                return false;
            }
            d->extraData->propertyNames.removeAt(idx);
            d->extraData->propertyValues.removeAt(idx);
        }
        else {
            if (idx == -1) {
                //value合法，extraData中没有此属性，将数据加入到extraData中
                d->extraData->propertyNames.append(name);
                d->extraData->propertyValues.append(value);
            }
            else {
                //数据类型相同，且数据相同无需设置，否则替换
                if (value.userType() == d->extraData->propertyValues.at(idx).userType()
                    && value == d->extraData->propertyValues.at(idx)) {
                    return false;
                }
                d->extraData->propertyValues[idx] = value;
            }
        }
        QDynamicPropertyChangeEvent ev(name);
        QCoreApplication::sendEvent(this, &ev);
        return false;
    }
    else {
        QMetaProperty p = meta->property(id);
        if (!p.isWriteable()) {
            Q_ASSERT(false);
        }
        return p.write(this, value);
    }
}

QThread *QObject::thread() const
{
    return d_func()->threadData.loadRelaxed()->thread.loadAcquire();
}

/*
 * 采用QObject::connect(obj, SIGNAL(sig1(int, double)), obj, SLOT(slot1(int, double)));方式连接
 * */
QMetaObject::Connection QObject::connect(const QObject *sender, const char *signal,
                                         const QObject *receiver, const char *method, Qt::ConnectionType type)
{
    if (sender == nullptr || receiver == nullptr || signal == nullptr || method == nullptr) {
        Q_ASSERT(false);  //非法数据，返回空, bool判断为false
        return QMetaObject::Connection(nullptr);
    }
    QByteArray tmp_signal_name;

    //检查发射信号正确性
    if (!check_signal_macro(sender, signal, "connect", "bind")) {
        return QMetaObject::Connection(nullptr);
    }

    const QMetaObject *smeta = sender->metaObject();
    const char *signal_arg = signal;
    ++signal;  //跳过了在前面添加的方法类型标志位

    QArgumentTypeArray signalTypes;  //信号参数类型，可以从moc信息中获取
    Q_ASSERT(QMetaObjectPrivate::get(smeta)->revision >= 7);

    //SIGNAL(sig1(int, double))的字符串中解析出参数
    QByteArray signalName = QMetaObjectPrivate::decodeMethodSignature(signal, signalTypes);

    //查找符合的信号索引
    int signal_index = QMetaObjectPrivate::indexOfSignalRelative(&smeta, signalName, signalTypes.size(), signalTypes.constData());
    if (signal_index < 0) { //没有找到完全匹配的
        //标准化签名，移除空格，const &之类的信息
        tmp_signal_name = QMetaObject::normalizedSignature(signal - 1);
        signal = tmp_signal_name.constData() + 1;
        signalTypes.clear();
        signalName = QMetaObjectPrivate::decodeMethodSignature(signal, signalTypes);
        smeta = sender->metaObject();
        signal_index = QMetaObjectPrivate::indexOfSignalRelative(&smeta, signalName, signalTypes.size(), signalTypes.constData());
    }
    if (signal_index < 0) {
        //没有找到信号函数
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }
    signal_index = QMetaObjectPrivate::originalClone(smeta, signal_index);
    signal_index += QMetaObjectPrivate::signalOffset(smeta);

    QByteArray tmp_method_name;
    //提取了前面的第一位，触发方法可能是signal，也可以是slot
    int membcode = extract_code(method);

    if (!check_method_code(membcode, receiver, method, "connect")) {
        return QMetaObject::Connection(nullptr);
    }
    const char *method_arg = method;
    ++method;

    QArgumentTypeArray methodTypes;
    QByteArray methodName = QMetaObjectPrivate::decodeMethodSignature(method, methodTypes);
    const QMetaObject *rmeta = receiver->metaObject();
    int method_index_relative = -1;
    Q_ASSERT(QMetaObjectPrivate::get(rmeta)->revision >= 7);
    switch(membcode) {
        case QSLOT_CODE:
            method_index_relative = QMetaObjectPrivate::indexOfSlotRelative(
                    &rmeta, methodName, methodTypes.size(), methodTypes.constData());
            break;

        case QSIGNAL_CODE:
            method_index_relative = QMetaObjectPrivate::indexOfSignalRelative(
                    &rmeta, methodName, methodTypes.size(), methodTypes.constData());
            break;
    }
    if (method_index_relative < 0) {
        tmp_method_name = QMetaObject::normalizedSignature(method);
        method = tmp_method_name.constData();

        methodTypes.clear();
        methodName = QMetaObjectPrivate::decodeMethodSignature(method, methodTypes);
        rmeta = receiver->metaObject();
        switch(membcode) {
            case QSLOT_CODE:
                method_index_relative = QMetaObjectPrivate::indexOfSlotRelative(
                        &rmeta, methodName, methodTypes.size(), methodTypes.constData());
                break;
            case QSIGNAL_CODE:
                method_index_relative = QMetaObjectPrivate::indexOfSignalRelative(
                        &rmeta, methodName, methodTypes.size(), methodTypes.constData());
                break;
        }
    }

    if (method_index_relative < 0) {
        Q_ASSERT(false);
        //没有找到对应的触发函数
        return QMetaObject::Connection(nullptr);
    }
    //检查信号函数和槽函数的参数匹配, 信号的参数数量要大于等于槽，且参数类型要相同
    //float 与 double之类的不可以相互转换
    if (!QMetaObjectPrivate::checkConnectArgs(signalTypes.size(), signalTypes.constData(),
                                              methodTypes.size(), methodTypes.constData())) {
        Q_ASSERT(false);
        //信号与槽函数的参数不匹配
        return QMetaObject::Connection(nullptr);
    }

    int *types = nullptr;
    if ((type == Qt::QueuedConnection)) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }

    auto cp = QMetaObjectPrivate::connect(sender, signal_index, smeta,
                                         receiver, method_index_relative, rmeta,
                                         type, types
                                         );
    QMetaObject::Connection handle = QMetaObject::Connection(cp);
    return handle;
}

/*
 * method 可以是信号，槽函数，invoke函数
 * */
QMetaObject::Connection QObject::connect(const QObject *sender, const QMetaMethod &signal,
                                         const QObject *receiver, const QMetaMethod &method,
                                         Qt::ConnectionType type)
{
    if (sender == nullptr || receiver == nullptr || signal.methodType() != QMetaMethod::Signal || method.methodType() == QMetaMethod::Constructor) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }

    int signal_index;
    int method_index;
    {
        int dummy;
        QMetaObjectPrivate::memberIndexes(sender, signal, &signal_index, &dummy);
        QMetaObjectPrivate::memberIndexes(receiver, method, &dummy, &method_index);
    }

    const QMetaObject *smeta = sender->metaObject();
    const QMetaObject *rmeta = receiver->metaObject();

    if (signal_index == -1) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }
    if (method_index == -1) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }
    //检测函数签名
    if (!QMetaObject::checkConnectArgs(signal.methodSignature().constData(),
                                       method.methodSignature().constData())) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }

    int *types = nullptr;
    if (type == Qt::QueuedConnection) {
        Q_ASSERT(false);  //zhaoyujie TODO
        return QMetaObject::Connection(nullptr);
    }
    //zhaoyujie TODO rmeta为什么是nullptr?
    Q_ASSERT(false);
    auto cp = QMetaObjectPrivate::connect(sender, signal_index, signal.enclosingMetaObject(), receiver, method_index, nullptr, type, types);
    QMetaObject::Connection handle = QMetaObject::Connection(cp);
    return handle;
}

QMetaObject::Connection QObject::connect(const QObject *sender, const char *signal, const char *method, Qt::ConnectionType type) const
{
    return connect(sender, signal, this, method, type);
}

//zhaoyujie TODO 这里的signal与slot是参数地址？
QMetaObject::Connection QObject::connectImpl(const QObject *sender, void **signal,
                                             const QObject *receiver, void **slot,
                                             QtPrivate::QSlotObjectBase *slotObj, Qt::ConnectionType type,
                                             const int *types, const QMetaObject *senderMetaObject) {
    if (!signal) {
        Q_ASSERT(false);
        if (slotObj) {  //zhaoyujie TODO 为什么需要destroy
            slotObj->destroyIfLastRef();
        }
        return QMetaObject::Connection();
    }

    int signal_index = -1;
    void *args[] = { &signal_index, signal };
    for (; senderMetaObject && signal_index < 0; senderMetaObject = senderMetaObject->superClass()) {
        senderMetaObject->static_metacall(QMetaObject::IndexOfMethod, 0, args);
        if (signal_index >= 0 & signal_index < QMetaObjectPrivate::get(senderMetaObject)->signalCount) {
            break;
        }
    }

    if (!senderMetaObject) {
        Q_ASSERT(false);
        slotObj->destroyIfLastRef();
        return QMetaObject::Connection(nullptr);
    }
    signal_index += QMetaObjectPrivate::signalOffset(senderMetaObject);
    return QObjectPrivate::connectImpl(sender, signal_index, receiver, slot, slotObj, type, types, senderMetaObject);
}

QMetaObject::Connection QObjectPrivate::connectImpl(const QObject *sender, int signal_index, const QObject *receiver,
                                                    void **slot, QtPrivate::QSlotObjectBase *slotObj, int type,
                                                    const int *types, const QMetaObject *senderMetaObject) {
    if (!sender || !receiver || !slotObj || !senderMetaObject) {
        Q_ASSERT(false);
        if (slotObj) {
            slotObj->destroyIfLastRef();
        }
        return QMetaObject::Connection();
    }

    QObject *s = const_cast<QObject *>(sender);
    QObject *r = const_cast<QObject *>(receiver);

    QOrderedMutexLocker locker(signalSlotLock(sender),
                               signalSlotLock(receiver));

    //uniqueConnection
    if (type & Qt::UniqueConnection && slot && QObjectPrivate::get(s)->connections.loadRelaxed()) {
        Q_ASSERT(false);
        QObjectPrivate::ConnectionData *connections = QObjectPrivate::get(s)->connections.loadRelaxed();
        if (connections->signalVectorCount() > signal_index) {
            const QObjectPrivate::Connection *c2 = connections->signalVector.loadRelaxed()->at(signal_index).first.loadRelaxed();

            while (c2) {
                if (c2->receiver.loadRelaxed() == receiver && c2->isSlotObject && c2->slotObj->compare(slot)) {
                    slotObj->destroyIfLastRef();
                    return QMetaObject::Connection();
                }
                c2 = c2->nextConnectionList.loadRelaxed();
            }
        }
    }
    type &= ~Qt::UniqueConnection;

    const bool isSignalShot = type & Qt::SingleShotConnection;
    type &= ~Qt::SingleShotConnection;

    Q_ASSERT(type >= 0);
    Q_ASSERT(type <= 3);

    std::unique_ptr<QObjectPrivate::Connection> c{new QObjectPrivate::Connection};
    c->sender = s;
    c->signal_index = signal_index;
    QThreadData *td = r->d_func()->threadData;
    td->ref();  //ref一下，被c的receiverThreadData持有了
    c->receiverThreadData.storeRelaxed(td);
    c->receiver.storeRelaxed(r);
    c->slotObj = slotObj;
    c->connectionType = type;
    c->isSlotObject = true;
    if (types) {
        Q_ASSERT(false);
        c->argumentTypes.storeRelaxed(types);
        c->ownArgumentTypes = false;
    }
    c->isSingleShot = isSignalShot;

    QObjectPrivate::get(s)->addConnection(signal_index, c.get());
    QMetaObject::Connection ret(c.release());
    locker.unlock();

    QMetaMethod method = QMetaObjectPrivate::signal(senderMetaObject, signal_index);
    Q_ASSERT(method.isValid());
    s->connectNotify(method);

    return ret;
}

//method为空，删除receiver所有和sender的signal的连接
bool QObject::disconnect(const QObject *sender, const char *signal, const QObject *receiver, const char *method)
{
    if (sender == nullptr || (receiver == nullptr && method != nullptr)) {
        return false;
    }

    const char *signal_arg = signal;
    QByteArray signal_name;
    bool signal_found = false;
    if (signal) {
        signal_name = QMetaObject::normalizedSignature(signal);
        signal = signal_name.constData();

        if (!check_signal_macro(sender, signal, "disconnect", "unbind")) {
            return false;
        }
        signal++;
    }

    QByteArray method_name;
    const char *method_arg = method;
    int membcode = -1;
    bool method_found = false;
    if (method) {
        method_name = QMetaObject::normalizedSignature(method);
        method = method_name.constData();
        membcode = extract_code(method);
        if (!check_method_code(membcode, receiver, method, "disconnect")) {
            return false;
        }
        method++;
    }

    bool res = false;
    const QMetaObject *smeta = sender->metaObject();
    QByteArray signalName;
    QArgumentTypeArray signalTypes;
    Q_ASSERT(QMetaObjectPrivate::get(smeta)->revision >= 7);
    if (signal) {
        signalName = QMetaObjectPrivate::decodeMethodSignature(signal, signalTypes);
    }
    QByteArray methodName;
    QArgumentTypeArray methodTypes;
    Q_ASSERT(!receiver || QMetaObjectPrivate::get(receiver->metaObject())->revision >= 7);
    if (method) {
        methodName = QMetaObjectPrivate::decodeMethodSignature(method, methodTypes);
    }
    do {
        int signal_index = -1;
        if (signal) {
            signal_index = QMetaObjectPrivate::indexOfSignalRelative(&smeta, signalName, signalTypes.size(), signalTypes.constData());
            if (signal_index < 0) {
                break;
            }
            signal_index = QMetaObjectPrivate::originalClone(smeta, signal_index);
            signal_index += QMetaObjectPrivate::signalOffset(smeta);
            signal_found = true;
        }

        if (!method) {
            //断开receiver的所有连接
            res |= QMetaObjectPrivate::disconnect(sender, signal_index, smeta, receiver, -1, nullptr);
        }
        else {
            const QMetaObject *rmeta = receiver->metaObject();
            do {
                int method_index = QMetaObjectPrivate::indexOfMethod(rmeta, methodName, methodTypes.size(), methodTypes.constData());
                if (method_index >= 0) {
                    while (method_index < rmeta->methodOffset()) {
                        rmeta = rmeta->superClass();
                    }
                }
                if (method_index < 0) {
                    break;
                }
                res |= QMetaObjectPrivate::disconnect(sender, signal_index, smeta, receiver, method_index, nullptr);
                method_found = true;
            } while ((rmeta = rmeta->superClass()));
        }
    } while (signal && (smeta = smeta->superClass()));

    if (signal && !signal_found) {
        Q_ASSERT(false);
    }
    else if (method && !method_found) {
        Q_ASSERT(false);
    }
    if (res) {
        if (!signal) {   //如果signal不为空，在QObjectPrivate::disconnect里已经发送过通知了。这里做下signal为空的通知
            const_cast<QObject *>(sender)->disconnectNotify(QMetaMethod());
        }
    }
    return res;
}

bool QObject::disconnect(const QObject *sender, const QMetaMethod &signal, const QObject *receiver,
                         const QMetaMethod &method) {
    Q_ASSERT(false);
    if (sender == nullptr || (receiver == nullptr && method.mobj != nullptr)) {
        Q_ASSERT(false);
        return false;
    }
    if (signal.mobj && signal.methodType() != QMetaMethod::Signal) {
        Q_ASSERT(false);
        return false;
    }
    if (method.mobj && method.methodType() == QMetaMethod::Constructor) {
        Q_ASSERT(false);
        return false;
    }
    QByteArray signalSignature;
    if (signal.mobj) {
        auto tmp = signal.methodSignature();
        signalSignature.reserve(tmp.size() + 1);
        signalSignature.append((char)(QSIGNAL_CODE + '0'));
        signalSignature.append(tmp);
    }

    int signal_index;
    int method_index;
    {
        int dummy;
        QMetaObjectPrivate::memberIndexes(sender, signal, &signal_index, &dummy);
        QMetaObjectPrivate::memberIndexes(receiver, method, &dummy, &method_index);
    }

    if (signal.mobj && signal_index == -1) {
        Q_ASSERT(false);
        return false;
    }
    if (receiver && method.mobj && method_index == -1) {
        Q_ASSERT(false);
        return false;
    }

    if (!QMetaObjectPrivate::disconnect(sender, signal_index, signal.mobj, receiver, method_index, nullptr)) {
        return false;
    }
    if (!signal.isValid()) {  //zhaoyujie TODO 移除最后一个的时候会调用这个方法？
        const_cast<QObject *>(sender)->disconnectNotify(signal);
    }
    return true;
}

bool QObject::disconnect(const QMetaObject::Connection &connection)
{
    QObjectPrivate::Connection *c = static_cast<QObjectPrivate::Connection *>(connection.d_ptr);
    if (!c) {
        return false;
    }
    const bool disconnected = QObjectPrivate::disconnect(c);
    const_cast<QMetaObject::Connection &>(connection).d_ptr = nullptr;
    c->deref();
    return disconnected;
}

bool QObject::disconnectImpl(const QObject *sender, void **signal, const QObject *receiver, void **slot, const QMetaObject *senderMetaObject)
{
    if (sender == nullptr || (receiver == nullptr && slot != nullptr)) {
        Q_ASSERT(false);
        return false;
    }

    int signal_index = -1;
    if (signal) {
        void *args[] = { &signal_index, signal };
        for (; senderMetaObject && signal_index < 0; senderMetaObject = senderMetaObject->superClass()) {
            senderMetaObject->static_metacall(QMetaObject::IndexOfMethod, 0, args);
            if (signal_index >= 0 && signal_index < QMetaObjectPrivate::get(senderMetaObject)->signalCount) {
                break;
            }
        }
        if (!senderMetaObject) {
            Q_ASSERT(false);
            return false;
        }
        signal_index += QMetaObjectPrivate::signalOffset(senderMetaObject);
    }

    return QMetaObjectPrivate::disconnect(sender, signal_index, senderMetaObject, receiver, -1, slot);
}

void QObject::connectNotify(const QMetaMethod &signal)
{
    Q_UNUSED(signal);
}

void QObject::disconnectNotify(const QMetaMethod &signal)
{
    Q_UNUSED(signal);
}

void QObject::timerEvent(QTimerEvent *)
{
}

void QObject::childEvent(QChildEvent *)
{
}

void QObject::customEvent(QEvent *)
{
}

QObject *QObject::sender() const
{
    Q_D(const QObject);

    QBasicMutexLocker locker(signalSlotLock(this));
    QObjectPrivate::ConnectionData *cd = d->connections.loadRelaxed();
    if (!cd || !cd->currentSender) {
        return nullptr;
    }

    for (QObjectPrivate::Connection *c = cd->senders; c; c = c->next) {
        if (c->sender == cd->currentSender->sender) {
            return cd->currentSender->sender;
        }
    }
    return nullptr;
}

int QObject::senderSignalIndex() const
{
    Q_D(const QObject);

    QBasicMutexLocker locker(signalSlotLock(this));
    QObjectPrivate::ConnectionData *cd = d->connections.loadRelaxed();

    if (!cd || !cd->currentSender) {
        return -1;
    }

    for (QObjectPrivate::Connection *c = cd->senders; c; c = c->next) {
        if (c->sender == cd->currentSender->sender) {
            return QMetaObjectPrivate::signal(c->sender->metaObject(), cd->currentSender->signal).methodIndex();
        }
    }
    return -1;
}

int QObject::receivers(const char *signal) const
{
    Q_D(const QObject);
    int receivers = 0;
    if (signal) {
        QByteArray signal_name = QMetaObject::normalizedSignature(signal);
        signal = signal_name;
#ifndef QT_NO_DEBUG
        if (!check_signal_macro(this, signal, "receivers", "bind")) {
            return 0;
        }
#endif
        signal++;
        int signal_index = d->signalIndex(signal);
        if (signal_index < 0) {
#ifndef QT_NO_DEBUG
            err_method_notfound(this, signal - 1, "receivers");
#endif
            return 0;
        }

        if (!d->isSignalConnected(signal_index)) {
            return receivers;
        }
        if (d->declarativeData && QAbstractDeclarativeData::receivers) {
            receivers += QAbstractDeclarativeData::receivers(d->declarativeData, this, signal_index);
        }

        QObjectPrivate::ConnectionData *cd = d->connections.loadRelaxed();
        QBasicMutexLocker locker(signalSlotLock(this));
        if (cd && signal_index < cd->signalVectorCount()) {
            const QObjectPrivate::Connection *c = cd->signalVector.loadRelaxed()->at(signal_index).first.loadRelaxed();
            while (c) {
                receivers += c->receiver.loadRelaxed() ? 1 : 0;
                c = c->nextConnectionList.loadRelaxed();
            }
        }

    }
    return receivers;
}

bool QObject::isSignalConnected(const QMetaMethod &signal) const
{
    Q_D(const QObject);
    if (!signal.mobj) {
        return false;
    }

    Q_ASSERT_X(signal.mobj->cast(this) && signal.methodType() == QMetaMethod::Signal,
               "QObject::isSignalConnected" , "the parameter must be a signal member of the object");

    uint signalIndex = signal.relativeMethodIndex();

    if (signal.data.flags() & MethodCloned) {
        Q_ASSERT(false);   //methodClone什么作用。。。
        signalIndex = QMetaObjectPrivate::originalClone(signal.mobj, signalIndex);
    }

    signalIndex += QMetaObjectPrivate::signalOffset(signal.mobj);

    QBasicMutexLocker locker(signalSlotLock(this));
    return d->isSignalConnected(signalIndex, true);
}

void QObject::registerInvokeMethod(const std::string &key, const InvokeMethod &func)
{
    m_invokeMethodMap.push_back({key, std::move(func)});
}

void QObject::setParent(QObject *parent)
{
    Q_D(QObject);
    Q_ASSERT(!d->isWidget);
    d->setParent_helper(parent);
}

//通过信号触发
template <bool callbacks_enabled>
void doActivate(QObject *sender, int signal_index, void **argv)
{
    QObjectPrivate *sp = QObjectPrivate::get(sender);
    if (sp->blockSig) {  //信号被阻塞
        return;
    }
    Q_TRACE_SCOPE(QMetaObject_activate, sender, signal_index);
    if (sp->isDeclarativeSignalConnected(signal_index) && QAbstractDeclarativeData::signalEmitted) {
        Q_TRACE_SCOPE(QMetaObject_activate_declarative_signal, sender, signal_index);
        QAbstractDeclarativeData::signalEmitted(sp->declarativeData, sender, signal_index, argv);
        Q_ASSERT(false);
    }
    const QSignalSpyCallbackSet *signal_spy_set = callbacks_enabled ? qt_signal_spy_callback_set.loadAcquire() : nullptr;

    void *empty_argv[] = { nullptr };
    if (!argv) {
        argv = empty_argv;
    }
    if (!sp->maybeSignalConnected(signal_index)) {  //没有连接到信号的槽
        if (callbacks_enabled && signal_spy_set->signal_begin_callback != nullptr) {
            signal_spy_set->signal_begin_callback(sender, signal_index, argv);
        }
        if (callbacks_enabled && signal_spy_set->signal_end_callback != nullptr) {
            signal_spy_set->signal_end_callback(sender, signal_index);
        }
        return;
    }
    if (callbacks_enabled && signal_spy_set->signal_begin_callback != nullptr) {
        signal_spy_set->signal_begin_callback(sender, signal_index, argv);
    }

    bool senderDeleted = false;  //发送者有没有被删除？
    {
        Q_ASSERT(sp->connections.loadAcquire());
        QObjectPrivate::ConnectionDataPointer connections(sp->connections.loadRelaxed());
        QObjectPrivate::SignalVector *signalVector = connections->signalVector.loadRelaxed();

        const QObjectPrivate::ConnectionList *list;
        if (signal_index < signalVector->count()) {
            list = &signalVector->at(signal_index);
        }
        else {
            Q_ASSERT(false);
            list = &signalVector->at(-1);  //为什么是-1？这个-1是什么作用？父类的信号？
        }

        Qt::HANDLE currentThreadId = QThread::currentThreadId();
        //当前线程是否在发送者线程
        bool inSenderThread = currentThreadId == QObjectPrivate::get(sender)->threadData.loadRelaxed()->threadId.loadRelaxed();
        //确保这次信号周期里新增的连接不会被调用
        uint highestConnectionId = connections->currentConnectionId.loadRelaxed();
        do {
            QObjectPrivate::Connection *c = list->first.loadRelaxed();
            if (!c) {
                continue;
            }
            do {
                QObject *const receiver = c->receiver.loadRelaxed();
                if (!receiver) {
                    continue;
                }
                QThreadData *td = c->receiverThreadData.loadRelaxed();
                if (!td) {
                    continue;
                }
                bool receiverInSameThread;
                if (inSenderThread) {
                    receiverInSameThread = currentThreadId == td->threadId.loadRelaxed();
                }
                else {
                    QMutexLocker lock(signalSlotLock(receiver));
                    receiverInSameThread = currentThreadId == td->threadId.loadRelaxed();
                }
                if ((c->connectionType == Qt::AutoConnection && !receiverInSameThread)
                    || (c->connectionType == Qt::QueuedConnection))
                {
                    //接收者不在当前线程
                    Q_ASSERT(false);
                    continue;
                }
                else if (c->connectionType == Qt::BlockingQueuedConnection) {
                    Q_ASSERT(false);
                    continue;
                }
                //单次触发，尝试解除连接
                if (c->isSingleShot) {
                    Q_ASSERT(false);
                    continue;
                }

                QObjectPrivate::Sender senderData(receiverInSameThread ? receiver : nullptr, sender, signal_index);
                if (c->isSlotObject) {
                    SlotObjectGuard obj {c->slotObj};
                    {
//                        Q_TRACE_SCOPE(QMetaObject_activate_slot_functor, obj.get());
                        obj->call(receiver, argv);
                    }
                }
                else if (c->callFunction && c->method_offset <= receiver->metaObject()->methodOffset()) {
                    const int method_relative = c->method_relative;
                    const auto callFunction = c->callFunction;
                    const int methodIndex = (Q_HAS_TRACEPOINTS || callbacks_enabled) ? c->method() : 0;
                    if (callbacks_enabled && signal_spy_set->slot_begin_callback != nullptr)
                        signal_spy_set->slot_begin_callback(receiver, methodIndex, argv);

                    {
                        Q_TRACE_SCOPE(QMetaObject_activate_slot, receiver, methodIndex);
                        callFunction(receiver, QMetaObject::InvokeMetaMethod, method_relative, argv);
                    }

                    if (callbacks_enabled && signal_spy_set->slot_end_callback != nullptr)
                        signal_spy_set->slot_end_callback(receiver, methodIndex);
                }
                else {
                    Q_ASSERT(false);
                    const int method = c->method_relative + c->method_offset;
                    if (callbacks_enabled && signal_spy_set->slot_begin_callback != nullptr) {
                        signal_spy_set->slot_begin_callback(receiver, method, argv);
                    }
                    {
                        Q_TRACE_SCOPE(QMetaObject_activate_slot, receiver, method);
                        QMetaObject::metacall(receiver, QMetaObject::InvokeMetaMethod, method, argv);
                    }
                    if (callbacks_enabled && signal_spy_set->slot_end_callback != nullptr) {
                        signal_spy_set->slot_end_callback(receiver, method);
                    }
                }
            } while ((c = c->nextConnectionList.loadRelaxed()) != nullptr && c->id <= highestConnectionId);    //依次出发connectionList中的链接，新加入的链接不触发
        } while (list != &signalVector->at(-1) && ((list = &signalVector->at(-1)), true));

        if (connections->currentConnectionId.loadRelaxed() == 0) {  //sender被释放了，会将currentConnectionId设置为0
            senderDeleted = true;
        }
    }

    if (!senderDeleted) {
        sp->connections.loadRelaxed()->cleanOrphanedConnections(sender);

        if (callbacks_enabled && signal_spy_set->signal_end_callback != nullptr) {
            signal_spy_set->signal_end_callback(sender, signal_index);
        }
    }
}

void QMetaObject::activate(QObject *sender, const QMetaObject *m, int local_signal_index, void **argv)
{
    int signal_index = local_signal_index + QMetaObjectPrivate::signalOffset(m);
    if (Q_UNLIKELY(qt_signal_spy_callback_set.loadRelaxed())) {
        doActivate<true>(sender, signal_index, argv);
    }
    else {
        doActivate<false>(sender, signal_index, argv);
    }
}

QT_END_NAMESPACE