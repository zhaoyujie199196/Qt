//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QSCOPEDPOINTER_H
#define QSCOPEDPOINTER_H

#include <QtCore/qglobal.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

//template <typename T>
//struct QScopedPointerDeleter
//{
//    static inline void cleanup(T *pointer) noexcept
//    {
////        // Enforce a complete type.
////        // If you get a compile error here, read the section on forward declared
////        // classes in the QScopedPointer documentation.
////        typedef char IsIncompleteType[ sizeof(T) ? 1 : -1 ];
////        (void) sizeof(IsIncompleteType);
//
//        delete pointer;
//    }
//    void operator()(T *pointer) const noexcept
//    {
//        cleanup(pointer);
//    }
//};
//
//template <typename T, typename Cleanup = QScopedPointerDeleter<T> >
//class QScopedPointer
//{
//public:
//    explicit QScopedPointer(T *p = nullptr) noexcept : d(p)
//    {
//    }
//
//    inline ~QScopedPointer()
//    {
//        T *oldD = this->d;
//        Cleanup::cleanup(oldD);
//    }
//
//    inline T &operator*() const
//    {
//        Q_ASSERT(d);
//        return *d;
//    }
//
//    T *operator->() const noexcept
//    {
//        return d;
//    }
//
//    bool operator!() const noexcept
//    {
//        return !d;
//    }
//
//    explicit operator bool() const
//    {
//        return !isNull();
//    }
//
//    T *data() const noexcept
//    {
//        return d;
//    }
//
//    T *get() const noexcept
//    {
//        return d;
//    }
//
//    bool isNull() const noexcept
//    {
//        return !d;
//    }
//
//    void reset(T *other = nullptr) noexcept(noexcept(Cleanup::cleanup(std::declval<T *>())))
//    {
//        if (d == other)
//            return;
//        T *oldD = qExchange(d, other);
//        Cleanup::cleanup(oldD);
//    }
//
//    T *take() noexcept
//    {
//        T *oldD = qExchange(d, nullptr);
//        return oldD;
//    }
//
//    void swap(QScopedPointer<T, Cleanup> &other) noexcept
//    {
//        qSwap(d, other.d);
//    }
//
//    typedef T *pointer;
//
//    friend bool operator==(const QScopedPointer<T, Cleanup> &lhs, const QScopedPointer<T, Cleanup> &rhs) noexcept
//    {
//        return lhs.data() == rhs.data();
//    }
//
//    friend bool operator!=(const QScopedPointer<T, Cleanup> &lhs, const QScopedPointer<T, Cleanup> &rhs) noexcept
//    {
//        return lhs.data() != rhs.data();
//    }
//
//    friend bool operator==(const QScopedPointer<T, Cleanup> &lhs, std::nullptr_t) noexcept
//    {
//        return lhs.isNull();
//    }
//
//    friend bool operator==(std::nullptr_t, const QScopedPointer<T, Cleanup> &rhs) noexcept
//    {
//        return rhs.isNull();
//    }
//
//    friend bool operator!=(const QScopedPointer<T, Cleanup> &lhs, std::nullptr_t) noexcept
//    {
//        return !lhs.isNull();
//    }
//
//    friend bool operator!=(std::nullptr_t, const QScopedPointer<T, Cleanup> &rhs) noexcept
//    {
//        return !rhs.isNull();
//    }
//
//    friend void swap(QScopedPointer<T, Cleanup> &p1, QScopedPointer<T, Cleanup> &p2) noexcept
//    { p1.swap(p2); }
//
//protected:
//    T *d;
//
//private:
//    Q_DISABLE_COPY(QScopedPointer)
//};


QT_END_NAMESPACE

#endif //QSCOPEDPOINTER_H
