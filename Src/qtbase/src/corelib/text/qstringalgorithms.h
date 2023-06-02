//
// Created by Yujie Zhao on 2023/2/28.
//

#ifndef QSTRINGALGORITHMS_H
#define QSTRINGALGORITHMS_H

#include <QtCore/qnamespace.h>

QT_BEGIN_NAMESPACE

class QLatin1String;
class QStringView;

constexpr int lencmp(qsizetype lhs, qsizetype rhs) noexcept;

namespace QtPrivate {
    int compareStrings(QLatin1String lhs, QLatin1String rhs, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    int compareStrings(QStringView lhs, QStringView rhs, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    int compareStrings(QStringView lhs, QLatin1String rhs, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    int compareStrings(QLatin1String lhs, QStringView rhs, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;

    bool startsWith(QLatin1String haystack, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
}

QT_END_NAMESPACE

#endif //QSTRINGALGORITHMS_H
