//
// Created by Yujie Zhao on 2023/4/13.
//

#include <QObject>
#include <QTest>
#include <QList>
#include <QByteArray>

static const QByteArray::DataPointer staticStandard = {
        nullptr,
        const_cast<char *>("data"),
        4
};
static const QByteArray::DataPointer staticNotNullTerminated = {
        nullptr,
        const_cast<char *>("dataBAD"),
        4
};


class tst_QByteArray : public QObject {
public:
    using QObject::QObject;

    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(swap);
        REGISTER_OBJECT_INVOKE_METHOD(qChecksum_data);
        REGISTER_OBJECT_INVOKE_METHOD(qChecksum);
//        REGISTER_OBJECT_INVOKE_METHOD(qCompress_data);
//        REGISTER_OBJECT_INVOKE_METHOD(qCompress);
//        REGISTER_OBJECT_INVOKE_METHOD(qUncompressCorruptedData_data);
//        REGISTER_OBJECT_INVOKE_METHOD(qUncompressCorruptedData);
//        REGISTER_OBJECT_INVOKE_METHOD(qCompressionZeroTermination);
        REGISTER_OBJECT_INVOKE_METHOD(constByteArray);
        REGISTER_OBJECT_INVOKE_METHOD(leftJustified);
        REGISTER_OBJECT_INVOKE_METHOD(rightJustified);
        REGISTER_OBJECT_INVOKE_METHOD(setNum);
        REGISTER_OBJECT_INVOKE_METHOD(iterators);
        REGISTER_OBJECT_INVOKE_METHOD(reverseIterators);
        REGISTER_OBJECT_INVOKE_METHOD(split_data);
        REGISTER_OBJECT_INVOKE_METHOD(split);
        REGISTER_OBJECT_INVOKE_METHOD(base64_data);
        REGISTER_OBJECT_INVOKE_METHOD(base64);
        REGISTER_OBJECT_INVOKE_METHOD(fromBase64_data);
        REGISTER_OBJECT_INVOKE_METHOD(fromBase64);
        REGISTER_OBJECT_INVOKE_METHOD(qvsnprintf);
        REGISTER_OBJECT_INVOKE_METHOD(qstrlen);
        REGISTER_OBJECT_INVOKE_METHOD(qstrnlen);
        REGISTER_OBJECT_INVOKE_METHOD(qstrcpy);
        REGISTER_OBJECT_INVOKE_METHOD(qstrncpy);
        REGISTER_OBJECT_INVOKE_METHOD(chop_data);
        REGISTER_OBJECT_INVOKE_METHOD(chop);
        REGISTER_OBJECT_INVOKE_METHOD(prepend);
        REGISTER_OBJECT_INVOKE_METHOD(prependExtended_data);
        REGISTER_OBJECT_INVOKE_METHOD(prependExtended);
        REGISTER_OBJECT_INVOKE_METHOD(append);
        REGISTER_OBJECT_INVOKE_METHOD(appendExtended_data);
        REGISTER_OBJECT_INVOKE_METHOD(appendExtended);
        REGISTER_OBJECT_INVOKE_METHOD(insert);
        REGISTER_OBJECT_INVOKE_METHOD(insertExtended_data);
        REGISTER_OBJECT_INVOKE_METHOD(insertExtended);
    }

    void swap() const;
    void qChecksum_data() const;
    void qChecksum() const;
//    void qCompress_data() const {}
//    void qCompress() const {}
//    void qUncompressCorruptedData_data() const;
//    void qUncompressCorruptedData() const;
//    void qCompressionZeroTermination() const;
    void constByteArray() const;
    void leftJustified();
    void rightJustified();
    void setNum();
    void iterators();
    void reverseIterators();
    void split_data();
    void split();
    void base64_data();
    void base64();
    void fromBase64_data();
    void fromBase64();
    void qvsnprintf();
    void qstrlen();
    void qstrnlen();
    void qstrcpy();
    void qstrncpy();
    void chop_data();
    void chop();
    void prepend();
    void prependExtended_data();
    void prependExtended();
    void append();
    void appendExtended_data();
    void appendExtended();
    void insert();
    void insertExtended_data();
    void insertExtended();
};

void tst_QByteArray::swap() const
{
    QByteArray b1 = "b1";
    QByteArray b2 = "b2";
    b1.swap(b2);
    QCOMPARE(b1, QByteArray("b2"));
    QCOMPARE(b2, QByteArray("b1"));
}

void tst_QByteArray::qChecksum_data() const
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<uint>("len");
    QTest::addColumn<Qt::ChecksumType>("standard");
    QTest::addColumn<uint>("checksum");

    // Examples from ISO 14443-3
    QTest::newRow("1") << QByteArray("\x00\x00", 2)         << 2U << Qt::ChecksumItuV41  << 0x1EA0U;
    QTest::newRow("2") << QByteArray("\x12\x34", 2)         << 2U << Qt::ChecksumItuV41  << 0xCF26U;
    QTest::newRow("3") << QByteArray("\x00\x00\x00", 3)     << 3U << Qt::ChecksumIso3309 << 0xC6CCU;
    QTest::newRow("4") << QByteArray("\x0F\xAA\xFF", 3)     << 3U << Qt::ChecksumIso3309 << 0xD1FCU;
    QTest::newRow("5") << QByteArray("\x0A\x12\x34\x56", 4) << 4U << Qt::ChecksumIso3309 << 0xF62CU;
}

void tst_QByteArray::qChecksum() const
{
    QFETCH(QByteArray, data);
    QFETCH(uint, len);
    QFETCH(Qt::ChecksumType, standard);
    QFETCH(uint, checksum);

    QCOMPARE(data.length(), int(len));
    if (standard == Qt::ChecksumIso3309) {
        QCOMPARE(::qChecksum(QByteArrayView(data.constData(), len)), static_cast<quint16>(checksum));
    }
    QCOMPARE(::qChecksum(QByteArrayView(data.constData(), len), standard), static_cast<quint16>(checksum));
}

void tst_QByteArray::constByteArray() const
{
    const char *ptr = "abc";
    QByteArray cba = QByteArray::fromRawData(ptr, 3);
    QVERIFY(cba.constData() == ptr);
    cba.squeeze();
    QVERIFY(cba.constData() == ptr);
    cba.detach();
    QVERIFY(cba.size() == 3);
    QVERIFY(cba.capacity() == 3);
    QVERIFY(cba.constData() != ptr);
    QVERIFY(cba.constData()[0] == 'a');
    QVERIFY(cba.constData()[1] == 'b');
    QVERIFY(cba.constData()[2] == 'c');
    QVERIFY(cba.constData()[3] == '\0');
}

void tst_QByteArray::leftJustified()
{
    QByteArray a;

    QCOMPARE(a.leftJustified(3, '-'), QByteArray("---"));
    QCOMPARE(a.leftJustified(2, ' '), QByteArray("  "));
    QVERIFY(!a.isDetached());

    a = "ABC";
    QCOMPARE(a.leftJustified(5,'-'), QByteArray("ABC--"));
    QCOMPARE(a.leftJustified(4,'-'), QByteArray("ABC-"));
    QCOMPARE(a.leftJustified(4), QByteArray("ABC "));
    QCOMPARE(a.leftJustified(3), QByteArray("ABC"));
    QCOMPARE(a.leftJustified(2), QByteArray("ABC"));
    QCOMPARE(a.leftJustified(1), QByteArray("ABC"));
    QCOMPARE(a.leftJustified(0), QByteArray("ABC"));

    QCOMPARE(a.leftJustified(4,' ',true), QByteArray("ABC "));
    QCOMPARE(a.leftJustified(3,' ',true), QByteArray("ABC"));
    QCOMPARE(a.leftJustified(2,' ',true), QByteArray("AB"));
    QCOMPARE(a.leftJustified(1,' ',true), QByteArray("A"));
    QCOMPARE(a.leftJustified(0,' ',true), QByteArray(""));
}

void tst_QByteArray::rightJustified()
{
    QByteArray a;

    QCOMPARE(a.rightJustified(3, '-'), QByteArray("---"));
    QCOMPARE(a.rightJustified(2, ' '), QByteArray("  "));
    QVERIFY(!a.isDetached());

    a="ABC";
    QCOMPARE(a.rightJustified(5,'-'),QByteArray("--ABC"));
    QCOMPARE(a.rightJustified(4,'-'),QByteArray("-ABC"));
    QCOMPARE(a.rightJustified(4),QByteArray(" ABC"));
    QCOMPARE(a.rightJustified(3),QByteArray("ABC"));
    QCOMPARE(a.rightJustified(2),QByteArray("ABC"));
    QCOMPARE(a.rightJustified(1),QByteArray("ABC"));
    QCOMPARE(a.rightJustified(0),QByteArray("ABC"));

    QCOMPARE(a.rightJustified(4,'-',true),QByteArray("-ABC"));
    QCOMPARE(a.rightJustified(4,' ',true),QByteArray(" ABC"));
    QCOMPARE(a.rightJustified(3,' ',true),QByteArray("ABC"));
    QCOMPARE(a.rightJustified(2,' ',true),QByteArray("AB"));
    QCOMPARE(a.rightJustified(1,' ',true),QByteArray("A"));
    QCOMPARE(a.rightJustified(0,' ',true),QByteArray(""));
    QCOMPARE(a,QByteArray("ABC"));
}

void tst_QByteArray::setNum()
{
    QByteArray a;
    int num = std::numeric_limits<int>::min();
    a.setNum(num);
    QCOMPARE(a.setNum(-1), QByteArray("-1"));
    QCOMPARE(a.setNum(0), QByteArray("0"));
    QCOMPARE(a.setNum(0, 2), QByteArray("0"));
    QCOMPARE(a.setNum(0, 36), QByteArray("0"));
    QCOMPARE(a.setNum(1), QByteArray("1"));
    QCOMPARE(a.setNum(35, 36), QByteArray("z"));
    QCOMPARE(a.setNum(37, 2), QByteArray("100101"));
    QCOMPARE(a.setNum(37, 36), QByteArray("11"));

    QCOMPARE(a.setNum(short(-1), 16), QByteArray("-1"));
    QCOMPARE(a.setNum(int(-1), 16), QByteArray("-1"));
    QCOMPARE(a.setNum(qlonglong(-1), 16), QByteArray("-1"));

    QCOMPARE(a.setNum(short(-1), 10), QByteArray("-1"));
    QCOMPARE(a.setNum(int(-1), 10), QByteArray("-1"));
    QCOMPARE(a.setNum(qlonglong(-1), 10), QByteArray("-1"));

    QCOMPARE(a.setNum(-123), QByteArray("-123"));
    QCOMPARE(a.setNum(0x123, 16), QByteArray("123"));
    QCOMPARE(a.setNum(short(123)), QByteArray("123"));

//    QCOMPARE(a.setNum(1.23), QByteArray("1.23"));
//    QCOMPARE(a.setNum(1.234567), QByteArray("1.23457"));
//
//    // Note that there are no 'long' overloads, so not all of the
//    // QString::setNum() tests can be re-used.
    QCOMPARE(a.setNum(Q_INT64_C(123)), QByteArray("123"));
//    // 2^40 == 1099511627776
    QCOMPARE(a.setNum(Q_INT64_C(-1099511627776)), QByteArray("-1099511627776"));
    QCOMPARE(a.setNum(Q_UINT64_C(1099511627776)), QByteArray("1099511627776"));
    QCOMPARE(a.setNum(Q_INT64_C(9223372036854775807)), // LLONG_MAX
             QByteArray("9223372036854775807"));
    QCOMPARE(a.setNum(-Q_INT64_C(9223372036854775807) - Q_INT64_C(1)),
             QByteArray("-9223372036854775808"));
    QCOMPARE(a.setNum(Q_UINT64_C(18446744073709551615)), // ULLONG_MAX
             QByteArray("18446744073709551615"));
//    QCOMPARE(a.setNum(0.000000000931322574615478515625), QByteArray("9.31323e-10"));
}

void tst_QByteArray::iterators()
{
    QByteArray emptyArr;
    QCOMPARE(emptyArr.constBegin(), emptyArr.constEnd());
    QCOMPARE(emptyArr.cbegin(), emptyArr.cend());
    QVERIFY(!emptyArr.isDetached());  //此时emptyArr的d为空，d为空是isShared为true
    QCOMPARE(emptyArr.begin(), emptyArr.end());

    QByteArray a("0123456789");

    auto it = a.begin();
    auto constIt = a.cbegin();
    qsizetype idx = 0;

    QCOMPARE(*it, a[idx]);
    QCOMPARE(*constIt, a[idx]);

    it++;
    constIt++;
    idx++;
    QCOMPARE(*it, a[idx]);
    QCOMPARE(*constIt, a[idx]);

    it += 5;
    constIt += 5;
    idx += 5;
    QCOMPARE(*it, a[idx]);
    QCOMPARE(*constIt, a[idx]);

    it -= 3;
    constIt -= 3;
    idx -= 3;
    QCOMPARE(*it, a[idx]);
    QCOMPARE(*constIt, a[idx]);

    it--;
    constIt--;
    idx--;
    QCOMPARE(*it, a[idx]);
    QCOMPARE(*constIt, a[idx]);
}

void tst_QByteArray::reverseIterators()
{
    QByteArray emptyArr;
    QCOMPARE(emptyArr.crbegin(), emptyArr.crend());
    QVERIFY(!emptyArr.isDetached());
    QCOMPARE(emptyArr.rbegin(), emptyArr.rend());

    QByteArray s = "1234";
    QByteArray sr = s;
    std::reverse(sr.begin(), sr.end());
    const QByteArray &csr = sr;
    QVERIFY(std::equal(s.begin(), s.end(), sr.rbegin()));
    QVERIFY(std::equal(s.begin(), s.end(), sr.crbegin()));
    QVERIFY(std::equal(s.begin(), s.end(), csr.rbegin()));
    QVERIFY(std::equal(sr.rbegin(), sr.rend(), s.begin()));
    QVERIFY(std::equal(sr.crbegin(), sr.crend(), s.begin()));
    QVERIFY(std::equal(csr.rbegin(), csr.rend(), s.begin()));
}

void tst_QByteArray::split_data()
{
    QTest::addColumn<QByteArray>("sample");
    QTest::addColumn<int>("size");

    QTest::newRow("1") << QByteArray("-rw-r--r--  1 0  0  519240 Jul  9  2002 bigfile") << 14;
    QTest::newRow("2") << QByteArray("abcde") << 1;
    QTest::newRow("one empty") << QByteArray("") << 1;
    QTest::newRow("two empty") << QByteArray(" ") << 2;
    QTest::newRow("three empty") << QByteArray("  ") << 3;
    QTest::newRow("null") << QByteArray() << 1;
}

void tst_QByteArray::split()
{
    QFETCH(QByteArray, sample);
    QFETCH(int, size);

    QList<QByteArray> list = sample.split(' ');
    QCOMPARE(list.count(), size);
}

void tst_QByteArray::base64_data()
{
    QTest::addColumn<QByteArray>("rawdata");
    QTest::addColumn<QByteArray>("base64");

    QTest::newRow("null") << QByteArray() << QByteArray();
    QTest::newRow("1") << QByteArray("") << QByteArray("");
    QTest::newRow("2") << QByteArray("1") << QByteArray("MQ==");
    QTest::newRow("3") << QByteArray("12") << QByteArray("MTI=");
    QTest::newRow("4") << QByteArray("123") << QByteArray("MTIz");
    QTest::newRow("5") << QByteArray("1234") << QByteArray("MTIzNA==");
    QTest::newRow("6") << QByteArray("\n") << QByteArray("Cg==");
    QTest::newRow("7") << QByteArray("a\n") << QByteArray("YQo=");
    QTest::newRow("8") << QByteArray("ab\n") << QByteArray("YWIK");
    QTest::newRow("9") << QByteArray("abc\n") << QByteArray("YWJjCg==");
    QTest::newRow("a") << QByteArray("abcd\n") << QByteArray("YWJjZAo=");
    QTest::newRow("b") << QByteArray("abcde\n") << QByteArray("YWJjZGUK");
    QTest::newRow("c") << QByteArray("abcdef\n") << QByteArray("YWJjZGVmCg==");
    QTest::newRow("d") << QByteArray("abcdefg\n") << QByteArray("YWJjZGVmZwo=");
    QTest::newRow("e") << QByteArray("abcdefgh\n") << QByteArray("YWJjZGVmZ2gK");

    QByteArray ba;
    ba.resize(256);
    for (int i = 0; i < 256; ++i)
        ba[i] = i;
    QTest::newRow("f") << ba << QByteArray("AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+/w==");

    QTest::newRow("g") << QByteArray("foo\0bar", 7) << QByteArray("Zm9vAGJhcg==");
    QTest::newRow("h") << QByteArray("f\xd1oo\x9ctar") << QByteArray("ZtFvb5x0YXI=");
    QTest::newRow("i") << QByteArray("\"\0\0\0\0\0\0\"", 8) << QByteArray("IgAAAAAAACI=");
}

void tst_QByteArray::base64()
{
    //zhaoyujie TODO
//    QFETCH(QByteArray, rawdata);
//    QFETCH(QByteArray, base64);
//    QByteArray::FromBase64Result result;
//
//    result = QByteArray::fromBase64Encoding(base64, QByteArray::Base64Encoding | QByteArray::AbortOnBase64DecodingErrors);
//    QVERIFY(result);
//    QCOMPARE(result.decoded, rawdata);
//
//    QByteArray arr = base64;
//    result = QByteArray::fromBase64Encoding(std::move(arr), QByteArray::Base64Encoding | QByteArray::AbortOnBase64DecodingErrors);
//    QVERIFY(result);
//    QCOMPARE(result.decoded, rawdata);
//
//    QByteArray arr64 = rawdata.toBase64();
//    QCOMPARE(arr64, base64);
//
//    arr64 = rawdata.toBase64(QByteArray::Base64Encoding);
//    QCOMPARE(arr64, base64);
//
//    QByteArray base64noequals = base64;
//    base64noequals.replace('=', "");
//    arr64 = rawdata.toBase64(QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);
//    QCOMPARE(arr64, base64noequals);
//
//    QByteArray base64url = base64;
//    base64url.replace('/', '_').replace('+', '-');
//    arr64 = rawdata.toBase64(QByteArray::Base64UrlEncoding);
//    QCOMPARE(arr64, base64url);
//
//    QByteArray base64urlnoequals = base64url;
//    base64urlnoequals.replace('=', "");
//    arr64 = rawdata.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
//    QCOMPARE(arr64, base64urlnoequals);
}

void tst_QByteArray::fromBase64_data()
{
    QTest::addColumn<QByteArray>("rawdata");
    QTest::addColumn<QByteArray>("base64");
    QTest::addColumn<QByteArray::Base64DecodingStatus>("status");

    QTest::newRow("1") << QByteArray("") << QByteArray("  ") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("2") << QByteArray("1") << QByteArray("MQ=") << QByteArray::Base64DecodingStatus::IllegalInputLength;
    QTest::newRow("3") << QByteArray("12") << QByteArray("MTI       ") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("4") << QByteArray("123") << QByteArray("M=TIz") << QByteArray::Base64DecodingStatus::IllegalInputLength;
    QTest::newRow("5") << QByteArray("1234") << QByteArray("MTI zN A ") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("6") << QByteArray("\n") << QByteArray("Cg@") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("7") << QByteArray("a\n") << QByteArray("======YQo=") << QByteArray::Base64DecodingStatus::IllegalInputLength;
    QTest::newRow("8") << QByteArray("ab\n") << QByteArray("Y\nWIK ") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("9") << QByteArray("abc\n") << QByteArray("YWJjCg=") << QByteArray::Base64DecodingStatus::IllegalInputLength;
    QTest::newRow("a") << QByteArray("abcd\n") << QByteArray("YWJ\1j\x9cZAo=") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("b") << QByteArray("abcde\n") << QByteArray("YW JjZ\n G\tUK") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("c") << QByteArray("abcdef\n") << QByteArray("YWJjZGVmCg=") << QByteArray::Base64DecodingStatus::IllegalInputLength;
    QTest::newRow("d") << QByteArray("abcdefg\n") << QByteArray("YWJ\rjZGVmZwo") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("e") << QByteArray("abcdefgh\n") << QByteArray("YWJjZGVmZ2gK====") << QByteArray::Base64DecodingStatus::IllegalPadding;

    QByteArray ba;
    ba.resize(256);
    for (int i = 0; i < 256; ++i)
        ba[i] = i;
    QTest::newRow("f") << ba << QByteArray("AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1Nj\n"
                                           "c4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1u\n"
                                           "b3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpa\n"
                                           "anqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd\n"
                                           "3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+/w==                            ") << QByteArray::Base64DecodingStatus::IllegalCharacter;


    QTest::newRow("g") << QByteArray("foo\0bar", 7) << QByteArray("Zm9vAGJhcg=") << QByteArray::Base64DecodingStatus::IllegalInputLength;
    QTest::newRow("h") << QByteArray("f\xd1oo\x9ctar") << QByteArray("ZtFvb5x 0YXI") << QByteArray::Base64DecodingStatus::IllegalCharacter;
    QTest::newRow("i") << QByteArray("\"\0\0\0\0\0\0\"", 8) << QByteArray("IgAAAAAAACI ") << QByteArray::Base64DecodingStatus::IllegalCharacter;
}

void tst_QByteArray::fromBase64()
{
    //zhaoyujie TODO
//    QFETCH(QByteArray, rawdata);
//    QFETCH(QByteArray, base64);
//    QFETCH(QByteArray::Base64DecodingStatus, status);
//
//    QByteArray::FromBase64Result result;
//
//    result = QByteArray::fromBase64Encoding(base64);
//    QVERIFY(result);
//    QCOMPARE(result.decoded, rawdata);
//
//    result = QByteArray::fromBase64Encoding(base64, QByteArray::Base64Encoding);
//    QVERIFY(result);
//    QCOMPARE(result.decoded, rawdata);
//
//    result = QByteArray::fromBase64Encoding(base64, QByteArray::Base64Encoding | QByteArray::AbortOnBase64DecodingErrors);
//    QVERIFY(!result);
//    QCOMPARE(result.decodingStatus, status);
//    QVERIFY(result.decoded.isEmpty());
//
//    QByteArray arr = base64;
//    QVERIFY(!arr.isDetached());
//    result = QByteArray::fromBase64Encoding(std::move(arr), QByteArray::Base64Encoding | QByteArray::AbortOnBase64DecodingErrors);
//    QVERIFY(!arr.isEmpty());
//    QVERIFY(!result);
//    QCOMPARE(result.decodingStatus, status);
//    QVERIFY(result.decoded.isEmpty());
//
//    arr.detach();
//    QVERIFY(arr.isDetached());
//    result = QByteArray::fromBase64Encoding(std::move(arr), QByteArray::Base64Encoding | QByteArray::AbortOnBase64DecodingErrors);
//    QVERIFY(arr.isEmpty());
//    QVERIFY(!result);
//    QCOMPARE(result.decodingStatus, status);
//    QVERIFY(result.decoded.isEmpty());
//
//    // try "base64url" encoding
//    QByteArray base64url = base64;
//    base64url.replace('/', '_').replace('+', '-');
//    result = QByteArray::fromBase64Encoding(base64url, QByteArray::Base64UrlEncoding);
//    QVERIFY(result);
//    QCOMPARE(result.decoded, rawdata);
//
//    result = QByteArray::fromBase64Encoding(base64url, QByteArray::Base64UrlEncoding | QByteArray::AbortOnBase64DecodingErrors);
//    QVERIFY(!result);
//    QCOMPARE(result.decodingStatus, status);
//    QVERIFY(result.decoded.isEmpty());
//
//    arr = base64url;
//    arr.detach();
//    result = QByteArray::fromBase64Encoding(std::move(arr), QByteArray::Base64UrlEncoding | QByteArray::AbortOnBase64DecodingErrors);
//    QVERIFY(arr.isEmpty());
//    QVERIFY(!result);
//    QCOMPARE(result.decodingStatus, status);
//    QVERIFY(result.decoded.isEmpty());
//
//    if (base64 != base64url) {
//        // check that the invalid decodings fail
//        result = QByteArray::fromBase64Encoding(base64, QByteArray::Base64UrlEncoding);
//        QVERIFY(result);
//        QVERIFY(result.decoded != rawdata);
//        result = QByteArray::fromBase64Encoding(base64url, QByteArray::Base64Encoding);
//        QVERIFY(result);
//        QVERIFY(result.decoded != rawdata);
//
//        result = QByteArray::fromBase64Encoding(base64, QByteArray::Base64UrlEncoding | QByteArray::AbortOnBase64DecodingErrors);
//        QVERIFY(!result);
//        QVERIFY(result.decoded.isEmpty());
//
//        arr = base64;
//        arr.detach();
//        result = QByteArray::fromBase64Encoding(std::move(arr), QByteArray::Base64UrlEncoding | QByteArray::AbortOnBase64DecodingErrors);
//        QVERIFY(arr.isEmpty());
//        QVERIFY(!result);
//        QVERIFY(result.decoded.isEmpty());
//
//        result = QByteArray::fromBase64Encoding(base64url, QByteArray::Base64Encoding | QByteArray::AbortOnBase64DecodingErrors);
//        QVERIFY(!result);
//        QVERIFY(result.decoded.isEmpty());
//
//        arr = base64url;
//        arr.detach();
//        result = QByteArray::fromBase64Encoding(std::move(arr), QByteArray::Base64Encoding | QByteArray::AbortOnBase64DecodingErrors);
//        QVERIFY(arr.isEmpty());
//        QVERIFY(!result);
//        QVERIFY(result.decoded.isEmpty());
//    }
//
//    // also remove padding, if any, and test again. note that by doing
//    // that we might be sanitizing the illegal input, so we can't assume now
//    // that result will be invalid in all cases
//    {
//        auto rightmostNotEqualSign = std::find_if_not(base64url.rbegin(), base64url.rend(), [](char c) { return c == '='; });
//        base64url.chop(std::distance(base64url.rbegin(), rightmostNotEqualSign)); // no QByteArray::erase...
//    }
//
//    result = QByteArray::fromBase64Encoding(base64url, QByteArray::Base64UrlEncoding);
//    QVERIFY(result);
//    QCOMPARE(result.decoded, rawdata);
//
//    result = QByteArray::fromBase64Encoding(base64url, QByteArray::Base64UrlEncoding | QByteArray::AbortOnBase64DecodingErrors);
//    if (result) {
//        QCOMPARE(result.decoded, rawdata);
//    } else {
//        QCOMPARE(result.decodingStatus, status);
//        QVERIFY(result.decoded.isEmpty());
//    }
//
//    arr = base64url;
//    arr.detach();
//    result = QByteArray::fromBase64Encoding(std::move(arr), QByteArray::Base64UrlEncoding | QByteArray::AbortOnBase64DecodingErrors);
//    QVERIFY(arr.isEmpty());
//    if (result) {
//        QCOMPARE(result.decoded, rawdata);
//    } else {
//        QCOMPARE(result.decodingStatus, status);
//        QVERIFY(result.decoded.isEmpty());
//    }
}

void tst_QByteArray::qvsnprintf()
{
    //zhaoyujie TODO
//    char buf[20];
//    memset(buf, 42, sizeof(buf));
//
//    QCOMPARE(::qsnprintf(buf, 10, "%s", "bubu"), 4);
//    QCOMPARE(static_cast<const char *>(buf), "bubu");
//
//#ifndef Q_CC_MSVC
//    // MSVC implementation of vsnprintf overwrites bytes after null terminator so this would fail.
//    QCOMPARE(buf[5], char(42));
//#endif
//
//    memset(buf, 42, sizeof(buf));
//    QCOMPARE(::qsnprintf(buf, 5, "%s", "bubu"), 4);
//    QCOMPARE(static_cast<const char *>(buf), "bubu");
//    QCOMPARE(buf[5], char(42));
//
//    memset(buf, 42, sizeof(buf));
//
//#ifdef Q_OS_WIN
//    // VS 2005 uses the Qt implementation of vsnprintf.
//# if defined(_MSC_VER)
//    QCOMPARE(::qsnprintf(buf, 3, "%s", "bubu"), -1);
//    QCOMPARE(static_cast<const char*>(buf), "bu");
//# else
//    // windows has to do everything different, of course.
//    QCOMPARE(::qsnprintf(buf, 3, "%s", "bubu"), -1);
//    buf[19] = '\0';
//    QCOMPARE(static_cast<const char *>(buf), "bub****************");
//# endif
//#else
//    QCOMPARE(::qsnprintf(buf, 3, "%s", "bubu"), 4);
//    QCOMPARE(static_cast<const char*>(buf), "bu");
//#endif
//    QCOMPARE(buf[4], char(42));
//#ifndef Q_OS_WIN
//    memset(buf, 42, sizeof(buf));
//    QCOMPARE(::qsnprintf(buf, 10, ""), 0);
//#endif
}

void tst_QByteArray::qstrlen()
{
    const char *src = "Something about ... \0 a string.";
    QCOMPARE(::qstrlen((char*)0), (uint)0);
    QCOMPARE(::qstrlen(src), (uint)20);
}

void tst_QByteArray::qstrnlen()
{
    const char *src = "Something about ... \0 a string.";
    QCOMPARE(::qstrnlen((char*)0, 1), (uint)0);
    QCOMPARE(::qstrnlen(src, 31), (uint)20);
    QCOMPARE(::qstrnlen(src, 19), (uint)19);
    QCOMPARE(::qstrnlen(src, 21), (uint)20);
    QCOMPARE(::qstrnlen(src, 20), (uint)20);
}

void tst_QByteArray::qstrcpy()
{
    //zhaoyujie TODO
//    const char *src = "Something about ... \0 a string.";
//    const char *expected = "Something about ... ";
//    char dst[128];
//
//    QCOMPARE(::qstrcpy(0, 0), (char*)0);
//    QCOMPARE(::qstrcpy(dst, 0), (char*)0);
//
//    QCOMPARE(::qstrcpy(dst ,src), (char *)dst);
//    QCOMPARE((char *)dst, const_cast<char *>(expected));
}

void tst_QByteArray::qstrncpy()
{
    //zhaoyujie TODO
//    QByteArray src(1024, 'a'), dst(1024, 'b');
//
//    // dst == nullptr
//    QCOMPARE(::qstrncpy(0, src.data(),  0), (char*)0);
//    QCOMPARE(::qstrncpy(0, src.data(), 10), (char*)0);
//
//    // src == nullptr
//    QCOMPARE(::qstrncpy(dst.data(), 0,  0), (char*)0);
//    QCOMPARE(::qstrncpy(dst.data(), 0, 10), (char*)0);
//
//    // valid pointers, but len == 0
//    QCOMPARE(::qstrncpy(dst.data(), src.data(), 0), dst.data());
//    QCOMPARE(*dst.data(), 'b'); // must not have written to dst
//
//    // normal copy
//    QCOMPARE(::qstrncpy(dst.data(), src.data(), src.size()), dst.data());
//
//    src = QByteArray( "Tumdelidum" );
//    QCOMPARE(QByteArray(::qstrncpy(dst.data(), src.data(), src.size())),
//             QByteArray("Tumdelidu"));
//
//    // normal copy with length is longer than necessary
//    src = QByteArray( "Tumdelidum\0foo" );
//    dst.resize(128*1024);
//    QCOMPARE(QByteArray(::qstrncpy(dst.data(), src.data(), dst.size())),
//             QByteArray("Tumdelidum"));
}

void tst_QByteArray::chop_data()
{
    QTest::addColumn<QByteArray>("src");
    QTest::addColumn<int>("choplength");
    QTest::addColumn<QByteArray>("expected");

    QTest::newRow("1") << QByteArray("short1") << 128 << QByteArray();
    QTest::newRow("2") << QByteArray("short2") << int(strlen("short2"))
                       << QByteArray();
    QTest::newRow("3") << QByteArray("abcdef\0foo", 10) << 2
                       << QByteArray("abcdef\0f", 8);
    QTest::newRow("4") << QByteArray("STARTTLS\r\n") << 2
                       << QByteArray("STARTTLS");
    QTest::newRow("5") << QByteArray("") << 1 << QByteArray();
    QTest::newRow("6") << QByteArray("foo") << 0 << QByteArray("foo");
    QTest::newRow("7") << QByteArray(0) << 28 << QByteArray();
    QTest::newRow("null 0") << QByteArray() << 0 << QByteArray();
    QTest::newRow("null 10") << QByteArray() << 10 << QByteArray();
}

void tst_QByteArray::chop()
{
    QFETCH(QByteArray, src);
    QFETCH(int, choplength);
    QFETCH(QByteArray, expected);

    src.chop(choplength);
    QCOMPARE(src, expected);
}

void tst_QByteArray::prepend() {
    const char data[] = "data";

    QCOMPARE(QByteArray().prepend(QByteArray()), QByteArray());
    QCOMPARE(QByteArray().prepend('a'), QByteArray("a"));
    QCOMPARE(QByteArray().prepend(2, 'a'), QByteArray("aa"));
    QCOMPARE(QByteArray().prepend(QByteArray("data")), QByteArray("data"));
    QCOMPARE(QByteArray().prepend(data), QByteArray("data"));
    QCOMPARE(QByteArray().prepend(data, 2), QByteArray("da"));
    QCOMPARE(QByteArray().prepend(QByteArrayView(data)), QByteArray("data"));

    QByteArray ba("foo");
    QCOMPARE(ba.prepend((char*)0), QByteArray("foo"));
    QCOMPARE(ba.prepend(QByteArray()), QByteArray("foo"));
    QCOMPARE(ba.prepend("1"), QByteArray("1foo"));
    QCOMPARE(ba.prepend(QByteArray("2")), QByteArray("21foo"));
    QCOMPARE(ba.prepend('3'), QByteArray("321foo"));
    QCOMPARE(ba.prepend(-1, 'x'), QByteArray("321foo"));
    QCOMPARE(ba.prepend(3, 'x'), QByteArray("xxx321foo"));
    QCOMPARE(ba.prepend("\0 ", 2), QByteArray::fromRawData("\0 xxx321foo", 11));

    QByteArray tenChars;
    tenChars.reserve(10);
    QByteArray twoChars("ab");
    tenChars.prepend(twoChars);
    QCOMPARE(tenChars.capacity(), 10);
}

void tst_QByteArray::prependExtended_data()
{
    QTest::addColumn<QByteArray>("array");
    QTest::newRow("literal") << QByteArray(QByteArrayLiteral("data"));
    QTest::newRow("standard") << QByteArray(staticStandard);
    QTest::newRow("notNullTerminated") << QByteArray(staticNotNullTerminated);
    QTest::newRow("non static data") << QByteArray("data");
    QTest::newRow("from raw data") << QByteArray::fromRawData("data", 4);
    QTest::newRow("from raw data not terminated") << QByteArray::fromRawData("dataBAD", 4);
}

void tst_QByteArray::prependExtended()
{
    QFETCH(QByteArray, array);

    QCOMPARE(QByteArray().prepend(array), QByteArray("data"));
    QCOMPARE(QByteArray("").prepend(array), QByteArray("data"));

    QCOMPARE(array.prepend((char*)0), QByteArray("data"));
    QCOMPARE(array.prepend(QByteArray()), QByteArray("data"));
    QCOMPARE(array.prepend("1"), QByteArray("1data"));
    QCOMPARE(array.prepend(QByteArray("2")), QByteArray("21data"));
    QCOMPARE(array.prepend('3'), QByteArray("321data"));
    QCOMPARE(array.prepend(-1, 'x'), QByteArray("321data"));
    QCOMPARE(array.prepend(3, 'x'), QByteArray("xxx321data"));
    QCOMPARE(array.prepend("\0 ", 2), QByteArray::fromRawData("\0 xxx321data", 12));
    QCOMPARE(array.size(), 12);
}

void tst_QByteArray::append()
{
    const char data[] = "data";

    QCOMPARE(QByteArray().append(QByteArray()), QByteArray());
    QCOMPARE(QByteArray().append('a'), QByteArray("a"));
    QCOMPARE(QByteArray().append(2, 'a'), QByteArray("aa"));
    QCOMPARE(QByteArray().append(QByteArray("data")), QByteArray("data"));
    QCOMPARE(QByteArray().append(data), QByteArray("data"));
    QCOMPARE(QByteArray().append(data, 2), QByteArray("da"));
    QCOMPARE(QByteArray().append(QByteArrayView(data)), QByteArray("data"));

    QByteArray ba("foo");
    QCOMPARE(ba.append((char*)0), QByteArray("foo"));
    QCOMPARE(ba.append(QByteArray()), QByteArray("foo"));
    QCOMPARE(ba.append("1"), QByteArray("foo1"));
    QCOMPARE(ba.append(QByteArray("2")), QByteArray("foo12"));
    QCOMPARE(ba.append('3'), QByteArray("foo123"));
    QCOMPARE(ba.append(-1, 'x'), QByteArray("foo123"));
    QCOMPARE(ba.append(3, 'x'), QByteArray("foo123xxx"));
    QCOMPARE(ba.append("\0"), QByteArray("foo123xxx"));
    QCOMPARE(ba.append("\0", 1), QByteArray::fromRawData("foo123xxx\0", 10));
    QCOMPARE(ba.size(), 10);

    QByteArray tenChars;
    tenChars.reserve(10);
    QByteArray twoChars("ab");
    tenChars.append(twoChars);
    QCOMPARE(tenChars.capacity(), 10);

    {
        QByteArray prepended("abcd");
        prepended.prepend('a');
        const qsizetype freeAtEnd = prepended.data_ptr()->freeSpaceAtEnd();
        QVERIFY(prepended.size() + freeAtEnd < prepended.capacity());
        prepended += QByteArray(freeAtEnd, 'b');
        prepended.append('c');
        QCOMPARE(prepended, QByteArray("aabcd") + QByteArray(freeAtEnd, 'b') + QByteArray("c"));
    }

    {
        QByteArray prepended2("aaaaaaaaaa");
        while (prepended2.size()) {
            prepended2.remove(0, 1);
        }
        QVERIFY(prepended2.data_ptr()->freeSpaceAtBegin() > 0);
        QByteArray array(prepended2.data_ptr()->freeSpaceAtEnd(), 'a');
        prepended2 += array;
        prepended2.append('b');
        QCOMPARE(prepended2, array + QByteArray("b"));
    }
}

void tst_QByteArray::appendExtended_data()
{
    prependExtended_data();
}

void tst_QByteArray::appendExtended()
{
    QFETCH(QByteArray, array);

    QCOMPARE(QByteArray().append(array), QByteArray("data"));
    QCOMPARE(QByteArray("").append(array), QByteArray("data"));

    QCOMPARE(array.append((char*)0), QByteArray("data"));
    QCOMPARE(array.append(QByteArray()), QByteArray("data"));
    QCOMPARE(array.append("1"), QByteArray("data1"));
    QCOMPARE(array.append(QByteArray("2")), QByteArray("data12"));
    QCOMPARE(array.append('3'), QByteArray("data123"));
    QCOMPARE(array.append(-1, 'x'), QByteArray("data123"));
    QCOMPARE(array.append(3, 'x'), QByteArray("data123xxx"));
    QCOMPARE(array.append("\0"), QByteArray("data123xxx"));
    QCOMPARE(array.append("\0", 1), QByteArray::fromRawData("data123xxx\0", 11));
    QCOMPARE(array.size(), 11);
}

void tst_QByteArray::insert()
{
    const char data[] = "data";

    QCOMPARE(QByteArray().insert(0, QByteArray()), QByteArray());
    QCOMPARE(QByteArray().insert(0, 'a'), QByteArray("a"));
    QCOMPARE(QByteArray().insert(0, 2, 'a'), QByteArray("aa"));
    QCOMPARE(QByteArray().insert(0, QByteArray("data")), QByteArray("data"));
    QCOMPARE(QByteArray().insert(0, data), QByteArray("data"));
    QCOMPARE(QByteArray().insert(0, data, 2), QByteArray("da"));
    QCOMPARE(QByteArray().insert(0, QByteArrayView(data)), QByteArray("data"));

    // 超出范围会补充空格
    // insert into empty with offset
    QCOMPARE(QByteArray().insert(2, QByteArray()), QByteArray());
    QCOMPARE(QByteArray().insert(2, 'a'), QByteArray("  a"));
    QCOMPARE(QByteArray().insert(2, 2, 'a'), QByteArray("  aa"));
    QCOMPARE(QByteArray().insert(2, QByteArray("data")), QByteArray("  data"));
    QCOMPARE(QByteArray().insert(2, data), QByteArray("  data"));
    QCOMPARE(QByteArray().insert(2, data, 2), QByteArray("  da"));
    QCOMPARE(QByteArray().insert(2, QByteArrayView(data)), QByteArray("  data"));

    QByteArray ba("Meal");
    QCOMPARE(ba.insert(1, QByteArray("ontr")), QByteArray("Montreal"));
    QCOMPARE(ba.insert(ba.size(), "foo"), QByteArray("Montrealfoo"));

    ba = QByteArray("13");
    QCOMPARE(ba.insert(1, QByteArray("2")), QByteArray("123"));

    ba = "ac";
    QCOMPARE(ba.insert(1, 'b'), QByteArray("abc"));
    QCOMPARE(ba.size(), 3);

    ba = "ac";
    QCOMPARE(ba.insert(-1, 3, 'x'), QByteArray("ac"));
    QCOMPARE(ba.insert(1, 3, 'x'), QByteArray("axxxc"));
    QCOMPARE(ba.insert(6, 3, 'x'), QByteArray("axxxc xxx"));
    QCOMPARE(ba.size(), 9);

    ba = "ikl";
    QCOMPARE(ba.insert(1, "j"), QByteArray("ijkl"));
    QCOMPARE(ba.size(), 4);

    ba = "ab";
    QCOMPARE(ba.insert(1, "\0X\0", 3), QByteArray::fromRawData("a\0X\0b", 5));
    QCOMPARE(ba.size(), 5);

    ba = "Hello World";
    QCOMPARE(ba.insert(5, QByteArrayView(",")), QByteArray("Hello, World"));
    QCOMPARE(ba.size(), 12);

    ba = "one";
    QCOMPARE(ba.insert(1, ba), QByteArray("oonene"));
    QCOMPARE(ba.size(), 6);

    ba = "one";
    QCOMPARE(ba.insert(1, QByteArrayView(ba)), QByteArray("oonene"));
    QCOMPARE(ba.size(), 6);

    {
        ba = "one";
        ba.prepend('a');
        QByteArray b(ba.data_ptr()->freeSpaceAtEnd(), 'b');
        QCOMPARE(ba.insert(ba.size() + 1, QByteArrayView(b)), QByteArray("aone ") + b);
    }

    {
        ba = "onetwothree";
        while (ba.size() - 1)
            ba.remove(0, 1);
        QByteArray b(ba.data_ptr()->freeSpaceAtEnd() + 1, 'b');
        QCOMPARE(ba.insert(ba.size() + 1, QByteArrayView(b)), QByteArray("e ") + b);
    }

    {
        ba = "one";
        ba.prepend('a');
        const qsizetype freeAtEnd = ba.data_ptr()->freeSpaceAtEnd();
        QCOMPARE(ba.insert(ba.size() + 1, freeAtEnd + 1, 'b'),
                 QByteArray("aone ") + QByteArray(freeAtEnd + 1, 'b'));
    }

    {
        ba = "onetwothree";
        while (ba.size() - 1)
            ba.remove(0, 1);
        const qsizetype freeAtEnd = ba.data_ptr()->freeSpaceAtEnd();
        QCOMPARE(ba.insert(ba.size() + 1, freeAtEnd + 1, 'b'),
                 QByteArray("e ") + QByteArray(freeAtEnd + 1, 'b'));
    }
}

void tst_QByteArray::insertExtended_data()
{
    prependExtended_data();
}

void tst_QByteArray::insertExtended()
{
    QFETCH(QByteArray, array);
    QCOMPARE(array.insert(1, "i"), QByteArray("diata"));
    QCOMPARE(array.insert(1, 3, 'x'), QByteArray("dxxxiata"));
    QCOMPARE(array.size(), 8);
}

QTEST_APPLESS_MAIN(tst_QByteArray)