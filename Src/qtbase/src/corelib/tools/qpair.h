//
// Created by Yujie Zhao on 2023/3/6.
//

#ifndef QPAIR_H
#define QPAIR_H

#include <QtCore/qglobal.h>
#include <QtCore/QMetaType>

QT_BEGIN_NAMESPACE

template <typename T1, typename T2>
using QPair = std::pair<T1, T2>;

template <typename T1, typename T2>
constexpr decltype(auto) qMakePair(T1 &&value1, T2 &&value2) noexcept
{
    return std::make_pair(std::forward<T1>(value1), std::forward<T2>(value2));
}

template <class T1, class T2>
class QTypeInfo<std::pair<T1, T2>> : public QTypeInfoMerger<std::pair<T1, T2>, T1, T2>
{
};

QT_END_NAMESPACE

#endif //QPAIR_H
