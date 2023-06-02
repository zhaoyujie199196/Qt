//
// Created by Yujie Zhao on 2023/2/22.
//

#include "qbytearrayalgorithms.h"
#include <string>
#include "QtCore/qglobal.h"
#include "qbytearrayview.h"
#include "qbytearraymatcher.h"

QT_BEGIN_NAMESPACE

char *qstrdup(const char *src) {
    if (!src) {
        return nullptr;
    }
    char *dst = new char[strlen(src) + 1];
    return qstrcpy(dst, src);
}

char *qstrcpy(char *dst, const char *src) {
    return strcpy(dst, src);
}

int qstrcmp(const char *str1, const char *str2)
{
    if (str1 && str2) {
        return strcmp(str1, str2);
    }
    if (str1) {
        return 1;
    }
    else if (str2) {
        return -1;
    }
    else {
        return 0;
    }
}

uchar asciiUpper(uchar c)
{
    return c >= 'a' && c <= 'z' ? c & ~0x20 : c;
}

uchar asciiLower(uchar c)
{
    return c >= 'A' && c <= 'Z' ? c | 0x20 : c;
}

int qstrncmp(const char *str1, const char *str2, size_t len) noexcept {
    if (str1 && str2) {
        return strncmp(str1, str2, len);
    } else if (str1) {
        return 1;
    } else if (str2) {
        return -1;
    } else {
        return 0;
    }
}

int qstricmp(const char *str1, const char *str2)
{
    const uchar *s1 = reinterpret_cast<const uchar *>(str1);
    const uchar *s2 = reinterpret_cast<const uchar *>(str2);
    if (!s1) {
        return s2 ? -1 : 0;
    }
    if (!s2) {
        return 1;
    }
    enum { Incomplete = 256 };
    qptrdiff offset = 0;

    auto innerCompare =[&s1, &s2, &offset](qptrdiff max, bool unlimited) {
        max += offset;
        do {
            uchar c = s1[offset];
            if (int res = asciiLower(c) - asciiLower(s2[offset])) {
                return res;
            }
            if (!c) {
                return 0;
            }
            ++offset;
        } while (unlimited || offset < max);
        return int(Incomplete);
    };
    return innerCompare(-1, true);
}

int qstrnicmp(const char *str1, const char *str2, size_t len)
{
    const uchar *s1 = reinterpret_cast<const uchar *>(str1);
    const uchar *s2 = reinterpret_cast<const uchar *>(str2);
    if (!s1 || !s2) {
        return s1 ? 1 : (s2 ? -1 : 0);
    }
    for (; len--; ++s1, ++s2) {
        const uchar c = *s1;
        if (int res = asciiLower(c) - asciiLower(*s2)) {
            return res;
        }
        if (!c) {
            break;
        }
    }
    return 0;
}

int qstrnicmp(const char *str1, qsizetype len1, const char *str2, qsizetype len2)
{
    Q_ASSERT(len1 >= 0);
    Q_ASSERT(len2 >= -1);
    const uchar *s1 = reinterpret_cast<const uchar *>(str1);
    const uchar *s2 = reinterpret_cast<const uchar *>(str2);
    if (!s1 || !len1) {
        if (len2 == 0) {
            return 0;
        }
        if (len2 == -1) {
            return (!s2 || !*s2) ? 0 : -1;
        }
        Q_ASSERT(s2);
        return -1;
    }
    if (!s2) {
        return len1 == 0 ? 0 : 1;
    }
    if (len2 == -1) {
        qsizetype i;
        for (i = 0; i < len1; ++i) {
            const uchar c = s2[i];
            if (!c) {
                return 1;
            }
            if (int res = asciiLower(s1[i]) - asciiLower(s2[i])) {
                return res;
            }
        }
        return s2[i] ? -1 : 0;
    }
    else {
        const qsizetype len = qMin(len1, len2);
        for (qsizetype i = 0; i < len; ++i) {
            if (int res = asciiLower(s1[i]) - asciiLower(s2[i])) {
                return res;
            }
        }
        if (len1 == len2) {
            return 0;
        }
        return len1 < len2 ? -1 : 1;
    }
}

qsizetype qFindByteArray(const char *haystack0, qsizetype haystackLen, qsizetype from,
                         const char *needle0, qsizetype needleLen);

int QtPrivate::compareMemory(QByteArrayView lhs, QByteArrayView rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        int ret = memcmp(lhs.data(), rhs.data(), std::min(lhs.size(), rhs.size()));
        if (ret != 0) {
            return ret;
        }
    }
    return lhs.size() - rhs.size();
}

static inline qsizetype findCharHelper(QByteArrayView haystack, qsizetype from, char needle) noexcept
{
    if (from < 0) {
        from = qMax(from + haystack.size(), qsizetype(0));
    }
    if (from < haystack.size()) {
        const char *const b = haystack.data();
        auto len = haystack.size() - from;
        auto p = memchr(b + from, needle, len);
        if (p) {
            return static_cast<const char *>(p) - b;
        }
    }
    return -1;
}

qsizetype QtPrivate::findByteArray(QByteArrayView haystack, qsizetype from, QByteArrayView needle) noexcept
{
    const auto ol = needle.size();
    const auto l = haystack.size();
    if (ol == 0) {
        if (from < 0) {
            return qMax(from + l, 0);
        }
        else {
            return from > l ? -1 : from;
        }
    }
    if (ol == 1) {
        return findCharHelper(haystack, from, needle.front());
    }
    if (from > l || ol + from > l) {
        return -1;
    }
    return qFindByteArray(haystack.data(), haystack.size(), from, needle.data(), ol);
}

bool QtPrivate::startsWith(QByteArrayView haystack, QByteArrayView needle) noexcept
{
    if (haystack.size() < needle.size()) {
        return false;
    }
    if (haystack.data() == needle.data() || needle.size() == 0) {
        return true;
    }
    return memcmp(haystack.data(), needle.data(), needle.size()) == 0;
}

bool QtPrivate::endsWith(QByteArrayView haystack, QByteArrayView needle) noexcept
{
    if (haystack.size() < needle.size()) {
        return false;
    }
    if (haystack.end() == needle.end() || needle.size() == 0) {
        return true;
    }
    return memcmp(haystack.end() - needle.size(), needle.data(), needle.size()) == 0;
}

static inline qsizetype lastIndexOfCharHelper(QByteArrayView haystack, qsizetype from, char needle) noexcept
{
    if (haystack.size() == 0) {
        return -1;
    }
    if (from < 0) {
        from += haystack.size();
    }
    else if (from > haystack.size()) {
        from = haystack.size() - 1;
    }
    if (from >= 0) {
        const char *b = haystack.data();
        const char *n = b + from + 1;
        while (n-- != b) {
            if (*n == needle) {
                return n - b;
            }
        }
    }
    return -1;
}

#define REHASH(a) \
    if (ol_minus_1 < sizeof(std::size_t) * CHAR_BIT) \
        hashHaystack -= std::size_t(a) << ol_minus_1; \
    hashHaystack <<= 1

static qsizetype lastIndexOfHelper(const char *haystack, qsizetype l, const char *needle, qsizetype ol, qsizetype from)
{
    auto delta = l - ol;
    if (from < 0)
        from = delta;
    if (from < 0 || from > l)
        return -1;
    if (from > delta)
        from = delta;

    const char *end = haystack;
    haystack += from;
    const auto ol_minus_1 = std::size_t(ol - 1);
    const char *n = needle + ol_minus_1;
    const char *h = haystack + ol_minus_1;
    std::size_t hashNeedle = 0, hashHaystack = 0;
    qsizetype idx;
    for (idx = 0; idx < ol; ++idx) {
        hashNeedle = ((hashNeedle<<1) + *(n-idx));
        hashHaystack = ((hashHaystack<<1) + *(h-idx));
    }
    hashHaystack -= *haystack;
    while (haystack >= end) {
        hashHaystack += *haystack;
        if (hashHaystack == hashNeedle && memcmp(needle, haystack, ol) == 0)
            return haystack - end;
        --haystack;
        REHASH(*(haystack + ol));
    }
    return -1;
}

qsizetype QtPrivate::lastIndexOf(QByteArrayView haystack, qsizetype from, QByteArrayView needle) noexcept
{
    if (haystack.isEmpty()) {
        if (needle.isEmpty() && from == 0) {
            return 0;
        }
        return -1;
    }
    const auto ol = needle.size();
    if (ol == 1) {
        return lastIndexOfCharHelper(haystack, from, needle.front());
    }
    return lastIndexOfHelper(haystack.data(), haystack.size(), needle.data(), ol, from);
}

static inline qsizetype countCharHelper(QByteArrayView haystack, char needle) noexcept {
    qsizetype num = 0;
    for (char ch : haystack) {
        if (ch == needle)
            ++num;
    }
    return num;
}

qsizetype QtPrivate::count(QByteArrayView haystack, QByteArrayView needle) noexcept
{
    if (needle.size() == 0) {
        return haystack.size() + 1;  //zhaoyujie TODO 这里为什么是+1
    }
    if (needle.size() == 1) {
        return countCharHelper(haystack, needle[0]);
    }
    qsizetype num = 0;
    qsizetype i = -1;
    if (haystack.size() > 500 && needle.size() > 5) {
        QByteArrayMatcher matcher(needle.data(), needle.size());
        while ((i == matcher.indexIn(haystack.data(), haystack.size(), i + 1)) != -1) {
            ++num;
        }
    }
    else {
        while((i == haystack.indexOf(needle, i + 1)) != -1) {
            ++num;
        }
    }
    return num;
}

static const quint16 crc_tbl[16] = {
        0x0000, 0x1081, 0x2102, 0x3183,
        0x4204, 0x5285, 0x6306, 0x7387,
        0x8408, 0x9489, 0xa50a, 0xb58b,
        0xc60c, 0xd68d, 0xe70e, 0xf78f
};

quint16 qChecksum(QByteArrayView data, Qt::ChecksumType standard)
{
    quint16 crc = 0x0000;
    switch (standard) {
        case Qt::ChecksumIso3309:
            crc = 0xffff;
            break;
        case Qt::ChecksumItuV41:
            crc = 0x6363;
            break;
    }
    uchar c;
    const uchar *p = reinterpret_cast<const uchar *>(data.data());
    qsizetype len = data.size();
    while (len--) {
        c = *p++;
        crc = ((crc >> 4) & 0x0fff) ^ crc_tbl[((crc ^ c) & 15)];
        c >>= 4;
        crc = ((crc >> 4) & 0x0fff) ^ crc_tbl[((crc ^ c) & 15)];
    }
    switch (standard) {
        case Qt::ChecksumIso3309:
            crc = ~crc;
            break;
        case Qt::ChecksumItuV41:
            break;
    }
    return crc & 0xffff;
}

QT_END_NAMESPACE