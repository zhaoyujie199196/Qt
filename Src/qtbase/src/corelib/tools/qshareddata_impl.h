//
// Created by Yujie Zhao on 2023/9/21.
//

#ifndef QSHAREDDATA_IMPL_H
#define QSHAREDDATA_IMPL_H

#include <QtCore/qglobal.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {

    template <typename T>
    class QExplicitlySharedDataPointerV2
    {
        T *d;
    public:
        constexpr QExplicitlySharedDataPointerV2() noexcept : d(nullptr) {}

        explicit QExplicitlySharedDataPointerV2(T *t) noexcept
            : d(t)
        {
            if (d) {
                d->ref.ref();
            }
        }

        QExplicitlySharedDataPointerV2(T *t, QAdoptSharedDataTag) noexcept : d(t)
        {
        }

        QExplicitlySharedDataPointerV2(const QExplicitlySharedDataPointerV2 &other) noexcept
            : d(other.d)
        {
            if (d) {
                d->ref.ref();
            }
        }

        QExplicitlySharedDataPointerV2 &operator=(const QExplicitlySharedDataPointerV2 &other) noexcept
        {
            QExplicitlySharedDataPointerV2 copy(other);
            swap(copy);
            return *this;
        }

        QExplicitlySharedDataPointerV2(QExplicitlySharedDataPointerV2 &&other) noexcept
            : d(qExchange(other.d, nullptr))
        {
        }

        QExplicitlySharedDataPointerV2 &operator=(QExplicitlySharedDataPointerV2 &&other) noexcept
        {
            QExplicitlySharedDataPointerV2 moved(std::move(other));
            swap(moved);
            return *this;
        }

        ~QExplicitlySharedDataPointerV2()
        {
            if (d && !d->ref.deref()) {
                delete d;
            }
        }

        void detach()
        {
            if (!d) {
                d = new T;
                d->ref.ref();
            }
            else if (d->ref.loadRelaxed() != 1) {
                QExplicitlySharedDataPointerV2 copy(new T(*d));
                swap(copy);
            }
        }

        void reset(T *t = nullptr) noexcept
        {
            if (d && !d->ref.deref()) {
                delete d;
            }
            d = t;
            if (d) {
                d->ref.ref();
            }
        }

        constexpr T *take() noexcept
        {
            return qExchange(d, nullptr);
        }

        bool isShared() const noexcept
        {
            return d && d->ref.loadRelaxed() != 1;
        }

        constexpr void swap(QExplicitlySharedDataPointerV2 &other) noexcept
        {
            qSwap(d, other.d);
        }

        constexpr T &operator*() { return *d; }
        constexpr T *operator->() { return d; }
        constexpr const T &operator*() const { return *d; }
        constexpr const T *operator->() const { return d; }

        constexpr T *data() noexcept  { return d; }
        constexpr const T *data() const noexcept { return d; }

        constexpr explicit operator bool() const noexcept { return d; }

        constexpr friend bool operator==(const QExplicitlySharedDataPointerV2 &lhs, const QExplicitlySharedDataPointerV2 &rhs) noexcept
        {
            return lhs.d == rhs.d;
        }

        constexpr friend bool operator!=(const QExplicitlySharedDataPointerV2 &lhs, const QExplicitlySharedDataPointerV2 &rhs) noexcept
        {
            return lhs.d != rhs.d;
        }
    };

    template <typename T>
    constexpr void swap(QExplicitlySharedDataPointerV2<T> &lhs, QExplicitlySharedDataPointerV2<T> &rhs) noexcept
    {
        lhs.swap(rhs);
    }
}

QT_END_NAMESPACE

#endif //QSHAREDDATA_IMPL_H
