//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QBYTEARRAYALGORITHMS_H
#define QBYTEARRAYALGORITHMS_H

#include <QtCore/qglobal.h>
#include <stdarg.h>

QT_BEGIN_NAMESPACE

//复制char *数组，会开辟新的空间
char *qstrdup(const char *);

//拷贝src到dst
char *qstrcpy(char *dst, const char *src);

//格式化字符串
int qvsnprintf(char *str, size_t n, const char *fmt, va_list ap);

QT_END_NAMESPACE

#endif //QBYTEARRAYALGORITHMS_H
