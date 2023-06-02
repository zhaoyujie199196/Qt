//
// Created by Yujie Zhao on 2023/5/23.
//

#ifndef QCONTAINERFWD_H
#define QCONTAINERFWD_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

template <typename T> class QList;
template <typename T, qsizetype Prealloc = 256> class QVarLengthArray;  //QVarLengthArray的前置声明

QT_END_NAMESPACE

#endif //QCONTAINERFWD_H
