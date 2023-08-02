//
// Created by Yujie Zhao on 2023/3/1.
//

#ifndef QSTRINGVIEW_H
#define QSTRINGVIEW_H

#include "QtCore/qchar.h"
#include <iterator>
#include "qstringalgorithms.h"
#include "QtCore/qarraydata.h"

QT_BEGIN_NAMESPACE

//定位是char16_t *数据的容器，提供一些只读访问公共方法，不参与生命周期的管理，不可对数据进行更改
//只接受16位的char

class QString;
class QStringView;
class QLatin1String;

namespace QtPrivate {
    template <typename Char>
    struct IsCompatibleCharTypeHelper
            : std::integral_constant<bool,
                                     std::is_same<Char, QChar>::value ||
                                     std::is_same<Char, ushort>::value ||
                                     std::is_same<Char, char16_t>::value ||
                                     (std::is_same<Char, wchar_t>::value && sizeof(wchar_t) == sizeof(QChar))>
    {
    };

    template <typename Char>
    struct IsCompatibleCharType : IsCompatibleCharTypeHelper<typename std::remove_cv<typename std::remove_reference<Char>::type>::type> {};

    template <typename Pointer>
    struct IsCompatiblePointerHelper : std::false_type {};
    template <typename Char>
    struct IsCompatiblePointerHelper<Char *> : IsCompatibleCharType<Char> {};
    template <typename Pointer>
    struct IsCompatiblePointer : IsCompatiblePointerHelper<typename std::remove_cv<typename std::remove_reference<Pointer>::type>::type> {};

    template <typename T, typename Enable = void>
    struct IsContainerCompatibleWithQStringView : std::false_type {};
    template <typename T>
    struct IsContainerCompatibleWithQStringView<T, std::enable_if_t<std::conjunction_v<
            IsCompatiblePointer<decltype( std::data(std::declval<const T &>()) )>,  //有data()函数
            std::is_convertible<decltype( std::size(std::declval<const T &>()) ), qsizetype>, //::size()函数返回值可以与qsizetype转换
            IsCompatibleCharType<typename std::iterator_traits<decltype(std::begin(std::declval<const T &>()) )>::value_type>,  //begin迭代器的value_type
            std::is_convertible<  //::begin() != ::end()
                decltype(std::begin(std::declval<const T &>()) != std::end(std::declval<const T &>()) ),
                bool>,
            std::negation<std::is_same<std::decay_t<T>, QString>>,  //不是QString类型
            std::negation<std::is_same<std::decay_t<T>, QStringView>>  //不是QStringView类型
            >>> : std::true_type {};
}

class QStringView {
public:
    typedef char16_t storage_type;
    typedef const QChar value_type;
    typedef std::ptrdiff_t difference_type;
    typedef qsizetype size_type;
    typedef value_type &reference;
    typedef value_type &const_reference;
    typedef value_type *pointer;
    typedef value_type *const_pointer;

    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
    //是否是char_16类型
    template <typename Char>
    using if_compatible_char = typename std::enable_if<QtPrivate::IsCompatibleCharTypeHelper<Char>::value, bool>::type;

    template <typename Pointer>
    using if_compatible_pointer = typename std::enable_if<QtPrivate::IsCompatiblePointer<Pointer>::value, bool>::type;

    template <typename T>
    using if_compatible_qstring_like = typename std::enable_if<std::is_same<T, QString>::value, bool>::type;

    template <typename T>
    using if_compatible_container = typename std::enable_if<QtPrivate::IsContainerCompatibleWithQStringView<T>::value, bool>::type;

    template<typename Char>
    static const storage_type *castHelper(const Char *str) noexcept {
        return reinterpret_cast<const storage_type *>(str);
    }
    static constexpr const storage_type *castHelper(const storage_type *str) noexcept {
        return str;
    }

    template <typename Container>
    static qsizetype lengthHelperContainer(const Container &c) noexcept
    { return qsizetype(std::size(c)); }

    template <typename Char, size_t N>
    static constexpr qsizetype lengthHelperContainer(const Char (&str)[N]) {
        const auto it = std::char_traits<Char>::find(str, N, Char(0));
        const auto end = it ? it : std::end(str);
        return qsizetype(std::distance(str, end));
    }

    template <typename Char>
    static qsizetype lengthHelperPointer(const Char *str) noexcept
    { return QtPrivate::qustrlen(reinterpret_cast<const char16_t *>(str)); }

    static qsizetype lengthHelperPointer(const QChar *str) noexcept
    { return QtPrivate::qustrlen(reinterpret_cast<const char16_t *>(str)); }

public:
    constexpr QStringView() noexcept
        : m_size(0)
        , m_data(nullptr) {
    }

    constexpr QStringView(std::nullptr_t) noexcept
        : QStringView() {
    }

    //如果不符合char16_t，则if_compatible_char<Char>没有类型（详见std::enable_if定义），编译不过
    template <typename Char, if_compatible_char<Char> = true>
    constexpr QStringView(const Char *str, qsizetype len)
        : m_size((Q_ASSERT(len >= 0), Q_ASSERT(str || !len), len))
        , m_data(castHelper(str)) {
    }

    template <typename Char, if_compatible_char<Char> = true>
    constexpr QStringView(const Char *f, const Char *l)
        : QStringView(f, l - f) {
    }

    template <typename String, if_compatible_qstring_like<String> = true>
    QStringView(const String &str) noexcept
        : QStringView(str.isNull() ? nullptr : str.data(), qsizetype(str.size()))
    {
    }

    //QStringView view1(u"aaaa"); 这种形式会导致编译错误。u"aaaa"的类型是const char16_t[], 不是const char16_t *
    template <typename Pointer, if_compatible_pointer<Pointer> = true>
    constexpr QStringView(const Pointer &str) noexcept
        : QStringView(str, str ? lengthHelperPointer(str) : 0)
    {}

    template <typename Container, if_compatible_container<Container> = false>
    constexpr QStringView(const Container &c) noexcept
        : QStringView(std::data(c), lengthHelperContainer(c))
    {
    }

    template <typename Char, size_t Size, if_compatible_char<Char> = true>
    static QStringView fromArray(const Char(&string)[Size]) noexcept {
        return QStringView(string, Size);
    }

    QString toString() const;

    constexpr QChar operator[](qsizetype n) const noexcept {
        return Q_ASSERT(n >= 0), Q_ASSERT(n < size()), QChar(m_data[n]);
    }
    constexpr QChar at(qsizetype n) const noexcept {return (*this)[n];}
    constexpr QStringView mid(qsizetype pos, qsizetype n = -1) const noexcept
    {
        using namespace QtPrivate;
        auto result = QContainerImplHelper::mid(size(), &pos, &n);
        return result == QContainerImplHelper::Null ? QStringView() : QStringView(m_data + pos, n);
    }
    constexpr QStringView left(qsizetype n) const noexcept
    {
        if (size_t(n) >= size_t(size())) {
            n = size();
        }
        return QStringView(m_data, n);
    }
    constexpr QStringView right(qsizetype n) const noexcept
    {
        if (size_t(n) >= size_t(size())) {
            n = size();
        }
        return QStringView(m_data + m_size - n, n);
    }


    constexpr qsizetype size() const noexcept {return m_size;}
    //const char16_t * -> const QChar *
    const_pointer data() const noexcept {return reinterpret_cast<const_pointer>(m_data);}
    const_pointer constData() const noexcept { return data(); }
    constexpr const storage_type *utf16() const noexcept { return m_data; }

    //STL容器需要适配的接口
    const_iterator const begin() const noexcept {return data();}
    const_iterator const end() const noexcept {return data() + m_size;}
    const_iterator const cbegin() const noexcept {return data();}
    const_iterator const cend() const noexcept {return data() + m_size;}
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    constexpr bool empty() const noexcept { return size() == 0; }
    constexpr QChar front() const { Q_ASSERT(!empty()); return QChar(m_data[0]);}
    constexpr QChar back() const { Q_ASSERT(!empty()); return QChar(m_data[m_size - 1]); }

    //Qt容器适配接口
    const_iterator constBegin() const noexcept { return begin(); }
    const_iterator constEnd() const noexcept { return end(); }
    constexpr bool isNull() const { return !m_data; }
    constexpr bool isEmpty() const { return empty(); }
    constexpr qsizetype length() const { return size(); }
    constexpr QChar first() const { return front(); }
    constexpr QChar last() const { return back(); }
    constexpr inline QStringView chopped(qsizetype n) const noexcept;

    constexpr inline void truncate(qsizetype n) noexcept;
    constexpr inline void chop(qsizetype n) noexcept;

    //compare
    int compare(QStringView other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::compareStrings(*this, other, cs); }
    inline int compare(QLatin1String other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    constexpr int compare(QChar c) const noexcept
    { return size() >= 1 ? compare_single_char_helper(*utf16() - c.unicode()) : -1; }
    int compare(QChar c, Qt::CaseSensitivity cs) const noexcept
    { return QtPrivate::compareStrings(*this, QStringView(&c, 1), cs); }

    //startsWith
    bool startsWith(QStringView s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    bool startsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    constexpr bool startsWith(QChar c) const noexcept
    { return !isEmpty() && front() == c; }
    inline bool startsWith(QChar c, Qt::CaseSensitivity cs) const noexcept
    { return QtPrivate::startsWith(*this, QStringView(&c, 1), cs); }

    //endsWith
    bool endsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    bool endsWith(QStringView s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    bool endsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    constexpr bool endsWith(QChar c) const noexcept
    { return !isEmpty() && back() == c; }
    inline bool endsWith(QChar c, Qt::CaseSensitivity cs) const noexcept
    { return QtPrivate::endsWith(*this, QStringView(&c, 1), cs); }

    //QStringView <> QStringView
    friend bool operator==(QStringView lhs, QStringView rhs) noexcept { return lhs.size() == rhs.size() && QtPrivate::equalStrings(lhs, rhs); }
    friend bool operator!=(QStringView lhs, QStringView rhs) noexcept { return !(lhs == rhs); }
    friend bool operator< (QStringView lhs, QStringView rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) <  0; }
    friend bool operator<=(QStringView lhs, QStringView rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) <= 0; }
    friend bool operator> (QStringView lhs, QStringView rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) >  0; }
    friend bool operator>=(QStringView lhs, QStringView rhs) noexcept { return QtPrivate::compareStrings(lhs, rhs) >= 0; }

    //QStringView <> QChar
    friend bool operator==(QStringView lhs, QChar rhs) noexcept { return lhs == QStringView(&rhs, 1); }
    friend bool operator!=(QStringView lhs, QChar rhs) noexcept { return lhs != QStringView(&rhs, 1); }
    friend bool operator< (QStringView lhs, QChar rhs) noexcept { return lhs <  QStringView(&rhs, 1); }
    friend bool operator<=(QStringView lhs, QChar rhs) noexcept { return lhs <= QStringView(&rhs, 1); }
    friend bool operator> (QStringView lhs, QChar rhs) noexcept { return lhs >  QStringView(&rhs, 1); }
    friend bool operator>=(QStringView lhs, QChar rhs) noexcept { return lhs >= QStringView(&rhs, 1); }

    friend bool operator==(QChar lhs, QStringView rhs) noexcept { return QStringView(&lhs, 1) == rhs; }
    friend bool operator!=(QChar lhs, QStringView rhs) noexcept { return QStringView(&lhs, 1) != rhs; }
    friend bool operator< (QChar lhs, QStringView rhs) noexcept { return QStringView(&lhs, 1) <  rhs; }
    friend bool operator<=(QChar lhs, QStringView rhs) noexcept { return QStringView(&lhs, 1) <= rhs; }
    friend bool operator> (QChar lhs, QStringView rhs) noexcept { return QStringView(&lhs, 1) >  rhs; }
    friend bool operator>=(QChar lhs, QStringView rhs) noexcept { return QStringView(&lhs, 1) >= rhs; }

private:
    //diff是xiang jian
    constexpr int compare_single_char_helper(int diff) const noexcept {
        return diff ? diff : size() > 1 ? 1 : 0;
    }

private:
    qsizetype m_size;
    const storage_type *m_data;
};

Q_DECLARE_TYPEINFO(QStringView, Q_PRIMITIVE_TYPE);

template <typename QStringLike, typename std::enable_if<
        std::is_same<QStringLike, QString>::value, bool
        >::type = true>
inline QStringView qToStringViewIgnoringNull(const QStringLike &s) noexcept
{ return QStringView(s.data(), s.size()); }

constexpr inline QStringView QStringView::chopped(qsizetype n) const noexcept {
    Q_ASSERT(n >= 0);
    Q_ASSERT(n <= size());
    return QStringView(m_data, m_size - n);
}

constexpr inline void QStringView::truncate(qsizetype n) noexcept {
    Q_ASSERT(n >= 0 && n <= size());
    m_size = n;
}

constexpr inline void QStringView::chop(qsizetype n) noexcept {
    Q_ASSERT(n >= 0 && n <= size());
    m_size -= n;
}

constexpr auto QChar::fromUcs4(char32_t c) noexcept {
    struct R {
        char16_t chars[2];
        constexpr operator QStringView() const noexcept { return {begin(), end()}; }
        constexpr qsizetype size() const noexcept { return chars[1] ? 2 : 1; }
        constexpr const char16_t *begin() const noexcept { return chars; }
        constexpr const char16_t *end() const { return begin() + size(); }
    };
    return requiresSurrogates(c) ? R { { QChar::highSurrogate(c), QChar::lowSurrogate(c) } }
                                 : R { { char16_t(c), u'\0'} };
}


QT_END_NAMESPACE

#endif //QSTRINGVIEW_H
