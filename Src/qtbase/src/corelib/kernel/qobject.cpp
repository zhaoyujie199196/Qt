//
// Created by Yujie Zhao on 2023/2/24.
//

#include "qobject.h"
#include "qcoreevent.h"
#include "qobject_p.h"
#include "qmetaobject_p.h"
#include "qobject_p_p.h"
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

Q_CORE_EXPORT QBasicAtomicPointer<QSignalSpyCallbackSet> qt_signal_spy_callback_set = Q_BASIC_ATOMIC_INITIALIZER(nullptr);

namespace {
    class FlaggedDebugSignatures
    {
        uint idx = 0;
        std::array<const char *, 2> locations = {};

    public:
        void store(const char *method) noexcept
        {
            locations[idx++ & locations.size()] = method;
        }
        bool contains(const char *method) const noexcept
        {
            return std::find(locations.begin(), locations.end(), method) != locations.end();
        }
    };

    Q_CONSTINIT static thread_local FlaggedDebugSignatures flaggedSignatures = {};
}

const char *qFlagLocation(const char *method)
{
    return method;
}

static int extract_code(const char *member)
{
    return (((int)(*member - '0')) & 0x3);
}

static const char *extract_location(const char *member)
{
    if (flaggedSignatures.contains(member)) {
        const char *location = member + qstrlen(member) + 1;
        if (*location != '\0') {
            return location;
        }
    }
    return nullptr;
}

static bool check_signal_macro(const QObject *sendr, const char *signal, const char *func, const char *op)
{
    int sigcode = extract_code(signal);
    if (sigcode != QSIGNAL_CODE) {
        Q_ASSERT(false);
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

QObject *qt_qFindChild_helper(const QObject *parent, const QString &name, const QMetaObject &mo, Qt::FindChildOptions options)
{
    Q_ASSERT(parent);
    for (QObject *obj : parent->children()) {
        if (mo.cast(obj) && (name.isNull() || obj->objectName() == name)) {
            return obj;
        }
    }
    if (options & Qt::FindChildrenRecursively) {
        for (QObject *child : parent->children()) {
            if (QObject *obj = qt_qFindChild_helper(child, name, mo, options)) {
                return obj;
            }
        }
    }
    return nullptr;
}

static void qt_qFindChildren_with_name(const QObject *parent, const QString &name, const QMetaObject &mo, QList<void *> *list, Qt::FindChildOptions options)
{
    Q_ASSERT(parent);
    Q_ASSERT(list);
    Q_ASSERT(!name.isNull());
    for (QObject *obj : parent->children()) {
        if (mo.cast(obj) && obj->objectName() == name) {
            list->append(obj);
        }
        if (options & Qt::FindChildrenRecursively) {
            qt_qFindChildren_with_name(obj, name, mo, list, options);
        }
    }
}

void qt_qFindChildren_helper(const QObject *parent, const QString &name, const QMetaObject &mo, QList<void *> *list, Qt::FindChildOptions options)
{
    if (name.isNull()) {
        return qt_qFindChildren_helper(parent, mo, list, options);
    }
    else {
        return qt_qFindChildren_with_name(parent, name, mo, list, options);
    }
}

void qt_qFindChildren_helper(const QObject *parent, const QMetaObject &mo,
                             QList<void *> *list, Qt::FindChildOptions options)
{
    Q_ASSERT(parent);
    Q_ASSERT(list);
    for (QObject *obj : parent->children()) {
        if (mo.cast(obj)) {
            list->append(obj);
        }
        if (options & Qt::FindChildrenRecursively) {
            qt_qFindChildren_helper(obj, mo, list, options);
        }
    }
}

static void computeOffsets(const QMetaObject *metaobject, int *signalOffset, int *methodOffset)
{
    *signalOffset = *methodOffset = 0;
    const QMetaObject *m = metaobject->d.superdata;
    while (m) {
        const QMetaObjectPrivate *d = QMetaObjectPrivate::get(m);
        *methodOffset += d->methodCount;
        Q_ASSERT(d->revision >= 4);
        *signalOffset += d->signalCount;
        m = m->d.superdata;
    }
}

static bool check_parent_thread(QObject *parent, QThreadData *parentThreadData, QThreadData *currentThreadData)
{
    if (parent && parentThreadData != currentThreadData) {
        Q_ASSERT(false);
        return false;
    }
    return true;
}

Q_CONSTINIT static QBasicMutex _q_ObjectMutexPool[131];
static inline QBasicMutex *signalSlotLock(const QObject *o)
{
    return &_q_ObjectMutexPool[uint(quintptr(o)) % sizeof(_q_ObjectMutexPool)/sizeof(QBasicMutex)];
}

void (*QAbstractDeclarativeData::destroyed)(QAbstractDeclarativeData *, QObject *) = nullptr;
void (*QAbstractDeclarativeData::signalEmitted)(QAbstractDeclarativeData *, QObject *, int, void **) = nullptr;
int  (*QAbstractDeclarativeData::receivers)(QAbstractDeclarativeData *, const QObject *, int) = nullptr;
bool (*QAbstractDeclarativeData::isSignalConnected)(QAbstractDeclarativeData *, const QObject *, int) = nullptr;
void (*QAbstractDeclarativeData::setWidgetParent)(QObject *, QObject *) = nullptr;


static void err_method_notfound(const QObject *object, const char *method, const char *func)
{
    Q_ASSERT(false);
}

QObjectData::~QObjectData() {}

QMetaObject *QObjectData::dynamicMetaObject() const
{
    return metaObject->toDynamicMetaObject(q_ptr);
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
    //如果有父并且父没有线程？
    if (parent && !parent->thread()) {
        Q_ASSERT(false);  //TODO
    }
    auto threadData = (parent && !parent->thread()) ? parent->d_func()->threadData.loadRelaxed() : QThreadData::current();
    threadData->ref();
    d->threadData.storeRelaxed(threadData);
    if (parent) {
        if (!check_parent_thread(parent, parent ? parent->d_func()->threadData.loadRelaxed() : nullptr, threadData)) {
            parent = nullptr;
        }
        if (d->willBeWidget) {
            if (parent) {
                d->parent = parent;
                d->parent->d_func()->children.append(this);
            }
        }
        else {
            setParent(parent);
        }
    }

    if (Q_UNLIKELY(qtHookData[QHooks::AddQObject])) {
        reinterpret_cast<QHooks::AddQObjectCallback >(qtHookData[QHooks::AddQObject])(this);
    }
    Q_TRACE(QObject_ctor, this);
}

QObject::~QObject()
{
    Q_D(QObject);
    d->wasDeleted = true;
    d->blockSig = 0;  //避免destroyed信号被阻挡

    if (!d->children.isEmpty()) {
        d->deleteChildren();
    }

    if (d->parent) {
        d->setParent_helper(nullptr);
    }
}

void QObject::setParent(QObject *parent)
{
    Q_D(QObject);
    Q_ASSERT(!d->isWidget);
    d->setParent_helper(parent);
}

QString QObject::objectName() const
{
    Q_D(const QObject);
    if (QThread::currentThreadId() != d->threadData.loadRelaxed()->threadId.loadRelaxed()) {
        Q_ASSERT(false);  //多线程这样也不一定安全啊
        return d->extraData ? d->extraData->objectName.valueBypassingBindings() : QString();
    }
    if (!d->extraData && QtPrivate::isAnyBindingEvaluating()) {
        Q_ASSERT(false);  //判断QtPrivate::isAnyBindingEvaluating()是什么用意？
        QObjectPrivate *dd = const_cast<QObjectPrivate *>(d);
        dd->extraData = new QObjectPrivate::ExtraData(dd);
    }
    return d->extraData ? d->extraData->objectName : QString();
}

void QObject::doSetObjectName(const QString &name)
{
    Q_D(QObject);
    d->ensureExtraData();
    //zhaoyujie TODO inBindingWrapper是什么意思？
    d->extraData->objectName.removeBindingUnlessInWrapper();
    if (d->extraData->objectName.valueBypassingBindings() != name) {
        d->extraData->objectName.setValueBypassingBindings(name);
        d->extraData->objectName.notify();
    }
}

void QObject::setObjectName(QStringView name)
{
    Q_D(QObject);
    d->ensureExtraData();
    d->extraData->objectName.removeBindingUnlessInWrapper();
    if (d->extraData->objectName.valueBypassingBindings() != name) {
        d->extraData->objectName.setValueBypassingBindings(name.toString());
        d->extraData->objectName.notify();
    }
}

QBindable<QString> QObject::bindableObjectName()
{
    Q_D(QObject);
    d->ensureExtraData();
    return QBindable<QString>(&d->extraData->objectName);
}

bool QObject::setProperty(const char *name, const QVariant &value)
{
    return doSetProperty(name, &value, nullptr);
}

bool QObject::setProperty(const char *name, QVariant &&value)
{
    return doSetProperty(name, &value, &value);
}

/*
 * property是使用Q_PROPERTY定义，写成功返回true，否则返回false
 * 属性分为Q_PROPERTY定义的属性和定义在QObjectPrivate中的动态属性
 * */
bool QObject::doSetProperty(const char *name, const QVariant *lvalue, QVariant *rvalue)
{
    Q_D(QObject);
    const auto &value = *lvalue;
    //获取metaObject
    const QMetaObject *meta = metaObject();
    if (!name || !meta) {
        return false;
    }
    int id = meta->indexOfProperty(name);
    if (id < 0) {  //不是元对象中定义的属性，为动态属性
        d->ensureExtraData();
        const int idx = d->extraData->propertyNames.indexOf(name);
        if (!value.isValid()) {
            //移除属性
            if (idx == -1) {
                return false;
            }
            d->extraData->propertyNames.removeAt(idx);
            d->extraData->propertyValues.removeAt(idx);
        }
        else {
            if (idx == -1) {
                d->extraData->propertyNames.append(name);
                if (rvalue) {
                    d->extraData->propertyValues.append(std::move(*rvalue));
                }
                else {
                    d->extraData->propertyValues.append(*lvalue);
                }
            }
            else {
                if (value.userType() == d->extraData->propertyValues.at(idx).userType()
                      && value == d->extraData->propertyValues.at(idx)) {
                    //值相同
                    return false;
                }
                if (rvalue) {
                    d->extraData->propertyValues[idx] = std::move(*rvalue);
                }
                else {
                    d->extraData->propertyValues[idx] = *lvalue;
                }
            }
        }
        //发出动态属性变更事件
        QDynamicPropertyChangeEvent ev(name);
        QCoreApplication::sendEvent(this, &ev);
        return false;
    }

    QMetaProperty p = meta->property(id);
    return rvalue ? p.write(this, std::move(*rvalue)) : p.write(this, *lvalue);
}

QVariant QObject::property(const char *name) const
{
    Q_D(const QObject);
    const QMetaObject *meta = metaObject();
    if (!name || !meta) {
        return QVariant();
    }
    int id = meta->indexOfProperty(name);
    if (id < 0) {  //动态属性
        if (!d->extraData) {
            return QVariant();
        }
        const int i = d->extraData->propertyNames.indexOf(name);
        return d->extraData->propertyValues.value(i);
    }
    else {  //Q_PROPERTY定义的属性
        QMetaProperty p = meta->property(id);
        return p.read(this);
    }
}

//动态属性
QList<QByteArray> QObject::dynamicPropertyNames() const
{
    Q_D(const QObject);
    if (d->extraData) {
        return d->extraData->propertyNames;
    }
    return QList<QByteArray>();
}

bool QObject::blockSignals(bool b) noexcept
{
    Q_D(QObject);
    bool previous = d->blockSig;
    d->blockSig = b;
    return previous;
}

QThread *QObject::thread() const
{
    return d_func()->threadData.loadRelaxed()->thread.loadRelaxed();
}

void QObject::moveToThread(QThread *targetThread)
{
    Q_ASSERT(false);
}

void QObject::installEventFilter(QObject *filterObj)
{
    Q_D(QObject);
    if (!filterObj) {
        return;
    }
    if (d->threadData.loadRelaxed() != filterObj->d_func()->threadData.loadRelaxed()) {
        //相同线程才可以install
        Q_ASSERT(false);
        return;
    }
    d->ensureExtraData();
    d->extraData->eventFilters.removeAll((QObject *)nullptr);
    d->extraData->eventFilters.removeAll(filterObj);
    d->extraData->eventFilters.prepend(filterObj);
}

void QObject::removeEventFilter(QObject *obj)
{
    Q_D(QObject);
    if (d->extraData) {
        //直接removeAll不好吗？
        for (int i = 0; i < d->extraData->eventFilters.size(); ++i) {
            if (d->extraData->eventFilters.at(i) == obj) {
                d->extraData->eventFilters[i] = nullptr;
            }
        }
    }
}

bool QObject::event(QEvent *e)
{
    switch (e->type()) {
        case QEvent::Timer:
            timerEvent((QTimerEvent *) e);
            break;
        case QEvent::ChildAdded:
        case QEvent::ChildRemoved:
        case QEvent::ChildPolished:
            childEvent((QChildEvent *) e);
            break;
        case QEvent::DeferredDelete:
            Q_ASSERT(false);
            break;
        case QEvent::MetaCall: {
            Q_ASSERT(false);
            break;
        }
        case QEvent::ThreadChange: {
            Q_ASSERT(false);
            break;
        }
        default:
            if (e->type() >= QEvent::User) {
                customEvent(e);
                break;
            }
            return false;
    }
    return true;
}

bool QObject::eventFilter(QObject *, QEvent *)
{
    return false;
}

void QObject::timerEvent(QTimerEvent *event)
{
}

void QObject::childEvent(QChildEvent *event)
{
}

void QObject::customEvent(QEvent *event)
{
}

void QObject::deleteLater()
{
    if (qApp == this) {
        qWarning("You are deferring the delete of QCoreApplication, this may not work as expected.");
    }
    QCoreApplication::postEvent(this, new QDeferredDeleteEvent());
}

QObject *QObject::sender() const
{
    Q_D(const QObject);
    QMutexLocker locker(signalSlotLock(this));
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
    QMutexLocker locker(signalSlotLock(this));
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
        if (!check_signal_macro(this, signal, "receivers", "bind")) {
            return 0;
        }
        signal++;
        int signal_index = d->signalIndex(signal);
        if (signal_index < 0) {
            err_method_notfound(this, signal - 1, "receivers");
            return 0;
        }
        if (!d->isSignalConnected(signal_index)) {
            return receivers;
        }
        if (!d->isDeletingChildren && d->declarativeData && QAbstractDeclarativeData::receivers) {
            Q_ASSERT(false);
        }

        QObjectPrivate::ConnectionData *cd = d->connections.loadRelaxed();
        QMutexLocker locker(signalSlotLock(this));
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
    Q_ASSERT(signal.mobj->cast(this) && signal.methodType() == QMetaMethod::Signal);
    uint signalIndex = signal.relativeMethodIndex();
    if (signal.data.flags() & MethodCloned) {
        signalIndex = QMetaObjectPrivate::originalClone(signal.mobj, signalIndex);
    }
    signalIndex += QMetaObjectPrivate::signalOffset(signal.mobj);

    QMutexLocker locker(signalSlotLock(this));
    return d->isSignalConnected(signalIndex, true);
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

    QtPrivate::QSlotObjectBase const *operator->() const {
        return m_slotObject.get();
    }

    QtPrivate::QSlotObjectBase *operator->() {
        return m_slotObject.get();
    }

    ~SlotObjectGuard() = default;

private:
    QtPrivate::SlotObjUniquePtr m_slotObject;
};

static void queued_activate(QObject *sender, int signal, QObjectPrivate::Connection *c, void **argv)
{
    Q_ASSERT(false);
}

template <bool callbacks_enabled>
void doActivate(QObject *sender, int signal_index, void **argv)
{
    QObjectPrivate *sp = QObjectPrivate::get(sender);
    if (sp->blockSig) {  //信号被阻塞了
        return;
    }

    Q_TRACE_SCOPE(QMetaObject_activate, sender, signal_index);

    //TODO 好像是一些spy的回掉？
    if (sp->isDeclarativeSignalConnected(signal_index) && QAbstractDeclarativeData::signalEmitted) {
        Q_TRACE_SCOPE(QMetaObject_activate_declarative_signal, sender, signal_index);
        QAbstractDeclarativeData::signalEmitted(sp->declarativeData, sender, signal_index, argv);
    }
    const QSignalSpyCallbackSet *signal_spy_set = callbacks_enabled ? qt_signal_spy_callback_set.loadAcquire() : nullptr;

    void *empty_argv[] = { nullptr };
    if (!argv) {
        argv = empty_argv;
    }

    auto TryCallSpySetSignalBeginFunc =[&]() {
        if (callbacks_enabled && signal_spy_set->signal_begin_callback != nullptr) {
            signal_spy_set->signal_begin_callback(sender, signal_index, argv);
        }
    };
    auto TryCallSpySetSignalEndFunc =[&]() {
        if (callbacks_enabled && signal_spy_set->signal_end_callback != nullptr) {
            signal_spy_set->signal_end_callback(sender, signal_index);
        }
    };

    if (!sp->maybeSignalConnected(signal_index)) {
        // The possible declarative connection is done, and nothing else is connected
        TryCallSpySetSignalBeginFunc();
        TryCallSpySetSignalEndFunc();
        return;
    }

    TryCallSpySetSignalBeginFunc();

    bool senderDeleted = false;
    {
        Q_ASSERT(sp->connections.loadAcquire());
        //使用connection保存，防止sender的connection被释放。触发函数中可能会delete sender
        QObjectPrivate::ConnectionDataPointer connections(sp->connections.loadRelaxed());
        QObjectPrivate::SignalVector *signalVector = connections->signalVector.loadRelaxed();

        const QObjectPrivate::ConnectionList *list;
        if (signal_index < signalVector->count()) {
            list = &signalVector->at(signal_index);
        }
        else {
            Q_ASSERT(false);  //TODO 什么时候会出现这种情况？
            list = &signalVector->at(-1);
        }

        Qt::HANDLE currentThreadId = QThread::currentThreadId();
        bool inSenderThread = currentThreadId == QObjectPrivate::get(sender)->threadData.loadRelaxed()->threadId.loadRelaxed();

        //记录当前的最高的id，触发函数中新建立的连接不会被触发
        uint highestConnectionId = connections->currentConnectionId.loadRelaxed();
        //遍历触发函数依次执行
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
                    || (c->connectionType == Qt::QueuedConnection)) {
                    //连接放到事件队列里 TODO
                    queued_activate(sender, signal_index, c, argv);
                    continue;
                }
                else if (c->connectionType == Qt::BlockingQueuedConnection){
                    //阻塞运行
                    if (receiverInSameThread) {
                        qWarning("Qt: Dead lock detected while activating a BlockingQueuedConnection: "
                                 "Sender is %s(%p), receiver is %s(%p)",
                                 sender->metaObject()->className(), sender,
                                 receiver->metaObject()->className(), receiver);
                    }
                    if (c->isSignalShot && !QObjectPrivate::removeConnection(c)) {
                        //只触发一次，尝试removeConnection
                        //removeConnection中，c被放在orphaned列表中，没有被销毁
                        continue;
                    }
                    Q_ASSERT(false);  //TODO通过postEvent放到循环中，
                    continue;
                }

                //直接调用
                if (c->isSignalShot && !QObjectPrivate::removeConnection(c)) {
                    continue;
                }
                QObjectPrivate::Sender senderData(receiverInSameThread ? receiver : nullptr, sender, signal_index);
                if (c->isSlotObject) {
                    //connect函数指针形式
                    SlotObjectGuard obj{ c->slotObj };
                    Q_TRACE_SCOPE(QMetaObject_activate_slot_functor, c->slotObj);
                    obj->call(receiver, argv);
                }
                else if (c->callFunction && c->method_offset <= receiver->metaObject()->methodOffset()) {
                    //qt_metacall，触发元方法
                    const int method_relative = c->method_relative;
                    const auto callFunction = c->callFunction;
                    const int methodIndex = (Q_HAS_TRACEPOINTS || callbacks_enabled) ? c->method() : 0;
                    TryCallSpySetSignalBeginFunc();
                    Q_TRACE_SCOPE(QMetaObject_activate_slot, receiver, methodIndex);
                    //调用执行函数
                    callFunction(receiver, QMetaObject::InvokeMetaMethod, method_relative, argv);
                    TryCallSpySetSignalEndFunc();
                }
                else {
                    const int method = c->method_relative + c->method_offset;
                    TryCallSpySetSignalBeginFunc();
                    Q_TRACE_SCOPE(QMetaObject_activate_slot, receiver, method);
                    QMetaObject::metacall(receiver, QMetaObject::InvokeMetaMethod, method, argv);
                    TryCallSpySetSignalEndFunc();
                }
            } while ((c = c->nextConnectionList.loadRelaxed()) != nullptr && c->id <= highestConnectionId);
        } while (list != &signalVector->at(-1) && ((list = &signalVector->at(-1)), true));

        //槽函数运行过程中，sender可能被删除掉了。QObject的析构函数会将connectionData的currentConnectId设置为0
        if (connections->currentConnectionId.loadRelaxed() == 0) {
            senderDeleted = true;
        }
    }
    if (!senderDeleted) {
        //在触发函数中断开了的链接会在这里清除
        sp->connections.loadRelaxed()->cleanOrphanedConnections(sender);
        TryCallSpySetSignalEndFunc();
    }
}

void QMetaObject::activate(QObject *sender, int signal_offset, int local_signal_index, void **argv)
{
    int signal_index = local_signal_index + signal_offset;
    if (Q_UNLIKELY(qt_signal_spy_callback_set.loadRelaxed())) {
        doActivate<true>(sender, signal_index, argv);
    }
    else {
        doActivate<false>(sender, signal_index, argv);
    }
}

void QMetaObject::activate(QObject *sender, const QMetaObject *m, int local_signal_index, void **argv)
{
    int signal_index = local_signal_index + QMetaObjectPrivate::signalOffset(m);
    if (Q_UNLIKELY(qt_signal_spy_callback_set.loadRelaxed()))
        doActivate<true>(sender, signal_index, argv);
    else
        doActivate<false>(sender, signal_index, argv);
}

//查找object与其所有的child，自动连接on_<object name>_<signal name>(<signal parameters>)的信号
//比如QPushButton有信号clicked，自动与on_button1_clicked()这样格式的槽函数相连
void QMetaObject::connectSlotsByName(QObject *o)
{
    if (!o)
        return;
    const QMetaObject *mo = o->metaObject();
    Q_ASSERT(mo);
    const QObjectList list = o->findChildren<QObject *>() << o;

    // for each method/slot of o ...
    for (int i = 0; i < mo->methodCount(); ++i) {
        const QByteArray slotSignature = mo->method(i).methodSignature();
        const char *slot = slotSignature.constData();
        Q_ASSERT(slot);

        // ...that starts with "on_", ...
        if (slot[0] != 'o' || slot[1] != 'n' || slot[2] != '_')
            continue;

        // ...we check each object in our list, ...
        bool foundIt = false;
        for (int j = 0; j < list.size(); ++j) {
            const QObject *co = list.at(j);
            const QByteArray coName = co->objectName().toLatin1();

            // ...discarding those whose objectName is not fitting the pattern "on_<objectName>_...", ...
            if (coName.isEmpty() || qstrncmp(slot + 3, coName.constData(), coName.size()) || slot[coName.size()+3] != '_')
                continue;

            const char *signal = slot + coName.size() + 4; // the 'signal' part of the slot name

            // ...for the presence of a matching signal "on_<objectName>_<signal>".
            const QMetaObject *smeta;
            int sigIndex = co->d_func()->signalIndex(signal, &smeta);
            if (sigIndex < 0) {
                // if no exactly fitting signal (name + complete parameter type list) could be found
                // look for just any signal with the correct name and at least the slot's parameter list.
                // Note: if more than one of those signals exist, the one that gets connected is
                // chosen 'at random' (order of declaration in source file)
                QList<QByteArray> compatibleSignals;
                const QMetaObject *smo = co->metaObject();
                int sigLen = int(qstrlen(signal)) - 1; // ignore the trailing ')'
                for (int k = QMetaObjectPrivate::absoluteSignalCount(smo)-1; k >= 0; --k) {
                    const QMetaMethod method = QMetaObjectPrivate::signal(smo, k);
                    if (!qstrncmp(method.methodSignature().constData(), signal, sigLen)) {
                        smeta = method.enclosingMetaObject();
                        sigIndex = k;
                        compatibleSignals.prepend(method.methodSignature());
                    }
                }
                if (compatibleSignals.size() > 1) {
                    Q_ASSERT(false); //TODO
                }
            }

            if (sigIndex < 0)
                continue;

            // we connect it...
            if (Connection(QMetaObjectPrivate::connect(co, sigIndex, smeta, o, i))) {
                foundIt = true;
//                qCDebug(lcConnectSlotsByName, "%s",
//                        msgConnect(smeta, coName, QMetaObjectPrivate::signal(smeta, sigIndex), o,  i).constData());
                // ...and stop looking for further objects with the same name.
                // Note: the Designer will make sure each object name is unique in the above
                // 'list' but other code may create two child objects with the same name. In
                // this case one is chosen 'at random'.
                break;
            }
        }
        if (foundIt) {
            // we found our slot, now skip all overloads
            while (mo->method(i + 1).attributes() & QMetaMethod::Cloned)
                ++i;
        } else if (!(mo->method(i).attributes() & QMetaMethod::Cloned)) {
            // check if the slot has the following signature: "on_..._...(..."
            int iParen = slotSignature.indexOf('(');
            int iLastUnderscore = slotSignature.lastIndexOf('_', iParen - 1);
            if (iLastUnderscore > 3)
                    qCWarning(lcConnectSlotsByName,
                              "QMetaObject::connectSlotsByName: No matching signal for %s", slot);
        }
    }
}

static int getConnectReceiverInfo(int membcode, const QMetaObject **rmeta, const QByteArray &methodName, int argc, const QArgumentType *types)
{
    if (membcode == QSLOT_CODE) {
        return QMetaObjectPrivate::indexOfSlotRelative(rmeta, methodName, argc, types);
    }
    else {
        return QMetaObjectPrivate::indexOfSignalRelative(rmeta, methodName, argc, types);
    }
}

QMetaObject::Connection QObject::connect(const QObject *sender, const char *signal, const QObject *receiver, const char *method, Qt::ConnectionType type)
{
    if (sender == nullptr || receiver == nullptr || signal == nullptr || method == nullptr) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }
    QByteArray tmp_signal_name;
    //检查信号标志
    if (!check_signal_macro(sender, signal, "connect", "bind")) {
        return QMetaObject::Connection(nullptr);
    }
    const QMetaObject *smeta = sender->metaObject();
    const char *signal_org = signal;
    ++signal;  //跳过标志位
    QArgumentTypeArray signalTypes;
    Q_ASSERT(QMetaObjectPrivate::get(smeta)->revision >= 7);
    //提取信号名称，提取参数名称
    QByteArray signalName = QMetaObjectPrivate::decodeMethodSignature(signal, signalTypes);
    int signal_index = QMetaObjectPrivate::indexOfSignalRelative(&smeta, signalName, signalTypes.size(), signalTypes.constData());
    if (signal_index < 0) {
        //不是标准化的写法，先做一下签名标准化
        tmp_signal_name = QMetaObject::normalizedSignature(signal - 1);
        signal = tmp_signal_name.constData() + 1;
        signalTypes.clear();
        signalName = QMetaObjectPrivate::decodeMethodSignature(signal, signalTypes);
        smeta = sender->metaObject();
        signal_index = QMetaObjectPrivate::indexOfSignalRelative(&smeta, signalName, signalTypes.size(), signalTypes.constData());
    }
    if (signal_index < 0) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }
    //消除克隆的影响，查找到原方法。原方法签名中的函数参数一定比克隆出来的函数参数要多
    signal_index = QMetaObjectPrivate::originalClone(smeta, signal_index);
    //算上偏移取绝对索引
    signal_index += QMetaObjectPrivate::signalOffset(smeta);

    QByteArray tmp_method_name;
    int membcode = extract_code(method);
    if (!check_method_code(membcode, receiver, method, "connect")) {
        return QMetaObject::Connection(nullptr);
    }
    const char *method_arg = method;
    ++method;

    //receiver可能是槽函数也可能是信号函数
    QArgumentTypeArray methodTypes;
    QByteArray methodName = QMetaObjectPrivate::decodeMethodSignature(method, methodTypes);
    const QMetaObject *rmeta = receiver->metaObject();
    int method_index_relative = -1;
    Q_ASSERT(QMetaObjectPrivate::get(rmeta)->revision >= 7);
    method_index_relative = getConnectReceiverInfo(membcode, &rmeta, methodName, methodTypes.size(), methodTypes.constData());
    if (method_index_relative < 0) {
        tmp_method_name = QMetaObject::normalizedSignature(method);
        method = tmp_method_name.constData();
        methodTypes.clear();
        rmeta = receiver->metaObject();
        method_index_relative = getConnectReceiverInfo(membcode, &rmeta, methodName, methodTypes.size(), methodTypes.constData());
    }

    if (method_index_relative < 0) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }

    //检测参数是否匹配
    if (!QMetaObjectPrivate::checkConnectArgs(signalTypes.size(), signalTypes.constData(),
                                              methodTypes.size(), methodTypes.constData())) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }

    int *types = nullptr;
    if (type == Qt::QueuedConnection) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }

    //连接
    auto c = QMetaObjectPrivate::connect(sender, signal_index, smeta, receiver, method_index_relative, rmeta, type, types);
    QMetaObject::Connection handle = QMetaObject::Connection(c);
    return handle;
}

QMetaObject::Connection QObject::connect(const QObject *sender, const QMetaMethod &signal, const QObject *receiver, const QMetaMethod &method, Qt::ConnectionType type)
{
    //sender与receiver要存在，signal不许是信号，method不能为构造函数
    if (sender == nullptr || receiver == nullptr
            || signal.methodType() != QMetaMethod::Signal
            || method.methodType() == QMetaMethod::Constructor)
    {
        Q_ASSERT(false);
        qCWarning(lcConnect, "QObject::connect: Cannot connect %s::%s to %s::%s",
                  sender ? sender->metaObject()->className() : "(nullptr)",
                  signal.methodSignature().constData(),
                  receiver ? receiver->metaObject()->className() : "(nullptr)",
                  method.methodSignature().constData());
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
    if (!QMetaObject::checkConnectArgs(signal.methodSignature().constData(),
                                       method.methodSignature().constData())) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }

    int *types = nullptr;
    if (type == Qt::QueuedConnection) {
        Q_ASSERT(false);
        return QMetaObject::Connection(nullptr);
    }

    //zhaoyujie TODO  参数里的smeta为什么使用nullptr？这里的是什么设计系统？
    Q_ASSERT(false);
    auto c = QMetaObjectPrivate::connect(sender, signal_index, signal.enclosingMetaObject(), receiver, method_index, nullptr, type, types);
    return QMetaObject::Connection(c);
}

QMetaObject::Connection QObject::connect(const QObject *sender, const char *signal, const char *member, Qt::ConnectionType type) const
{
    return connect(sender, signal, this, member, type);
}

/*
 * 1. 断开sender的所有链接  disconnect(sender, nullptr, nullptr, nullptr)
 *    等同于 sender->disconnect();
 * 2. 断开到sender的特定信号的链接  disconnect(sender, SIGNAL(sig1()), nullptr, nullptr)
 *    等同于 sender->disconnect(SIGNAL(sig1()))
 * 3. 断开sender与receiver的链接  disconnect(sender, nullptr, receiver, nullptr)
 *    等同于 sender->disconnect(receiver)
 * 4. 断开sender与receiver特定slot的链接   disconnect(sender, nullptr, receiver, SLOT(onSig1()))
 * 5. 断开sender特性signal与receiver特性slot的链接  disconnect(sender, SIGNAL(sig1()), receiver, SLOT(onSig1()))
 * */
bool QObject::disconnect(const QObject *sender, const char *signal, const QObject *receiver, const char *method)
{
    bool senderInvalid = sender == nullptr;  //sender不能为空
    bool receiverInvalid = receiver == nullptr && method != nullptr;  //receiver为空时，method不能为空
    if (senderInvalid || receiverInvalid) {
        Q_ASSERT(false);
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
        //解析signalName与signalTypes
        signalName = QMetaObjectPrivate::decodeMethodSignature(signal, signalTypes);
    }
    QByteArray methodName;
    QArgumentTypeArray methodTypes;
    Q_ASSERT(!receiver || QMetaObjectPrivate::get(receiver->metaObject())->revision >= 7);
    if (method) {
        //解析methodName与methodTypes
        methodName = QMetaObjectPrivate::decodeMethodSignature(method, methodTypes);
    }

    do {
        int signal_index = -1;
        if (signal) {
            signal_index = QMetaObjectPrivate::indexOfSignalRelative(&smeta, signalName, signalTypes.size(), signalTypes.constData());
            if (signal_index < 0) {
                break;  //indexOfSignalRelative会在继承链条上向上查找，查找不到整个链条上都没有同名方法
            }
            signal_index = QMetaObjectPrivate::originalClone(smeta, signal_index);
            signal_index += QMetaObjectPrivate::signalOffset(smeta);
            signal_found = true;
        }

        if (!method) {
            res != QMetaObjectPrivate::disconnect(sender, signal_index, smeta, receiver, -1, nullptr);
        }
        else {
            const QMetaObject *rmeta = receiver->metaObject();
            do {
                //indexOfMethod返回绝对值
                int method_index = QMetaObjectPrivate::indexOfMethod(rmeta, methodName, methodTypes.size(), methodTypes.constData());
                if (method_index >= 0) {
                    //找到method对应的rmeta
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
        if (!signal) {
            //没有具体信号时，在这里调用disconnectNotify
            const_cast<QObject *>(sender)->disconnectNotify(QMetaMethod());
        }
    }
    return res;
}

bool QObject::disconnect(const QObject *sender, const QMetaMethod &signal, const QObject *receiver, const QMetaMethod &method)
{
    if (sender == nullptr || (receiver == nullptr && method.mobj != nullptr)) {
        qCWarning(lcConnect, "QObject::disconnect: Unexpected nullptr parameter");
    }
    if (signal.mobj) {
        if (signal.methodType() != QMetaMethod::Signal) {
            Q_ASSERT(false);
            return false;
        }
    }
    if (method.mobj) {
        if (method.methodType() == QMetaMethod::Constructor) {
            Q_ASSERT(false);
            return false;
        }
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
    if (!signal.isValid()) {
        //没有具体信号时，在这里调用disconnectNotify
        const_cast<QObject *>(sender)->disconnectNotify(signal);
    }
    return true;
}

bool QObject::disconnect(const char *signal, const QObject *receiver, const char *member) const
{
    return disconnect(this, signal, receiver, member);
}

bool QObject::disconnect(const QMetaObject::Connection &connection)
{
    QObjectPrivate::Connection *c = static_cast<QObjectPrivate::Connection *>(connection.d_ptr);
    if (!c) {
        return false;
    }
    const bool disconnected = QObjectPrivate::removeConnection(c);
    const_cast<QMetaObject::Connection &>(connection).d_ptr = nullptr;
    c->deref();  //deref里面会delete
    return disconnected;
}

QMetaObject::Connection QObject::connectImpl(const QObject *sender, void **signal,
                                             const QObject *receiver, void **slot,
                                             QtPrivate::QSlotObjectBase *slotObjRaw, Qt::ConnectionType type,
                                             const int *types, const QMetaObject *senderMetaObject)
{
    QtPrivate::SlotObjUniquePtr slotObj(slotObjRaw);
    if (!signal) {
        qCWarning(lcConnect, "QObject::connect: invalid nullptr parameter");
        return QMetaObject::Connection();
    }

    //查找信号在sender中是否存在
    int signal_index = -1;
    void *args[] = { &signal_index, signal };
    for (; senderMetaObject && signal_index < 0; senderMetaObject = senderMetaObject->superClass()) {
        senderMetaObject->static_metacall(QMetaObject::IndexOfMethod, 0, args);
        if (signal_index >= 0 && signal_index < QMetaObjectPrivate::get(senderMetaObject)->signalCount)
            break;
    }
    if (!senderMetaObject) {
        qCWarning(lcConnect, "QObject::connect: signal not found");
        return QMetaObject::Connection(nullptr);
    }

    signal_index += QMetaObjectPrivate::signalOffset(senderMetaObject);
    return QObjectPrivate::connectImpl(sender, signal_index, receiver, slot, slotObj.release(), type, types, senderMetaObject);
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

QDebug operator<<(QDebug dbg, const QObject *o)
{
    Q_ASSERT(false);
    return dbg;
}

QMetaObject::Connection::Connection()
    : d_ptr(nullptr)
{
}

QMetaObject::Connection::Connection(const Connection &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr) {
        static_cast<QObjectPrivate::Connection *>(d_ptr)->ref();
    }
}

QMetaObject::Connection &QMetaObject::Connection::operator=(const Connection &other)
{
    if (other.d_ptr != d_ptr) {
        if (d_ptr) {
            static_cast<QObjectPrivate::Connection *>(d_ptr)->deref();
        }
        d_ptr = other.d_ptr;
        if (other.d_ptr) {
            static_cast<QObjectPrivate::Connection *>(other.d_ptr)->ref();
        }
    }
    return *this;
}

QMetaObject::Connection::~Connection()
{
    if (d_ptr) {
        static_cast<QObjectPrivate::Connection *>(d_ptr)->deref();
    }
}

bool QMetaObject::Connection::isConnected_helper() const
{
    Q_ASSERT(d_ptr);
    QObjectPrivate::Connection *c = static_cast<QObjectPrivate::Connection *>(d_ptr);
    return c->receiver.loadRelaxed();
}

void QObjectPrivate::setParent_helper(QObject *o)
{
    Q_Q(QObject);
    Q_ASSERT(q != o);

    //检查深度
#ifdef QT_DEBUG
    const auto checkForParentChildLoops = qScopeGuard([&]() {
        int depth = 0;
        auto p = parent;
        while (p) {
            if (++depth == CheckForParentChildLoopsWarnDepth) {
                qWarning("QObject %p (class: '%s', object name: '%s') may have a loop in its parent-child chain; "
                         "this is undefined behavior",
                         q, q->metaObject()->className(), qPrintable(q->objectName()));
            }
            p = p->parent();
        }
    });
#endif

    if (o == parent) {
        return;
    }
    if (parent) {
        //此object的当前的父
        QObjectPrivate *parentD = parent->d_func();

        /*
         * 区分四种case：
         * 1. parentD没有deleteChildren
         * 2. parent正在deleteChildren，我已经被删除掉了
         * 3. parent正在deleteChildren，我正在被删除
         * 4. parent正在deleteChildren，我还没被删除
         * */
        if (parentD->isDeletingChildren && wasDeleted && parentD->currentChildBeingDeleted == q) {
            //正在从parent中删除我这个子
            Q_ASSERT(false);
        }
        else {
            const int index = parentD->children.indexOf(q);
            if (index < 0) {
                //在childRemoved事件中调用了setParent，parent的值还没有改变，但是parent的children里已经删掉了此object
            }
            else if (parentD->isDeletingChildren) {
                //parent正在删除children，但是还没有删除到我。需要将我从children中移除，否则会错误将我析构
                parentD->children[index] = nullptr;
            }
            else {
                //parentD没有deletingChildren
                parentD->children.removeAt(index);
                if (sendChildEvents && parentD->receiveChildEvents) {
                    QChildEvent e(QEvent::ChildRemoved, q);
                    QCoreApplication::sendEvent(parent, &e);
                }
            }
        }
    }
    parent = o;
    if (parent) {
        if (threadData.loadRelaxed() != parent->d_func()->threadData.loadRelaxed()) {
            //不同的线程对象没有办法建立父子关系
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
            else {
                //TODO  isWidget有什么特殊？
                Q_ASSERT(false);
            }
        }
    }
}

void QObjectPrivate::deleteChildren()
{
    Q_ASSERT(!isDeletingChildren);
    isDeletingChildren = true;
    for (int i = 0; i < children.size(); ++i) {
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

inline void QObjectPrivate::ensureConnectionData()
{
    if (connections.loadRelaxed()) {
        return;
    }
    ConnectionData *cd = new ConnectionData;
    cd->ref.ref();
    connections.storeRelaxed(cd);
}

inline void QObjectPrivate::addConnection(int signal, Connection *c)
{
    Q_ASSERT(c->sender == q_ptr);
    //添加到发送者的链条中
    ensureConnectionData();
    ConnectionData *cd = connections.loadRelaxed();
    cd->resizeSignalVector(signal + 1);
    ConnectionList &connectionList = cd->connectionsForSignal(signal);
    if (connectionList.last.loadRelaxed()) {
        Q_ASSERT(connectionList.last.loadRelaxed()->receiver.loadRelaxed());
        connectionList.last.loadRelaxed()->nextConnectionList.storeRelaxed(c);
    }
    else {
        connectionList.first.storeRelaxed(c);
    }
    c->id = ++cd->currentConnectionId;
    c->prevConnectionList = connectionList.last.loadRelaxed();
    connectionList.last.storeRelaxed(c);

    //添加到接受者中
    QObjectPrivate *rd = QObjectPrivate::get(c->receiver.loadRelaxed());
    rd->ensureConnectionData();

    //每次有新的Connection，都将senders设置为此Connection，然后将此Connection与原来的首Connection建立链接关系
    c->prev = &(rd->connections.loadRelaxed()->senders);
    c->next = *c->prev;
    *c->prev = c;
    if (c->next) {
        c->next->prev = &c->next;
    }
}

bool QObjectPrivate::isSignalConnected(uint signalIndex, bool checkDeclarative) const
{
    if (checkDeclarative && !isDeclarativeSignalConnected(signalIndex)) {
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
    if (signalVector->at(-1).first.loadRelaxed()) {
        return true;
    }

    if (signalIndex < uint(cd->signalVectorCount())) {
        const QObjectPrivate::Connection *c = signalVector->at(signalIndex).first.loadRelaxed();
        while (c) {
            if (c->receiver.loadRelaxed()) {  //TODO 为什么要判断receiver？
                return true;
            }
            else {
                Q_ASSERT(false);
            }
            c = c->nextConnectionList.loadRelaxed();
        }
    }
    return false;
}

inline bool QObjectPrivate::removeConnection(Connection *c)
{
    if (!c) {
        return false;
    }
    QObject *receiver = c->receiver.loadRelaxed();
    if (!receiver) {
        return false;
    }
    QBasicMutex *senderMutex = signalSlotLock(c->sender);
    QBasicMutex *receiverMutex = signalSlotLock(receiver);

    QObjectPrivate::ConnectionData *connections;
    {
        QOrderedMutexLocker locker(senderMutex, receiverMutex);

        connections = QObjectPrivate::get(c->sender)->connections.loadRelaxed();
        Q_ASSERT(connections);
        connections->removeConnection(c);

        c->sender->disconnectNotify(QMetaObjectPrivate::signal(c->sender->metaObject(), c->signal_index));
        if (receiverMutex != senderMutex) {
            receiverMutex->unlock();
        }
        connections->cleanOrphanedConnections(c->sender, ConnectionData::AlreadyLockedAndTemporarilyReleasingLock);
        senderMutex->unlock();
        locker.dismiss();
    }
    return true;
}

static void connectWarning(const QObject *sender, const QMetaObject *senderMetaObject, const QObject *receiver, const char *message)
{
}

QMetaObject::Connection QObjectPrivate::connectImpl(const QObject *sender, int signal_index,
                                                    const QObject *receiver, void **slot,
                                                    QtPrivate::QSlotObjectBase *slotObjRaw, int type,
                                                    const int *types, const QMetaObject *senderMetaObject)
{
    QtPrivate::SlotObjUniquePtr slotObj(slotObjRaw);

    if (!sender || !receiver || !slotObj || !senderMetaObject) {
        connectWarning(sender, senderMetaObject, receiver, "invalid nullptr parameter");
        return QMetaObject::Connection();
    }
    if (type & Qt::UniqueConnection && !slot) {
        //uniqueConnection必须是函数成员
        connectWarning(sender, senderMetaObject, receiver, "unique connections require a pointer to member function of a QObject subclass");
        return QMetaObject::Connection();
    }
    QObject *s = const_cast<QObject *>(sender);
    QObject *r = const_cast<QObject *>(receiver);

    QOrderedMutexLocker locker(signalSlotLock(sender),
                               signalSlotLock(receiver));

    if (type & Qt::UniqueConnection && slot && QObjectPrivate::get(s)->connections.loadRelaxed()) {
        QObjectPrivate::ConnectionData *connections = QObjectPrivate::get(s)->connections.loadRelaxed();
        if (connections->signalVectorCount() > signal_index) {
            const QObjectPrivate::Connection *c2 = connections->signalVector.loadRelaxed()->at(signal_index).first.loadRelaxed();

            while (c2) {
                if (c2->receiver.loadRelaxed() == receiver && c2->isSlotObject && c2->slotObj->compare(slot)) {
                    return QMetaObject::Connection();
                }
                c2 = c2->nextConnectionList.loadRelaxed();
            }
        }
    }
    type &= ~Qt::UniqueConnection;

    const bool isSingleShot = type & Qt::SingleShotConnection;
    type &= ~Qt::SingleShotConnection;

    Q_ASSERT(type >= 0 && type <= 3);

    std::unique_ptr<QObjectPrivate::Connection> c{new QObjectPrivate::Connection};
    c->sender = s;
    c->signal_index = signal_index;
    QThreadData *td = r->d_func()->threadData.loadAcquire();
    td->ref();
    c->receiverThreadData.storeRelaxed(td);
    c->receiver.storeRelaxed(r);
    c->connectionType = type;
    c->isSlotObject = true;
    c->slotObj = slotObj.release();
    if (types) {
        c->argumentTypes.storeRelaxed(types);
        c->ownArgumentTypes = false;
    }
    c->isSignalShot = isSingleShot;

    QObjectPrivate::get(s)->addConnection(signal_index, c.get());
    QMetaObject::Connection ret(c.release());
    locker.unlock();

    QMetaMethod method = QMetaObjectPrivate::signal(senderMetaObject, signal_index);
    Q_ASSERT(method.isValid());
    s->connectNotify(method);

    return ret;
}

void QObjectPrivate::ConnectionData::cleanOrphanedConnections(QObject *sender, LockPolicy lockPolicy)
{
    //zhaoyujie TODO 为什么要判断ref？
    if (orphaned.load(std::memory_order_relaxed) && ref.loadAcquire() == 1) {
        cleanOrphanedConnectionsImpl(sender, lockPolicy);
    }
    else {
        Q_ASSERT(false);
    }
}

void QObjectPrivate::ConnectionData::cleanOrphanedConnectionsImpl(QObject *sender, LockPolicy lockPolicy)
{
    QBasicMutex *senderMutex = signalSlotLock(sender);
    TaggedSignalVector c = nullptr;
    {
        std::unique_lock<QBasicMutex> lock(*senderMutex, std::defer_lock_t{});
        if (lockPolicy == NeedToLock) {
            lock.lock();
        }
        if (ref.loadAcquire() > 1) {
            //TODO ref什么时候值 > 1 ？
            Q_ASSERT(false);
            return;
        }
        //因为ref为1，上了锁之后，没有activate()被调用，
        //这意味着我们orphaned connection objects没有被其他引用
        //因此我们可以将其安全删除
        c = orphaned.exchange(nullptr, std::memory_order_relaxed);
    }
    if (c) {
        if (lockPolicy == AlreadyLockedAndTemporarilyReleasingLock) {
            Q_ASSERT(false);
            //TODO 这里unlock / lock是什么意图？
            senderMutex->unlock();
            deleteOrphaned(c);
            senderMutex->lock();
        }
        else {
            deleteOrphaned(c);
        }
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
    if (signalVector->at(-1).first.loadAcquire()) {
        Q_ASSERT(false);  //TODO -1有什么作用
        return true;
    }
    if (signalIndex < uint(cd->signalVectorCount())) {
        const QObjectPrivate::Connection *c = signalVector->at(signalIndex).first.loadAcquire();
        return c != nullptr;
    }
    return false;
}

static bool findConnectionInList(const QObjectPrivate::ConnectionList &connections, QObjectPrivate::Connection *c)
{
    for (QObjectPrivate::Connection *cc = connections.first.loadRelaxed(); cc; cc = cc->nextConnectionList.loadRelaxed()) {
        if (cc == c) {
            return true;
        }
    }
    return false;
}

inline QObjectPrivate::Connection::~Connection()
{
    if (ownArgumentTypes) {
        const int *v = argumentTypes.loadRelaxed();
        if (v != &DIRECT_CONNECTION_ONLY) {
            delete []v;
        }
    }
    if (isSlotObject) {
        slotObj->destroyIfLastRef();
    }
}

void QObjectPrivate::ConnectionData::removeConnection(QObjectPrivate::Connection *c)
{
    Q_ASSERT(c->receiver.loadRelaxed());
    ConnectionList &connections = signalVector.loadRelaxed()->at(c->signal_index);
    c->receiver.storeRelaxed(nullptr);
    //处理线程数据
    QThreadData *td = c->receiverThreadData.loadRelaxed();
    if (td) {
        td->deref();
    }
    c->receiverThreadData.storeRelaxed(nullptr);

    Q_ASSERT(findConnectionInList(connections, c));

    //从链接到接受者槽函数的链表中删除
    *c->prev = c->next;
    if (c->next) {
        c->next->prev = c->prev;
    }
    c->prev = nullptr;

    //处理链接到发送者信号的链表
    //处理头尾
    if (connections.first.loadRelaxed() == c) {
        connections.first.storeRelaxed(c->nextConnectionList.loadRelaxed());
    }
    if (connections.last.loadRelaxed() == c) {
        connections.last.storeRelaxed(c->prevConnectionList);
    }
    Q_ASSERT(signalVector.loadRelaxed()->at(c->signal_index).first.loadRelaxed() != c);
    Q_ASSERT(signalVector.loadRelaxed()->at(c->signal_index).last.loadRelaxed() != c);
    //将c从链表中移除
    Connection *n = c->nextConnectionList.loadRelaxed();
    if (n) {
        n->prevConnectionList = c->prevConnectionList;
    }
    if (c->prevConnectionList) {
        c->prevConnectionList->nextConnectionList.storeRelaxed(n);
    }
    c->prevConnectionList = nullptr;

    //加入到orphaned中
    Q_ASSERT(c != static_cast<Connection *>(orphaned.load(std::memory_order_relaxed)));
    TaggedSignalVector o = nullptr;
    o = orphaned.load(std::memory_order_acquire);
    do {
        c->nextInOrphanList = o;
    } while (!orphaned.compare_exchange_strong(o, TaggedSignalVector(c), std::memory_order_release));

    Q_ASSERT(!findConnectionInList(connections, c));
}

inline void QObjectPrivate::ConnectionData::deleteOrphaned(TaggedSignalVector o)
{
    Q_ASSERT(false);
}

QObjectPrivate::Connection *QMetaObjectPrivate::connect(const QObject *sender, int signal_index, const QMetaObject *smeta,
                                                        const QObject *receiver, int method_index, const QMetaObject *rmeta,
                                                        int type, int *types)
{
    QObject *s = const_cast<QObject *>(sender);
    QObject *r = const_cast<QObject *>(receiver);

    int method_offset = rmeta ? rmeta->methodOffset() : 0;
    Q_ASSERT(!rmeta || QMetaObjectPrivate::get(rmeta)->revision >= 6);

    QObjectPrivate::StaticMetaCallFunction callFunction = rmeta ? rmeta->d.static_metacall : nullptr;

    QOrderedMutexLocker locker(signalSlotLock(sender),
                               signalSlotLock(receiver));

    QObjectPrivate::ConnectionData *scd = QObjectPrivate::get(s)->connections.loadRelaxed();

    if (type & Qt::UniqueConnection && scd) {
        //uniqueConnection 保证唯一性
        if (scd->signalVectorCount() > signal_index) {
            const QObjectPrivate::Connection *c2 = scd->signalVector.loadRelaxed()->at(signal_index).first.loadRelaxed();
            int method_index_absolute = method_index + method_index;
            while (c2) {
                if (!c2->isSlotObject && c2->receiver.loadRelaxed() == receiver && c2->method() == method_index_absolute) {
                    return nullptr;
                }
                c2 = c2->nextConnectionList.loadRelaxed();
            }
        }
    }

    type &= ~Qt::UniqueConnection;

    const bool isSignalShot = type & Qt::SingleShotConnection;
    type & ~Qt::SingleShotConnection;

    Q_ASSERT(type >= 0);
    Q_ASSERT(type <= 3);

    std::unique_ptr<QObjectPrivate::Connection> c{new QObjectPrivate::Connection};
    c->sender = s;
    c->signal_index = signal_index;
    c->receiver.storeRelaxed(r);
    QThreadData *td = r->d_func()->threadData.loadAcquire();
    td->ref();
    c->receiverThreadData.storeRelaxed(td);
    c->method_relative = method_index;
    c->method_offset = method_offset;
    c->connectionType = type;
    c->isSlotObject = false;
    c->argumentTypes.storeRelaxed(types);
    c->callFunction = callFunction;
    c->isSignalShot = isSignalShot;

    QObjectPrivate::get(s)->addConnection(signal_index, c.get());

    locker.unlock();
    QMetaMethod smethod = QMetaObjectPrivate::signal(smeta, signal_index);
    if (smethod.isValid()) {
        s->connectNotify(smethod);
    }
    return c.release();
}

//signal_index < 0 处理所有信号
//signal_index >= 0 处理单个信号
bool QMetaObjectPrivate::disconnect(const QObject *sender, int signal_index, const QMetaObject *smeta,
                                    const QObject *receiver, int method_index, void **slot,
                                    DisconnectType disconnectType)
{
    if (!sender) {
        return false;
    }
    QObject *s = const_cast<QObject *>(sender);

    QBasicMutex *senderMutex = signalSlotLock(sender);
    QMutexLocker locker(senderMutex);

    QObjectPrivate::ConnectionData *scd = QObjectPrivate::get(s)->connections.loadRelaxed();
    if (!scd) {
        return false;
    }

    bool success = false;
    {
        QObjectPrivate::ConnectionDataPointer connections(scd);

        if (signal_index < 0) {  //在所有信号中断开符合条件的链接
            //TODO 为什么从-1开始？
            for (int sig_index = -1; sig_index < scd->signalVectorCount(); ++sig_index) {
                if (disconnectHelper(connections.data(), signal_index, receiver, method_index, slot, senderMutex, disconnectType)) {
                    success = true;
                }
            }
        }
        else if (signal_index < scd->signalVectorCount()) {  //在单个信号中断开符合条件的链接
            if (disconnectHelper(connections.data(), signal_index, receiver, method_index, slot, senderMutex, disconnectType)) {
                success = true;
            }
        }
    }

    locker.unlock();
    if (success) {
        scd->cleanOrphanedConnections(s);

        //signal_index为-1在QObject::disconnect中做的通知
        //signal_index不为-1在这里通知
        QMetaMethod smethod = QMetaObjectPrivate::signal(smeta, signal_index);
        if (smethod.isValid()) {
            s->disconnectNotify(smethod);
        }
    }
    return success;
}

// 断开connectionData中和signalIndex相连的Connection
// method_index不为-1，断开单个连接
// method_index为-1，断开signalIndex上的所有连接
bool QMetaObjectPrivate::disconnectHelper(QObjectPrivate::ConnectionData *connections, int signalIndex,
                                          const QObject *receiver, int method_index, void **slot,
                                          QBasicMutex *senderMutex, DisconnectType disconnectType)
{
    bool success = false;
    auto &connectionList = connections->connectionsForSignal(signalIndex);
    auto *c = connectionList.first.loadRelaxed();
    while (c) {
        QObject *r = c->receiver.loadRelaxed();

        //method_index < 0 或者 connection是methodIndex类型的，methodIndex相等
        bool methodMatch = method_index < 0 || (!c->isSlotObject && c->method() == method_index);
        //slot为空，或者connection是slot类型的，slot相等
        bool slotMatch = (slot == nullptr) || (c->isSlotObject && c->slotObj->compare(slot));
        bool receiverMatch = receiver == r && methodMatch && slotMatch;
        if (r && (receiver == nullptr || receiverMatch)) {
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

        c = c->nextConnectionList.loadRelaxed();
    }
    return success;
}

void QMetaObjectPrivate::memberIndexes(const QObject *obj, const QMetaMethod &member, int *signalIndex, int *methodIndex)
{
    *signalIndex = -1;
    *methodIndex = -1;
    if (!obj || !member.mobj) {
        return;
    }
    //检查member是否是obj的方法
    const QMetaObject *m = obj->metaObject();
    while (m != nullptr && m != member.mobj) {
        m = m->d.superdata;
    }
    if (!m) {
        return;
    }
    //获取相对索引
    *signalIndex = *methodIndex = member.relativeMethodIndex();

    int signalOffset;
    int methodOffset;
    computeOffsets(m, &signalOffset, &methodOffset);

    *methodIndex += methodOffset;
    if (member.methodType() == QMetaMethod::Signal) {
        *signalIndex = originalClone(m, *signalIndex);
        *signalIndex += signalOffset;
    }
    else {
        *signalIndex = -1;
    }
}

QT_END_NAMESPACE