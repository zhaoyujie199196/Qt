//
// Created by Yujie Zhao on 2023/2/20.
//

#ifndef QATOMIC_CXX11_H
#define QATOMIC_CXX11_H

#include <QtCore/qglobal.h>
#include <atomic>
#include "qgenericatomic.h"

/*
 * 封装c++11的atomic
 * 如果没有const volatile &为参的函数，const volatile a 之类的变量将编译不过
 * */
QT_BEGIN_NAMESPACE

template <>
struct QAtomicQpsSupport<1> { enum { IsSupported = 1 }; };
template <>
struct QAtomicQpsSupport<2> { enum { IsSupported = 1 }; };
template <>
struct QAtomicQpsSupport<8> {enum {IsSupported = 1};};


template <typename X>
struct QAtomicOps {
    typedef std::atomic<X> Type;

    template <typename T> static inline
    T load(const std::atomic<T> &_q_value) noexcept {
        return _q_value.load(std::memory_order_relaxed);
    }

    template <typename T> static inline
    T load(const volatile std::atomic<T> &_q_value) noexcept {
        return _q_value.load(std::memory_order_relaxed);
    }

    template <typename T> static inline
    T loadRelaxed(const std::atomic<T> &_q_value) noexcept {
        return _q_value.load(std::memory_order_relaxed);
    }

    template <typename T> static inline
    T loadRelaxed(const volatile std::atomic<T> &_q_value) noexcept {
        return _q_value, load(std::memory_order_relaxed);
    }

    template <typename T> static inline
    T loadAcquire(const std::atomic<T> &_q_value) noexcept {
        return _q_value.load(std::memory_order_acquire);
    }

    template <typename T> static inline
    T loadAcquire(const volatile std::atomic<T> &_q_value) noexcept {
        return _q_value.load(std::memory_order_acquire);
    }

    template <typename T> static inline
    void store(std::atomic<T> &_q_value, T newValue) noexcept {
        _q_value.store(newValue, std::memory_order_relaxed);
    }

    template <typename T> static inline
    void storeRelaxed(std::atomic<T> &_q_value, T newValue) noexcept {
        _q_value.store(newValue, std::memory_order_relaxed);
    }

    template <typename T> static inline
    void storeRelease(std::atomic<T> &_q_value, T newValue) noexcept {
        return _q_value.store(newValue, std::memory_order_relaxed);
    }

    static inline bool isReferenceCountingNative() noexcept {return isTestAndSetNative();}
    static inline constexpr bool isReferenceCountingWaitFree() noexcept {return false;}

    template <typename T>
    static inline bool ref(std::atomic<T> &_q_value) noexcept {
        return ++_q_value != 0;
    }

    template <typename T>
    static inline bool deref(std::atomic<T> &_q_value) noexcept {
        return --_q_value != 0;
    }

    static inline bool isTestAndSetNative() noexcept {
        //zhaoyujie TODO
        assert(false);
        return false;
//        return QAtomicTraits<sizeof(X)>::isLockFree();
    }

    static inline constexpr bool isTestAndSetWaitFree() noexcept {
        return false;
    }

    template <typename T>
    static bool testAndSetRelaxed(std::atomic<T> &_q_value, T expectedValue, T newValue, T *currentValue = nullptr) noexcept {
        bool tmp = _q_value.compare_exchange_strong(expectedValue, newValue, std::memory_order_relaxed, std::memory_order_relaxed);
        if (currentValue) {
            *currentValue = expectedValue;
        }
        return tmp;
    }

    template <typename T>
    static bool testAndSetAcquire(std::atomic<T> &_q_value, T expectedValue, T newValue, T *currentValue = nullptr) noexcept {
        bool tmp = _q_value.compare_exchange_strong(expectedValue, newValue, std::memory_order_acquire, std::memory_order_acquire);
        if (currentValue) {
            *currentValue = expectedValue;
        }
        return tmp;
    }

    template <typename T>
    static bool testAndSetRelease(std::atomic<T> &_q_value, T expectedValue, T newValue, T *currentValue = nullptr) noexcept {
        bool tmp = _q_value.compare_exchange_strong(expectedValue, newValue, std::memory_order_release, std::memory_order_release);
        if (currentValue) {
            *currentValue = expectedValue;
        }
        return tmp;
    }

    template <typename T>
    static bool testAndSetOrdered(std::atomic<T> &_q_value, T expectedValue, T newValue, T *currentValue = nullptr) noexcept {
        bool tmp = _q_value.compare_exchange_strong(expectedValue, newValue, std::memory_order_acq_rel, std::memory_order_acquire);
        if (currentValue ){
            *currentValue = expectedValue;
        }
        return tmp;
    }

    static inline bool isFetchAndStoreNative() noexcept { return isTestAndSetNative(); }
    static inline constexpr bool isFetchAndStoreWaitFree() noexcept { return false; }

    template <typename T>
    static T fetchAndStoreRelaxed(std::atomic<T> &_q_value, T newValue) noexcept {
        return _q_value.exchange(newValue, std::memory_order_relaxed);
    }

    template <typename T>
    static T fetchAndStoreAcquire(std::atomic<T> &_q_value, T newValue) noexcept {
        return _q_value.exchange(newValue, std::memory_order_acquire);
    }

    template <typename T>
    static T fetchAndStoreRelease(std::atomic<T> &_q_value, T newValue) noexcept {
        return _q_value.exchange(newValue, std::memory_order_release);
    }

    template <typename T>
    static T fetchAndStoreOrdered(std::atomic<T> &_q_value, T newValue) noexcept {
        return _q_value.exchange(newValue, std::memory_order_acq_rel);
    }

    static inline bool isFetchAndAddNative() noexcept {return isTestAndSetNative();}
    static inline constexpr bool isFetchAndAddWaitFree() noexcept {return false;}

    template <typename T> static inline
    T fetchAndAddRelaxed(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept {
        return _q_value.fetch_add(valueToAdd, std::memory_order_relaxed);
    }

    template <typename T> static inline
    T fetchAndAddAcquire(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept {
        return _q_value.fetch_add(valueToAdd, std::memory_order_acquire);
    }

    template <typename T> static inline
    T fetchAndAddRelease(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept {
        return _q_value.fetch_add(valueToAdd, std::memory_order_release);
    }

    template <typename T> static inline
    T fetchAndAddOrdered(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept {
        return _q_value.fetch_add(valueToAdd, std::memory_order_acq_rel);
    }

    template <typename T> static inline
    T fetchAndSubRelaxed(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToSub) noexcept {
        return _q_value.fetch_sub(valueToSub, std::memory_order_relaxed);
    }

    template <typename T> static inline
    T fetchAndSubAcquire(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToSub) noexcept {
        return _q_value.fetch_sub(valueToSub, std::memory_order_acquire);
    }

    template <typename T> static inline
    T fetchAndSubRelease(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToSub) noexcept {
        return _q_value.fetch_sub(valueToSub, std::memory_order_release);
    }

    template <typename T> static inline
    T fetchAndSubOrdered(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToSub) noexcept {
        return _q_value.fetch_sub(valueToSub, std::memory_order_acq_rel);
    }

    template <typename T> static inline
    T fetchAndAndRelaxed(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAnd) noexcept {
        return _q_value.fetch_and(valueToAnd, std::memory_order_relaxed);
    }

    template <typename T> static inline
    T fetchAndAndAcquire(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAnd) noexcept {
        return _q_value.fetch_and(valueToAnd, std::memory_order_acquire);
    }

    template <typename T> static inline
    T fetchAndAndRelease(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAnd) noexcept {
        return _q_value.fetch_and(valueToAnd, std::memory_order_release);
    }

    template <typename T> static inline
    T fetchAndAndOrdered(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAnd) noexcept {
        return _q_value.fetch_and(valueToAnd, std::memory_order_acq_rel);
    }

    template <typename T> static inline
    T fetchAndOrRelaxed(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToOr) noexcept {
        return _q_value.fetch_or(valueToOr, std::memory_order_relaxed);
    }

    template <typename T> static inline
    T fetchAndOrAcquire(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToOr) noexcept {
        return _q_value.fetch_or(valueToOr, std::memory_order_acquire);
    }

    template <typename T> static inline
    T fetchAndOrRelease(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToOr) noexcept {
        return _q_value.fetch_or(valueToOr, std::memory_order_release);
    }

    template <typename T> static inline
    T fetchAndOrOrdered(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToOr) noexcept {
        return _q_value.fetch_or(valueToOr, std::memory_order_acq_rel);
    }

    template <typename T> static inline
    T fetchAndNorRelaxed(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToNor) noexcept {
        return _q_value.fetch_nor(valueToNor, std::memory_order_relaxed);
    }

    template <typename T> static inline
    T fetchAndNorAcquire(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToNor) noexcept {
        return _q_value.fetch_nor(valueToNor, std::memory_order_acquire);
    }

    template <typename T> static inline
    T fetchAndNorRelease(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToNor) noexcept {
        return _q_value.fetch_nor(valueToNor, std::memory_order_release);
    }

    template <typename T> static inline
    T fetchAndNorOrdered(std::atomic<T> &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToNor) noexcept {
        return _q_value.fetch_nor(valueToNor, std::memory_order_acq_rel);
    }
};

#define Q_BASIC_ATOMIC_INITIALIZER(a) { a }

QT_END_NAMESPACE

#endif //QATOMIC_CXX11_H
