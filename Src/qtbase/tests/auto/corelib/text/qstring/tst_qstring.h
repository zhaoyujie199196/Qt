//
// Created by Yujie Zhao on 2023/7/11.
//

#ifndef TST_QSTRING_H
#define TST_QSTRING_H

#include <QtCore/QString>
#include <QTest>
#include <QObject>
#include <QByteArray>

namespace {

    template <typename T>
    class Arg;

    template <typename T>
    class Reversed {};

    class ArgBase {
    protected:
        QString pinned;
        explicit ArgBase(const char *str)
            : pinned(QString::fromLatin1(str))
        {}
    };

    template<>
    class Arg<QChar> : protected ArgBase
    {
    public:
        explicit Arg(const char *str) : ArgBase(str) {}

        template <typename MemFunc>
        void apply0(QString &s, MemFunc mf) const {
            for (QChar ch : qAsConst(this->pinned)) {
                (s.*mf)(ch);
            }
        }

        template <typename MemFunc, typename A1>
        void apply1(QString &s, MemFunc mf, A1 a1) const {
            for (QChar ch : qAsConst(this->pinned)) {
                (s.*mf)(a1, ch);
            }
        }
    };

    template <>
    class Arg<Reversed<QChar>> : private Arg<QChar>
    {
    public:
        explicit Arg(const char *str)
            : Arg<QChar>(str)
        {
            std::reverse(this->pinned.begin(), this->pinned.end());
        }
        using Arg<QChar>::apply0;
        using Arg<QChar>::apply1;
    };

    template <>
    class Arg<QString> : ArgBase{
    public:
        explicit Arg(const char *str)
            : ArgBase(str)
        {}
        template <typename MemFun>
        void apply0(QString &s, MemFun mf) const {
            (s.*mf)(this->pinned);
        }
        template <typename MemFunc, typename A1>
        void apply1(QString &s, MemFunc mf, A1 a1) const {
            (s.*mf)(a1, this->pinned);
        }
    };

    template <>
    class Arg<QStringView> : ArgBase
    {
        QStringView view() const {
            return this->pinned.isNull() ? QStringView() : QStringView(this->pinned);
        }
    public:
        explicit Arg(const char *str) : ArgBase(str) {}

        template <typename MemFun>
        void apply0(QString &s, MemFun mf) const
        { (s.*mf)(view()); }
        template <typename MemFun, typename A1>
        void apply1(QString &s, MemFun mf, A1 a1) const {
            (s.*mf)(a1, view());
        }
    };

    template <>
    class Arg<QPair<const QChar *, int> > : ArgBase
    {
    public:
        explicit Arg(const char *str) : ArgBase(str) {}

        template <typename MemFun>
        void apply0(QString &s, MemFun mf) const {
            (s.*mf)(this->pinned.constData(), this->pinned.length());
        }
        template <typename MemFun, typename A1>
        void apply1(QString &s, MemFun mf, A1 a1) const {
            (s.*mf)(a1, this->pinned.constData(), this->pinned.length());
        }
    };

    template <>
    class Arg<QLatin1String> {
        QLatin1String l1;
    public:
        explicit Arg(const char *str) : l1(str) {
        }

        template <typename MemFun>
        void apply0(QString &s, MemFun mf) const {
            (s.*mf)(l1);
        }

        template <typename MemFun, typename A1>
        void apply1(QString &s, MemFun mf, A1 a1) const {
            (s.*mf)(a1, l1);
        }
    };

    template <>
    class Arg<char> {
    protected:
        const char *str;
    public:
        explicit Arg(const char *str) : str(str) {}

        template <typename MemFun>
        void apply0(QString &s, MemFun mf) const {
            if (str) {
                for (const char *it = str; *it; ++it) {
                    (s.*mf)(*it);
                }
            }
        }

        template <typename MemFun, typename A1>
        void apply1(QString &s, MemFun mf, A1 a1) const {
            if (str) {
                for (const char *it = str; *it; ++it) {
                    (s.*mf)(a1, *it);
                }
            }
        }
    };

    template <>
    class Arg<Reversed<char>> : private Arg<char> {
        static const char *dupAndReverse(const char *s) {
            char *s2 = qstrdup(s);
            std::reverse(s2, s2 + qstrlen(s2));
            return s2;
        }

    public:
        explicit Arg(const char *str) : Arg<char>(dupAndReverse(str)) {
        }
        ~Arg() {
            delete[] str;
        }
        using Arg<char>::apply0;
        using Arg<char>::apply1;
    };

    template <>
    class Arg<const char *> {
        const char *str;
    public:
        explicit Arg(const char *str) : str(str) {}

        template <typename MemFun>
        void apply0(QString &s, MemFun mf) const {
            (s.*mf)(str);
        }
        template <typename MemFun, typename A1>
        void apply1(QString &s, MemFun mf, A1 a1) const {
            (s.*mf)(a1, str);
        }
    };

    template <>
    class Arg<QByteArray> {
        QByteArray ba;
    public:
        explicit Arg(const char *str) : ba(str) {}

        template <typename MemFun>
        void apply0(QString &s, MemFun mf) const {
            (s.*mf)(ba);
        }
        template <typename MemFun, typename A1>
        void apply1(QString &s, MemFun mf, A1 a1) const {
            (s.*mf)(a1, ba);
        }
    };


    class CharStarContainer {
    public:
        explicit constexpr CharStarContainer(const char *s = nullptr) : str(s) {}

        constexpr operator const char *() const { return str; }

    private:
        const char *str;
    };
}
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(CharStarContainer, Q_PRIMITIVE_TYPE);
QT_END_NAMESPACE

template <typename ArgType, typename MemFunc>
static void do_apply0(MemFunc mf)
{
    QFETCH(QString, s);
    QFETCH(CharStarContainer, arg);
    QFETCH(QString, expected);

    Arg<ArgType>(arg).apply0(s, mf);
    QCOMPARE(s, expected);
    QCOMPARE(s.isEmpty(), expected.isEmpty());
    QCOMPARE(s.isNull(), expected.isNull());
}

template <typename ArgType, typename A1, typename MemFunc>
static void do_apply1(MemFunc mf)
{
    QFETCH(QString, s);
    QFETCH(CharStarContainer, arg);
    QFETCH(A1, a1);
    QFETCH(QString, expected);

    Arg<ArgType>(arg).apply1(s, mf, a1);

    QCOMPARE(s, expected);
    QCOMPARE(s.isEmpty(), expected.isEmpty());
    QCOMPARE(s.isNull(), expected.isNull());
}

class tst_QString : public QObject
{
public:
    void registerInvokeMethods() override
    {
        REGISTER_OBJECT_INVOKE_METHOD(fromStdString)
        REGISTER_OBJECT_INVOKE_METHOD(toStdString)
//        REGISTER_OBJECT_INVOKE_METHOD(check_QTextIOStream)
//        REGISTER_OBJECT_INVOKE_METHOD(check_QTextStream)
//        REGISTER_OBJECT_INVOKE_METHOD(check_QDataStream)
        REGISTER_OBJECT_INVOKE_METHOD(fromRawData)
        REGISTER_OBJECT_INVOKE_METHOD(setRawData)
        REGISTER_OBJECT_INVOKE_METHOD(setUnicode)
        REGISTER_OBJECT_INVOKE_METHOD(endsWith)
        REGISTER_OBJECT_INVOKE_METHOD(startsWith)
//        REGISTER_OBJECT_INVOKE_METHOD(setNum)
//        REGISTER_OBJECT_INVOKE_METHOD(toDouble_data)
//        REGISTER_OBJECT_INVOKE_METHOD(toDouble)
//        REGISTER_OBJECT_INVOKE_METHOD(toFloat);
//        REGISTER_OBJECT_INVOKE_METHOD(toLong_data);
//        REGISTER_OBJECT_INVOKE_METHOD(toLong);
//        REGISTER_OBJECT_INVOKE_METHOD(toULong_data);
//        REGISTER_OBJECT_INVOKE_METHOD(toULong);
//        REGISTER_OBJECT_INVOKE_METHOD(toLongLong);
//        REGISTER_OBJECT_INVOKE_METHOD(toULongLong);
//        REGISTER_OBJECT_INVOKE_METHOD(toUInt);
//        REGISTER_OBJECT_INVOKE_METHOD(toInt);
//        REGISTER_OBJECT_INVOKE_METHOD(toShort);
//        REGISTER_OBJECT_INVOKE_METHOD(toUShort);
        REGISTER_OBJECT_INVOKE_METHOD(replace_qchar_qchar_data)
        REGISTER_OBJECT_INVOKE_METHOD(replace_qchar_qchar)
        REGISTER_OBJECT_INVOKE_METHOD(replace_qchar_qstring_data)
        REGISTER_OBJECT_INVOKE_METHOD(replace_qchar_qstring)
        REGISTER_OBJECT_INVOKE_METHOD(replace_uint_uint_data)
        REGISTER_OBJECT_INVOKE_METHOD(replace_uint_uint)
        REGISTER_OBJECT_INVOKE_METHOD(replace_uint_uint_extra)
        REGISTER_OBJECT_INVOKE_METHOD(replace_extra)
        REGISTER_OBJECT_INVOKE_METHOD(replace_string_data)
        REGISTER_OBJECT_INVOKE_METHOD(replace_string)
        REGISTER_OBJECT_INVOKE_METHOD(replace_string_extra)
        REGISTER_OBJECT_INVOKE_METHOD(remove_uint_uint_data)
        REGISTER_OBJECT_INVOKE_METHOD(remove_uint_uint)
        REGISTER_OBJECT_INVOKE_METHOD(remove_string_data)
        REGISTER_OBJECT_INVOKE_METHOD(remove_string)
        REGISTER_OBJECT_INVOKE_METHOD(remove_extra)
        REGISTER_OBJECT_INVOKE_METHOD(swap)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qstring)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qstring_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qstringview)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qstringview_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qlatin1string)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qlatin1string_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qcharstar_int)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qcharstar_int_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qchar)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qchar_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qbytearray)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_qbytearray_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_char)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_char_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_charstar)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_charstar_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_bytearray_special_cases_data)
        REGISTER_OBJECT_INVOKE_METHOD(prepend_bytearray_special_cases)

        REGISTER_OBJECT_INVOKE_METHOD(append_qstring)
        REGISTER_OBJECT_INVOKE_METHOD(append_qstring_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_qstringview)
        REGISTER_OBJECT_INVOKE_METHOD(append_qstringview_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_qlatin1string)
        REGISTER_OBJECT_INVOKE_METHOD(append_qlatin1string_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_qcharstar_int)
        REGISTER_OBJECT_INVOKE_METHOD(append_qcharstar_int_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_qchar)
        REGISTER_OBJECT_INVOKE_METHOD(append_qchar_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_qbytearray)
        REGISTER_OBJECT_INVOKE_METHOD(append_qbytearray_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_char)
        REGISTER_OBJECT_INVOKE_METHOD(append_char_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_charstar)
        REGISTER_OBJECT_INVOKE_METHOD(append_charstar_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_special_cases)
        REGISTER_OBJECT_INVOKE_METHOD(append_bytearray_special_cases_data)
        REGISTER_OBJECT_INVOKE_METHOD(append_bytearray_special_cases)

        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qstring)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qstring_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qstringview)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qstringview_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qlatin1string)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qlatin1string_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qchar)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qchar_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qbytearray)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_qbytearray_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_charstar)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_charstar_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_special_cases)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_bytearray_special_cases_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_pluseq_bytearray_special_cases)

        REGISTER_OBJECT_INVOKE_METHOD(operator_eqeq_bytearray_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_eqeq_bytearray)
        REGISTER_OBJECT_INVOKE_METHOD(operator_eqeq_nullstring)
        REGISTER_OBJECT_INVOKE_METHOD(operator_smaller)

        REGISTER_OBJECT_INVOKE_METHOD(insert_qstring)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qstring_data)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qstringview)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qstringview_data)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qlatin1string)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qlatin1string_data)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qcharstar_int)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qcharstar_int_data)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qchar)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qchar_data)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qbytearray)
        REGISTER_OBJECT_INVOKE_METHOD(insert_qbytearray_data)
        REGISTER_OBJECT_INVOKE_METHOD(insert_char)
        REGISTER_OBJECT_INVOKE_METHOD(insert_char_data)
        REGISTER_OBJECT_INVOKE_METHOD(insert_charstar)
        REGISTER_OBJECT_INVOKE_METHOD(insert_charstar_data)
        REGISTER_OBJECT_INVOKE_METHOD(insert_special_cases)

        REGISTER_OBJECT_INVOKE_METHOD(simplified_data)
        REGISTER_OBJECT_INVOKE_METHOD(simplified)
        REGISTER_OBJECT_INVOKE_METHOD(trimmed)
        REGISTER_OBJECT_INVOKE_METHOD(unicodeTableAccess_data)
        REGISTER_OBJECT_INVOKE_METHOD(unicodeTableAccess)
        REGISTER_OBJECT_INVOKE_METHOD(toUpper)
        REGISTER_OBJECT_INVOKE_METHOD(toLower)

        REGISTER_OBJECT_INVOKE_METHOD(isLower_isUpper_data)
        REGISTER_OBJECT_INVOKE_METHOD(isLower_isUpper)
        REGISTER_OBJECT_INVOKE_METHOD(toCaseFolded)
        REGISTER_OBJECT_INVOKE_METHOD(rightJustified)
        REGISTER_OBJECT_INVOKE_METHOD(leftJustified)

        REGISTER_OBJECT_INVOKE_METHOD(mid)
        REGISTER_OBJECT_INVOKE_METHOD(right)
        REGISTER_OBJECT_INVOKE_METHOD(left)
        REGISTER_OBJECT_INVOKE_METHOD(contains)
        REGISTER_OBJECT_INVOKE_METHOD(lastIndexOf_data)
        REGISTER_OBJECT_INVOKE_METHOD(lastIndexOf)
        REGISTER_OBJECT_INVOKE_METHOD(indexOf_data)
        REGISTER_OBJECT_INVOKE_METHOD(indexOf)
        REGISTER_OBJECT_INVOKE_METHOD(indexOf2_data)
        REGISTER_OBJECT_INVOKE_METHOD(indexOf2)
        REGISTER_OBJECT_INVOKE_METHOD(asprintf)
        REGISTER_OBJECT_INVOKE_METHOD(asprintfS)

        //TODO
        REGISTER_OBJECT_INVOKE_METHOD(constructorQByteArray_data)
    }

    void fromStdString();
    void toStdString();
//    void check_QTextIOStream();
//    void check_QTextStream();
//    void check_QDataStream();
    void fromRawData();
    void setRawData();
    void setUnicode();
    void endsWith();
    void startsWith();
//    void setNum();
//    void toDouble_data();
//    void toDouble();
//    void toFloat();
//    void toLong_data();
//    void toLong();
//    void toULong_data();
//    void toULong();
//    void toLongLong();
//    void toULongLong();
//    void toUInt();
//    void toInt();
//    void toShort();
//    void toUShort();
    void replace_qchar_qchar_data();
    void replace_qchar_qchar();
    void replace_qchar_qstring_data();
    void replace_qchar_qstring();
    void replace_uint_uint_data();
    void replace_uint_uint();
    void replace_uint_uint_extra();
    void replace_extra();
    void replace_string_data();
    void replace_string();
    void replace_string_extra();
//    void replace_regexp_data();
//    void replace_regexp();
//    void replace_regexp_extra();
    void remove_uint_uint_data();
    void remove_uint_uint();
    void remove_string_data();
    void remove_string();
//    void remove_regexp_data();
//    void remove_regexp();
    void remove_extra();
    void swap();

    //QString &(QString::*)(QStringView)>(); 这种创建一个对象为了调用到prepend_impl的模板
    void prepend_qstring() { prepend_impl<QString>(); }
    void prepend_qstring_data() { prepend_data(true); }
    void prepend_qstringview() { prepend_impl<QStringView, QString &(QString::*)(QStringView)>(); }
    void prepend_qstringview_data() { prepend_data(true); }
    void prepend_qlatin1string() { prepend_impl<QLatin1String, QString&(QString::*)(QLatin1String)>(); }
    void prepend_qlatin1string_data() { prepend_data(true); }
    void prepend_qcharstar_int() { prepend_impl<QPair<const QChar *, int>, QString &(QString::*)(const QChar *, qsizetype)>(); }
    void prepend_qcharstar_int_data() { prepend_data(true); }
    void prepend_qchar() { prepend_impl<Reversed<QChar>, QString &(QString::*)(QChar)>(); }
    void prepend_qchar_data() { prepend_data(true); }
    void prepend_qbytearray()         { prepend_impl<QByteArray>(); }
    void prepend_qbytearray_data()    { prepend_data(true); }
    void prepend_char()               { prepend_impl<Reversed<char>, QString &(QString::*)(QChar)>(); }
    void prepend_char_data()          { prepend_data(true); }
    void prepend_charstar()           { prepend_impl<const char *, QString &(QString::*)(const char *)>(); }
    void prepend_charstar_data()      { prepend_data(true); }
    void prepend_bytearray_special_cases_data();
    void prepend_bytearray_special_cases();

    void append_qstring()            { append_impl<QString>(); }
    void append_qstring_data()       { append_data(); }
    void append_qstringview()        { append_impl<QStringView,  QString &(QString::*)(QStringView)>(); }
    void append_qstringview_data()   { append_data(true); }
    void append_qlatin1string()      { append_impl<QLatin1String, QString &(QString::*)(QLatin1String)>(); }
    void append_qlatin1string_data() { append_data(); }
    void append_qcharstar_int()      { append_impl<QPair<const QChar *, int>, QString&(QString::*)(const QChar *, qsizetype)>(); }
    void append_qcharstar_int_data() { append_data(true); }
    void append_qchar()              { append_impl<QChar, QString &(QString::*)(QChar)>(); }
    void append_qchar_data()         { append_data(true); }
    void append_qbytearray()         { append_impl<QByteArray>(); }
    void append_qbytearray_data()    { append_data(); }
    void append_char()               { append_impl<char, QString &(QString::*)(QChar)>(); }
    void append_char_data()          { append_data(true); }
    void append_charstar()           { append_impl<const char *, QString &(QString::*)(const char *)>(); }
    void append_charstar_data()      { append_data(); }
    void append_special_cases();
    void append_bytearray_special_cases_data();
    void append_bytearray_special_cases();

    void operator_pluseq_qstring()            { operator_pluseq_impl<QString>(); }
    void operator_pluseq_qstring_data()       { operator_pluseq_data(); }
    void operator_pluseq_qstringview()        { operator_pluseq_impl<QStringView, QString &(QString::*)(QStringView)>(); }
    void operator_pluseq_qstringview_data()   { operator_pluseq_data(true); }
    void operator_pluseq_qlatin1string()      { operator_pluseq_impl<QLatin1String, QString &(QString::*)(QLatin1String)>(); }
    void operator_pluseq_qlatin1string_data() { operator_pluseq_data(); }
    void operator_pluseq_qchar()              { operator_pluseq_impl<QChar, QString &(QString::*)(QChar)>(); }
    void operator_pluseq_qchar_data()         { operator_pluseq_data(true); }
    void operator_pluseq_qbytearray()         { operator_pluseq_impl<QByteArray>(); }
    void operator_pluseq_qbytearray_data()    { operator_pluseq_data(); }
    void operator_pluseq_charstar()           { operator_pluseq_impl<const char *, QString &(QString::*)(const char *)>(); }
    void operator_pluseq_charstar_data()      { operator_pluseq_data(); }
    void operator_pluseq_special_cases();
    void operator_pluseq_bytearray_special_cases_data();
    void operator_pluseq_bytearray_special_cases();

    void operator_eqeq_bytearray_data();
    void operator_eqeq_bytearray();
    void operator_eqeq_nullstring();
    void operator_smaller();

    void insert_qstring() { insert_impl<QString>(); }
    void insert_qstring_data() { insert_data(true); }
    void insert_qstringview() { insert_impl<QStringView, QString &(QString::*)(qsizetype, QStringView)>(); }
    void insert_qstringview_data() { insert_data(true); }
    void insert_qlatin1string() { insert_impl<QLatin1String, QString &(QString::*)(qsizetype, QLatin1String)>(); }
    void insert_qlatin1string_data() { insert_data(true); }
    void insert_qcharstar_int() { insert_impl<QPair<const QChar *, int>, QString &(QString::*)(qsizetype, const QChar *, qsizetype)>(); }
    void insert_qcharstar_int_data() { insert_data(true); }
    void insert_qchar() { insert_impl<Reversed<QChar>, QString &(QString::*)(qsizetype, QChar)>(); }
    void insert_qchar_data() { insert_data(true); }
    void insert_qbytearray() { insert_impl<QByteArray>(); }
    void insert_qbytearray_data() { insert_data(true); }
    void insert_char() { insert_impl<Reversed<char>, QString &(QString::*)(qsizetype, QChar)>(); }
    void insert_char_data() { insert_data(true); }
    void insert_charstar() { insert_impl<const char *, QString &(QString::*)(qsizetype, const char *)>(); }
    void insert_charstar_data() { insert_data(true); };
    void insert_special_cases();

    void simplified_data();
    void simplified();
    void trimmed();
    void unicodeTableAccess_data();
    void unicodeTableAccess();
    void toUpper();
    void toLower();
    void isLower_isUpper_data();
    void isLower_isUpper();
    void toCaseFolded();
    void rightJustified();
    void leftJustified();

    void mid();
    void right();
    void left();
    void contains();
    void count();
    void lastIndexOf_data();
    void lastIndexOf();
    void indexOf_data();
    void indexOf();
//    void indexOfInvalidRegex();
//    void lastIndexOfInvalidRegex();
    void indexOf2_data();
    void indexOf2();
//    void indexOf3_data();
//  void indexOf3();
    void asprintf();
    void asprintfS();

    void constructorQByteArray_data();

private:
    void prepend_data(bool emptyIsNoop = false);
    template <typename ArgType, typename MemFun>
    void prepend_impl() const { do_apply0<ArgType>(MemFun(&QString::prepend)); }
    template <typename ArgType>
    void prepend_impl() const { prepend_impl<ArgType, QString &(QString::*)(const ArgType &)>(); }

    template <typename ArgType, typename MemFun>
    void append_impl() const { do_apply0<ArgType>(MemFun(&QString::append)); }
    template <typename ArgType>
    void append_impl() const { append_impl<ArgType, QString &(QString::*)(const ArgType&)>(); }
    void append_data(bool emptyIsNoop = false);

    template <typename ArgType, typename MemFun>
    void operator_pluseq_impl() const { do_apply0<ArgType>(MemFun(&QString::operator+=)); }
    template <typename ArgType>
    void operator_pluseq_impl() const { operator_pluseq_impl<ArgType, QString &(QString::*)(const ArgType&)>(); }
    void operator_pluseq_data(bool emptyIsNoop = false);

    template <typename ArgType, typename MemFun>
    void insert_impl() const { do_apply1<ArgType, int>(MemFun(&QString::insert)); }
    template <typename ArgType>
    void insert_impl() const { insert_impl<ArgType, QString &(QString::*)(qsizetype, const ArgType &)>(); }
    void insert_data(bool emptyIsNoop = false);
};

#endif //TST_QSTRING_H
