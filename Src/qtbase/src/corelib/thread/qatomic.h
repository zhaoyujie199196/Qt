//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QATOMIC_H
#define QATOMIC_H

#include <QtCore/qglobal.h>
#include "qbasicatomic.h"

QT_BEGIN_NAMESPACE

template <typename T>
class QAtomicInteger : public QBasicAtomicInteger<T>
{
public:
    constexpr QAtomicInteger(T value = 0) noexcept
        : QBasicAtomicInteger<T>(value) {
    }

    inline QAtomicInteger(const QAtomicInteger &other) noexcept
        : QBasicAtomicInteger<T>(){
        this->storeRelease(other.loadAcquire());
    }

    inline QAtomicInteger &operator=(const QAtomicInteger &other) noexcept {
        this->storeRelease(other.loadAcquire());
        return *this;
    }
};

class QAtomicInt : public QAtomicInteger<int>{
public:
    QAtomicInt(int value = 0) noexcept
        : QAtomicInteger<int>(value) {
    }
};

template <typename T>
class QAtomicPointer : public QBasicAtomicPointer<T>
{
public:
    constexpr QAtomicPointer(T *value = nullptr) noexcept
        : QBasicAtomicPointer<T>(value) {
    }

    inline QAtomicPointer(const QAtomicPointer<T> &other) noexcept {
        this->storeRelease(other.loadAcquire());
    }

    inline QAtomicPointer<T> &operator=(const QAtomicPointer<T> &other) noexcept {
        this->storeRelease(other.loadAcquire());
        return *this;
    }
};

QT_END_NAMESPACE

#endif //QATOMIC_H
