//
// Created by Yujie Zhao on 2024/1/28.
//

#ifndef QOBJECT_P_P_H
#define QOBJECT_P_P_H

#include <QtCore/qobject.h>
#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

//头尾原子指针链表
struct QObjectPrivate::ConnectionList
{
    QAtomicPointer<Connection> first;
    QAtomicPointer<Connection> last;
};

//quintptr最低位0表示Connection，最低位1表示SignalVector
struct QObjectPrivate::TaggedSignalVector
{
    quintptr c;

    TaggedSignalVector() = default;

    TaggedSignalVector(std::nullptr_t) noexcept
        : c(0)
    {}

    TaggedSignalVector(Connection *v) noexcept
        : c(reinterpret_cast<quintptr>(v))
    {
        Q_ASSERT(v && (reinterpret_cast<quintptr>(v) & 0x1) == 0);
    }

    TaggedSignalVector(SignalVector *v) noexcept
        : c(reinterpret_cast<quintptr >(v) | quintptr(1u))
    {
        Q_ASSERT(v);
    }

    explicit operator SignalVector *() const noexcept
    {
        if (c & 0x1) {
            return reinterpret_cast<SignalVector *>(c & ~quintptr(1u));
        }
        return nullptr;
    }

    explicit operator Connection *() const noexcept
    {
        return reinterpret_cast<Connection *>(c);
    }

    operator uintptr_t() const noexcept { return c; }
};

struct QObjectPrivate::ConnectionOrSignalVector
{
    // Connection不是在有效的链表里，就是在需要清理的orphaned链表里。
    // RemoveConnection会将Connection从有效链表中去除，并且放到orphaned链表里
    union {
        //需要清理的orphaned链接
        TaggedSignalVector nextInOrphanList;
        //链接到object的槽的链表
        Connection *next;
    };
};
static_assert(std::is_trivial_v<QObjectPrivate::ConnectionOrSignalVector>);

struct QObjectPrivate::Connection : public ConnectionOrSignalVector
{
    //链接到Slot的Connection链条链接
    Connection **prev;
    //信号发送者的链条链接
    QAtomicPointer<Connection> nextConnectionList;
    Connection *prevConnectionList;

    QObject *sender;
    QAtomicPointer<QObject> receiver;
    QAtomicPointer<QThreadData> receiverThreadData;

    union {
        StaticMetaCallFunction callFunction;  //TODO
        QtPrivate::QSlotObjectBase *slotObj;
    };
    QAtomicPointer<const int> argumentTypes;
    QAtomicInt ref_ {
        2  //Connection被发送者和接受者一起持有
    };
    uint id = 0;
    ushort method_offset;
    ushort method_relative;
    signed int signal_index: 27;  //信号在sender中的索引
    ushort connectionType : 2;  //连接方式 0: Auto, 1: direct, 2: queued, 3: blocking
    ushort isSlotObject: 1;  //槽函数使用的是否是函数指针形式
    ushort ownArgumentTypes: 1;  //是否拥有argumentTypes的所有权。如果拥有所有权，需要在析构函数中释放argumentTypes指针
    ushort isSignalShot: 1;  //

    Connection() : ownArgumentTypes(true) {}
    ~Connection();
    int method() const
    {
        Q_ASSERT(!isSlotObject);
        return method_offset + method_relative;
    }
    void ref() { ref_.ref(); }
    void freeSlotObject()
    {
        if (isSlotObject) {
            slotObj->destroyIfLastRef();
            isSlotObject = false;
        }
    }
    void deref()
    {
        if (!ref_.deref()) {  //得确保delete this之前，先设置相关成员
            Q_ASSERT(!receiver.loadRelaxed());
            Q_ASSERT(!isSlotObject);
            delete this;
        }
    }
};
Q_DECLARE_TYPEINFO(QObjectPrivate::Connection, Q_RELOCATABLE_TYPE);

//TODO  内存结构没有捋清楚
struct QObjectPrivate::SignalVector : public ConnectionOrSignalVector
{
    quintptr allocated;
    ConnectionList &at(int i)
    {
        return reinterpret_cast<ConnectionList *>(this + 1)[i + 1];
    }

    int count()
    {
        return static_cast<int>(allocated);
    }
};
static_assert(std::is_trivial_v<QObjectPrivate::SignalVector>);

struct QObjectPrivate::ConnectionData
{
    QAtomicInteger<uint> currentConnectionId;
    QAtomicInt ref;
    QAtomicPointer<SignalVector> signalVector;
    Connection *senders = nullptr; //此Object作为receiver的Connection的连接
    Sender *currentSender = nullptr;  //激活这个object方法的信号的object
    std::atomic<TaggedSignalVector> orphaned = {};  //orphaned可以存放SignalVector也可以存放单个Connection

    ~ConnectionData()
    {
        Q_ASSERT(ref.loadRelaxed() == 0);
        TaggedSignalVector c = orphaned.exchange(nullptr, std::memory_order_relaxed);
        if (c) {
            deleteOrphaned(c);
        }
        SignalVector *v = signalVector.loadRelaxed();
        if (v) {
            v->~SignalVector();
            free(v);
        }
    }

    void removeConnection(Connection *c);
    enum LockPolicy {
        NeedToLock,
        AlreadyLockedAndTemporarilyReleasingLock
    };

    void cleanOrphanedConnections(QObject *sender, LockPolicy lockPolicy = NeedToLock);
    void cleanOrphanedConnectionsImpl(QObject *sender, LockPolicy lockPolicy);

    //获取信号上的连接链表
    ConnectionList &connectionsForSignal(int signal)
    {
        return signalVector.loadRelaxed()->at(signal);
    }
    //resize
    void resizeSignalVector(uint size)
    {
        SignalVector *vector = this->signalVector.loadRelaxed();
        if (vector && vector->allocated > size) {
            return;
        }
        size = (size + 7) & ~7; //size确保是8的倍数
        //分配SignalVector的内存以及 (size + 1) 个ConnectionList的大小的内存
        void *ptr = malloc(sizeof(SignalVector) + (size + 1) * sizeof(ConnectionList));
        //原地调用构造函数
        auto newVector = new(ptr) SignalVector;
        //复制源值
        if (vector) {
            memcpy(newVector, vector, sizeof(SignalVector) + (vector->allocated + 1) * sizeof(ConnectionList));
        }
        int start = vector->count();
        for (int i = 0; i < int(size); ++i) {
            new (&newVector->at(i)) ConnectionList();
        }
        newVector->next = nullptr;
        newVector->allocated = size;

        signalVector.storeRelaxed(newVector);
        //老的vector是需要清理的对象了
        if (vector) {
            TaggedSignalVector o = nullptr;
            //将原来需要清理的orphaned与新加入的vector相连
            o = orphaned.load(std::memory_order_acquire);
            do {
                vector->nextInOrphanList = o;
            } while (!orphaned.compare_exchange_strong(o, TaggedSignalVector(vector), std::memory_order_release));
        }
    }

    int signalVectorCount() const
    {
        return signalVector.loadAcquire() ? signalVector.loadRelaxed()->count() : -1;
    }

    static void deleteOrphaned(TaggedSignalVector o);
};

struct QObjectPrivate::Sender
{
    Sender(QObject *receiver, QObject *sender, int signal)
        : receiver(receiver)
        , sender(sender)
        , signal(signal)
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
        Q_ASSERT(false);
//        Sender *s = this;
//        while (s) {
//            s->receiver = nullptr;
//            s = s->previous;
//        }
    }

    Sender *previous;
    QObject *receiver;
    QObject *sender;
    int signal;
};

Q_DECLARE_TYPEINFO(QObjectPrivate::Sender, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

#endif //QOBJECT_P_P_H
