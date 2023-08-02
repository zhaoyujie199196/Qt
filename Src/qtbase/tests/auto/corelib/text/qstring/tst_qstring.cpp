//
// Created by Yujie Zhao on 2023/2/28.

#include "tst_qstring.h"
#include <QTest>
#include <QStringMatcher>
#include <QByteArrayMatcher>
#include <private/qglobal_p.h>

#define CREATE_VIEW(string) \
    const QString padded = QLatin1Char(' ') + string + QLatin1Char(' '); \
    const QStringView view = QStringView{padded}.mid(1, padded.size() - 2);

void tst_QString::fromStdString()
{
    QVERIFY(QString::fromStdString(std::string()).isEmpty());
    std::string stroustrup = "foo";
    QString eng = QString::fromStdString( stroustrup );
    QCOMPARE( eng, QString("foo") );
    const char cnull[] = "Embedded\0null\0character!";
    std::string stdnull( cnull, sizeof(cnull)-1 );
    QString qtnull = QString::fromStdString( stdnull );
    QCOMPARE(qtnull.size(), qsizetype(stdnull.size()));
}

void tst_QString::toStdString() {
    QString nullStr;
    QVERIFY(nullStr.toStdString().empty());
    QVERIFY(!nullStr.isDetached());

    QString emptyStr("");
    QVERIFY(emptyStr.toStdString().empty());
    QVERIFY(!emptyStr.isDetached());

    QString nord = "foo";
    std::string stroustrup1 = nord.toStdString();
    QVERIFY( qstrcmp(stroustrup1.c_str(), "foo") == 0 );
    // For now, most QString constructors are also broken with respect
    // to embedded null characters, had to find one that works...
    const QChar qcnull[] = {
            'E', 'm', 'b', 'e', 'd', 'd', 'e', 'd', '\0',
            'n', 'u', 'l', 'l', '\0',
            'c', 'h', 'a', 'r', 'a', 'c', 't', 'e', 'r', '!'
    };
    QString qtnull( qcnull, sizeof(qcnull) / sizeof(QChar) );
    std::string stdnull = qtnull.toStdString();
    QCOMPARE( int(stdnull.size()), qtnull.size() );
}

void tst_QString::fromRawData()
{
    const QChar ptr[] = { QChar(0x1234), QChar(0x0000) };
    QString cstr = QString::fromRawData(ptr, 1);
    QVERIFY(!cstr.isDetached());
    QVERIFY(cstr.constData() == ptr);
    QVERIFY(cstr == QString(ptr, 1));
    cstr.squeeze();
    QVERIFY(cstr.constData() == ptr);
    cstr.detach();
    QVERIFY(cstr.size() == 1);
    QVERIFY(cstr.capacity() == 1);
    QVERIFY(cstr.constData() != ptr);
    QVERIFY(cstr.constData()[0] == QChar(0x1234));
    QVERIFY(cstr.constData()[1] == QChar(0x0000));
}

void tst_QString::setRawData()
{
    const QChar ptr[] = { QChar(0x1234), QChar(0x0000) };
    const QChar ptr2[] = { QChar(0x4321), QChar(0x0000) };
    QString cstr;

    // This just tests the fromRawData() fallback
    QVERIFY(!cstr.isDetached());
    cstr.setRawData(ptr, 1);
    QVERIFY(!cstr.isDetached());
    QVERIFY(cstr.constData() == ptr);
    QVERIFY(cstr == QString(ptr, 1));

//    QSKIP("This is currently not working.");
//    // This actually tests the recycling of the shared data object
//    QString::DataPointer csd = cstr.data_ptr();
//    cstr.setRawData(ptr2, 1);
//    QVERIFY(cstr.isDetached());
//    QVERIFY(cstr.constData() == ptr2);
//    QVERIFY(cstr == QString(ptr2, 1));
//    QVERIFY(cstr.data_ptr() == csd);

    // This tests the discarding of the shared data object
    cstr = "foo";
    QVERIFY(cstr.isDetached());
    QVERIFY(cstr.constData() != ptr2);

//    // Another test of the fallback
//    csd = cstr.data_ptr();
//    cstr.setRawData(ptr2, 1);
//    QVERIFY(cstr.isDetached());
//    QVERIFY(cstr.constData() == ptr2);
//    QVERIFY(cstr == QString(ptr2, 1));
//    QVERIFY(cstr.data_ptr() != csd);
}

void tst_QString::setUnicode() {
    const QChar ptr[] = { QChar(0x1234), QChar(0x0000) };

    QString str;
    QVERIFY(!str.isDetached());
    str.setUnicode(ptr, 1);
    // make sure that the data is copied
    QVERIFY(str.constData() != ptr);
    QVERIFY(str.isDetached());
    QCOMPARE(str, QString(ptr, 1));

    // make sure that the string is resized, even if the data is nullptr
    str = "test";
    QCOMPARE(str.size(), 4);
    str.setUnicode(nullptr, 1);  //第一个参数为nullptr，没有进行内存拷贝
    QCOMPARE(str.size(), 1);
    QCOMPARE(str, u"t");
}

void tst_QString::endsWith() {
    QString a;

    QVERIFY(!a.endsWith('A'));
    QVERIFY(!a.endsWith("AB"));
    {
        CREATE_VIEW("AB");
        QVERIFY(!a.endsWith(view));
    }
    QVERIFY(!a.isDetached());

    a = "AB";
    QVERIFY( a.endsWith("B") );
    QVERIFY( a.endsWith("AB") );
    QVERIFY( !a.endsWith("C") );
    QVERIFY( !a.endsWith("ABCDEF") );
    QVERIFY( a.endsWith("") );
    QVERIFY( a.endsWith(QString()) );
    QVERIFY( a.endsWith('B') );
    QVERIFY( a.endsWith(QLatin1Char('B')) );
    QVERIFY( a.endsWith(QChar('B')) );
    QVERIFY( !a.endsWith('C') );
    QVERIFY( !a.endsWith(QChar()) );
    QVERIFY( !a.endsWith(QLatin1Char(0)) );

    QVERIFY( a.endsWith(QLatin1String("B")) );
    QVERIFY( a.endsWith(QLatin1String("AB")) );
    QVERIFY( !a.endsWith(QLatin1String("C")) );
    QVERIFY( !a.endsWith(QLatin1String("ABCDEF")) );
    QVERIFY( a.endsWith(QLatin1String("")) );
    QVERIFY( a.endsWith(QLatin1String(0)) );

    QVERIFY( a.endsWith("B", Qt::CaseSensitive) );
    QVERIFY( a.endsWith("B", Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith("b", Qt::CaseSensitive) );
    QVERIFY( a.endsWith("b", Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith("aB", Qt::CaseSensitive) );
    QVERIFY( a.endsWith("aB", Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith("C", Qt::CaseSensitive) );
    QVERIFY( !a.endsWith("C", Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith("c", Qt::CaseSensitive) );
    QVERIFY( !a.endsWith("c", Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith("abcdef", Qt::CaseInsensitive) );
    QVERIFY( a.endsWith("", Qt::CaseInsensitive) );
    QVERIFY( a.endsWith(QString(), Qt::CaseInsensitive) );
    QVERIFY( a.endsWith('b', Qt::CaseInsensitive) );
    QVERIFY( a.endsWith('B', Qt::CaseInsensitive) );
    QVERIFY( a.endsWith(QLatin1Char('b'), Qt::CaseInsensitive) );
    QVERIFY( a.endsWith(QChar('b'), Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith('c', Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith(QChar(), Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith(QLatin1Char(0), Qt::CaseInsensitive) );

    QVERIFY( a.endsWith(QLatin1String("B"), Qt::CaseSensitive) );
    QVERIFY( a.endsWith(QLatin1String("B"), Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith(QLatin1String("b"), Qt::CaseSensitive) );
    QVERIFY( a.endsWith(QLatin1String("b"), Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith(QLatin1String("aB"), Qt::CaseSensitive) );
    QVERIFY( a.endsWith(QLatin1String("aB"), Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith(QLatin1String("C"), Qt::CaseSensitive) );
    QVERIFY( !a.endsWith(QLatin1String("C"), Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith(QLatin1String("c"), Qt::CaseSensitive) );
    QVERIFY( !a.endsWith(QLatin1String("c"), Qt::CaseInsensitive) );
    QVERIFY( !a.endsWith(QLatin1String("abcdef"), Qt::CaseInsensitive) );
    QVERIFY( a.endsWith(QLatin1String(""), Qt::CaseInsensitive) );
    QVERIFY( a.endsWith(QLatin1String(0), Qt::CaseInsensitive) );
    QVERIFY( a.endsWith('B', Qt::CaseSensitive) );
    QVERIFY( a.endsWith(QLatin1Char('B'), Qt::CaseSensitive) );
    QVERIFY( a.endsWith(QChar('B'), Qt::CaseSensitive) );
    QVERIFY( !a.endsWith('b', Qt::CaseSensitive) );
    QVERIFY( !a.endsWith(QChar(), Qt::CaseSensitive) );
    QVERIFY( !a.endsWith(QLatin1Char(0), Qt::CaseSensitive) );

#define TEST_VIEW_ENDS_WITH(string, yes) { CREATE_VIEW(string); QCOMPARE(a.endsWith(view), yes); }
    TEST_VIEW_ENDS_WITH(QLatin1String("B"), true);
    TEST_VIEW_ENDS_WITH(QLatin1String("AB"), true);
    TEST_VIEW_ENDS_WITH(QLatin1String("C"), false);
    TEST_VIEW_ENDS_WITH(QLatin1String("ABCDEF"), false);
    TEST_VIEW_ENDS_WITH(QLatin1String(""), true);
    TEST_VIEW_ENDS_WITH(QLatin1String(0), true);
#undef TEST_VIEW_ENDS_WITH

    a = "";
    QVERIFY( a.endsWith("") );
    QVERIFY( a.endsWith(QString()) );
    QVERIFY( !a.endsWith("ABC") );
    QVERIFY( !a.endsWith(QLatin1Char(0)) );
    QVERIFY( !a.endsWith(QLatin1Char('x')) );
    QVERIFY( !a.endsWith(QChar()) );

    QVERIFY( a.endsWith(QLatin1String("")) );
    QVERIFY( a.endsWith(QLatin1String(0)) );
    QVERIFY( !a.endsWith(QLatin1String("ABC")) );

    a = QString();
    QVERIFY( !a.endsWith("") );
    QVERIFY( a.endsWith(QString()) );
    QVERIFY( !a.endsWith("ABC") );

    QVERIFY( !a.endsWith(QLatin1String("")) );
    QVERIFY( a.endsWith(QLatin1String(0)) );
    QVERIFY( !a.endsWith(QLatin1String("ABC")) );

    QVERIFY( !a.endsWith(QLatin1Char(0)) );
    QVERIFY( !a.endsWith(QLatin1Char('x')) );
    QVERIFY( !a.endsWith(QChar()) );

    // this test is independent of encoding
    a = "\xc3\xa9";
    QVERIFY( a.endsWith("\xc3\xa9") );
    QVERIFY( !a.endsWith("\xc3\xa1") );

    // this one is dependent of encoding
    QVERIFY( a.endsWith("\xc3\x89", Qt::CaseInsensitive) );
}

void tst_QString::startsWith() {
    QString a;

    QVERIFY(!a.startsWith('A'));
    QVERIFY(!a.startsWith("AB"));
    {
        CREATE_VIEW("AB");
        QVERIFY(!a.startsWith(view));
    }
    QVERIFY(!a.isDetached());

    a = "AB";
    QVERIFY( a.startsWith("A") );
    QVERIFY( a.startsWith("AB") );
    QVERIFY( !a.startsWith("C") );
    QVERIFY( !a.startsWith("ABCDEF") );
    QVERIFY( a.startsWith("") );
    QVERIFY( a.startsWith(QString()) );
    QVERIFY( a.startsWith('A') );
    QVERIFY( a.startsWith(QLatin1Char('A')) );
    QVERIFY( a.startsWith(QChar('A')) );
    QVERIFY( !a.startsWith('C') );
    QVERIFY( !a.startsWith(QChar()) );
    QVERIFY( !a.startsWith(QLatin1Char(0)) );

    QVERIFY( a.startsWith(QLatin1String("A")) );
    QVERIFY( a.startsWith(QLatin1String("AB")) );
    QVERIFY( !a.startsWith(QLatin1String("C")) );
    QVERIFY( !a.startsWith(QLatin1String("ABCDEF")) );
    QVERIFY( a.startsWith(QLatin1String("")) );
    QVERIFY( a.startsWith(QLatin1String(0)) );

    QVERIFY( a.startsWith("A", Qt::CaseSensitive) );
    QVERIFY( a.startsWith("A", Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith("a", Qt::CaseSensitive) );
    QVERIFY( a.startsWith("a", Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith("aB", Qt::CaseSensitive) );
    QVERIFY( a.startsWith("aB", Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith("C", Qt::CaseSensitive) );
    QVERIFY( !a.startsWith("C", Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith("c", Qt::CaseSensitive) );
    QVERIFY( !a.startsWith("c", Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith("abcdef", Qt::CaseInsensitive) );
    QVERIFY( a.startsWith("", Qt::CaseInsensitive) );
    QVERIFY( a.startsWith(QString(), Qt::CaseInsensitive) );
    QVERIFY( a.startsWith('a', Qt::CaseInsensitive) );
    QVERIFY( a.startsWith('A', Qt::CaseInsensitive) );
    QVERIFY( a.startsWith(QLatin1Char('a'), Qt::CaseInsensitive) );
    QVERIFY( a.startsWith(QChar('a'), Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith('c', Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith(QChar(), Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith(QLatin1Char(0), Qt::CaseInsensitive) );

    QVERIFY( a.startsWith(QLatin1String("A"), Qt::CaseSensitive) );
    QVERIFY( a.startsWith(QLatin1String("A"), Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith(QLatin1String("a"), Qt::CaseSensitive) );
    QVERIFY( a.startsWith(QLatin1String("a"), Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith(QLatin1String("aB"), Qt::CaseSensitive) );
    QVERIFY( a.startsWith(QLatin1String("aB"), Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith(QLatin1String("C"), Qt::CaseSensitive) );
    QVERIFY( !a.startsWith(QLatin1String("C"), Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith(QLatin1String("c"), Qt::CaseSensitive) );
    QVERIFY( !a.startsWith(QLatin1String("c"), Qt::CaseInsensitive) );
    QVERIFY( !a.startsWith(QLatin1String("abcdef"), Qt::CaseInsensitive) );
    QVERIFY( a.startsWith(QLatin1String(""), Qt::CaseInsensitive) );
    QVERIFY( a.startsWith(QLatin1String(0), Qt::CaseInsensitive) );
    QVERIFY( a.startsWith('A', Qt::CaseSensitive) );
    QVERIFY( a.startsWith(QLatin1Char('A'), Qt::CaseSensitive) );
    QVERIFY( a.startsWith(QChar('A'), Qt::CaseSensitive) );
    QVERIFY( !a.startsWith('a', Qt::CaseSensitive) );
    QVERIFY( !a.startsWith(QChar(), Qt::CaseSensitive) );
    QVERIFY( !a.startsWith(QLatin1Char(0), Qt::CaseSensitive) );

#define TEST_VIEW_STARTS_WITH(string, yes) { CREATE_VIEW(string); QCOMPARE(a.startsWith(view), yes); }
    TEST_VIEW_STARTS_WITH("A", true);
    TEST_VIEW_STARTS_WITH("AB", true);
    TEST_VIEW_STARTS_WITH("C", false);
    TEST_VIEW_STARTS_WITH("ABCDEF", false);
#undef TEST_VIEW_STARTS_WITH

    a = "";
    QVERIFY( a.startsWith("") );
    QVERIFY( a.startsWith(QString()) );
    QVERIFY( !a.startsWith("ABC") );

    QVERIFY( a.startsWith(QLatin1String("")) );
    QVERIFY( a.startsWith(QLatin1String(0)) );
    QVERIFY( !a.startsWith(QLatin1String("ABC")) );

    QVERIFY( !a.startsWith(QLatin1Char(0)) );
    QVERIFY( !a.startsWith(QLatin1Char('x')) );
    QVERIFY( !a.startsWith(QChar()) );

    a = QString();
    QVERIFY( !a.startsWith("") );
    QVERIFY( a.startsWith(QString()) );
    QVERIFY( !a.startsWith("ABC") );

    QVERIFY( !a.startsWith(QLatin1String("")) );
    QVERIFY( a.startsWith(QLatin1String(0)) );
    QVERIFY( !a.startsWith(QLatin1String("ABC")) );

    QVERIFY( !a.startsWith(QLatin1Char(0)) );
    QVERIFY( !a.startsWith(QLatin1Char('x')) );
    QVERIFY( !a.startsWith(QChar()) );

    // this test is independent of encoding
    a = "\xc3\xa9";
    QVERIFY( a.startsWith("\xc3\xa9") );
    QVERIFY( !a.startsWith("\xc3\xa1") );

    // this one is dependent of encoding
    QVERIFY( a.startsWith("\xc3\x89", Qt::CaseInsensitive) );
}

//void tst_QString::toDouble_data() {
//    QTest::addColumn<QString>("str" );
//    QTest::addColumn<double>("result" );
//    QTest::addColumn<bool>("result_ok" );
//
//    QTest::newRow("null") << QString() << 0.0 << false;
//    QTest::newRow("empty") << QString("") << 0.0 << false;
//
//    QTest::newRow( "ok00" ) << QString("0.000000000931322574615478515625") << 0.000000000931322574615478515625 << true;
//    QTest::newRow( "ok01" ) << QString(" 123.45") << 123.45 << true;
//
//    QTest::newRow( "ok02" ) << QString("0.1e10") << 0.1e10 << true;
//    QTest::newRow( "ok03" ) << QString("0.1e-10") << 0.1e-10 << true;
//
//    QTest::newRow( "ok04" ) << QString("1e10") << 1.0e10 << true;
//    QTest::newRow( "ok05" ) << QString("1e+10") << 1.0e10 << true;
//    QTest::newRow( "ok06" ) << QString("1e-10") << 1.0e-10 << true;
//
//    QTest::newRow( "ok07" ) << QString(" 1e10") << 1.0e10 << true;
//    QTest::newRow( "ok08" ) << QString("  1e+10") << 1.0e10 << true;
//    QTest::newRow( "ok09" ) << QString("   1e-10") << 1.0e-10 << true;
//
//    QTest::newRow( "ok10" ) << QString("1.") << 1.0 << true;
//    QTest::newRow( "ok11" ) << QString(".1") << 0.1 << true;
//
//    QTest::newRow( "wrong00" ) << QString("123.45 ") << 123.45 << true;
//    QTest::newRow( "wrong01" ) << QString(" 123.45 ") << 123.45 << true;
//
//    QTest::newRow( "wrong02" ) << QString("aa123.45aa") << 0.0 << false;
//    QTest::newRow( "wrong03" ) << QString("123.45aa") << 0.0 << false;
//    QTest::newRow( "wrong04" ) << QString("123erf") << 0.0 << false;
//
//    QTest::newRow( "wrong05" ) << QString("abc") << 0.0 << false;
//    QTest::newRow( "wrong06" ) << QString() << 0.0 << false;
//    QTest::newRow( "wrong07" ) << QString("") << 0.0 << false;
//}
//
//void tst_QString::toDouble() {
//    QFETCH( QString, str );
//    QFETCH( bool, result_ok );
//    bool ok;
//    double d = str.toDouble( &ok );
//    if ( result_ok ) {
//        QVERIFY( ok );
//        QTEST( d, "result" );
//    } else {
//        QVERIFY( !ok );
//    }
//}

void tst_QString::replace_qchar_qchar_data() {
    QTest::addColumn<QString>("src" );
    QTest::addColumn<QChar>("before" );
    QTest::addColumn<QChar>("after" );
    QTest::addColumn<Qt::CaseSensitivity>("cs");
    QTest::addColumn<QString>("expected" );

    QTest::newRow("1") << QString("foo") << QChar('o') << QChar('a') << Qt::CaseSensitive
                       << QString("faa");
    QTest::newRow("2") << QString("foo") << QChar('o') << QChar('a') << Qt::CaseInsensitive
                       << QString("faa");
    QTest::newRow("3") << QString("foo") << QChar('O') << QChar('a') << Qt::CaseSensitive
                       << QString("foo");
    QTest::newRow("4") << QString("foo") << QChar('O') << QChar('a') << Qt::CaseInsensitive
                       << QString("faa");
    QTest::newRow("5") << QString("ababABAB") << QChar('a') << QChar(' ') << Qt::CaseSensitive
                       << QString(" b bABAB");
    QTest::newRow("6") << QString("ababABAB") << QChar('a') << QChar(' ') << Qt::CaseInsensitive
                       << QString(" b b B B");
    QTest::newRow("7") << QString("ababABAB") << QChar() << QChar(' ') << Qt::CaseInsensitive
                       << QString("ababABAB");
    QTest::newRow("8") << QString() << QChar() << QChar('x') << Qt::CaseInsensitive << QString();
    QTest::newRow("9") << QString() << QChar('a') << QChar('x') << Qt::CaseInsensitive << QString();
}

void tst_QString::replace_qchar_qchar() {
    QFETCH(QString, src);
    QFETCH(QChar, before);
    QFETCH(QChar, after);
    QFETCH(Qt::CaseSensitivity, cs);
    QFETCH(QString, expected);

    QCOMPARE(src.replace(before, after, cs), expected);
}

void tst_QString::replace_qchar_qstring_data() {
    QTest::addColumn<QString>("src" );
    QTest::addColumn<QChar>("before" );
    QTest::addColumn<QString>("after" );
    QTest::addColumn<Qt::CaseSensitivity>("cs");
    QTest::addColumn<QString>("expected" );

    QTest::newRow("1") << QString("foo") << QChar('o') << QString("aA") << Qt::CaseSensitive
                       << QString("faAaA");
    QTest::newRow("2") << QString("foo") << QChar('o') << QString("aA") << Qt::CaseInsensitive
                       << QString("faAaA");
    QTest::newRow("3") << QString("foo") << QChar('O') << QString("aA") << Qt::CaseSensitive
                       << QString("foo");
    QTest::newRow("4") << QString("foo") << QChar('O') << QString("aA") << Qt::CaseInsensitive
                       << QString("faAaA");
    QTest::newRow("5") << QString("ababABAB") << QChar('a') << QString("  ") << Qt::CaseSensitive
                       << QString("  b  bABAB");
    QTest::newRow("6") << QString("ababABAB") << QChar('a') << QString("  ") << Qt::CaseInsensitive
                       << QString("  b  b  B  B");
    QTest::newRow("7") << QString("ababABAB") << QChar() << QString("  ") << Qt::CaseInsensitive
                       << QString("ababABAB");
    QTest::newRow("8") << QString("ababABAB") << QChar() << QString() << Qt::CaseInsensitive
                       << QString("ababABAB");
    QTest::newRow("null-in-null-with-X") << QString() << QChar() << QString("X")
                                         << Qt::CaseSensitive << QString();
    QTest::newRow("x-in-null-with-abc") << QString() << QChar('x') << QString("abc")
                                        << Qt::CaseSensitive << QString();
    QTest::newRow("null-in-empty-with-X") << QString("") << QChar() << QString("X")
                                          << Qt::CaseInsensitive << QString();
    QTest::newRow("x-in-empty-with-abc") << QString("") << QChar('x') << QString("abc")
                                         << Qt::CaseInsensitive << QString();
}

void tst_QString::replace_qchar_qstring() {
    QFETCH(QString, src);
    QFETCH(QChar, before);
    QFETCH(QString, after);
    QFETCH(Qt::CaseSensitivity, cs);
    QFETCH(QString, expected);

    QCOMPARE(src.replace(before, after, cs), expected);
}

void tst_QString::replace_uint_uint_data() {
    QTest::addColumn<QString>("string" );
    QTest::addColumn<int>("index" );
    QTest::addColumn<int>("len" );
    QTest::addColumn<QString>("after" );
    QTest::addColumn<QString>("result" );

    QTest::newRow("empty_rem00") << QString() << 0 << 0 << QString("") << QString();
    QTest::newRow("empty_rem01") << QString() << 0 << 3 << QString("") << QString();
    QTest::newRow("empty_rem02") << QString() << 5 << 3 << QString("") << QString();

    QTest::newRow( "rem00" ) << QString("-<>ABCABCABCABC>") << 0 << 3 << QString("") << QString("ABCABCABCABC>");
    QTest::newRow( "rem01" ) << QString("ABCABCABCABC>") << 1 << 4 << QString("") << QString("ACABCABC>");
    QTest::newRow( "rem04" ) << QString("ACABCABC>") << 8 << 4 << QString("") << QString("ACABCABC");
    QTest::newRow( "rem05" ) << QString("ACABCABC") << 7 << 1 << QString("") << QString("ACABCAB");
    QTest::newRow( "rem06" ) << QString("ACABCAB") << 4 << 0 << QString("") << QString("ACABCAB");

    QTest::newRow("empty_rep00") << QString() << 0 << 0 << QString("X") << QString("X");
    QTest::newRow("empty_rep01") << QString() << 0 << 3 << QString("X") << QString("X");
    QTest::newRow("empty_rep02") << QString() << 5 << 3 << QString("X") << QString();

    QTest::newRow( "rep00" ) << QString("ACABCAB") << 4 << 0 << QString("X") << QString("ACABXCAB");
    QTest::newRow( "rep01" ) << QString("ACABXCAB") << 4 << 1 << QString("Y") << QString("ACABYCAB");
    QTest::newRow( "rep02" ) << QString("ACABYCAB") << 4 << 1 << QString("") << QString("ACABCAB");
    QTest::newRow( "rep03" ) << QString("ACABCAB") << 0 << 9999 << QString("XX") << QString("XX");
    QTest::newRow( "rep04" ) << QString("XX") << 0 << 9999 << QString("") << QString("");
    QTest::newRow( "rep05" ) << QString("ACABCAB") << 0 << 2 << QString("XX") << QString("XXABCAB");
    QTest::newRow( "rep06" ) << QString("ACABCAB") << 1 << 2 << QString("XX") << QString("AXXBCAB");
    QTest::newRow( "rep07" ) << QString("ACABCAB") << 2 << 2 << QString("XX") << QString("ACXXCAB");
    QTest::newRow( "rep08" ) << QString("ACABCAB") << 3 << 2 << QString("XX") << QString("ACAXXAB");
    QTest::newRow( "rep09" ) << QString("ACABCAB") << 4 << 2 << QString("XX") << QString("ACABXXB");
    QTest::newRow( "rep10" ) << QString("ACABCAB") << 5 << 2 << QString("XX") << QString("ACABCXX");
    QTest::newRow( "rep11" ) << QString("ACABCAB") << 6 << 2 << QString("XX") << QString("ACABCAXX");
    QTest::newRow( "rep12" ) << QString() << 0 << 10 << QString("X") << QString("X");
    QTest::newRow( "rep13" ) << QString("short") << 0 << 10 << QString("X") << QString("X");
    QTest::newRow( "rep14" ) << QString() << 0 << 10 << QString("XX") << QString("XX");
    QTest::newRow( "rep15" ) << QString("short") << 0 << 10 << QString("XX") << QString("XX");

    // This is a regression test for an old bug where QString would add index and len parameters,
    // potentially causing integer overflow.
    QTest::newRow( "no overflow" ) << QString("ACABCAB") << 1 << INT_MAX - 1 << QString("") << QString("A");
    QTest::newRow( "overflow" ) << QString("ACABCAB") << 1 << INT_MAX << QString("") << QString("A");
}

void tst_QString::replace_uint_uint() {
    QFETCH( QString, string );
    QFETCH( int, index );
    QFETCH( int, len );
    QFETCH( QString, after );

    QString s1 = string;
    s1.replace( (uint) index, (int) len, after );
    QTEST( s1, "result" );

    QString s2 = string;
    s2.replace( (uint) index, (uint) len, after.unicode(), after.length() );
    QTEST( s2, "result" );

    if ( after.length() == 1 ) {
        QString s3 = string;
        s3.replace( (uint) index, (uint) len, QChar(after[0]) );
        QTEST( s3, "result" );

        QString s4 = string;
        s4.replace( (uint) index, (uint) len, QChar(after[0]).toLatin1() );
        QTEST( s4, "result" );
    }
}

void tst_QString::replace_uint_uint_extra() {
    {
        QString s;
        s.insert(0, QChar('A'));

        auto bigReplacement = QString("B").repeated(s.capacity() * 3);

        s.replace( 0, 1, bigReplacement );
        QCOMPARE( s, bigReplacement );
    }

    {
        QString s;
        s.insert(0, QLatin1String("BBB"));

        auto smallReplacement = QString("C");

        s.replace( 0, 3, smallReplacement );
        QCOMPARE( s, smallReplacement );
    }

    {
        QString s;
        s.insert(0, QLatin1String("BBB"));

        auto smallReplacement = QString("C");

        s.replace( 5, 3, smallReplacement );
        QCOMPARE( s, QLatin1String("BBB") );
    }
}

void tst_QString::replace_extra() {
    /*
        This test is designed to be extremely slow if QString::replace() doesn't optimize the case
        len == after.size().
    */
    QString str("dsfkljfdsjklsdjsfjklfsdjkldfjslkjsdfkllkjdsfjklsfdkjsdflkjlsdfjklsdfkjldsflkjsddlkj");
    for (int j = 1; j < 12; ++j)
        str += str;

    QString str2("aaaaaaaaaaaaaaaaaaaa");
    for (int i = 0; i < 2000000; ++i) {
        str.replace(10, 20, str2);
    }

    /*
        Make sure that replacing with itself works.
    */
    QString copy(str);
    copy.detach();
    str.replace(0, str.length(), str);
    QVERIFY(copy == str);

    /*
        Make sure that replacing a part of oneself with itself works.
    */
    QString str3("abcdefghij");
    str3.replace(0, 1, str3);
    QCOMPARE(str3, QString("abcdefghijbcdefghij"));

    QString str4("abcdefghij");
    str4.replace(1, 3, str4);
    QCOMPARE(str4, QString("aabcdefghijefghij"));

    QString str5("abcdefghij");
    str5.replace(8, 10, str5);
    QCOMPARE(str5, QString("abcdefghabcdefghij"));

    // Replacements using only part of the string modified:
    QString str6("abcdefghij");
    str6.replace(1, 8, str6.constData() + 3, 3);
    QCOMPARE(str6, QString("adefj"));

    QString str7("abcdefghibcdefghij");
    str7.replace(str7.constData() + 1, 6, str7.constData() + 2, 3);
    QCOMPARE(str7, QString("acdehicdehij"));

    const int many = 1024;
    /*
      QS::replace(const QChar *, int, const QChar *, int, Qt::CaseSensitivity)
      does its replacements in batches of many (please keep in sync with any
      changes to batch size), which lead to misbehaviour if ether QChar * array
      was part of the data being modified.
    */
    QString str8("abcdefg"), ans8("acdeg");
    {
        // Make str8 and ans8 repeat themselves many + 1 times:
        int i = many;
        QString big(str8), small(ans8);
        while (i && !(i & 1)) { // Exploit many being a power of 2:
            big += big;
            small += small;
            i >>= 1;
        }
        while (i-- > 0) {
            str8 += big;
            ans8 += small;
        }
    }
    str8.replace(str8.constData() + 1, 5, str8.constData() + 2, 3);
    // Pre-test the bit where the diff happens, so it gets displayed:
    QCOMPARE(str8.mid((many - 3) * 5), ans8.mid((many - 3) * 5));
    // Also check the full values match, of course:
    QCOMPARE(str8.size(), ans8.size());
    QCOMPARE(str8, ans8);
}

void tst_QString::replace_string_data() {
    QTest::addColumn<QString>("string" );
    QTest::addColumn<QString>("before" );
    QTest::addColumn<QString>("after" );
    QTest::addColumn<QString>("result" );
    QTest::addColumn<bool>("bcs" );

    QTest::newRow( "rem00" ) << QString("") << QString("") << QString("") << QString("") << true;
    QTest::newRow( "rem01" ) << QString("A") << QString("") << QString("") << QString("A") << true;
    QTest::newRow( "rem02" ) << QString("A") << QString("A") << QString("") << QString("") << true;
    QTest::newRow( "rem03" ) << QString("A") << QString("B") << QString("") << QString("A") << true;
    QTest::newRow( "rem04" ) << QString("AA") << QString("A") << QString("") << QString("") << true;
    QTest::newRow( "rem05" ) << QString("AB") << QString("A") << QString("") << QString("B") << true;
    QTest::newRow( "rem06" ) << QString("AB") << QString("B") << QString("") << QString("A") << true;
    QTest::newRow( "rem07" ) << QString("AB") << QString("C") << QString("") << QString("AB") << true;
    QTest::newRow( "rem08" ) << QString("ABA") << QString("A") << QString("") << QString("B") << true;
    QTest::newRow( "rem09" ) << QString("ABA") << QString("B") << QString("") << QString("AA") << true;
    QTest::newRow( "rem10" ) << QString("ABA") << QString("C") << QString("") << QString("ABA") << true;
    QTest::newRow( "rem11" ) << QString("banana") << QString("an") << QString("") << QString("ba") << true;
    QTest::newRow( "rem12" ) << QString("") << QString("A") << QString("") << QString("") << true;
    QTest::newRow( "rem13" ) << QString("") << QString("A") << QString() << QString("") << true;
    QTest::newRow( "rem14" ) << QString() << QString("A") << QString("") << QString() << true;
    QTest::newRow( "rem15" ) << QString() << QString("A") << QString() << QString() << true;
    QTest::newRow( "rem16" ) << QString() << QString("") << QString("") << QString("") << true;
    QTest::newRow( "rem17" ) << QString("") << QString() << QString("") << QString("") << true;
    QTest::newRow( "rem18" ) << QString("a") << QString("a") << QString("") << QString("") << false;
    QTest::newRow( "rem19" ) << QString("A") << QString("A") << QString("") << QString("") << false;
    QTest::newRow( "rem20" ) << QString("a") << QString("A") << QString("") << QString("") << false;
    QTest::newRow( "rem21" ) << QString("A") << QString("a") << QString("") << QString("") << false;
    QTest::newRow( "rem22" ) << QString("Alpha beta") << QString("a") << QString("") << QString("lph bet") << false;

    QTest::newRow( "rep00" ) << QString("ABC") << QString("B") << QString("-") << QString("A-C") << true;
    QTest::newRow( "rep01" ) << QString("$()*+.?[\\]^{|}") << QString("$()*+.?[\\]^{|}") << QString("X") << QString("X") << true;
    QTest::newRow( "rep02" ) << QString("ABCDEF") << QString("") << QString("X") << QString("XAXBXCXDXEXFX") << true;
    QTest::newRow( "rep03" ) << QString("") << QString("") << QString("X") << QString("X") << true;
    QTest::newRow( "rep04" ) << QString("a") << QString("a") << QString("b") << QString("b") << false;
    QTest::newRow( "rep05" ) << QString("A") << QString("A") << QString("b") << QString("b") << false;
    QTest::newRow( "rep06" ) << QString("a") << QString("A") << QString("b") << QString("b") << false;
    QTest::newRow( "rep07" ) << QString("A") << QString("a") << QString("b") << QString("b") << false;
    QTest::newRow( "rep08" ) << QString("a") << QString("a") << QString("a") << QString("a") << false;
    QTest::newRow( "rep09" ) << QString("A") << QString("A") << QString("a") << QString("a") << false;
    QTest::newRow( "rep10" ) << QString("a") << QString("A") << QString("a") << QString("a") << false;
    QTest::newRow( "rep11" ) << QString("A") << QString("a") << QString("a") << QString("a") << false;
    QTest::newRow( "rep12" ) << QString("Alpha beta") << QString("a") << QString("o") << QString("olpho beto") << false;
    QTest::newRow( "rep13" ) << QString() << QString("") << QString("A") << QString("A") << true;
    QTest::newRow( "rep14" ) << QString("") << QString() << QString("A") << QString("A") << true;
    QTest::newRow( "rep15" ) << QString("fooxbarxbazxblub") << QString("x") << QString("yz") << QString("fooyzbaryzbazyzblub") << true;
    QTest::newRow( "rep16" ) << QString("fooxbarxbazxblub") << QString("x") << QString("z") << QString("foozbarzbazzblub") << true;
    QTest::newRow( "rep17" ) << QString("fooxybarxybazxyblub") << QString("xy") << QString("z") << QString("foozbarzbazzblub") << true;
    QTest::newRow("rep18") << QString() << QString() << QString("X") << QString("X") << false;
    QTest::newRow("rep19") << QString() << QString("A") << QString("X") << QString("") << false;
}

void tst_QString::replace_string() {
    QFETCH( QString, string );
    QFETCH( QString, before );
    QFETCH( QString, after );
    QFETCH( bool, bcs );

    Qt::CaseSensitivity cs = bcs ? Qt::CaseSensitive : Qt::CaseInsensitive;

    if ( before.length() == 1 ) {
        QChar ch = before.at( 0 );

        QString s1 = string;
        s1.replace( ch, after, cs );
        QTEST( s1, "result" );

        if ( QChar(ch.toLatin1()) == ch ) {
            QString s2 = string;
            s2.replace( ch.toLatin1(), after, cs );
            QTEST( s2, "result" );
        }
    }

    QString s3 = string;
    s3.replace( before, after, cs );
    QTEST( s3, "result" );
}

void tst_QString::replace_string_extra() {
    {
        QString s;
        s.insert(0, QChar('A'));

        auto bigReplacement = QString("B").repeated(s.capacity() * 3);

        s.replace( QString("A"), bigReplacement );
        QCOMPARE( s, bigReplacement );
    }

    {
        QString s;
        s.insert(0, QLatin1String("BBB"));

        auto smallReplacement = QString("C");

        s.replace( QString("BBB"), smallReplacement );
        QCOMPARE( s, smallReplacement );
    }

    {
        QString s(QLatin1String("BBB"));
        QString expected(QLatin1String("BBB"));
        for (int i = 0; i < 1028; ++i) {
            s.append("X");
            expected.append("GXU");
        }
        s.replace(QChar('X'), "GXU");
        QCOMPARE(s, expected);
    }
}

void tst_QString::remove_uint_uint_data() {
    replace_uint_uint_data();
}

void tst_QString::remove_uint_uint() {
    QFETCH( QString, string );
    QFETCH( int, index );
    QFETCH( int, len );
    QFETCH( QString, after );

    if ( after.length() == 0 ) {
        QString s1 = string;
        s1.remove( (uint) index, (uint) len );
        QTEST( s1, "result" );
    } else
        QCOMPARE( 0, 0 ); // shut Qt Test
}

void tst_QString::remove_string_data() {
    replace_string_data();
}

void tst_QString::remove_string() {
    QFETCH( QString, string );
    QFETCH( QString, before );
    QFETCH( QString, after );
    QFETCH( bool, bcs );

    Qt::CaseSensitivity cs = bcs ? Qt::CaseSensitive : Qt::CaseInsensitive;

    if ( after.length() == 0 ) {
        if ( before.length() == 1 && cs ) {
            QChar ch = before.at( 0 );

            QString s1 = string;
            s1.remove( ch );
            QTEST( s1, "result" );

            if ( QChar(ch.toLatin1()) == ch ) {
                QString s2 = string;
                s2.remove( ch );
                QTEST( s2, "result" );
            }
        }

        QString s3 = string;
        s3.remove( before, cs );
        QTEST( s3, "result" );

        if (QtPrivate::isLatin1(before)) {
            QString s6 = string;
            s6.remove( QLatin1String(before.toLatin1()), cs );
            QTEST( s6, "result" );
        }
    } else {
        QCOMPARE( 0, 0 ); // shut Qt Test
    }
}

void tst_QString::remove_extra() {
    {
        QString s = "The quick brown fox jumps over the lazy dog. "
                    "The lazy dog jumps over the quick brown fox.";
        s.remove(s);
    }

    {
        QString s = "BCDEFGHJK";
        QString s1 = s;
        s1.insert(0, u'A');  // detaches
        s1.remove(0, 1);
        QCOMPARE(s1, s);
    }
}

void tst_QString::swap() {
    QString s1, s2;
    s1 = "s1";
    s2 = "s2";
    s1.swap(s2);
    QCOMPARE(s1,QLatin1String("s2"));
    QCOMPARE(s2,QLatin1String("s1"));
}

void tst_QString::prepend_data(bool emptyIsNoop)
{
    QTest::addColumn<QString>("s");
    QTest::addColumn<CharStarContainer>("arg");
    QTest::addColumn<QString>("expected");

    const CharStarContainer nullC;
    const CharStarContainer emptyC("");
    const CharStarContainer aC("a");
    const CharStarContainer bC("b");
    const CharStarContainer baC("ba");

    const QString null;
    const QString empty("");
    const QString a("a");
    //const QString b("b");
    const QString ba("ba");

    QTest::newRow("null.prepend(null)") << null << nullC << null;
    QTest::newRow("null.prepend(empty)") << null << emptyC << (emptyIsNoop ? null : empty);
    QTest::newRow("null.prepend(a)") << null << aC << a;
    QTest::newRow("empty.prepend(null)") << empty << nullC << empty;
    QTest::newRow("empty.prepend(empty)") << empty << emptyC << empty;
    QTest::newRow("empty.prepend(a)") << empty << aC << a;
    QTest::newRow("a.prepend(null)") << a << nullC << a;
    QTest::newRow("a.prepend(empty)") << a << emptyC << a;
    QTest::newRow("a.prepend(b)") << a << bC << ba;
    QTest::newRow("a.prepend(ba)") << a << baC << (ba + a);
}

void tst_QString::prepend_bytearray_special_cases_data() {
    QTest::addColumn<QString>("str" );
    QTest::addColumn<QByteArray>("ba" );
    QTest::addColumn<QString>("res" );

    QByteArray ba( 5, 0 );
    ba[0] = 'a';
    ba[1] = 'b';
    ba[2] = 'c';
    ba[3] = 'd';

    // byte array with only a 0
    ba.resize( 1 );
    ba[0] = 0;
    QTest::newRow( "emptyString" ) << QString("foobar ") << ba << QStringView::fromArray(u"\0foobar ").chopped(1).toString();

    // empty byte array
    ba.resize( 0 );
    QTest::newRow( "emptyByteArray" ) << QString(" foobar") << ba << QString(" foobar");

    // non-ascii byte array
    QTest::newRow( "nonAsciiByteArray") << QString() << QByteArray("\xc3\xa9") << QString("\xc3\xa9");
    QTest::newRow( "nonAsciiByteArray2") << QString() << QByteArray("\xc3\xa9") << QString::fromUtf8("\xc3\xa9");
}

void tst_QString::prepend_bytearray_special_cases() {
    {
        QFETCH( QString, str );
        QFETCH( QByteArray, ba );

        str.prepend( ba );

        QFETCH( QString, res );
        QCOMPARE( str, res );
    }
    {
        QFETCH( QString, str );
        QFETCH( QByteArray, ba );

        str.prepend( ba );

        QTEST( str, "res" );
    }

    QFETCH( QByteArray, ba );
    if (!ba.contains('\0') && ba.constData()[ba.length()] == '\0') {
        QFETCH( QString, str );

        str.prepend(ba.constData());
        QTEST( str, "res" );
    }
}

void tst_QString::append_data(bool emptyIsNoop)
{
    QTest::addColumn<QString>("s");
    QTest::addColumn<CharStarContainer>("arg");
    QTest::addColumn<QString>("expected");

    const CharStarContainer nullC;
    const CharStarContainer emptyC("");
    const CharStarContainer aC("a");
    const CharStarContainer bC("b");
    //const CharStarContainer abC("ab");

    const QString null;
    const QString empty("");
    const QString a("a");
    //const QString b("b");
    const QString ab("ab");

    QTest::newRow("null + null") << null << nullC << null;
    QTest::newRow("null + empty") << null << emptyC << (emptyIsNoop ? null : empty);
    QTest::newRow("null + a") << null << aC << a;
    QTest::newRow("empty + null") << empty << nullC << empty;
    QTest::newRow("empty + empty") << empty << emptyC << empty;
    QTest::newRow("empty + a") << empty << aC << a;
    QTest::newRow("a + null") << a << nullC << a;
    QTest::newRow("a + empty") << a << emptyC << a;
    QTest::newRow("a + b") << a << bC << ab;
}

void tst_QString::append_special_cases() {
    {
        QString a;
        static const QChar unicode[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        a.append(unicode, sizeof unicode / sizeof *unicode);
        QCOMPARE(a, QLatin1String("Hello, World!"));
        static const QChar nl('\n');
        a.append(&nl, 1);
        QCOMPARE(a, QLatin1String("Hello, World!\n"));
        a.append(unicode, sizeof unicode / sizeof *unicode);
        QCOMPARE(a, QLatin1String("Hello, World!\nHello, World!"));
        a.append(unicode, 0); // no-op
        QCOMPARE(a, QLatin1String("Hello, World!\nHello, World!"));
        a.append(unicode, -1); // no-op
        QCOMPARE(a, QLatin1String("Hello, World!\nHello, World!"));
        a.append(0, 1); // no-op
        QCOMPARE(a, QLatin1String("Hello, World!\nHello, World!"));
    }

    {
        QString a;
        a.insert(0, QChar(u'A'));
        QCOMPARE(a.size(), 1);
        QVERIFY(a.capacity() > 0);
        a.append(QLatin1String("BC"));
        QCOMPARE(a, QLatin1String("ABC"));
    }

    {
        QString a = "one";
        a.prepend(u'a');
        QString b(a.data_ptr()->freeSpaceAtEnd(), u'b');
        QCOMPARE(a.append(QLatin1String(b.toLatin1())), QString("aone") + b);
    }

    {
        QString a = "onetwothree";
        while (a.size() - 1)
            a.remove(0, 1);
        QString b(a.data_ptr()->freeSpaceAtEnd(), u'b');
        QCOMPARE(a.append(QLatin1String(b.toLatin1())), QString("e") + b);
    }

    {
        QString a = "one";
        a.prepend(u'a');
        QString b(a.data_ptr()->freeSpaceAtEnd(), u'b');
        QCOMPARE(a.append(b), QString("aone") + b);
    }

    {
        QString a = "onetwothree";
        while (a.size() - 1)
            a.remove(0, 1);
        QString b(a.data_ptr()->freeSpaceAtEnd() + 1, u'b');
        QCOMPARE(a.append(b), QString("e") + b);
    }

    {
        QString a = "one";
        a.prepend(u'a');
        QCOMPARE(a.append(u'b'), QString("aoneb"));
    }

    {
        QString a = "onetwothree";
        while (a.size() - 1)
            a.remove(0, 1);
        QCOMPARE(a.append(u'b'), QString("eb"));
    }
}

void tst_QString::append_bytearray_special_cases_data()
{
    QTest::addColumn<QString>("str" );
    QTest::addColumn<QByteArray>("ba" );
    QTest::addColumn<QString>("res" );

    QByteArray ba( 5, 0 );
    ba[0] = 'a';
    ba[1] = 'b';
    ba[2] = 'c';
    ba[3] = 'd';

    // no 0 termination
    ba.resize( 4 );
    QTest::newRow( "notTerminated_0" ) << QString() << ba << QString("abcd");
    QTest::newRow( "notTerminated_1" ) << QString("") << ba << QString("abcd");
    QTest::newRow( "notTerminated_2" ) << QString("foobar ") << ba << QString("foobar abcd");

    // byte array with only a 0
    ba.resize( 1 );
    ba[0] = 0;
    QByteArray ba2("foobar ");
    ba2.append('\0');
    QTest::newRow( "emptyString" ) << QString("foobar ") << ba << QString(ba2);

    // empty byte array
    ba.resize( 0 );
    QTest::newRow( "emptyByteArray" ) << QString("foobar ") << ba << QString("foobar ");

    // non-ascii byte array
    QTest::newRow( "nonAsciiByteArray") << QString() << QByteArray("\xc3\xa9") << QString("\xc3\xa9");
    QTest::newRow( "nonAsciiByteArray2") << QString() << QByteArray("\xc3\xa9") << QString::fromUtf8("\xc3\xa9");
}

void tst_QString::append_bytearray_special_cases()
{
    {
        QFETCH( QString, str );
        QFETCH( QByteArray, ba );

        str.append( ba );

        QTEST( str, "res" );
    }
    {
        QFETCH( QString, str );
        QFETCH( QByteArray, ba );

        str.append( ba );

        QTEST( str, "res" );
    }

    QFETCH( QByteArray, ba );
    if (!ba.contains('\0') && ba.constData()[ba.length()] == '\0') {
        QFETCH( QString, str );

        str.append(ba.constData());
        QTEST( str, "res" );
    }
}

void tst_QString::operator_pluseq_data(bool emptyIsNoop)
{
    append_data(emptyIsNoop);
}

void tst_QString::operator_pluseq_special_cases()
{
    {
        QString a;
        a += QChar::CarriageReturn;
        a += '\r';
        a += u'\x1111';
        QCOMPARE(a, QStringView(u"\r\r\x1111"));
    }
}

void tst_QString::operator_pluseq_bytearray_special_cases_data()
{
    append_bytearray_special_cases_data();
}

void tst_QString::operator_pluseq_bytearray_special_cases()
{
    {
        QFETCH( QString, str );
        QFETCH( QByteArray, ba );

        str += ba;

        QTEST( str, "res" );
    }
    {
        QFETCH( QString, str );
        QFETCH( QByteArray, ba );

        str += ba;

        QTEST( str, "res" );
    }

    QFETCH( QByteArray, ba );
    if (!ba.contains('\0') && ba.constData()[ba.length()] == '\0') {
        QFETCH( QString, str );

        str += ba.constData();
        QTEST( str, "res" );
    }
}

void tst_QString::operator_eqeq_bytearray_data() {
    constructorQByteArray_data();
}

void tst_QString::operator_eqeq_bytearray() {
    auto tmp = QString::fromUtf16(u"abc\0def", 7);

    QFETCH(QByteArray, src);
    QFETCH(QString, expected);

    QVERIFY(expected == src);
    QVERIFY(!(expected != src));

    if (!src.contains('\0') && src.constData()[src.length()] == '\0') {
        QVERIFY(expected == src.constData());
        QVERIFY(!(expected != src.constData()));
    }
}

void tst_QString::operator_eqeq_nullstring()
{
    /* Some of these might not be all that logical but it's the behaviour we've had since 3.0.0
       so we should probably stick with it. */

    QVERIFY( QString() == "" );
    QVERIFY( "" == QString() );

    QVERIFY( QString("") == "" );
    QVERIFY( "" == QString("") );

    QVERIFY(QString() == nullptr);
    QVERIFY(nullptr == QString());

    QVERIFY(QString("") == nullptr);
    QVERIFY(nullptr == QString(""));

    QVERIFY( QString().size() == 0 );

    QVERIFY( QString("").size() == 0 );

    QVERIFY( QString() == QString("") );
    QVERIFY( QString("") == QString() );
}

void tst_QString::operator_smaller()
{
    QString null;
    QString empty("");
    QString foo("foo");
    const char *nullC = nullptr;
    const char *emptyC = "";

    QVERIFY( !(null < QString()) );
    QVERIFY( !(null > QString()) );

    QVERIFY( !(empty < QString("")) );
    QVERIFY( !(empty > QString("")) );

    QVERIFY( !(null < empty) );
    QVERIFY( !(null > empty) );

    QVERIFY( !(nullC < empty) );
    QVERIFY( !(nullC > empty) );

    QVERIFY( !(null < emptyC) );
    QVERIFY( !(null > emptyC) );

    QVERIFY( null < foo );
    QVERIFY( !(null > foo) );
    QVERIFY( foo > null );
    QVERIFY( !(foo < null) );

    QVERIFY( empty < foo );
    QVERIFY( !(empty > foo) );
    QVERIFY( foo > empty );
    QVERIFY( !(foo < empty) );

    QVERIFY( !(null < QLatin1String(0)) );
    QVERIFY( !(null > QLatin1String(0)) );
    QVERIFY( !(null < QLatin1String("")) );
    QVERIFY( !(null > QLatin1String("")) );

    QVERIFY( !(null < QLatin1String("")) );
    QVERIFY( !(null > QLatin1String("")) );
    QVERIFY( !(empty < QLatin1String("")) );
    QVERIFY( !(empty > QLatin1String("")) );

    QVERIFY( !(QLatin1String(0) < null) );
    QVERIFY( !(QLatin1String(0) > null) );
    QVERIFY( !(QLatin1String("") < null) );
    QVERIFY( !(QLatin1String("") > null) );

    QVERIFY( !(QLatin1String(0) < empty) );
    QVERIFY( !(QLatin1String(0) > empty) );
    QVERIFY( !(QLatin1String("") < empty) );
    QVERIFY( !(QLatin1String("") > empty) );

    QVERIFY( QLatin1String(0) < foo );
    QVERIFY( !(QLatin1String(0) > foo) );
    QVERIFY( QLatin1String("") < foo );
    QVERIFY( !(QLatin1String("") > foo) );

    QVERIFY( foo > QLatin1String(0) );
    QVERIFY( !(foo < QLatin1String(0)) );
    QVERIFY( foo > QLatin1String("") );
    QVERIFY( !(foo < QLatin1String("")) );

    QVERIFY( QLatin1String(0) == empty);
    QVERIFY( QLatin1String(0) == null);
    QVERIFY( QLatin1String("") == empty);
    QVERIFY( QLatin1String("") == null);

    QVERIFY( !(foo < QLatin1String("foo")));
    QVERIFY( !(foo > QLatin1String("foo")));
    QVERIFY( !(QLatin1String("foo") < foo));
    QVERIFY( !(QLatin1String("foo") > foo));

    QVERIFY( !(foo < QLatin1String("a")));
    QVERIFY( (foo > QLatin1String("a")));
    QVERIFY( (QLatin1String("a") < foo));
    QVERIFY( !(QLatin1String("a") > foo));

    QVERIFY( (foo < QLatin1String("z")));
    QVERIFY( !(foo > QLatin1String("z")));
    QVERIFY( !(QLatin1String("z") < foo));
    QVERIFY( (QLatin1String("z") > foo));

    // operator< is not locale-aware (or shouldn't be)
    QVERIFY( foo < QString("\xc3\xa9") );
    QVERIFY( foo < "\xc3\xa9" );

    QVERIFY(QString("a") < QString("b"));
    QVERIFY(QString("a") <= QString("b"));
    QVERIFY(QString("a") <= QString("a"));
    QVERIFY(QString("a") == QString("a"));
    QVERIFY(QString("a") >= QString("a"));
    QVERIFY(QString("b") >= QString("a"));
    QVERIFY(QString("b") > QString("a"));

    QVERIFY("a" < QString("b"));
    QVERIFY("a" <= QString("b"));
    QVERIFY("a" <= QString("a"));
    QVERIFY("a" == QString("a"));
    QVERIFY("a" >= QString("a"));
    QVERIFY("b" >= QString("a"));
    QVERIFY("b" > QString("a"));

    QVERIFY(QString("a") < "b");
    QVERIFY(QString("a") <= "b");
    QVERIFY(QString("a") <= "a");
    QVERIFY(QString("a") == "a");
    QVERIFY(QString("a") >= "a");
    QVERIFY(QString("b") >= "a");
    QVERIFY(QString("b") > "a");

    QVERIFY(QString("a") < QByteArray("b"));
    QVERIFY(QString("a") <= QByteArray("b"));
    QVERIFY(QString("a") <= QByteArray("a"));
    QVERIFY(QString("a") == QByteArray("a"));
    QVERIFY(QString("a") >= QByteArray("a"));
    QVERIFY(QString("b") >= QByteArray("a"));
    QVERIFY(QString("b") > QByteArray("a"));

    QVERIFY(QLatin1String("a") < QString("b"));
    QVERIFY(QLatin1String("a") <= QString("b"));
    QVERIFY(QLatin1String("a") <= QString("a"));
    QVERIFY(QLatin1String("a") == QString("a"));
    QVERIFY(QLatin1String("a") >= QString("a"));
    QVERIFY(QLatin1String("b") >= QString("a"));
    QVERIFY(QLatin1String("b") > QString("a"));

    QVERIFY(QString("a") < QLatin1String("b"));
    QVERIFY(QString("a") <= QLatin1String("b"));
    QVERIFY(QString("a") <= QLatin1String("a"));
    QVERIFY(QString("a") == QLatin1String("a"));
    QVERIFY(QString("a") >= QLatin1String("a"));
    QVERIFY(QString("b") >= QLatin1String("a"));
    QVERIFY(QString("b") > QLatin1String("a"));

    QVERIFY("a" < QLatin1String("b"));
    QVERIFY("a" <= QLatin1String("b"));
    QVERIFY("a" <= QLatin1String("a"));
    QVERIFY("a" == QLatin1String("a"));
    QVERIFY("a" >= QLatin1String("a"));
    QVERIFY("b" >= QLatin1String("a"));
    QVERIFY("b" > QLatin1String("a"));

    QVERIFY(QLatin1String("a") < "b");
    QVERIFY(QLatin1String("a") <= "b");
    QVERIFY(QLatin1String("a") <= "a");
    QVERIFY(QLatin1String("a") == "a");
    QVERIFY(QLatin1String("a") >= "a");
    QVERIFY(QLatin1String("b") >= "a");
    QVERIFY(QLatin1String("b") > "a");
}

void tst_QString::constructorQByteArray_data() {
    QTest::addColumn<QByteArray>("src" );
    QTest::addColumn<QString>("expected" );

    QByteArray ba( 4, 0 );
    ba[0] = 'C';
    ba[1] = 'O';
    ba[2] = 'M';
    ba[3] = 'P';

    QTest::newRow( "1" ) << ba << QString("COMP");

    QByteArray ba1( 7, 0 );
    ba1[0] = 'a';
    ba1[1] = 'b';
    ba1[2] = 'c';
    ba1[3] = '\0';
    ba1[4] = 'd';
    ba1[5] = 'e';
    ba1[6] = 'f';

    QTest::newRow( "2" ) << ba1 << QString::fromUtf16(u"abc\0def", 7);

    QTest::newRow( "3" ) << QByteArray::fromRawData("abcd", 3) << QString("abc");
    QTest::newRow( "4" ) << QByteArray("\xc3\xa9") << QString("\xc3\xa9");
    QTest::newRow( "4-bis" ) << QByteArray("\xc3\xa9") << QString::fromUtf8("\xc3\xa9");
    QTest::newRow( "4-tre" ) << QByteArray("\xc3\xa9") << QString::fromLatin1("\xe9");
}

void tst_QString::insert_data(bool emptyIsNoop)
{
    QTest::addColumn<QString>("s");
    QTest::addColumn<CharStarContainer>("arg");
    QTest::addColumn<int>("a1");
    QTest::addColumn<QString>("expected");

    const CharStarContainer nullC;
    const CharStarContainer emptyC("");
    const CharStarContainer aC("a");
    const CharStarContainer bC("b");
    //const CharStarContainer abC("ab");
    const CharStarContainer baC("ba");

    const QString null;
    const QString empty("");
    const QString a("a");
    const QString b("b");
    const QString ab("ab");
    const QString ba("ba");

    QTest::newRow("null.insert(0, null)") << null << nullC << 0 << null;
    QTest::newRow("null.insert(0, empty)") << null << emptyC << 0 << (emptyIsNoop ? null : empty);
    QTest::newRow("null.insert(0, a)") << null << aC << 0 << a;
    QTest::newRow("empty.insert(0, null)") << empty << nullC << 0 << empty;
    QTest::newRow("empty.insert(0, empty)") << empty << emptyC << 0 << empty;
    QTest::newRow("empty.insert(0, a)") << empty << aC << 0 << a;
    QTest::newRow("a.insert(0, null)") << a << nullC << 0 << a;
    QTest::newRow("a.insert(0, empty)") << a << emptyC << 0 << a;
    QTest::newRow("a.insert(0, b)") << a << bC << 0 << ba;
    QTest::newRow("a.insert(0, ba)") << a << baC << 0 << (ba + a);
    QTest::newRow("a.insert(1, null)") << a << nullC << 1 << a;
    QTest::newRow("a.insert(1, empty)") << a << emptyC << 1 << a;
    QTest::newRow("a.insert(1, b)") << a << bC << 1 << ab;
    QTest::newRow("a.insert(1, ba)") << a << baC << 1 << (a + ba);
    QTest::newRow("ba.insert(1, a)") << ba << aC << 1 << (ba + a);
    QTest::newRow("ba.insert(2, b)") << ba << bC << 2 << (ba + b);
}

void tst_QString::insert_special_cases()
{
    QString a;

    a = "Ys";
    QCOMPARE(a.insert(1,'e'), QString("Yes"));
    QCOMPARE(a.insert(3,'!'), QString("Yes!"));
    QCOMPARE(a.insert(5,'?'), QString("Yes! ?"));
    QCOMPARE(a.insert(-1,'a'), QString("Yes! a?"));

    a = "ABC";
    QCOMPARE(a.insert(5,"DEF"), QString("ABC  DEF"));

    a = "ABC";
    QCOMPARE(a.insert(2, QString()), QString("ABC"));
    QCOMPARE(a.insert(0,"ABC"), QString("ABCABC"));
    QCOMPARE(a, QString("ABCABC"));
    QCOMPARE(a.insert(0,a), QString("ABCABCABCABC"));

    QCOMPARE(a, QString("ABCABCABCABC"));
    QCOMPARE(a.insert(0,'<'), QString("<ABCABCABCABC"));
    QCOMPARE(a.insert(1,'>'), QString("<>ABCABCABCABC"));

    a = "Meal";
    const QString montreal = QStringLiteral("Montreal");
    QCOMPARE(a.insert(1, QLatin1String("ontr")), montreal);
    QCOMPARE(a.insert(4, ""), montreal);
    QCOMPARE(a.insert(3, QLatin1String("")), montreal);
    QCOMPARE(a.insert(3, QLatin1String(0)), montreal);
    QCOMPARE(a.insert(3, static_cast<const char *>(0)), montreal);
    QCOMPARE(a.insert(0, QLatin1String("a")), QLatin1String("aMontreal"));

    a = "Mont";
    QCOMPARE(a.insert(a.size(), QLatin1String("real")), montreal);
    QCOMPARE(a.insert(a.size() + 1, QLatin1String("ABC")), QString("Montreal ABC"));

    a = "AEF";
    QCOMPARE(a.insert(1, QLatin1String("BCD")), QString("ABCDEF"));
    QCOMPARE(a.insert(3, QLatin1String("-")), QString("ABC-DEF"));
    QCOMPARE(a.insert(a.size() + 1, QLatin1String("XYZ")), QString("ABC-DEF XYZ"));

    {
        a = "one";
        a.prepend(u'a');
        QString b(a.data_ptr()->freeSpaceAtEnd(), u'b');
        QCOMPARE(a.insert(a.size() + 1, QLatin1String(b.toLatin1())), QString("aone ") + b);
    }

    {
        a = "onetwothree";
        while (a.size() - 1)
            a.remove(0, 1);
        QString b(a.data_ptr()->freeSpaceAtEnd() + 1, u'b');
        QCOMPARE(a.insert(a.size() + 1, QLatin1String(b.toLatin1())), QString("e ") + b);
    }

    {
        a = "one";
        a.prepend(u'a');
        QString b(a.data_ptr()->freeSpaceAtEnd(), u'b');
        QCOMPARE(a.insert(a.size() + 1, b), QString("aone ") + b);
    }

    {
        a = "onetwothree";
        while (a.size() - 1)
            a.remove(0, 1);
        QString b(a.data_ptr()->freeSpaceAtEnd() + 1, u'b');
        QCOMPARE(a.insert(a.size() + 1, b), QString("e ") + b);
    }
}

void tst_QString::simplified_data() {
    QTest::addColumn<QString>("full" );
    QTest::addColumn<QString>("simple" );

    QTest::newRow("null") << QString() << QString();
    QTest::newRow("empty") << "" << "";
    QTest::newRow("one char") << "a" << "a";
    QTest::newRow("one word") << "foo" << "foo";
    QTest::newRow("chars trivial") << "a b" << "a b";
    QTest::newRow("words trivial") << "foo bar" << "foo bar";
    QTest::newRow("allspace") << "  \t\v " << "";
    QTest::newRow("char trailing") << "a " << "a";
    QTest::newRow("char trailing tab") << "a\t" << "a";
    QTest::newRow("char multitrailing") << "a   " << "a";
    QTest::newRow("char multitrailing tab") << "a   \t" << "a";
    QTest::newRow("char leading") << " a" << "a";
    QTest::newRow("char leading tab") << "\ta" << "a";
    QTest::newRow("char multileading") << "   a" << "a";
    QTest::newRow("char multileading tab") << "\t   a" << "a";
    QTest::newRow("chars apart") << "a  b" << "a b";
    QTest::newRow("words apart") << "foo  bar" << "foo bar";
    QTest::newRow("enclosed word") << "   foo \t " << "foo";
    QTest::newRow("enclosed chars apart") << " a   b " << "a b";
    QTest::newRow("enclosed words apart") << " foo   bar " << "foo bar";
    QTest::newRow("chars apart posttab") << "a \tb" << "a b";
    QTest::newRow("chars apart pretab") << "a\t b" << "a b";
    QTest::newRow("many words") << "  just some    random\ttext here" << "just some random text here";
    QTest::newRow("newlines") << "a\nb\nc" << "a b c";
    QTest::newRow("newlines-trailing") << "a\nb\nc\n" << "a b c";
}

void tst_QString::simplified() {
    QFETCH(QString, full);
    QFETCH(QString, simple);

    QString orig_full = full;
    orig_full.data();       // forces a detach

    QString result = full.simplified();
    if (simple.isNull()) {
        QVERIFY(result.isNull());
    } else if (simple.isEmpty()) {
        QVERIFY(result.isEmpty() && !result.isNull());
    } else {
        QCOMPARE(result, simple);
    }
    QCOMPARE(full, orig_full);

    // without detaching:
    QString copy1 = full;
    QCOMPARE(std::move(full).simplified(), simple);
    QCOMPARE(full, orig_full);

    // force a detach
    if (!full.isEmpty())
        full[0] = full[0];
    QCOMPARE(std::move(full).simplified(), simple);
}

void tst_QString::trimmed() {
    QString a;

    QVERIFY(a.trimmed().isNull()); // lvalue
    QVERIFY(QString().trimmed().isNull()); // rvalue
    QVERIFY(!a.isDetached());

    a="Text";
    QCOMPARE(a, QLatin1String("Text"));
    QCOMPARE(a.trimmed(), QLatin1String("Text"));
    QCOMPARE(a, QLatin1String("Text"));
    a=" ";
    QCOMPARE(a.trimmed(), QLatin1String(""));
    QCOMPARE(a, QLatin1String(" "));
    a=" a   ";
    QCOMPARE(a.trimmed(), QLatin1String("a"));

    a="Text";
    QCOMPARE(std::move(a).trimmed(), QLatin1String("Text"));
    a=" ";
    QCOMPARE(std::move(a).trimmed(), QLatin1String(""));
    a=" a   ";
    QCOMPARE(std::move(a).trimmed(), QLatin1String("a"));
}

void tst_QString::unicodeTableAccess_data() {
    QTest::addColumn<QString>("invalid");

    const auto join = [](char16_t high, char16_t low) {
        const QChar pair[2] = { high, low };
        return QString(pair, 2);
    };
    // Least high surrogate for which an invalid successor produces an error:
    QTest::newRow("least-high") << join(0xdbf8, 0xfc00);
    // Least successor that, after a high surrogate, produces invalid:
    QTest::newRow("least-follow") << join(0xdbff, 0xe000);
}

void tst_QString::unicodeTableAccess() {
    // QString processing must not access unicode tables out of bounds.
    QFETCH(QString, invalid);
    // Exercise methods, to see if any assertions trigger:
    const auto upper = invalid.toUpper();
    const auto lower = invalid.toLower();
    const auto folded = invalid.toCaseFolded();
    // Fatuous test, just to use those.
    QVERIFY(upper == invalid || lower == invalid || folded == invalid || lower != upper);
}

void tst_QString::toUpper()
{
    const QString s;
    QCOMPARE( s.toUpper(), QString() ); // lvalue
    QCOMPARE( QString().toUpper(), QString() ); // rvalue
    QCOMPARE( QString("").toUpper(), QString("") );
    QCOMPARE( QStringLiteral("text").toUpper(), QString("TEXT") );
    QCOMPARE( QString("text").toUpper(), QString("TEXT") );
    QCOMPARE( QString("Text").toUpper(), QString("TEXT") );
    QCOMPARE( QString("tExt").toUpper(), QString("TEXT") );
    QCOMPARE( QString("teXt").toUpper(), QString("TEXT") );
    QCOMPARE( QString("texT").toUpper(), QString("TEXT") );
    QCOMPARE( QString("TExt").toUpper(), QString("TEXT") );
    QCOMPARE( QString("teXT").toUpper(), QString("TEXT") );
    QCOMPARE( QString("tEXt").toUpper(), QString("TEXT") );
    QCOMPARE( QString("tExT").toUpper(), QString("TEXT") );
    QCOMPARE( QString("TEXT").toUpper(), QString("TEXT") );
    QCOMPARE( QString("@ABYZ[").toUpper(), QString("@ABYZ["));
    QCOMPARE( QString("@abyz[").toUpper(), QString("@ABYZ["));
    QCOMPARE( QString("`ABYZ{").toUpper(), QString("`ABYZ{"));
    QCOMPARE( QString("`abyz{").toUpper(), QString("`ABYZ{"));

    QCOMPARE( QString(1, QChar(0xdf)).toUpper(), QString("SS"));
    {
        QString s = QString::fromUtf8("Gro\xc3\x9fstra\xc3\x9f""e");

        // call lvalue-ref version, mustn't change the original
        QCOMPARE(s.toUpper(), QString("GROSSSTRASSE"));
        QCOMPARE(s, QString::fromUtf8("Gro\xc3\x9fstra\xc3\x9f""e"));

        // call rvalue-ref while shared (the original mustn't change)
        QString copy = s;
        QCOMPARE(std::move(copy).toUpper(), QString("GROSSSTRASSE"));
        QCOMPARE(s, QString::fromUtf8("Gro\xc3\x9fstra\xc3\x9f""e"));

        // call rvalue-ref version on detached case
        copy.clear();
        QCOMPARE(std::move(s).toUpper(), QString("GROSSSTRASSE"));
    }

    QString lower, upper;
    lower += QChar(QChar::highSurrogate(0x10428));
    lower += QChar(QChar::lowSurrogate(0x10428));
    upper += QChar(QChar::highSurrogate(0x10400));
    upper += QChar(QChar::lowSurrogate(0x10400));
    QCOMPARE( lower.toUpper(), upper);
    lower += lower;
    upper += upper;
    QCOMPARE( lower.toUpper(), upper);

    // test for broken surrogate pair handling (low low hi low hi low)
    lower.prepend(QChar(QChar::lowSurrogate(0x10428)));
    lower.prepend(QChar(QChar::lowSurrogate(0x10428)));
    upper.prepend(QChar(QChar::lowSurrogate(0x10428)));
    upper.prepend(QChar(QChar::lowSurrogate(0x10428)));
    QCOMPARE(lower.toUpper(), upper);
    // test for broken surrogate pair handling (low low hi low hi low hi hi)
    lower += QChar(QChar::highSurrogate(0x10428));
    lower += QChar(QChar::highSurrogate(0x10428));
    upper += QChar(QChar::highSurrogate(0x10428));
    upper += QChar(QChar::highSurrogate(0x10428));
    QCOMPARE(lower.toUpper(), upper);

#if QT_CONFIG(icu)
    // test doesn't work with ICU support, since QChar is unaware of any locale
    QEXPECT_FAIL("", "test doesn't work with ICU support, since QChar is unaware of any locale", Continue);
    QVERIFY(false);
#else
    for (int i = 0; i < 65536; ++i) {
        QString str(1, QChar(i));
        QString upper = str.toUpper();
        QVERIFY(upper.length() >= 1);
        if (upper.length() == 1)
            QVERIFY(upper == QString(1, QChar(i).toUpper()));
    }
#endif // icu
}

void tst_QString::toLower()
{
    const QString s;
    QCOMPARE(s.toLower(), QString()); // lvalue
    QCOMPARE( QString().toLower(), QString() ); // rvalue
    QCOMPARE( QString("").toLower(), QString("") );
    QCOMPARE( QString("text").toLower(), QString("text") );
    QCOMPARE( QStringLiteral("Text").toLower(), QString("text") );
    QCOMPARE( QString("Text").toLower(), QString("text") );
    QCOMPARE( QString("tExt").toLower(), QString("text") );
    QCOMPARE( QString("teXt").toLower(), QString("text") );
    QCOMPARE( QString("texT").toLower(), QString("text") );
    QCOMPARE( QString("TExt").toLower(), QString("text") );
    QCOMPARE( QString("teXT").toLower(), QString("text") );
    QCOMPARE( QString("tEXt").toLower(), QString("text") );
    QCOMPARE( QString("tExT").toLower(), QString("text") );
    QCOMPARE( QString("TEXT").toLower(), QString("text") );
    QCOMPARE( QString("@ABYZ[").toLower(), QString("@abyz["));
    QCOMPARE( QString("@abyz[").toLower(), QString("@abyz["));
    QCOMPARE( QString("`ABYZ{").toLower(), QString("`abyz{"));
    QCOMPARE( QString("`abyz{").toLower(), QString("`abyz{"));

    QCOMPARE( QString(1, QChar(0x130)).toLower(), QString(QString(1, QChar(0x69)) + QChar(0x307)));

    QString lower, upper;
    lower += QChar(QChar::highSurrogate(0x10428));
    lower += QChar(QChar::lowSurrogate(0x10428));
    upper += QChar(QChar::highSurrogate(0x10400));
    upper += QChar(QChar::lowSurrogate(0x10400));
    QCOMPARE( upper.toLower(), lower);
    lower += lower;
    upper += upper;
    QCOMPARE( upper.toLower(), lower);

    // test for broken surrogate pair handling (low low hi low hi low)
    lower.prepend(QChar(QChar::lowSurrogate(0x10400)));
    lower.prepend(QChar(QChar::lowSurrogate(0x10400)));
    upper.prepend(QChar(QChar::lowSurrogate(0x10400)));
    upper.prepend(QChar(QChar::lowSurrogate(0x10400)));
    QCOMPARE( upper.toLower(), lower);
    // test for broken surrogate pair handling (low low hi low hi low hi hi)
    lower += QChar(QChar::highSurrogate(0x10400));
    lower += QChar(QChar::highSurrogate(0x10400));
    upper += QChar(QChar::highSurrogate(0x10400));
    upper += QChar(QChar::highSurrogate(0x10400));
    QCOMPARE( upper.toLower(), lower);

#if QT_CONFIG(icu)
    // test doesn't work with ICU support, since QChar is unaware of any locale
    QEXPECT_FAIL("", "test doesn't work with ICU support, since QChar is unaware of any locale", Continue);
    QVERIFY(false);
#else
    for (int i = 0; i < 65536; ++i) {
        QString str(1, QChar(i));
        QString lower = str.toLower();
        QVERIFY(lower.length() >= 1);
        if (lower.length() == 1)
            QVERIFY(str.toLower() == QString(1, QChar(i).toLower()));
    }
#endif // icu
}

void tst_QString::isLower_isUpper_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<bool>("isLower");
    QTest::addColumn<bool>("isUpper");

    int row = 0;
    QTest::addRow("lower-and-upper-%02d", row++) << QString() << true << true;
    QTest::addRow("lower-and-upper-%02d", row++) << QString("") << true << true;
    QTest::addRow("lower-and-upper-%02d", row++) << QString(" ") << true << true;
    QTest::addRow("lower-and-upper-%02d", row++) << QString("123") << true << true;
    QTest::addRow("lower-and-upper-%02d", row++) << QString("@123$#") << true << true;
    QTest::addRow("lower-and-upper-%02d", row++) << QString("𝄞𝄴𝆏♫") << true << true; // Unicode Block 'Musical Symbols'
    // not foldable
    QTest::addRow("lower-and-upper-%02d", row++) << QString("𝚊𝚋𝚌𝚍𝚎") << true << true; // MATHEMATICAL MONOSPACE SMALL A, ... E
    QTest::addRow("lower-and-upper-%02d", row++) << QString("𝙖,𝙗,𝙘,𝙙,𝙚") << true << true; // MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL A, ... E
    QTest::addRow("lower-and-upper-%02d", row++) << QString("𝗔𝗕𝗖𝗗𝗘") << true << true; // MATHEMATICAL SANS-SERIF BOLD CAPITAL A, ... E
    QTest::addRow("lower-and-upper-%02d", row++) << QString("𝐀,𝐁,𝐂,𝐃,𝐄") << true << true; // MATHEMATICAL BOLD CAPITAL A, ... E

    row = 0;
    QTest::addRow("only-lower-%02d", row++) << QString("text") << true << false;
    QTest::addRow("only-lower-%02d", row++) << QString("àaa") << true << false;
    QTest::addRow("only-lower-%02d", row++) << QString("øæß") << true << false;
    QTest::addRow("only-lower-%02d", row++) << QString("text ") << true << false;
    QTest::addRow("only-lower-%02d", row++) << QString(" text") << true << false;
    QTest::addRow("only-lower-%02d", row++) << QString("hello, world!") << true << false;
    QTest::addRow("only-lower-%02d", row++) << QString("123@abyz[") << true << false;
    QTest::addRow("only-lower-%02d", row++) << QString("`abyz{") << true << false;
    QTest::addRow("only-lower-%02d", row++) << QString("a𝙖a|b𝙗b|c𝙘c|d𝙙d|e𝙚e") << true << false; // MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL A, ... E
    QTest::addRow("only-lower-%02d", row++) << QString("𐐨") << true << false; // DESERET SMALL LETTER LONG I
    // uppercase letters, not foldable
    QTest::addRow("only-lower-%02d", row++) << QString("text𝗔text") << true << false; // MATHEMATICAL SANS-SERIF BOLD CAPITAL A

    row = 0;
    QTest::addRow("only-upper-%02d", row++) << QString("TEXT") << false << true;
    QTest::addRow("only-upper-%02d", row++) << QString("ÀAA") << false << true;
    QTest::addRow("only-upper-%02d", row++) << QString("ØÆẞ") << false << true;
    QTest::addRow("only-upper-%02d", row++) << QString("TEXT ") << false << true;
    QTest::addRow("only-upper-%02d", row++) << QString(" TEXT") << false << true;
    QTest::addRow("only-upper-%02d", row++) << QString("HELLO, WORLD!") << false << true;
    QTest::addRow("only-upper-%02d", row++) << QString("123@ABYZ[") << false << true;
    QTest::addRow("only-upper-%02d", row++) << QString("`ABYZ{") << false << true;
    QTest::addRow("only-upper-%02d", row++) << QString("A𝐀A|B𝐁B|C𝐂C|D𝐃D|E𝐄E") << false << true; // MATHEMATICAL BOLD CAPITAL A, ... E
    QTest::addRow("only-upper-%02d", row++) << QString("𐐀") << false << true; // DESERET CAPITAL LETTER LONG I
    // lowercase letters, not foldable
    QTest::addRow("only-upper-%02d", row++) << QString("TEXT𝚊TEXT") << false << true; // MATHEMATICAL MONOSPACE SMALL A

    row = 0;
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("Text") << false << false;
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("tExt") << false << false;
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("teXt") << false << false;
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("texT") << false << false;
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("TExt") << false << false;
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("teXT") << false << false;
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("tEXt") << false << false;
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("tExT") << false << false;
    // not foldable
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("TEXT𝚊text") << false << false; // MATHEMATICAL MONOSPACE SMALL A
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("text𝗔TEXT") << false << false; // MATHEMATICAL SANS-SERIF BOLD CAPITAL A
    // titlecase, foldable
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("abcǈdef") << false << false; // LATIN CAPITAL LETTER L WITH SMALL LETTER J
    QTest::addRow("not-lower-nor-upper-%02d", row++) << QString("ABCǈDEF") << false << false; // LATIN CAPITAL LETTER L WITH SMALL LETTER J
}

void tst_QString::isLower_isUpper()
{
    QFETCH(QString, string);
    QFETCH(bool, isLower);
    QFETCH(bool, isUpper);

    QCOMPARE(string.isLower(), isLower);
    QCOMPARE(string.toLower() == string, isLower);
    QVERIFY(string.toLower().isLower());

    QCOMPARE(string.isUpper(), isUpper);
    QCOMPARE(string.toUpper() == string, isUpper);
    QVERIFY(string.toUpper().isUpper());
}

void tst_QString::toCaseFolded()
{
    const QString s;
    QCOMPARE( s.toCaseFolded(), QString() ); // lvalue
    QCOMPARE( QString().toCaseFolded(), QString() ); // rvalue
    QCOMPARE( QString("").toCaseFolded(), QString("") );
    QCOMPARE( QString("text").toCaseFolded(), QString("text") );
    QCOMPARE( QString("Text").toCaseFolded(), QString("text") );
    QCOMPARE( QString("tExt").toCaseFolded(), QString("text") );
    QCOMPARE( QString("teXt").toCaseFolded(), QString("text") );
    QCOMPARE( QString("texT").toCaseFolded(), QString("text") );
    QCOMPARE( QString("TExt").toCaseFolded(), QString("text") );
    QCOMPARE( QString("teXT").toCaseFolded(), QString("text") );
    QCOMPARE( QString("tEXt").toCaseFolded(), QString("text") );
    QCOMPARE( QString("tExT").toCaseFolded(), QString("text") );
    QCOMPARE( QString("TEXT").toCaseFolded(), QString("text") );
    QCOMPARE( QString("@ABYZ[").toCaseFolded(), QString("@abyz["));
    QCOMPARE( QString("@abyz[").toCaseFolded(), QString("@abyz["));
    QCOMPARE( QString("`ABYZ{").toCaseFolded(), QString("`abyz{"));
    QCOMPARE( QString("`abyz{").toCaseFolded(), QString("`abyz{"));

    QCOMPARE( QString(1, QChar(0xa77d)).toCaseFolded(), QString(1, QChar(0x1d79)));
    QCOMPARE( QString(1, QChar(0xa78d)).toCaseFolded(), QString(1, QChar(0x0265)));

    QString lower, upper;
    upper += QChar(QChar::highSurrogate(0x10400));
    upper += QChar(QChar::lowSurrogate(0x10400));
    lower += QChar(QChar::highSurrogate(0x10428));
    lower += QChar(QChar::lowSurrogate(0x10428));
    QCOMPARE( upper.toCaseFolded(), lower);
    lower += lower;
    upper += upper;
    QCOMPARE( upper.toCaseFolded(), lower);

    // test for broken surrogate pair handling (low low hi low hi low)
    lower.prepend(QChar(QChar::lowSurrogate(0x10400)));
    lower.prepend(QChar(QChar::lowSurrogate(0x10400)));
    upper.prepend(QChar(QChar::lowSurrogate(0x10400)));
    upper.prepend(QChar(QChar::lowSurrogate(0x10400)));
    QCOMPARE(upper.toCaseFolded(), lower);
    // test for broken surrogate pair handling (low low hi low hi low hi hi)
    lower += QChar(QChar::highSurrogate(0x10400));
    lower += QChar(QChar::highSurrogate(0x10400));
    upper += QChar(QChar::highSurrogate(0x10400));
    upper += QChar(QChar::highSurrogate(0x10400));
    QCOMPARE(upper.toCaseFolded(), lower);

    //### we currently don't support full case foldings
    for (int i = 0; i < 65536; ++i) {
        QString str(1, QChar(i));
        QString lower = str.toCaseFolded();
        QVERIFY(lower.length() >= 1);
        if (lower.length() == 1)
            QVERIFY(str.toCaseFolded() == QString(1, QChar(i).toCaseFolded()));
    }
}

void tst_QString::leftJustified()
{
    QString a;

    QCOMPARE(a.leftJustified(3, '-'), QLatin1String("---"));
    QCOMPARE(a.leftJustified(2), QLatin1String("  "));
    QVERIFY(!a.isDetached());

    a="ABC";
    QCOMPARE(a.leftJustified(5,'-'), QLatin1String("ABC--"));
    QCOMPARE(a.leftJustified(4,'-'), QLatin1String("ABC-"));
    QCOMPARE(a.leftJustified(4), QLatin1String("ABC "));
    QCOMPARE(a.leftJustified(3), QLatin1String("ABC"));
    QCOMPARE(a.leftJustified(2), QLatin1String("ABC"));
    QCOMPARE(a.leftJustified(1), QLatin1String("ABC"));
    QCOMPARE(a.leftJustified(0), QLatin1String("ABC"));

    QCOMPARE(a.leftJustified(4,' ',true), QLatin1String("ABC "));
    QCOMPARE(a.leftJustified(3,' ',true), QLatin1String("ABC"));
    QCOMPARE(a.leftJustified(2,' ',true), QLatin1String("AB"));
    QCOMPARE(a.leftJustified(1,' ',true), QLatin1String("A"));
    QCOMPARE(a.leftJustified(0,' ',true), QLatin1String(""));
}

void tst_QString::rightJustified()
{
    QString a;

    QCOMPARE(a.rightJustified(3, '-'), QLatin1String("---"));
    QCOMPARE(a.rightJustified(2), QLatin1String("  "));
    QVERIFY(!a.isDetached());

    a="ABC";
    QCOMPARE(a.rightJustified(5,'-'), QLatin1String("--ABC"));
    QCOMPARE(a.rightJustified(4,'-'), QLatin1String("-ABC"));
    QCOMPARE(a.rightJustified(4), QLatin1String(" ABC"));
    QCOMPARE(a.rightJustified(3), QLatin1String("ABC"));
    QCOMPARE(a.rightJustified(2), QLatin1String("ABC"));
    QCOMPARE(a.rightJustified(1), QLatin1String("ABC"));
    QCOMPARE(a.rightJustified(0), QLatin1String("ABC"));

    QCOMPARE(a.rightJustified(4,'-',true), QLatin1String("-ABC"));
    QCOMPARE(a.rightJustified(4,' ',true), QLatin1String(" ABC"));
    QCOMPARE(a.rightJustified(3,' ',true), QLatin1String("ABC"));
    QCOMPARE(a.rightJustified(2,' ',true), QLatin1String("AB"));
    QCOMPARE(a.rightJustified(1,' ',true), QLatin1String("A"));
    QCOMPARE(a.rightJustified(0,' ',true), QLatin1String(""));
    QCOMPARE(a, QLatin1String("ABC"));
}

void tst_QString::mid()
{
    QString a;

    QVERIFY(a.mid(0).isNull());
    QVERIFY(a.mid(5, 6).isNull());
    QVERIFY(a.mid(-4, 3).isNull());
    QVERIFY(a.mid(4, -3).isNull());
    QVERIFY(!a.isDetached());

    a="ABCDEFGHIEfGEFG"; // 15 chars

    QCOMPARE(a.mid(3,3), QLatin1String("DEF"));
    QCOMPARE(a.mid(0,0), QLatin1String(""));
    QVERIFY(!a.mid(15,0).isNull());
    QVERIFY(a.mid(15,0).isEmpty());
    QVERIFY(!a.mid(15,1).isNull());
    QVERIFY(a.mid(15,1).isEmpty());
    QVERIFY(a.mid(9999).isNull());
    QVERIFY(a.mid(9999,1).isNull());

    QCOMPARE(a.mid(-1, 6), a.mid(0, 5));
    QVERIFY(a.mid(-100, 6).isEmpty());
    QVERIFY(a.mid(INT_MIN, 0).isEmpty());
    QCOMPARE(a.mid(INT_MIN, -1), a);
    QVERIFY(a.mid(INT_MIN, INT_MAX).isNull());
    QVERIFY(a.mid(INT_MIN + 1, INT_MAX).isEmpty());
    QCOMPARE(a.mid(INT_MIN + 2, INT_MAX), a.left(1));
    QCOMPARE(a.mid(INT_MIN + a.size() + 1, INT_MAX), a);
    QVERIFY(a.mid(INT_MAX).isNull());
    QVERIFY(a.mid(INT_MAX, INT_MAX).isNull());
    QCOMPARE(a.mid(-5, INT_MAX), a);
    QCOMPARE(a.mid(-1, INT_MAX), a);
    QCOMPARE(a.mid(0, INT_MAX), a);
    QCOMPARE(a.mid(1, INT_MAX), QString("BCDEFGHIEfGEFG"));
    QCOMPARE(a.mid(5, INT_MAX), QString("FGHIEfGEFG"));
    QVERIFY(a.mid(20, INT_MAX).isNull());
    QCOMPARE(a.mid(-1, -1), a);

    QString n;
    QVERIFY(n.mid(3,3).isNull());
    QVERIFY(n.mid(0,0).isNull());
    QVERIFY(n.mid(9999,0).isNull());
    QVERIFY(n.mid(9999,1).isNull());

    QVERIFY(n.mid(-1, 6).isNull());
    QVERIFY(n.mid(-100, 6).isNull());
    QVERIFY(n.mid(INT_MIN, 0).isNull());
    QVERIFY(n.mid(INT_MIN, -1).isNull());
    QVERIFY(n.mid(INT_MIN, INT_MAX).isNull());
    QVERIFY(n.mid(INT_MIN + 1, INT_MAX).isNull());
    QVERIFY(n.mid(INT_MIN + 2, INT_MAX).isNull());
    QVERIFY(n.mid(INT_MIN + n.size() + 1, INT_MAX).isNull());
    QVERIFY(n.mid(INT_MAX).isNull());
    QVERIFY(n.mid(INT_MAX, INT_MAX).isNull());
    QVERIFY(n.mid(-5, INT_MAX).isNull());
    QVERIFY(n.mid(-1, INT_MAX).isNull());
    QVERIFY(n.mid(0, INT_MAX).isNull());
    QVERIFY(n.mid(1, INT_MAX).isNull());
    QVERIFY(n.mid(5, INT_MAX).isNull());
    QVERIFY(n.mid(20, INT_MAX).isNull());
    QVERIFY(n.mid(-1, -1).isNull());

    QString x = "Nine pineapples";
    QCOMPARE(x.mid(5, 4), QString("pine"));
    QCOMPARE(x.mid(5), QString("pineapples"));

    QCOMPARE(x.mid(-1, 6), x.mid(0, 5));
    QVERIFY(x.mid(-100, 6).isEmpty());
    QVERIFY(x.mid(INT_MIN, 0).isEmpty());
    QCOMPARE(x.mid(INT_MIN, -1), x);
    QVERIFY(x.mid(INT_MIN, INT_MAX).isNull());
    QVERIFY(x.mid(INT_MIN + 1, INT_MAX).isEmpty());
    QCOMPARE(x.mid(INT_MIN + 2, INT_MAX), x.left(1));
    QCOMPARE(x.mid(INT_MIN + x.size() + 1, INT_MAX), x);
    QVERIFY(x.mid(INT_MAX).isNull());
    QVERIFY(x.mid(INT_MAX, INT_MAX).isNull());
    QCOMPARE(x.mid(-5, INT_MAX), x);
    QCOMPARE(x.mid(-1, INT_MAX), x);
    QCOMPARE(x.mid(0, INT_MAX), x);
    QCOMPARE(x.mid(1, INT_MAX), QString("ine pineapples"));
    QCOMPARE(x.mid(5, INT_MAX), QString("pineapples"));
    QVERIFY(x.mid(20, INT_MAX).isNull());
    QCOMPARE(x.mid(-1, -1), x);
}

void tst_QString::right()
{
    QString a;

    QVERIFY(a.right(0).isNull());
    QVERIFY(a.right(5).isNull());
    QVERIFY(a.right(-4).isNull());
    QVERIFY(!a.isDetached());

    a="ABCDEFGHIEfGEFG"; // 15 chars
    QCOMPARE(a.right(3), QLatin1String("EFG"));
    QCOMPARE(a.right(0), QLatin1String(""));

    QString n;
    QVERIFY(n.right(3).isNull());
    QVERIFY(n.right(0).isNull());

    QString r = "Right";
    QCOMPARE(r.right(-1), r);
    QCOMPARE(r.right(100), r);
}

void tst_QString::left()
{
    QString a;

    QVERIFY(a.left(0).isNull());
    QVERIFY(a.left(5).isNull());
    QVERIFY(a.left(-4).isNull());
    QVERIFY(!a.isDetached());

    a="ABCDEFGHIEfGEFG"; // 15 chars
    QCOMPARE(a.left(3), QLatin1String("ABC"));
    QVERIFY(!a.left(0).isNull());
    QCOMPARE(a.left(0), QLatin1String(""));

    QString n;
    QVERIFY(n.left(3).isNull());
    QVERIFY(n.left(0).isNull());
    QVERIFY(n.left(0).isNull());

    QString l = "Left";
    QCOMPARE(l.left(-1), l);
    QCOMPARE(l.left(100), l);
}

void tst_QString::contains()
{
    QString a;
    a="ABCDEFGHIEfGEFG"; // 15 chars
    QVERIFY(a.contains('A'));
    QVERIFY(!a.contains('Z'));
    QVERIFY(a.contains('E'));
    QVERIFY(a.contains('F'));
    QVERIFY(a.contains('F',Qt::CaseInsensitive));
    QVERIFY(a.contains("FG"));
    QVERIFY(a.contains("FG",Qt::CaseInsensitive));
    QVERIFY(a.contains(QLatin1String("FG")));
    QVERIFY(a.contains(QLatin1String("fg"),Qt::CaseInsensitive));
    QVERIFY(a.contains( QString(), Qt::CaseInsensitive));
    QVERIFY(a.contains( "", Qt::CaseInsensitive));
//#if QT_CONFIG(regularexpression)
//    QVERIFY(a.contains(QRegularExpression("[FG][HI]")));
//    QVERIFY(a.contains(QRegularExpression("[G][HE]")));
//
//    {
//        QRegularExpressionMatch match;
//        QVERIFY(!match.hasMatch());
//
//        QVERIFY(a.contains(QRegularExpression("[FG][HI]"), &match));
//        QVERIFY(match.hasMatch());
//        QCOMPARE(match.capturedStart(), 6);
//        QCOMPARE(match.capturedEnd(), 8);
//        QCOMPARE(match.captured(), QStringLiteral("GH"));
//
//        QVERIFY(a.contains(QRegularExpression("[G][HE]"), &match));
//        QVERIFY(match.hasMatch());
//        QCOMPARE(match.capturedStart(), 6);
//        QCOMPARE(match.capturedEnd(), 8);
//        QCOMPARE(match.captured(), QStringLiteral("GH"));
//
//        QVERIFY(a.contains(QRegularExpression("[f](.*)[FG]"), &match));
//        QVERIFY(match.hasMatch());
//        QCOMPARE(match.capturedStart(), 10);
//        QCOMPARE(match.capturedEnd(), 15);
//        QCOMPARE(match.captured(), QString("fGEFG"));
//        QCOMPARE(match.capturedStart(1), 11);
//        QCOMPARE(match.capturedEnd(1), 14);
//        QCOMPARE(match.captured(1), QStringLiteral("GEF"));
//
//        QVERIFY(a.contains(QRegularExpression("[f](.*)[F]"), &match));
//        QVERIFY(match.hasMatch());
//        QCOMPARE(match.capturedStart(), 10);
//        QCOMPARE(match.capturedEnd(), 14);
//        QCOMPARE(match.captured(), QString("fGEF"));
//        QCOMPARE(match.capturedStart(1), 11);
//        QCOMPARE(match.capturedEnd(1), 13);
//        QCOMPARE(match.captured(1), QStringLiteral("GE"));
//
//        QVERIFY(!a.contains(QRegularExpression("ZZZ"), &match));
//        // doesn't match, but ensure match didn't change
//        QVERIFY(match.hasMatch());
//        QCOMPARE(match.capturedStart(), 10);
//        QCOMPARE(match.capturedEnd(), 14);
//        QCOMPARE(match.captured(), QStringLiteral("fGEF"));
//        QCOMPARE(match.capturedStart(1), 11);
//        QCOMPARE(match.capturedEnd(1), 13);
//        QCOMPARE(match.captured(1), QStringLiteral("GE"));
//
//        // don't crash with a null pointer
//        QVERIFY(a.contains(QRegularExpression("[FG][HI]"), 0));
//        QVERIFY(!a.contains(QRegularExpression("ZZZ"), 0));
//    }
//
//    QTest::ignoreMessage(QtWarningMsg, "QString::contains: invalid QRegularExpression object");
//    QVERIFY(!a.contains(QRegularExpression("invalid regex\\")));
//#endif

    CREATE_VIEW(QLatin1String("FG"));
    QVERIFY(a.contains(view));
    QVERIFY(a.contains(view, Qt::CaseInsensitive));
    QVERIFY(a.contains( QStringView(), Qt::CaseInsensitive));

    QString nullStr;
    QVERIFY(!nullStr.contains('A'));
    QVERIFY(!nullStr.contains("AB"));
    QVERIFY(!nullStr.contains(view));
//#if QT_CONFIG(regularexpression)
//    QVERIFY(!nullStr.contains(QRegularExpression("[FG][HI]")));
//    QRegularExpressionMatch nullMatch;
//    QVERIFY(nullStr.contains(QRegularExpression(""), &nullMatch));
//    QVERIFY(nullMatch.hasMatch());
//    QCOMPARE(nullMatch.captured(), "");
//    QCOMPARE(nullMatch.capturedStart(), 0);
//    QCOMPARE(nullMatch.capturedEnd(), 0);
//#endif
    QVERIFY(!nullStr.isDetached());

    QString emptyStr("");
    QVERIFY(!emptyStr.contains('A'));
    QVERIFY(!emptyStr.contains("AB"));
    QVERIFY(!emptyStr.contains(view));
//#if QT_CONFIG(regularexpression)
//    QVERIFY(!emptyStr.contains(QRegularExpression("[FG][HI]")));
//    QRegularExpressionMatch emptyMatch;
//    QVERIFY(emptyStr.contains(QRegularExpression(""), &emptyMatch));
//    QVERIFY(emptyMatch.hasMatch());
//    QCOMPARE(emptyMatch.captured(), "");
//    QCOMPARE(emptyMatch.capturedStart(), 0);
//    QCOMPARE(emptyMatch.capturedEnd(), 0);
//#endif
    QVERIFY(!emptyStr.isDetached());
}

void tst_QString::lastIndexOf_data()
{
    QTest::addColumn<QString>("haystack" );
    QTest::addColumn<QString>("needle" );
    QTest::addColumn<int>("from" );
    QTest::addColumn<int>("expected" );
    QTest::addColumn<bool>("caseSensitive" );

    QString a = "ABCDEFGHIEfGEFG";

    QTest::newRow("-1") << a << "G" << int(a.size()) - 1 << 14 << true;
    QTest::newRow("1") << a << "G" << - 1 << 14 << true;
    QTest::newRow("2") << a << "G" << -3 << 11 << true;
    QTest::newRow("3") << a << "G" << -5 << 6 << true;
    QTest::newRow("4") << a << "G" << 14 << 14 << true;
    QTest::newRow("5") << a << "G" << 13 << 11 << true;
    QTest::newRow("6") << a << "B" << int(a.size()) - 1 << 1 << true;
    QTest::newRow("7") << a << "B" << - 1 << 1 << true;
    QTest::newRow("8") << a << "B" << 1 << 1 << true;
    QTest::newRow("9") << a << "B" << 0 << -1 << true;

    QTest::newRow("10") << a << "G" <<  -1 <<  int(a.size())-1 << true;
    QTest::newRow("11") << a << "G" <<  int(a.size())-1 <<  int(a.size())-1 << true;
    QTest::newRow("12") << a << "G" <<  int(a.size()) <<  int(a.size())-1 << true;
    QTest::newRow("13") << a << "A" <<  0 <<  0 << true;
    QTest::newRow("14") << a << "A" <<  -1*int(a.size()) <<  0 << true;

    QTest::newRow("15") << a << "efg" << 0 << -1 << false;
    QTest::newRow("16") << a << "efg" << int(a.size()) << 12 << false;
    QTest::newRow("17") << a << "efg" << -1 * int(a.size()) << -1 << false;
    QTest::newRow("19") << a << "efg" << int(a.size()) - 1 << 12 << false;
    QTest::newRow("20") << a << "efg" << 12 << 12 << false;
    QTest::newRow("21") << a << "efg" << -12 << -1 << false;
    QTest::newRow("22") << a << "efg" << 11 << 9 << false;

    QTest::newRow("24") << "" << "asdf" << -1 << -1 << false;
    QTest::newRow("25") << "asd" << "asdf" << -1 << -1 << false;
    QTest::newRow("26") << "" << QString() << -1 << -1 << false;

    QTest::newRow("27") << a << "" << int(a.size()) << int(a.size()) << false;
    QTest::newRow("28") << a << "" << int(a.size()) + 10 << -1 << false;

    QTest::newRow("null-in-null") << QString() << QString() << 0 << 0 << false;
    QTest::newRow("empty-in-null") << QString() << QString("") << 0 << 0 << false;
    QTest::newRow("null-in-empty") << QString("") << QString() << 0 << 0 << false;
    QTest::newRow("empty-in-empty") << QString("") << QString("") << 0 << 0 << false;
    QTest::newRow("data-in-null") << QString() << QString("a") << 0 << -1 << false;
    QTest::newRow("data-in-empty") << QString("") << QString("a") << 0 << -1 << false;
}

void tst_QString::lastIndexOf()
{
    QFETCH(QString, haystack);
    QFETCH(QString, needle);
    QFETCH(int, from);
    QFETCH(int, expected);
    QFETCH(bool, caseSensitive);
    CREATE_VIEW(needle);

    Qt::CaseSensitivity cs = (caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);

    QCOMPARE(haystack.lastIndexOf(needle, from, cs), expected);
    QCOMPARE(haystack.lastIndexOf(view, from, cs), expected);
    QCOMPARE(haystack.lastIndexOf(needle.toLatin1(), from, cs), expected);
    QCOMPARE(haystack.lastIndexOf(needle.toLatin1().data(), from, cs), expected);

//#if QT_CONFIG(regularexpression)
//    if (from >= -1 && from < haystack.size() && needle.size() > 0) {
//        // unfortunately, QString and QRegularExpression don't have the same out of bound semantics
//        // I think QString is wrong -- See file log for contact information.
//        {
//            QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
//            if (!caseSensitive)
//                options |= QRegularExpression::CaseInsensitiveOption;
//
//            QRegularExpression re(QRegularExpression::escape(needle), options);
//            QCOMPARE(haystack.lastIndexOf(re, from), expected);
//            QCOMPARE(haystack.lastIndexOf(re, from, nullptr), expected);
//            QRegularExpressionMatch match;
//            QVERIFY(!match.hasMatch());
//            QCOMPARE(haystack.lastIndexOf(re, from, &match), expected);
//            QCOMPARE(match.hasMatch(), expected > -1);
//            if (expected > -1) {
//                if (caseSensitive)
//                    QCOMPARE(match.captured(), needle);
//                else
//                    QCOMPARE(match.captured().toLower(), needle.toLower());
//            }
//        }
//    }
//#endif

    if (cs == Qt::CaseSensitive) {
        QCOMPARE(haystack.lastIndexOf(needle, from), expected);
        QCOMPARE(haystack.lastIndexOf(view, from), expected);
        QCOMPARE(haystack.lastIndexOf(needle.toLatin1(), from), expected);
        QCOMPARE(haystack.lastIndexOf(needle.toLatin1().data(), from), expected);
        if (from == haystack.size()) {
            QCOMPARE(haystack.lastIndexOf(needle), expected);
            QCOMPARE(haystack.lastIndexOf(view), expected);
            QCOMPARE(haystack.lastIndexOf(needle.toLatin1()), expected);
            QCOMPARE(haystack.lastIndexOf(needle.toLatin1().data()), expected);
        }
    }
    if (needle.size() == 1) {
        QCOMPARE(haystack.lastIndexOf(needle.at(0), from), expected);
        QCOMPARE(haystack.lastIndexOf(view.at(0), from), expected);
    }
}

void tst_QString::indexOf_data()
{
    QTest::addColumn<QString>("haystack" );
    QTest::addColumn<QString>("needle" );
    QTest::addColumn<int>("startpos" );
    QTest::addColumn<bool>("bcs" );
    QTest::addColumn<int>("resultpos" );

    QTest::newRow( "data0" ) << QString("abc") << QString("a") << 0 << true << 0;
    QTest::newRow( "data1" ) << QString("abc") << QString("a") << 0 << false << 0;
    QTest::newRow( "data2" ) << QString("abc") << QString("A") << 0 << true << -1;
    QTest::newRow( "data3" ) << QString("abc") << QString("A") << 0 << false << 0;
    QTest::newRow( "data4" ) << QString("abc") << QString("a") << 1 << true << -1;
    QTest::newRow( "data5" ) << QString("abc") << QString("a") << 1 << false << -1;
    QTest::newRow( "data6" ) << QString("abc") << QString("A") << 1 << true << -1;
    QTest::newRow( "data7" ) << QString("abc") << QString("A") << 1 << false << -1;
    QTest::newRow( "data8" ) << QString("abc") << QString("b") << 0 << true << 1;
    QTest::newRow( "data9" ) << QString("abc") << QString("b") << 0 << false << 1;
    QTest::newRow( "data10" ) << QString("abc") << QString("B") << 0 << true << -1;
    QTest::newRow( "data11" ) << QString("abc") << QString("B") << 0 << false << 1;
    QTest::newRow( "data12" ) << QString("abc") << QString("b") << 1 << true << 1;
    QTest::newRow( "data13" ) << QString("abc") << QString("b") << 1 << false << 1;
    QTest::newRow( "data14" ) << QString("abc") << QString("B") << 1 << true << -1;
    QTest::newRow( "data15" ) << QString("abc") << QString("B") << 1 << false << 1;
    QTest::newRow( "data16" ) << QString("abc") << QString("b") << 2 << true << -1;
    QTest::newRow( "data17" ) << QString("abc") << QString("b") << 2 << false << -1;

    QTest::newRow( "data20" ) << QString("ABC") << QString("A") << 0 << true << 0;
    QTest::newRow( "data21" ) << QString("ABC") << QString("A") << 0 << false << 0;
    QTest::newRow( "data22" ) << QString("ABC") << QString("a") << 0 << true << -1;
    QTest::newRow( "data23" ) << QString("ABC") << QString("a") << 0 << false << 0;
    QTest::newRow( "data24" ) << QString("ABC") << QString("A") << 1 << true << -1;
    QTest::newRow( "data25" ) << QString("ABC") << QString("A") << 1 << false << -1;
    QTest::newRow( "data26" ) << QString("ABC") << QString("a") << 1 << true << -1;
    QTest::newRow( "data27" ) << QString("ABC") << QString("a") << 1 << false << -1;
    QTest::newRow( "data28" ) << QString("ABC") << QString("B") << 0 << true << 1;
    QTest::newRow( "data29" ) << QString("ABC") << QString("B") << 0 << false << 1;
    QTest::newRow( "data30" ) << QString("ABC") << QString("b") << 0 << true << -1;
    QTest::newRow( "data31" ) << QString("ABC") << QString("b") << 0 << false << 1;
    QTest::newRow( "data32" ) << QString("ABC") << QString("B") << 1 << true << 1;
    QTest::newRow( "data33" ) << QString("ABC") << QString("B") << 1 << false << 1;
    QTest::newRow( "data34" ) << QString("ABC") << QString("b") << 1 << true << -1;
    QTest::newRow( "data35" ) << QString("ABC") << QString("b") << 1 << false << 1;
    QTest::newRow( "data36" ) << QString("ABC") << QString("B") << 2 << true << -1;
    QTest::newRow( "data37" ) << QString("ABC") << QString("B") << 2 << false << -1;

    QTest::newRow( "data40" ) << QString("aBc") << QString("bc") << 0 << true << -1;
    QTest::newRow( "data41" ) << QString("aBc") << QString("Bc") << 0 << true << 1;
    QTest::newRow( "data42" ) << QString("aBc") << QString("bC") << 0 << true << -1;
    QTest::newRow( "data43" ) << QString("aBc") << QString("BC") << 0 << true << -1;
    QTest::newRow( "data44" ) << QString("aBc") << QString("bc") << 0 << false << 1;
    QTest::newRow( "data45" ) << QString("aBc") << QString("Bc") << 0 << false << 1;
    QTest::newRow( "data46" ) << QString("aBc") << QString("bC") << 0 << false << 1;
    QTest::newRow( "data47" ) << QString("aBc") << QString("BC") << 0 << false << 1;
    QTest::newRow( "data48" ) << QString("AbC") << QString("bc") << 0 << true << -1;
    QTest::newRow( "data49" ) << QString("AbC") << QString("Bc") << 0 << true << -1;
    QTest::newRow( "data50" ) << QString("AbC") << QString("bC") << 0 << true << 1;
    QTest::newRow( "data51" ) << QString("AbC") << QString("BC") << 0 << true << -1;
    QTest::newRow( "data52" ) << QString("AbC") << QString("bc") << 0 << false << 1;
    QTest::newRow( "data53" ) << QString("AbC") << QString("Bc") << 0 << false << 1;

    QTest::newRow( "data54" ) << QString("AbC") << QString("bC") << 0 << false << 1;
    QTest::newRow( "data55" ) << QString("AbC") << QString("BC") << 0 << false << 1;
    QTest::newRow( "data56" ) << QString("AbC") << QString("BC") << 1 << false << 1;
    QTest::newRow( "data57" ) << QString("AbC") << QString("BC") << 2 << false << -1;

    QTest::newRow( "null-in-null") << QString() << QString() << 0 << false << 0;
    QTest::newRow( "empty-in-null") << QString() << QString("") << 0 << false << 0;
    QTest::newRow( "null-in-empty") << QString("") << QString() << 0 << false << 0;
    QTest::newRow( "empty-in-empty") << QString("") << QString("") << 0 << false << 0;
    QTest::newRow( "data-in-null") << QString() << QString("a") << 0 << false << -1;
    QTest::newRow( "data-in-empty") << QString("") << QString("a") << 0 << false << -1;


    QString s1 = "abc";
    s1 += QChar(0xb5);
    QString s2;
    s2 += QChar(0x3bc);
    QTest::newRow( "data58" ) << s1 << s2 << 0 << false << 3;
    s2.prepend(QLatin1Char('C'));
    QTest::newRow( "data59" ) << s1 << s2 << 0 << false << 2;

    QString veryBigHaystack(500, 'a');
    veryBigHaystack += 'B';
    QTest::newRow("BoyerMooreStressTest") << veryBigHaystack << veryBigHaystack << 0 << true << 0;
    QTest::newRow("BoyerMooreStressTest2") << QString(veryBigHaystack + 'c') << veryBigHaystack << 0 << true << 0;
    QTest::newRow("BoyerMooreStressTest3") << QString('c' + veryBigHaystack) << veryBigHaystack << 0 << true << 1;
    QTest::newRow("BoyerMooreStressTest4") << veryBigHaystack << QString(veryBigHaystack + 'c') << 0 << true << -1;
    QTest::newRow("BoyerMooreStressTest5") << veryBigHaystack << QString('c' + veryBigHaystack) << 0 << true << -1;
    QTest::newRow("BoyerMooreStressTest6") << QString('d' + veryBigHaystack) << QString('c' + veryBigHaystack) << 0 << true << -1;
    QTest::newRow("BoyerMooreStressTest7") << QString(veryBigHaystack + 'c') << QString('c' + veryBigHaystack) << 0 << true << -1;

    QTest::newRow("BoyerMooreInsensitiveStressTest") << veryBigHaystack << veryBigHaystack << 0 << false << 0;
}

void tst_QString::indexOf()
{
    QFETCH( QString, haystack );
    QFETCH( QString, needle );
    QFETCH( int, startpos );
    QFETCH( bool, bcs );
    QFETCH( int, resultpos );
    CREATE_VIEW(needle);

    Qt::CaseSensitivity cs = bcs ? Qt::CaseSensitive : Qt::CaseInsensitive;

    bool needleIsLatin = (QString::fromLatin1(needle.toLatin1()) == needle);

    QCOMPARE( haystack.indexOf(needle, startpos, cs), resultpos );
    QCOMPARE( haystack.indexOf(view, startpos, cs), resultpos );
    if (needleIsLatin) {
        QCOMPARE( haystack.indexOf(needle.toLatin1(), startpos, cs), resultpos );
        QCOMPARE( haystack.indexOf(needle.toLatin1().data(), startpos, cs), resultpos );
    }

//#if QT_CONFIG(regularexpression)
//    {
//        QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
//        if (!bcs)
//            options |= QRegularExpression::CaseInsensitiveOption;
//
//        QRegularExpression re(QRegularExpression::escape(needle), options);
//        QCOMPARE( haystack.indexOf(re, startpos), resultpos );
//        QCOMPARE(haystack.indexOf(re, startpos, nullptr), resultpos);
//
//        QRegularExpressionMatch match;
//        QVERIFY(!match.hasMatch());
//        QCOMPARE(haystack.indexOf(re, startpos, &match), resultpos);
//        QCOMPARE(match.hasMatch(), resultpos != -1);
//        if (resultpos > -1 && needleIsLatin) {
//            if (bcs)
//                QVERIFY(match.captured() == needle);
//            else
//                QVERIFY(match.captured().toLower() == needle.toLower());
//        }qq
//    }
//#endif

    if (cs == Qt::CaseSensitive) {
        QCOMPARE( haystack.indexOf(needle, startpos), resultpos );
        QCOMPARE( haystack.indexOf(view, startpos), resultpos );
        if (needleIsLatin) {
            QCOMPARE( haystack.indexOf(needle.toLatin1(), startpos), resultpos );
            QCOMPARE( haystack.indexOf(needle.toLatin1().data(), startpos), resultpos );
        }
        if (startpos == 0) {
            QCOMPARE( haystack.indexOf(needle), resultpos );
            QCOMPARE( haystack.indexOf(view), resultpos );
            if (needleIsLatin) {
                QCOMPARE( haystack.indexOf(needle.toLatin1()), resultpos );
                QCOMPARE( haystack.indexOf(needle.toLatin1().data()), resultpos );
            }
        }
    }
    if (needle.size() == 1) {
        QCOMPARE(haystack.indexOf(needle.at(0), startpos, cs), resultpos);
        QCOMPARE(haystack.indexOf(view.at(0), startpos, cs), resultpos);
    }
}

void tst_QString::indexOf2_data()
{
    QTest::addColumn<QString>("haystack" );
    QTest::addColumn<QString>("needle" );
    QTest::addColumn<int>("resultpos" );

    QTest::newRow( "data0" ) << QString() << QString() << 0;
    QTest::newRow( "data1" ) << QString() << QString("") << 0;
    QTest::newRow( "data2" ) << QString("") << QString() << 0;
    QTest::newRow( "data3" ) << QString("") << QString("") << 0;
    QTest::newRow( "data4" ) << QString() << QString("a") << -1;
    QTest::newRow( "data5" ) << QString() << QString("abcdefg") << -1;
    QTest::newRow( "data6" ) << QString("") << QString("a") << -1;
    QTest::newRow( "data7" ) << QString("") << QString("abcdefg") << -1;

    QTest::newRow( "data8" ) << QString("a") << QString() << 0;
    QTest::newRow( "data9" ) << QString("a") << QString("") << 0;
    QTest::newRow( "data10" ) << QString("a") << QString("a") << 0;
    QTest::newRow( "data11" ) << QString("a") << QString("b") << -1;
    QTest::newRow( "data12" ) << QString("a") << QString("abcdefg") << -1;
    QTest::newRow( "data13" ) << QString("ab") << QString() << 0;
    QTest::newRow( "data14" ) << QString("ab") << QString("") << 0;
    QTest::newRow( "data15" ) << QString("ab") << QString("a") << 0;
    QTest::newRow( "data16" ) << QString("ab") << QString("b") << 1;
    QTest::newRow( "data17" ) << QString("ab") << QString("ab") << 0;
    QTest::newRow( "data18" ) << QString("ab") << QString("bc") << -1;
    QTest::newRow( "data19" ) << QString("ab") << QString("abcdefg") << -1;

    QTest::newRow( "data30" ) << QString("abc") << QString("a") << 0;
    QTest::newRow( "data31" ) << QString("abc") << QString("b") << 1;
    QTest::newRow( "data32" ) << QString("abc") << QString("c") << 2;
    QTest::newRow( "data33" ) << QString("abc") << QString("d") << -1;
    QTest::newRow( "data34" ) << QString("abc") << QString("ab") << 0;
    QTest::newRow( "data35" ) << QString("abc") << QString("bc") << 1;
    QTest::newRow( "data36" ) << QString("abc") << QString("cd") << -1;
    QTest::newRow( "data37" ) << QString("abc") << QString("ac") << -1;

    // sizeof(whale) > 32
    QString whale = "a5zby6cx7dw8evf9ug0th1si2rj3qkp4lomn";
    QString minnow = "zby";
    QTest::newRow( "data40" ) << whale << minnow << 2;
    QTest::newRow( "data41" ) << QString(whale + whale) << minnow << 2;
    QTest::newRow( "data42" ) << QString(minnow + whale) << minnow << 0;
    QTest::newRow( "data43" ) << whale << whale << 0;
    QTest::newRow( "data44" ) << QString(whale + whale) << whale << 0;
    QTest::newRow( "data45" ) << whale << QString(whale + whale) << -1;
    QTest::newRow( "data46" ) << QString(whale + whale) << QString(whale + whale) << 0;
    QTest::newRow( "data47" ) << QString(whale + whale) << QString(whale + minnow) << -1;
    QTest::newRow( "data48" ) << QString(minnow + whale) << whale << (int)minnow.length();
}

void tst_QString::indexOf2()
{
    QFETCH( QString, haystack );
    QFETCH( QString, needle );
    QFETCH( int, resultpos );
    CREATE_VIEW(needle);

    QByteArray chaystack = haystack.toLatin1();
    QByteArray cneedle = needle.toLatin1();
    int got;

    QCOMPARE( haystack.indexOf(needle, 0, Qt::CaseSensitive), resultpos );
    QCOMPARE( haystack.indexOf(view, 0, Qt::CaseSensitive), resultpos );
    QCOMPARE( QStringMatcher(needle, Qt::CaseSensitive).indexIn(haystack, 0), resultpos );
    QCOMPARE( haystack.indexOf(needle, 0, Qt::CaseInsensitive), resultpos );
    QCOMPARE( haystack.indexOf(view, 0, Qt::CaseInsensitive), resultpos );
    QCOMPARE( QStringMatcher(needle, Qt::CaseInsensitive).indexIn(haystack, 0), resultpos );
    if ( needle.length() > 0 ) {
        got = haystack.lastIndexOf( needle, -1, Qt::CaseSensitive );
        QVERIFY( got == resultpos || (resultpos >= 0 && got >= resultpos) );
        got = haystack.lastIndexOf( needle, -1, Qt::CaseInsensitive );
        QVERIFY( got == resultpos || (resultpos >= 0 && got >= resultpos) );
    }

    QCOMPARE( chaystack.indexOf(cneedle, 0), resultpos );
    QCOMPARE( QByteArrayMatcher(cneedle).indexIn(chaystack, 0), resultpos );
    if ( cneedle.length() > 0 ) {
        got = chaystack.lastIndexOf(cneedle, -1);
        QVERIFY( got == resultpos || (resultpos >= 0 && got >= resultpos) );
    }
}

static inline const void *ptrValue(quintptr v)
{
    return reinterpret_cast<const void *>(v);
}

void tst_QString::asprintf() {
//    QString a;
//    QCOMPARE(QString::asprintf("COMPARE"), QLatin1String("COMPARE"));
//    QCOMPARE(QString::asprintf("%%%d", 1), QLatin1String("%1"));
//    QCOMPARE(QString::asprintf("X%dY",2), QLatin1String("X2Y"));
//    QCOMPARE(QString::asprintf("X%9iY", 50000 ), QLatin1String("X    50000Y"));
//    QCOMPARE(QString::asprintf("X%-9sY","hello"), QLatin1String("Xhello    Y"));
//    QCOMPARE(QString::asprintf("X%-9iY", 50000 ), QLatin1String("X50000    Y"));
//    QCOMPARE(QString::asprintf("%lf", 1.23), QLatin1String("1.230000"));
//    QCOMPARE(QString::asprintf("%lf", 1.23456789), QLatin1String("1.234568"));
//    QCOMPARE(QString::asprintf("%p", ptrValue(0xbfffd350)), QLatin1String("0xbfffd350"));
//    QCOMPARE(QString::asprintf("%p", ptrValue(0)), QLatin1String("0x0"));
//    QCOMPARE(QString::asprintf("%td", ptrdiff_t(6)), QString::fromLatin1("6"));
//    QCOMPARE(QString::asprintf("%td", ptrdiff_t(-6)), QString::fromLatin1("-6"));
//    QCOMPARE(QString::asprintf("%zu", size_t(6)), QString::fromLatin1("6"));
//    QCOMPARE(QString::asprintf("%zu", size_t(1) << 31), QString::fromLatin1("2147483648"));
//
//    // cross z and t
//    using ssize_t = std::make_signed<size_t>::type;         // should be ptrdiff_t
//    using uptrdiff_t = std::make_unsigned<ptrdiff_t>::type; // should be size_t
//    QCOMPARE(QString::asprintf("%tu", uptrdiff_t(6)), QString::fromLatin1("6"));
//    QCOMPARE(QString::asprintf("%tu", uptrdiff_t(1) << 31), QString::fromLatin1("2147483648"));
//    QCOMPARE(QString::asprintf("%zd", ssize_t(-6)), QString::fromLatin1("-6"));
//
//    if (sizeof(qsizetype) > sizeof(int)) {
//        // 64-bit test
//        QCOMPARE(QString::asprintf("%zu", SIZE_MAX), QString::fromLatin1("18446744073709551615"));
//        QCOMPARE(QString::asprintf("%td", PTRDIFF_MAX), QString::fromLatin1("9223372036854775807"));
//        QCOMPARE(QString::asprintf("%td", PTRDIFF_MIN), QString::fromLatin1("-9223372036854775808"));
//
//        // sign extension is easy, make sure we can get something middle-ground
//        // (24 + 8 = 32; addition used to avoid warning about shifting more
//        // than size type on 32-bit systems)
//        size_t ubig = size_t(1) << (24 + sizeof(size_t));
//        ptrdiff_t sbig = ptrdiff_t(1) << (24 + sizeof(ptrdiff_t));
//        QCOMPARE(QString::asprintf("%zu", ubig), QString::fromLatin1("4294967296"));
//        QCOMPARE(QString::asprintf("%td", sbig), QString::fromLatin1("4294967296"));
//        QCOMPARE(QString::asprintf("%td", -sbig), QString::fromLatin1("-4294967296"));
//    }
//
//    int i = 6;
//    long l = -2;
//    float f = 4.023f;
//    QCOMPARE(QString::asprintf("%d %ld %f", i, l, f), QLatin1String("6 -2 4.023000"));
//
//    double d = -514.25683;
//    QCOMPARE(QString::asprintf("%f", d), QLatin1String("-514.256830"));
//
//    {
//        /* This code crashed. I don't know how to reduce it further. In other words,
//         * both %zu and %s needs to be present. */
//        size_t s = 6;
//        QCOMPARE(QString::asprintf("%zu%s", s, "foo"), QString::fromLatin1("6foo"));
//        QCOMPARE(QString::asprintf("%zu %s\n", s, "foo"), QString::fromLatin1("6 foo\n"));
//    }
}

void tst_QString::asprintfS() {
//    QCOMPARE(QString::asprintf("%.3s", "Hello" ), QLatin1String("Hel"));
//    QCOMPARE(QString::asprintf("%10.3s", "Hello" ), QLatin1String("       Hel"));
//    QCOMPARE(QString::asprintf("%.10s", "Hello" ), QLatin1String("Hello"));
//    QCOMPARE(QString::asprintf("%10.10s", "Hello" ), QLatin1String("     Hello"));
//    QCOMPARE(QString::asprintf("%-10.10s", "Hello" ), QLatin1String("Hello     "));
//    QCOMPARE(QString::asprintf("%-10.3s", "Hello" ), QLatin1String("Hel       "));
//    QCOMPARE(QString::asprintf("%-5.5s", "Hello" ), QLatin1String("Hello"));
//
//    // Check utf8 conversion for %s
//    QCOMPARE(QString::asprintf("%s", "\303\266\303\244\303\274\303\226\303\204\303\234\303\270\303\246\303\245\303\230\303\206\303\205"), QString::fromLatin1("\366\344\374\326\304\334\370\346\345\330\306\305"));
//
//    int n1;
//    QCOMPARE(QString::asprintf("%s%n%s", "hello", &n1, "goodbye"), QString("hellogoodbye"));
//    QCOMPARE(n1, 5);
//    qlonglong n2;
//    QCOMPARE(QString::asprintf("%s%s%lln%s", "foo", "bar", &n2, "whiz"), QString("foobarwhiz"));
//    QCOMPARE((int)n2, 6);
//
//    { // %ls
//        QCOMPARE(QString::asprintf("%.3ls",     qUtf16Printable("Hello")), QLatin1String("Hel"));
//        QCOMPARE(QString::asprintf("%10.3ls",   qUtf16Printable("Hello")), QLatin1String("       Hel"));
//        QCOMPARE(QString::asprintf("%.10ls",    qUtf16Printable("Hello")), QLatin1String("Hello"));
//        QCOMPARE(QString::asprintf("%10.10ls",  qUtf16Printable("Hello")), QLatin1String("     Hello"));
//        QCOMPARE(QString::asprintf("%-10.10ls", qUtf16Printable("Hello")), QLatin1String("Hello     "));
//        QCOMPARE(QString::asprintf("%-10.3ls",  qUtf16Printable("Hello")), QLatin1String("Hel       "));
//        QCOMPARE(QString::asprintf("%-5.5ls",   qUtf16Printable("Hello")), QLatin1String("Hello"));
//
//        // Check utf16 is preserved for %ls
//        QCOMPARE(QString::asprintf("%ls",
//                                   qUtf16Printable("\303\266\303\244\303\274\303\226\303\204\303\234\303\270\303\246\303\245\303\230\303\206\303\205")),
//                 QLatin1String("\366\344\374\326\304\334\370\346\345\330\306\305"));
//
//        int n;
//        QCOMPARE(QString::asprintf("%ls%n%s", qUtf16Printable("hello"), &n, "goodbye"), QLatin1String("hellogoodbye"));
//        QCOMPARE(n, 5);
//    }
}

QTEST_APPLESS_MAIN(tst_QString)