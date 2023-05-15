//
// Created by Yujie Zhao on 2023/3/3.
//

#ifndef QBYTEARRAYVIEW_H
#define QBYTEARRAYVIEW_H

#include <QtCore/qglobal.h>
#include <QtCore/qnamespace.h>
#include <string>
#include "qbytearrayalgorithms.h"

QT_BEGIN_NAMESPACE

class QByteArray;
namespace QtPrivate {
    // 支持char,uchar, signed char, std::byte类型数据
    // 都会被转换成char类型，可能会产生范围溢出的问题
    template<typename Byte>
    struct IsCompatibleByteTypeHelper : std::integral_constant<bool, std::is_same_v<Byte, char> ||
                                                                     std::is_same_v<Byte, uchar> ||
                                                                     std::is_same_v<Byte, signed char> ||
                                                                     std::is_same_v<Byte, std::byte>> {
    };

    //移除 const volatile &限定符
    template<typename Byte>
    struct IsCompatibleByteType
            : IsCompatibleByteTypeHelper<typename std::remove_cv_t<typename std::remove_reference_t<Byte>>> {
    };

    template<typename Pointer>
    struct IsCompatibleByteArrayPointerHelper : std::false_type {
    };
    template<typename Byte>
    struct IsCompatibleByteArrayPointerHelper<Byte *> : IsCompatibleByteType<Byte> {
    };

    //移除引用，移除cv，
    template<typename Pointer>
    struct IsCompatibleByteArrayPointer : IsCompatibleByteArrayPointerHelper<
            typename std::remove_cv_t<typename std::remove_reference_t<Pointer>>> {
    };

    template <typename T, typename Enable = void>
    struct IsContainerCompatibleWithQByteArrayView : std::false_type {};

    //Coontainer容器是否与QByteArray有一样的固定接口
    template <typename T>
    struct IsContainerCompatibleWithQByteArrayView<T, std::enable_if_t<
            std::conjunction_v<  //std::conjunction，条件与
                    IsCompatibleByteArrayPointer<decltype(std::data(std::declval<const T &>()))>,  //T->data()返回是Byte *类型
                    std::is_convertible<decltype(std::size(std::declval<const T &>())), qsizetype>, //T->size()可以与qsizetype转换
                    IsCompatibleByteType<typename std::iterator_traits<decltype(std::begin(std::declval<const T &>()))>::value_type>,  //迭代器的value_type是byte类型
                    std::is_convertible<decltype(std::begin(std::declval<const T &>()) != std::end(std::declval<const T &>())), bool>,  //T->begin() != T->end()与bool
                    std::negation<std::is_same<std::decay_t<T>, QByteArray>>,  //T不是QByteArray类型。QByteArray类型已经被if_compatible_qbytearray_like处理了
                    std::negation<std::is_array<T>>, //不是数组，数组被其他处理了
                    std::negation<std::is_same<std::decay_t<T>, QByteArrayView>>>>> : std::true_type {};   //不是QByteArrayView，QByteArrayView走拷贝构造
}

class QByteArrayView {
public:
    typedef char storage_type;
    typedef const char value_type;
    typedef qptrdiff difference_type;
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
    template <typename Byte>
    using if_compatible_byte = typename std::enable_if_t<QtPrivate::IsCompatibleByteType<Byte>::value, bool>;

    template <typename Pointer>
    using if_compatible_pointer = typename std::enable_if_t<QtPrivate::IsCompatibleByteArrayPointer<Pointer>::value, bool>;

    template <typename T>
    using if_compatible_qbytearray_like = typename std::enable_if_t<std::is_same_v<T, QByteArray>, bool>;

    //容器中的元素类型是否是QByteArrayView支持的类型
    template <typename T>
    using if_compatible_container = typename std::enable_if_t<QtPrivate::IsContainerCompatibleWithQByteArrayView<T>::value, bool>;

    template <typename Char>
    static constexpr qsizetype lengthHelperPointer(const Char *data) noexcept {
        return qsizetype(std::char_traits<Char>::length(data));
    }

    static constexpr qsizetype lengthHelperCharArray(const char *data, size_t size) noexcept {
        const auto it = std::char_traits<char>::find(data, size, '\0');
        const auto end = it ? it : std::next(data, size);
        return qsizetype(std::distance(data, end));
    }

    template <typename Byte>
    static const storage_type *castHelper(const Byte *data) noexcept {
        return reinterpret_cast<const storage_type *>(data);
    }
    static constexpr const storage_type *castHelper(const storage_type *data) noexcept {
        return data;
    }


public:
    constexpr QByteArrayView() noexcept
        : m_data(nullptr), m_size(0) {
    }

    constexpr QByteArrayView(std::nullptr_t) noexcept
        : QByteArrayView() {
    }

    template <typename Byte, if_compatible_byte<Byte> = true>
    constexpr QByteArrayView(const Byte *data, qsizetype len)
        : m_size((Q_ASSERT(len >= 0), Q_ASSERT(data || !len), len))
        , m_data(castHelper(data))
    {}

    template <typename Byte, if_compatible_byte<Byte> = true>
    constexpr QByteArrayView(const Byte *first, const Byte *last)
        : QByteArrayView(first, last - first) {}

    //没有添加explicit，允许隐式转换
    template <typename Pointer, if_compatible_pointer<Pointer> = true>
    constexpr QByteArrayView(const Pointer &data) noexcept
        : QByteArrayView(data, data ? lengthHelperPointer(data) : 0)
    {}

    template <typename ByteArray, if_compatible_qbytearray_like<ByteArray> = true>
    QByteArrayView(const ByteArray &ba) noexcept
        : QByteArrayView(ba.isNull() ? nullptr : ba.data(), qsizetype(ba.size()))
    {}

    template <typename Container, if_compatible_container<Container> = true>
    constexpr QByteArrayView(const Container &c) noexcept
        : QByteArrayView(std::data(c), lengthHelperContainer(c))
    {}

    //QByteArrayView("aaa")之类的构造函数，const char[n]
    template <size_t Size>
    constexpr QByteArrayView(const char (&data)[Size]) noexcept
        : QByteArrayView(data, lengthHelperCharArray(data, Size))
    {
    }

    //fromArray可以将数组中的'\0'字符存储进来
    template <typename Byte, size_t size, if_compatible_byte<Byte> = true>
    constexpr static QByteArrayView fromArray(const Byte(&data)[size]) noexcept
    { return QByteArrayView(data, size); }
    inline QByteArray toByteArray() const;

    constexpr qsizetype size() const noexcept {return m_size;}
    constexpr const_pointer data() const noexcept {return m_data;}
    constexpr const_pointer constData() const noexcept {return m_data;}
    constexpr char operator[](qsizetype n) const
    { Q_ASSERT(n >= 0); Q_ASSERT(n < size()); return m_data[n]; }

    //isNull与empty的区别： isNull只判断data指针是否为空，empty包含了isNull的语义，额外包含了size为0
    constexpr bool empty() const noexcept {return size() == 0;}
    constexpr bool isEmpty() const noexcept {return empty();}
    constexpr bool isNull() const noexcept {return !m_data;}
    constexpr qsizetype length() const noexcept {return size();}
    constexpr char front() const { Q_ASSERT(!empty()); return m_data[0]; }
    constexpr char back() const { Q_ASSERT(!empty()); return m_data[m_size - 1]; }
    constexpr char first() const { return front(); }
    constexpr char last() const { return back(); }

    constexpr char at(qsizetype n) const { assert(n >= 0 && n < m_size); return m_data[n]; }
    constexpr QByteArrayView sliced(qsizetype pos) const {
        Q_ASSERT(pos >= 0); Q_ASSERT(pos <= size());
        return QByteArrayView(data() + pos, size() - pos);
    }
    constexpr QByteArrayView first(qsizetype n) const {
        Q_ASSERT(n >= 0);
        Q_ASSERT(n <= size());
        return QByteArrayView(data(), n);
    }
    constexpr QByteArrayView last(qsizetype n) const {
        Q_ASSERT(n >= 0);
        Q_ASSERT(n <= size());
        return QByteArrayView(data() + size() - n, n);
    }

    constexpr QByteArrayView sliced(qsizetype pos, qsizetype n) const {
        Q_ASSERT(pos >= 0);
        Q_ASSERT(n >= 0);
        Q_ASSERT(size_t(pos) + size_t(n) <= size_t(size()));
        return QByteArrayView(data() + pos, n);
    }
    QByteArrayView chopped(qsizetype len) const {
        Q_ASSERT(len >= 0);
        Q_ASSERT(len <= size());
        return first(size() - len);
    }

    constexpr void truncate(qsizetype n) {
        Q_ASSERT(n >= 0);
        Q_ASSERT(n <= size());
        m_size = n;
    }
    constexpr void chop(qsizetype n) {
        Q_ASSERT(n >= 0);
        Q_ASSERT(n <= size());
        m_size -= n;
    }
    bool startsWith(QByteArrayView other) const noexcept
    { return QtPrivate::startsWith(*this, other); }
    bool startsWith(char c) const noexcept
    { return !empty() && front() == c; }
    bool endsWith(QByteArrayView other) const noexcept
    { return QtPrivate::endsWith(*this, other); }
    bool endsWith(char c) const noexcept
    { return !empty() && back() == c; }

    qsizetype indexOf(QByteArrayView a, qsizetype from = 0) const noexcept
    { return QtPrivate::findByteArray(*this, from, a); }
    qsizetype indexOf(char ch, qsizetype from = 0) const noexcept
    { return QtPrivate::findByteArray(*this, from, QByteArrayView(&ch, 1)); }

    bool contains(QByteArrayView a) const noexcept
    { return indexOf(a) != qsizetype(-1); }
    bool contains(char c) const noexcept
    { return indexOf(c) != qsizetype(-1); }

    qsizetype lastIndexOf(QByteArrayView a) const noexcept
    { return lastIndexOf(a, size()); }
    qsizetype lastIndexOf(QByteArrayView a, qsizetype from) const noexcept
    { return QtPrivate::lastIndexOf(*this, from, a); }
    //from为-1时，内部算法会加上size，从末尾开始
    qsizetype lastIndexOf(char ch, qsizetype from = -1) const noexcept
    { return QtPrivate::lastIndexOf(*this, from, QByteArrayView(&ch, 1)); }

    qsizetype count(QByteArrayView a) const noexcept
    { return QtPrivate::count(*this, a); }
    qsizetype count(char ch) const noexcept
    { return QtPrivate::count(*this, QByteArrayView(&ch, 1)); }

    inline int compare(QByteArrayView a, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;

    //适配STL算法的API
    constexpr const_iterator begin() const noexcept { return data(); }
    constexpr const_iterator end() const noexcept { return data() + size(); }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr const_iterator cend() const noexcept { return end(); }
    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    friend inline bool operator==(QByteArrayView lhs, QByteArrayView rhs) noexcept {
        return lhs.size() == rhs.size() && QtPrivate::compareMemory(lhs, rhs) == 0;
    }
    friend inline bool operator!=(QByteArrayView lhs, QByteArrayView rhs) noexcept {
        return !(lhs == rhs);
    }
    friend inline bool operator <(QByteArrayView lhs, QByteArrayView rhs) noexcept {
        return QtPrivate::compareMemory(lhs, rhs) < 0;
    }
    friend inline bool operator<=(QByteArrayView lhs, QByteArrayView rhs) noexcept {
        return QtPrivate::compareMemory(lhs, rhs) <= 0;
    }
    friend inline bool operator >(QByteArrayView lhs, QByteArrayView rhs) noexcept {
        return !(lhs <= rhs);
    }
    friend inline bool operator>=(QByteArrayView lhs, QByteArrayView rhs) noexcept {
        return !(lhs < rhs);
    }

private:
    template <typename Container>
    static constexpr qsizetype lengthHelperContainer(const Container &c) noexcept
    {
        return qsizetype(std::size(c));
    }

private:
    const storage_type *m_data;
    qsizetype m_size;
};

template <typename QByteArrayLike,
        std::enable_if_t<std::is_same_v<QByteArrayLike, QByteArray>, bool> = true>
inline QByteArrayView qToByteArrayViewIgnoringNull(const QByteArrayLike &b) noexcept
{
    return QByteArrayView(b.data(), b.size());
}

inline int QByteArrayView::compare(QByteArrayView a, Qt::CaseSensitivity cs) const noexcept
{
    if (cs == Qt::CaseSensitive) {
        return QtPrivate::compareMemory(*this, a);
    }
    else {
        return qstrnicmp(data(), size(), a.data(), size());
    }
}

QT_END_NAMESPACE

#endif //QBYTEARRAYVIEW_H
