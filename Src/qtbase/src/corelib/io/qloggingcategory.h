//
// Created by Yujie Zhao on 2023/11/20.
//

#ifndef QLOGGINGCATEGORY_H
#define QLOGGINGCATEGORY_H

#include <QtCore/qglobal.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

#define Q_LOGGING_CATEGORY(name, ...)

#define qCWarning(category, ...)

QT_END_NAMESPACE

#endif //QLOGGINGCATEGORY_H
