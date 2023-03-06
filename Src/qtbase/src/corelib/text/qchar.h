//
// Created by Yujie Zhao on 2023/2/6.
//

#ifndef QCHAR_H
#define QCHAR_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

/*
 * unicode编码使用ucs4能完全表示，但是ucs2覆盖了大部分，所以一般编码使用char16_t表示，以解决内存
 * */

struct QLatin1Char
{
public:
    constexpr inline explicit QLatin1Char(char c) noexcept : ch(c) {}
    constexpr inline char toLatin1() const noexcept {return ch;}
    constexpr inline char16_t unicode() const noexcept {return char16_t(uchar(ch));}

    friend constexpr inline bool operator==(QLatin1Char lhs, QLatin1Char rhs) noexcept {return lhs.ch == rhs.ch;}
    friend constexpr inline bool operator!=(QLatin1Char lhs, QLatin1Char rhs) noexcept {return lhs.ch != rhs.ch;}
    friend constexpr inline bool operator< (QLatin1Char lhs, QLatin1Char rhs) noexcept {return lhs.ch <  rhs.ch;}
    friend constexpr inline bool operator<=(QLatin1Char lhs, QLatin1Char rhs) noexcept {return lhs.ch <= rhs.ch;}
    friend constexpr inline bool operator> (QLatin1Char lhs, QLatin1Char rhs) noexcept {return lhs.ch >  rhs.ch;}
    friend constexpr inline bool operator>=(QLatin1Char lhs, QLatin1Char rhs) noexcept {return lhs.ch >= rhs.ch;}

    friend constexpr inline bool operator==(QLatin1Char lhs, char rhs) noexcept {return lhs.ch == rhs;}
    friend constexpr inline bool operator!=(QLatin1Char lhs, char rhs) noexcept {return lhs.ch != rhs;}
    friend constexpr inline bool operator< (QLatin1Char lhs, char rhs) noexcept {return lhs.ch <  rhs;}
    friend constexpr inline bool operator<=(QLatin1Char lhs, char rhs) noexcept {return lhs.ch <= rhs;}
    friend constexpr inline bool operator> (QLatin1Char lhs, char rhs) noexcept {return lhs.ch >  rhs;}
    friend constexpr inline bool operator>=(QLatin1Char lhs, char rhs) noexcept {return lhs.ch >= rhs;}

    friend constexpr inline bool operator==(char lhs, QLatin1Char rhs) noexcept {return lhs == rhs.ch;}
    friend constexpr inline bool operator!=(char lhs, QLatin1Char rhs) noexcept {return lhs != rhs.ch;}
    friend constexpr inline bool operator< (char lhs, QLatin1Char rhs) noexcept {return lhs <  rhs.ch;}
    friend constexpr inline bool operator<=(char lhs, QLatin1Char rhs) noexcept {return lhs <= rhs.ch;}
    friend constexpr inline bool operator> (char lhs, QLatin1Char rhs) noexcept {return lhs >  rhs.ch;}
    friend constexpr inline bool operator>=(char lhs, QLatin1Char rhs) noexcept {return lhs >= rhs.ch;}

private:
    char ch;
};

class QChar
{
public:
    //特殊字符
    enum SpecialCharacter {
        Null = 0x0000,
        Tabulation = 0x0009,
        LineFeed = 0x000a,
        FormFeed = 0x000c,
        CarriageReturn = 0x000d,
        Space = 0x0020,
        Nbsp = 0x00a0,
        SoftHyphen = 0x00ad,
        ReplacementCharacter = 0xfffd,
        ObjectReplacementCharacter = 0xfffc,
        ByteOrderMark = 0xfeff,
        ByteOrderSwapped = 0xfffe,
        ParagraphSeparator = 0x2029,
        LineSeparator = 0x2028,
        VisualTabCharacter = 0x2192,
        LastValidCodePoint = 0x10ffff  //Unicode序号的范围为（0~1114111，16进制为0~0x10FFFF） https://zhuanlan.zhihu.com/p/494350246
    };

    enum Category
    {
        Mark_NonSpacing,          //   Mn
        Mark_SpacingCombining,    //   Mc
        Mark_Enclosing,           //   Me

        Number_DecimalDigit,      //   Nd
        Number_Letter,            //   Nl
        Number_Other,             //   No

        Separator_Space,          //   Zs
        Separator_Line,           //   Zl
        Separator_Paragraph,      //   Zp

        Other_Control,            //   Cc
        Other_Format,             //   Cf
        Other_Surrogate,          //   Cs
        Other_PrivateUse,         //   Co
        Other_NotAssigned,        //   Cn

        Letter_Uppercase,         //   Lu
        Letter_Lowercase,         //   Ll
        Letter_Titlecase,         //   Lt
        Letter_Modifier,          //   Lm
        Letter_Other,             //   Lo

        Punctuation_Connector,    //   Pc
        Punctuation_Dash,         //   Pd
        Punctuation_Open,         //   Ps
        Punctuation_Close,        //   Pe
        Punctuation_InitialQuote, //   Pi
        Punctuation_FinalQuote,   //   Pf
        Punctuation_Other,        //   Po

        Symbol_Math,              //   Sm
        Symbol_Currency,          //   Sc
        Symbol_Modifier,          //   Sk
        Symbol_Other              //   So
    };

    enum Direction
    {
        DirL, DirR, DirEN, DirES, DirET, DirAN, DirCS, DirB, DirS, DirWS, DirON,
        DirLRE, DirLRO, DirAL, DirRLE, DirRLO, DirPDF, DirNSM, DirBN,
        DirLRI, DirRLI, DirFSI, DirPDI
    };

    enum JoiningType {
        Joining_None,
        Joining_Causing,
        Joining_Dual,
        Joining_Right,
        Joining_Left,
        Joining_Transparent
    };

    enum CombiningClass
    {
        Combining_BelowLeftAttached       = 200,
        Combining_BelowAttached           = 202,
        Combining_BelowRightAttached      = 204,
        Combining_LeftAttached            = 208,
        Combining_RightAttached           = 210,
        Combining_AboveLeftAttached       = 212,
        Combining_AboveAttached           = 214,
        Combining_AboveRightAttached      = 216,

        Combining_BelowLeft               = 218,
        Combining_Below                   = 220,
        Combining_BelowRight              = 222,
        Combining_Left                    = 224,
        Combining_Right                   = 226,
        Combining_AboveLeft               = 228,
        Combining_Above                   = 230,
        Combining_AboveRight              = 232,

        Combining_DoubleBelow             = 233,
        Combining_DoubleAbove             = 234,
        Combining_IotaSubscript           = 240
    };

    enum UnicodeVersion {
        Unicode_Unassigned,
        Unicode_1_1,
        Unicode_2_0,
        Unicode_2_1_2,
        Unicode_3_0,
        Unicode_3_1,
        Unicode_3_2,
        Unicode_4_0,
        Unicode_4_1,
        Unicode_5_0,
        Unicode_5_1,
        Unicode_5_2,
        Unicode_6_0,
        Unicode_6_1,
        Unicode_6_2,
        Unicode_6_3,
        Unicode_7_0,
        Unicode_8_0,
        Unicode_9_0,
        Unicode_10_0,
        Unicode_11_0,
        Unicode_12_0,
        Unicode_12_1,
        Unicode_13_0
    };

    enum Decomposition
    {
        NoDecomposition,
        Canonical,
        Font,
        NoBreak,
        Initial,
        Medial,
        Final,
        Isolated,
        Circle,
        Super,
        Sub,
        Vertical,
        Wide,
        Narrow,
        Small,
        Square,
        Compat,
        Fraction
    };

    enum Script
    {
        Script_Unknown,
        Script_Inherited,
        Script_Common,

        Script_Latin,
        Script_Greek,
        Script_Cyrillic,
        Script_Armenian,
        Script_Hebrew,
        Script_Arabic,
        Script_Syriac,
        Script_Thaana,
        Script_Devanagari,
        Script_Bengali,
        Script_Gurmukhi,
        Script_Gujarati,
        Script_Oriya,
        Script_Tamil,
        Script_Telugu,
        Script_Kannada,
        Script_Malayalam,
        Script_Sinhala,
        Script_Thai,
        Script_Lao,
        Script_Tibetan,
        Script_Myanmar,
        Script_Georgian,
        Script_Hangul,
        Script_Ethiopic,
        Script_Cherokee,
        Script_CanadianAboriginal,
        Script_Ogham,
        Script_Runic,
        Script_Khmer,
        Script_Mongolian,
        Script_Hiragana,
        Script_Katakana,
        Script_Bopomofo,
        Script_Han,
        Script_Yi,
        Script_OldItalic,
        Script_Gothic,
        Script_Deseret,
        Script_Tagalog,
        Script_Hanunoo,
        Script_Buhid,
        Script_Tagbanwa,
        Script_Coptic,

        // Unicode 4.0 additions
        Script_Limbu,
        Script_TaiLe,
        Script_LinearB,
        Script_Ugaritic,
        Script_Shavian,
        Script_Osmanya,
        Script_Cypriot,
        Script_Braille,

        // Unicode 4.1 additions
        Script_Buginese,
        Script_NewTaiLue,
        Script_Glagolitic,
        Script_Tifinagh,
        Script_SylotiNagri,
        Script_OldPersian,
        Script_Kharoshthi,

        // Unicode 5.0 additions
        Script_Balinese,
        Script_Cuneiform,
        Script_Phoenician,
        Script_PhagsPa,
        Script_Nko,

        // Unicode 5.1 additions
        Script_Sundanese,
        Script_Lepcha,
        Script_OlChiki,
        Script_Vai,
        Script_Saurashtra,
        Script_KayahLi,
        Script_Rejang,
        Script_Lycian,
        Script_Carian,
        Script_Lydian,
        Script_Cham,

        // Unicode 5.2 additions
        Script_TaiTham,
        Script_TaiViet,
        Script_Avestan,
        Script_EgyptianHieroglyphs,
        Script_Samaritan,
        Script_Lisu,
        Script_Bamum,
        Script_Javanese,
        Script_MeeteiMayek,
        Script_ImperialAramaic,
        Script_OldSouthArabian,
        Script_InscriptionalParthian,
        Script_InscriptionalPahlavi,
        Script_OldTurkic,
        Script_Kaithi,

        // Unicode 6.0 additions
        Script_Batak,
        Script_Brahmi,
        Script_Mandaic,

        // Unicode 6.1 additions
        Script_Chakma,
        Script_MeroiticCursive,
        Script_MeroiticHieroglyphs,
        Script_Miao,
        Script_Sharada,
        Script_SoraSompeng,
        Script_Takri,

        // Unicode 7.0 additions
        Script_CaucasianAlbanian,
        Script_BassaVah,
        Script_Duployan,
        Script_Elbasan,
        Script_Grantha,
        Script_PahawhHmong,
        Script_Khojki,
        Script_LinearA,
        Script_Mahajani,
        Script_Manichaean,
        Script_MendeKikakui,
        Script_Modi,
        Script_Mro,
        Script_OldNorthArabian,
        Script_Nabataean,
        Script_Palmyrene,
        Script_PauCinHau,
        Script_OldPermic,
        Script_PsalterPahlavi,
        Script_Siddham,
        Script_Khudawadi,
        Script_Tirhuta,
        Script_WarangCiti,

        // Unicode 8.0 additions
        Script_Ahom,
        Script_AnatolianHieroglyphs,
        Script_Hatran,
        Script_Multani,
        Script_OldHungarian,
        Script_SignWriting,

        // Unicode 9.0 additions
        Script_Adlam,
        Script_Bhaiksuki,
        Script_Marchen,
        Script_Newa,
        Script_Osage,
        Script_Tangut,

        // Unicode 10.0 additions
        Script_MasaramGondi,
        Script_Nushu,
        Script_Soyombo,
        Script_ZanabazarSquare,

        // Unicode 12.1 additions
        Script_Dogra,
        Script_GunjalaGondi,
        Script_HanifiRohingya,
        Script_Makasar,
        Script_Medefaidrin,
        Script_OldSogdian,
        Script_Sogdian,
        Script_Elymaic,
        Script_Nandinagari,
        Script_NyiakengPuachueHmong,
        Script_Wancho,

        // Unicode 13.0 additions
        Script_Chorasmian,
        Script_DivesAkuru,
        Script_KhitanSmallScript,
        Script_Yezidi,

        ScriptCount
    };


    constexpr Q_EXPLICIT QChar() noexcept : ucs(0) {}
    constexpr Q_EXPLICIT QChar(ushort rc) noexcept : ucs(rc) {}
    constexpr Q_EXPLICIT QChar(uint rc) noexcept : ucs((Q_ASSERT(rc <= 0xffff), char16_t(rc))) {}
    constexpr Q_EXPLICIT QChar(int rc) noexcept : QChar(uint(rc)) {}
    constexpr Q_EXPLICIT QChar(SpecialCharacter s) noexcept : ucs(char16_t(s)) {}
    constexpr Q_EXPLICIT QChar(QLatin1Char ch) noexcept : QChar(ch.unicode()) {}
    constexpr Q_EXPLICIT QChar(char16_t ch) noexcept : ucs(ch) {}

    inline Category category() const noexcept {return QChar::category(ucs);}
    inline Direction direction() const noexcept {return QChar::direction(ucs);}
    inline JoiningType joiningType() const noexcept {return QChar::joiningType(ucs);}
    inline unsigned char combiningClass() const noexcept {return QChar::combiningClass(ucs);}
    constexpr inline bool isNull() const noexcept {return ucs == 0;}
    static constexpr QChar fromLatin1(char c) noexcept {return QLatin1Char(c);}

    inline QChar mirroredChar() const noexcept {return QChar(QChar::mirroredChar(ucs));}
    inline bool hasMirrored() const noexcept {return QChar::hasMirrored(ucs);}

    inline UnicodeVersion unicodeVersion() const noexcept {return QChar::unicodeVersion(ucs);}
    inline Decomposition decompositionTag() const noexcept {return QChar::decompositionTag(ucs);}

    //最基础的两个比较
    friend constexpr inline bool operator==(QChar c1, QChar c2) noexcept {return c1.ucs == c2.ucs;}
    friend constexpr inline bool operator<(QChar c1, QChar c2) noexcept {return c1.ucs < c2.ucs;}

    friend constexpr inline bool operator!=(QChar c1, QChar c2) noexcept {return !operator==(c1, c2);}
    friend constexpr inline bool operator>=(QChar c1, QChar c2) noexcept {return !operator<(c1, c2);}
    friend constexpr inline bool operator> (QChar c1, QChar c2) noexcept {return  operator<(c2, c1);}
    friend constexpr inline bool operator<=(QChar c1, QChar c2) noexcept {return !operator<(c2, c1);}
    //和nullptr比较  std::nullptr_t是nullptr的数据类型
    friend constexpr inline bool operator==(QChar lhs, std::nullptr_t) noexcept {return lhs.isNull();}
    friend constexpr inline bool operator< (QChar lhs, std::nullptr_t) noexcept {return false;}
    friend constexpr inline bool operator==(std::nullptr_t, QChar rhs) noexcept {return rhs.isNull();}
    friend constexpr inline bool operator< (std::nullptr_t, QChar rhs) noexcept {return !rhs.isNull();}

    friend constexpr inline bool operator!=(QChar lhs, std::nullptr_t) noexcept {return !operator==(lhs, nullptr);}
    friend constexpr inline bool operator<=(QChar lhs, std::nullptr_t) noexcept {return !operator< (nullptr, lhs);}
    friend constexpr inline bool operator> (QChar lhs, std::nullptr_t) noexcept {return  operator< (nullptr, lhs);}
    friend constexpr inline bool operator>=(QChar lhs, std::nullptr_t) noexcept {return !operator< (lhs, nullptr);}

    friend constexpr inline bool operator!=(std::nullptr_t, QChar rhs) noexcept {return !operator==(nullptr, rhs);}
    friend constexpr inline bool operator<=(std::nullptr_t, QChar rhs) noexcept {return !operator< (rhs, nullptr);}
    friend constexpr inline bool operator> (std::nullptr_t, QChar rhs) noexcept {return  operator< (rhs, nullptr);}
    friend constexpr inline bool operator>=(std::nullptr_t, QChar rhs) noexcept {return !operator< (nullptr, rhs);}

    inline int digitValue() const noexcept {return QChar::digitValue(ucs);}
    inline QChar toUpper() const noexcept {return QChar::toUpper(ucs);}
    inline QChar toLower() const noexcept {return QChar::toLower(ucs);}
    inline QChar toTitleCase() const noexcept {return QChar::toTitleCase(ucs);}
    //大小写折叠 http://www.lvesu.com/blog/es/case-folding.html
    inline QChar toCaseFolded() const noexcept {return QChar::toCaseFolded(ucs);}
    inline Script script() const noexcept {return QChar::script(ucs);}

    inline bool isUpper() const noexcept {return QChar::isUpper(ucs);}
    inline bool isLower() const noexcept {return QChar::isLower(ucs);}
    inline bool isDigit() const noexcept {return QChar::isDigit(ucs);}
    inline bool isLetter() const noexcept {return QChar::isLetter(ucs);}
    inline bool isLetterOrNumber() const noexcept {return QChar::isLetterOrNumber(ucs);}
    //是否是可打印的字符
    inline bool isPrint() const noexcept {return QChar::isPrint(ucs);}
    inline bool isSpace() const noexcept {return QChar::isSpace(ucs);}
    inline bool isTitleCase() const noexcept {return QChar::isTitleCase(ucs);}

    constexpr inline bool isNonCharacter() const noexcept {return QChar::isNonCharacter(ucs);}
    constexpr inline bool isHighSurrogate() const noexcept {return QChar::isHighSurrogate(ucs);}
    constexpr inline bool isLowSurrogate() const noexcept {return QChar::isLowSurrogate(ucs);}
    constexpr inline bool isSurrogate() const noexcept {return QChar::isSurrogate(ucs);}

    static Category category(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static Direction direction(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static JoiningType joiningType(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static unsigned char combiningClass(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;

    static int digitValue(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static char32_t toLower(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static char32_t toUpper(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static char32_t toTitleCase(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static char32_t toCaseFolded(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static Script script(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;

    static bool isUpper(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isLower(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isDigit(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isLetter(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isLetterOrNumber(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isPrint(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isSpace(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isTitleCase(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;

    static char32_t mirroredChar(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool hasMirrored(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;

    static constexpr inline bool isNonCharacter(char32_t ucs4) noexcept {
        return ucs4 >= 0xfdd0 && (ucs4 <= 0xfdef || (ucs4 & 0xfffe) == 0xfffe);
    }
    static constexpr inline bool isHighSurrogate(char32_t ucs4) noexcept {
        return (ucs4 & 0xfffffc00) == 0xd800;
    }
    static constexpr inline bool isLowSurrogate(char32_t ucs4) noexcept {
        return (ucs4 & 0xfffffc00) == 0xdc00;
    }
    static constexpr inline bool isSurrogate(char32_t ucs4) noexcept {
        return (ucs4 - 0xd800u < 2048u);
    }
    static constexpr inline bool requiresSurrogates(char32_t ucs4) noexcept {
        return (ucs4 >= 0x10000);
    }
    static constexpr inline char32_t surrogateToUcs4(char16_t high, char16_t low) noexcept {
        return char32_t(high)<<10 + low - 0x35fdc00;
    }
    static constexpr inline char32_t surrogateToUcs4(QChar high, QChar low) noexcept {
        return surrogateToUcs4(high.unicode(), low.unicode());
    }
    static constexpr inline char16_t highSurrogate(char32_t ucs4) noexcept {
        return char16_t((ucs4>>10) + 0xd7c0);
    }
    static constexpr inline char16_t lowSurrogate(char32_t ucs4) noexcept {
        return char16_t(ucs4 % 0x400 + 0xdc00);
    }

    static UnicodeVersion unicodeVersion(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static Decomposition decompositionTag(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;

    constexpr inline char16_t unicode() const noexcept {return ucs;}
    constexpr inline char16_t& unicode() noexcept {return ucs;}

    bool compare(QChar c) const noexcept;

private:
    static bool isLetter_helper(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isLetterOrNumber_helper(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;
    static bool isSpace_helper(char32_t ucs4) noexcept Q_DECL_CONST_FUNCTION;

private:
    char16_t ucs;
};

QT_END_NAMESPACE

#endif //QCHAR_H
