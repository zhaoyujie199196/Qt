//
// Created by Yujie Zhao on 2023/10/7.
//

#ifndef QGLOBALSTATIC_H
#define QGLOBALSTATIC_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

#define Q_GLOBAL_STATIC(TYPE, NAME) \
static TYPE *NAME() {               \
    static TYPE type;               \
    return &type;\
}

QT_END_NAMESPACE

#endif //QGLOBALSTATIC_H
