//
// Created by Yujie Zhao on 2023/4/19.
//
#include "qbytearraymatcher.h"
#include "QtCore/qglobal.h"
#include <string.h>

QT_BEGIN_NAMESPACE

// boyer Moore算法，字符串快速匹配
// boyer Moore
// https://zhuanlan.zhihu.com/p/530369703
// https://blog.csdn.net/weixin_45573560/article/details/107926512?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0-107926512-blog-50738770.235^v32^pc_relevant_default_base3&spm=1001.2101.3001.4242.1&utm_relevant_index=3
// KMP
// https://blog.csdn.net/starstar1992/article/details/54913261

static inline void bm_init_skiptable(const uchar *cc, qsizetype len, uchar *skiptable)
{
    int l = int(qMin(len, qsizetype(255)));
    memset(skiptable, l, 255 * sizeof(uchar));
    cc += len - l;
    while (l--) {
        skiptable[*cc++] = l;
    }
}

static inline qsizetype bm_find(const uchar *cc, qsizetype l, qsizetype index,
                                const uchar *puc, qsizetype pl, const uchar *skiptable) {
    if (pl == 0)
        return index > l ? -1 : index;
    const qsizetype pl_minus_one = pl - 1;

    const uchar *current = cc + index + pl_minus_one;
    const uchar *end = cc + l;
    while (current < end) {
        qsizetype skip = skiptable[*current];
        if (!skip) {
            // possible match
            while (skip < pl) {
                if (*(current - skip) != puc[pl_minus_one - skip])
                    break;
                skip++;
            }
            if (skip > pl_minus_one) // we have a match
                return (current - cc) - skip + 1;

            // in case we don't have a match we are a bit inefficient as we only skip by one
            // when we have the non matching char in the string.
            if (skiptable[*(current - skip)] == pl)
                skip = pl - skip;
            else
                skip = 1;
        }
        if (current > end - skip)
            break;
        current += skip;
    }
    return -1; // not found
}

QByteArrayMatcher::QByteArrayMatcher()
{
    p.p = nullptr;
    p.l = 0;
    memset(p.q_skiptable, 0, sizeof(p.q_skiptable));
}

QByteArrayMatcher::QByteArrayMatcher(const char *pattern, qsizetype length)
{
    p.p = reinterpret_cast<const uchar *>(pattern);
    p.l = length;
    bm_init_skiptable(p.p, p.l, p.q_skiptable);
}

QByteArrayMatcher::QByteArrayMatcher(const QByteArray &pattern)
    : q_pattern(pattern)
{
    p.p = reinterpret_cast<const uchar *>(pattern.constData());
    p.l = pattern.size();
    bm_init_skiptable(p.p, p.l, p.q_skiptable);
}

QByteArrayMatcher::QByteArrayMatcher(const QByteArrayMatcher &other)
{
    //原来可以这样调用。。。
    operator=(other);
}

QByteArrayMatcher::~QByteArrayMatcher()
{

}

QByteArrayMatcher &QByteArrayMatcher::operator=(const QByteArrayMatcher &other) {
    q_pattern = other.q_pattern;
    memcpy(&p, &other.p, sizeof(p));
    return *this;
}

void QByteArrayMatcher::setPattern(const QByteArray &pattern) {
    q_pattern = pattern;
    p.p = reinterpret_cast<const uchar *>(pattern.constData());
    p.l = pattern.size();
    bm_init_skiptable(p.p, p.l, p.q_skiptable);
}

qsizetype QByteArrayMatcher::indexIn(const QByteArray &ba, qsizetype from) const {
    if (from < 0) {
        from = 0;
    }
    return bm_find(reinterpret_cast<const uchar *>(ba.constData()), ba.size(), from, p.p, p.l, p.q_skiptable);
}

qsizetype QByteArrayMatcher::indexIn(const char *str, qsizetype len, qsizetype from) const {
    if (from < 0) {
        from = 0;
    }
    return bm_find(reinterpret_cast<const uchar *>(str), len, from, p.p, p.l, p.q_skiptable);
}

static qsizetype findChar(const char *str, qsizetype len, char ch, qsizetype from) {
    const uchar *s = (const uchar *)str;
    uchar c = (uchar)ch;
    if (from < 0) {
        from = qMax(from + len, qsizetype(0));
    }
    if (from < len) {
        const uchar *n = s + from - 1;
        const uchar *e = s + len;
        while (++n != e) {
            if (*n == c) {
                return n - s;
            }
        }
    }
    return -1;
}

static qsizetype qFindByteArrayBoyerMoore(
        const char *haystack, qsizetype haystackLen, qsizetype haystackOffset,
        const char *needle, qsizetype needleLen ) {
    uchar skiptable[256];
    bm_init_skiptable((const uchar *)needle, needleLen, skiptable);
    if (haystackOffset < 0) {
        haystackOffset = 0;
    }
    return bm_find((const uchar *)haystack, haystackLen, haystackOffset,
                   (const uchar *)needle, needleLen, skiptable);
}

#define REHASH(a) \
    if (sl_minus_1 < sizeof(std::size_t) * CHAR_BIT) \
        hashHaystack -= std::size_t(a) << sl_minus_1;\
    hashHaystack <<= 1;

//没有使用static修饰
qsizetype qFindByteArray(
        const char *haystack0, qsizetype haystackLen, qsizetype from,
        const char *needle, qsizetype needleLen)
{
    const auto l = haystackLen;
    const auto sl = needleLen;
    if (from < 0) {
        from += l;
    }
    //起始位置+needle长度超出范围
    if (std::size_t(sl + from) > std::size_t(l)) {
        return -1;
    }
    //needle长度为0，直接返回起始位置
    if (!sl) {
        return from;
    }
    //haystack长度为0，直接返回-1
    if (!l) {
        return -1;
    }
    //needle长度为1，调用findChar更高效
    if (sl == 1) {
        return findChar(haystack0, haystackLen, needle[0], from);
    }
    //TODO 算法优化
    return qFindByteArrayBoyerMoore(haystack0, haystackLen, from, needle, needleLen);
    //haystack长度 > 500 并且needle长度 > 5，调用BoyerMoore算法
//    if (l > 500 && sl > 5) {
//        return qFindByteArrayBoyerMoore(haystack0, haystackLen, from, needle, needleLen);
//    }
//    const char *haystack = haystack0 + from;
//    const char *end = haystack0 + (l - sl);
//    const auto sl_minus_1 = std::size_t(sl - 1);
//    std::size_t hashNeedle = 0, hashHaystack = 0;
//    qsizetype idx;
//    for (idx = 0; idx < sl; ++idx) {
//        hashNeedle = ((hashNeedle<<1) + needle[idx]);
//        hashHaystack = ((hashHaystack<<1) + haystack[idx]);
//    }
//    hashHaystack -= *(haystack + sl_minus_1);
//
//    while (haystack <= end) {
//        hashHaystack += *(haystack + sl_minus_1);
//        if (hashHaystack == hashNeedle && *needle == *haystack
//            && memcmp(needle, haystack, sl) == 0)
//            return haystack - haystack0;
//
//        REHASH(*haystack);
//        ++haystack;
//    }
//    return -1;
}

int QStaticByteArrayMatcherBase::indexOfIn(const char *needle, uint nlen, const char *haystack, int hlen,
                                           int from) const noexcept {
    if (from < 0) {
        from = 0;
    }
    return bm_find(reinterpret_cast<const uchar *>(haystack), hlen, from,
                   reinterpret_cast<const uchar *>(needle), nlen, m_skiptable.data);
}

QT_END_NAMESPACE