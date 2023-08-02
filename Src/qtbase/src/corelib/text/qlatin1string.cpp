//
// Created by Yujie Zhao on 2023/3/1.
//
#include "qlatin1string.h"
#include "qstringview.h"
#include "qstringalgorithms.h"
#include "qstring.h"

QT_BEGIN_NAMESPACE

//比较
int QLatin1String::compare(QLatin1String other, Qt::CaseSensitivity cs) const noexcept {
    return QtPrivate::compareStrings(*this, other, cs);
}

constexpr int QLatin1String::compare(QChar c) const noexcept {
    if (isEmpty() || front() == c) {
        return size() - 1;
    }
    else {
        return uchar(m_data[0]) - c.unicode();
    }
}

int QLatin1String::compare(QChar c, Qt::CaseSensitivity cs) const noexcept {
    return QtPrivate::compareStrings(*this, QStringView(&c, 1), cs);
}

constexpr bool QLatin1String::startsWith(QChar c) const noexcept {
    return !isNull() && m_data[0] == c;
}

bool QLatin1String::startsWith(QChar c, Qt::CaseSensitivity cs) const noexcept {
    Q_ASSERT(false);
    return false;
}

//以xxx开头
bool QLatin1String::startsWith(QLatin1String s, Qt::CaseSensitivity cs) const noexcept {
    return QtPrivate::startsWith(*this, s, cs);
}

int QLatin1String::compare_helper(const QLatin1String &s1, const char *s2)
{
    return QString::compare(s1, QString::fromUtf8(s2));
}

int QLatin1String::compare_helper(const QChar *data1, qsizetype length1, QLatin1String s2,
                                  Qt::CaseSensitivity cs) noexcept
{
    Q_ASSERT(length1 >= 0);
    Q_ASSERT(data1 || length1 == 0);
    return QtPrivate::compareStrings(QStringView(data1, length1), s2, cs);
}

bool QLatin1String::operator==(const char *s) const {
    return QString::fromUtf8(s) == *this;
}

bool QLatin1String::operator!=(const char *s) const {
    return QString::fromUtf8(s) != *this;
}

bool QLatin1String::operator<(const char *s) const {
    return QString::fromUtf8(s) > *this;
}

bool QLatin1String::operator>(const char *s) const {
    return QString::fromUtf8(s) < *this;
}

bool QLatin1String::operator<=(const char *s) const {
    return !(QString::fromUtf8(s) < *this);
}

bool QLatin1String::operator>=(const char *s) const {
    return !(QString::fromUtf8(s) > *this);
}

bool QLatin1String::operator==(const QByteArray &s) const {
    return QString::fromUtf8(s) == *this;
}

bool QLatin1String::operator!=(const QByteArray &s) const {
    return QString::fromUtf8(s) != *this;
}

bool QLatin1String::operator<(const QByteArray &s) const {
    return QString::fromUtf8(s) > *this;
}

bool QLatin1String::operator>(const QByteArray &s) const {
    return QString::fromUtf8(s) < *this;
}

bool QLatin1String::operator<=(const QByteArray &s) const {
    return QString::fromUtf8(s) >= *this;
}

bool QLatin1String::operator>=(const QByteArray &s) const {
    return QString::fromUtf8(s) <= *this;
}

QT_END_NAMESPACE