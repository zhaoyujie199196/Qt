#include "QtCore/qglobal.h"
#include <limits>

QT_BEGIN_NAMESPACE

//overflow时返回true
template <typename T>
inline typename std::enable_if_t<std::is_unsigned_v<T>, bool> qAddOverflow(T v1, T v2, T *r)
{
    *r = v1 + v2;
    return v1 > *r;
}

template <typename T>
inline typename std::enable_if_t<std::is_signed_v<T>, bool> qAddOverflow(T v1, T v2, T *r)
{
    using U = typename std::make_unsigned_t<T>;
    *r = T(U(v1) + U(v2));

//    if (std::is_same_v<int32_t, int>)
//    {
//        return ((v1 ^ *r) & (v2 ^ *r)) < 0;
//    }
    bool s1 = (v1 < 0);
    bool s2 = (v2 < 0);
    bool sr = (*r < 0);
    return s1 != sr && s2 != sr;
}

template <typename T>
inline typename std::enable_if_t<std::is_unsigned_v<T>, bool> qSubOverflow(T v1, T v2, T *r)
{
    *r = v1 - v2;
    return v1 < v2;
}

template <typename T>
inline typename std::enable_if_t<std::is_signed_v<T>, bool> qSubOverflow(T v1, T v2, T *r)
{
    //同号相减不会发生溢出
    //异号相减才会发生溢出
    using U = typename std::make_unsigned_t<T>;
    *r = T(U(v1) - U(v2));

//    if (std::is_same_v<int32_t, int>) {
//        return ((v2 ^ *r) & (~v2 ^ *r)) < 0;
//    }
    bool s1 = (v1 < 0);
    bool s2 = !(v2 < 0);
    bool sr = (*r < 0);
    return s1 != sr && s2 != sr;
}

template <typename T>
inline typename std::enable_if_t<std::is_unsigned_v<T> || std::is_signed_v<T>, bool> qMulOverflow(T v1, T v2, T *r)
{
    //只支持64位及以下，128位编译会报错
    using LargerInt = QIntegerForSize<sizeof(T) * 2>;
    using Larger = typename std::conditional_t<std::is_signed_v<T>, typename LargerInt::Signed, typename LargerInt::Unsigned>;
    Larger lr = Larger(v1) * Larger(v2);
    *r = T(lr);
    return lr > std::numeric_limits<T>::max() || lr < std::numeric_limits<T>::min();
}

template <typename T>
inline bool add_overflow(T v1, T v2, T *r) { return qAddOverflow(v1, v2, r); }
template <typename T>
inline bool sub_overflow(T v1, T v2, T *r) { return qSubOverflow(v1, v2, r); }
template <typename T>
inline bool mul_overflow(T v1, T v2, T *r) { return qMulOverflow(v1, v2, r); }

QT_END_NAMESPACE