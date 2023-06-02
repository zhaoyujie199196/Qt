//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QBYTEARRAYALGORITHMS_H
#define QBYTEARRAYALGORITHMS_H

#include <QtCore/qglobal.h>
#include <QtCore/qnamespace.h>
#include <stdarg.h>
#include <string>

QT_BEGIN_NAMESPACE

class QByteArrayView;

//复制char *数组，会开辟新的空间
char *qstrdup(const char *);

//拷贝src到dst
char *qstrcpy(char *dst, const char *src);

//格式化字符串
int qvsnprintf(char *str, size_t n, const char *fmt, va_list ap);
int qsnprintf(char *str, size_t n, const char *fmt, ...);
inline size_t qstrlen(const char *str) { return str ? strlen(str) : 0; }
inline size_t qstrnlen(const char *str, size_t maxLen) {
    size_t length = 0;
    if (str) {
        while (length < maxLen && *str) {
            length++;
            str++;
        }
    }
    return length;
}

//大小写不敏感的字符串比较
int qstricmp(const char *, const char *);
int qstrnicmp(const char *, const char *, size_t len);
int qstrnicmp(const char *, qsizetype, const char *, qsizetype = -1);

int qstrcmp(const char *str1, const char *str2);
int qstrncmp(const char *str1, const char *str2, size_t len) noexcept;

//zhaoyujie TODO CRC16循环冗余校验 https://blog.csdn.net/weixin_38253113/article/details/81267881
quint16 qChecksum(QByteArrayView data, Qt::ChecksumType standard = Qt::ChecksumIso3309);

namespace QtPrivate
{
    bool startsWith(QByteArrayView haystack, QByteArrayView needle) noexcept;
    bool endsWith(QByteArrayView haystack, QByteArrayView needle) noexcept;
    qsizetype findByteArray(QByteArrayView haystack, qsizetype from, QByteArrayView needle) noexcept;
    qsizetype lastIndexOf(QByteArrayView haystack, qsizetype from, QByteArrayView needle) noexcept;
    qsizetype count(QByteArrayView haystack, QByteArrayView needle) noexcept;
    int compareMemory(QByteArrayView lhs, QByteArrayView rhs);
}

QT_END_NAMESPACE

#endif //QBYTEARRAYALGORITHMS_H
