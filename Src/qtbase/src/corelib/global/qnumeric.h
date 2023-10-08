//
// Created by Yujie Zhao on 2023/10/8.
//

#ifndef QNUMERIC_H
#define QNUMERIC_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

bool qFuzzyCompare(double p1, double p2)
{
    return (qAbs(p1 - p2) * 1000000000000. <= qMin(qAbs(p1), qAbs(p2)));
}

bool qFuzzyCompare(float p1, float p2)
{
    return (qAbs(p1 - p2) * 100000.f <= qMin(qAbs(p1), qAbs(p2)));
}

bool qFuzzyIsNull(double d)
{
    return qAbs(d) <= 0.000000000001;
}

bool qFuzzyIsNull(float f)
{
    return qAbs(f) <= 0.00001f;
}

QT_END_NAMESPACE

#endif //QNUMERIC_H
