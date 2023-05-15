//
// Created by Yujie Zhao on 2023/4/25.
//

#ifndef QBYTEARRAYMATCHER_H
#define QBYTEARRAYMATCHER_H

#include <QtCore/qbytearray.h>
#include <limits>

QT_BEGIN_NAMESPACE

class QByteArrayMatcher
{
public:
    QByteArrayMatcher();
    explicit QByteArrayMatcher(const QByteArray &pattern);
    explicit QByteArrayMatcher(const char *pattern, qsizetype length);
    QByteArrayMatcher(const QByteArrayMatcher &other);
    ~QByteArrayMatcher();

    QByteArrayMatcher &operator=(const QByteArrayMatcher &other);
    void setPattern(const QByteArray &pattern);

    qsizetype indexIn(const QByteArray &ba, qsizetype from = 0) const;
    qsizetype indexIn(const char *str, qsizetype len, qsizetype from = 0) const;
    inline QByteArray pattern() const {
        if (q_pattern.isNull()) {
            return QByteArray(reinterpret_cast<const char *>(p.p), p.l);
        }
        return q_pattern;
    }

private:
    QByteArray q_pattern;
    struct Data {
        uchar q_skiptable[256];
        const uchar *p;
        qsizetype l;
    };
    union {
        uint dummy[256];
        Data p;
    };
};

class QStaticByteArrayMatcherBase
{
    alignas(16)
    struct Skiptable {
        uchar data[256];
    } m_skiptable;

protected:
    explicit constexpr QStaticByteArrayMatcherBase(const char *pattern, uint n) noexcept
        : m_skiptable(generate(pattern, n)) {

    }
    int indexOfIn(const char *needle, uint nlen, const char *haystack, int hlen, int from) const noexcept;

private:
    constexpr Skiptable generate(const char *pattern, uint n) noexcept
    {
        const auto uchar_max = (std::numeric_limits<uchar>::max)();
        uchar max = n > uchar_max ? uchar_max : uchar(n);
        Skiptable table = {
            {
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,

                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
                max, max, max, max, max, max, max, max,   max, max, max, max, max, max, max, max,
            }
        };
        pattern += n - max;
        while (max--) {
            table.data[uchar(*pattern++)] = max;
        }
        return table;
    }
};

template <uint N>
class QStaticByteArrayMatcher : public QStaticByteArrayMatcherBase
{
    char m_pattern[N];
    static_assert(N > 2, "QStaticByteArrayMatcher makes no sense for finding a single-char pattern");

public:
    explicit constexpr QStaticByteArrayMatcher(const char (&patternToMatch)[N]) noexcept
        : QStaticByteArrayMatcherBase(patternToMatch, N - 1), m_pattern()
    {
        for (uint i = 0; i < N; ++i) {
            m_pattern[i] = patternToMatch[i];
        }
    }

    int indexIn(const QByteArray &haystack, int from = 0) const noexcept
    {
        return this->indexOfIn(m_pattern, N - 1, haystack.data(), haystack.size(), from);
    }

    int indexIn(const char *haystack, int hlen, int from = 0) const noexcept
    {
        return this->indexOfIn(m_pattern, N - 1, haystack, hlen, from);
    }

    QByteArray pattern() const
    {
        return QByteArray(m_pattern, int(N - 1));
    }
};

template <uint N>
constexpr QStaticByteArrayMatcher<N> qMakeStaticByteArrayMatcher(const char(&pattern)[N]) noexcept
{
    return QStaticByteArrayMatcher<N>(pattern);
}

QT_END_NAMESPACE

#endif //QBYTEARRAYMATCHER_H
