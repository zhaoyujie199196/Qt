//
// Created by Yujie Zhao on 2023/6/1.
//
#include "qobjectdefs.h"
#include <QtCore/qbytearray.h>

QT_BEGIN_NAMESPACE

static QByteArray normalizeTypeInternal(const char *t, const char *e)
{
    Q_ASSERT(false);
    return QByteArray();
//    int len = QtPrivate::qNormalizeType(t, e, nullptr);
//    if (len == 0) {
//        return QByteArray();
//    }
//    QByteArray result(len, Qt::Uninitialized);
//    len = QtPrivate::qNormalizeType(t, e, result.data());
//    Q_ASSERT(len == result.size());
//    return result;
}

QByteArray QMetaObject::normalizedType(const char *type)
{
    Q_ASSERT(false);  //zhaoyujie TODO
    return normalizeTypeInternal(type, type + qstrlen(type));
}

QT_END_NAMESPACE
