//
// Created by Yujie Zhao on 2023/3/2.
//

#ifndef QMETATYPENORMALIZER_P_H
#define QMETATYPENORMALIZER_P_H

#include "QtCore/qglobal.h"
#include <string_view>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
    //是否是std::pair的萃取
    template <typename T>
    struct is_std_pair : std::false_type {};

    template <typename T1_, typename T2_>
    struct is_std_pair<std::pair<T1_, T2_>> : std::true_type  {
        using T1 = T1_;
        using T2 = T2_;
    };

    struct QTypeNormalizer
    {
        char *output;
        int len = 0;
        char last = 0;

    private:
        static constexpr bool is_ident_char(char s)
        {
            return ((s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z') || (s >= '0' && s <= '9')
                    || s == '_');
        }
        static constexpr bool is_space(char s) { return (s == ' ' || s == '\t' || s == '\n'); }
        static constexpr bool is_number(char s) { return s >= '0' && s <= '9'; }
        static constexpr bool starts_with_token(const char *b, const char *e, const char *token,
                                                bool msvcKw = false)
        {
            while (b != e && *token && *b == *token) {
                b++;
                token++;
            }
            if (*token)
                return false;
#ifdef Q_CC_MSVC
            /// On MSVC, keywords like class or struct are not separated with spaces in constexpr
        /// context
        if (msvcKw)
            return true;
#endif
            Q_UNUSED(msvcKw);
            return b == e || !is_ident_char(*b);
        }
        static constexpr bool skipToken(const char *&x, const char *e, const char *token,
                                        bool msvcKw = false)
        {
            if (!starts_with_token(x, e, token, msvcKw))
                return false;
            while (*token++)
                x++;
            while (x != e && is_space(*x))
                x++;
            return true;
        }
        static constexpr const char *skipString(const char *x, const char *e)
        {
            char delim = *x;
            x++;
            while (x != e && *x != delim) {
                if (*x == '\\') {
                    x++;
                    if (x == e)
                        return e;
                }
                x++;
            }
            if (x != e)
                x++;
            return x;
        };
        static constexpr const char *skipTemplate(const char *x, const char *e, bool stopAtComa = false)
        {
            int scopeDepth = 0;
            int templateDepth = 0;
            while (x != e) {
                switch (*x) {
                    case '<':
                        if (!scopeDepth)
                            templateDepth++;
                        break;
                    case ',':
                        if (stopAtComa && !scopeDepth && !templateDepth)
                            return x;
                        break;
                    case '>':
                        if (!scopeDepth)
                            if (--templateDepth < 0)
                                return x;
                        break;
                    case '(':
                    case '[':
                    case '{':
                        scopeDepth++;
                        break;
                    case '}':
                    case ']':
                    case ')':
                        scopeDepth--;
                        break;
                    case '\'':
                        if (is_number(x[-1]))
                            break;
                        assert(false);
                    case '\"':
                        x = skipString(x, e);
                        continue;
                }
                x++;
            }
            return x;
        };

        constexpr void append(char x)
        {
            last = x;
            len++;
            if (output)
                *output++ = x;
        }

        constexpr void replaceLast(char x)
        {
            last = x;
            if (output)
                *(output - 1) = x;
        }

        constexpr void appendStr(const char *x)
        {
            while (*x)
                append(*x++);
        };

        constexpr void normalizeIntegerTypes(const char *&begin, const char *end)
        {
            int numLong = 0;
            int numSigned = 0;
            int numUnsigned = 0;
            int numInt = 0;
            int numShort = 0;
            int numChar = 0;
            while (begin < end) {
                if (skipToken(begin, end, "long")) {
                    numLong++;
                    continue;
                }
                if (skipToken(begin, end, "int")) {
                    numInt++;
                    continue;
                }
                if (skipToken(begin, end, "short")) {
                    numShort++;
                    continue;
                }
                if (skipToken(begin, end, "unsigned")) {
                    numUnsigned++;
                    continue;
                }
                if (skipToken(begin, end, "signed")) {
                    numSigned++;
                    continue;
                }
                if (skipToken(begin, end, "char")) {
                    numChar++;
                    continue;
                }
#ifdef Q_CC_MSVC
                if (skipToken(begin, end, "__int64")) {
                numLong = 2;
                continue;
            }
#endif
                break;
            }
            if (numLong == 2)
                append('q'); // q(u)longlong
            if (numSigned && numChar)
                appendStr("signed ");
            else if (numUnsigned)
                appendStr("u");
            if (numChar)
                appendStr("char");
            else if (numShort)
                appendStr("short");
            else if (numLong == 1)
                appendStr("long");
            else if (numLong == 2)
                appendStr("longlong");
            else if (numUnsigned || numSigned || numInt)
                appendStr("int");
        }

        constexpr void skipStructClassOrEnum(const char *&begin, const char *end)
        {
            // discard 'struct', 'class', and 'enum'; they are optional
            // and we don't want them in the normalized signature
            skipToken(begin, end, "struct", true) || skipToken(begin, end, "class", true)
            || skipToken(begin, end, "enum", true);
        }

        constexpr void skipQtNamespace(const char *&begin, const char *end)
        {
#ifdef QT_NAMESPACE
            const char *nsbeg = begin;
        if (skipToken(nsbeg, end, QT_STRINGIFY(QT_NAMESPACE)) && nsbeg + 2 < end && nsbeg[0] == ':'
            && nsbeg[1] == ':') {
            begin = nsbeg + 2;
            while (begin != end && is_space(*begin))
                begin++;
        }
#else
            Q_UNUSED(begin);
            Q_UNUSED(end);
#endif
        }

    public:
#if defined(Q_CC_CLANG) || defined (Q_CC_GNU)
        // this is much simpler than the full type normalization below
        // the reason is that the signature returned by Q_FUNC_INFO is already
        // normalized to the largest degree, and we need to do only small adjustments
        constexpr int normalizeTypeFromSignature(const char *begin, const char *end)
        {
            // bail out if there is an anonymous struct
            std::string_view name(begin, end-begin);
#if defined (Q_CC_CLANG)
            if (name.find("anonymous ") != std::string_view::npos)
                return normalizeType(begin, end);
#else
            if (name.find("unnamed ") != std::string_view::npos)
            return normalizeType(begin, end);
#endif
            while (begin < end) {
                if (*begin == ' ') {
                    if (last == ',' || last == '>' || last == '<' || last == '*' || last == '&') {
                        ++begin;
                        continue;
                    }
                }
                if (last == ' ') {
                    if (*begin == '*' || *begin == '&' || *begin == '(') {
                        replaceLast(*begin);
                        ++begin;
                        continue;
                    }
                }
                if (!is_ident_char(last)) {
                    skipStructClassOrEnum(begin, end);
                    if (begin == end)
                        break;

                    skipQtNamespace(begin, end);
                    if (begin == end)
                        break;

                    normalizeIntegerTypes(begin, end);
                    if (begin == end)
                        break;
                }
                append(*begin);
                ++begin;
            }
            return len;
        }
#else
        // MSVC needs the full normalization, as it puts the const in a different
    // place than we expect
    constexpr int normalizeTypeFromSignature(const char *begin, const char *end)
    { return normalizeType(begin, end); }
#endif

        constexpr int normalizeType(const char *begin, const char *end, bool adjustConst = true)
        {
            // Trim spaces
            while (begin != end && is_space(*begin))
                begin++;
            while (begin != end && is_space(*(end - 1)))
                end--;

            // Convert 'char const *' into 'const char *'. Start at index 1,
            // not 0, because 'const char *' is already OK.
            const char *cst = begin + 1;
            if (*begin == '\'' || *begin == '"')
                cst = skipString(begin, end);
            bool seenStar = false;
            bool hasMiddleConst = false;
            while (cst < end) {
                if (*cst == '\"' || (*cst == '\'' && !is_number(cst[-1]))) {
                    cst = skipString(cst, end);
                    if (cst == end)
                        break;
                }

                // We mustn't convert 'char * const *' into 'const char **'
                // and we must beware of 'Bar<const Bla>'.
                if (*cst == '&' || *cst == '*' || *cst == '[') {
                    seenStar = *cst != '&' || cst != (end - 1);
                    break;
                }
                if (*cst == '<') {
                    cst = skipTemplate(cst + 1, end);
                    if (cst == end)
                        break;
                }
                cst++;
                const char *skipedCst = cst;
                if (!is_ident_char(*(cst - 1)) && skipToken(skipedCst, end, "const")) {
                    const char *testEnd = end;
                    while (skipedCst < testEnd--) {
                        if (*testEnd == '*' || *testEnd == '['
                            || (*testEnd == '&' && testEnd != (end - 1))) {
                            seenStar = true;
                            break;
                        }
                        if (*testEnd == '>')
                            break;
                    }
                    if (adjustConst && !seenStar) {
                        if (*(end - 1) == '&')
                            end--;
                    } else {
                        appendStr("const ");
                    }
                    normalizeType(begin, cst, false);
                    begin = skipedCst;
                    hasMiddleConst = true;
                    break;
                }
            }
            if (skipToken(begin, end, "const")) {
                if (adjustConst && !seenStar) {
                    if (*(end - 1) == '&')
                        end--;
                } else {
                    appendStr("const ");
                }
            }
            if (seenStar && adjustConst) {
                const char *e = end;
                if (*(end - 1) == '&' && *(end - 2) != '&')
                    e--;
                while (begin != e && is_space(*(e - 1)))
                    e--;
                const char *token = "tsnoc"; // 'const' reverse, to check if it ends with const
                while (*token && begin != e && *(--e) == *token++)
                    ;
                if (!*token && begin != e && !is_ident_char(*(e - 1))) {
                    while (begin != e && is_space(*(e - 1)))
                        e--;
                    end = e;
                }
            }

            skipStructClassOrEnum(begin, end);
            skipQtNamespace(begin, end);

            if (skipToken(begin, end, "QVector")) {
                // Replace QVector by QList
                appendStr("QList");
            }

            if (skipToken(begin, end, "QPair")) {
                // replace QPair by std::pair
                appendStr("std::pair");
            }

            if (!hasMiddleConst)
                // Normalize the integer types
                normalizeIntegerTypes(begin, end);

            bool spaceSkiped = true;
            while (begin != end) {
                char c = *begin++;
                if (is_space(c)) {
                    spaceSkiped = true;
                } else if ((c == '\'' && !is_number(last)) || c == '\"') {
                    begin--;
                    auto x = skipString(begin, end);
                    while (begin < x)
                        append(*begin++);
                } else {
                    if (spaceSkiped && is_ident_char(last) && is_ident_char(c))
                        append(' ');
                    append(c);
                    spaceSkiped = false;
                    if (c == '<') {
                        do {
                            // template recursion
                            const char *tpl = skipTemplate(begin, end, true);
                            normalizeType(begin, tpl, false);
                            if (tpl == end)
                                return len;
                            append(*tpl);
                            begin = tpl;
                        } while (*begin++ == ',');
                    }
                }
            }
            return len;
        }
    };


    template <typename T>
    constexpr auto typenameHelper()
    {
        if constexpr(is_std_pair<T>::value) {
            assert(false);
            return "";
        }
        else {
            constexpr auto prefix = sizeof(
#ifdef QT_NAMESPACE
                                            QT_STRINGIFY(QT_NAMESPACE) "::"
#endif
#ifdef Q_CC_MSVC
                                            assert(false);
#elif defined(Q_CC_CLANG)
                                            "auto QtPrivate::typenameHelper() [T = "
#elif defined(Q_CC_GHS)
                                            assert(false);
#else
        "constexpr auto QtPrivate::typenameHelper() [with T = "
#endif
                                    ) - 1;
#ifdef Q_CC_MSVC
            constexpr int suffix = sizeof(">(void)");
#else
            constexpr int suffix = sizeof("]");
#endif

#if (defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && !defined(Q_CC_CLANG) && Q_CC_GNU < 804)
            assert(false);
#else
            constexpr auto func = Q_FUNC_INFO;
            constexpr const char *begin = func + prefix;
            constexpr const char *end = func + sizeof(Q_FUNC_INFO) - suffix;
            const int len = QtPrivate::QTypeNormalizer{ nullptr }.normalizeTypeFromSignature(begin, end);
#endif

            std::array<char, len + 1> result {};
            QtPrivate::QTypeNormalizer{ result.data() }.normalizeTypeFromSignature(begin, end);
            return result;
        }
    }

}

QT_END_NAMESPACE

#endif //QMETATYPENORMALIZER_P_H
