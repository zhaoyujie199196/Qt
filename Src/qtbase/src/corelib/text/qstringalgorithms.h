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
    qsizetype qustrlen(const char16_t *str) noexcept;
    const char16_t *qustrchr(QStringView str, char16_t ch) noexcept;

    int compareStrings(QLatin1String lhs, QLatin1String rhs, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    int compareStrings(QStringView lhs, QStringView rhs, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    int compareStrings(QStringView lhs, QLatin1String rhs, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    int compareStrings(QLatin1String lhs, QStringView rhs, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;

    bool equalStrings(QStringView lhs, QStringView rhs) noexcept;
    bool equalStrings(QStringView   lhs, QLatin1String rhs) noexcept;
//    bool equalStrings(QStringView   lhs, QBasicUtf8StringView<false> rhs) noexcept;
    bool equalStrings(QLatin1String lhs, QStringView   rhs) noexcept;
    bool equalStrings(QLatin1String lhs, QLatin1String rhs) noexcept;
//    bool equalStrings(QLatin1String lhs, QBasicUtf8StringView<false> rhs) noexcept;
//    bool equalStrings(QBasicUtf8StringView<false> lhs, QStringView   rhs) noexcept;
//    bool equalStrings(QBasicUtf8StringView<false> lhs, QLatin1String rhs) noexcept;
//    bool equalStrings(QBasicUtf8StringView<false> lhs, QBasicUtf8StringView<false> rhs) noexcept;

    bool startsWith(QStringView haystack, QStringView needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    bool startsWith(QStringView hayStack, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    bool startsWith(QLatin1String haystack, QStringView needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    bool startsWith(QLatin1String haystack, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;

    bool endsWith(QStringView haystack, QStringView needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    bool endsWith(QStringView hayStack, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    bool endsWith(QLatin1String haystack, QStringView needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    bool endsWith(QLatin1String haystack, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;

    qsizetype findString(QStringView haystack, qsizetype from, QStringView needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    qsizetype findString(QStringView haystack, qsizetype from, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    qsizetype findString(QLatin1String haystack, qsizetype from, QStringView needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    qsizetype findString(QLatin1String haystack, qsizetype from, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseInsensitive) noexcept;

    qsizetype lastIndexOf(QStringView haystack, qsizetype from, QStringView needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    qsizetype lastIndexOf(QStringView haystack, qsizetype from, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    qsizetype lastIndexOf(QLatin1String haystack, qsizetype from, QStringView needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    qsizetype lastIndexOf(QLatin1String haystack, qsizetype from, QLatin1String needle, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;


    bool isLatin1(QLatin1String s) noexcept;
    bool isLatin1(QStringView s) noexcept;

    QLatin1String trimmed(QLatin1String s) noexcept;
}

QT_END_NAMESPACE

#endif //QSTRINGALGORITHMS_H
