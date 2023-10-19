//
// Created by Yujie Zhao on 2023/10/17.
//

#ifndef QRANDOM_P_H
#define QRANDOM_P_H

#include <QtCore/private/qglobal_p.h>
#include "qrandom.h"

QT_BEGIN_NAMESPACE

//生成初始化的随即值
QRandomGenerator::InitialRandomData qt_initial_random_value() noexcept;

QT_END_NAMESPACE

#endif //QRANDOM_P_H
