//
// Created by Yujie Zhao on 2023/2/28.
//

#include <stdio.h>
#include "qbytearrayalgorithms.h"

QT_BEGIN_NAMESPACE

int qvsnprintf(char *str, size_t n, const char *fmt, va_list ap)
{
    return ::vsnprintf(str, n, fmt, ap);
}

QT_END_NAMESPACE
