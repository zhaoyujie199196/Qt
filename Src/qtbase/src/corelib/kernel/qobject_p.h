//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QOBJECT_P_H
#define QOBJECT_P_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/private/qobjectdefs_impl.h>
#include <QtCore/private/qproperty_p.h>

QT_BEGIN_NAMESPACE

class QThreadData;

struct QSignalSpyCallbackSet
{
    typedef void (*BeginCallback)(QObject *caller, int signal_or_method_index, void **argv);
    typedef void (*EndCallback)(QObject *caller, int signal_or_method_index);
    BeginCallback signal_begin_callback;
    BeginCallback slot_begin_callback;
    EndCallback signal_end_callback;
    EndCallback slot_end_callback;
};

void qt_register_signal_spy_callbacks(QSignalSpyCallbackSet *callback_set);
extern QBasicAtomicPointer<QSignalSpyCallbackSet> qt_signal_spy_callback_set;

enum { QObjectPrivateVersion = QT_VERSION };

class QAbstractDeclarativeData
{
public:
    static void (*destroyed)(QAbstractDeclarativeData *, QObject *);
    static void (*signalEmitted)(QAbstractDeclarativeData *, QObject *, int, void **);
    static int  (*receivers)(QAbstractDeclarativeData *, const QObject *, int);
    static bool (*isSignalConnected)(QAbstractDeclarativeData *, const QObject *, int);
    static void (*setWidgetParent)(QObject *, QObject *);
};

//zhaoyujie TODO 这段代码什么作用，好奇怪的代码。，。。
struct QAbstractDynamicMetaObject;
struct QDynamicMetaObjectData
{
    virtual ~QDynamicMetaObjectData() {}
    virtual void objectDestroyed(QObject *) {
        Q_ASSERT(false);
        delete this;
    }
    virtual QAbstractDynamicMetaObject *toDynamicMetaObject(QObject *) = 0;
    virtual int metaCall(QObject *, QMetaObject::Call, int _id, void **) = 0;
};


struct QAbstractDynamicMetaObject : public QDynamicMetaObjectData, public QMetaObject
{
    ~QAbstractDynamicMetaObject() {}
    QAbstractDynamicMetaObject *toDynamicMetaObject(QObject *) override { return this; }
    int metaCall(QObject *, QMetaObject::Call c, int _id, void **a) override {
        return metaCall(c, _id, a);
    }
    virtual int metaCall(QMetaObject::Call, int _id,void **) {
        return _id;
    }
};

class QObjectPrivate : public QObjectData
{
    Q_DECLARE_PUBLIC(QObject)
public:
    typedef void(*StaticMetaCallFunction)(QObject *, QMetaObject::Call, int, void **);

    struct ExtraData {
        ExtraData(QObjectPrivate *ptr) : parent(ptr)
        {
        }

        QList<QByteArray> propertyNames;  //propertyNames与propertyValues为QObject的动态属性
        QList<QVariant> propertyValues;
        QObjectPrivate *parent = nullptr;  //zhaoyujie TODO 这个parent和QObjectData中的parent是不是重复了？

        inline void setObjectNameForwarder(const QString &name) {
            parent->q_func()->setObjectName(name);
        }

        inline void nameChangedForwarder(const QString &name) {
            emit parent->q_func()->objectNameChanged(name, QObject::QPrivateSignal());
        }

        Q_OBJECT_COMPAT_PROPERTY(QObjectPrivate::ExtraData, QString, objectName,
                                 &QObjectPrivate::ExtraData::setObjectNameForwarder,
                                 &QObjectPrivate::ExtraData::nameChangedForwarder)
    };

    struct Connection;
    struct SignalVector;
    struct ConnectionOrSignalVector {
        union {
            //zhaoyujie TODO
            ConnectionOrSignalVector *nextInOrphanList;   //用来串联孤儿
            Connection *next;
        };

        static SignalVector *asSignalVector(ConnectionOrSignalVector *c) {
            if (reinterpret_cast<quintptr>(c) & 1) {
                Q_ASSERT(false);
                return reinterpret_cast<SignalVector *>(reinterpret_cast<quintptr>(c) & ~quintptr(1u));
            }
            return nullptr;
        }

        static Connection *fromSignalVector(SignalVector *v) {
            Q_ASSERT(false);
            return reinterpret_cast<Connection *>(reinterpret_cast<quintptr >(v) | quintptr(1u));
        }
    };

    struct Connection : public ConnectionOrSignalVector {
        //目标为这个object中函数的Connection的链表，next在基础类中
        Connection **prev;
        //连接到信号的槽函数列表
        QAtomicPointer<Connection> nextConnectionList;
        Connection *prevConnectionList;

        QObject *sender;  //发送者
        QAtomicPointer<QObject> receiver; //接收者  //zhaoyujie TODO 为什么要使用QAtomicPointer？
        QAtomicPointer<QThreadData> receiverThreadData;  //接收者的线程数据

        union {
            StaticMetaCallFunction callFunction;  //执行函数 一般是moc中的qt_static_metacall函数, 使用的是SLOT()这种连接方式
            QtPrivate::QSlotObjectBase *slotObj;  //使用函数指针的连接方式
        };

        QAtomicPointer<const int> argumentTypes;
        QAtomicInt ref_; //引用计数
        uint id = 0;
        ushort method_offset;    //触发函数在方法偏移
        ushort method_relative;  //触发函数的相对索引
        signed int signal_index: 27;  //信号函数的索引
        ushort connectionType: 2;  //连接类型 0 == auto, 1 == direct, 2 == queued, 3 == blocking
        ushort isSlotObject :1;  //是否是槽函数
        ushort ownArgumentTypes: 1;  //zhaoyujie TODO
        ushort isSingleShot: 1;  //是否只触发一次

        int method() const {
            Q_ASSERT(!isSlotObject);
            return method_offset + method_relative;
        }

        void ref() {
            ref_.ref();
        }
        void deref() {
            if (!ref_.deref()) {
                Q_ASSERT(!receiver.loadRelaxed());  //加入orphan时，receiver应该设置为空？
                Q_ASSERT(!isSlotObject);
                delete this;
            }
        }

        void freeSlotObject()
        {
            if (isSlotObject) {   //使用函数指针构造了slotObject对象
                slotObj->destroyIfLastRef();
                isSlotObject = false;
            }
        }

        Connection() : ref_(2), ownArgumentTypes(true) {
            //zhaoyujie TODO 为什么ref_为2？
        }

        ~Connection();
    };

    struct ConnectionList {
        QAtomicPointer<Connection> first;  //first和last构成了一个链表
        QAtomicPointer<Connection> last;
    };

    struct Sender {
        Sender(QObject *receiver, QObject *sender, int signal)
            : receiver(receiver), sender(sender), signal(signal)
        {
            if (receiver) {
                ConnectionData *cd = receiver->d_func()->connections.loadRelaxed();
                previous = cd->currentSender;
                cd->currentSender = this;
            }
        }

        ~Sender() {
            if (receiver) {
                receiver->d_func()->connections.loadRelaxed()->currentSender = previous;
            }
        }

        void receiverDeleted() {
            //receiver被删除了，将Connection的receiver设置为空
            //receiverDeleted会形成空receiver的Connection，这些Connection在哪里销毁？
            Sender *s = this;
            while (s) {
                s->receiver = nullptr;
                s = s->previous;
            }
        }

        Sender *previous;
        QObject *receiver;
        QObject *sender;
        int signal;
    };

    //分配内存时，SignalVector后面紧跟着没陪了connectionList的内存，this + 1即为connectionList的地址
    //还不如直接用一个vector...
    //zhaoyujie TODO index的索引为什么从-1开始？
    struct SignalVector : public ConnectionOrSignalVector {
        quintptr allocated;
        ConnectionList &at(int i) {
            return reinterpret_cast<ConnectionList *>(this + 1)[i + 1];
        }
        const ConnectionList &at(int i) const {
            return reinterpret_cast<const ConnectionList *>(this + 1)[i + 1];
        }
        int count() const {
            return static_cast<int>(allocated);
        }
    };

    struct ConnectionData {
        enum LockPolicy {
            NeedToLock,
            // Beware that we need to temporarily release the lock
            // and thus calling code must carefully consider whether
            // invariants still hold.
            AlreadyLockedAndTemporarilyReleasingLock
        };

        QAtomicInteger<uint> currentConnectionId;
        QAtomicInt ref;
        QAtomicPointer<SignalVector> signalVector;  //信号数组，SignalVector中存放了连接到此信号的Connection
        Connection *senders = nullptr;    //以此conenctionData为目标的连接链表
        Sender *currentSender = nullptr;  //zhaoyujie TODO 这两个sender有啥用
        QAtomicPointer<Connection> orphaned;  //孤儿，使用原子指针

        ~ConnectionData() {  //从QObject中释放
            Q_ASSERT(ref.loadRelaxed() == 0);
            auto *c = orphaned.fetchAndStoreRelaxed(nullptr);
            if (c) {
                deleteOrphaned(c);
            }
            SignalVector *v = signalVector.loadRelaxed();
            if (v) {
                free(v);
            }
        }

        int signalVectorCount() const
        {
            //zhaoyujie TODO 为什么是-1？ 为什么不是0？
            return signalVector.loadAcquire() ? signalVector.loadRelaxed()->count() : -1;
        }

        void removeConnection(Connection *c);

        void resizeSignalVector(int size) {
            SignalVector *vector = this->signalVector.loadRelaxed();
            if (vector && vector->allocated > size) {
                return;
            }
            size = (size + 7) & ~7;  //size为8的倍数，做了容量冗余
            //分配内存时，ConnectionList内存紧跟在SignalVector的内存之后
            SignalVector *newVector = reinterpret_cast<SignalVector *>(malloc(sizeof(SignalVector) + (size + 1) * sizeof(ConnectionList)));
            int start = -1;  //zhaoyujie TODO 为什么start从-1开始
            //拷贝connectionList
            if (vector) {
                memcpy(newVector, vector, sizeof(SignalVector) + (vector->allocated + 1) * sizeof(ConnectionList));
                start = vector->count();
            }
            for (int i = start; i < int(size); ++i) {  //初始化ConnectionList的内存，有一些前面已经拷贝了，所以i从start开始
                newVector->at(i) = ConnectionList();
            }
            newVector->next = nullptr;
            newVector->allocated = size;
            signalVector.storeRelaxed(newVector);
            //zhaoyujie TODO 这段代码什么意思？
            if (vector) {
                Connection *o = nullptr;
                do {
                    o = orphaned.loadRelaxed();
                    vector->nextInOrphanList = o;
                } while (!orphaned.testAndSetRelaxed(o, ConnectionOrSignalVector::fromSignalVector(vector)));
            }
        }

        ConnectionList &connectionsForSignal(int signal) {
            return signalVector.loadRelaxed()->at(signal);
        }

        void cleanOrphanedConnections(QObject *sender, LockPolicy lockPolicy = NeedToLock) {
            if (orphaned.loadRelaxed() && ref.loadAcquire() == 1) {
                cleanOrphanedConnectionsImpl(sender, lockPolicy);
            }
        }

        void cleanOrphanedConnectionsImpl(QObject *sender, LockPolicy lockPolicy);

        static void deleteOrphaned(ConnectionOrSignalVector *c);
    };

    void ensureExtraData() {
        if (!extraData) {
            extraData = new ExtraData(this);
        }
    }
    void ensureConnectionData() {
        if (connections.loadRelaxed()) {
            return;
        }
        ConnectionData *cd = new ConnectionData;
        cd->ref.ref();
        connections.storeRelaxed(cd);
    }

    explicit QObjectPrivate(int version = QObjectPrivateVersion);
    ~QObjectPrivate() override ;

    void deleteChildren();
    void clearBindingStorage();

public:
    //添加连接
    void addConnection(int signal, Connection *c);

    static bool disconnect(Connection *c);

    static QObjectPrivate *get(QObject *o) { return o->d_func(); }
    static const QObjectPrivate *get(const QObject *o) { return o->d_func(); }

    int signalIndex(const char *signalName, const QMetaObject **meta = nullptr) const;
    bool isSignalConnected(uint signalIdx, bool checkDeclarative = true) const;
    bool maybeSignalConnected(uint signalIndex) const;
    inline bool isDeclarativeSignalConnected(uint signalIdx) const;

    void setParent_helper(QObject *o);

    static QMetaObject::Connection connectImpl(const QObject *sender, int signal_index,
                                               const QObject *receiver, void **slot,
                                               QtPrivate::QSlotObjectBase *slobObj, int type,
                                               const int *types, const QMetaObject *senderMetaObject);

public:
    using ConnectionDataPointer = QExplicitlySharedDataPointer<ConnectionData>;

    mutable ExtraData *extraData;  //用户设置的其他属性
    QAtomicPointer<QThreadData> threadData;  //QObject所属线程

    //信号槽连接数据
    QAtomicPointer<ConnectionData> connections;

    union {
        QObject *currentChildBeingDeleted;  //正在删除的子
        QAbstractDeclarativeData *declarativeData;
    };
    QAtomicPointer<QtSharedPointer::ExternalRefCountData> sharedRefCount;

};

inline bool QObjectPrivate::isDeclarativeSignalConnected(uint signalIdx) const
{
    return declarativeData && QAbstractDeclarativeData::isSignalConnected && QAbstractDeclarativeData::isSignalConnected(declarativeData, q_func(), signalIdx);
}

inline const QBindingStorage *qGetBindingStorage(const QObjectPrivate *d)
{
    return &d->bindingStorage;
}

inline QBindingStorage *qGetBindingStorage(QObjectPrivate *d)
{
    return &d->bindingStorage;
}

inline const QBindingStorage *qGetBindingStorage(const QObjectPrivate::ExtraData *ed)
{
    return &ed->parent->bindingStorage;
}

inline QBindingStorage *qGetBindingStorage(QObjectPrivate::ExtraData *ed)
{
    return &ed->parent->bindingStorage;
}

QT_END_NAMESPACE

#endif //QOBJECT_P_H
