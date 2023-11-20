//
// Created by Yujie Zhao on 2023/2/24.
//

#ifndef QOBJECT_H
#define QOBJECT_H

#include <QtCore/qglobal.h>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>
#include <QtCore/qproperty.h>
#include "qtmetamacros.h"
#include "qobjectdefs_impl.h"

QT_BEGIN_NAMESPACE

#define REGISTER_OBJECT_INVOKE_METHOD(functionName) \
    registerInvokeMethod(#functionName, [this](){ \
        this->functionName();                     \
    });

class QEvent;
class QTimerEvent;
class QChildEvent;
class QObjectPrivate;
class QObject;
class QThread;

struct QDynamicMetaObjectData;

typedef QList<QObject *> QObjectList;

class Q_CORE_EXPORT QObjectData
{
    Q_DISABLE_COPY(QObjectData)
public:
    QObjectData() = default;
    virtual ~QObjectData() {}
    QObject *q_ptr;  //q指针，objectData的owner指针
    QObject *parent; //父亲指针
    QObjectList children;  //子对象列表

    uint isWidget : 1;  //是否是widget
    uint blockSig : 1;   //阻塞信号
    uint wasDeleted: 1;  //是否已经被删除
    uint isDeletingChildren : 1;  //正在删除子
    uint sendChildEvents: 1;
    uint receiveChildEvents: 1;
    uint isWindow: 1;   //for QWindow
    uint deleteLaterCalled : 1;  //调用了deleteLater
    uint unused: 24;
    int postedEvents;
    QDynamicMetaObjectData *metaObject;
    QBindingStorage bindingStorage;  //zhaoyujie TODO
    QMetaObject *dynamicMetaObject() const;

};

class QObject {
    Q_OBJECT
    //声明private指针
    Q_DECLARE_PRIVATE(QObject)
    Q_PROPERTY(QString objectName READ objectName WRITE setObjectName)
public:
    friend struct QMetaObject;
    friend struct QMetaObjectPrivate;

    typedef std::function<void()> InvokeMethod;
    typedef std::vector<std::pair<std::string, InvokeMethod>> InvokeMethodMap;

    Q_INVOKABLE explicit QObject(QObject *parent = nullptr);
    virtual ~QObject();


    QString objectName() const;
    void setObjectName(const QString &name);
//    QBindable<QString> QObject::bindableObjectName();

    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);

    //翻译
    static QString tr(const char *sourceText, const char * = nullptr, int = -1) {
        //zhaoyujie TODO windows怎么弄的
#if defined(Q_OS_WIN)
        Q_ASSERT(false);
#endif
        return QString::fromUtf8(sourceText);
    }

    //property系统
    //通过name设置属性
    bool setProperty(const char *name, const QVariant &value);
    QVariant property(const char *name) const;

    //connection系统
    //使用字符串标记信号槽，字符串对应到moc系统
    static QMetaObject::Connection connect(const QObject *sender, const char *signal,
                                           const QObject *receiver, const char *member,
                                           Qt::ConnectionType = Qt::AutoConnection);
    static QMetaObject::Connection connect(const QObject *sender, const QMetaMethod &signal,
                                           const QObject *receiver, const QMetaMethod &method,
                                           Qt::ConnectionType type = Qt::AutoConnection);
    QMetaObject::Connection connect(const QObject *sender, const char *signal,
                                    const char *member, Qt::ConnectionType type = Qt::AutoConnection) const;

    //FunctionPointer接受函数指针，适配connect(obj, &Obj::sig1, obj, &Obj::func)之类的写法
    template <typename Func1, typename Func2>
    static inline QMetaObject::Connection connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                                  const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                                                  Qt::ConnectionType type = Qt::AutoConnection) {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;
        //需要有Q_Object宏
        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value);
        //参数个数要匹配
        static_assert(int(SignalType::ArgumentCount) >= int(SlotType::ArgumentCount));
        //参数类型要匹配
        static_assert(QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value);
        //返回值要匹配
        static_assert(QtPrivate::AreArgumentsCompatible<typename SlotType::ReturnType, typename SignalType::ReturnType>::value);

        const int *types = nullptr;
        if (type == Qt::QueuedConnection || type == Qt::BlockingQueuedConnection) {
            Q_ASSERT(false);
//            types = QtPrivate::ConnectionTypes<typename SignalType::Arguments>::types();
        }
        //生成slotObject，QSlotObject适配obj::func1形式
        auto slotObject = new QtPrivate::QSlotObject<Func2,
                                                    typename QtPrivate::List_Left<typename SignalType::Arguments, SlotType::ArgumentCount>::Value,
                                                    typename SignalType::ReturnType>(slot);
        return connectImpl(sender, reinterpret_cast<void **>(&signal),
                           receiver, reinterpret_cast<void **>(&slot),
                           slotObject,
                           type, types, &SignalType::Object::staticMetaObject);
    }


    template <typename Func1, typename Func2>
    static inline typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) >= 0, QMetaObject::Connection>::type
            connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
    {
        //connect(sender, &signal, &slot)形式，slot为全局函数，receiver设置为sender为了控制生命周期
        return connect(sender, signal, sender, slot, Qt::DirectConnection);
    }

    //非类成员的函数指针
    template <typename Func1, typename Func2>
    static inline typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) >= 0 &&
                                          !QtPrivate::FunctionPointer<Func2>::IsPointerToMemberFunction, QMetaObject::Connection>::type
             connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, const QObject *context, Func2 slot, Qt::ConnectionType type = Qt::AutoConnection)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;

        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value, "No Q_OBJECT in the class with the signal");

        static_assert(int(SignalType::ArgumentCount) >= int(SlotType::ArgumentCount), "The slot requires more arguments than the signal provides.");

        static_assert((QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value),
                "Signal and slot arguments are not compatible.");
        static_assert((QtPrivate::AreArgumentsCompatible<typename SignalType::ReturnType, typename SlotType::Arguments>::value),
                "Return type of the slot is not compatible with the return type of the signal");

        const int *types = nullptr;
        if (type == Qt::QueuedConnection || type == Qt::BlockingQueuedConnection) {
            Q_ASSERT(false);
            return QMetaObject::Connection();
        }
        auto slotObj = new QtPrivate::QStaticSlotObject<Func2,
                                                        typename QtPrivate::List_Left<typename SignalType::Arguments, SlotType::ArgumentCount>::Value,
                                                        typename SignalType::ReturnType>(slot);
        return connectImpl(sender, reinterpret_cast<void **>(&signal), context, nullptr, slotObj, type, types, &SignalType::Object::staticMetaObject);
    }

    //ArgumentCount == -1 为仿函数或者lambda表达式，会走到下面的含有context的函数
    template <typename Func1, typename Func2>
    static inline typename std::enable_if<QtPrivate::FunctionPointer<Func2>::ArgumentCount == -1, QMetaObject::Connection>::type
            connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
    {
        return connect(sender, signal, sender, std::move(slot), Qt::DirectConnection);
    }

    //仿函数或者lambda表达式，context用来控制生命周期
    template <typename Func1, typename Func2>
    static inline typename std::enable_if<QtPrivate::FunctionPointer<Func2>::ArgumentCount == -1, QMetaObject::Connection>::type
            connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, const QObject *context, Func2 slot, Qt::ConnectionType type = Qt::AutoConnection)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        //计算函子的参数个数
        const int FunctorArgumentCount = QtPrivate::ComputeFunctorArgumentCount<Func2, typename SignalType::Arguments>::Value;
        static_assert(FunctorArgumentCount >= 0, "Signal and slot arguments are not compatible.");

        const int SlotArgumentCount = (FunctorArgumentCount >= 0) ? FunctorArgumentCount : 0;
        //仿函数的返回类型
        typedef typename QtPrivate::FunctorReturnType<Func2, typename QtPrivate::List_Left<typename SignalType::Arguments, SlotArgumentCount>::Value>::Value SlotReturnType;

        static_assert(QtPrivate::AreArgumentsCompatible<SlotReturnType, typename SignalType::ReturnType >::value, "Return type of the slot is not compatible with the return type of the signal.");

        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value, "No Q_OBJECT in the class with the signal");

        const int *types = nullptr;
        if (type == Qt::QueuedConnection || type == Qt::BlockingQueuedConnection) {
            Q_ASSERT(false);
            return QMetaObject::Connection();
        }
        auto slotObj = new QtPrivate::QFunctorSlotObject<Func2, SlotArgumentCount,
                                                         typename QtPrivate::List_Left<typename SignalType::Arguments, SlotArgumentCount>::Value,
                                                         typename SignalType::ReturnType>(std::move(slot));
        return connectImpl(sender, reinterpret_cast<void **>(&signal), context, nullptr, slotObj, type, types, &SignalType::Object::staticMetaObject);
    }

    static bool disconnect(const QObject *sender, const char *signal,
                           const QObject *receiver, const char *member);
    static bool disconnect(const QObject *sender, const QMetaMethod &signal,
                           const QObject *receiver, const QMetaMethod &method);
    inline bool disconnect(const char *signal = nullptr,
                           const QObject *receiver = nullptr, const char *member = nullptr) const
    {
        return disconnect(this, signal, receiver, member);
    }
    inline bool disconnect(QObject *receiver, const char *member = nullptr) const
    {
        return disconnect(this, nullptr, receiver, member);
    }
    static bool disconnect(const QMetaObject::Connection &);

    template <typename Func1, typename Func2>
    static inline bool disconnect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                             const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;
        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value, "No Q_OBJECT in the class with the signal");
        static_assert(QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value, "Signal and slot arguments are not compatible.");

        return disconnectImpl(sender, reinterpret_cast<void **>(&signal), receiver, reinterpret_cast<void **>(&slot), &SignalType::Object::staticMetaObject);
    }

    template <typename Func1>
    static inline bool disconnect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                  const QObject *receiver, void **zero) {
        Q_ASSERT(!zero);
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        return disconnectImpl(sender, reinterpret_cast<void **>(&signal), receiver, zero, &SignalType::Object::staticMetaObject);
    }

    inline QObject *parent() const { return d_ptr->parent; }
    inline bool inherits(const char *classname) const {
        Q_ASSERT(false);  //zhaoyujie TODO 这里的逻辑是什么意思？不能用来判断父？只能做精准判断？
        return const_cast<QObject *>(this)->qt_metacast(classname) != nullptr;
    }


    //QObject所属的线程
    QThread *thread() const;

    //在实现Q_INVOKABLE之前先使用这种硬编码的方式
    virtual void registerInvokeMethods() {}

    const InvokeMethodMap &getInvokeMethodMap() const {return m_invokeMethodMap;}

signals:
    void destroyed(QObject * = nullptr);
    void objectNameChanged(const QString &objectName, QPrivateSignal);

public Q_SLOTS:
    void deleteLater();

protected:
    QObject(QObjectPrivate &dd, QObject *parent = nullptr);

    virtual void timerEvent(QTimerEvent *event);
    virtual void childEvent(QChildEvent *event);
    virtual void customEvent(QEvent *event);

    virtual void connectNotify(const QMetaMethod &signal);
    virtual void disconnectNotify(const QMetaMethod &signal);
    QObject *sender() const;
    int senderSignalIndex() const;
    int receivers(const char *signal) const;
    bool isSignalConnected(const QMetaMethod &signal) const;

    void registerInvokeMethod(const std::string &key, const InvokeMethod &func);

private:
    static QMetaObject::Connection connectImpl(const QObject *sender, void **signal,
                                               const QObject *receiver, void **slotPtr,
                                               QtPrivate::QSlotObjectBase *slot,
                                               Qt::ConnectionType type,
                                               const int *types, const QMetaObject *senderMetaObject);
    static bool disconnectImpl(const QObject *sender, void **signal,
                               const QObject *receiver, void **slot,
                               const QMetaObject *senderMetaObject);

protected:
    std::unique_ptr<QObjectData> d_ptr;

    InvokeMethodMap m_invokeMethodMap;
};

QT_END_NAMESPACE

#endif //QOBJECT_H
