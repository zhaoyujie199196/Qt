//
// Created by Yujie Zhao on 2023/8/9.
//

#ifndef QSHAREDPOINTER_H
#define QSHAREDPOINTER_H

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qobject.h>
#include <QtCore/qhashfunctions.h>
#include <memory>
#include <new>

QT_BEGIN_NAMESPACE

template <class T> class QWeakPointer;
template <class T> class QSharedPointer;
template <class T> class QEnableSharedFromThis;

template <class X, class T>
QSharedPointer<X> qSharedPointerCast(const QSharedPointer<T> &ptr);
template <class X, class T>
QSharedPointer<X> qSharedPointerDynamicCast(const QSharedPointer<T> &ptr);
template <class X, class T>
QSharedPointer<X> qSharedPointerConstCast(const QSharedPointer<T> &ptr);


namespace QtSharedPointer {

#define QT_SHAREDPOINTER_TYPEDEF(T) \
        typedef T element_type; \
        typedef T value_type; \
        typedef value_type *pointer; \
        typedef const value_type *const_pointer; \
        typedef value_type &reference; \
        typedef const value_type &const_reference; \
        typedef qptrdiff difference_type;

    template <class T>
    class ExternalRefCount;

    template <class X, class Y>
    QSharedPointer<X> copyAndSetPointer(X *ptr, const QSharedPointer<Y> &src);

//    void internalSafetyCheckAdd(const void *, const volatile void *);
//    void internalSafetyCheckRemove(const void *);

    template <class T, typename Klass, typename RetVal>
    inline void executeDeleter(T *t, RetVal (Klass::*memberDeleter)) {
        if (t) {
            (t->*memberDeleter)();
        }
    }
    template <class T, typename Deleter>
    inline void executeDeleter(T *t, Deleter d) {
        d(t);
    }
    struct NormalDeleter {};

    struct ExternalRefCountData {
        typedef void (*DestroyerFn)(ExternalRefCountData *);
        QBasicAtomicInt weakref;    //shared指针与weak指针
        QBasicAtomicInt strongref;  //shared指针
        DestroyerFn destroyer;

        inline ExternalRefCountData(DestroyerFn d)
            : destroyer(d) {
            strongref.storeRelaxed(1);  //zhaoyujie TODO 为什么strong和weak都是1
            weakref.storeRelaxed(1);
        }
        inline ExternalRefCountData(Qt::Initialization)
        {}
        ~ExternalRefCountData() {
            Q_ASSERT(!weakref.loadRelaxed());
            Q_ASSERT(strongref.loadRelaxed() <= 0);
        }

        void destroy() {
            destroyer(this);
        }

        //zhaoyujie TODO
        static ExternalRefCountData *getAndRef(const QObject *);
        void setQObjectShared(const QObject *, bool enable);
        void checkQObjectShared(const QObject *);

        inline void checkQObjectShared(...) {}
        inline void setQObjectShared(...) {}

        //禁止使用全局默认的new操作符来分配内存
        inline void *operator new(std::size_t) = delete;
        //定义了一个new操作符的重载版本
        inline void *operator new(std::size_t, void *ptr) noexcept { return ptr; }
        //重载的delete函数，调用全局的delete
        inline void operator delete(void *ptr) { ::operator delete(ptr); }
        inline void operator delete(void *, void *) {
            Q_ASSERT(false);
            //zhaoyujie TODO  这个函数什么时候会被调用
        }
    };

    template <class T, typename Deleter>
    struct CustomDeleter
    {
        Deleter deleter;
        T *ptr;

        CustomDeleter(T *p, Deleter d) : deleter(d), ptr(p) {}
        void execute() { executeDeleter(ptr, deleter); }
    };

    template <class T>
    struct CustomDeleter<T, NormalDeleter>
    {
        T *ptr;
        CustomDeleter(T *p, NormalDeleter) : ptr(p) {}
        void execute() { delete ptr; }
    };

    template <class T, typename Deleter>
    struct ExternalRefCountWithCustomDeleter : public ExternalRefCountData
    {
        typedef ExternalRefCountWithCustomDeleter Self;
        typedef ExternalRefCountData BaseClass;
        CustomDeleter<T, Deleter> extra;

        static inline void deleter(ExternalRefCountData *self) {
            Self *realself = static_cast<Self *>(self);
            realself->extra.execute();

            realself->extra.~CustomDeleter<T, Deleter>();
        }

//        static void safetyCheckDeleter(ExternalRefCountData *self) {
//            internalSafetyCheckRemove(self);
//            deleter(self);
//        }

        static inline Self *create(T *ptr, Deleter userDeleter, DestroyerFn actualDeleter) {
            Self *d = static_cast<Self *>(::operator new(sizeof(Self)));

            new (&d->extra) CustomDeleter<T, Deleter>(ptr, userDeleter);
            new(d) BaseClass(actualDeleter);

            return d;
        }

    private:
        ExternalRefCountWithCustomDeleter() = delete;
        ~ExternalRefCountWithCustomDeleter() = delete;
        Q_DISABLE_COPY(ExternalRefCountWithCustomDeleter)
    };

    template <class T>
    struct ExternalRefCountWithContiguousData : public ExternalRefCountData
    {
        typedef ExternalRefCountData Parent;
        typedef typename std::remove_cv<T>::type NoCVType;
        NoCVType data;

        static void deleter(ExternalRefCountData *self) {
            auto that = static_cast<ExternalRefCountWithContiguousData *>(self);
            that->data.~T();
            Q_UNUSED(that);
        }
//        static void safetyCheckDeleter(ExternalRefCountData *self) {
//            internalSafetyCheckRemove(self);
//            deleter(self);
//        }
        static void noDeleter(ExternalRefCountData *) { }

        static inline ExternalRefCountData *create(NoCVType **ptr, DestroyerFn destroy) {
            auto d = static_cast<ExternalRefCountWithContiguousData *>(::operator new(sizeof(ExternalRefCountWithContiguousData)));
            new (d) Parent(destroy);
            *ptr = &d->data;
            return d;
        }

    private:
        ExternalRefCountWithContiguousData() = delete;
        ~ExternalRefCountWithContiguousData() = delete;

        Q_DISABLE_COPY(ExternalRefCountWithContiguousData)

    };

//    QWeakPointer<QObject> weakPointerFromVariant_internal(const QVariant &variant);
//    QSharedPointer<QObject> sharedPointerFromVariant_internal(const QVariant &variant);
}

template <class T>
class QSharedPointer {
    typedef QtSharedPointer::ExternalRefCountData Data;
    template <typename X>
    using IfCompatible = typename std::enable_if<std::is_convertible<X *, T *>::value, bool>::type;

public:
    template <class X> friend class QSharedPointer;
    template <class X> friend class QWeakPointer;
    template <class X, class Y> friend QSharedPointer<X> QtSharedPointer::copyAndSetPointer(X *ptr, const QSharedPointer<Y> &src);

    typedef T Type;
    QT_SHAREDPOINTER_TYPEDEF(T)

    T *data() const noexcept { return value; }
    T *get() const noexcept { return value; }
    bool isNull() const noexcept { return !data(); }
    explicit operator bool() const noexcept { return !isNull(); }
    bool operator!() const noexcept { return isNull(); }
    T &operator*() const { return *data(); }
    T *operator->() const noexcept { return data(); }

    constexpr QSharedPointer() noexcept
        : value(nullptr), d(nullptr)
    {}
    ~QSharedPointer() { deref(); }

    constexpr QSharedPointer(std::nullptr_t) noexcept
        : value(nullptr), d(nullptr)
    {}

    template <class X, IfCompatible<X> = true>
    inline explicit QSharedPointer(X *ptr)
        : value(ptr) {
        internalConstruct(ptr, QtSharedPointer::NormalDeleter());
    }

    template <class X, typename Deleter, IfCompatible<X> = true>
    inline QSharedPointer(X *ptr, Deleter deleter)
        : value(ptr) {
        internalConstruct(ptr, deleter);
    }

    template <typename Deleter>
    QSharedPointer(std::nullptr_t, Deleter deleter)
        : value(nullptr) {
        internalConstruct(static_cast<T *>(nullptr), deleter);
    }

    QSharedPointer(const QSharedPointer &other) noexcept
         : value(other.value), d(other.d) {
        if (d) {
            ref();
        }
    }

    QSharedPointer &operator=(const QSharedPointer &other) noexcept {
        //copy析构的时候会调用deref()函数
        QSharedPointer copy(other);
        swap(copy);
        return *this;
    }
    QSharedPointer(QSharedPointer &&other) noexcept
        : value(other.value), d(other.d)
    {
        other.d = nullptr;
        other.value = nullptr;
    }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QSharedPointer)

    template <class X, IfCompatible<X> = true>
    QSharedPointer(QSharedPointer<X> &&other) noexcept
        : value(other.value), d(other.d)
    {
        other.d = nullptr;
        other.value = nullptr;
    }
    template <class X, IfCompatible<X> = true>
    QSharedPointer &operator=(QSharedPointer<X> &&other) noexcept {
        QSharedPointer moved(std::move(other));
        swap(moved);
        return *this;
    }
    template <class X, IfCompatible<X> = true>
    QSharedPointer(const QSharedPointer<X> &other) noexcept
        : value(other.value), d(other.d) {
        if (d) {
            ref();
        }
    }

    template <class X, IfCompatible<X> = true>
    inline QSharedPointer &operator=(const QSharedPointer<X> &other) {
        QSharedPointer copy(other);
        swap(copy);
        return *this;
    }

    template <class X, IfCompatible<X> = true>
    inline QSharedPointer(const QWeakPointer<X> &other )
        : value(nullptr), d(nullptr) {
        *this = other;
    }

    template <class X, IfCompatible<X> = true>
    inline QSharedPointer<T> &operator=(const QWeakPointer<X> &other) {
        internalSet(other.d, other.value);
        return *this;
    }

    inline void swap(QSharedPointer &other) noexcept {
        this->internalSwap(other);
    }
    inline void reset() { clear(); }
    inline void reset(T *t) {
        QSharedPointer copy(t);
        swap(copy);
    }
    template <typename Deleter>
    inline void reset(T *t, Deleter deleter) {
        QSharedPointer copy(t, deleter);
        swap(copy);
    }

    template <class X>
    QSharedPointer<X> staticCast() const {
        return qSharedPointerCast<X, T>(*this);
    }
    template <class X>
    QSharedPointer<X> dynamicCast() const {
        return qSharedPointerDynamicCast<X, T>(*this);
    }
    template <class X>
    QSharedPointer<X> constCast() const {
        return qSharedPointerConstCast<X, T>(*this);
    }
//    template <class X>
//    QSharedPointer<X> objectCast() const {
//        return qSharedPointerObjectCast<X, T>(*this);
//    }

    inline void clear() {
        QSharedPointer copy;
        swap(copy);
    }

    inline QWeakPointer<T> toWeakRef() const;

    template <typename... Args>
    static QSharedPointer create(Args &&...arguments) {
        typedef QtSharedPointer::ExternalRefCountWithContiguousData<T> Private;
        typename Private::DestroyerFn destroy = &Private::deleter;
        typename Private::DestroyerFn noDestroy = &Private::noDeleter;
        QSharedPointer result(Qt::Uninitialized);
        typename std::remove_cv<T>::type *ptr;
        result.d = Private::create(&ptr, noDestroy);
        new (ptr) T(std::forward<Args>(arguments)...);
        result.value = ptr;
        result.d->destroyer = destroy;
        result.d->setQObjectShared(result.value, true);
        result.enableSharedFromThis(result.data());
        return result;
    }

#define DECLARE_COMPARE_SET(T1, A1, T2, A2) \
    friend bool operator==(T1, T2) noexcept \
    { return A1 == A2; }                    \
    friend bool operator!=(T1, T2) noexcept \
    { return A1 != A2; }

#define DECLARE_TEMPLATE_COMPARE_SET(T1, A1, T2, A2) \
    template <typename X>                            \
    friend bool operator==(T1, T2) noexcept          \
    { return A1 == A2; }                             \
    template <typename X>                            \
    friend bool operator!=(T1, T2) noexcept          \
    { return A1 != A2; }

    DECLARE_TEMPLATE_COMPARE_SET(const QSharedPointer &p1, p1.data(), const QSharedPointer<X> &p2, p2.data())
    DECLARE_TEMPLATE_COMPARE_SET(const QSharedPointer &p1, p1.data(), X *ptr, ptr)
    DECLARE_TEMPLATE_COMPARE_SET(X *ptr, ptr, const QSharedPointer &p2, p2.data())
    DECLARE_COMPARE_SET(const QSharedPointer &p1, p1.data(), std::nullptr_t, nullptr)
    DECLARE_COMPARE_SET(std::nullptr_t, nullptr, const QSharedPointer &p2, p2.data())
#undef DECLARE_TEMPLATE_COMPARE_SET
#undef DECLARE_COMPARE_SET

private:
    explicit QSharedPointer(Qt::Initialization) {}
    void deref() noexcept {
        deref(d);
    }
    static void deref(Data *dd) noexcept {
        if (!dd) {
            return;
        }
        if (!dd->strongref.deref()) {
            dd->destroy();
        }
        if (!dd->weakref.deref()) {
            delete dd;
        }
    }

    //函数重载，参数如果是QEnableSharedFromThis<X>，将调用有效函数，否则调用到下方的无效函数
    template <class X>
    inline void enableSharedFromThis(const QEnableSharedFromThis<X> *ptr) {
        ptr->initializeFromSharedPointer(constCast<typename std::remove_cv<T>::type>());
    }
    inline void enableSharedFromThis(...)
    {}

    template <typename X, typename Deleter>
    inline void internalConstruct(X *ptr, Deleter deleter) {
        typedef QtSharedPointer::ExternalRefCountWithCustomDeleter<X, Deleter> Private;
        typename Private::DestroyerFn actualDeleter = &Private::deleter;
        d = Private::create(ptr, deleter, actualDeleter);
        d->setQObjectShared(ptr, true);
        enableSharedFromThis(ptr);
    }

    void internalSwap(QSharedPointer &other) noexcept {
        qSwap(d, other.d);
        qSwap(this->value, other.value);
    }

    void ref() const noexcept {
        d->weakref.ref();
        d->strongref.ref();
    }

    inline void internalSet(Data *o, T *actual) {
        if (o) {
            int tmp = o->strongref.loadRelaxed();
            while (tmp > 0) {
                if (o->strongref.testAndSetRelaxed(tmp, tmp + 1)) {
                    break;
                }
                tmp = o->strongref.loadRelaxed();
            }
            if (tmp > 0) {
                o->weakref.ref();
            }
            else {
                o->checkQObjectShared(actual);
                o = nullptr;
            }
        }
        qSwap(d, o);
        qSwap(this->value, actual);
        if (!d || d->strongref.loadRelaxed() == 0) {
            this->value = nullptr;
        }
        deref(o);
    }

private:
    Type *value;
    Data *d;
};

template <class T>
class QWeakPointer {
    typedef QtSharedPointer::ExternalRefCountData Data;
    //X*是否可以隐式转换为T*
    template <typename X>
    using IfCompatible = typename std::enable_if<std::is_convertible<X *, T *>::value, bool>::type;
public:
    template <class X> friend class QSharedPointer;
    template <class X> friend class QPointer;
    QT_SHAREDPOINTER_TYPEDEF(T)

    bool isNull() const noexcept {
        //RefCount为空或者没有强引用或者value为空
        return d == nullptr || d->strongref.loadRelaxed() == 0 || value == nullptr;
    }
    explicit operator bool() const noexcept {
        return !isNull();
    }
    bool operator !() const noexcept { return isNull(); }

    inline QWeakPointer() noexcept
        : d(nullptr), value(nullptr) {
    }
    inline ~QWeakPointer() {
        if (d && !d->weakref.deref()) {
            delete d;
        }
    }
    QWeakPointer(const QWeakPointer &other) noexcept
        : d(other.d), value(other.value) {
        if (d) {
            d->weakref.ref();
        }
    }
    QWeakPointer(QWeakPointer &&other) noexcept
        : d(other.d), value(other.value) {
        other.d = nullptr;
        other.value = nullptr;
    }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QWeakPointer)
    QWeakPointer &operator=(const QWeakPointer &other) noexcept {
        QWeakPointer copy(other);
        swap(copy);
        return *this;
    }
    void swap(QWeakPointer &other) noexcept {
        qSwap(this->d, other.d);
        qSwap(this->value, other.value);
    }
    inline QWeakPointer(const QSharedPointer<T> &o)
        : d(o.d)
        , value(o.data()) {
        if (d) {
            d->weakref.ref();
        }
    }
    inline QWeakPointer &operator=(const QSharedPointer<T> &o) {
        internalSet(o.d, o.value);
        return *this;
    }

    template <class X, IfCompatible<X> = true>
    inline QWeakPointer(const QWeakPointer<X> &o)
        : d(nullptr), value(nullptr) {
        *this = o;
    }
    template <class X, IfCompatible<X> = true>
    inline QWeakPointer &operator=(const QWeakPointer<X> &o) {
        *this = o.toStrongRef();
        return *this;
    }
    template <class X, IfCompatible<X> = true>
    inline QWeakPointer(const QSharedPointer<X> &o)
        : d(nullptr), value(nullptr) {
        *this = o;
    }
    template <class X, IfCompatible<X> = true>
    inline QWeakPointer &operator=(const QSharedPointer<X> &o) {
        internalSet(o.d, o.data());
        return *this;
    }

    inline void clear() { *this = QWeakPointer(); }

    inline QSharedPointer<T> toStrongRef() const { return QSharedPointer<T>(*this); }
    inline QSharedPointer<T> lock() const { return toStrongRef(); }

    template <class X>
    bool operator==(const QWeakPointer<X> &o) const noexcept {
        return d == o.d && value == static_cast<const T *>(o.value);
    }
    template <class X>
    bool operator!=(const QWeakPointer<X> &o) const noexcept {
        return !(*this == o);
    }
    template <class X>
    bool operator==(const QSharedPointer<X> &o) const noexcept {
        return d == o.d;
    }
    template <class X>
    bool operator!=(const QSharedPointer<X> &o) const noexcept {
        return !(*this == o);
    }
    template <typename X>
    friend bool operator==(const QSharedPointer<X> &p1, const QWeakPointer &p2) noexcept {
        return p2 == p1;
    }
    template <typename X>
    friend bool operator!=(const QSharedPointer<X> &p1, const QWeakPointer &p2) noexcept {
        return p2 != p1;
    }

    friend bool operator==(const QWeakPointer &p, std::nullptr_t) {
        return p.isNull();
    }
    friend bool operator==(std::nullptr_t, const QWeakPointer &p) {
        return p.isNull();
    }
    friend bool operator!=(const QWeakPointer &p, std::nullptr_t) {
        return !p.isNull();
    }
    friend bool operator!=(std::nullptr_t, const QWeakPointer &p) {
        return !p.isNull();
    }

    template <class X>
    inline QWeakPointer &assign(X *ptr) {
        return *this = QWeakPointer<X>(ptr, true);
    }

    template <class X, IfCompatible<X> = true>
    inline QWeakPointer(X *ptr, bool)
        : d(ptr ? Data::getAndRef(ptr) : nullptr), value(ptr)
    {}

private:
    inline void internalSet(Data *o, T *actual) {
        if (d == o) {
            return;
        }
        if (o) {
            o->weakref.ref();
        }
        if (d && !d->weakref.deref()) {
            Q_ASSERT(false);
            delete d;
        }
        d = o;
        value = actual;
    }

    inline T *internalData() const noexcept {
        return d == nullptr || d->strongref.loadRelaxed() == 0 ? nullptr : value;
    }

private:


private:
    Data *d;
    T *value;
};

/*
 * QEnableSharedFromThis内部存放了weakPointer
 * 在创建智能指针时，QSharedPointer会对weakPointer进行初始化
 * */
template <class T>
class QEnableSharedFromThis {
    //模板累作为别的类的友元： https://blog.csdn.net/caroline_wendy/article/details/16916441
    template <class X> friend class QSharedPointer;
protected:
    QEnableSharedFromThis() = default;
    QEnableSharedFromThis(const QEnableSharedFromThis &) {}
    //不进行任何操作
    QEnableSharedFromThis &operator=(const QEnableSharedFromThis &) { return *this; }

public:
    inline QSharedPointer<T> sharedFromThis() {
        return QSharedPointer<T>(weakPointer);
    }
    inline QSharedPointer<const T> sharedFromThis() const {
        return QSharedPointer<const T>(weakPointer);
    }

private:
    template <class X>
    inline void initializeFromSharedPointer(const QSharedPointer<X> &ptr) const {
        //安全性的保障交给编译器与 operator=
        weakPointer = ptr;
    }

private:
    mutable QWeakPointer<T> weakPointer;
};

template <class T>
inline QWeakPointer<T> QSharedPointer<T>::toWeakRef() const {
    return QWeakPointer<T>(*this);
}

//operator -
template <class T, class X>
inline typename QSharedPointer<T>::difference_type operator-(const QSharedPointer<T> &ptr1, const QSharedPointer<X> &ptr2) {
    return ptr1.data() - ptr2.data();
}
template <class T, class X>
inline typename QSharedPointer<T>::difference_type operator-(const QSharedPointer<T> &ptr1, X *ptr2) {
    return ptr1.data() - ptr2;
}
template <class T, class X>
inline typename QSharedPointer<X>::difference_type operator-(T *ptr1, const QSharedPointer<X> &ptr2) {
    return ptr1 - ptr2.data();
}

//operator <
template <class T, class X>
inline bool operator<(const QSharedPointer<T> &ptr1, const QSharedPointer<X> &ptr2) {
    using CT = typename std::common_type<T *, X *>::type;
    return std::less<CT>()(ptr1.data(), ptr2.data());
}
template <class T, class X>
inline bool operator<(const QSharedPointer<T> &ptr1, X *ptr2) {
    using CT = typename std::common_type<T *, X *>::type;
    return std::less<CT>()(ptr1.data(), ptr2);
}
template <class T, class X>
inline bool operator<(T *ptr1, const QSharedPointer<X> &ptr2) {
    using CT = typename std::common_type<T *, X *>::type;
    return std::less<CT>()(ptr1, ptr2.data());
}

//swap
template <class T>
inline void swap(QSharedPointer<T> &p1, QSharedPointer<T> &p2) noexcept {
    p1.swap(p2);
}
template <class T>
inline void swap(QWeakPointer<T> &p1, QWeakPointer<T> &p2) noexcept {
    p1.swap(p2);
}

//cast
template <class X, class T>
inline QSharedPointer<X> qSharedPointerCast(const QSharedPointer<T> &src) {
    X *ptr = static_cast<X *>(src.data());
    return QtSharedPointer::copyAndSetPointer(ptr, src);
}
template <class X, class T>
inline QSharedPointer<X> qSharedPointerCast(const QWeakPointer<T> &src) {
    return qSharedPointerCast<X, T>(src.toStrongRef());
}

template <class X, class T>
inline QSharedPointer<X> qSharedPointerDynamicCast(const QSharedPointer<T> &src) {
    X *ptr = dynamic_cast<X *>(src.data());
    if (!ptr) {
        return QSharedPointer<X>();
    }
    return QtSharedPointer::copyAndSetPointer(ptr, src);
}
template <class X, class T>
inline QSharedPointer<X> qSharedPointerDynamicCast(const QWeakPointer<T> &src) {
    return qSharedPointerDynamicCast<X, T>(src.toStrongRef());
}
template <class X, class T>
inline QSharedPointer<X> qSharedPointerConstCast(const QSharedPointer<T> &src) {
    X *ptr = const_cast<X *>(src.data());
    return QtSharedPointer::copyAndSetPointer(ptr, src);
}
template <class X, class T>
inline QSharedPointer<X> qSharedPointerConstCast(const QWeakPointer<T> &src) {
    return qSharedPointerConstCast<X, T>(src.toStrongRef());
}

template <class X, class T>
inline QWeakPointer<X> qWeakPointerCast(const QSharedPointer<T> &src) {
    return qSharedPointerCast<X, T>(src).toWeakRef();
}

namespace QtSharedPointer {
        template <class X, class T>
        inline QSharedPointer<X> copyAndSetPointer(X *ptr, const QSharedPointer<T> &src) {
            QSharedPointer<X> result;
            result.internalSet(src.d, ptr);
            return result;
        }
    }

#undef QT_SHAREDPOINTER_TYPEDEF


QT_END_NAMESPACE

#endif //QSHAREDPOINTER_H
