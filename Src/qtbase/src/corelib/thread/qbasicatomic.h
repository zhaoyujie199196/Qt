#ifndef QBASICATOMIC_H
#define QBASICATOMIC_H

#include <QtCore/qglobal.h>
#include <QtCore/QTypeInfo>
#include <atomic>
#include "qatomic_cxx11.h"

QT_BEGIN_NAMESPACE

template <typename T>
class QBasicAtomicInteger
{
public:
    typedef T Type;
    typedef QAtomicOps<T> Ops;
    //必须是integral类型
    static_assert(QTypeInfo<T>::isIntegral, "QBasicAtomicInteger template parameter is not an integral type");
    static_assert(QAtomicQpsSupport<sizeof(T)>::IsSupported, "QBasicAtomicInteger template parameter is an integral of a size not supported on this platform");

    //定义_q_value数据
    typename Ops::Type _q_value;

    QBasicAtomicInteger() = default;
    constexpr QBasicAtomicInteger(Type value) noexcept
        : _q_value(value) {
    }
    QBasicAtomicInteger(const QBasicAtomicInteger &) = delete;
    QBasicAtomicInteger &operator=(const QBasicAtomicInteger &) = delete;
    QBasicAtomicInteger &operator=(const QBasicAtomicInteger &) volatile = delete;


    T loadRelaxed() const noexcept {return Ops::loadRelaxed(_q_value);}
    void storeRelaxed(T newValue) noexcept {Ops::storeRelaxed(newValue);}

    T loadAcquire() const noexcept {return Ops::loadAcquire(_q_value);}
    void storeRelease(const T &newValue) noexcept {Ops::storeRelease(_q_value, newValue);}
    // 使用诸如这种场景：QBasicAtomicInteger<int> k(12); int a = k;
    operator T() const noexcept {return loadAcquire();}
    T operator=(const T &newValue) noexcept { storeRelease(newValue); return newValue;}

    static constexpr bool isReferenceCountingNative() noexcept {return Ops::isReferenceCountingNative();}
    static constexpr bool isReferenceCountingWaitFree() noexcept {return Ops::isReferenceCountingWaitFree();}

    bool ref() noexcept {return Ops::ref(_q_value);}
    bool deref() noexcept {return Ops::deref(_q_value);}

    static constexpr bool isTestAndSetNative() noexcept {return Ops::isTestAndSetNative();}
    static constexpr bool isTestAndSetWaitFree() noexcept {return Ops::IsTestAndSetWaitFree();}

    bool testAndSetRelaxed(Type expectedValue, Type newValue) noexcept {
        return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue);
    }
    bool testAndSetAcquire(Type expectedValue, Type newValue) noexcept {
        return Ops::testAndSetAcquire(_q_value, expectedValue, newValue);
    }
    bool testAndSetRelease(Type expectedValue, Type newValue) noexcept {
        return Ops::testAndSetRelease(_q_value, expectedValue, newValue);
    }
    bool testAndSetOrdered(Type expectedValue, Type newValue) noexcept {
        return Ops::testAndSetOrdered(_q_value, expectedValue, newValue);
    }
    bool testAndSetRelaxed(Type expectedValue, Type newValue, Type &currentValue) noexcept {
        return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue, &currentValue);
    }
    bool testAndSetAcquire(Type expectedValue, Type newValue, Type &currentValue) noexcept {
        return Ops::testAndSetAcquire(_q_value, expectedValue, newValue, &currentValue);
    }
    bool testAndSetRelease(Type expectedValue, Type newValue, Type &currentValue) noexcept {
        return Ops::testAndSetRelease(_q_value, expectedValue, newValue, &currentValue);
    }
    bool testAndSetOrdered(Type expectedValue, Type newValue, Type &currentValue) noexcept {
        return Ops::testAndSetOrdered(_q_value, expectedValue, newValue, &currentValue);
    }

    static constexpr bool isFetchAndStoreNative() noexcept { return Ops::isFetchAndStoreNative(); }
    static constexpr bool isFetchAndStoreWaitFree() noexcept {return Ops::isFetchAndStoreWaitFree(); }

    Type fetchAndStoreRelaxed(Type newValue) noexcept {
        return Ops::fetchAndStoreRelaxed(_q_value, newValue);
    }
    Type fetchAndStoreAcquire(Type newValue) noexcept {
        return Ops::fetchAndStoreAcquire(_q_value, newValue);
    }
    Type fetchAndStoreRelease(Type newValue) noexcept {
        return Ops::fetchAndStoreRelease(_q_value, newValue);
    }
    Type fetchAndStoreOrdered(Type newValue) noexcept {
        return Ops::fetchAndStoreOrdered(_q_value, newValue);
    }

    static constexpr bool isFetchAndAddNative() noexcept {return Ops::isFetchAndAddNative();}
    static constexpr bool isFetchAndAddWaitFree() noexcept {return Ops::isFetchAndAddWaitFree();}

    //zhaoyujie TODO 为什么Qt源码这里的数据类型用的qptrdiff ？
    Type fetchAndAddRelaxed(Type valueToAdd) noexcept {
        return Ops::fetchAndAddRelaxed(_q_value, valueToAdd);
    }
    Type fetchAndAddAcquire(Type valueToAdd) noexcept {
        return Ops::fetchAndAddAcquire(_q_value, valueToAdd);
    }
    Type fetchAndAddRelease(Type valueToAdd) noexcept {
        return Ops::fetchAndAddRelease(_q_value, valueToAdd);
    }
    Type fetchAndAddOrdered(Type valueToAdd) noexcept {
        return Ops::fetchAndAddOrdered(_q_value, valueToAdd);
    }

    Type fetchAndSubRelaxed(Type valueToSub) noexcept {
        return Ops::fetchAndSubRelaxed(_q_value, valueToSub);
    }
    Type fetchAndSubAcquire(Type valueToSub) noexcept {
        return Ops::fetchAndSubAcquire(_q_value, valueToSub);
    }
    Type fetchAndSubRelease(Type valueToSub) noexcept {
        return Ops::fetchAndSubRelease(_q_value, valueToSub);
    }
    Type fetchAndSubOrdered(Type valueToSub) noexcept {
        return Ops::fetchAndAddOrdered(_q_value, valueToSub);
    }

    Type fetchAndAndRelaxed(Type valueToAnd) noexcept {
        return Ops::fetchAndAndRelaxed(_q_value, valueToAnd);
    }
    Type fetchAndAndAcquire(Type valueToAnd) noexcept {
        return Ops::fetchAndAndAcquire(_q_value, valueToAnd);
    }
    Type fetchAndAndRelease(Type valueToAnd) noexcept {
        return Ops::fetchAndAndRelease(_q_value, valueToAnd);
    }
    Type fetchAndAndOrdered(Type valueToAnd) noexcept {
        return Ops::fetchAndAndOrdered(_q_value, valueToAnd);
    }

    Type fetchAndOrRelaxed(Type valueToOr) noexcept {
        return Ops::fetchAndOrRelaxed(_q_value, valueToOr);
    }
    Type fetchAndOrAcquire(Type valueToOr) noexcept {
        return Ops::fetchAndOrAcquire(_q_value, valueToOr);
    }
    Type fetchAndOrRelease(Type valueToOr) noexcept {
        return Ops::fetchAndOrRelease(_q_value, valueToOr);
    }
    Type fetchAndOrOrdered(Type valueToOr) noexcept {
        return Ops::fetchAndOrOrdered(_q_value, valueToOr);
    }

    Type fetchAndNorRelaxed(Type valueToNor) noexcept {
        return Ops::fetchAndNorRelaxed(_q_value, valueToNor);
    }
    Type fetchAndNorAcquire(Type valueToNor) noexcept {
        return Ops::fetchAndNorAcquire(_q_value, valueToNor);
    }
    Type fetchAndNorRelease(Type valueToNor) noexcept {
        return Ops::fetchAndNorRelease(_q_value, valueToNor);
    }
    Type fetchAndNorOrdered(Type valueToNor) noexcept {
        return Ops::fetchAndNorOrdered(_q_value, valueToNor);
    }

    //++a;
    Type operator++() noexcept {
        return fetchAndAddOrdered(1) + 1;
    }
    //++只能接受int操作类型 a++
    Type operator++(int) noexcept{
        return fetchAndAddOrdered(1) + 1;
    }
    Type operator--() noexcept {
        return fetchAndSubOrdered(1) - 1;
    }
    Type operator--(int) noexcept {
        return fetchAndSubOrdered(1) - 1;
    }
    Type operator+=(Type valueToAdd) {
        return fetchAndAddOrdered(valueToAdd) + valueToAdd;
    }
    Type operator-=(Type valueToSub) {
        return fetchAndSubOrdered(valueToSub) - valueToSub;
    }
    Type operator &=(Type valueToAnd) {
        return fetchAndAndOrdered(valueToAnd) & valueToAnd;
    }
    Type operator |=(Type valueToOr) {
        return fetchAndOrOrdered(valueToOr) | valueToOr;
    }
    Type operator ^=(Type valueToNor) {
        return fetchAndNorOrdered(valueToNor) ^ valueToNor;
    }
};

typedef QBasicAtomicInteger<int> QBasicAtomicInt;

//指针类型
template <typename X>
class QBasicAtomicPointer
{
public:
    typedef X *Type;
    typedef QAtomicOps<Type> Ops;
    typedef typename Ops::Type AtomicType;

    AtomicType _q_value;

    QBasicAtomicPointer() = default;
    constexpr QBasicAtomicPointer(Type value) noexcept : _q_value(value) {}
    QBasicAtomicPointer(const QBasicAtomicPointer &) = delete;
    QBasicAtomicPointer &operator=(const QBasicAtomicPointer &) = delete;
    QBasicAtomicPointer &operator=(const QBasicAtomicPointer &) volatile = delete;

    Type loadRelaxed() const noexcept { return Ops::loadRelaxed(_q_value); }
    void storeRelaxed(Type newValue) noexcept { Ops::storeRelaxed(_q_value, newValue); }

    operator Type() const noexcept {return loadAcquire();}
    Type operator=(Type newValue) noexcept { storeRelease(newValue); return newValue;}

    Type loadAcquire() const noexcept {return Ops::loadAcquire(_q_value);}
    void storeRelease(Type newValue) noexcept {return Ops::storeRelease(_q_value, newValue);}

    static constexpr bool isTestAndSetNative() noexcept {return Ops::isTestAndSetNative();}
    static constexpr bool isTestAndSetWaitFree() noexcept {return Ops::isTestAndSetWaitFree();}

    bool testAndSetRelaxed(Type expectedValue, Type newValue) noexcept {
        return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue);
    }
    bool testAndSetAcquire(Type expectedValue, Type newValue) noexcept {
        return Ops::testAndSetAcquire(_q_value, expectedValue, newValue);
    }
    bool testAndSetRelease(Type expectedValue, Type newValue) noexcept {
        return Ops::testAndSetRelease(_q_value, expectedValue, newValue);
    }
    bool testAndSetOrdered(Type expectedValue, Type newValue) noexcept {
        return Ops::testAndSetOrdered(_q_value, expectedValue, newValue);
    }

    bool testAndSetRelaxed(Type expectedValue, Type newValue, Type &currentValue) noexcept {
        return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue, &currentValue);
    }
    bool testAndSetAcquire(Type expectedValue, Type newValue, Type &currentValue) noexcept {
        return Ops::testAndSetAcquire(_q_value, expectedValue, newValue, &currentValue);
    }
    bool testAndSetRelease(Type expectedValue, Type newValue, Type &currentValue) noexcept {
        return Ops::testAndSetRelease(_q_value, expectedValue, newValue, &currentValue);
    }
    bool testAndSetOrdered(Type expectedValue, Type newValue, Type &currentValue) noexcept {
        return Ops::testAndSetOrdered(_q_value, expectedValue, newValue, &currentValue);
    }

    static constexpr bool isFetchAndStoreNative() noexcept {return Ops::isFetchAndStoreNative();}
    static constexpr bool isFetchAndStoreWaitFree() noexcept {return Ops::isFetchAndStoreWaitFree();}

    Type fetchAndStoreRelaxed(Type newValue) noexcept {
        return Ops::fetchAndStoreRelaxed(_q_value, newValue);
    }
    Type fetchAndStoreAcquire(Type newValue) noexcept {
        return Ops::fetchAndStoreAcquire(_q_value, newValue);
    }
    Type fetchAndStoreRelease(Type newValue) noexcept {
        return Ops::fetchAndStoreRelease(_q_value, newValue);
    }
    Type fetchAndStoreOrdered(Type newValue) noexcept {
        return Ops::fetchAndStoreOrdered(_q_value, newValue);
    }

    static constexpr bool isFetchAndAddNative() noexcept {return Ops::isFetchAndAddNative();}
    static constexpr  bool isFetchAndAddWaitFree() noexcept {return Ops::isFetchAndAddWaitFree();}

    Type fetchAndAddRelaxed(qptrdiff valueToAdd) noexcept {
        return Ops::fetchAndAddRelaxed(_q_value, valueToAdd);
    }
    Type fetchAndAddAcquire(qptrdiff valueToAdd) noexcept {
        return Ops::fetchAndAddAcquire(_q_value, valueToAdd);
    }
    Type fetchAndAddRelease(qptrdiff valueToAdd) noexcept {
        return Ops::fetchAndAddRelease(_q_value, valueToAdd);
    }
    Type fetchAndAddOrdered(qptrdiff valueToAdd) noexcept {
        return Ops::fetchAndAddOrdered(_q_value, valueToAdd);
    }

    Type fetchAndSubRelaxed(qptrdiff valueToSub) noexcept {
        return Ops::fetchAndSubRelaxed(_q_value, valueToSub);
    }
    Type fetchAndSubAcquire(qptrdiff valueToSub) noexcept {
        return Ops::fetchAndSubAcquire(_q_value, valueToSub);
    }
    Type fetchAndSubRelease(qptrdiff valueToSub) noexcept {
        return Ops::fetchAndSubRelease(_q_value, valueToSub);
    }
    Type fetchAndSubOrdered(qptrdiff valueToSub) noexcept {
        return Ops::fetchAndSubOrdered(_q_value, valueToSub);
    }
    //指针类型禁用或 / 与 / 非
    Type operator++() noexcept {
        return fetchAndAddOrdered(1) + 1;
    }
    Type operator++(int) noexcept {
        return fetchAndAddOrdered(1) + 1;
    }
    Type operator--() noexcept {
        return fetchAndSubOrdered(1) - 1;
    }
    Type operator--(int) noexcept {
        return fetchAndSubOrdered(1) - 1;
    }
    Type operator +=(qptrdiff valueToAdd) noexcept {
        return fetchAndAddOrdered(valueToAdd) + valueToAdd;
    }
    Type operator -=(qptrdiff valueToSub) noexcept {
        return fetchAndSubOrdered(valueToSub) - valueToSub;
    }
};

QT_END_NAMESPACE

#endif //QBASICATOMIC_H