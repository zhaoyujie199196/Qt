//
// Created by Yujie Zhao on 2023/11/1.
//

#ifndef QPROPERTY_H
#define QPROPERTY_H

#include <QtCore/qglobal.h>
#include <QtCore/qtaggedpointer.h>
#include <QtCore/qbindingstorage.h>
#include "qpropertyprivate.h"

#if __has_include(<source_location>) && __cplusplus >= 202002L && !defined(Q_CLANG_QDOC)
#include <source_location>
#if defined(__cpp_lib_source_location)
#define QT_SOURCE_LOCATION_NAMESPACE std
#define QT_PROPERTY_COLLECT_BINDING_LOCATION
#define QT_PROPERTY_DEFAULT_BINDING_LOCATION QPropertyBindingSourceLocation(std::source_location::current())
#endif
#endif

#if !defined(QT_PROPERTY_COLLECT_BINDING_LOCATION) && __has_include(<experimental/source_location>) && __cplusplus >= 201703L && !defined(Q_CLANG_QDOC)
#include <experimental/source_location>
#if defined(__cpp_lib_experimental_source_location)
#define QT_SOURCE_LOCATION_NAMESPACE std::experimental
#define QT_PROPERTY_COLLECT_BINDING_LOCATION
#define QT_PROPERTY_DEFAULT_BINDING_LOCATION QPropertyBindingSourceLocation(std::experimental::source_location::current())
#endif
#endif

#if !defined(QT_PROPERTY_COLLECT_BINDING_LOCATION)
#define QT_PROPERTY_DEFAULT_BINDING_LOCATION QPropertyBindingSourceLocation()
#endif

QT_BEGIN_NAMESPACE

template <class T> class QProperty;

//QProperty绑定的源代码的位置，给qml使用的吧
struct QPropertyBindingSourceLocation
{
    const char *fileName = nullptr;
    const char *functionName = nullptr;
    quint32 line = 0;
    quint32 column = 0;
    QPropertyBindingSourceLocation() = default;
};

//QProperty绑定错误
class QPropertyBindingErrorPrivate;
class QPropertyBindingError
{
public:
    enum Type {
        NoError,  //没有错误
        BindingLoop,  //循环绑定
        EvaluationError, //求值错误
        UnknownError,    //未知错误
    };

    QPropertyBindingError();
    QPropertyBindingError(Type type, const QString &description = QString());

    QPropertyBindingError(const QPropertyBindingError &other);
    QPropertyBindingError &operator=(const QPropertyBindingError &other);
    QPropertyBindingError(QPropertyBindingError &&other);
    QPropertyBindingError &operator=(QPropertyBindingError &&other);
    ~QPropertyBindingError();

    bool hasError() const { return d.get() != nullptr; }
    Type type() const;
    QString description() const;

private:
    //有错误时才初始化d
    QSharedDataPointer<QPropertyBindingErrorPrivate> d;
};

class QUntypedPropertyBinding
{
    friend class QPropertyBindingData;
    friend class QPropertyBindingPrivate;
    template <typename>
    friend class QPropertyBinding;
public:
    using BindingFunctionVTable = QtPrivate::BindingFunctionVTable;
    QUntypedPropertyBinding() = default;
    QUntypedPropertyBinding(QMetaType metaType, const BindingFunctionVTable *vtable, void *function, const QPropertyBindingSourceLocation &location);

    template <typename Functor>
    QUntypedPropertyBinding(QMetaType metaType, Functor &&f, const QPropertyBindingSourceLocation &location)
        : QUntypedPropertyBinding(metaType, &QtPrivate::bindingFunctionVTable<std::remove_reference_t<Functor>>, &f, location)
    {}

    explicit QUntypedPropertyBinding(QPropertyBindingPrivate *priv);
    QUntypedPropertyBinding(QUntypedPropertyBinding &&other);
    QUntypedPropertyBinding(const QUntypedPropertyBinding &other);
    QUntypedPropertyBinding &operator=(const QUntypedPropertyBinding &other);
    QUntypedPropertyBinding &operator=(QUntypedPropertyBinding &&other);
    ~QUntypedPropertyBinding();

    bool isNull() const;

    QPropertyBindingError error() const;

    QMetaType valueMetaType() const;

private:
    QPropertyBindingPrivatePtr d;
};

template <typename PropertyType>
class QPropertyBinding : public QUntypedPropertyBinding
{
public:
    QPropertyBinding() = default;

    template <typename Functor>
    QPropertyBinding(Functor &&f, const QPropertyBindingSourceLocation &location)
        : QUntypedPropertyBinding(QMetaType::fromType<PropertyType>(), &QtPrivate::bindingFunctionVTable<std::remove_reference_t<Functor>, PropertyType>, &f, location)
    {
    }

    explicit QPropertyBinding(const QUntypedPropertyBinding &binding)
        : QUntypedPropertyBinding(binding)
    {
    }
};

namespace Qt {
    //Functor如果带参数，std::enable_if_t没有类型，编译失败
    template <typename Functor>
    auto makePropertyBinding(Functor &&f, const QPropertyBindingSourceLocation &location = QT_PROPERTY_DEFAULT_BINDING_LOCATION,
                             std::enable_if_t<std::is_invocable_v<Functor>> * = nullptr)
    {
        return QPropertyBinding<std::invoke_result_t<Functor>>(std::forward<Functor>(f), location);
    }
}

class QPropertyObserver;
class QPropertyObserverBase
{
    friend struct QPropertyObserverPointer;
    friend struct QPropertyBindingDataPointer;
    friend class QPropertyObserver;
    friend class QPropertyBindingPrivate;
    friend struct QPropertyObserverNodeProtector;
public:
    enum ObserverTag {
        ObserverNotifiesBinding,  //被观察的属性变化以通知观察者
        ObserverNotifiesChangeHandler,  //observer是一个changeHandler，每次值改变的时候都会运行
        ObserverIsPlaceholder,  //TODO
        ObserverIsAlias, //TODO
    };

protected:
    using ChangeHandler = void(*)(QPropertyObserver *, QUntypedPropertyData *);

private:
    QTaggedPointer<QPropertyObserver, ObserverTag > next;
    QTagPreservingPointerToPointer<QPropertyObserver, ObserverTag> prev;

    union {
        QPropertyBindingPrivate *binding = nullptr;
        ChangeHandler changeHandler;
        QUntypedPropertyData *aliasData;
    };
};

class QPropertyObserver : public QPropertyObserverBase
{
public:
    constexpr QPropertyObserver() = default;
    QPropertyObserver(QPropertyObserver &&other) noexcept ;
    QPropertyObserver &operator=(QPropertyObserver &&other) noexcept ;
    ~QPropertyObserver();

    template <typename Property, typename = typename Property::InheritsQUntypedPropertyData>
    void setSource(const Property &property)
    {
        setSource(property.bindingData());
    }

    void setSource(const QPropertyBindingData &property);

protected:
    QPropertyObserver(ChangeHandler changeHandler);
    QPropertyObserver(QUntypedPropertyData *aliasedPropertyPtr);

    QUntypedPropertyData *aliasedProperty() const
    {
        return aliasData;
    }

private:
    QPropertyObserver(const QPropertyObserver &) = delete;
    QPropertyObserver &operator=(const QPropertyObserver &) = delete;
};

template <typename Functor>
class QPropertyChangeHandler : public QPropertyObserver
{
    Functor m_handler;
public:
    Q_NODISCARD_CTOR QPropertyChangeHandler(Functor handler)
        : QPropertyObserver([](QPropertyObserver *self, QUntypedPropertyData *) {
            auto This = static_cast<QPropertyChangeHandler<Functor>*>(self);
            This->m_handler();
        })
        , m_handler(handler)
    {
    }

    template <typename Property, typename = typename Property::InheritsQUntypedPropertyData>
    Q_NODISCARD_CTOR QPropertyChangeHandler(const Property &property, Functor handler)
        : QPropertyObserver([](QPropertyObserver *self, QUntypedPropertyData *) {
            auto This = static_cast<QPropertyChangeHandler<Functor> *>(self);
            This->m_handler();
        })
        , m_handler(handler)
    {
        setSource(property);
    }
};

class QPropertyNotifier : public QPropertyObserver
{
    std::function<void()> m_handler;
public:
    QPropertyNotifier() = default;
    template <typename Functor>
    QPropertyNotifier(Functor handler)
        : QPropertyObserver([](QPropertyObserver *self, QUntypedPropertyData *) {
            auto This = static_cast<QPropertyNotifier *>(self);
            This->m_handler();
        })
        , m_handler(handler)
    {
    }

    template <typename Functor, typename Property, typename = typename Property::InheritsQUntypedPropertyData>
    QPropertyNotifier(const Property &property, Functor handler)
        : QPropertyObserver([](QPropertyObserver *self, QUntypedPropertyData *data) {
            auto This = static_cast<QPropertyNotifier *>(self);
            This->m_handler();
        })
        , m_handler(handler)
    {
        setSource(property);
    }
};

template <typename T>
class QPropertyData : public QUntypedPropertyData
{
protected:
    mutable T val = T();  //存放的数据

private:
    class DisableRValueRefs {};
protected:
    //传参数使用饮用
    static constexpr bool UseReferences = !(std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_pointer_v<T>);
public:
    using value_type = T;
    //参数类型，如果是数值 / 枚举 / 指针直接传T，否则传const T &
    using parameter_type = std::conditional_t<UseReferences, const T &, T>;
    //右值引用类型，数值 / 枚举 / 指针类型不可以使用右值引用。
    using rvalue_ref = typename std::conditional_t<UseReferences, T &&, DisableRValueRefs>;
    //->操作结果
    using arrow_operator_result = std::conditional_t<std::is_pointer_v<T>, const T &,
                                              std::conditional_t<QTypeTraits::is_dereferenceable_v<T>, const T &, void>>;

    QPropertyData() = default;
    QPropertyData(parameter_type t) : val(t) {}
    //数值 / 枚举 / 指针的右值不会走到这个函数，会走到QPropertyData(parameter_type)函数
    //没有走到这个函数不会导致编译失败
    QPropertyData(rvalue_ref t) : val(std::move(t)) {}
    ~QPropertyData() = default;

    parameter_type valueBypassingBindings() const { return val; }
    void setValueByPassingBindings(parameter_type v) { val = v; }
    void setValueBypassingBindings(rvalue_ref v) { val = std::move(v); }
};

template <typename T>
class QProperty : public QPropertyData<T>
{
public:
    using value_type = typename QPropertyData<T>::value_type ;
    using parameter_type = typename QPropertyData<T>::parameter_type ;
    using rvalue_ref = typename QPropertyData<T>::rvalue_ref ;
    using arrow_operator_result = typename QPropertyData<T>::arrow_operator_result ;

    QProperty() = default;
    explicit QProperty(parameter_type initialValue) : QPropertyData<T>(initialValue) {}
    explicit QProperty(rvalue_ref initialValue) : QPropertyData<T>(std::move(initialValue)) {}
    explicit QProperty(const QPropertyBinding<T> &binding)
        : QProperty()
    {
        setBinding(binding);
    }
    //lambda表达式，绑定的时候会使用lambda表达式进行值的计算
    template <typename Functor>
    explicit QProperty(Functor &&f, const QPropertyBindingSourceLocation &location = QT_PROPERTY_DEFAULT_BINDING_LOCATION,
                       typename std::enable_if_t<std::is_invocable_r_v<T, Functor &>> * = nullptr)
        : QProperty(QPropertyBinding<T>(std::forward<Functor>(f), location))
    {}

    ~QProperty() = default;

    parameter_type value() const
    {
        //取数值数值的时候进行了相关绑定的注册
        d.registerWithCurrentlyEvaluatingBinding();
        return this->val;
    }

    arrow_operator_result operator->() const
    {
        //->也进行了注册
        if constexpr (QTypeTraits::is_dereferenceable_v<T>) {
            return value();
        }
        else if constexpr (std::is_pointer_v<T>) {
            value();  //zhaoyujie TODO 为什么不直接return value();
            return this->val;
        }
        else {
            return;
        }
    }

    //*返回的只是值，如果存放的是指针，要获取指针的对象，需要使用**property的调用方法
    parameter_type operator*() const
    {
        return value();
    }

    operator parameter_type() const
    {
        return value();
    }

    void setValue(rvalue_ref newValue)
    {
        d.removeBinding(); //如果直接设置数据，说明不需要绑定了，需要将绑定移除
        if (is_equal(newValue)) {
            return;
        }
        this->val = newValue;
        notify();   //通知依赖于此数值的绑定更新数据
    }

    void setValue(parameter_type newValue)
    {
        d.removeBinding();  //移除绑定，如果直接调用了setValue，那么这个QProperty不需要监听其他数据了，此时可以移除BindingData
        if (is_equal(newValue)) {
            return;
        }
        this->val = newValue;
        notify();
    }

    QProperty<T> &operator=(rvalue_ref newValue)
    {
        setValue(std::move(newValue));
        return *this;
    }

    QProperty<T> &operator=(parameter_type newValue)
    {
        setValue(newValue);
        return *this;
    }

    QPropertyBinding<T> setBinding(const QPropertyBinding<T> &newBinding)
    {
        return QPropertyBinding<T>(d.setBinding(newBinding, this));
    }

    bool setBinding(const QUntypedPropertyBinding &newBinding)
    {
        //newBinding为null也合法
        if (!newBinding.isNull() && newBinding.valueMetaType().id() != qMetaTypeId<T>()) {
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

    bool hasBinding() const { return d.hasBinding(); }

    QPropertyBinding<T> binding() const
    {
        return QPropertyBinding<T>(QUntypedPropertyBinding(d.binding()));
    }

    //takeBinding之后，将不再监听其他的Observer
    QPropertyBinding<T> takeBinding()
    {
        return QPropertyBinding<T>(d.setBinding(QUntypedPropertyBinding(), this));
    }

    const QPropertyBindingData &bindingData() const { return d; }

    template <typename Functor>
    QPropertyChangeHandler<Functor> onValueChanged(Functor f)
    {
        //f不能带有任何参数
        static_assert(std::is_invocable_v<Functor>, "Functor callback must be callable without any parameters");
        return QPropertyChangeHandler<Functor>(*this, f);
    }

    //订阅？
    template <typename Functor>
    QPropertyChangeHandler<Functor> subscribe(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>, "Functor callback must be callable without any parameters");
        f();
        return onValueChanged(f);
    }

private:
    QPropertyBindingData d;
    bool is_equal(const T &v)
    {
        //T有==操作符，使用==进行比较，没有直接返回false
        if constexpr (QTypeTraits::has_operator_equal_v<T>) {
            if (v == this->val) {
                return true;
            }
        }
        return false;
    }

    void notify()
    {
        d.notifyObservers(this);
    }

    Q_DISABLE_COPY_MOVE(QProperty)
};

namespace QtPrivate{
    namespace BindableWarnings {
        enum Reason {
            InvalidInterface,
            NonBindableInterface,
            ReadOnlyInterface
        };
    }

    void printUnsuitableBindableWarning(const QString &prefix, BindableWarnings::Reason reason);
    void printMetaTypeMismatch(QMetaType actual, QMetaType expected);

    struct QBindableInterface
    {
        using Getter = void(*)(const QUntypedPropertyData *d, void *value);
        using Setter = void(*)(QUntypedPropertyData *d, const void *value);
        using BindingGetter = QUntypedPropertyBinding(*)(const QUntypedPropertyData *d);
        using BindingSetter = QUntypedPropertyBinding(*)(QUntypedPropertyData *d, const QUntypedPropertyBinding &binding);
        using MakeBinding = QUntypedPropertyBinding(*)(const QUntypedPropertyData *d, const QPropertyBindingSourceLocation &location);
        using SetObserver = void(*)(const QUntypedPropertyData *d, QPropertyObserver *observer);
        using GetMetaType = QMetaType(*)();
        Getter getter;
        Setter setter;
        BindingGetter getBinding;
        BindingSetter setBinding;
        MakeBinding makeBinding;
        SetObserver setObserver;
        GetMetaType metaType;

        static const quintptr MetaTypeAccessorFlag = 0x1;
    };

    template <typename Property, typename = void>  //没有binding方法
    class QBindableInterfaceForProperty
    {
        using T = typename Property::value_type;
    public:
        static constexpr QBindableInterface iface = {
                [](const QUntypedPropertyData *d, void *value)->void {
                    *static_cast<T *>(value) = static_cast<const Property *>(d)->value();
                },
                nullptr,  //setter
                nullptr,  //getBinding
                nullptr,  //setBinding
                [](const QUntypedPropertyData *d, const QPropertyBindingSourceLocation &location)->QUntypedPropertyBinding {
                    return Qt::makePropertyBinding([d]()-> T { return static_cast<const Property *>(d)->value(); }, location);
                },
                [](const QUntypedPropertyData *d, QPropertyObserver *observer)->void {
                    observer->setSource(static_cast<const Property *>(d)->bindingData());
                },
                []() { return QMetaType::fromType<T>(); }
        };
    };

    //含有binding方法的const Property
    template <typename Property>
    class QBindableInterfaceForProperty<const Property, std::void_t<decltype(std::declval<Property>().binding())>>
    {
        using T = typename Property::value_type;
    public:
        static constexpr QBindableInterface iface = {
            [](const QUntypedPropertyData *d, void *value)->void {
                *static_cast<T *>(value) = static_cast<const Property *>(d)->value();
            },
            nullptr,  //setter
            [](const QUntypedPropertyData *d)->QUntypedPropertyBinding {
                return static_cast<const Property *>(d)->binding();
            },
            nullptr, //setBinding
            [](const QUntypedPropertyData *d, const QPropertyBindingSourceLocation &location)->QUntypedPropertyBinding {
                return Qt::makePropertyBinding([d]()->T {return static_cast<const Property *>(d)->value(); }, location);
            },
            [](const QUntypedPropertyData *d, QPropertyObserver *observer)->void {
                observer->setSource(static_cast<const Property *>(d)->bindingData());
            },
            []() { return QMetaType::fromType<T>(); }
        };
    };

    //含有binding方法的非const的Property
    template <typename Property>
    class QBindableInterfaceForProperty<Property, std::void_t<decltype(std::declval<Property>().binding())>>
    {
        using T = typename Property::value_type;
    public:
        static constexpr QBindableInterface iface = {
            [](const QUntypedPropertyData *d, void *value)->void {
                *static_cast<T *>(value) = static_cast<const Property *>(d)->value();
            },
            [](QUntypedPropertyData *d, const void *value) ->void {
                static_cast<Property *>(d)->setValue(*static_cast<const T*>(value));
            },
            [](const QUntypedPropertyData *d)->QUntypedPropertyBinding {
                return static_cast<const Property *>(d)->binding();
            },
            [](QUntypedPropertyData *d, const QUntypedPropertyBinding &binding)->QUntypedPropertyBinding {
                return static_cast<Property *>(d)->setBinding(static_cast<const QPropertyBinding<T> &>(binding));
            },
            [](const QUntypedPropertyData *d, const QPropertyBindingSourceLocation &location)->QUntypedPropertyBinding {
                return Qt::makePropertyBinding([d]()->T { return static_cast<const Property *>(d)->value(); }, location);
            },
            [](const QUntypedPropertyData *d, QPropertyObserver *observer)->void {
                observer->setSource(static_cast<const Property *>(d)->bindingData());
            },
            [](){ return QMetaType::fromType<T>(); }
        };
    };

    namespace PropertyAdaptorSlotObjectHelpers {
        void getter(const QUntypedPropertyData *d, void *value);
        void setter(QUntypedPropertyData *d, const void *value);
        QUntypedPropertyBinding getBinding(const QUntypedPropertyData *d);
        bool bindingWrapper(QMetaType type, QUntypedPropertyData *d,
                            QtPrivate::QPropertyBindingFunction binding,
                            QUntypedPropertyData *temp, void *value);
        QUntypedPropertyBinding setBinding(QUntypedPropertyData *d,
                                           const QUntypedPropertyBinding &binding,
                                           QPropertyBindingWrapper wrapper);
        void setObserver(const QUntypedPropertyData *d, QPropertyObserver *observer);

        template <typename T>
        bool bindingWrapper(QMetaType type, QUntypedPropertyData *d, QtPrivate::QPropertyBindingFunction binding)
        {
            struct Data : QPropertyData<T>
            {
                void *data() { return &this->val; }
            } temp;
            return bindingWrapper(type, d, binding, &temp, temp.data());
        }

        template <typename T>
        QUntypedPropertyBinding setBinding(QUntypedPropertyData *d, const QUntypedPropertyBinding &binding)
        {
            return setBinding(d, binding, &bindingWrapper<T>);
        }

        template <typename T>
        QUntypedPropertyBinding makeBinding(const QUntypedPropertyData *d,
                                            const QPropertyBindingSourceLocation &location)
        {
            return Qt::makePropertyBinding(
                    [d]()->T {
                        T r;
                        getter(d, &r);
                        return r;
                    },
                    location);
        }

        template <class T>
        inline constexpr QBindableInterface iface = {
            &getter,
            &setter,
            &getBinding,
            &setBinding<T>,
            &makeBinding<T>,
            &setObserver,
            &QMetaType::fromType<T>,
        };
    }
}

class QUntypedBindable
{
    friend struct QUntypedBindablePrivate;
protected:
    QUntypedPropertyData *data = nullptr;
    const QtPrivate::QBindableInterface *iface = nullptr;
    //QProperty
    constexpr QUntypedBindable(QUntypedPropertyData *d, const QtPrivate::QBindableInterface *f)
        : data(d), iface(f)
    {}

    //QObject的QMetaProperty
    explicit QUntypedBindable(QObject *obj, const QMetaProperty &property, const QtPrivate::QBindableInterface *i);
    explicit QUntypedBindable(QObject *obj, const char *property, const QtPrivate::QBindableInterface *i);

public:
    constexpr QUntypedBindable() = default;
    template <typename Property>
    QUntypedBindable(Property *p)
        : data(const_cast<std::remove_cv_t<Property> *>(p))
        , iface(&QtPrivate::QBindableInterfaceForProperty<Property>::iface)
    {
        //Property得是QUntypedPropertyData派生的子类
        Q_ASSERT(data && iface);
    }

    bool isValid() const { return data != nullptr; }
    bool isBindable() const { return iface && iface->getBinding; }
    bool isReadOnly() const { return !(iface && iface->setBinding && iface->setObserver); }

    QUntypedPropertyBinding makeBinding(const QPropertyBindingSourceLocation &location = QT_PROPERTY_DEFAULT_BINDING_LOCATION) const
    {
        return iface ? iface->makeBinding(data, location) : QUntypedPropertyBinding();
    }

    QUntypedPropertyBinding takeBinding()
    {
        if (!iface) {
            return QUntypedPropertyBinding {};
        }
        if (!(iface->getBinding && iface->setBinding)) {
            return QUntypedPropertyBinding{};
        }
        QUntypedPropertyBinding binding = iface->getBinding(data);
        iface->setBinding(data, QUntypedPropertyBinding{});
        return binding;
    }

    void observe(QPropertyObserver *observer) const
    {
        if (iface) {
            iface->setObserver(data, observer);
        }
        else {
            Q_ASSERT(false);
        }
    }

    template <typename Functor>
    QPropertyChangeHandler<Functor> onValueChanged(Functor f) const
    {
        QPropertyChangeHandler<Functor> handler(f);
        observe(&handler);
        return handler;
    }

    template <typename Functor>
    QPropertyChangeHandler<Functor> subscribe(Functor f) const
    {
        f();
        return onValueChanged(f);
    }

    template <typename Functor>
    QPropertyNotifier addNotifier(Functor f)
    {
        QPropertyNotifier handler(f);
        observe(&handler);
        return handler;
    }

    QUntypedPropertyBinding binding() const
    {
        if (!isBindable()) {
            return QUntypedPropertyBinding();
        }
        return iface->getBinding(data);
    }

    bool setBinding(const QUntypedPropertyBinding &binding)
    {
        if (isReadOnly()) {
            const auto errorType = iface ? QtPrivate::BindableWarnings::ReadOnlyInterface
                                         : QtPrivate::BindableWarnings::InvalidInterface;
            return false;
        }
        iface->setBinding(data, binding);
        return true;
    }

    bool hasBinding() const
    {
        return !binding().isNull();
    }

    QMetaType metaType() const
    {
        if (!(iface && data)) {
            return QMetaType();
        }
        if (iface->metaType) {
            return iface->metaType();
        }
        Q_ASSERT(iface->getter);
        QMetaType result;
        Q_ASSERT(false);
        iface->getter(data, reinterpret_cast<void *>(quintptr(&result) | QtPrivate::QBindableInterface::MetaTypeAccessorFlag));
        return result;
    }
};

template <typename T>
class QBindable : public QUntypedBindable
{
    template <typename U>
    friend class QPropertyAlias;
    constexpr QBindable(QUntypedPropertyData *d, const QtPrivate::QBindableInterface *i)
        : QUntypedBindable(d, i)
    {}

public:
    using QUntypedBindable::QUntypedBindable;
    explicit QBindable(const QUntypedBindable &b)
        : QUntypedBindable(b)
    {
        if (iface && metaType() != QMetaType::fromType<T>()) {
            //数据类型不匹配
            data = nullptr;
            iface = nullptr;
        }
    }

    explicit QBindable(QObject *obj, const QMetaProperty &property)
            : QUntypedBindable(obj, property, &QtPrivate::PropertyAdaptorSlotObjectHelpers::iface<T>)
    {}

    explicit QBindable(QObject *obj, const char *property)
            : QUntypedBindable(obj, property, &QtPrivate::PropertyAdaptorSlotObjectHelpers::iface<T>)
    {}

    QPropertyBinding<T> makeBinding(const QPropertyBindingSourceLocation &location = QT_PROPERTY_DEFAULT_BINDING_LOCATION)
    {
        return static_cast<QPropertyBinding<T> &&>(QUntypedBindable::makeBinding(location));
    }

    QPropertyBinding<T> binding() const
    {
        //static_cast<QPropertyBinding<T> &&>等同于std::move()
        //这里为什么要static_cast<QPropertyBinding> && ？
        return static_cast<QPropertyBinding<T> &&>(QUntypedBindable::binding());
    }

    QPropertyBinding<T> takeBinding()
    {
        return static_cast<QPropertyBinding<T> &&>(QUntypedBindable::takeBinding());
    }

    using QUntypedBindable::setBinding;
    QPropertyBinding<T> setBinding(const QPropertyBinding<T> &binding)
    {
        Q_ASSERT(!iface || binding.isNull() || binding.valueMetaType() == metaType());
        if (iface && iface->setBinding) {
            return static_cast<QPropertyBinding<T> &&>(iface->setBinding(data, binding));
        }
        Q_ASSERT(false);
        return QPropertyBinding<T>();
    }

    template <typename Functor>
    QPropertyBinding<T> setBinding(Functor &&f,
                                   const QPropertyBindingSourceLocation &location = QT_PROPERTY_DEFAULT_BINDING_LOCATION,
                                   std::enable_if_t<std::is_invocable_v<Functor>> * = nullptr)
    {
        return setBinding(Qt::makePropertyBinding(std::forward<Functor>(f), location));
    }

    T value() const
    {
        if (iface) {
            T result;
            iface->getter(data, &result);
            return result;
        }
        return {};
    }

    void setValue(const T &value)
    {
        if (iface && iface->setter) {
            iface->setter(data, &value);
        }
    }

};

namespace Qt {
    template <typename PropertyType>
    QPropertyBinding<PropertyType> makePropertyBinding(const QProperty<PropertyType> &otherProperty,
                                                       const QPropertyBindingSourceLocation &location = QT_PROPERTY_DEFAULT_BINDING_LOCATION)
    {
        return Qt::makePropertyBinding([&otherProperty]()->PropertyType {
            return otherProperty;
        }, location);
    }
}

//别名属性，监听原属性
template <typename T>
class QPropertyAlias : public QPropertyObserver
{
    Q_DISABLE_COPY_MOVE(QPropertyAlias)
    const QtPrivate::QBindableInterface *iface = nullptr;

public:
    QT_WARNING_PUSH QT_WARNING_DISABLE_DEPRECATED
    QPropertyAlias(QProperty<T> *property)
        : QPropertyObserver(property)
        , iface(&QtPrivate::QBindableInterfaceForProperty<QProperty<T>>::iface)
    {
        if (iface) {
            iface->setObserver(aliasedProperty(), this);
        }
    }
    //Property是从QUntypedPropertyData继承的，里面有InheritsQUntypedPropertyData类
    template <typename Property, typename = typename Property::InheritsQUntypedPropertyData>
    QPropertyAlias(Property *property)
        : QPropertyObserver(property)
        , iface(&QtPrivate::QBindableInterfaceForProperty<Property>::iface)
    {
        if (iface) {
            iface->setObserver(aliasedProperty(), this);
        }
    }

    //QPropertyAlias是另一个QPropertyAlias的别名
    QPropertyAlias(QPropertyAlias<T> *alias)
        : QPropertyObserver(alias->aliasedProperty())
        , iface(alias->iface)
    {
        if (iface) {
            iface->setObserver(aliasedProperty(), this);
        }
    }

    QPropertyAlias(const QBindable<T> &property)
        : QPropertyObserver(property.data)
        , iface(property.iface)
    {
        Q_ASSERT(false);
        if (iface) {
            iface->setObserver(aliasedProperty(), this);
        }
    }

    T value() const
    {
        T t = T();
        if (auto *p = aliasedProperty()) {
            iface->getter(p, &t);
        }
        return t;
    }

    operator T() const { return value(); }

    void setValue(const T &newValue)
    {
        if (auto *p = aliasedProperty()) {
            iface->setter(p, &newValue);
        }
    }

    QPropertyAlias<T> &operator=(const T &newValue) {
        setValue(newValue);
        return *this;
    }

    QPropertyBinding<T> setBinding(const QPropertyBinding<T> &newBinding)
    {
        return QBindable<T>(aliasedProperty(), iface).setBinding(newBinding);
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
        return QBindiable<T>(aliasedProperty(), iface).hasBinding();
    }

    QPropertyBinding<T> binding() const
    {
        return QBindable<T>(aliasedProperty(), iface).binding();
    }

    QPropertyBinding<T> takeBinding()
    {
        return QBindable<T>(aliasedProperty(), iface).takeBinding();
    }

    template <typename Functor>
    QPropertyChangeHandler<Functor> onValueChanged(Functor f)
    {
        return QBindiable<T>(aliasedProperty(), iface).onValueChanged(f);
    }

    template <typename Functor>
    QPropertyChangeHandler<Functor> subscribe(Functor f)
    {
        return QBindable<T>(aliasedProperty(), iface).subscribe(f);
    }

    template <typename Functor>
    QPropertyNotifier addNotifier(Functor f)
    {
        return QBindable<T>(aliasedProperty(), iface).addNotifier(f);
    }

    bool isValid() const
    {
        return aliasedProperty() != nullptr;
    }

    QT_WARNING_POP
};

template <typename Class, typename T, auto Offset, auto Signal = nullptr>
class QObjectBindableProperty : public QPropertyData<T>
{
    using ThisType = QObjectBindableProperty<Class, T, Offset, Signal>;
    //含有信号
    static bool constexpr HasSignal = !std::is_same_v<decltype(Signal), std::nullptr_t>;
    //zhaoyujie TODO
    using SignalTakesValue = std::is_invocable<decltype(Signal), Class, T>;

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

    static void signalCallBack(QUntypedPropertyData *o)
    {
        QObjectBindableProperty *that = static_cast<QObjectBindableProperty *>(o);
        if constexpr (HasSignal) {
            if constexpr (SignalTakesValue::value) {
                (that->owner()->*Signal)(that->valueBypassingBindings());
            }
            else {
                (that->owner()->*Signal)();
            }
        }
    }

public:
    using value_type = typename QPropertyData<T>::value_type;
    using parameter_type = typename QPropertyData<T>::parameter_type ;
    using rvalue_ref = typename QPropertyData<T>::rvalue_ref ;
    using arrow_operator_result  = typename QPropertyData<T>::arrow_operator_result ;

    QObjectBindableProperty() = default;
    explicit QObjectBindableProperty(const T &initialValue) : QPropertyData<T>(initialValue) {}
    explicit QObjectBindableProperty(T &&initialValue) : QPropertyData<T>(std::move(initialValue)) {}
    explicit QObjectBindableProperty(const QPropertyBinding<T> &binding)
        : QObjectBindableProperty()
    {
        setBinding(binding);
    }
    template <typename Functor>
    explicit QObjectBindableProperty(Functor &&f, const QPropertyBindingSourceLocation &location = QT_PROPERTY_DEFAULT_BINDING_LOCATION,
                                     typename std::enable_if_t<std::is_invocable_r_v<T, Functor &>> * = nullptr)
        : QObjectBindableProperty(QPropertyBinding<T>(std::forward<Functor>(f), location))
    {
    }

    parameter_type value() const
    {
        qGetBindingStorage(owner())->registerDependency(this);
        return this->val;
    }

    arrow_operator_result operator->() const
    {
        //解引用操作符号->
        if constexpr(QTypeTraits::is_dereferenceable_v<T>) {
            return value();
        }
        else if constexpr(std::is_pointer_v<T>) {
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

    void setValue(parameter_type t)
    {
        auto *bd = qGetBindingStorage(owner())->bindingData(this);
        if (bd) {
            bd->removeBinding();
        }
        if (this->val == t) {
            return;
        }
        this->val = t;
        notify(bd);
    }

    operator parameter_type() const
    {
        return value();
    }

    void notify() {
        auto *bd = qGetBindingStorage(owner())->bindingData(this);
        notify(bd);
    }

    void setValue(rvalue_ref t)
    {
        auto *bd = qGetBindingStorage(owner())->bindingData(this);
        if (bd) {
            bd->removeBinding();
        }
        if (this->val == t) {
            return;
        }
        this->val = std::move(t);
        notify(bd);
    }

    QObjectBindableProperty &operator=(rvalue_ref newValue)
    {
        setValue(std::move(newValue));
        return *this;
    }

    QObjectBindableProperty &operator=(parameter_type newValue)
    {
        setValue(newValue);
        return *this;
    }

    QPropertyBinding<T> setBinding(const QPropertyBinding<T> &newBinding)
    {
        QPropertyBindingData *bd = qGetBindingStorage(owner())->bindingData(this, true);
        QUntypedPropertyBinding oldBinding(bd->setBinding(newBinding, this, HasSignal ? &signalCallBack : nullptr));
        return static_cast<QPropertyBinding<T> &>(oldBinding);
    }

    bool setBinding(const QUntypedPropertyBinding &newBinding)
    {
        if (!newBinding.isNull() && newBinding.valueMetaType().id() != qMetaTypeId<T>()) {
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
        return bd ? bd->binding() : nullptr;
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
        static_assert(std::is_invocable_v<Functor>, "Functor callback must be callable without any parameters");
        return QPropertyChangeHandler<Functor>(*this, f);
    }

    template <typename Functor>
    QPropertyChangeHandler<Functor> subscribe(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>, "Functor callback must be callable without any parameters");
        f();
        return onValueChanged(f);
    }

    template <typename Functor>
    QPropertyNotifier addNotifier(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>, "Functor callback must be callable without any parameters");
        return QPropertyNotifier(*this, f);
    }

    const QPropertyBindingData &bindingData() const
    {
        auto *storage = const_cast<QBindingStorage *>(qGetBindingStorage(owner()));
        return *storage->bindingData(const_cast<ThisType *>(this), true);
    }

private:
    void notify(const QPropertyBindingData *binding)
    {
        if (binding) {
            binding->notifyObservers(this, qGetBindingStorage(owner()));
        }
        if constexpr (HasSignal) {
            if constexpr(SignalTakesValue::value) {
                (owner()->*Signal)(this->valueBypassingBindings());
            }
            else {
                (owner()->*Signal)();
            }
        }
    }
};

#define QT_OBJECT_BINDABLE_PROPERTY_3(Class, Type, name) \
    static constexpr size_t _qt_property_##name##_offset() { \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
        return offsetof(Class, name); \
        QT_WARNING_POP \
    } \
    QObjectBindableProperty<Class, Type, Class::_qt_property_##name##_offset, nullptr> name;

#define QT_OBJECT_BINDABLE_PROPERTY_4(Class, Type, name, Signal) \
    static constexpr size_t _qt_property_##name##_offset() {     \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF      \
        return offsetof(Class, name);                            \
        QT_WARNING_POP                                           \
    }                                                            \
    QObjectBindableProperty<Class, Type, Class::_qt_property_##name##_offset, Signal> name;

#define Q_OBJECT_BINDABLE_PROPERTY(...) \
    QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
    QT_OVERLOADED_MACRO(QT_OBJECT_BINDABLE_PROPERTY, __VA_ARGS__) \
    QT_WARNING_POP

#define QT_OBJECT_BINDABLE_PROPERTY_WITH_ARGS_4(Class, Type, name, value) \
    static constexpr size_t _qt_property_##name##_offset()                 \
    {                                                                     \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF                                        \
        return offsetof(Class, name);                                                              \
        QT_WARNING_POP                  \
    }                                                                     \
    QObjectBindableProperty<Class, Type, Class::_qt_property_##name##_offset, nullptr> name =      \
            QObjectBindableProperty<Class, Type, Class::_qt_property_##name##_offset, nullptr>(value);

#define QT_OBJECT_BINDABLE_PROPERTY_WITH_ARGS_5(Class, Type, name, value, Signal)                  \
    static constexpr size_t _qt_property_##name##_offset()                                         \
    {                                                                                              \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF                                        \
        return offsetof(Class, name);                                                              \
        QT_WARNING_POP                                                                             \
    }                                                                                              \
    QObjectBindableProperty<Class, Type, Class::_qt_property_##name##_offset, Signal> name =       \
            QObjectBindableProperty<Class, Type, Class::_qt_property_##name##_offset, Signal>(value);

#define Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(...)                                                  \
    QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
    QT_OVERLOADED_MACRO(QT_OBJECT_BINDABLE_PROPERTY_WITH_ARGS, __VA_ARGS__) \
    QT_WARNING_POP

template<typename Class, typename T, auto Offset, auto Getter>
class QObjectComputedProperty : public QUntypedPropertyData
{
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

public:
    using value_type = T;
    using parameter_type = T;

    QObjectComputedProperty() = default;

    parameter_type value() const
    {
        qGetBindingStorage(owner())->registerDependency(this);
        return (owner()->*Getter)();
    }

    std::conditional_t<QTypeTraits::is_dereferenceable_v<T>, parameter_type, void>
    operator->() const
    {
        if constexpr (QTypeTraits::is_dereferenceable_v<T>)
            return value();
        else
            return;
    }

    parameter_type operator*() const
    {
        return value();
    }

    operator parameter_type() const
    {
        return value();
    }

    constexpr bool hasBinding() const { return false; }

    template<typename Functor>
    QPropertyChangeHandler<Functor> onValueChanged(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>, "Functor callback must be callable without any parameters");
        return QPropertyChangeHandler<Functor>(*this, f);
    }

    template<typename Functor>
    QPropertyChangeHandler<Functor> subscribe(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>, "Functor callback must be callable without any parameters");
        f();
        return onValueChanged(f);
    }

    template<typename Functor>
    QPropertyNotifier addNotifier(Functor f)
    {
        static_assert(std::is_invocable_v<Functor>, "Functor callback must be callable without any parameters");
        return QPropertyNotifier(*this, f);
    }

    QPropertyBindingData &bindingData() const
    {
        auto *storage = const_cast<QBindingStorage *>(qGetBindingStorage(owner()));
        return *storage->bindingData(const_cast<QObjectComputedProperty *>(this), true);
    }

    void notify() {
        // computed property can't store a binding, so there's nothing to mark
        auto *storage = const_cast<QBindingStorage *>(qGetBindingStorage(owner()));
        auto bd = storage->bindingData(const_cast<QObjectComputedProperty *>(this), false);
        if (bd)
            bd->notifyObservers(this, qGetBindingStorage(owner()));
    }
};

#define Q_OBJECT_COMPUTED_PROPERTY(Class, Type, name,  ...) \
    static constexpr size_t _qt_property_##name##_offset() { \
        QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF \
        return offsetof(Class, name); \
        QT_WARNING_POP \
    } \
    QObjectComputedProperty<Class, Type, Class::_qt_property_##name##_offset, __VA_ARGS__> name;

QT_END_NAMESPACE

#endif //QPROPERTY_H
