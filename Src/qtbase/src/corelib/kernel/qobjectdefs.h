//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QOBJECTDEFS_H
#define QOBJECTDEFS_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QByteArray;
struct QMetaObject {
    static QByteArray normalizedType(const char *type);
};

QT_END_NAMESPACE

#endif //QOBJECTDEFS_H
