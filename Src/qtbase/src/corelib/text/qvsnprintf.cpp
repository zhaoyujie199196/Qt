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

int qsnprintf(char *str, size_t n, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = qvsnprintf(str, n, fmt, ap);
    va_end(ap);
    return ret;
}

QT_END_NAMESPACE
