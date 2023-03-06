//
// Created by Yujie Zhao on 2023/3/1.
//

#ifndef QSTRINGVIEW_H
#define QSTRINGVIEW_H

#include "QtCore/qchar.h"
#include <iterator>

QT_BEGIN_NAMESPACE

//定位是char16_t *数据的容器，提供一些只读访问公共方法，不参与生命周期的管理，不可对数据进行更改
//只接受16位的char

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

    template<typename Char>
    static const storage_type *castHelper(const Char *str) noexcept {
        return reinterpret_cast<const storage_type *>(str);
    }
    static constexpr const storage_type *castHelper(const storage_type *str) noexcept {
        return str;
    }

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
        : m_size((Q_ASSERT(len > 0), Q_ASSERT(str), len))
        , m_data(castHelper(str)) {
    }

    template <typename Char, if_compatible_char<Char> = true>
    constexpr QStringView(const Char *f, const Char *l)
        : QStringView(f, l - f) {
    }

    constexpr QChar operator[](qsizetype n) const noexcept {
        return Q_ASSERT(n >= 0), Q_ASSERT(n < size()), QChar(m_data[n]);
    }
    constexpr QChar at(qsizetype n) const noexcept {return (*this)[n];}

    constexpr qsizetype size() const noexcept {return m_size;}
    //const char16_t * -> const QChar *
    const_pointer data() const noexcept {return reinterpret_cast<const_pointer>(m_data);}

    const_iterator const begin() const noexcept {return data();}
    const_iterator const end() const noexcept {return data() + m_size;}
    const_iterator const cbegin() const noexcept {return data();}
    const_iterator const cend() const noexcept {return data() + m_size;}


private:
    qsizetype m_size;
    const storage_type *m_data;
};

QT_END_NAMESPACE

#endif //QSTRINGVIEW_H
