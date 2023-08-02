//
// Created by Yujie Zhao on 2023/7/21.
//

#include "qendian.h"

QT_BEGIN_NAMESPACE

template <typename T>
static inline size_t simdSwapLoop(const uchar *, size_t, uchar *) noexcept {
    return 0;
}

template <typename T>
static inline void *bswapLoop(const uchar *src, size_t n, uchar *dst) noexcept {
    if (src != dst) {
        quintptr s = quintptr(src);
        quintptr d = quintptr(dst);
        //dst不在源数据中
        if (s < d) {
            Q_ASSERT( s + n <= d);
        }
        else {
            Q_ASSERT(d + n <= s);
        }
    }
    size_t i = simdSwapLoop<T>(src, n, dst);
    for (; i < n; i += sizeof(T)) {
        qbswap(qFromUnaligned<T>(src + i), dst + i);
    }
    return dst + i;
}

template<>
void *qbswap<1>(const void *source, qsizetype count, void *dest) noexcept
{
    return source != dest ? memcpy(dest, source, size_t(count)) : dest;
}

template <>
void *qbswap<2>(const void *source, qsizetype n, void *dest) noexcept {
    const uchar *src = reinterpret_cast<const uchar *>(source);
    uchar *dst = reinterpret_cast<uchar *>(dest);
    return bswapLoop<quint16>(src, n << 1, dst);
}

template <>
void *qbswap<4>(const void *source, qsizetype n, void *dest) noexcept {
    const uchar *src = reinterpret_cast<const uchar *>(source);
    uchar *dst = reinterpret_cast<uchar *>(dest);
    return bswapLoop<quint32>(src, n << 2, dst);
}

template <>
void *qbswap<8>(const void *source, qsizetype n, void *dest) noexcept {
    const uchar *src = reinterpret_cast<const uchar *>(source);
    uchar *dst = reinterpret_cast<uchar *>(dest);

    return bswapLoop<quint64>(src, n << 3, dst);
}

QT_END_NAMESPACE