//
// Created by Yujie Zhao on 2023/3/1.
//
#include "qlatin1string.h"
#include "qstringview.h"
#include "qstringalgorithms.h"

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

QT_END_NAMESPACE