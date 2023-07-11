//
// Created by Yujie Zhao on 2023/6/5.
//
#include <QObject>
#include <QStringView>
#include <QLatin1String>
#include <QTest>
#include <QList>
#include <QVarLengthArray>
#include <deque>
#include <list>
#include <QString>

//std::is_convertible是否可以隐式转换为另一种类型
template <typename T>
using CanConvert = std::is_convertible<T, QStringView>;

static_assert(!CanConvert<QLatin1String>::value);
static_assert(!CanConvert<const char *>::value);
static_assert(!CanConvert<QByteArray>::value);

//QChar
static_assert(!CanConvert<QChar>::value);
static_assert(CanConvert<QChar[123]>::value);

static_assert(CanConvert<QString>::value);
static_assert(CanConvert<const QString>::value);
static_assert(CanConvert<QString &>::value);
static_assert(CanConvert<const QString &>::value);

//ushort
static_assert(!CanConvert<ushort>::value);
static_assert(CanConvert<ushort[123]>::value);
static_assert(CanConvert<ushort *>::value);
static_assert(CanConvert<const ushort *>::value);

static_assert(CanConvert<QList<ushort>>::value);
static_assert(CanConvert<QVarLengthArray<ushort>>::value);
static_assert(CanConvert<std::vector<ushort>>::value);
static_assert(CanConvert<std::array<ushort, 123>>::value);
static_assert(!CanConvert<std::deque<ushort>>::value);
static_assert(!CanConvert<std::list<ushort>>::value);

//char16_t
static_assert(!CanConvert<char16_t>::value);
static_assert(CanConvert<char16_t *>::value);
static_assert(CanConvert<const char16_t *>::value);
static_assert(CanConvert<std::u16string>::value);
static_assert(CanConvert<const std::u16string>::value);
static_assert(CanConvert<std::u16string &>::value);
static_assert(CanConvert<const std::u16string &>::value);

static_assert(CanConvert<std::u16string_view>::value);
static_assert(CanConvert<const std::u16string_view>::value);
static_assert(CanConvert<std::u16string_view &>::value);
static_assert(CanConvert<const std::u16string_view &>::value);

static_assert(CanConvert<QList<char16_t>>::value);
static_assert(CanConvert<QVarLengthArray<char16_t>>::value);
static_assert(CanConvert<std::vector<char16_t>>::value);
static_assert(CanConvert<std::array<char16_t, 123>>::value);
static_assert(!CanConvert<std::deque<char16_t>>::value);
static_assert(!CanConvert<std::list<char16_t>>::value);

//wchar_t 在 平台上表现不同，mac上为4字节
constexpr bool CanConvertFromWCharT =
#ifdef Q_OS_WIN
        true;
#else
        false;
#endif

static_assert(!CanConvert<wchar_t>::value);
static_assert(CanConvert<wchar_t *>::value == CanConvertFromWCharT);
static_assert(CanConvert<const wchar_t *>::value == CanConvertFromWCharT);

static_assert(CanConvert<std::wstring>::value == CanConvertFromWCharT);
static_assert(CanConvert<const std::wstring>::value == CanConvertFromWCharT);
static_assert(CanConvert<std::wstring &>::value == CanConvertFromWCharT);
static_assert(CanConvert<const std::wstring &>::value == CanConvertFromWCharT);

static_assert(CanConvert<std::wstring_view>::value == CanConvertFromWCharT);
static_assert(CanConvert<const std::wstring_view>::value == CanConvertFromWCharT);
static_assert(CanConvert<std::wstring_view &>::value == CanConvertFromWCharT);
static_assert(CanConvert<const std::wstring_view &>::value == CanConvertFromWCharT);

static_assert(CanConvert<QList<wchar_t>>::value == CanConvertFromWCharT);
static_assert(CanConvert<QVarLengthArray<wchar_t>>::value == CanConvertFromWCharT);
static_assert(CanConvert<std::vector<wchar_t>>::value == CanConvertFromWCharT);
static_assert(CanConvert<std::array<wchar_t, 123>>::value == CanConvertFromWCharT);
static_assert(!CanConvert<std::deque<wchar_t>>::value);
static_assert(!CanConvert<std::list<wchar_t>>::value);

class tst_QStringView : public QObject {
public:
    using QObject::QObject;
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(constExpr);
        REGISTER_OBJECT_INVOKE_METHOD(basics);
        REGISTER_OBJECT_INVOKE_METHOD(literals);
        REGISTER_OBJECT_INVOKE_METHOD(fromArray);
        REGISTER_OBJECT_INVOKE_METHOD(at);
        REGISTER_OBJECT_INVOKE_METHOD(arg);
    }

    void constExpr() const;
    void basics() const;
    void literals() const;
    void fromArray() const;
    void at() const;
    void arg() const;
};

void tst_QStringView::constExpr() const
{
    {
        constexpr QStringView sv;
        static_assert(sv.size() == 0);
        static_assert(sv.isNull());
        static_assert(sv.empty());
        static_assert(sv.isEmpty());
        static_assert(sv.utf16() == nullptr);

        constexpr QStringView sv2(sv.utf16(), sv.utf16() + sv.size());
        static_assert(sv2.isNull());
        static_assert(sv2.empty());
    }

    {
        constexpr QStringView sv = nullptr;
        static_assert(sv.size() == 0);
        static_assert(sv.isNull());
        static_assert(sv.empty());
        static_assert(sv.isEmpty());
        static_assert(sv.utf16() == nullptr);
    }

    {
        constexpr QStringView sv = u"";
        static_assert(sv.size() == 0);
        static_assert(!sv.isNull());
        static_assert(sv.empty());
        static_assert(sv.isEmpty());
        static_assert(sv.utf16() != nullptr);

        constexpr QStringView sv2(sv.utf16(), sv.utf16() + sv.size());
        static_assert(!sv.isNull());
        static_assert(sv2.empty());
    }

    {
        constexpr QStringView sv = u"Hello";
        static_assert(sv.size() == 5);
        static_assert(!sv.empty());
        static_assert(!sv.isEmpty());
        static_assert(!sv.isNull());
        static_assert(*sv.utf16() == 'H');
        static_assert(sv[0] == QLatin1Char('H'));
        static_assert(sv.at(0) == QLatin1Char('H'));
        static_assert(sv.front() == QLatin1Char('H'));
        static_assert(sv.first() == QLatin1Char('H'));
        static_assert(sv[4] == QLatin1Char('o'));
        static_assert(sv.at(4) == QLatin1Char('o'));
        static_assert(sv.back() == QLatin1Char('o'));
        static_assert(sv.last() == QLatin1Char('o'));

        constexpr QStringView sv2(sv.utf16(), sv.utf16() + sv.size());
        static_assert(!sv2.isNull());
        static_assert(!sv2.empty());
        static_assert(sv.size() == 5);
    }

    {
        static_assert(QStringView(u"Hello").size() == 5);
        constexpr QStringView sv = u"Hello";
        static_assert(sv.size() == 5);
        static_assert(!sv.empty());
        static_assert(!sv.isEmpty());
        static_assert(!sv.isNull());
        static_assert(*sv.utf16() == 'H');
        static_assert(sv[0] == QLatin1Char('H'));
        static_assert(sv.at(0) == QLatin1Char('H'));
        static_assert(sv.front() == QLatin1Char('H'));
        static_assert(sv.first() == QLatin1Char('H'));
        static_assert(sv[4] == QLatin1Char('o'));
        static_assert(sv.at(4) == QLatin1Char('o'));
        static_assert(sv.back() == QLatin1Char('o'));
        static_assert(sv.back() == QLatin1Char('o'));

        constexpr QStringView sv2(sv.utf16(), sv.utf16() + sv.size());
        static_assert(!sv2.isNull());
        static_assert(!sv2.empty());
        static_assert(sv2.size() == 5);

        constexpr char16_t *null = nullptr;
        constexpr QStringView sv3(null);
        static_assert(sv3.isNull());
        static_assert(sv3.isEmpty());
        static_assert(sv3.size() == 0);
    }
}

void tst_QStringView::basics() const
{
    QStringView sv1;
    QVERIFY(sv1.isNull());
    QVERIFY(sv1.isEmpty());

    QStringView sv2;
    QVERIFY(sv2 == sv1);
    QVERIFY(!(sv2 != sv1));
}

void tst_QStringView::literals() const
{
    const char16_t hello[] = u"Hello";
    const char16_t longhello[] = u"Hello World. This is a much longer message, to exercise qustrlen.";
    const char16_t withnull[] = u"a\0zzz";
    static_assert(sizeof(longhello) >= 16);

    QCOMPARE(QStringView(hello).size(), 5);
    QCOMPARE(QStringView(hello + 0).size(), 5);
    QStringView sv = hello;
    QCOMPARE(sv.size(), 5);
    QVERIFY(!sv.empty());
    QVERIFY(!sv.isEmpty());
    QVERIFY(!sv.isNull());
    QCOMPARE(*sv.utf16(), 'H');
    QCOMPARE(sv[0], QLatin1Char('H'));
    QCOMPARE(sv.at(0), QLatin1Char('H'));
    QCOMPARE(sv.front(), QLatin1Char('H'));
    QCOMPARE(sv.first(), QLatin1Char('H'));
    QCOMPARE(sv[4], QLatin1Char('o'));
    QCOMPARE(sv.back(), QLatin1Char('o'));
    QCOMPARE(sv.last(), QLatin1Char('o'));

    QStringView sv2(sv.utf16(), sv.utf16() + sv.size());
    QVERIFY(!sv2.isNull());
    QVERIFY(!sv2.empty());
    QCOMPARE(sv2.size(), 5);

    QStringView sv3(longhello);
    QCOMPARE(size_t(sv3.size()), sizeof(longhello) / sizeof(longhello[0]) - 1);
    QCOMPARE(sv3.last(), QLatin1Char('.'));
    sv3 = longhello;
    QCOMPARE(size_t(sv3.size()), sizeof(longhello) / sizeof(longhello[0]) - 1);

    for (int i = 0; i < sv3.size(); ++i) {
        QStringView sv4(longhello + i);
        QCOMPARE(size_t(sv4.size()), sizeof(longhello) / sizeof(longhello[0]) - 1 - i);
        QCOMPARE(sv4.last(), QLatin1Char('.'));
        sv4 = longhello + i;
        QCOMPARE(size_t(sv4.size()), sizeof(longhello) / sizeof(longhello[0]) - 1 - i);
    }

    QCOMPARE(size_t(QStringView(withnull).size()), size_t(1));
    QCOMPARE(size_t(QStringView::fromArray(withnull).size()), sizeof(withnull) / sizeof(withnull[0]));
    QCOMPARE(QStringView(withnull + 0).size(), qsizetype(1));
}

void tst_QStringView::fromArray() const
{
    static constexpr char16_t hello[] = u"Hello\0abc\0\0.";

    constexpr QStringView sv = QStringView::fromArray(hello);
    QCOMPARE(sv.size(), 13);
    QVERIFY(!sv.empty());
    QVERIFY(!sv.isEmpty());
    QVERIFY(!sv.isNull());
    QCOMPARE(*sv.data(), 'H');
    QCOMPARE(sv[0], 'H');
    QCOMPARE(sv.at(0), 'H');
    QCOMPARE(sv.front(), 'H');
    QCOMPARE(sv.first(), 'H');
    QCOMPARE(sv[4], 'o');
    QCOMPARE(sv.at(4), 'o');
    QCOMPARE(sv[5], '\0');
    QCOMPARE(sv.at(5), '\0');
    QCOMPARE(*(sv.data() + sv.size() -2), '.');
    QCOMPARE(sv.back(), '\0');
    QCOMPARE(sv.last(), '\0');

    const char16_t bytes[] = {u'a', u'b', u'c'};
    QStringView sv2 = QStringView::fromArray(bytes);
    QCOMPARE(sv2.data(), reinterpret_cast<const QChar *>(bytes + 0));
    QCOMPARE(sv2.size(), 3);
    QCOMPARE(sv2.first(), u'a');
    QCOMPARE(sv2.last(), u'c');
}

void tst_QStringView::at() const
{
//    QString hello("Hello");
//    QStringView sv(hello);
//    QCOMPARE(sv.at(0), QChar('H'));
//    QCOMPARE(sv[0], QChar('H'));
//    QCOMPARE(sv.at(1), QChar('e'));
//    QCOMPARE(sv[1], QChar('e'));
//    QCOMPARE(sv.at(2), QChar('l'));
//    QCOMPARE(sv[2], QChar('l'));
//    QCOMPARE(sv.at(3), QChar('l'));
//    QCOMPARE(sv[3], QChar('l'));
//    QCOMPARE(sv.at(4), QChar('o'));
//    QCOMPARE(sv[4], QChar('o'));

    QStringView sv(u"Hello");
    QCOMPARE(sv.at(0), QChar('H'));
    QCOMPARE(sv[0], QChar('H'));
    QCOMPARE(sv.at(1), QChar('e'));
    QCOMPARE(sv[1], QChar('e'));
    QCOMPARE(sv.at(2), QChar('l'));
    QCOMPARE(sv[2], QChar('l'));
    QCOMPARE(sv.at(3), QChar('l'));
    QCOMPARE(sv[3], QChar('l'));
    QCOMPARE(sv.at(4), QChar('o'));
    QCOMPARE(sv[4], QChar('o'));
}

void tst_QStringView::arg() const
{
}

QTEST_APPLESS_MAIN(tst_QStringView)