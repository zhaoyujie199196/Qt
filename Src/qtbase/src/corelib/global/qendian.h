//
// Created by Yujie Zhao on 2023/7/21.
//

#ifndef QENDIAN_H
#define QENDIAN_H

#include <QtCore/qglobal.h>
#include <QtCore/qfloat16.h>
#include <limits>
#include <stdlib.h>
#include <string>

//MSVC中的一些宏  zhaoyujie TODO 放在.h文件里好吗？
#ifdef min
#undef min
#undef max
#endif

QT_BEGIN_NAMESPACE

//zhaoyujie TODO 这两个函数有什么存在的必要？
template <typename T>
inline void qToUnaligned(const T src, void *dest)
{
    const size_t size = sizeof(T);
    memcpy(dest, &src, size);
}
template <typename T>
inline T qFromUnaligned(const void *src) {
    T dest;
    const size_t size = sizeof(T);
    memcpy(&dest, src, size);
    return dest;
}

//存储反转函数 接受的参数只能是unsigned
inline constexpr quint64 qbswap_helper(quint64 source)
{
    return 0
           | ((source & Q_UINT64_C(0x00000000000000ff)) << 56)
           | ((source & Q_UINT64_C(0x000000000000ff00)) << 40)
           | ((source & Q_UINT64_C(0x0000000000ff0000)) << 24)
           | ((source & Q_UINT64_C(0x00000000ff000000)) << 8)
           | ((source & Q_UINT64_C(0x000000ff00000000)) >> 8)
           | ((source & Q_UINT64_C(0x0000ff0000000000)) >> 24)
           | ((source & Q_UINT64_C(0x00ff000000000000)) >> 40)
           | ((source & Q_UINT64_C(0xff00000000000000)) >> 56);
}

inline constexpr quint32 qbswap_helper(quint32 source)
{
    return 0
           | ((source & 0x000000ff) << 24)
           | ((source & 0x0000ff00) << 8)
           | ((source & 0x00ff0000) >> 8)
           | ((source & 0xff000000) >> 24);
}

inline constexpr quint16 qbswap_helper(quint16 source)
{
    return quint16( 0
                    | ((source & 0x00ff) << 8)
                    | ((source & 0xff00) >> 8) );
}

inline constexpr quint8 qbswap_helper(quint8 source)
{
    return source;
}

//整数swap
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
inline constexpr T qbswap(T source)
{
    return T(qbswap_helper(typename QIntegerForSizeof<T>::Unsigned(source)));
}

template <typename Float>
Float qbswapFloatHelper(Float source)
{
    //将Float的内存解释为整数内存， 先取size = sizeof(Float),根据size便特化不同的int
    auto temp = qFromUnaligned<typename QIntegerForSizeof<Float>::Unsigned>(&source);
    temp = qbswap(temp);
    return qFromUnaligned<Float>(&temp);
}

//inline qfloat16 qbswap(qfloat16 source)
//{
//    return qbswapFloatHelper(source);
//}

inline float qbswap(float source)
{
    return qbswapFloatHelper(source);
}

inline double qbswap(double source)
{
    return qbswapFloatHelper(source);
}

template <typename T> inline void qbswap(const T src, void *dest)
{
    qToUnaligned<T>(qbswap(src), dest);
}

template <int Size>
void *qbswap(const void *source, qsizetype count, void *dest) noexcept;
//1个字节就是自身
template <>
void *qbswap<1>(const void *source, qsizetype count, void *dest) noexcept;
template <>
void *qbswap<2>(const void *source, qsizetype count, void *dest) noexcept;
template <>
void *qbswap<4>(const void *source, qsizetype count, void *dest) noexcept;
template <>
void *qbswap<8>(const void *source, qsizetype count, void *dest) noexcept;

#if Q_BYTE_ORDER == Q_BIG_ENDIAN

template <typename T>
inline constexpr T qToBigEndian(T source) {
    return source;
}
template <typename T>
inline constexpr T qFromBigEndian(T source) {
    return source;
}
template <typename T>
inline constexpr T qToLittleEndian(T source) {
    return qbswap(source);
}
template <typename T>
inline constexpr T qFromLittleEndian(T source) {
    return qbswap(source);
}
template <typename T>
inline void qToBigEndian(T src, void *dest) {
    qToUnaligned<T>(src, dest);
}
template <typename T>
inline void qToLittleEndian(T src, void *dest) {
    qbswap<T>(src, dest);
}

template <typename T>
inline void qToBigEndian(const void *source, qsizetype count, void *dest) {
    if (source != dest) {
        memcpy(dest, source, count * sizeof(T));
    }
}
template <typename T>
inline void qToLittleEndian(const void *source, qsizetype count, void *dest) {
    qbswap<sizeof(T)>(source, count, dest);
}
template <typename T>
inline void qFromBigEndian(const void *source, qsizetype count, void *dest) {
    if (source != dest) {
        memcpy(dest, source, count * sizeof(T));
    }
}
template <typename T>
inline void qFromLittleEndian(const void *source, qsizetype count, void *dest) {
    qbswap<sizeof(T)>(source, count, dest);
}

#else

template <typename T>
inline constexpr T qToBigEndian(T source) {
    return qbswap(source);
}

template <typename T>
inline constexpr T qFromBigEndian(T source) {
    return qbswap(source);
}

template <typename T>
inline constexpr T qToLittleEndian(T source) {
    return source;
}

template <typename T>
inline constexpr T qFromLittleEndian(T source) {
    return source;
}

template <typename T>
inline void qToBigEndian(T src, void *dest) {
    qbswap<T>(src, dest);
}

template <typename T>
inline void qToLittleEndian(T src, void *dest) {
    qToUnaligned<T>(src, dest);
}

template <typename T>
inline void qToBigEndian(const void *source, qsizetype count, void *dest) {
    qbswap<sizeof(T)>(source, count, dest);
}

template <typename T>
inline void qToLittleEndian(const void *source, qsizetype count, void *dest) {
    if (source != dest) {
        memcpy(dest, source, count * sizeof(T));
    }
}

template <typename T>
inline void qFromBigEndian(const void *source, qsizetype count, void *dest) {
    qbswap<sizeof(T)>(source, count, dest);
}

template <typename T>
inline void qFromLittleEndian(const void *source, qsizetype count, void *dest) {
    if (source != dest) {
        memcpy(dest, source, count * sizeof(T));
    }
}

#endif

template <typename T>
inline T qFromLittleEndian(const void *src) {
    return qFromLittleEndian(qFromUnaligned<T>(src));
}
template <>
inline quint8 qFromLittleEndian<quint8>(const void *src) {
    return static_cast<const quint8 *>(src)[0];
}
template <>
inline qint8 qFromLittleEndian<qint8>(const void *src) {
    return static_cast<const qint8 *>(src)[0];
}

template <class T>
inline T qFromBigEndian(const void *src) {
    return qFromBigEndian(qFromUnaligned<T>(src));
}
template <>
inline quint8 qFromBigEndian<quint8>(const void *src) {
    return static_cast<const quint8 *>(src)[0];
}
template <>
inline qint8 qFromBigEndian<qint8>(const void *src) {
    return static_cast<const qint8 *>(src)[0];
}

//zhaoyujie TODO
//QSpecialInteger

QT_END_NAMESPACE

#endif //QENDIAN_H
