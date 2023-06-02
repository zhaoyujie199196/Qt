//
// Created by Yujie Zhao on 2023/2/28.
//

#include "qstringalgorithms.h"
#include "qstringview.h"
#include "qlatin1string.h"
#include "qchar.cpp"
#include "qbytearrayalgorithms.h"

QT_BEGIN_NAMESPACE

constexpr int lencmp(qsizetype lhs, qsizetype rhs) noexcept {
    if (lhs == rhs) {
        return 0;
    } else if (lhs > rhs) {
        return 1;
    } else {
        return -1;
    }
}

namespace QtPrivate {

//大小写不敏感判断两个字符串大小
    static int latin1nicmp(const char *lhsChar, qsizetype lSize, const char *rhsChar, qsizetype rSize) {
        //查表法
        constexpr uchar latin1Lower[256] = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
                0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
                0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
                0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
                0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
                0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
                0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
                0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
                0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
                // 0xd7 (multiplication sign) and 0xdf (sz ligature) complicate life
                0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xd7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xdf,
                0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        };
        Q_ASSERT(lSize >= 0 && rSize >= 0);
        if (!lSize) {
            return rSize ? -1 : 0;
        }
        if (!rSize) {
            return 1;
        }
        const qsizetype size = std::min(lSize, rSize);
        const uchar *lhs = reinterpret_cast<const uchar *>(lhsChar);
        const uchar *rhs = reinterpret_cast<const uchar *>(rhsChar);
        Q_ASSERT(lhs && rhs);
        for (qsizetype i = 0; i < size; ++i) {
            Q_ASSERT(lhs[i] && rhs[i]);
            int res = latin1Lower[lhs[i] - latin1Lower[rhs[i]]];
            if (res != 0) {
                return res;
            }
        }
        return lencmp(lSize, rSize);   //字符串的前面相同，取长的
    }

//unicode的字符比较，char16_t
    static int ucstrncmp(const QChar *a, const QChar *b, size_t l) {
        const QChar *end = a + l;
        while (a < end) {
            if (int diff = (int) a->unicode() - (int) b->unicode()) {
                return diff;
            }
            ++a;
            ++b;
        }
        return 0;
    }

    static int ucstrcmp(const QChar *a, size_t alen, const QChar *b, size_t blen) {
        if (a == b && alen == blen) {
            return 0;
        }
        const size_t l = std::min(alen, blen);
        int cmp = ucstrncmp(a, b, l);
        return cmp ? cmp : lencmp(alen, blen);
    }

    static int ucstricmp(const QChar *a, const QChar *ae, const QChar *b, const QChar *be) {
        //同一块地址
        if (a == b) {
            return (ae - be);
        }
        auto len = std::min(ae - a, be - b);
        const QChar *e = a + len;
        char32_t alast = 0;
        char32_t blast = 0;
        while (a < e) {
            int diff = foldCase(a->unicode(), alast) - foldCase(b->unicode(), blast);
            if ((diff)) {
                return diff;
            }
            ++a;
            ++b;
        }
        if (a == ae) {
            if (b == be) {
                return 0;
            } else {
                return -1;
            }
        } else {
            return 1;
        }
    }

    int compareStrings(QLatin1String lhs, QLatin1String rhs, Qt::CaseSensitivity cs) noexcept {
        if (lhs.isEmpty()) {
            return lencmp(qsizetype(0), rhs.size());
        }
        if (cs == Qt::CaseInsensitive) {
            return latin1nicmp(lhs.data(), lhs.size(), rhs.data(), rhs.size());
        } else {
            auto sizeMin = std::min(lhs.size(), rhs.size());
            auto r = qstrncmp(lhs.data(), rhs.data(), sizeMin);
            if (r == 0) {
                r = lencmp(lhs.size() - sizeMin, rhs.size() - sizeMin);
            }
            return r;
        }
    }

    int compareStrings(QStringView lhs, QStringView rhs, Qt::CaseSensitivity cs) noexcept {
        if (cs == Qt::CaseSensitive) {
            return ucstrcmp(lhs.begin(), lhs.size(), rhs.begin(), rhs.size());
        } else {
            return ucstricmp(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        }
    }

    int compareStrings(QStringView lhs, QLatin1String rhs, Qt::CaseSensitivity cs) noexcept {
        Q_ASSERT(false);
        return 0;
    }

    int compareStrings(QLatin1String lhs, QStringView rhs, Qt::CaseSensitivity cs) noexcept {
        Q_ASSERT(false);
        return 0;
    }

    template<typename Haystack, typename Needle>
    bool qt_starts_with_impl(Haystack haystack, Needle needle, Qt::CaseSensitivity cs) noexcept {
        if (haystack.isNull()) {
            return needle.isNull();
        }
        const auto haystackLen = haystack.size();
        const auto needleLen = needle.size();
        if (haystackLen == 0) {
            return needleLen == 0;
        }
        if (needleLen > haystackLen) {
            return false;
        }
        return QtPrivate::compareStrings(haystack.left(needleLen), needle, cs) == 0;
    }

    bool startsWith(QLatin1String haystack, QLatin1String needle, Qt::CaseSensitivity cs) noexcept {
        return qt_starts_with_impl(haystack, needle, cs);
    }
}

QT_END_NAMESPACE
