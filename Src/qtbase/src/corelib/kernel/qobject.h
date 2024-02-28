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
#include "QtCore/qscopedpointer.h"
#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>
#include <QtCore/qproperty.h>
#include "qtmetamacros.h"
#include "qobjectdefs_impl.h"
#include "qbindingstorage.h"
#include "qobject_impl.h"

QT_BEGIN_NAMESPACE

#define REGISTER_OBJECT_INVOKE_METHOD(functionName) \
    registerInvokeMethod(#functionName, [this](){ \
        this->functionName();                     \
    });

class QThread;
class QObjectPrivate;
class QEvent;
class QTimerEvent;
class QChildEvent;
class QMetaMethod;
struct QDynamicMetaObjectData;

typedef QList<QObject *> QObjectList;

Q_CORE_EXPORT void qt_qFindChildren_helper(const QObject *parent, const QString &name, const QMetaObject &mo, QList<void *> *list, Qt::FindChildOptions);
Q_CORE_EXPORT void qt_qFindChildren_helper(const QObject *parent, const QMetaObject &mo, QList<void *> *list, Qt::FindChildOptions options);
//Q_CORE_EXPORT void qt_qFindChildren_helper(const QObject *parent, const QRegularExpression &re, const QMetaObject &mo, QList<void *> *list, Qt::FindChildOptions options);
Q_CORE_EXPORT QObject *qt_qFindChild_helper(const QObject *parent, const QString &name, const QMetaObject &mo, Qt::FindChildOptions options);

class Q_CORE_EXPORT QObjectData
{
    Q_DISABLE_COPY(QObjectData)

public:
    QObjectData() = default;
    virtual ~QObjectData() = 0;  //需要被继承重写
    QObject *q_ptr;  //q指针
    QObject *parent; //QObject的父指针
    QObjectList children; //QObject的子

    uint isWidget : 1;   //是否是widget
    uint blockSig : 1;   //阻塞信号
    uint wasDeleted : 1;  //被删除了
    uint isDeletingChildren : 1;  //正在删除子
    uint sendChildEvents : 1;  //TODO
    uint receiveChildEvents : 1;  //TODO
    uint isWindow : 1;  //QWindow
    uint deleteLaterCalled; //调用了deleteLater
    uint isQuickItem : 1;
    uint willBeWidget; //TODO
    uint wasWidget : 1;  //TODO
    uint unused: 21;
//    QAtomicInt postedEvents;  //TODO
    QDynamicMetaObjectData *metaObject; //TODO
    QBindingStorage bindingStorage;  //TODO

    QMetaObject *dynamicMetaObject() const;

    enum { CheckForParentChildLoopsWarnDepth = 4096 };
};

class Q_CORE_EXPORT QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QObject)

    Q_PROPERTY(QString objectName READ objectName WRITE setObjectName Notify objectNameChanged BINDABLE bindableObjectName)

public:
    friend struct QMetaObject;
    friend struct QMetaObjectPrivate;
    friend class QCoreApplication;
    friend class QCoreApplicationPrivate;
    friend class QThreadData;

    //TODO 需要删除的测试方法
    typedef std::function<void()> InvokeMethod;
    typedef std::vector<std::pair<std::string, InvokeMethod>> InvokeMethodMap;
    const InvokeMethodMap &getInvokeMethodMap() const { return m_invokeMethodMap; }
    InvokeMethodMap m_invokeMethodMap;
    virtual void registerInvokeMethods() {}
    void registerInvokeMethod(const std::string &key, const InvokeMethod &func)
    {
        m_invokeMethodMap.push_back({key, std::move(func)});
    }

    Q_INVOKABLE explicit QObject(QObject *parent = nullptr);
    virtual ~QObject();

    //父子关系相关
    inline QObject *parent() const { return d_ptr->parent; }
    void setParent(QObject *parent);
    inline const QObjectList &children() const { return d_ptr->children; }
    inline bool inherits(const char *classname) const
    {
        return const_cast<QObject *>(this)->qt_metacast(classname) != nullptr;
    }
    //查找child， 调用格式：auto c = obj->findChild<Base *>(); 所以需要remove_pointer
    template <typename T>
    inline T findChild(const QString &name = QString(), Qt::FindChildOptions options = Qt::FindChildrenRecursively) const
    {
        typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type ObjType;
        return static_cast<T>(qt_qFindChild_helper(this, name, ObjType::staticMetaObject, options));
    }
    template <typename T>
    inline QList<T> findChildren(const QString &name, Qt::FindChildOptions options = Qt::FindChildrenRecursively) const
    {
        typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type ObjType;
        QList<T> list;
        qt_qFindChildren_helper(this, name, ObjType::staticMetaObject, reinterpret_cast<QList<void *> *>(&list), options);
        return list;
    }
    template <typename T>
    QList<T> findChildren(Qt::FindChildOptions options = Qt::FindChildrenRecursively) const
    {
        typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type ObjType;
        QList<T> list;
        qt_qFindChildren_helper(this, ObjType::staticMetaObject, reinterpret_cast<QList<void *> *>(&list), options);
        return list;
    }
//    template<typename T>
//    inline QList<T> findChildren(const QRegularExpression &re, Qt::FindChildOptions options = Qt::FindChildrenRecursively) const
//    {
//        typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type ObjType;
//        QList<T> list;
//        qt_qFindChildren_helper(this, re, ObjType::staticMetaObject,
//                                reinterpret_cast<QList<void *> *>(&list), options);
//        return list;
//    }

    //objectName相关
    QString objectName() const;
    Q_WEAK_OVERLOAD void setObjectName(const QString &name) { doSetObjectName(name); }
    void setObjectName(QStringView name);
    QBindable<QString> bindableObjectName();

    //property相关
    bool setProperty(const char *name, const QVariant &value);
    inline bool setProperty(const char *name, QVariant &&value);
    QVariant property(const char *name) const;
    QList<QByteArray> dynamicPropertyNames() const;

    //动态绑定
    QBindingStorage *bindingStorage() { return &d_ptr->bindingStorage; }
    const QBindingStorage *bindingStorage() const { return &d_ptr->bindingStorage; }

    //UI标记
    inline bool isWidgetType() const { return d_ptr->isWidget; }
    inline bool isWindowType() const { return d_ptr->isWindow; }
    inline bool isQuickItemType() const { return d_ptr->isQuickItem; }

    //信号阻塞
    inline bool signalsBlocked() const noexcept { return d_ptr->blockSig; }
    bool blockSignals(bool b) noexcept ;

    //线程相关
    QThread *thread() const;
    void moveToThread(QThread *targetThread);

    //timer相关 TODO
//    int startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer);
//    int startTimer(std::chrono::milliseconds time, Qt::TimerType timerType = Qt::CoarseTimer);
//    void killTimer(int id);

    //event相关
    void installEventFilter(QObject *filterObj);
    void removeEventFilter(QObject *obj);
    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);

    //connect相关
    static QMetaObject::Connection connect(const QObject *sender, const char *signal, const QObject *receiver, const char *member, Qt::ConnectionType type = Qt::AutoConnection);
    static QMetaObject::Connection connect(const QObject *sender, const QMetaMethod &signal, const QObject *receiver, const QMetaMethod &method, Qt::ConnectionType type = Qt::AutoConnection);
    QMetaObject::Connection connect(const QObject *sender, const char *signal, const char *member, Qt::ConnectionType type = Qt::AutoConnection) const;

    template <typename Func1, typename Func2>
    static inline QMetaObject::Connection connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                                  const typename QtPrivate::ContextTypeForFunctor<Func2>::ContextType *context, Func2 &&slot,
                                                  Qt::ConnectionType type = Qt::AutoConnection)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<std::decay_t<Func2>> SlotType;

        if constexpr (SlotType::ArgumentCount != -1) {         //非仿函数和lambda
           static_assert(QtPrivate::AreArgumentsCompatible<typename SlotType::ReturnType, typename SignalType::ReturnType>::value);
        }
        else {
            constexpr int FunctorArgumentCount = QtPrivate::ComputeFunctorArgumentCount<std::decay_t<Func2>, typename SignalType::Arguments>::Value;
            constexpr int SlotArgumentCount = (FunctorArgumentCount >= 0) ? FunctorArgumentCount : 0;
            //参数不匹配这里会编译报错
            typedef typename QtPrivate::FunctorReturnType<std::decay_t<Func2>, typename QtPrivate::List_Left<typename SignalType::Arguments, SlotArgumentCount>::Value>::Value SlotReturnType;
            static_assert(QtPrivate::AreArgumentsCompatible<SlotReturnType, typename SignalType::ReturnType >::Value);
        }

        //信号发射者里有Q_OBJECT宏
        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value);

        static_assert(int(SignalType::ArgumentCount) >= SlotType::ArgumentCount);

        //信号的参数类型
        const int *types = nullptr;
        if (type == Qt::QueuedConnection || type == Qt::BlockingQueuedConnection) {
            types = QtPrivate::ConnectionTypes<typename SignalType::Arguments>::types();
        }

        void **pSlot = nullptr;
        //成员函数的地址放到pSlot中
        if constexpr (std::is_member_function_pointer_v<std::decay_t<Func2>>) {
            pSlot = const_cast<void **>(reinterpret_cast<void *const *>(&slot));
        }

        return connectImpl(sender, reinterpret_cast<void **>(&signal), context, pSlot,
                           QtPrivate::makeCallableObject<Func1>(std::forward<Func2>(slot)),
                           type, types, &SignalType::Object::staticMetaObject);
    }

    template <typename Func1, typename Func2>
    static inline QMetaObject::Connection connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 &&slot)
    {
        return connect(sender, signal, sender, std::forward<Func2>(slot), Qt::DirectConnection);
    }


    //disconnect相关
    static bool disconnect(const QObject *sender, const char *signal, const QObject *receiver, const char *member);
    static bool disconnect(const QObject *sender, const QMetaMethod &signal, const QObject *receiver, const QMetaMethod &member);
    bool disconnect(const char *signal, const QObject *receiver = nullptr, const char *member = nullptr) const;
    static bool disconnect(const QMetaObject::Connection &);

    template <typename Func1, typename Func2>
    static inline bool disconnect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                  const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;

        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value);
        static_assert(QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value);

        return disconnectImpl(sender, reinterpret_cast<void **>(&signal),
                              receiver, reinterpret_cast<void **>(&slot), &SignalType::Object::staticMetaObject);
    }

    template <typename Func1>
    static inline bool disconnect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                  const QObject *receiver, void **zero)
    {
        Q_ASSERT(!zero);
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        return disconnectImpl(sender, reinterpret_cast<void **>(&signal), receiver, zero, &SignalType::Object::staticMetaObject);
    }

Q_SIGNALS:
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

private:
    void doSetObjectName(const QString &name);
    bool doSetProperty(const char *name, const QVariant *lvalue, QVariant *rvalue);

    static QMetaObject::Connection connectImpl(const QObject *sender, void **signal,
                                               const QObject *receiver, void **slotPtr,
                                               QtPrivate::QSlotObjectBase *slot, Qt::ConnectionType,
                                               const int *types, const QMetaObject *senderMetaObject);
    static bool disconnectImpl(const QObject *sender, void **signal, const QObject *receiver, void **slot,
                               const QMetaObject *senderMetaObject);

protected:
    QScopedPointer<QObjectData> d_ptr;  //存放私有数据的d指针
};

inline const QBindingStorage *qGetBindingStorage(const QObject *o)
{
    return o->bindingStorage();
}

inline QBindingStorage *qGetBindingStorage(QObject *o)
{
    return o->bindingStorage();
}

QT_END_NAMESPACE

#endif //QOBJECT_H
