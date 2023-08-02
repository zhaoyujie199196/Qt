//
// Created by Yujie Zhao on 2023/3/1.
//

#include "qstringview.h"
#include "qstring.h"

QT_BEGIN_NAMESPACE

QString QStringView::toString() const
{
    Q_ASSERT(size() == length());
    return QString(data(), length());
}

bool QStringView::startsWith(QStringView s, Qt::CaseSensitivity cs) const noexcept
{
    return QtPrivate::startsWith(*this, s, cs);
}

bool QStringView::startsWith(QLatin1String s, Qt::CaseSensitivity cs) const noexcept
{
    return QtPrivate::startsWith(*this, s, cs);
}

bool QStringView::endsWith(QStringView s, Qt::CaseSensitivity cs) const noexcept
{
    return QtPrivate::endsWith(*this, s, cs);
}

bool QStringView::endsWith(QLatin1String s, Qt::CaseSensitivity cs) const noexcept
{
    return QtPrivate::endsWith(*this, s, cs);
}

inline int QStringView::compare(QLatin1String s, Qt::CaseSensitivity cs) const noexcept {
    return QtPrivate::compareStrings(*this, s, cs);
}

QT_END_NAMESPACE