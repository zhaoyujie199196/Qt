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

QT_END_NAMESPACE

#endif //QALGORITHMS_H
