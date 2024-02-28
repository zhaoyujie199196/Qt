//
// Created by Yujie Zhao on 2023/3/1.
//

#ifndef QLATIN1STRING_H
#define QLATIN1STRING_H

#include <QtCore/qglobal.h>
#include <QtCore/qchar.h>
#include <QtCore/qtypeinfo.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qarraydata.h>
#include <string>
#include "qbytearray.h"
#include "qstringview.h"

QT_BEGIN_NAMESPACE

/*
 * latin字母的只读字符串，定位同QStringView
 * m_data依赖于外部内存管理。注意QLatin1String a("aaa")这种形式不会发生内存泄漏。"aaa"定义在数据段，与程序的生命周期相同
 * 数据短是程序运行起脚用于存储全局变量和静态变量的一块内存区域，在程序启东市被分配，结束时被释放
 * */
class QLatin1String
{
public:
    using value_type = const char;
    using reference = value_type&;
    using const_reference = reference;
    using iterator = value_type *;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;
    using difference_type = qsizetype;
    using size_type = qsizetype;

    constexpr inline QLatin1String() noexcept : m_size(0), m_data(nullptr) {}
    //const char *的构造函数是浅拷贝
    constexpr inline explicit QLatin1String(const char *s) noexcept : m_size(s ? qsizetype(strlen(s)) : 0), m_data(s) {}
    constexpr inline explicit QLatin1String(const char *s, qsizetype sz) noexcept : m_size(sz), m_data(s) {}
    constexpr inline explicit QLatin1String(const char *f, const char *l) : QLatin1String(f, qsizetype(l - f)) {}

    constexpr const char *latin1() const noexcept {return m_data;}
    constexpr qsizetype size() const noexcept {return m_size;}
    constexpr const char *data() const noexcept {return m_data;}
    constexpr bool isNull() const noexcept {return !data();}
    constexpr bool isEmpty() const noexcept {return !data();}

    //获取
    constexpr QLatin1Char at(qsizetype i) const {
        return Q_ASSERT(i >= 0), Q_ASSERT(i < m_size), QLatin1Char(m_data[i]);
    }
    constexpr QLatin1Char operator[](qsizetype i) const {
        return at(i);
    }
    constexpr QLatin1Char front() const {
        Q_ASSERT(m_data), Q_ASSERT(m_size > 0);
        return at(0);
    }
    constexpr QLatin1Char back() const {
        Q_ASSERT(m_data), Q_ASSERT(m_size > 0);
        return at(m_size - 1);
    }
    constexpr QLatin1String left(qsizetype n) const {
        if (size_t(n) >= size_t(size())) {
            n = size();
        }
        return QLatin1String(m_data, n);
    }
    constexpr QLatin1String right(qsizetype n) const {
        if (size_t(n) >= size_t(size())) {
            n = size();
        }
        return QLatin1String(m_data + size() - n, n);
    }

    constexpr QLatin1String mid(qsizetype pos) const noexcept {
        return (pos < 0 || pos >= size()) ? QLatin1String() : QLatin1String(m_data + pos, size() - pos);
    }

    constexpr QLatin1String mid(qsizetype pos, qsizetype n) const  noexcept {
        using namespace QtPrivate;
        auto result = QContainerImplHelper::mid(size(), &pos, &n);
        return result == QContainerImplHelper::Null ? QLatin1String() : QLatin1String(m_data + pos, n);
    }
    //从pos开始往后截取
    constexpr QLatin1String sliced(qsizetype pos) const {
        verify(pos);
        return QLatin1String(m_data + pos, m_size - pos);
    }
    constexpr QLatin1String sliced(qsizetype pos, qsizetype n) const {
        verify(pos, n);
        return QLatin1String(m_data + pos, n);
    }

    constexpr QLatin1String first(qsizetype n) const {
        verify(n); return QLatin1String{ m_data, n };
    }
    constexpr QLatin1String last(qsizetype n) const {
        verify(n); return QLatin1String{ m_data + size() - n, n };
    }

    //比较
    int compare(QLatin1String other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    //没有调用其他函数，可以在编译期间计算出来，所以使用constexpr
    constexpr int compare(QChar c) const noexcept;
    //调用了其他的函数，其他的函数没有使用constexpr修饰的话，此函数则不可以在编译期间算来
    int compare(QChar c, Qt::CaseSensitivity cs) const noexcept;

    //以xxx开头 / 结尾
    constexpr bool startsWith(QChar c) const noexcept;
    bool startsWith(QChar c, Qt::CaseSensitivity cs) const noexcept;
    bool startsWith(QLatin1String s, Qt::CaseSensitivity cs) const noexcept;

    qsizetype indexOf(const QStringView ss, qsizetype from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::findString(*this, from, ss, cs); }
    qsizetype indexOf(const QLatin1String &s, qsizetype from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::findString(*this, from, s, cs); }
    qsizetype indexOf(QChar c, qsizetype from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::findString(*this, from, QStringView(&c, 1), cs); }

    [[nodiscard]] qsizetype lastIndexOf(QStringView s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return lastIndexOf(s, size(), cs); }
    [[nodiscard]] qsizetype lastIndexOf(QStringView s, qsizetype from, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::lastIndexOf(*this, from, s, cs); }
    [[nodiscard]] qsizetype lastIndexOf(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return lastIndexOf(s, size(), cs); }
    [[nodiscard]] qsizetype lastIndexOf(QLatin1String s, qsizetype from, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::lastIndexOf(*this, from, s, cs); }
    [[nodiscard]] qsizetype lastIndexOf(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return lastIndexOf(c, -1, cs); }
    [[nodiscard]] qsizetype lastIndexOf(QChar c, qsizetype from, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::lastIndexOf(*this, from, QStringView(&c, 1), cs); }

    QLatin1String trimmed() const noexcept { return QtPrivate::trimmed(*this); }

    //重载比较操作符。加了friend等于operator==不是类的成员函数。如果不加friend,只能接受一个参数：operator==(QLatin1String s1)
    // QLatin1String <> QLatin1String
    friend inline bool operator==(QLatin1String s1, QLatin1String s2) noexcept {
        return (s1.size() == s2.size()) && (!s1.size() || memcmp(s1.data(), s2.data(), s1.size()) == 0);
    }
    friend inline bool operator <(QLatin1String s1, QLatin1String s2) noexcept {
        const qsizetype len = std::min(s1.size(), s2.size());
        const int r = len ? memcmp(s1.latin1(), s2.latin1(), len) : 0;
        return r < 0 || (r == 0 && s1.size() < s2.size());
    }
    friend inline bool operator!=(QLatin1String s1, QLatin1String s2) noexcept {return !(s1 == s2);}
    friend inline bool operator >(QLatin1String s1, QLatin1String s2) noexcept {return s2 < s1;}
    friend inline bool operator<=(QLatin1String s1, QLatin1String s2) noexcept {return !(s2 < s1);}
    friend inline bool operator>=(QLatin1String s1, QLatin1String s2) noexcept {return !(s1 < s2);}

    // QChar <> QLatin1String
    friend inline bool operator==(QChar lhs, QLatin1String rhs) noexcept { return rhs.size() == 1 && lhs == rhs.front(); }
    friend inline bool operator< (QChar lhs, QLatin1String rhs) noexcept { return compare_helper(&lhs, 1, rhs) < 0; }
    friend inline bool operator> (QChar lhs, QLatin1String rhs) noexcept { return compare_helper(&lhs, 1, rhs) > 0; }
    friend inline bool operator!=(QChar lhs, QLatin1String rhs) noexcept { return !(lhs == rhs); }
    friend inline bool operator<=(QChar lhs, QLatin1String rhs) noexcept { return !(lhs >  rhs); }
    friend inline bool operator>=(QChar lhs, QLatin1String rhs) noexcept { return !(lhs <  rhs); }
    // QLatin1String <> QChar
    friend inline bool operator==(QLatin1String lhs, QChar rhs) noexcept { return   rhs == lhs; }
    friend inline bool operator!=(QLatin1String lhs, QChar rhs) noexcept { return !(rhs == lhs); }
    friend inline bool operator< (QLatin1String lhs, QChar rhs) noexcept { return   rhs >  lhs; }
    friend inline bool operator> (QLatin1String lhs, QChar rhs) noexcept { return   rhs <  lhs; }
    friend inline bool operator<=(QLatin1String lhs, QChar rhs) noexcept { return !(rhs <  lhs); }
    friend inline bool operator>=(QLatin1String lhs, QChar rhs) noexcept { return !(rhs >  lhs); }
    //QStringView <> QLatin1String
    friend inline bool operator==(QStringView lhs, QLatin1String rhs) noexcept { return lhs.size() == rhs.size() && QtPrivate::equalStrings(lhs, rhs); }
    friend inline bool operator!=(QStringView lhs, QLatin1String rhs) noexcept { return !(lhs == rhs); }
    friend inline bool operator< (QStringView lhs, QLatin1String rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) <  0; }
    friend inline bool operator<=(QStringView lhs, QLatin1String rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) <= 0; }
    friend inline bool operator> (QStringView lhs, QLatin1String rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) >  0; }
    friend inline bool operator>=(QStringView lhs, QLatin1String rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) >= 0; }
    //QLatin1String <> QStringView
    friend inline bool operator==(QLatin1String lhs, QStringView rhs) noexcept { return lhs.size() == rhs.size() && QtPrivate::equalStrings(lhs, rhs); }
    friend inline bool operator!=(QLatin1String lhs, QStringView rhs) noexcept { return !(lhs == rhs); }
    friend inline bool operator< (QLatin1String lhs, QStringView rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) <  0; }
    friend inline bool operator<=(QLatin1String lhs, QStringView rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) <= 0; }
    friend inline bool operator> (QLatin1String lhs, QStringView rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) >  0; }
    friend inline bool operator>=(QLatin1String lhs, QStringView rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) >= 0; }
    //QLatin1String <> char *
    bool operator==(const char *s) const;
    bool operator!=(const char *s) const;
    bool operator<(const char *s) const;
    bool operator>(const char *s) const;
    bool operator<=(const char *s) const;
    bool operator>=(const char *s) const;
    //QLatin1String <> QByteArray
    bool operator==(const QByteArray &s) const;
    bool operator!=(const QByteArray &s) const;
    bool operator<(const QByteArray &s) const;
    bool operator>(const QByteArray &s) const;
    bool operator<=(const QByteArray &s) const;
    bool operator>=(const QByteArray &s) const;
    //char * <> QLatin1String
    friend bool operator==(const char *s1, QLatin1String s2) { return compare_helper(s2, s1) == 0; }
    friend bool operator!=(const char *s1, QLatin1String s2) { return compare_helper(s2, s1) != 0; }
    friend bool operator< (const char *s1, QLatin1String s2) { return compare_helper(s2, s1) >  0; }
    friend bool operator> (const char *s1, QLatin1String s2) { return compare_helper(s2, s1) <  0; }
    friend bool operator<=(const char *s1, QLatin1String s2) { return compare_helper(s2, s1) >= 0; }
    friend bool operator>=(const char *s1, QLatin1String s2) { return compare_helper(s2, s1) <= 0; }

    //iterators
    inline const_iterator begin() const noexcept {return data();}
    inline const_iterator end() const noexcept {return data() + size();}
    inline const_iterator cbegin() const noexcept {return data();}
    inline const_iterator cend() const noexcept {return data() + size();}
    //reverseIterator
    inline const_reverse_iterator rbegin() const noexcept {return const_reverse_iterator (end());}
    inline const_reverse_iterator rend() const noexcept {return const_reverse_iterator (begin());}
    inline const_reverse_iterator rcbegin() const noexcept {return const_reverse_iterator (end());}
    inline const_reverse_iterator rcend() const noexcept {return const_reverse_iterator (begin());}

private:
    constexpr void verify(qsizetype pos, qsizetype n = 0) const {
        Q_ASSERT(pos >= 0);
        Q_ASSERT(pos <= size());
        Q_ASSERT(n >= 0);
        Q_ASSERT(n <= size() - pos);
    }
    static int compare_helper(const QLatin1String &s1, const char *s2);
    static int compare_helper(const QChar *data1, qsizetype length1,  QLatin1String s2,  Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;

private:
    qsizetype m_size;
    const char *m_data; //这里将m_data定义成const，表明QLatin1String不可更改
};

using QLatin1StringView = QLatin1String;

constexpr inline QLatin1StringView operator""_L1(const char *str, size_t size) noexcept
{
    return QLatin1String(str, qsizetype(size));
}

Q_DECLARE_TYPEINFO(QLatin1String, Q_RELOCATABLE_TYPE)

QT_END_NAMESPACE

#endif //QLATIN1STRING_H
