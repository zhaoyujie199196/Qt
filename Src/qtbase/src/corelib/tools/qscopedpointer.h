//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QSCOPEDPOINTER_H
#define QSCOPEDPOINTER_H

#include <QtCore/qglobal.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

/*
 * 超出作用域，删除指针
 * */

template <typename T>
struct QScopedPointerDeleter
{
    static inline void cleanup(T *pointer) noexcept {
        typedef char IsIncompleteType[sizeof(T) ? 1 : -1];
        (void) sizeof(IsIncompleteType);

        delete pointer;
    }
    void operator()(T *pointer) const noexcept {
        cleanup(pointer);
    }
};

template <typename T>
struct QScopedPointerArrayDeleter
{
    static inline void cleanup(T *pointer) noexcept {
        typedef char IsIncompleteType[sizeof(T) ? 1 : -1];
        (void) sizeof(IsIncompleteType);
        delete[] pointer;
    }
    void operator()(T *pointer) const noexcept
    {
        cleanup(pointer);
    }
};

struct QScopedPointerPodDeleter
{
    static inline void cleanup(void *pointer) noexcept {
        free(pointer);
    }
    void operator()(void *pointer) const noexcept  {
        cleanup(pointer);
    }
};

//QObject类型，调用deleteLater接口
template <typename T>
struct QScopedPointerObjectDeleteLater {
    static inline void cleanup(T *pointer) {
        if (pointer) {
            pointer->deleteLater();
        }
    }
    void operator()(T *pointer) const {
        cleanup(pointer);
    }
};
class QObject;
typedef QScopedPointerObjectDeleteLater<QObject> QScopedPointerDeleteLater;

template <typename T, typename Cleanup = QScopedPointerDeleter<T>>
class QScopedPointer
{
public:
    explicit QScopedPointer(T *p = nullptr) noexcept
        : d(p)
    {}

    inline ~QScopedPointer()
    {
        T *oldD = this->d;
        Cleanup::cleanup(oldD);
    }

    inline T &operator*() const
    {
        Q_ASSERT(d);
        return *d;
    }

    T *operator->() const noexcept
    {
        return d;
    }

    bool operator!() const noexcept
    {
        return !d;
    }

    explicit operator bool() const
    {
        return !isNull();
    }

    T *data() const noexcept
    {
        return d;
    }

    T *get() const noexcept
    {
        return d;
    }

    bool isNull() const noexcept
    {
        return !d;
    }

    void reset(T *other = nullptr) noexcept
    {
        if (d == other) {
            return;
        }
        T *oldD = qExchange(d, other);
        Cleanup::cleanup(oldD);
    }

    T *take() noexcept
    {
        T *oldD = qExchange(d, nullptr);
        return oldD;
    }

    void swap(QScopedPointer<T, Cleanup> &other) noexcept
    {
        qSwap(d, other.d);
    }

    typedef T *pointer;

    friend bool operator==(const QScopedPointer<T, Cleanup> &lhs, const QScopedPointer<T, Cleanup> &rhs) noexcept
    {
        return lhs.data() == rhs.data();
    }

    friend bool operator!=(const QScopedPointer<T, Cleanup> &lhs, const QScopedPointer<T, Cleanup> &rhs) noexcept
    {
        return lhs.data() != rhs.data();
    }

    friend bool operator==(const QScopedPointer<T, Cleanup> &lhs, std::nullptr_t) noexcept
    {
        return lhs.isNull();
    }

    friend bool operator==(std::nullptr_t, const QScopedPointer<T, Cleanup> &rhs) noexcept
    {
        return rhs.isNull();
    }

    friend bool operator!=(const QScopedPointer<T, Cleanup> &lhs, std::nullptr_t) noexcept
    {
        return !lhs.isNull();
    }

    friend bool operator!=(std::nullptr_t, const QScopedPointer<T, Cleanup> &rhs) noexcept
    {
        return !rhs.isNull();
    }

    friend void swap(QScopedPointer<T, Cleanup> &p1, QScopedPointer<T, Cleanup> &p2) noexcept
    {
        p1.swap(p2);
    }

protected:
    T *d;

private:
    Q_DISABLE_COPY(QScopedPointer)
};

template <typename T, typename Cleanup = QScopedPointerArrayDeleter<T>>
class QScopedArrayPointer : public QScopedPointer<T, Cleanup>
{
    template <typename Ptr>
    using if_same_type = typename std::enable_if<std::is_same<typename std::remove_cv<T>::type, Ptr>::value, bool>::type;

public:
    inline QScopedArrayPointer()
        : QScopedPointer<T, Cleanup>(nullptr)
    {}

    template <typename D, if_same_type<D> = true>
    explicit QScopedArrayPointer(D *p)
        : QScopedPointer<T, Cleanup>(p)
    {}

    inline T &operator[](int i)
    {
        return this->d[i];
    }

    inline const T &operator[](int i) const
    {
        return this->d[i];
    }

    void swap(QScopedArrayPointer &other) noexcept
    {
        QScopedPointer<T, Cleanup>::swap(other);
    }

private:
    explicit inline QScopedArrayPointer(void *)
    {
    }

    Q_DISABLE_COPY(QScopedArrayPointer)
};

template <typename T, typename Cleanup>
inline void swap(QScopedArrayPointer<T, Cleanup> &lhs, QScopedArrayPointer<T, Cleanup> &rhs) noexcept
{
    lhs.swap(rhs);
}

QT_END_NAMESPACE

#endif //QSCOPEDPOINTER_H
