//
// Created by Yujie Zhao on 2023/2/6.
//

#include <iostream>
#include <QChar>
#include <QTest>
#include <sstream>
#include <QObject>
#include <private/qunicodetables_p.h>

/*
 * QChar类的测试文件
 * */

//10进制转16进制
static std::string numToHex(int num) {
    std::stringstream ss;
    ss << std::hex << num;
    return ss.str();
}

class tst_QChar : public QObject{
public:
    using QObject::QObject;
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(fromChar16_t)
        REGISTER_OBJECT_INVOKE_METHOD(fromUcs4_data)
        REGISTER_OBJECT_INVOKE_METHOD(fromUcs4)
        REGISTER_OBJECT_INVOKE_METHOD(fromWchar_t)
        REGISTER_OBJECT_INVOKE_METHOD(operator_eqeq_null)
        REGISTER_OBJECT_INVOKE_METHOD(operators_data)
        REGISTER_OBJECT_INVOKE_METHOD(operators)
        REGISTER_OBJECT_INVOKE_METHOD(toUpper)
        REGISTER_OBJECT_INVOKE_METHOD(toLower)
        REGISTER_OBJECT_INVOKE_METHOD(toTitle)
        REGISTER_OBJECT_INVOKE_METHOD(toCaseFolded)
        REGISTER_OBJECT_INVOKE_METHOD(isDigit_data)
        REGISTER_OBJECT_INVOKE_METHOD(isDigit)
        REGISTER_OBJECT_INVOKE_METHOD(isLetter_data)
        REGISTER_OBJECT_INVOKE_METHOD(isLetter)
        REGISTER_OBJECT_INVOKE_METHOD(isLetterOrNumber_data)
        REGISTER_OBJECT_INVOKE_METHOD(isLetterOrNumber)
        REGISTER_OBJECT_INVOKE_METHOD(isPrint)
        REGISTER_OBJECT_INVOKE_METHOD(isUpper)
        REGISTER_OBJECT_INVOKE_METHOD(isLower)
        REGISTER_OBJECT_INVOKE_METHOD(isTitleCase)
        REGISTER_OBJECT_INVOKE_METHOD(isSpace_data)
        REGISTER_OBJECT_INVOKE_METHOD(isSpace)
        REGISTER_OBJECT_INVOKE_METHOD(isSpaceSpecial)
        REGISTER_OBJECT_INVOKE_METHOD(category)
        REGISTER_OBJECT_INVOKE_METHOD(direction)
        REGISTER_OBJECT_INVOKE_METHOD(joiningType)
        REGISTER_OBJECT_INVOKE_METHOD(combiningClass)
        REGISTER_OBJECT_INVOKE_METHOD(digitValue)
        REGISTER_OBJECT_INVOKE_METHOD(mirroredChar)
        REGISTER_OBJECT_INVOKE_METHOD(decomposition)
        REGISTER_OBJECT_INVOKE_METHOD(lineBreakClass)
        REGISTER_OBJECT_INVOKE_METHOD(script)
        REGISTER_OBJECT_INVOKE_METHOD(normalization_data)
        REGISTER_OBJECT_INVOKE_METHOD(normalization)
        REGISTER_OBJECT_INVOKE_METHOD(normalization_manual)
        REGISTER_OBJECT_INVOKE_METHOD(normalizationCorrections)
        REGISTER_OBJECT_INVOKE_METHOD(unicodeVersion)
    }

    void fromChar16_t();
    void fromUcs4_data();
    void fromUcs4();
    void fromWchar_t();
    void operator_eqeq_null();
    void operators_data();
    void operators();
    void toUpper();
    void toLower();
    void toTitle();
    void toCaseFolded();
    void isDigit_data();
    void isDigit();
    void isLetter_data();
    void isLetter();
    void isLetterOrNumber_data();
    void isLetterOrNumber();
    void isPrint();
    void isUpper();
    void isLower();
    void isTitleCase();
    void isSpace_data();
    void isSpace();
    void isSpaceSpecial();
    void category();
    void direction();
    void joiningType();
    void combiningClass();
    void digitValue();
    void mirroredChar();
    void decomposition();
    void lineBreakClass();
    void script();
    void normalization_data();
    void normalization();
    void normalization_manual();
    void normalizationCorrections();
    void unicodeVersion();
};

void tst_QChar::fromChar16_t()
{
    QChar aUmlaut = u'\u00E4';
    QCOMPARE(aUmlaut, QChar(0xE4));
    QChar replacementCharacter = u'\uFFFD';
    QCOMPARE(replacementCharacter, QChar(QChar::ReplacementCharacter));
}

void tst_QChar::fromUcs4_data()
{
    //zhaoyujie TODO
//    QTest::addColumn<uint>("ucs4");
//    auto row =[](uint ucs4) {
//        QTest::addRow("0x%08X", ucs4) << ucs4;
//    };
//    row(0x2f868);
//    row(0x1D157);
//    row(0x1D157);
}

void tst_QChar::fromUcs4()
{
    //zhaoyujie TODO
//    QFETCH(const uint, ucs4);
//    const auto result = QChar::fromUcs4(ucs4);
}

void tst_QChar::fromWchar_t()
{
    //zhaoyujie TODO； wchar_t windows onlu
}

void tst_QChar::operator_eqeq_null()
{
    {
        const QChar ch = QLatin1Char(' ');
#define CHECK(NUL) \
    do {            \
        QVERIFY(!(ch == NUL));  \
        QVERIFY(  ch != NUL );  \
        QVERIFY(!(ch <  NUL));  \
        QVERIFY(  ch >  NUL );  \
        QVERIFY(!(ch <= NUL));  \
        QVERIFY(  ch >= NUL );  \
        QVERIFY(!(NUL == ch));  \
        QVERIFY(  NUL != ch );  \
        QVERIFY(  NUL <  ch );  \
        QVERIFY(!(NUL >  ch));  \
        QVERIFY(  NUL <= ch );  \
        QVERIFY(!(NUL >= ch));  \
    } while (0);

        QVERIFY(  0 <= ch );
        CHECK(0);
        CHECK('\0');
#undef CHECK
    }

    {
        const QChar ch = QLatin1Char('\0');
#define CHECK(NUL) \
    do {           \
        QVERIFY(  ch == NUL ); \
        QVERIFY(!(ch != NUL));           \
        QVERIFY(!(ch <  NUL)); \
        QVERIFY(  ch <= NUL);           \
        QVERIFY(!(ch >  NUL)); \
        QVERIFY(  ch >= NUL);  \
        QVERIFY(  NUL == ch);  \
        QVERIFY(!(NUL != ch)); \
        QVERIFY(!(NUL < ch));  \
        QVERIFY(  NUL <= ch);   \
        QVERIFY(!(NUL > ch));  \
        QVERIFY(  NUL >= ch);\
    }while(0);
        CHECK(0);
        CHECK('\0');
    }

#undef CHECK
}

void tst_QChar::operators_data()
{
    QTest::addColumn<QChar>("lhs");
    QTest::addColumn<QChar>("rhs");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QTest::addRow("'\\%d' (op) '\\%d'", i, j) << QChar(ushort(i)) << QChar(ushort(j));
        }
    }
}

void tst_QChar::operators()
{
    QFETCH(QChar, lhs);
    QFETCH(QChar, rhs);

#define CHECK(op) QCOMPARE((lhs op rhs), (lhs.unicode() op rhs.unicode()))
    CHECK(==);
    CHECK(!=);
    CHECK(< );
    CHECK(>);
    CHECK(<=);
    CHECK(>=);
#undef CHECK
}

void tst_QChar::toUpper() {
    QVERIFY(QChar('a').toUpper() == QChar('A'));
    QVERIFY(QChar('A').toUpper() == QChar('A'));

    QVERIFY(QChar(0x1c7).toUpper().unicode() == 0x1c7);
    QVERIFY(QChar(0x1c8).toUpper().unicode() == 0x1c7);
    QVERIFY(QChar(0x1c9).toUpper().unicode() == 0x1c7);
    QVERIFY(QChar(0x25c).toUpper().unicode() == 0xa7ab);
    QVERIFY(QChar(0x29e).toUpper().unicode() == 0xa7b0);
    QVERIFY(QChar(0x1d79).toUpper().unicode() == 0xa77d);
    QVERIFY(QChar(0x0265).toUpper().unicode() == 0xa78d);

    QVERIFY(QChar::toUpper('a') == 'A');
    QVERIFY(QChar::toUpper('A') == 'A');
    QVERIFY(QChar::toUpper(0xdf) == 0xdf); // german sharp s
    QVERIFY(QChar::toUpper(0x1c7) == 0x1c7);
    QVERIFY(QChar::toUpper(0x1c8) == 0x1c7);
    QVERIFY(QChar::toUpper(0x1c9) == 0x1c7);
    QVERIFY(QChar::toUpper(0x25c) == 0xa7ab);
    QVERIFY(QChar::toUpper(0x29e) == 0xa7b0);
    QVERIFY(QChar::toUpper(0x1d79) == 0xa77d);
    QVERIFY(QChar::toUpper(0x0265) == 0xa78d);

    QVERIFY(QChar::toUpper(0x10400) == 0x10400);
    QVERIFY(QChar::toUpper(0x10428) == 0x10400);
}

void tst_QChar::toLower() {
    QVERIFY(QChar('A').toLower() == 'a');
    QVERIFY(QChar('a').toLower() == 'a');
    QVERIFY(QChar(0x1c7).toLower().unicode() == 0x1c9);
    QVERIFY(QChar(0x1c8).toLower().unicode() == 0x1c9);
    QVERIFY(QChar(0x1c9).toLower().unicode() == 0x1c9);
    QVERIFY(QChar(0xa77d).toLower().unicode() == 0x1d79);
    QVERIFY(QChar(0xa78d).toLower().unicode() == 0x0265);
    QVERIFY(QChar(0xa7ab).toLower().unicode() == 0x25c);
    QVERIFY(QChar(0xa7b1).toLower().unicode() == 0x287);

    QVERIFY(QChar::toLower('a') == 'a');
    QVERIFY(QChar::toLower('A') == 'a');
    QVERIFY(QChar::toLower(0x1c7) == 0x1c9);
    QVERIFY(QChar::toLower(0x1c8) == 0x1c9);
    QVERIFY(QChar::toLower(0x1c9) == 0x1c9);
    QVERIFY(QChar::toLower(0xa77d) == 0x1d79);
    QVERIFY(QChar::toLower(0xa78d) == 0x0265);
    QVERIFY(QChar::toLower(0xa7ab) == 0x25c);
    QVERIFY(QChar::toLower(0xa7b1) == 0x287);

    QVERIFY(QChar::toLower(0x10400) == 0x10428);
    QVERIFY(QChar::toLower(0x10428) == 0x10428);
}

void tst_QChar::toTitle() {
    QVERIFY(QChar('a').toTitleCase() == 'A');
    QVERIFY(QChar('A').toTitleCase() == 'A');
    QVERIFY(QChar(0x1c7).toTitleCase().unicode() == 0x1c8);
    QVERIFY(QChar(0x1c8).toTitleCase().unicode() == 0x1c8);
    QVERIFY(QChar(0x1c9).toTitleCase().unicode() == 0x1c8);
    QVERIFY(QChar(0x1d79).toTitleCase().unicode() == 0xa77d);
    QVERIFY(QChar(0x0265).toTitleCase().unicode() == 0xa78d);

    QVERIFY(QChar::toTitleCase('a') == 'A');
    QVERIFY(QChar::toTitleCase('A') == 'A');
    QVERIFY(QChar::toTitleCase(0xdf) == 0xdf); // german sharp s
    QVERIFY(QChar::toTitleCase(0x1c7) == 0x1c8);
    QVERIFY(QChar::toTitleCase(0x1c8) == 0x1c8);
    QVERIFY(QChar::toTitleCase(0x1c9) == 0x1c8);
    QVERIFY(QChar::toTitleCase(0x1d79) == 0xa77d);
    QVERIFY(QChar::toTitleCase(0x0265) == 0xa78d);

    QVERIFY(QChar::toTitleCase(0x10400) == 0x10400);
    QVERIFY(QChar::toTitleCase(0x10428) == 0x10400);
}

void tst_QChar::toCaseFolded() {
    QVERIFY(QChar('a').toCaseFolded() == 'a');
    QVERIFY(QChar('A').toCaseFolded() == 'a');
    QVERIFY(QChar(0x1c7).toCaseFolded().unicode() == 0x1c9);
    QVERIFY(QChar(0x1c8).toCaseFolded().unicode() == 0x1c9);
    QVERIFY(QChar(0x1c9).toCaseFolded().unicode() == 0x1c9);
    QVERIFY(QChar(0xa77d).toCaseFolded().unicode() == 0x1d79);
    QVERIFY(QChar(0xa78d).toCaseFolded().unicode() == 0x0265);
    QVERIFY(QChar(0xa7ab).toCaseFolded().unicode() == 0x25c);
    QVERIFY(QChar(0xa7b1).toCaseFolded().unicode() == 0x287);

    QVERIFY(QChar::toCaseFolded('a') == 'a');
    QVERIFY(QChar::toCaseFolded('A') == 'a');
    QVERIFY(QChar::toCaseFolded(0x1c7) == 0x1c9);
    QVERIFY(QChar::toCaseFolded(0x1c8) == 0x1c9);
    QVERIFY(QChar::toCaseFolded(0x1c9) == 0x1c9);
    QVERIFY(QChar::toCaseFolded(0xa77d) == 0x1d79);
    QVERIFY(QChar::toCaseFolded(0xa78d) == 0x0265);
    QVERIFY(QChar::toCaseFolded(0xa7ab) == 0x25c);
    QVERIFY(QChar::toCaseFolded(0xa7b1) == 0x287);

    QVERIFY(QChar::toCaseFolded(0x10400) == 0x10428);
    QVERIFY(QChar::toCaseFolded(0x10428) == 0x10428);

    QVERIFY(QChar::toCaseFolded(0xb5) == 0x3bc);
}

void tst_QChar::isPrint() {
    // noncharacters, reserved (General_Gategory =Cn)
    QVERIFY(!QChar(0x2064).isPrint());
    QVERIFY(!QChar(0x2069).isPrint());
    QVERIFY(!QChar(0xfdd0).isPrint());
    QVERIFY(!QChar(0xfdef).isPrint());
    QVERIFY(!QChar(0xfff0).isPrint());
    QVERIFY(!QChar(0xfff8).isPrint());
    QVERIFY(!QChar(0xfffe).isPrint());
    QVERIFY(!QChar(0xffff).isPrint());
    QVERIFY(!QChar::isPrint(0xe0000));
    QVERIFY(!QChar::isPrint(0xe0002));
    QVERIFY(!QChar::isPrint(0xe001f));
    QVERIFY(!QChar::isPrint(0xe0080));
    QVERIFY(!QChar::isPrint(0xe00ff));

    // Other_Default_Ignorable_Code_Point, Variation_Selector
    QVERIFY(QChar(0x034f).isPrint());
    QVERIFY(QChar(0x115f).isPrint());
    QVERIFY(QChar(0x180b).isPrint());
    QVERIFY(QChar(0x180d).isPrint());
    QVERIFY(QChar(0x3164).isPrint());
    QVERIFY(QChar(0xfe00).isPrint());
    QVERIFY(QChar(0xfe0f).isPrint());
    QVERIFY(QChar(0xffa0).isPrint());
    QVERIFY(QChar::isPrint(0xe0100));
    QVERIFY(QChar::isPrint(0xe01ef));

    // Cf, Cs, Cc, White_Space, Annotation Characters
    QVERIFY(!QChar(0x0008).isPrint());
    QVERIFY(!QChar(0x000a).isPrint());
    QVERIFY(QChar(0x0020).isPrint());
    QVERIFY(QChar(0x00a0).isPrint());
    QVERIFY(!QChar(0x00ad).isPrint());
    QVERIFY(!QChar(0x0085).isPrint());
    QVERIFY(!QChar(0xd800).isPrint());
    QVERIFY(!QChar(0xdc00).isPrint());
    QVERIFY(!QChar(0xfeff).isPrint());
    QVERIFY(!QChar::isPrint(0x1d173));

    QVERIFY(QChar('0').isPrint());
    QVERIFY(QChar('A').isPrint());
    QVERIFY(QChar('a').isPrint());

    QVERIFY(QChar(0x0370).isPrint()); // assigned in 5.1
    QVERIFY(QChar(0x0524).isPrint()); // assigned in 5.2
    QVERIFY(QChar(0x0526).isPrint()); // assigned in 6.0
    QVERIFY(QChar(0x08a0).isPrint()); // assigned in 6.1
    QVERIFY(!QChar(0x1aff).isPrint()); // not assigned
    QVERIFY(QChar(0x1e9e).isPrint()); // assigned in 5.1
    QVERIFY(QChar::isPrint(0x1b000)); // assigned in 6.0
    QVERIFY(QChar::isPrint(0x110d0)); // assigned in 5.1
    QVERIFY(!QChar::isPrint(0x1bca0)); // assigned in 7.0
}

void tst_QChar::isUpper() {
    QVERIFY(QChar('A').isUpper());
    QVERIFY(QChar('Z').isUpper());
    QVERIFY(!QChar('a').isUpper());
    QVERIFY(!QChar('z').isUpper());
    QVERIFY(!QChar('?').isUpper());
    QVERIFY(QChar(0xC2).isUpper());   // A with ^
    QVERIFY(!QChar(0xE2).isUpper());  // a with ^

    for (uint codepoint = 0; codepoint <= QChar::LastValidCodePoint; ++codepoint) {
        if (QChar::isUpper(codepoint))
            QVERIFY(codepoint == QChar::toUpper(codepoint));
    }
}

void tst_QChar::isLower() {
    QVERIFY(!QChar('A').isLower());
    QVERIFY(!QChar('Z').isLower());
    QVERIFY(QChar('a').isLower());
    QVERIFY(QChar('z').isLower());
    QVERIFY(!QChar('?').isLower());
    QVERIFY(!QChar(0xC2).isLower());   // A with ^
    QVERIFY(QChar(0xE2).isLower());  // a with ^

    for (uint codepoint = 0; codepoint <= QChar::LastValidCodePoint; ++codepoint) {
        if (QChar::isLower(codepoint))
            QVERIFY(codepoint == QChar::toLower(codepoint));
    }
}

void tst_QChar::isDigit_data() {
    QTest::addColumn<ushort>("ucs");
    QTest::addColumn<bool>("expected");
    for (ushort ucs = 0; ucs < 256; ++ucs) {
        bool isDigit = (ucs <= '9' && ucs >= '0');
        auto hexStr = "0x" + numToHex(ucs);
        QTest::newRow(hexStr.c_str()) << ucs <<isDigit;
    }
}

void tst_QChar::isDigit()
{
    QFETCH(ushort, ucs);
    QFETCH(bool, expected);
    QCOMPARE(QChar(ucs).isDigit(), expected);
}

static bool isExpectedLetter(ushort ucs)
{
    return (ucs >= 'a' && ucs <= 'z') || (ucs >= 'A' && ucs <= 'Z')
           || ucs == 0xAA || ucs == 0xB5 || ucs == 0xBA
           || (ucs >= 0xC0 && ucs <= 0xD6)
           || (ucs >= 0xD8 && ucs <= 0xF6)
           || (ucs >= 0xF8 && ucs <= 0xFF);
}

void tst_QChar::isLetter_data()
{
    QTest::addColumn<ushort>("ucs");
    QTest::addColumn<bool>("expected");
    for (ushort ucs = 0; ucs < 256; ++ucs) {
        auto hexStr = "0x" + numToHex(ucs);
        QTest::newRow(hexStr.c_str()) << ucs << isExpectedLetter(ucs);
    }
}

void tst_QChar::isLetter()
{
    QFETCH(ushort, ucs);
    QFETCH(bool, expected);
    QCOMPARE(QChar(ucs).isLetter(), expected);
}

void tst_QChar::isLetterOrNumber_data()
{
    QTest::addColumn<ushort>("ucs");
    QTest::addColumn<bool>("expected");
    for (ushort ucs = 0; ucs < 256; ++ucs) {
        bool isLetterOrNumber = isExpectedLetter(ucs)
                || (ucs >= '0' && ucs <= '9')
                || ucs == 0xB2 || ucs == 0xB3 || ucs == 0xB9
                || (ucs >= 0xBC && ucs <= 0xBE);
        auto hexStr = "0x" + numToHex(ucs);
        QTest::newRow(hexStr.c_str()) << ucs << isLetterOrNumber;
    }
}

void tst_QChar::isLetterOrNumber()
{
    QFETCH(ushort, ucs);
    QFETCH(bool, expected);
    QCOMPARE(QChar(ucs).isLetterOrNumber(), expected);
}

void tst_QChar::isTitleCase()
{
    for (uint codepoint = 0; codepoint <= QChar::LastValidCodePoint; ++codepoint) {
        if (QChar::isTitleCase(codepoint)) {
            QVERIFY(codepoint == QChar::toTitleCase(codepoint));
        }
    }
}

void tst_QChar::isSpace_data() {
    QTest::addColumn<ushort>("ucs");
    QTest::addColumn<bool>("expected");

    for (ushort ucs = 0; ucs < 256; ++ucs) {
        bool isSpace = (ucs <= 0x0D && ucs >= 0x09) || ucs == 0x20 || ucs == 0xA0 || ucs == 0x85;
        auto tag = "0x" + numToHex(ucs);
        QTest::newRow(tag.c_str()) << ucs << isSpace;
    }
}

void tst_QChar::isSpace() {
    QFETCH(ushort, ucs);
    QFETCH(bool, expected);
    QCOMPARE(QChar(ucs).isSpace(), expected);
}

void tst_QChar::isSpaceSpecial() {
    QVERIFY(!QChar(QChar::Null).isSpace());
    QVERIFY(QChar(QChar::Nbsp).isSpace());
    QVERIFY(QChar(QChar::ParagraphSeparator).isSpace());
    QVERIFY(QChar(QChar::LineSeparator).isSpace());
    QVERIFY(QChar(0x1680).isSpace());
}

void tst_QChar::category() {
    QVERIFY(QChar('a').category() == QChar::Letter_Lowercase);
    QVERIFY(QChar('A').category() == QChar::Letter_Uppercase);

    QVERIFY(QChar::category('a') == QChar::Letter_Lowercase);
    QVERIFY(QChar::category('A') == QChar::Letter_Uppercase);

    QVERIFY(QChar::category(0xe0100) == QChar::Mark_NonSpacing);
    QVERIFY(QChar::category(0xeffff) != QChar::Other_PrivateUse);
    QVERIFY(QChar::category(0xf0000) == QChar::Other_PrivateUse);
    QVERIFY(QChar::category(0xf0001) == QChar::Other_PrivateUse);

    QVERIFY(QChar::category(0xd900) == QChar::Other_Surrogate);
    QVERIFY(QChar::category(0xdc00) == QChar::Other_Surrogate);
    QVERIFY(QChar::category(0xdc01) == QChar::Other_Surrogate);

    QVERIFY(QChar::category(0x1aff) == QChar::Other_NotAssigned);
    QVERIFY(QChar::category(0x10fffd) == QChar::Other_PrivateUse);
    QVERIFY(QChar::category(0x10ffff) == QChar::Other_NotAssigned);
    QVERIFY(QChar::category(0x110000) == QChar::Other_NotAssigned);
}

void tst_QChar::direction()
{
    QVERIFY(QChar::direction(0x200E) == QChar::DirL);
    QVERIFY(QChar::direction(0x200F) == QChar::DirR);
    QVERIFY(QChar::direction(0x202A) == QChar::DirLRE);
    QVERIFY(QChar::direction(0x202B) == QChar::DirRLE);
    QVERIFY(QChar::direction(0x202C) == QChar::DirPDF);
    QVERIFY(QChar::direction(0x202D) == QChar::DirLRO);
    QVERIFY(QChar::direction(0x202E) == QChar::DirRLO);
    QVERIFY(QChar::direction(0x2066) == QChar::DirLRI);
    QVERIFY(QChar::direction(0x2067) == QChar::DirRLI);
    QVERIFY(QChar::direction(0x2068) == QChar::DirFSI);
    QVERIFY(QChar::direction(0x2069) == QChar::DirPDI);

    QVERIFY(QChar('a').direction() == QChar::DirL);
    QVERIFY(QChar('0').direction() == QChar::DirEN);
    QVERIFY(QChar(0x627).direction() == QChar::DirAL);
    QVERIFY(QChar(0x5d0).direction() == QChar::DirR);

    QVERIFY(QChar::direction('a') == QChar::DirL);
    QVERIFY(QChar::direction('0') == QChar::DirEN);
    QVERIFY(QChar::direction(0x627) == QChar::DirAL);
    QVERIFY(QChar::direction(0x5d0) == QChar::DirR);

    QVERIFY(QChar::direction(0xE01DA) == QChar::DirNSM);
    QVERIFY(QChar::direction(0xf0000) == QChar::DirL);
    QVERIFY(QChar::direction(0xE0030) == QChar::DirBN);
    QVERIFY(QChar::direction(0x2FA17) == QChar::DirL);
}

void tst_QChar::joiningType() {
    QVERIFY(QChar('a').joiningType() == QChar::Joining_None);
    QVERIFY(QChar('0').joiningType() == QChar::Joining_None);
    QVERIFY(QChar(0x0627).joiningType() == QChar::Joining_Right);
    QVERIFY(QChar(0x05d0).joiningType() == QChar::Joining_None);
    QVERIFY(QChar(0x00ad).joiningType() == QChar::Joining_Transparent);
    QVERIFY(QChar(0xA872).joiningType() == QChar::Joining_Left);

    QVERIFY(QChar::joiningType('a') == QChar::Joining_None);
    QVERIFY(QChar::joiningType('0') == QChar::Joining_None);
    QVERIFY(QChar::joiningType(0x0627) == QChar::Joining_Right);
    QVERIFY(QChar::joiningType(0x05d0) == QChar::Joining_None);
    QVERIFY(QChar::joiningType(0x00ad) == QChar::Joining_Transparent);

    QVERIFY(QChar::joiningType(0xE01DA) == QChar::Joining_Transparent);
    QVERIFY(QChar::joiningType(0xf0000) == QChar::Joining_None);
    QVERIFY(QChar::joiningType(0xE0030) == QChar::Joining_Transparent);
    QVERIFY(QChar::joiningType(0x2FA17) == QChar::Joining_None);

    QVERIFY(QChar::joiningType(0xA872) == QChar::Joining_Left);
    QVERIFY(QChar::joiningType(0x10ACD) == QChar::Joining_Left);
    QVERIFY(QChar::joiningType(0x10AD7) == QChar::Joining_Left);
}

void tst_QChar::combiningClass() {
    QVERIFY(QChar('a').combiningClass() == 0);
    QVERIFY(QChar('0').combiningClass() == 0);
    QVERIFY(QChar(0x627).combiningClass() == 0);
    QVERIFY(QChar(0x5d0).combiningClass() == 0);

    QVERIFY(QChar::combiningClass('a') == 0);
    QVERIFY(QChar::combiningClass('0') == 0);
    QVERIFY(QChar::combiningClass(0x627) == 0);
    QVERIFY(QChar::combiningClass(0x5d0) == 0);

    QVERIFY(QChar::combiningClass(0xE01DA) == 0);
    QVERIFY(QChar::combiningClass(0xf0000) == 0);
    QVERIFY(QChar::combiningClass(0xE0030) == 0);
    QVERIFY(QChar::combiningClass(0x2FA17) == 0);

    QVERIFY(QChar::combiningClass(0x300) == 230);

    QVERIFY(QChar::combiningClass(0x1d244) == 230);
}

void tst_QChar::digitValue()
{
    QVERIFY(QChar('9').digitValue() == 9);
    QVERIFY(QChar('0').digitValue() == 0);
    QVERIFY(QChar('a').digitValue() == -1);

    QVERIFY(QChar::digitValue('9') == 9);
    QVERIFY(QChar::digitValue('0') == 0);

    QVERIFY(QChar::digitValue(0x1049) == 9);
    QVERIFY(QChar::digitValue(0x1040) == 0);

    QVERIFY(QChar::digitValue(0xd800) == -1);
    QVERIFY(QChar::digitValue(0x110000) == -1);
}

void tst_QChar::mirroredChar()
{
    QVERIFY(QChar(0x169B).hasMirrored());
    QVERIFY(QChar(0x169B).mirroredChar() == QChar(0x169C));
    QVERIFY(QChar(0x169C).hasMirrored());
    QVERIFY(QChar(0x169C).mirroredChar() == QChar(0x169B));

    QVERIFY(QChar(0x301A).hasMirrored());
    QVERIFY(QChar(0x301A).mirroredChar() == QChar(0x301B));
    QVERIFY(QChar(0x301B).hasMirrored());
    QVERIFY(QChar(0x301B).mirroredChar() == QChar(0x301A));
}

void tst_QChar::decomposition() {
    //zhaoyujie TODO
//    for (uint ucs = 0xac00; ucs <= 0xd7af; ++ucs) {
//        QChar::Decomposition expected = QChar::unicodeVersion(ucs) > QChar::Unicode_Unassigned ? QChar::Canonical : QChar::NoDecomposition;
//        QString desc = QString::fromLatin1("ucs = 0x%1, tag = %2, expected = %3")
//                .arg(QString::number(ucs, 16)).arg(QChar::decompositionTag(ucs)).arg(expected);
//        QVERIFY2(QChar::decompositionTag(ucs) == expected, desc.toLatin1());
//    }
//
//    QVERIFY(QChar(0xa0).decompositionTag() == QChar::NoBreak);
//    QVERIFY(QChar(0xa8).decompositionTag() == QChar::Compat);
//    QVERIFY(QChar(0x41).decompositionTag() == QChar::NoDecomposition);
//
//    QVERIFY(QChar::decompositionTag(0xa0) == QChar::NoBreak);
//    QVERIFY(QChar::decompositionTag(0xa8) == QChar::Compat);
//    QVERIFY(QChar::decompositionTag(0x41) == QChar::NoDecomposition);
//
//    QVERIFY(QChar::decomposition(0xa0) == QString(QChar(0x20)));
//    QVERIFY(QChar::decomposition(0xc0) == (QString(QChar(0x41)) + QString(QChar(0x300))));
//
//    {
//        QString str;
//        str += QChar(QChar::highSurrogate(0x1D157));
//        str += QChar(QChar::lowSurrogate(0x1D157));
//        str += QChar(QChar::highSurrogate(0x1D165));
//        str += QChar(QChar::lowSurrogate(0x1D165));
//        QVERIFY(QChar::decomposition(0x1D15e) == str);
//    }
//
//    {
//        QString str;
//        str += QChar(0x1100);
//        str += QChar(0x1161);
//        QVERIFY(QChar::decomposition(0xac00) == str);
//    }
//    {
//        QString str;
//        str += QChar(0x110c);
//        str += QChar(0x1165);
//        str += QChar(0x11b7);
//        QVERIFY(QChar::decomposition(0xc810) == str);
//    }
}

void tst_QChar::lineBreakClass()
{
    QVERIFY(QUnicodeTables::lineBreakClass(0x0029) == QUnicodeTables::LineBreak_CP);
    QVERIFY(QUnicodeTables::lineBreakClass(0x0041) == QUnicodeTables::LineBreak_AL);
    QVERIFY(QUnicodeTables::lineBreakClass(0x0033) == QUnicodeTables::LineBreak_NU);
    QVERIFY(QUnicodeTables::lineBreakClass(0x00ad) == QUnicodeTables::LineBreak_BA);
    QVERIFY(QUnicodeTables::lineBreakClass(0x05d0) == QUnicodeTables::LineBreak_HL);
    QVERIFY(QUnicodeTables::lineBreakClass(0xfffc) == QUnicodeTables::LineBreak_CB);
    QVERIFY(QUnicodeTables::lineBreakClass(0xe0164) == QUnicodeTables::LineBreak_CM);
    QVERIFY(QUnicodeTables::lineBreakClass(0x2f9a4) == QUnicodeTables::LineBreak_ID);
    QVERIFY(QUnicodeTables::lineBreakClass(0x10000) == QUnicodeTables::LineBreak_AL);
    QVERIFY(QUnicodeTables::lineBreakClass(0x1f1e6) == QUnicodeTables::LineBreak_RI);

    // mapped to AL:
    QVERIFY(QUnicodeTables::lineBreakClass(0xfffd) == QUnicodeTables::LineBreak_AL); // AI -> AL
    QVERIFY(QUnicodeTables::lineBreakClass(0x100000) == QUnicodeTables::LineBreak_AL); // XX -> AL
}

void tst_QChar::script()
{
    QVERIFY(QChar::script(0x0020) == QChar::Script_Common);
    QVERIFY(QChar::script(0x0041) == QChar::Script_Latin);
    QVERIFY(QChar::script(0x0375) == QChar::Script_Greek);
    QVERIFY(QChar::script(0x0400) == QChar::Script_Cyrillic);
    QVERIFY(QChar::script(0x0531) == QChar::Script_Armenian);
    QVERIFY(QChar::script(0x0591) == QChar::Script_Hebrew);
    QVERIFY(QChar::script(0x0600) == QChar::Script_Arabic);
    QVERIFY(QChar::script(0x0700) == QChar::Script_Syriac);
    QVERIFY(QChar::script(0x0780) == QChar::Script_Thaana);
    QVERIFY(QChar::script(0x07c0) == QChar::Script_Nko);
    QVERIFY(QChar::script(0x0900) == QChar::Script_Devanagari);
    QVERIFY(QChar::script(0x0981) == QChar::Script_Bengali);
    QVERIFY(QChar::script(0x0a01) == QChar::Script_Gurmukhi);
    QVERIFY(QChar::script(0x0a81) == QChar::Script_Gujarati);
    QVERIFY(QChar::script(0x0b01) == QChar::Script_Oriya);
    QVERIFY(QChar::script(0x0b82) == QChar::Script_Tamil);
    QVERIFY(QChar::script(0x0c01) == QChar::Script_Telugu);
    QVERIFY(QChar::script(0x0c82) == QChar::Script_Kannada);
    QVERIFY(QChar::script(0x0d02) == QChar::Script_Malayalam);
    QVERIFY(QChar::script(0x0d82) == QChar::Script_Sinhala);
    QVERIFY(QChar::script(0x0e01) == QChar::Script_Thai);
    QVERIFY(QChar::script(0x0e81) == QChar::Script_Lao);
    QVERIFY(QChar::script(0x0f00) == QChar::Script_Tibetan);
    QVERIFY(QChar::script(0x1000) == QChar::Script_Myanmar);
    QVERIFY(QChar::script(0x10a0) == QChar::Script_Georgian);
    QVERIFY(QChar::script(0x1100) == QChar::Script_Hangul);
    QVERIFY(QChar::script(0x1680) == QChar::Script_Ogham);
    QVERIFY(QChar::script(0x16a0) == QChar::Script_Runic);
    QVERIFY(QChar::script(0x1780) == QChar::Script_Khmer);
    QVERIFY(QChar::script(0x200c) == QChar::Script_Inherited);
    QVERIFY(QChar::script(0x200d) == QChar::Script_Inherited);
    QVERIFY(QChar::script(0x1018a) == QChar::Script_Greek);
    QVERIFY(QChar::script(0x1f130) == QChar::Script_Common);
    QVERIFY(QChar::script(0xe0100) == QChar::Script_Inherited);
}

void tst_QChar::normalization_data() {

}

void tst_QChar::normalization() {

}

void tst_QChar::normalization_manual() {

}

void tst_QChar::normalizationCorrections() {

}

void tst_QChar::unicodeVersion() {
    QVERIFY(QChar('a').unicodeVersion() == QChar::Unicode_1_1);
    QVERIFY(QChar('0').unicodeVersion() == QChar::Unicode_1_1);
    QVERIFY(QChar(0x627).unicodeVersion() == QChar::Unicode_1_1);
    QVERIFY(QChar(0x5d0).unicodeVersion() == QChar::Unicode_1_1);

    QVERIFY(QChar::unicodeVersion('a') == QChar::Unicode_1_1);
    QVERIFY(QChar::unicodeVersion('0') == QChar::Unicode_1_1);
    QVERIFY(QChar::unicodeVersion(0x627) == QChar::Unicode_1_1);
    QVERIFY(QChar::unicodeVersion(0x5d0) == QChar::Unicode_1_1);

    QVERIFY(QChar(0x0591).unicodeVersion() == QChar::Unicode_2_0);
    QVERIFY(QChar::unicodeVersion(0x0591) == QChar::Unicode_2_0);

    QVERIFY(QChar(0x20AC).unicodeVersion() == QChar::Unicode_2_1_2);
    QVERIFY(QChar::unicodeVersion(0x20AC) == QChar::Unicode_2_1_2);
    QVERIFY(QChar(0xfffc).unicodeVersion() == QChar::Unicode_2_1_2);
    QVERIFY(QChar::unicodeVersion(0xfffc) == QChar::Unicode_2_1_2);

    QVERIFY(QChar(0x01f6).unicodeVersion() == QChar::Unicode_3_0);
    QVERIFY(QChar::unicodeVersion(0x01f6) == QChar::Unicode_3_0);

    QVERIFY(QChar(0x03F4).unicodeVersion() == QChar::Unicode_3_1);
    QVERIFY(QChar::unicodeVersion(0x03F4) == QChar::Unicode_3_1);
    QVERIFY(QChar::unicodeVersion(0x10300) == QChar::Unicode_3_1);

    QVERIFY(QChar(0x0220).unicodeVersion() == QChar::Unicode_3_2);
    QVERIFY(QChar::unicodeVersion(0x0220) == QChar::Unicode_3_2);
    QVERIFY(QChar::unicodeVersion(0xFF5F) == QChar::Unicode_3_2);

    QVERIFY(QChar(0x0221).unicodeVersion() == QChar::Unicode_4_0);
    QVERIFY(QChar::unicodeVersion(0x0221) == QChar::Unicode_4_0);
    QVERIFY(QChar::unicodeVersion(0x10000) == QChar::Unicode_4_0);

    QVERIFY(QChar(0x0237).unicodeVersion() == QChar::Unicode_4_1);
    QVERIFY(QChar::unicodeVersion(0x0237) == QChar::Unicode_4_1);
    QVERIFY(QChar::unicodeVersion(0x10140) == QChar::Unicode_4_1);

    QVERIFY(QChar(0x0242).unicodeVersion() == QChar::Unicode_5_0);
    QVERIFY(QChar::unicodeVersion(0x0242) == QChar::Unicode_5_0);
    QVERIFY(QChar::unicodeVersion(0x12000) == QChar::Unicode_5_0);

    QVERIFY(QChar(0x0370).unicodeVersion() == QChar::Unicode_5_1);
    QVERIFY(QChar::unicodeVersion(0x0370) == QChar::Unicode_5_1);
    QVERIFY(QChar::unicodeVersion(0x1f093) == QChar::Unicode_5_1);

    QVERIFY(QChar(0x0524).unicodeVersion() == QChar::Unicode_5_2);
    QVERIFY(QChar::unicodeVersion(0x0524) == QChar::Unicode_5_2);
    QVERIFY(QChar::unicodeVersion(0x2b734) == QChar::Unicode_5_2);

    QVERIFY(QChar(0x26ce).unicodeVersion() == QChar::Unicode_6_0);
    QVERIFY(QChar::unicodeVersion(0x26ce) == QChar::Unicode_6_0);
    QVERIFY(QChar::unicodeVersion(0x1f618) == QChar::Unicode_6_0);

    QVERIFY(QChar(0xa69f).unicodeVersion() == QChar::Unicode_6_1);
    QVERIFY(QChar::unicodeVersion(0xa69f) == QChar::Unicode_6_1);
    QVERIFY(QChar::unicodeVersion(0x1f600) == QChar::Unicode_6_1);

    QVERIFY(QChar(0x20ba).unicodeVersion() == QChar::Unicode_6_2);
    QVERIFY(QChar::unicodeVersion(0x20ba) == QChar::Unicode_6_2);

    QVERIFY(QChar(0x061c).unicodeVersion() == QChar::Unicode_6_3);
    QVERIFY(QChar::unicodeVersion(0x061c) == QChar::Unicode_6_3);

    QVERIFY(QChar(0x20bd).unicodeVersion() == QChar::Unicode_7_0);
    QVERIFY(QChar::unicodeVersion(0x20bd) == QChar::Unicode_7_0);
    QVERIFY(QChar::unicodeVersion(0x16b00) == QChar::Unicode_7_0);

    QVERIFY(QChar(0x08b3).unicodeVersion() == QChar::Unicode_8_0);
    QVERIFY(QChar::unicodeVersion(0x08b3) == QChar::Unicode_8_0);
    QVERIFY(QChar::unicodeVersion(0x108e0) == QChar::Unicode_8_0);

    QVERIFY(QChar(0x09ff).unicodeVersion() == QChar::Unicode_Unassigned);
    QVERIFY(QChar::unicodeVersion(0x09ff) == QChar::Unicode_Unassigned);
    QVERIFY(QChar::unicodeVersion(0x110000) == QChar::Unicode_Unassigned);
}

QTEST_APPLESS_MAIN(tst_QChar)