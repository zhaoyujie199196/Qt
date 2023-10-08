//
// Created by Yujie Zhao on 2023/10/8.
//

#ifndef QNUMERIC_H
#define QNUMERIC_H

#include <QtCore/qglobal.h>
#include <cmath>

QT_BEGIN_NAMESPACE

static bool qFuzzyCompare(double p1, double p2)
{
    return (qAbs(p1 - p2) * 1000000000000. <= qMin(qAbs(p1), qAbs(p2)));
}

static bool qFuzzyCompare(float p1, float p2)
{
    return (qAbs(p1 - p2) * 100000.f <= qMin(qAbs(p1), qAbs(p2)));
}

static bool qFuzzyIsNull(double d)
{
    return qAbs(d) <= 0.000000000001;
}

static bool qFuzzyIsNull(float f)
{
    return qAbs(f) <= 0.00001f;
}

QT_WARNING_PUSH
QT_WARNING_DISABLE_FLOAT_COMPARE

[[nodiscard]] constexpr bool qIsNull(double d) noexcept
{
    return d == 0.0;
}

[[nodiscard]] constexpr bool qIsNull(float f) noexcept
{
    return f == 0.0f;
}
QT_WARNING_POP

[[nodiscard]] constexpr bool qIsFinite(double d) noexcept
{
    return std::isfinite(d);
}

[[nodiscard]] constexpr bool qIsFinite(float f) noexcept
{
    return std::isfinite(f);
}

QT_END_NAMESPACE

#endif //QNUMERIC_H
