//
// Created by Yujie Zhao on 2023/10/10.
//
#include "qmargins.h"

QT_BEGIN_NAMESPACE

QDebug operator<<(QDebug debug, const QMarginsF &) { return debug; }

QDataStream &operator<<(QDataStream &s, const QMargins &) {
    return s;
}

QDataStream &operator>>(QDataStream &s, QMargins &) {
    return s;
}

QDataStream &operator<<(QDataStream &s, const QMarginsF &) {
    Q_ASSERT(false); return s;
}

QDataStream &operator>>(QDataStream &s, QMarginsF &) {
    Q_ASSERT(false); return s;
}


QT_END_NAMESPACE
