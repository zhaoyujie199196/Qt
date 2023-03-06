//
// Created by Yujie Zhao on 2023/3/3.
//

#ifndef QBYTEARRAYVIEW_H
#define QBYTEARRAYVIEW_H

#include <QtCore/qglobal.h>
#include <string>
#include "qbytearrayalgorithms.h"

QT_BEGIN_NAMESPACE

namespace QtPrivate {
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

    template <typename Byte, if_compatible_byte<Byte> = true>
    constexpr QByteArrayView(const Byte *data, qsizetype len)
        : m_size((Q_ASSERT(len > 0), Q_ASSERT(data || !len), len))
        , m_data(castHelper(data))
    {}

    //没有添加explicit，允许隐式转换
    template <typename Pointer, if_compatible_pointer<Pointer> = true>
    constexpr QByteArrayView(const Pointer &data) noexcept
        : QByteArrayView(data, data ? lengthHelperPointer(data) : 0) {
    }

    //QByteArrayView("aaa")之类的构造函数，const char[n]
    template <size_t Size>
    constexpr QByteArrayView(const char (&data)[Size]) noexcept
        : QByteArrayView(data, lengthHelperCharArray(data, Size))
    {
    }

    constexpr inline qsizetype size() const noexcept {return m_size;}
    constexpr inline const_pointer data() const noexcept {return m_data;}
    constexpr inline const_pointer constData() const noexcept {return m_data;}

    constexpr bool empty() const noexcept {return size() == 0;}
    constexpr bool isEmpty() const noexcept {return empty();}
    constexpr bool isNull() const noexcept {return !m_data;}
    constexpr qsizetype length() const noexcept {return size();}

    constexpr char at(qsizetype n) const {assert(n >= 0 && n < m_size); return m_data[n];}

    friend inline bool operator==(QByteArrayView lhs, QByteArrayView rhs) noexcept {
        return lhs.size() == rhs.size() && QtPrivate::compareMemory(lhs, rhs) == 0;
    }

private:
    const storage_type *m_data;
    qsizetype m_size;
};

QT_END_NAMESPACE

#endif //QBYTEARRAYVIEW_H
