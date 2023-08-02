//
// Created by Yujie Zhao on 2023/2/6.
//

#include "qchar.h"
#include "qunicodetables_p.h"
#include "qunicodetables.cpp"

QT_BEGIN_NAMESPACE

//定义的一些枚举就可以使用更自然的写法，不需要使用 1 << 2这种形式
#define FLAG(x) (1 << (x))

// constants for Hangul (de)composition, see UAX #15
enum {
    Hangul_SBase = 0xac00,
    Hangul_LBase = 0x1100,
    Hangul_VBase = 0x1161,
    Hangul_TBase = 0x11a7,
    Hangul_LCount = 19,
    Hangul_VCount = 21,
    Hangul_TCount = 28,
    Hangul_NCount = Hangul_VCount * Hangul_TCount,
    Hangul_SCount = Hangul_LCount * Hangul_NCount
};

template<typename T>
static inline T convertCase_helper(T uc, QUnicodeTables::Case which) noexcept {
    const auto fold = qGetProp(uc)->cases[which];
    if (Q_UNLIKELY(fold.special)) {
        const ushort *specialCase = specialCaseMap + fold.diff;
        // so far, there are no special cases beyond BMP (guaranteed by the qunicodetables generator)
        return *specialCase == 1 ? specialCase[1] : uc;
    }

    return uc + fold.diff;
}

bool QChar::compare(QChar c) const noexcept {
    if (ucs < c.ucs) {
        return -1;
    }
    else if (ucs == c.ucs) {
        return 0;
    }
    else {
        return 1;
    }
}

QChar::Category QChar::category(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return QChar::Other_NotAssigned;
    }
    return (QChar::Category) qGetProp(ucs4)->category;
}

QChar::Direction QChar::direction(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return QChar::DirL;
    }
    return (QChar::Direction) qGetProp(ucs4)->direction;
}

QChar::JoiningType QChar::joiningType(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return QChar::Joining_None;
    }
    return QChar::JoiningType(qGetProp(ucs4)->joining);
}

unsigned char QChar::combiningClass(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return 0;
    }
    return (unsigned char) qGetProp(ucs4)->combiningClass;
}

int QChar::digitValue(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return -1;
    }
    return qGetProp(ucs4)->digitValue;
}

char32_t QChar::toLower(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return ucs4;
    }
    return convertCase_helper(ucs4, QUnicodeTables::LowerCase);
}

char32_t QChar::toUpper(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return ucs4;
    }
    return convertCase_helper(ucs4, QUnicodeTables::UpperCase);
}

char32_t QChar::toTitleCase(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return ucs4;
    }
    return convertCase_helper(ucs4, QUnicodeTables::TitleCase);
}

char32_t QChar::toCaseFolded(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return ucs4;
    }
    return convertCase_helper(ucs4, QUnicodeTables::CaseFold);
}

QChar::Script QChar::script(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return QChar::Script_Unknown;
    }
    return (QChar::Script) qGetProp(ucs4)->script;
}

bool QChar::isUpper(char32_t ucs4) noexcept {
    return (ucs4 <= 'Z' && ucs4 >= 'A') || (ucs4 > 127 && QChar::category(ucs4) == Letter_Uppercase);
}

bool QChar::isLower(char32_t ucs4) noexcept {
    return (ucs4 <= 'z' && ucs4 >= 'a') || (ucs4 > 127 && QChar::category(ucs4) == Letter_Lowercase);
}

bool QChar::isDigit(char32_t ucs4) noexcept {
    return (ucs4 <= '9' && ucs4 >= '0') || (ucs4 > 127 && QChar::category(ucs4) == Number_DecimalDigit);
}

bool QChar::isLetter(char32_t ucs4) noexcept {
    return (ucs4 >= 'A' && ucs4 <= 'z' && (ucs4 >= 'a' || ucs4 <= 'Z'))
           || (ucs4 > 127 && QChar::isLetter_helper(ucs4));
}

bool QChar::isLetterOrNumber(char32_t ucs4) noexcept {
    return (ucs4 >= 'A' && ucs4 <= 'z' && (ucs4 >= 'a' || ucs4 <= 'Z'))
           || (ucs4 >= '0' && ucs4 <= '9')
           || (ucs4 > 127 && QChar::isLetterOrNumber_helper(ucs4));
}

bool QChar::isPrint(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return false;
    }
    static const int test = FLAG(Other_Control) |
                            FLAG(Other_Format) |
                            FLAG(Other_Surrogate) |
                            FLAG(Other_PrivateUse) |
                            FLAG(Other_NotAssigned);
    return !(FLAG(qGetProp(ucs4)->category) & test);
}

bool QChar::isTitleCase(char32_t ucs4) noexcept {
    return ucs4 > 127 && QChar::category(ucs4) == Letter_Titlecase;
}

bool QChar::hasMirrored(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return false;
    }
    return qGetProp(ucs4)->mirrorDiff != 0;
}

char32_t QChar::mirroredChar(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return ucs4;
    }
    return ucs4 + qGetProp(ucs4)->mirrorDiff;
}

bool QChar::isSpace(char32_t ucs4) noexcept {
    return ucs4 == 0x20 || (ucs4 <= 0x0d && ucs4 >= 0x09)
           || (ucs4 > 127 && (ucs4 == 0x85 || ucs4 == 0xa0 || QChar::isSpace_helper(ucs4)));
}

bool QChar::isLetter_helper(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint)
        return false;
    const int test = FLAG(Letter_Uppercase) |
                     FLAG(Letter_Lowercase) |
                     FLAG(Letter_Titlecase) |
                     FLAG(Letter_Modifier) |
                     FLAG(Letter_Other);
    return FLAG(qGetProp(ucs4)->category) & test;
}

bool QChar::isLetterOrNumber_helper(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint)
        return false;
    const int test = FLAG(Letter_Uppercase) |
                     FLAG(Letter_Lowercase) |
                     FLAG(Letter_Titlecase) |
                     FLAG(Letter_Modifier) |
                     FLAG(Letter_Other) |
                     FLAG(Number_DecimalDigit) |
                     FLAG(Number_Letter) |
                     FLAG(Number_Other);
    return FLAG(qGetProp(ucs4)->category) & test;
}

bool QChar::isSpace_helper(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint)
        return false;
    const int test = FLAG(Separator_Space) |
                     FLAG(Separator_Line) |
                     FLAG(Separator_Paragraph);
    return FLAG(qGetProp(ucs4)->category) & test;
}

QChar::UnicodeVersion QChar::unicodeVersion(char32_t ucs4) noexcept {
    if (ucs4 > LastValidCodePoint) {
        return QChar::Unicode_Unassigned;
    }
    return (QChar::UnicodeVersion) qGetProp(ucs4)->unicodeVersion;
}

QChar::Decomposition QChar::decompositionTag(char32_t ucs4) noexcept {
    if (ucs4 >= Hangul_SBase && ucs4 < Hangul_SBase + Hangul_SCount)
        return QChar::Canonical;
    const unsigned short index = GET_DECOMPOSITION_INDEX(ucs4);
    if (index == 0xffff)
        return QChar::NoDecomposition;
    return (QChar::Decomposition)(uc_decomposition_map[index] & 0xff);
}

char32_t QChar::foldCase(const char16_t *ch, const char16_t *start) {
    char32_t ucs4 = *ch;
    if (QChar::isLowSurrogate(ucs4) && ch > start && QChar::isHighSurrogate(*(ch - 1))) {
        ucs4 = QChar::surrogateToUcs4(*(ch - 1), ucs4);
    }
    return convertCase_helper(ucs4, QUnicodeTables::CaseFold);
}

char32_t QChar::foldCase(char32_t ch, char32_t &last) noexcept {
    char32_t ucs4 = ch;
    if (QChar::isLowSurrogate(ucs4) && QChar::isHighSurrogate(last)) {
        ucs4 = QChar::surrogateToUcs4(last, ucs4);
    }
    last = ch;
    return convertCase_helper(ucs4, QUnicodeTables::CaseFold);
}

char16_t QChar::foldCase(char16_t ch) noexcept {
    return convertCase_helper(ch, QUnicodeTables::CaseFold);
}

QChar QChar::foldCase(QChar ch) noexcept {
    return QChar(foldCase(ch.unicode()));
}

QT_END_NAMESPACE