//
// Created by Yujie Zhao on 2023/6/20.
//
#include "qstringmatcher.h"
#include "qchar.h"

QT_BEGIN_NAMESPACE

//skipTable的索引直接用的char16 & 0xff，解决char16_t类型可表示数据过多的问题
//剩下的就是boyer Moore算法 https://zhuanlan.zhihu.com/p/530369703

static void bm_init_skiptable(QStringView needle, uchar *skiptable, Qt::CaseSensitivity cs)
{

    const char16_t *uc = needle.utf16();
    const qsizetype len = needle.size();
    qsizetype l = qMin(len, qsizetype(255));
    memset(skiptable, l, 256 * sizeof(uchar));
    uc += len - l;
    if (cs == Qt::CaseSensitive) {
        while (l--) {
            skiptable[*uc & 0xff] = l;
            ++uc;
        }
    } else {
        const char16_t *start = uc;
        while (l--) {
            skiptable[QChar::foldCase(uc, start) & 0xff] = l;
            ++uc;
        }
    }
}

static inline qsizetype bm_find(QStringView haystack, qsizetype index, QStringView needle,
                                const uchar *skiptable, Qt::CaseSensitivity cs)
{
    const char16_t *uc = haystack.utf16();
    const qsizetype l = haystack.size();
    const char16_t *puc = needle.utf16();
    const qsizetype pl = needle.size();

    if (pl == 0)
        return index > l ? -1 : index;
    const qsizetype pl_minus_one = pl - 1;

    const char16_t *current = uc + index + pl_minus_one;
    const char16_t *end = uc + l;
    if (cs == Qt::CaseSensitive) {
        while (current < end) {
            qsizetype skip = skiptable[*current & 0xff];
            if (!skip) {
                // possible match
                while (skip < pl) {
                    if (*(current - skip) != puc[pl_minus_one-skip])
                        break;
                    ++skip;
                }
                if (skip > pl_minus_one) // we have a match
                    return (current - uc) - pl_minus_one;

                // in case we don't have a match we are a bit inefficient as we only skip by one
                // when we have the non matching char in the string.
                if (skiptable[*(current - skip) & 0xff] == pl)
                    skip = pl - skip;
                else
                    skip = 1;
            }
            if (current > end - skip)
                break;
            current += skip;
        }
    } else {
        while (current < end) {
            qsizetype skip = skiptable[QChar::foldCase(current, uc) & 0xff];
            if (!skip) {
                // possible match
                while (skip < pl) {
                    if (QChar::foldCase(current - skip, uc) != QChar::foldCase(puc + pl_minus_one - skip, puc))
                        break;
                    ++skip;
                }
                if (skip > pl_minus_one) // we have a match
                    return (current - uc) - pl_minus_one;
                // in case we don't have a match we are a bit inefficient as we only skip by one
                // when we have the non matching char in the string.
                if (skiptable[QChar::foldCase(current - skip, uc) & 0xff] == pl)
                    skip = pl - skip;
                else
                    skip = 1;
            }
            if (current > end - skip)
                break;
            current += skip;
        }
    }
    return -1; // not found
}

void QStringMatcher::updateSkipTable()
{
    bm_init_skiptable(q_sv, q_skiptable, q_cs);
}

QStringMatcher::QStringMatcher(const QString &pattern, Qt::CaseSensitivity cs)
        : q_cs(cs), q_pattern(pattern)
{
    q_sv = q_pattern;
    updateSkipTable();
}

QStringMatcher::QStringMatcher(QStringView str, Qt::CaseSensitivity cs)
        : q_cs(cs), q_sv(str)
{
    updateSkipTable();
}

QStringMatcher::QStringMatcher(const QStringMatcher &other)
{
    operator=(other);
}

QStringMatcher::~QStringMatcher()
{
}

QStringMatcher &QStringMatcher::operator=(const QStringMatcher &other)
{
    if (this != &other) {
        q_pattern = other.q_pattern;
        q_cs = other.q_cs;
        q_sv = other.q_sv;
        memcpy(q_skiptable, other.q_skiptable, sizeof(q_skiptable));
    }
    return *this;
}

void QStringMatcher::setPattern(const QString &pattern)
{
    q_pattern = pattern;
    q_sv = q_pattern;
    updateSkipTable();
}

QString QStringMatcher::pattern() const
{
    if (!q_pattern.isEmpty())
        return q_pattern;
    return q_sv.toString();
}

void QStringMatcher::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    if (cs == q_cs)
        return;
    q_cs = cs;
    updateSkipTable();
}

qsizetype QStringMatcher::indexIn(QStringView str, qsizetype from) const
{
    if (from < 0)
        from = 0;
    return bm_find(str, from, q_sv, q_skiptable, q_cs);
}

qsizetype qFindStringBoyerMoore(
        QStringView haystack, qsizetype haystackOffset,
        QStringView needle, Qt::CaseSensitivity cs)
{
    uchar skiptable[256];
    bm_init_skiptable(needle, skiptable, cs);
    if (haystackOffset < 0)
        haystackOffset = 0;
    return bm_find(haystack, haystackOffset, needle, skiptable, cs);
}


QT_END_NAMESPACE