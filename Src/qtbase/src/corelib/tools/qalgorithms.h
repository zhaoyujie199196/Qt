//
// Created by Yujie Zhao on 2023/9/7.
//

#ifndef QALGORITHMS_H
#define QALGORITHMS_H

#include <QtCore/QtGlobal>
#include <bit>

QT_BEGIN_NAMESPACE

inline uint qPopulationCount(quint32 v) noexcept {
    return std::popcount(v);
}

inline uint qPopulationCount(quint8 v) noexcept {
    return std::popcount(v);
}

inline uint qPopulationCount(quint16 v) noexcept {
    return std::popcount(v);
}

inline uint qPopulationCount(quint64 v) noexcept {
    return std::popcount(v);
}

inline uint qPopulationCount(long unsigned int v) noexcept {
    return qPopulationCount(static_cast<quint64>(v));
}

namespace QtPrivate {
    constexpr inline uint qConstexprCountTrailingZeroBits(quint32 v) noexcept
    {
        // see http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightParallel
        unsigned int c = 32; // c will be the number of zero bits on the right
        v &= -signed(v);
        if (v) c--;
        if (v & 0x0000FFFF) c -= 16;
        if (v & 0x00FF00FF) c -= 8;
        if (v & 0x0F0F0F0F) c -= 4;
        if (v & 0x33333333) c -= 2;
        if (v & 0x55555555) c -= 1;
        return c;
    }

    constexpr inline uint qConstexprCountTrailingZeroBits(quint64 v) noexcept
    {
        quint32 x = static_cast<quint32>(v);
        return x ? qConstexprCountTrailingZeroBits(x)
                 : 32 + qConstexprCountTrailingZeroBits(static_cast<quint32>(v >> 32));
    }

    constexpr inline uint qConstexprCountTrailingZeroBits(quint8 v) noexcept
    {
        unsigned int c = 8; // c will be the number of zero bits on the right
        v &= -signed(v);
        if (v) c--;
        if (v & 0x0000000F) c -= 4;
        if (v & 0x00000033) c -= 2;
        if (v & 0x00000055) c -= 1;
        return c;
    }

    constexpr inline uint qConstexprCountTrailingZeroBits(quint16 v) noexcept
    {
        unsigned int c = 16; // c will be the number of zero bits on the right
        v &= -signed(v);
        if (v) c--;
        if (v & 0x000000FF) c -= 8;
        if (v & 0x00000F0F) c -= 4;
        if (v & 0x00003333) c -= 2;
        if (v & 0x00005555) c -= 1;
        return c;
    }

    constexpr inline uint qConstexprCountTrailingZeroBits(unsigned long v) noexcept
    {
        return qConstexprCountTrailingZeroBits(QIntegerForSizeof<long>::Unsigned(v));
    }
}

QT_END_NAMESPACE

#endif //QALGORITHMS_H
