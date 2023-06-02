//
// Created by Yujie Zhao on 2023/3/9.
//

#ifndef QTOOLS_P_H
#define QTOOLS_P_H

#include "QtCore/qglobal.h"

QT_BEGIN_NAMESPACE

struct CalculateGrowingBlockSizeResult
{
    qsizetype size;
    qsizetype elementCount;
};

qsizetype Q_DECL_CONST_FUNCTION qCalculateBlockSize(qsizetype elementCount, qsizetype elementSize, qsizetype headerSize = 0) noexcept;
CalculateGrowingBlockSizeResult Q_DECL_CONST_FUNCTION qCalculateGrowingBlockSize(qsizetype elementCount, qsizetype elementSize, qsizetype headerSize = 0) noexcept;

QT_END_NAMESPACE

#endif //QTOOLS_P_H
