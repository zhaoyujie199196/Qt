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

[[nodiscard]] constexpr inline bool isAsciiDigit(char32_t c) noexcept
{
    return c >= '0' && c <= '9';
}

constexpr inline bool isAsciiUpper(char32_t c) noexcept
{
    return c >= 'A' && c <= 'Z';
}

constexpr inline bool isAsciiLower(char32_t c) noexcept
{
    return c >= 'a' && c <= 'z';
}

constexpr inline bool isAsciiLetterOrNumber(char32_t c) noexcept
{
    return  isAsciiDigit(c) || isAsciiLower(c) || isAsciiUpper(c);
}

QT_END_NAMESPACE

#endif //QTOOLS_P_H
