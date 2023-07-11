//
// Created by Yujie Zhao on 2023/6/8.
//

#ifndef QSTRINGLITERAL_H
#define QSTRINGLITERAL_H

#include <QtCore/qarraydata.h>
#include <QtCore/qarraydatapointer.h>

QT_BEGIN_NAMESPACE

//u"" "abc"之类的字符串
#define QT_UNICODE_LITERAL(str) u"" str
using QStringPrivate = QArrayDataPointer<char16_t>;

namespace QtPrivate {
    template <qsizetype N>
    static QStringPrivate qMakeStringPrivate(const char16_t(&literal)[N])
    {
        auto str = const_cast<char16_t *>(literal);
        return { nullptr, str, N - 1 };
    }
}

#define QStringLiteral(str) \
    (QString(QtPrivate::qMakeStringPrivate(QT_UNICODE_LITERAL(str)))) \

QT_END_NAMESPACE

#endif //QSTRINGLITERAL_H
