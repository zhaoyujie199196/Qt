//
// Created by Yujie Zhao on 2023/3/9.
//

#ifndef QMATH_H
#define QMATH_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
    constexpr inline quint32 qConstexprNextPowerOfTwo(quint32 v) {
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        ++v;
        return v;
    }

    constexpr inline quint64 qConstexprNextPowerOfTwo(quint64 v) {
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v |= v >> 32;
        ++v;
        return v;
    }

    constexpr inline quint32 qConstexprNextPowerOfTwo(qint32 v) {
        return qConstexprNextPowerOfTwo(quint32(v));
    }

    constexpr inline quint64 qConstexprNextPowerOfTwo(qint64 v) {
        return qConstexprNextPowerOfTwo(quint64(v));
    }
}

constexpr quint32 qNextPowerOfTwo(quint32 v)
{
    return QtPrivate::qConstexprNextPowerOfTwo(v);
}

constexpr inline quint64 qNextPowerOfTwo(quint64 v)
{
    return QtPrivate::qConstexprNextPowerOfTwo(v);
}

constexpr inline quint32 qNextPowerOfTwo(qint32 v)
{
    return qNextPowerOfTwo(quint32(v));
}

constexpr inline quint64 qNextPowerOfTwo(qint64 v)
{
    return qNextPowerOfTwo(quint64(v));
}

QT_END_NAMESPACE

#endif //QMATH_H
