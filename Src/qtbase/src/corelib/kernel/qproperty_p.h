//
// Created by Yujie Zhao on 2023/11/21.
//

#ifndef QPROPERTY_P_H
#define QPROPERTY_P_H

#include <QtCore/qglobal.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qproperty.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qscopedvaluerollback.h>
#include <QtCore/qmetaobject.h>
#include <vector>
#include "qobjectdefs_impl.h"
#include "qbindingstorage.h"

QT_BEGIN_NAMESPACE

namespace QtPrivate {
    bool isAnyBindingEvaluating();
    struct QBindingStatusAccessToken
    {
    };
}

struct QBindingObserverPtr
{
private:
    QPropertyObserver *d = nullptr;

public:
    QBindingObserverPtr() = default;

    Q_DISABLE_COPY(QBindingObserverPtr);
    void swap(QBindingObserverPtr &other) noexcept {
        qSwap(d, other.d);
    }
    QBindingObserverPtr(QBindingObserverPtr &&other) noexcept
        : d(qExchange(other.d, nullptr))
    {
    }

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QBindingObserverPtr);

    inline QBindingObserverPtr(QPropertyObserver *observer) noexcept;
    inline ~QBindingObserverPtr();
    inline QPropertyBindingPrivate *binding() const noexcept;
    inline QPropertyObserver *operator->() {
        return d;
    }
};

using PendingBindingObserverList = QVarLengthArray<QBindingObserverPtr>;

namespace QtPrivate {
    struct BindingEvaluationState
    {
        BindingEvaluationState(QPropertyBindingPrivate *binding, QBindingStatus *status);
        ~BindingEvaluationState();
        QPropertyBindingPrivate *binding;
        BindingEvaluationState *previousState = nullptr;
        BindingEvaluationState **currentState = nullptr;
        QVarLengthArray<const QPropertyBindingData *, 8> alreadyCaptureProperties;
    };

    struct CompatPropertySafePoint
    {
        CompatPropertySafePoint(QBindingStatus *status, QUntypedPropertyData *property);
        ~CompatPropertySafePoint()
        {
            *currentState = previousState;
            *currentlyEvaluatingBindingList = bindingState;
        }
        QUntypedPropertyData *property;
        CompatPropertySafePoint *previousState = nullptr;
        CompatPropertySafePoint **currentState = nullptr;
        QtPrivate::BindingEvaluationState **currentlyEvaluatingBindingList = nullptr;
        QtPrivate::BindingEvaluationState *bindingState = nullptr;
    };

    bool isPropertyInBindingWrapper(const QUntypedPropertyData *property);
    void initBindingStatusThreadId();
}

class QPropertyBindingErrorPrivate : public QSharedData
{
public:
    QPropertyBindingError::Type type = QPropertyBindingError::NoError;
    QString description;
};

//相对于QPropertyObserver拓展了一些功能
struct QPropertyObserverPointer
{
    QPropertyObserver *ptr = nullptr;  //zhaoyujie TODO ptr什么时候释放的

    void unlink()
    {
        unlink_common();
        if (ptr->next.tag() == QPropertyObserver::ObserverIsAlias) {
            Q_ASSERT(false);
            ptr->aliasData = nullptr;
        }
    }

    void unlink_fast()
    {
        Q_ASSERT(ptr->next.tag() != QPropertyObserver::ObserverIsAlias);
        unlink_common();
    }

    void setBindingToNotify(QPropertyBindingPrivate *binding);
    void setBindingToNotify_unsafe(QPropertyBindingPrivate *binding);
    inline void setChangeHandler(QPropertyObserver::ChangeHandler changeHandler);

    enum class Notify { Everything, OnlyChangeHandlers };
    inline void notify(QUntypedPropertyData *propertyDataPtr);

    void noSelfDependencies(QPropertyBindingPrivate *binding);
    void evaluateBindings(PendingBindingObserverList &bindingObservers, QBindingStatus *status);
    void observerProperty(QPropertyBindingDataPointer property);

    explicit operator bool() const { return ptr != nullptr; }

    QPropertyObserverPointer nextObserver() const { return { ptr->next.data() }; }

    QPropertyBindingPrivate *binding() const
    {
        Q_ASSERT(ptr->next.tag() == QPropertyObserver::ObserverNotifiesBinding);
        return ptr->binding;
    }

private:
    void unlink_common()  //将ptr从链表中移除
    {
        if (ptr->next) {
            ptr->next->prev = ptr->prev;
        }
        if (ptr->prev) {
            ptr->prev.setPointer(ptr->next.data());
        }
        ptr->next = nullptr;
        ptr->prev.clear();
    }
};

//BindingData的真实数据
class QPropertyBindingPrivate : public QtPrivate::RefCounted
{
    friend struct QPropertyBindingDataPointer;
    friend class QPropertyBindingPrivatePtr;
    using ObserverArray = std::array<QPropertyObserver, 4>;

private:
    bool updating = false;   //用来检测循环绑定
    bool hasStaticObserver = false;
    bool pendingNotify = false;
    bool hasBindingWrapper : 1;  //TODO 这个用来干嘛的？
    bool isQQmlPropertyBinding : 1;
    bool m_sticky: 1;  //使binding具有粘性，有粘性的binding，在QProperty调用setValue时，不会将binding移除

    //用在C++中？
    const QtPrivate::BindingFunctionVTable *vtable;  //绑定方法

    union {
        QtPrivate::QPropertyObserverCallback staticObserverCallback = nullptr;
        QtPrivate::QPropertyBindingWrapper staticBindingWrapper;
    };
    //inlineDependencyObservers和headObservers是属于此BindingData的观察着，即用作此观察者观察其他对象的观察者
    ObserverArray inlineDependencyObservers;  //干嘛要搞这个？直接都放在堆上不好吗？
    QPropertyObserverPointer firstObserver;   //观察此bindingData的观察者
    QScopedPointer<std::vector<QPropertyObserver>> heapObservers;  //放在堆上的观察者

protected:
    QUntypedPropertyData *propertyDataPtr = nullptr;

    using DeclaretiveErrorCallback = void(*)(QPropertyBindingPrivate *);
    union {
        //location存储c++中代码，QML的存储在其他地方
        QPropertyBindingSourceLocation location;
    };

private:
    QPropertyBindingError error;
    QMetaType metaType;

public:
    static constexpr size_t getSizeEnsuringAlignment() {
        constexpr auto align = alignof(std::max_align_t) - 1;
        constexpr size_t sizeEnsuringAlignment = (sizeof(QPropertyBindingPrivate) + align) & ~align;
        static_assert(sizeEnsuringAlignment % alignof(std::max_align_t) == 0);
        return sizeEnsuringAlignment;
    }

    //目前监视的对象的数量
    size_t dependencyObserverCount = 0;

    bool isUpdating() const { return updating; }
    void setSticky(bool keep = true) { m_sticky = keep; }
    bool isSticky() const { return m_sticky; }
    void scheduleNotify() { pendingNotify = true; }

    QPropertyBindingPrivate(QMetaType metaType, const QtPrivate::BindingFunctionVTable *vtable,
                            const QPropertyBindingSourceLocation &location, bool isQQmlPropertyBinding = false)
        : hasBindingWrapper(false)
        , isQQmlPropertyBinding(isQQmlPropertyBinding)
        , m_sticky(false)
        , vtable(vtable)
        , location(location)
        , metaType(metaType)
    {
    }

    ~QPropertyBindingPrivate();

    void setProperty(QUntypedPropertyData *propertyStr) { propertyDataPtr = propertyStr; }
    void setStaticObserver(QtPrivate::QPropertyObserverCallback callback, QtPrivate::QPropertyBindingWrapper bindingWrapper) {
        Q_ASSERT(!(callback && bindingWrapper));
        if (callback) {
            Q_ASSERT(false);
            hasStaticObserver = true;
            hasBindingWrapper = false;
            staticObserverCallback = callback;
        }
        else if (bindingWrapper) {
            hasStaticObserver = false;
            hasBindingWrapper = true;
            staticBindingWrapper = bindingWrapper;
        }
        else {
            hasStaticObserver = false;
            hasBindingWrapper = false;
            staticObserverCallback = nullptr;
            staticBindingWrapper = nullptr;
        }
    }

    void prependObserver(QPropertyObserverPointer observer) {
        Q_ASSERT(!firstObserver);
        observer.ptr->prev = const_cast<QPropertyObserver **>(&firstObserver.ptr);
        firstObserver = observer;
    }

    QPropertyObserverPointer takeObservers()
    {
        auto observers = firstObserver;
        firstObserver.ptr = nullptr;
        return observers;
    }

    void clearDependencyObservers();

    //分配监视的观察对象
    QPropertyObserverPointer allocateDependencyObserver() {
        //固定的数组  不太明白这个事情，必然有固定的四个观察对象？
        if (dependencyObserverCount < inlineDependencyObservers.size()) {
            ++dependencyObserverCount;
            return { &inlineDependencyObservers[dependencyObserverCount - 1] };
        }
        //从堆上分配
        return allocateDependencyObserver_slow();
    }

    QPropertyObserverPointer allocateDependencyObserver_slow();

    QPropertyBindingSourceLocation sourceLocation() const {
        if (!hasCustomVTable()) {  //从QML中来的可以精准定位？
            return this->location;
        }
        QPropertyBindingSourceLocation location;  //C++中的所以无法精准定位？
        constexpr auto msg = "Custom location";
        location.fileName = msg;
        return location;
    }

    QPropertyBindingError bindingError() const { return error; }
    QMetaType valueMetaType() const { return metaType; }

    void unlinkAndDeref();

    //递归计算。计算的过程中，如果值变化了，会接着计算观察此QProperty的其他Property
    bool evaluateRecursive(PendingBindingObserverList &bindingObservers, QBindingStatus *status = nullptr);
    bool evaluateRecursive_inline(PendingBindingObserverList &bindingObservers, QBindingStatus *status);

    void notifyNonRecursive(const PendingBindingObserverList &bindingObservers);
    enum NotificationState : bool { Delayed, Sent };
    NotificationState notifyNonRecursive();

    static QPropertyBindingPrivate *get(const QUntypedPropertyBinding &binding) {
        return static_cast<QPropertyBindingPrivate *>(binding.d.data());
    }

    void setError(QPropertyBindingError &&e) {
        error = std::move(e);
    }

    void detachFromProperty() {  //与Property分离
        hasStaticObserver = false;
        hasBindingWrapper = false;
        propertyDataPtr = nullptr;  //存放的propertyData为空
        clearDependencyObservers();
    }

    static QPropertyBindingPrivate *currentlyEvaluatingBinding();


    bool hasCustomVTable() const {
        return vtable->size == 0;
    }

    static void destroyAndFreeMemory(QPropertyBindingPrivate *priv) {
        if (priv->hasCustomVTable()) {
            Q_ASSERT(false);
            priv->vtable->destroy(priv);
        }
        else {
            //调用QPropertyBindingPrivate的析构函数
            //分配内存时，分配了QPropertyBindingPrivate和Functor的内存
            //functor的析构函数在～QPropertyBindingPrivate中调用
            priv->~QPropertyBindingPrivate();
            delete[] reinterpret_cast<std::byte *>(priv);
        }
    }
};

//相较于QPropertyBindingData，增加了一些方法
struct QPropertyBindingDataPointer
{
    const QPropertyBindingData *ptr = nullptr;

    QPropertyBindingPrivate *binding() const
    {
        return ptr->binding();
    }

    void setObservers(QPropertyObserver *observer)
    {
        auto &d = ptr->d_ref();
        observer->prev = reinterpret_cast<QPropertyObserver **>(&d);
        d = reinterpret_cast<quintptr >(observer);
    }

    static void fixupAfterMove(QPropertyBindingData *ptr);
    //其他Property观察此QProperty的观察者
    void addObserver(QPropertyObserver *observer);
    inline void setFirstObserver(QPropertyObserver *observer);
    inline QPropertyObserverPointer firstObserver() const;
    static QPropertyProxyBindingData *proxyData(QPropertyBindingData *ptr);

    inline int observerCount() const;

    template <typename T>
    static QPropertyBindingDataPointer get(QProperty<T> &property) {
        return QPropertyBindingDataPointer{ &property.bindingData() };
    }
};

inline QPropertyObserverPointer QPropertyBindingDataPointer::firstObserver() const
{
    if (auto *b = binding()) {
        return b->firstObserver;
    }
    return {reinterpret_cast<QPropertyObserver *>(ptr->d()) };
}

inline int QPropertyBindingDataPointer::observerCount() const
{
    int count = 0;
    for (auto observer = firstObserver(); observer; observer = observer.nextObserver()) {
        ++count;
    }
    return count;
}

struct QPropertyObserverNodeProtector
{
    Q_DISABLE_COPY_MOVE(QPropertyObserverNodeProtector)

    QPropertyObserverBase m_placeHolder;
    //在observer和next中间插入了一个placeholder？
    QPropertyObserverNodeProtector(QPropertyObserver *observer)
    {
        QPropertyObserver *next = observer->next.data();
        m_placeHolder.next = next;
        observer->next = static_cast<QPropertyObserver *>(&m_placeHolder);
        if (next) {
            next->prev = &m_placeHolder.next;
        }
        m_placeHolder.prev = &observer->next;
        m_placeHolder.next.setTag(QPropertyObserver::ObserverIsPlaceholder);
    }

    QPropertyObserver *next() const { return m_placeHolder.next.data(); }

    ~QPropertyObserverNodeProtector()
    {
        QPropertyObserverPointer d{ static_cast<QPropertyObserver *>(&m_placeHolder) };
        d.unlink_fast();
    }
};

inline QBindingObserverPtr::QBindingObserverPtr(QPropertyObserver *observer) noexcept
    : d(observer)
{
    Q_ASSERT(d);
    QPropertyObserverPointer{d}.binding()->addRef();
}

inline QBindingObserverPtr::~QBindingObserverPtr()
{
    if (d) {
        QPropertyObserverPointer{d}.binding()->deref();
    }
}

inline QPropertyBindingPrivate *QBindingObserverPtr::binding() const noexcept
{
    return QPropertyObserverPointer{d}.binding();
}

inline void QPropertyObserverPointer::notify(QUntypedPropertyData *propertyDataPtr)
{
    auto observer = const_cast<QPropertyObserver *>(ptr);
    while (observer) {
        QPropertyObserver *next = observer->next.data();
        switch (QPropertyObserver::ObserverTag(observer->next.tag())) {
            case QPropertyObserver::ObserverNotifiesChangeHandler: {
                //值更变化了，通知onValueChanged设置的观察者
                auto handlerToCall = observer->changeHandler;
                if (next && next->next.tag() == QPropertyObserver::ObserverIsPlaceholder) {
                    observer = next->next.data();
                    continue;
                }
                QPropertyObserverNodeProtector protector(observer);
                handlerToCall(observer, propertyDataPtr);
                next = protector.next();
                break;
            }
            case QPropertyObserver::ObserverNotifiesBinding: {
                break;
            }
            case QPropertyObserver::ObserverIsPlaceholder: {
                break;
            }
            case QPropertyObserver::ObserverIsAlias: {
                break;
            }
            default:
                Q_ASSERT(false);
        }
        observer = next;
    }
}

inline void QPropertyObserverPointer::setChangeHandler(QPropertyObserver::ChangeHandler changeHandler)
{
    Q_ASSERT(ptr->next.tag() != QPropertyObserver::ObserverIsPlaceholder);
    ptr->changeHandler = changeHandler;
    ptr->next.setTag(QPropertyObserver::ObserverNotifiesChangeHandler);
}

namespace QtPrivate {
    class QPropertyAdaptorSlotObject : public QUntypedPropertyData, public QSlotObjectBase {
        QPropertyBindingData bindingData_;
        QObject *obj;
        QMetaProperty metaProperty_;

        static void impl(int which, QSlotObjectBase *this_, QObject *r, void **a, bool *ret);

        QPropertyAdaptorSlotObject(QObject *o, const QMetaProperty &p);

    public:
        static QPropertyAdaptorSlotObject *cast(QSlotObjectBase *ptr, int propertyIndex) {
            if (ptr->isImpl(&QPropertyAdaptorSlotObject::impl)) {
                auto p = static_cast<QPropertyAdaptorSlotObject *>(ptr);
                if (p->metaProperty_.propertyIndex() == propertyIndex) {
                    return p;
                }
            }
            return nullptr;
        }

        inline const QPropertyBindingData &bindingData() const { return bindingData_; }

        inline QPropertyBindingData &bindingData() { return bindingData_; }

        inline QObject *object() const { return obj; }

        inline const QMetaProperty &metaProperty() const { return metaProperty_; }

        friend class QT_PREPEND_NAMESPACE(QUntypedBindable);
    };
}

inline void QPropertyBindingDataPointer::fixupAfterMove(QPropertyBindingData *ptr)
{
    Q_ASSERT(false);
    auto &d = ptr->d_ref();
    if (ptr->isNotificationDelayed()) {
        QPropertyProxyBindingData *proxy = ptr->proxyData();
        Q_ASSERT(proxy);
        proxy->originalBindingData = ptr;
    }
    if (d & QPropertyBindingData::BindingBit) {
        return;
    }
    if (auto observer = reinterpret_cast<QPropertyObserver *>(d)) {
        observer->prev = reinterpret_cast<QPropertyObserver **>(&d);
    }
}

template <typename Class, typename T, auto Offset, auto Setter, auto Signal = nullptr, auto Getter = nullptr>
class QObjectCompatProperty: public QPropertyData<T>
{
    template <typename Property, typename>
    friend class QtPrivate::QBindableInterfaceForProperty;

    using ThisType = QObjectCompatProperty<Class, T, Offset, Setter, Signal, Getter>;
    using SignalTakesValue = std::is_invocable<decltype(Signal), Class, T>;
    //获取QObject，通过地址偏移计算
    Class *owner()
    {
        char *that = reinterpret_cast<char *>(this);
        return reinterpret_cast<Class *>(that - QtPrivate::detail::getOffset(Offset));
    }
    const Class *owner() const
    {
        char *that = const_cast<char *>(reinterpret_cast<const char *>(this));
        return reinterpret_cast<Class *>(that - QtPrivate::detail::getOffset(Offset));
    }

    static bool bindingWrapper(QMetaType type, QUntypedPropertyData *dataPtr, QtPrivate::QPropertyBindingFunction binding)
    {
        auto *thisData = static_cast<ThisType *>(dataPtr);
        QPropertyData<T> copy;
        binding.vtable->call(type, &copy, binding.functor);
        if constexpr (QTypeTraits::has_operator_equal_v<T>) {
            if (copy.valueBypassingBindings() == thisData->valueBypassingBindings()) {
                return false;
            }
        }
        QBindingStorage *storage = qGetBindingStorage(thisData->owner());
        QtPrivate::CompatPropertySafePoint guardThis(storage->bindingStatus, thisData);
        (thisData->owner()->*Setter)(copy.valueBypassingBindings());
        return true;
    }

    bool inBindingWrapper(const QBindingStorage *storage) const
    {
        return storage->bindingStatus &&
               storage->bindingStatus->currentCompatProperty &&
               QtPrivate::isPropertyInBindingWrapper(this);
    }

    inline static T getPropertyValue(const QUntypedPropertyData *d)
    {
        auto prop = static_cast<const ThisType *>(d);
        if constexpr (std::is_null_pointer_v<decltype(Getter)>) {
            return prop->value();
        }
        else {
            return (prop->owner()->*Getter)();
        }
    }

public:
    using value_type = typename QPropertyData<T>::value_type;
    using parameter_type = typename QPropertyData<T>::parameter_type;
    using arrow_operator_result = typename QPropertyData<T>::arrow_operator_result;

    QObjectCompatProperty() = default;
    explicit QObjectCompatProperty(const T &initialValue) : QPropertyData<T>(initialValue) {}
    explicit QObjectCompatProperty(T &&initialValue) : QPropertyData<T>(std::move(initialValue)) {}

    parameter_type value() const
    {
        const QBindingStorage *storage = qGetBindingStorage(owner());
        if (storage->bindingStatus && storage->bindingStatus->currentlyEvaluatingBinding && !inBindingWrapper(storage)) {
            storage->registerDependency_helper(this);
        }
        return this->val;
    }

    arrow_operator_result operator->() const
    {
        if constexpr (QTypeTraits::is_dereferenceable_v<T>) {
            return value();
        }
        else if constexpr (std::is_pointer_v<T>) {
            value();
            return this->val;
        }
        else {
            return;
        }
    }

    parameter_type operator*() const
    {
        return value();
    }

    operator parameter_type() const
    {
        return value();
    }

    void setValue(parameter_type t)
    {
        QBindingStorage *storage = qGetBindingStorage(owner());
        if (auto *bd = storage->bindingData(this)) {
            if (bd->hasBinding() && !inBindingWrapper(storage)) {
                bd->removeBinding_helper();
            }
        }
        this->val = t;
    }

    QObjectCompatProperty &operator=(parameter_type newValue)
    {
        setValue(newValue);
        return *this;
    }

    QPropertyBinding<T> setBinding(const QPropertyBinding<T> &newBinding)
    {
        QPropertyBindingData *bd = qGetBindingStorage(owner())->bindingData(this, true);
        QUntypedPropertyBinding oldBinding(bd->setBinding(newBinding, this, nullptr, bindingWrapper));
        return static_cast<QPropertyBinding<T> &>(oldBinding);
    }

    bool setBinding(const QUntypedPropertyBinding &newBinding)
    {
        if (!newBinding.isNull() && newBinding.valueMetaType() != QMetaType::fromType<T>()) {
            return false;
        }
        setBinding(static_cast<const QPropertyBinding<T> &>(newBinding));
        return true;
    }

    template <typename Functor>
    QPropertyBinding<T> setBinding(Functor &&f,
                                   const QPropertyBindingSourceLocation &location = QT_PROPERTY_DEFAULT_BINDING_LOCATION,
                                   std::enable_if_t<std::is_invocable_v<Functor>> * = nullptr)
    {
        return setBinding(Qt::makePropertyBinding(std::forward<Functor>(f), location));
    }

    bool hasBinding() const
    {
        auto *bd = qGetBindingStorage(owner())->bindingData(this);
        return bd && bd->binding() != nullptr;
    }

    void removeBindingUnlessInWrapper()
    {
        QBindingStorage *storage = qGetBindingStorage(owner());
        if (auto *bd = storage->bindingData(this)) {
            if (bd->hasBinding() && !inBindingWrapper(storage)) {
                bd->removeBinding_helper();
            }
        }
    }

    void notify()
    {
        QBindingStorage *storage = qGetBindingStorage(owner());
        if (auto bd = storage->bindingData(this, false)) {
            if (!bd->isNotificationDelayed()) {
                QPropertyBindingDataPointer d{bd};
                if (QPropertyObserverPointer observer = d.firstObserver()) {
                    if (!inBindingWrapper(storage)) {
                        PendingBindingObserverList bindingObservers;
                        if (bd->notifyObserver_helper(this, storage, observer, bindingObservers) == QPropertyBindingData::Evaluated) {
                            if (QPropertyObserverPointer observer = d.firstObserver()) {
                                observer.notify(this);
                            }
                            for (auto && bindingObserver : bindingObservers) {
                                bindingObserver.binding()->notifyNonRecursive();
                            }
                        }
                    }
                }
            }
        }
        if constexpr (!std::is_null_pointer_v<decltype(Signal)>) {
            if constexpr (SignalTakesValue::value) {
                (owner()->*Signal)(getPropertyValue(this));
            }
            else {
                (owner()->*Signal)();
            }
        }
    }

    QPropertyBinding<T> binding() const
    {
        auto *bd = qGetBindingStorage(owner())->bindingData(this);
        return static_cast<QPropertyBinding<T> &&>(QUntypedPropertyBinding(bd ? bd->binding() : nullptr));
    }

    QPropertyBinding<T> takeBinding()
    {
        return setBinding(QPropertyBinding<T>());
    }

    template <typename Functor>
    QPropertyChangeHandler<Functor> onValueChanged(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>);
        return QPropertyChangeHandler<Functor>(*this, f);
    }

    template <typename Functor>
    QPropertyChangeHandler<Functor> subscribe(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>);
        f();
        return onValueChanged(f);
    }

    template <typename Functor>
    QPropertyNotifier addNotifier(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>);
        return QPropertyNotifier(*this, f);
    }

    QPropertyBindingData &bindingData() const
    {
        auto *storage = const_cast<QBindingStorage *>(qGetBindingStorage(owner()));
        return *storage->bindingData(const_cast<QObjectCompatProperty *>(this), true);
    }
};

namespace QtPrivate {
    template <typename Class, typename Ty, auto Offset, auto Setter, auto Signal, auto Getter>
    class QBindableInterfaceForProperty<QObjectCompatProperty<Class, Ty, Offset, Setter, Signal, Getter>, std::void_t<Class>>
    {
        using Property = QObjectCompatProperty<Class, Ty, Offset, Setter, Signal, Getter>;
        using T = typename Property::value_type;
    public:
        static constexpr QBindableInterface iface = {
            [](const QUntypedPropertyData *d, void *value) -> void {
                *static_cast<T *>(value) = Property::getPropertyValue(d);
            },
            [](QUntypedPropertyData *d, const void *value)->void {
                (static_cast<Property *>(d)->owner()->*Setter)(*static_cast<const T *>(value));
            },
            [](const QUntypedPropertyData *d)->QUntypedPropertyBinding {
                return static_cast<const Property *>(d)->binding();
            },
            [](QUntypedPropertyData *d, const QUntypedPropertyBinding &binding)->QUntypedPropertyBinding {
                return static_cast<Property *>(d)->setBinding(static_cast<const QPropertyBinding<T> &>(binding));
            },
            [](const QUntypedPropertyData *d, const QPropertyBindingSourceLocation &location)->QUntypedPropertyBinding {
                return Qt::makePropertyBinding([d]()->T { return Property::getPropertyValue(d); }, location);
            },
            [](const QUntypedPropertyData *d, QPropertyObserver *observer)->void {
                observer->setSource(static_cast<const Property *>(d)->bindingData());
            },
            [](){ return QMetaType::fromType<T>(); }
        };
    };
}


#define QT_OBJECT_COMPAT_PROPERTY_4(Class, Type, name, setter) \
    static constexpr size_t _qt_property_##name##_offset() {   \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
        return offsetof(Class, name);                          \
        QT_WARNING_POP \
    }                                                          \
    QObjectCompatProperty<Class, Type, Class::_qt_property_##name##_offset, setter> name;

#define QT_OBJECT_COMPAT_PROPERTY_5(Class, Type, name, setter, signal) \
    static constexpr size_t _qt_property_##name##_offset() {           \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
        return offsetof(Class, name);                                  \
        QT_WARNING_POP                                                               \
    }                                                                  \
    QObjectCompatProperty<Class, Type, Class::_qt_property_##name##_offset, setter, signal> name;

#define Q_OBJECT_COMPAT_PROPERTY(...) \
    QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
    QT_OVERLOADED_MACRO(QT_OBJECT_COMPAT_PROPERTY, __VA_ARGS__) \
    QT_WARNING_POP

#define QT_OBJECT_COMPAT_PROPERTY_WITH_ARGS_5(Class, Type, name, setter, value) \
    static constexpr size_t _qt_property_##name##_offset() {                     \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF                     \
        return offsetof(Class, name);                                           \
        QT_WARNING_POP                                                                            \
    }                                                                           \
    QObjectCompatProperty<Class, Type, Class::_qt_property_##name##_offset, setter> name =        \
        QObjectCompatProperty<Class, Type, Class::_qt_property_##name##_offset, setter>(value);

#define QT_OBJECT_COMPAT_PROPERTY_WITH_ARGS_6(Class, Type, name, setter, signal, value) \
    static constexpr size_t _qt_property_##name##_offset() {                            \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
        return offsetof(Class, name);                                               \
        QT_WARNING_POP    \
    }                                                                                   \
    QObjectCompatProperty<Class, Type, Class::_qt_property_##name##_offset, setter, signal> name = \
        QObjectCompatProperty<Class, Type, Class::_qt_property_##name##_offset, setter, signal>(value);

#define QT_OBJECT_COMPAT_PROPERTY_WITH_ARGS_7(Class, Type, name, setter, signal, getter, value) \
    static constexpr size_t _qt_property_##name##_offset() { \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
        return offsetof(Class, name); \
        QT_WARNING_POP \
    } \
    QObjectCompatProperty<Class, Type, Class::_qt_property_##name##_offset, setter, signal, getter>\
        name = QObjectCompatProperty<Class, Type, Class::_qt_property_##name##_offset, setter, signal, getter>(value);

#define Q_OBJECT_COMPAT_PROPERTY_WITH_ARGS(...) \
    QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
    QT_OVERLOADED_MACRO(QT_OBJECT_COMPAT_PROPERTY_WITH_ARGS, __VA_ARGS__) \
    QT_WARNING_POP

QT_END_NAMESPACE

#endif //QPROPERTY_P_H
