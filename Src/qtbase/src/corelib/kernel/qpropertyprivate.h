//
// Created by Yujie Zhao on 2023/11/21.
//

#ifndef QPROPERTYPRIVATE_H
#define QPROPERTYPRIVATE_H

#include <QtCore/qglobal.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qtaggedpointer.h>

QT_BEGIN_NAMESPACE

class QUntypedPropertyBinding;
class QPropertyBindingPrivate;
class QPropertyBindingData;
struct QPropertyBindingDataPointer;
struct QUntypedPropertyData;
struct QBindingStorage;
struct QPropertyObserverPointer;

struct QBindingObserverPtr;
using PendingBindingObserverList = QVarLengthArray<QBindingObserverPtr>;

namespace QtPrivate {
    struct BindingEvaluationState;
}

namespace QtPrivate {
    struct RefCounted {
        int ref = 0;
        void addRef() { ++ref; }
        bool deref() { --ref; return ref; }
    };
}

template <typename T>
class QPropertyData;

class QPropertyBindingPrivate;
//这个类真的是没啥意义，完全可以做成通用的模板类的，QProperty的代码确实写得有点乱
class QPropertyBindingPrivatePtr
{
public:
    using T = QtPrivate::RefCounted;
    T &operator*() const { return *d; }
    T *operator->() noexcept { return d; }
    T *operator->() const noexcept { return d; }
    explicit operator T *() { return d; }
    explicit operator const T *() const { return d; }
    T *data() const noexcept { return d; }
    T *get() const noexcept { return d; }
    const T *constData() const noexcept { return d; }
    T *take() noexcept  { T *x = d; d = nullptr; return x; }

    QPropertyBindingPrivatePtr() noexcept : d(nullptr) {}

    ~QPropertyBindingPrivatePtr() {
        if (d && (--d->ref == 0)) {
            destroyAndFreeMemory();
        }
    }

    void destroyAndFreeMemory();

    explicit QPropertyBindingPrivatePtr(T *data) noexcept
        : d(data) {
        if (d) {
            d->addRef();
        }
    }

    QPropertyBindingPrivatePtr(const QPropertyBindingPrivatePtr &o) noexcept
        : d(o.d) {
        if (d) {
            d->addRef();
        }
    }

    void reset(T *ptr = nullptr) noexcept ;

    QPropertyBindingPrivatePtr &operator=(const QPropertyBindingPrivatePtr &o) noexcept {
        reset(o.d);
        return *this;
    }

    QPropertyBindingPrivatePtr &operator=(T *o) noexcept {
        reset(o);
        return *this;
    }

    QPropertyBindingPrivatePtr(QPropertyBindingPrivatePtr &&o) noexcept
        : d(std::exchange(o.d, nullptr))
    {}
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QPropertyBindingPrivatePtr)

    operator bool() const noexcept { return d != nullptr; }
    bool operator!() const noexcept { return d == nullptr; }

    void swap(QPropertyBindingPrivatePtr &other) noexcept {
        qSwap(d, other.d);
    }

    friend bool operator==(const QPropertyBindingPrivatePtr &p1, const QPropertyBindingPrivatePtr &p2) noexcept
    { return p1.d == p2.d; }
    friend bool operator!=(const QPropertyBindingPrivatePtr &p1, const QPropertyBindingPrivatePtr &p2) noexcept
    { return p1.d != p2.d; }
    friend bool operator==(const QPropertyBindingPrivatePtr &p1, const T *ptr) noexcept
    { return p1.d == ptr; }
    friend bool operator!=(const QPropertyBindingPrivatePtr &p1, const T *ptr) noexcept
    { return p1.d != ptr; }
    friend bool operator==(const T *ptr, const QPropertyBindingPrivatePtr &p2) noexcept
    { return ptr == p2.d; }
    friend bool operator!=(const T *ptr, const QPropertyBindingPrivatePtr &p2) noexcept
    { return ptr != p2.d; }
    friend bool operator==(const QPropertyBindingPrivatePtr &p1, std::nullptr_t) noexcept
    { return !p1; }
    friend bool operator!=(const QPropertyBindingPrivatePtr &p1, std::nullptr_t) noexcept
    { return p1; }
    friend bool operator==(std::nullptr_t, const QPropertyBindingPrivatePtr &p2) noexcept
    { return !p2; }
    friend bool operator!=(std::nullptr_t, const QPropertyBindingPrivatePtr &p2) noexcept
    { return p2; }

private:
    QtPrivate::RefCounted *d;  //存放的是QPropertyBindingPrivate
};

struct QPropertyProxyBindingData
{
    quintptr d_ptr;
    const QPropertyBindingData *originalBindingData;
    QUntypedPropertyData *propertyData;
};

namespace QtPrivate {
    struct MSVCWorkAround {
    };

    //QProperty绑定的lambda表达式的封装，搞得太复杂了，直接使用一个std::function就可以搞定了
    struct BindingFunctionVTable {
        using CallFn = bool (*)(QMetaType, QUntypedPropertyData *, void *);
        using DtorFn = void (*)(void *);
        using MoveCtrFn = void (*)(void *, void *);

        const CallFn call;
        const DtorFn destroy;
        const MoveCtrFn moveConstruct;
        const qsizetype size;  //Callable的空间大小, 随着Lambda表达式捕捉的变量不同，Lambda表达式的大小也不同

        template<typename Callable, typename PropertyType=MSVCWorkAround>
        static constexpr BindingFunctionVTable createFor() {
            static_assert(alignof(Callable) <= alignof(std::max_align_t));
            return {
                    /*Call=*/ [](QMetaType metaType, QUntypedPropertyData *dataPtr, void *f) {
                        if constexpr (!std::is_invocable_v<Callable>) {  //如果不能无参调用
                            //只能接受bool Func(QMetaType, QUntypedPropertyData *) 形式的调用
                            static_assert(std::is_invocable_r_v<bool, Callable, QMetaType, QUntypedPropertyData *>);
                            auto untypedEvaluationFunction = static_cast<Callable *>(f);
                            return std::invoke(*untypedEvaluationFunction, metaType, dataPtr);
                        } else if constexpr(!std::is_same_v<PropertyType, MSVCWorkAround>) {
                            Q_UNUSED(metaType);
                            QPropertyData<PropertyType> *propertyPtr = static_cast<QPropertyData<PropertyType> *>(dataPtr);
                            auto evaluationFunction = static_cast<Callable *>(f);
                            PropertyType newValue = std::invoke(*evaluationFunction);
                            if constexpr (QTypeTraits::has_operator_equal_v<PropertyType>) { //如果有==操作符就先做==的比较
                                if (newValue == propertyPtr->valueBypassingBindings()) {
                                    return false;
                                }
                            }
                            propertyPtr->setValueByPassingBindings(std::move(newValue));  //直接设置值，通知在外面做
                            return true;
                        } else {
                            Q_UNREACHABLE_IMPL();
                            return false;
                        }
                    },
                    /*destroy*/[](void *f) {
                        //释放Callable的内存
                        static_cast<Callable *>(f)->~Callable();
                    },
                    /*moveConstruct*/[](void *addr, void *other) {
                        //在addr上执行拷贝拷贝构造函数
                        new (addr) Callable(std::move(*static_cast<Callable *>(other)));
                    },
                    /*size*/sizeof(Callable)
            };
        }
    };

    template<typename Callable, typename PropertyType=MSVCWorkAround>
    inline constexpr BindingFunctionVTable bindingFunctionVTable = BindingFunctionVTable::createFor<Callable, PropertyType>();

    struct QPropertyBindingFunction {
        const QtPrivate::BindingFunctionVTable *vtable;
        void *functor;
    };

    using QPropertyObserverCallback = void (*)(QUntypedPropertyData *);
    using QPropertyBindingWrapper = bool (*)(QMetaType, QUntypedPropertyData *dataPtr, QPropertyBindingFunction);
}

struct QPropertyBindingFunction
{
    const QtPrivate::BindingFunctionVTable *vtable;
    void *functor;
};

//QUntypedPropertyData 作为有类型的propertyData的基类
class QUntypedPropertyData
{
public:
    //用以判断一个类是否是继承自QUntypedPropertyData的标识
    struct InheritsQUntypedPropertyData
    {
    };
};

class QPropertyBindingData
{
    friend class QPropertyBindingDataPointer;

    template<typename Class, typename T, auto Offset, auto Setter, auto Signal, auto Getter>
    friend class QT_PREPEND_NAMESPACE(QObjectCompatProperty);
private:
    /*
     * 这个ptr里既可以存放QPropertyBindingPrivate又可以存放observer？
     * QProperty如果需要监听其他的QPropertyData，则d_ptr为QPropertyBindingPrivate
     * 但是有些QProperty不需要监听其他的的QProperty，反而被其他的QProperty监听，此时则不需要QPropertyBindingPrivate,直接用d_ptr存放observer
     * 感觉不如用个union，就为了节省一个Flag的内存，写得太Trick了
     * */
    mutable quintptr d_ptr = 0;  //zhaoyujie TODO 这个ptr里放的是什么东西。。。这代码看得一脸懵逼
    Q_DISABLE_COPY(QPropertyBindingData)
public:
    QPropertyBindingData() = default;
    QPropertyBindingData(QPropertyBindingData &&other);
    QPropertyBindingData &operator=(QPropertyBindingData &&other) = delete;
    ~QPropertyBindingData();

    //d_ptr指向binding获取notifier链表
    static inline constexpr quintptr BindingBit = 0x1;
    //zhaoyujie TODO
    static inline constexpr quintptr DelayedNotificationBit = 0x2;

    bool hasBinding() const { return d_ptr & BindingBit; }
    bool isNotificationDelayed() const { return d_ptr & DelayedNotificationBit; }

    QUntypedPropertyBinding setBinding(const QUntypedPropertyBinding &newBinding,
                                       QUntypedPropertyData *propertyDataPtr,
                                       QtPrivate::QPropertyObserverCallback staticObserverCallback = nullptr,
                                       QtPrivate::QPropertyBindingWrapper bindingWrapper = nullptr);

    QPropertyBindingPrivate *binding() const
    {
        quintptr dd = d();
        if (dd & BindingBit) {
            return reinterpret_cast<QPropertyBindingPrivate *>(dd - BindingBit);
        }
        return nullptr;
    }

    void evaluateIfDirty(const QUntypedPropertyData *) const;

    //移除绑定
    void removeBinding() {
        if (hasBinding()) {
            removeBinding_helper();
        }
    }

    void registerWithCurrentlyEvaluatingBinding(QtPrivate::BindingEvaluationState *currentBinding) const
    {
        if (!currentBinding) {
            return;
        }
        registerWithCurrentlyEvaluatingBinding_helper(currentBinding);
    }
    void registerWithCurrentlyEvaluatingBinding() const;

    void notifyObservers(QUntypedPropertyData *propertyDataPtr) const;
    void notifyObservers(QUntypedPropertyData *propertyDataPtr, QBindingStorage *storage) const;

private:
    quintptr &d_ref() const
    {
        quintptr &d = d_ptr;  //引用可以传递
        if (isNotificationDelayed()) {
            Q_ASSERT(false);
            return proxyData()->d_ptr;
        }
        return d;
    }

    quintptr d() const { return d_ref(); }
    QPropertyProxyBindingData *proxyData() const
    {
        Q_ASSERT(isNotificationDelayed());
        return reinterpret_cast<QPropertyProxyBindingData *>(d_ptr & ~(BindingBit | DelayedNotificationBit));
    }
    void registerWithCurrentlyEvaluatingBinding_helper(QtPrivate::BindingEvaluationState *currentBinding) const;
    void removeBinding_helper();

    enum NotificationResult { Delayed, Evaluated };
    NotificationResult notifyObserver_helper(QUntypedPropertyData *propertyDataPtr,
                                             QBindingStorage *storage,
                                             QPropertyObserverPointer observer,
                                             PendingBindingObserverList &bindingObservers) const;
};

//存放指向指针的指针
template <typename T, typename Tag>
class QTagPreservingPointerToPointer
{
public:
    constexpr QTagPreservingPointerToPointer() = default;
    QTagPreservingPointerToPointer(T **ptr)
        : d(reinterpret_cast<quintptr *>(ptr))
    {}

    QTagPreservingPointerToPointer<T, Tag> &operator=(T **ptr)
    {
        d = reinterpret_cast<quintptr *>(ptr);
        return *this;
    }

    QTagPreservingPointerToPointer<T, Tag> &operator=(QTaggedPointer<T, Tag> *ptr)
    {
        d = reinterpret_cast<quintptr *>(ptr);
        return *this;
    }

    void clear()
    {
        d = nullptr;
    }

    void setPointer(T *ptr)
    {
        *d = reinterpret_cast<quintptr >(ptr) | (*d & QTaggedPointer<T, Tag>::tagMask());
    }

    T *get() const
    {
        return reinterpret_cast<T *>(*d & QTaggedPointer<T, Tag>::pointerMask());
    }

    explicit operator bool() const
    {
        return d != nullptr;
    }

private:
    quintptr  *d = nullptr;
};

namespace QtPrivate {
    namespace detail {
        template <typename F>
        class ExtractClassFromFunctionPointer;

        //类函数指针的特化版本
        template <typename T, typename C>
        struct ExtractClassFromFunctionPointer<T C::*>
        {
            using Class = C;
        };

        constexpr size_t getOffset(size_t o) {
            return o;
        }

        constexpr size_t getOffset(size_t (*offsetFn)()) {
            return offsetFn();
        }
    }
}

QT_END_NAMESPACE

#endif //QPROPERTYPRIVATE_H
