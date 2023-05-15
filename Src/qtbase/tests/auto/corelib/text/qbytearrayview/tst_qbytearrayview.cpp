//
// Created by Yujie Zhao on 2023/5/9.
//

#include <QByteArrayView>
#include <QByteArray>
#include <QObject>
#include <QTest>
#include <vector>

//std::convertible 是否可以隐式转换成另一种类型
//template <typename T>
//constexpr bool CanConvert = std::is_convertible_v<T, QByteArrayView>;
//
//static_assert(!CanConvert<QString>);
//static_assert(!CanConvert<QStringView>);
//static_assert(!CanConvert<const char16_t*>);
//
//static_assert(!CanConvert<char>);
//static_assert(CanConvert<char[1]>);
//static_assert(CanConvert<const char[1]>);
//static_assert(CanConvert<char*>);
//static_assert(CanConvert<const char*>);
//
//static_assert(!CanConvert<uchar>);
//static_assert(!CanConvert<uchar[1]>);
//static_assert(!CanConvert<const uchar[1]>);
//static_assert(CanConvert<uchar*>);
//static_assert(CanConvert<const uchar*>);
//
//static_assert(!CanConvert<signed char>);
//static_assert(!CanConvert<signed char[1]>);
//static_assert(!CanConvert<const signed char[1]>);
//static_assert(CanConvert<signed char*>);
//static_assert(CanConvert<const signed char*>);
//
//static_assert(!CanConvert<std::byte>);
//static_assert(!CanConvert<std::byte[1]>);
//static_assert(!CanConvert<const std::byte[1]>);
//static_assert(CanConvert<std::byte*>);
//static_assert(CanConvert<const std::byte*>);
//
//static_assert(CanConvert<      QByteArray >);
//static_assert(CanConvert<const QByteArray >);
//static_assert(CanConvert<      QByteArray&>);
//static_assert(CanConvert<const QByteArray&>);
//
//static_assert(CanConvert<      std::string >);
//static_assert(CanConvert<const std::string >);
//static_assert(CanConvert<      std::string&>);
//static_assert(CanConvert<const std::string&>);
//
//static_assert(CanConvert<      std::string_view >);
//static_assert(CanConvert<const std::string_view >);
//static_assert(CanConvert<      std::string_view&>);
//static_assert(CanConvert<const std::string_view&>);
//
//static_assert(CanConvert<      QVector<char> >);
//static_assert(CanConvert<const QVector<char> >);
//static_assert(CanConvert<      QVector<char>&>);
//static_assert(CanConvert<const QVector<char>&>);
//
//static_assert(CanConvert<      QVarLengthArray<char> >);
//static_assert(CanConvert<const QVarLengthArray<char> >);
//static_assert(CanConvert<      QVarLengthArray<char>&>);
//static_assert(CanConvert<const QVarLengthArray<char>&>);
//
//static_assert(CanConvert<      std::vector<char> >);
//static_assert(CanConvert<const std::vector<char> >);
//static_assert(CanConvert<      std::vector<char>&>);
//static_assert(CanConvert<const std::vector<char>&>);
//
//static_assert(CanConvert<      std::array<char, 1> >);
//static_assert(CanConvert<const std::array<char, 1> >);
//static_assert(CanConvert<      std::array<char, 1>&>);
//static_assert(CanConvert<const std::array<char, 1>&>);
//
//static_assert(!CanConvert<std::deque<char>>);
//static_assert(!CanConvert<std::list<char>>);

class tst_QByteArrayView : public QObject
{
public:
    using QObject::QObject;
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(constExpr);
        REGISTER_OBJECT_INVOKE_METHOD(basics);
        REGISTER_OBJECT_INVOKE_METHOD(literals);
        REGISTER_OBJECT_INVOKE_METHOD(fromArray);
        REGISTER_OBJECT_INVOKE_METHOD(literalsWithInternalNulls);
        REGISTER_OBJECT_INVOKE_METHOD(at);
        REGISTER_OBJECT_INVOKE_METHOD(fromQByteArray);
        REGISTER_OBJECT_INVOKE_METHOD(fromCharStar);
        REGISTER_OBJECT_INVOKE_METHOD(fromUCharStar);
        REGISTER_OBJECT_INVOKE_METHOD(fromSignedCharStar);
        REGISTER_OBJECT_INVOKE_METHOD(fromStdByteArray);
        REGISTER_OBJECT_INVOKE_METHOD(fromCharRange);
        REGISTER_OBJECT_INVOKE_METHOD(fromUCharRange);
        REGISTER_OBJECT_INVOKE_METHOD(fromSignedCharRange);
        REGISTER_OBJECT_INVOKE_METHOD(fromStdByteRange);
        REGISTER_OBJECT_INVOKE_METHOD(fromCharContainers);
        REGISTER_OBJECT_INVOKE_METHOD(fromUCharContainers);
        REGISTER_OBJECT_INVOKE_METHOD(fromSignedCharContainers);
        REGISTER_OBJECT_INVOKE_METHOD(fromStdByteContainers);
        REGISTER_OBJECT_INVOKE_METHOD(comparison);
        REGISTER_OBJECT_INVOKE_METHOD(compare);
    }

    void constExpr() const;
    void basics() const;
    void literals() const;
    void fromArray() const;
    void literalsWithInternalNulls() const;
    void at() const;
    void fromQByteArray() const;
    void fromCharStar() const;
    void fromUCharStar() const;
    void fromSignedCharStar() const;
    void fromStdByteArray() const;
    void fromCharRange() const;
    void fromUCharRange() const;
    void fromSignedCharRange() const;
    void fromStdByteRange() const;
    void fromCharContainers() const;
    void fromUCharContainers() const;
    void fromSignedCharContainers() const;
    void fromStdByteContainers() const;
    void comparison() const;
    void compare() const;

private:
    template <typename Data>
    void conversionTests(Data arg) const;
    template <typename Char>
    void fromEmptyLiteral() const;
    template <typename Char>
    void fromRange(const Char *first, const Char *last) const;
    template <typename Char, typename Container>
    void fromContainer() const;
    template <typename Char>
    void fromContainers() const;
};

void tst_QByteArrayView::constExpr() const
{
    {
        constexpr QByteArrayView bv;
        static_assert(bv.size() == 0);
        static_assert(bv.isNull());
        static_assert(bv.empty());
        static_assert(bv.isEmpty());
        static_assert(bv.data() == nullptr);

        constexpr QByteArrayView bv2(bv.data(), bv.data() + bv.size());
        static_assert(bv2.isNull());
        static_assert(bv2.empty());
    }
    {
        constexpr QByteArrayView bv = "";
        static_assert(bv.size() == 0);
        static_assert(!bv.isNull());
        static_assert(bv.empty());
        static_assert(bv.isEmpty());
        static_assert(bv.data() != nullptr);

        constexpr QByteArrayView bv2(bv.data(), bv.data() + bv.size());
        static_assert(!bv2.isNull());
        static_assert(bv2.empty());
    }
    {
        static_assert(QByteArrayView("Hello").size() == 5);
        constexpr QByteArrayView bv = "Hello";
        static_assert(bv.size() == 5);
        static_assert(!bv.empty());
        static_assert(!bv.isEmpty());
        static_assert(!bv.isNull());
        static_assert(*bv.data() == 'H');
        static_assert(bv[0]      == 'H');
        static_assert(bv.at(0)   == 'H');
        static_assert(bv.front() == 'H');
        static_assert(bv.first() == 'H');
        static_assert(bv[4]      == 'o');
        static_assert(bv.at(4)   == 'o');
        static_assert(bv.back()  == 'o');
        static_assert(bv.last()  == 'o');

        static_assert(*bv.begin()      == 'H' );
        static_assert(*(bv.end() - 1)  == 'o' );
        static_assert(*bv.cbegin()     == 'H' );
        static_assert(*(bv.cend() - 1) == 'o' );
        static_assert(*bv.rbegin()     == 'o' );
        static_assert(*bv.crbegin()    == 'o' );

        static_assert(bv.rbegin()  != bv.rend());
        static_assert(bv.crbegin() != bv.crend());

        constexpr QByteArrayView bv2(bv.data(), bv.data() + bv.size());
        static_assert(!bv2.isNull());
        static_assert(!bv2.empty());
        static_assert(bv2.size() == 5);
    }
    {
        static constexpr char hello[] = "Hello";
        constexpr QByteArrayView bv(hello);
        static_assert(bv.size() == 5);
        static_assert(!bv.empty());
        static_assert(!bv.isEmpty());
        static_assert(!bv.isNull());
        static_assert(*bv.data() == 'H');
        static_assert(bv[0]      == 'H');
        static_assert(bv.at(0)   == 'H');
        static_assert(bv.front() == 'H');
        static_assert(bv.first() == 'H');
        static_assert(bv[4]      == 'o');
        static_assert(bv.at(4)   == 'o');
        static_assert(bv.back()  == 'o');
        static_assert(bv.last()  == 'o');
    }
    {
        static constexpr char hello[] = { 'H', 'e', 'l', 'l', 'o' };
        constexpr QByteArrayView bv(hello, std::size(hello));
        static_assert(bv.size() == 5);
        static_assert(!bv.empty());
        static_assert(!bv.isEmpty());
        static_assert(!bv.isNull());
        static_assert(*bv.data() == 'H');
        static_assert(bv[0]      == 'H');
        static_assert(bv.at(0)   == 'H');
        static_assert(bv.front() == 'H');
        static_assert(bv.first() == 'H');
        static_assert(bv[4]      == 'o');
        static_assert(bv.at(4)   == 'o');
        static_assert(bv.back()  == 'o');
        static_assert(bv.last()  == 'o');
    }
    {
        constexpr char *null = nullptr;
        constexpr QByteArrayView bv(null);
        static_assert(bv.isNull());
        static_assert(bv.isEmpty());
        static_assert(bv.size() == 0);
    }
}

void tst_QByteArrayView::basics() const {
    QByteArrayView bv1;
    QVERIFY(bv1.isNull());
    QVERIFY(bv1.isEmpty());

    QByteArrayView bv2;
    QVERIFY(bv2 == bv1);
    QVERIFY(!(bv2 != bv1));
}

void tst_QByteArrayView::literals() const {
    const char hello[] = "Hello\0This shouldn't be found";

    QCOMPARE(QByteArrayView(hello).size(), 5);
    QCOMPARE(QByteArrayView(hello + 0).size(), 5);
    QByteArrayView bv = hello;
    QCOMPARE(bv.size(), 5);
    QVERIFY(!bv.empty());
    QVERIFY(!bv.isEmpty());
    QVERIFY(!bv.isNull());

    QCOMPARE(*bv.data(), 'H');
    QCOMPARE(bv[0],      'H');
    QCOMPARE(bv.at(0),   'H');
    QCOMPARE(bv.front(), 'H');
    QCOMPARE(bv.first(), 'H');
    QCOMPARE(bv[4],      'o');
    QCOMPARE(bv.at(4),   'o');
    QCOMPARE(bv.back(),  'o');
    QCOMPARE(bv.last(),  'o');

    QByteArrayView bv2(bv.data(), bv.data() + bv.size());
    QVERIFY(!bv2.isNull());
    QVERIFY(!bv2.empty());
    QCOMPARE(bv2.size(), 5);

    const char abc[] = "abv";
    bv = abc;
    QCOMPARE(bv.size(), 3);

    const char def[3] = { 'd', 'e', 'f' };
    bv = def;
    QCOMPARE(bv.size(), 3);
}

void tst_QByteArrayView::fromArray() const {
    //普通C风格字符串末尾会补\0
    static constexpr char hello[] = "Hello\0abc\0\0.";
    //initializeList初始化，末尾没有自动补\0
    static constexpr char b[] = {'a'};
    int a[] = {1, 2, 3};

    int sizeB = sizeof(b);
    int sizeA = sizeof(a) / sizeof(int);
    int size = sizeof(hello) / sizeof(char);

    constexpr QByteArrayView bv = QByteArrayView::fromArray(hello);
    //TODO
    QCOMPARE(bv.size(), 13);
    QVERIFY(!bv.empty());
    QVERIFY(!bv.isEmpty());
    QVERIFY(!bv.isNull());
    QCOMPARE(*bv.data(), 'H');
    QCOMPARE(bv[0],      'H');
    QCOMPARE(bv.at(0),   'H');
    QCOMPARE(bv.front(), 'H');
    QCOMPARE(bv.first(), 'H');
    QCOMPARE(bv[4],      'o');
    QCOMPARE(bv.at(4),   'o');
    QCOMPARE(bv[5],      '\0');
    QCOMPARE(bv.at(5),   '\0');
    //最后一位补了\0，所以'.'变成了倒数第二位
    QCOMPARE(*(bv.data() + bv.size() - 2),  '.');
    QCOMPARE(bv.back(),  '\0');
    QCOMPARE(bv.last(),  '\0');

    const std::byte bytes[] = {std::byte(0x0), std::byte(0x1), std::byte(0x2)};
    QByteArrayView bbv = QByteArrayView::fromArray(bytes);
    QCOMPARE(bbv.data(), reinterpret_cast<const char *>(bytes + 0));
    QCOMPARE(bbv.size(), 3);
    QCOMPARE(bbv.first(), 0x0);
    QCOMPARE(bbv.last(), 0x2);
}

void tst_QByteArrayView::literalsWithInternalNulls() const {
    const char withnull[] = "a\0zzz";

    QCOMPARE(size_t(QByteArrayView::fromArray(withnull).size()), std::size(withnull));
    //QByteArrayView(Pointer)使用\0当作结束
    QCOMPARE(QByteArrayView(withnull + 0).size(), 1);

    //fromArray为模板函数，的size为数组的size
    QByteArrayView nulled = QByteArrayView::fromArray(withnull);
    QCOMPARE(nulled.last(), '\0');
    nulled.chop(1);  //把末尾的\0截掉
    QCOMPARE(nulled[1], '\0');
    QCOMPARE(nulled.indexOf('\0'), 1);
    QCOMPARE(nulled.indexOf('z'), 2);
    QCOMPARE(nulled.lastIndexOf('z'), 4);
    QCOMPARE(nulled.lastIndexOf('a'), 0);

    //C字符串会创建QByteArrayView,后面的z和y会被嘎掉
    QVERIFY(nulled.startsWith("a\0z"));
    QVERIFY(nulled.startsWith("a\0y"));  //TODO

    QVERIFY(!nulled.startsWith(QByteArrayView("a\0y", 3)));

    QVERIFY(nulled.endsWith("zz"));
    QVERIFY(nulled.contains("z"));

    QVERIFY(nulled.contains(QByteArrayView("\0z", 2)));
    QVERIFY(!nulled.contains(QByteArrayView("\0y", 2)));

    QCOMPARE(nulled.first(5), QByteArrayView(withnull, 5));
    QCOMPARE(nulled.last(5), QByteArrayView(withnull, 5));
    QCOMPARE(nulled.sliced(0), QByteArrayView(withnull, 5));

    QCOMPARE(nulled.sliced(2, 2), "zz");
    QCOMPARE(nulled.chopped(2), QByteArrayView("a\0z", 3));
    QVERIFY(nulled.chopped(2) != QByteArrayView("a\0y", 3));
    QCOMPARE(nulled.count('z'), 3);

    const char nullfirst[] = "\0buzz";
    QByteArrayView fromnull = QByteArrayView::fromArray(nullfirst);
    QVERIFY(!fromnull.isEmpty());

    const char nullNotEnd[] = { 'b', 'o', 'w', '\0', 'a', 'f', 't', 'z' };
    QByteArrayView midNull = QByteArrayView::fromArray(nullNotEnd);
    QCOMPARE(midNull.back(), 'z');
}

void tst_QByteArrayView::at() const
{
    QByteArray hello("Hello");
    QByteArrayView bv(hello);
    QCOMPARE(bv.at(0), 'H'); QCOMPARE(bv[0], 'H');
    QCOMPARE(bv.at(1), 'e'); QCOMPARE(bv[1], 'e');
    QCOMPARE(bv.at(2), 'l'); QCOMPARE(bv[2], 'l');
    QCOMPARE(bv.at(3), 'l'); QCOMPARE(bv[3], 'l');
    QCOMPARE(bv.at(4), 'o'); QCOMPARE(bv[4], 'o');
}

void tst_QByteArrayView::fromQByteArray() const
{
    QByteArray null;
    QByteArray empty = "";

    QVERIFY(QByteArrayView(null).isNull());
    //zhaoyujie TODO Qt 6.0之前默认构造函数给了Default值，Qt6.0之后直接是空的
//    QVERIFY(!qToByteArrayViewIgnoringNull(null).isNull());

    QVERIFY(QByteArrayView(null).isEmpty());
    QVERIFY(qToByteArrayViewIgnoringNull(null).isEmpty());

    QVERIFY(QByteArrayView(empty).isEmpty());
    QVERIFY(qToByteArrayViewIgnoringNull(empty).isEmpty());

    QVERIFY(!QByteArrayView(empty).isNull());
    QVERIFY(!qToByteArrayViewIgnoringNull(empty).isNull());

    conversionTests(QByteArray("Hello World!"));
}

namespace help {
    template <typename T>
    size_t size(const T &t) { return size_t(t.size()); }
    //zhaoyujie 这里是什么意思？
    template <typename T>
    size_t size(const T *t) { return std::char_traits<T>::length(t); }

    template <typename T>
    decltype(auto)             cbegin(const T &t) { return t.begin(); }
    template <typename T>
    const T *                  cbegin(const T *t) { return t; }

    template <typename T>
    decltype(auto)             cend(const T &t) { return t.end(); }
    template <typename T>
    const T *                  cend(const T *t) { return t + size(t); }

    template <typename T>
    decltype(auto)                     crbegin(const T &t) { return t.rbegin(); }
    template <typename T>
    std::reverse_iterator<const T*>    crbegin(const T *t) { return std::reverse_iterator<const T*>(cend(t)); }

    template <typename T>
    decltype(auto)                     crend(const T &t) { return t.rend(); }
    template <typename T>
    std::reverse_iterator<const T*>    crend(const T *t) { return std::reverse_iterator<const T*>(cbegin(t)); }

} // namespace help


template <typename Data>
void tst_QByteArrayView::conversionTests(Data data) const
{
    {
        QByteArrayView bv = data;

        QCOMPARE(help::size(bv), help::size(data));

        const auto compare = [](auto v1, auto v2) {
            if constexpr (std::is_same_v<decltype(v1), std::byte>)
                return std::to_integer<char>(v1) == v2;
            else
                return v1 == v2;
        };

        QVERIFY(std::equal(help::cbegin(data), help::cend(data),
                           QT_MAKE_CHECKED_ARRAY_ITERATOR(bv.cbegin(), bv.size()), compare));
        QVERIFY(std::equal(help::cbegin(data), help::cend(data),
                           QT_MAKE_CHECKED_ARRAY_ITERATOR(bv.begin(), bv.size()), compare));
        QVERIFY(std::equal(help::crbegin(data), help::crend(data), bv.crbegin(), compare));
        QVERIFY(std::equal(help::crbegin(data), help::crend(data), bv.rbegin(), compare));
        QCOMPARE(bv, data);
    }
}

template <typename Char>
void tst_QByteArrayView::fromEmptyLiteral() const
{
    const Char *null = nullptr;
    const Char empty[] = { Char{0} };

    QCOMPARE(QByteArrayView(null).size(), 0);
    QCOMPARE(QByteArrayView(null).data(), nullptr);
    QCOMPARE(QByteArrayView::fromArray(empty).size(), 1);
    QCOMPARE(static_cast<const void *>(QByteArrayView::fromArray(empty).data()),
             static_cast<const void *>(empty));
    QVERIFY(QByteArrayView(null).isNull());
    QVERIFY(QByteArrayView(null).isEmpty());
    QVERIFY(!QByteArrayView::fromArray(empty).isEmpty());
    QVERIFY(!QByteArrayView::fromArray(empty).isNull());
}

void tst_QByteArrayView::fromCharStar() const
{
    fromEmptyLiteral<char>();
    conversionTests("Hello, World!");
}

void tst_QByteArrayView::fromUCharStar() const
{
    fromEmptyLiteral<uchar>();

    const uchar data[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', 0 };
    conversionTests(data);
}

void tst_QByteArrayView::fromSignedCharStar() const
{
    fromEmptyLiteral<signed char>();

    const signed char data[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', 0 };
    conversionTests(data);
}

void tst_QByteArrayView::fromStdByteArray() const
{
    fromEmptyLiteral<std::byte>();

    const std::byte data[] = {std::byte{'H'}, std::byte{'e'}, std::byte{'l'}, std::byte{'l'},
                              std::byte{'o'}, std::byte{0}};
    conversionTests(data);
}

template <typename Char>
void tst_QByteArrayView::fromRange(const Char *first, const Char *last) const
{
    const Char *null = nullptr;
    QCOMPARE(QByteArrayView(null, null).size(), 0);
    QCOMPARE(QByteArrayView(null, null).data(), nullptr);
    QCOMPARE(QByteArrayView(first, first).size(), 0);
    QCOMPARE(static_cast<const void*>(QByteArrayView(first, first).data()),
             static_cast<const void*>(first));

    const auto bv = QByteArrayView(first, last);
    QCOMPARE(bv.size(), last - first);
    QCOMPARE(static_cast<const void*>(bv.data()),
             static_cast<const void*>(first));

    QCOMPARE(static_cast<const void*>(bv.last(0).data()),
             static_cast<const void*>(last));
    QCOMPARE(static_cast<const void*>(bv.sliced(bv.size()).data()),
             static_cast<const void*>(last));
}

void tst_QByteArrayView::fromCharRange() const
{
    const char data[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
    fromRange(std::begin(data), std::end(data));
}

void tst_QByteArrayView::fromUCharRange() const
{
    const uchar data[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', 0 };
    fromRange(std::begin(data), std::end(data));
}

void tst_QByteArrayView::fromSignedCharRange() const
{
    const signed char data[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', 0 };
    fromRange(std::begin(data), std::end(data));
}

void tst_QByteArrayView::fromStdByteRange() const
{
    const std::byte data[] = {std::byte{'H'}, std::byte{'e'}, std::byte{'l'}, std::byte{'l'},
                              std::byte{'o'}, std::byte{0}};
    fromRange(std::begin(data), std::end(data));
}

template <typename Char, typename Container>
void tst_QByteArrayView::fromContainer() const
{
    const QByteArray s = "Hello World";

    Container c;
    QVERIFY(QByteArrayView(c).isEmpty());

    QCOMPARE(sizeof(Char), sizeof(char));

    //std::back_inserter，在末尾插入
    const auto *data = reinterpret_cast<const Char *>(s.data());
    std::copy(data, data + s.size(), std::back_inserter(c));
    conversionTests(std::move(c));
}

template <typename Char>
void tst_QByteArrayView::fromContainers() const
{
//    fromContainer<Char, QVector<Char>>();
//    fromContainer<Char, QVarLengthArray<Char>>();
    fromContainer<Char, std::vector<Char>>();
    fromContainer<Char, std::basic_string<Char>>();
}

void tst_QByteArrayView::fromCharContainers() const
{
    fromContainers<char>();
}

void tst_QByteArrayView::fromUCharContainers() const
{
    fromContainers<uchar>();
}

void tst_QByteArrayView::fromSignedCharContainers() const
{
    fromContainers<unsigned char>();
}

void tst_QByteArrayView::fromStdByteContainers() const
{
    fromContainers<std::byte>();
}

void tst_QByteArrayView::comparison() const
{
    const QByteArrayView aa = "aa";
    const QByteArrayView bb = "bb";

    QVERIFY(aa == aa);
    QVERIFY(aa != bb);
    QVERIFY(aa < bb);
    QVERIFY(bb > aa);
}

void tst_QByteArrayView::compare() const
{
    QByteArrayView alpha = "original";

    QVERIFY(alpha.compare("original", Qt::CaseSensitive) == 0);
    QVERIFY(alpha.compare("Original", Qt::CaseSensitive) > 0);
    QVERIFY(alpha.compare("Original", Qt::CaseInsensitive) == 0);
    QByteArrayView beta = "unoriginal";
    QVERIFY(alpha.compare(beta, Qt::CaseInsensitive) < 0);
    beta = "Unoriginal";
    QVERIFY(alpha.compare(beta, Qt::CaseInsensitive) < 0);
    QVERIFY(alpha.compare(beta, Qt::CaseSensitive) > 0);
}

QTEST_APPLESS_MAIN(tst_QByteArrayView)