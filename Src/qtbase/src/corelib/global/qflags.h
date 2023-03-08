//
// Created by Yujie Zhao on 2023/3/6.
//

#ifndef QFLAGS_H
#define QFLAGS_H

#include <QtCore/qglobal.h>
#include <QtCore/qmetatype.h>
#include <initializer_list>

QT_BEGIN_NAMESPACE

class QFlag
{
    int i;
public:
    constexpr inline QFlag(int value) noexcept : i(value) {}
    constexpr inline QFlag(uint value) noexcept : i(int(value)) {}
    constexpr inline QFlag(short value) noexcept : i(int(value)) {}
    constexpr inline QFlag(ushort value) noexcept : i(int(uint(value))) {}
    constexpr inline operator int() const noexcept {return i;}
    constexpr inline operator uint() const noexcept {return uint(i);}
};
Q_DECLARE_TYPEINFO(QFlag, Q_PRIMITIVE_TYPE);

class QIncompatibleFlag
{
    int i;
public:
    constexpr inline QIncompatibleFlag(int value) noexcept : i(value){}
    constexpr inline operator int() const noexcept {return i;}
};
Q_DECLARE_TYPEINFO(QIncompatibleFlag, Q_PRIMITIVE_TYPE);

template <typename Enum>
class QFlags
{
    static_assert((sizeof(Enum) <= sizeof(int)));
    static_assert((std::is_enum<Enum>::value));
public:
#if defined(Q_CC_MSVC) || defined(Q_CLANG_QDOC)
    typedef int Int;
#else
    //获取枚举的底层数据类型是否是有符号的。如果有符号，使用signed int，如果无符号，使用unsigned int
typedef typename std::conditional<
                 std::is_unsigned<typename std::underlying_type<Enum>::type>::value,
                 unsigned int,
                 signed int
                 >::type Int;
#endif
    typedef Enum enum_type;

    constexpr inline QFlags() noexcept : i(0){}
    constexpr inline Q_EXPLICIT QFlags(Enum flags) noexcept : i(Int(flags)) {}
    constexpr inline Q_EXPLICIT QFlags(QFlag flag) noexcept : i(flag) {}
    constexpr inline QFlags(std::initializer_list<Enum> flags) noexcept
        :i(initializer_list_helper(flags.begin(), flags.end())){
    }
    constexpr static inline QFlags fromInt(Int i) noexcept {return QFlags(QFlag(i));}
    constexpr inline Int toInt() const noexcept {return i;}
    constexpr inline operator Int() const noexcept {return i;}

    constexpr inline QFlags &operator&=(int mask) noexcept { i &= mask; return *this; }
    constexpr inline QFlags &operator&=(uint mask) noexcept { i &= mask; return *this; }
    constexpr inline QFlags &operator&=(QFlags mask) noexcept { i &= mask.i; return *this; }
    constexpr inline QFlags &operator&=(Enum mask) noexcept { i &= Int(mask); return *this; }

    constexpr inline QFlags &operator|=(QFlags other) noexcept { i |= other.i; return *this; }
    constexpr inline QFlags &operator|=(Enum other) noexcept { i |= Int(other); return *this; }
    constexpr inline QFlags &operator^=(QFlags other) noexcept { i ^= other.i; return *this; }
    constexpr inline QFlags &operator^=(Enum other) noexcept { i ^= Int(other); return *this; }

    constexpr inline QFlags operator|(QFlags other) const noexcept {return QFlags(QFlag(i | other.i));}
    constexpr inline QFlags operator|(Enum other) const noexcept {return QFlags(QFlag(i | Int(other)));}
    constexpr inline QFlags operator^(QFlags other) const noexcept { return QFlags(QFlag(i ^ other.i)); }
    constexpr inline QFlags operator^(Enum other) const noexcept { return QFlags(QFlag(i ^ Int(other))); }
    constexpr inline QFlags operator&(int mask) const noexcept { return QFlags(QFlag(i & mask)); }
    constexpr inline QFlags operator&(uint mask) const noexcept { return QFlags(QFlag(i & mask)); }
    constexpr inline QFlags operator&(QFlags other) const noexcept { return QFlags(QFlag(i & other.i)); }
    constexpr inline QFlags operator&(Enum other) const noexcept { return QFlags(QFlag(i & Int(other))); }
    constexpr inline QFlags operator~() const noexcept { return QFlags(QFlag(~i)); }

    constexpr inline void operator+(QFlags other) const noexcept = delete;
    constexpr inline void operator+(Enum other) const noexcept = delete;
    constexpr inline void operator+(int other) const noexcept = delete;
    constexpr inline void operator-(QFlags other) const noexcept = delete;
    constexpr inline void operator-(Enum other) const noexcept = delete;
    constexpr inline void operator-(int other) const noexcept = delete;

    constexpr inline bool operator!() const noexcept { return !i; }

    //testFlags需要为flags中的所有，testAnyFlags条件为只需要满足flags中的一个
    constexpr inline bool testFlag(Enum flag) const noexcept { return testFlags(flag); }
    constexpr inline bool testFlags(QFlags flags) const noexcept {return flags.i ? ((i & flags.i) == flags.i) : i == Int(0);}
    constexpr inline bool testAnyFlag(Enum flag) const noexcept {return testAnyFlags(flag);}
    constexpr inline bool testAnyFlags(QFlags flags) const noexcept {return (i & flags.i) != Int(0);}
    constexpr inline QFlags &setFlag(Enum flag, bool on = true) noexcept {
        return on? (*this |= flag) : (*this &= ~QFlags(flag));
    }

    friend constexpr inline bool operator==(QFlags lhs, QFlags rhs) noexcept
    { return lhs.i == rhs.i; }
    friend constexpr inline bool operator!=(QFlags lhs, QFlags rhs) noexcept
    { return lhs.i != rhs.i; }
    friend constexpr inline bool operator==(QFlags lhs, Enum rhs) noexcept
    { return lhs == QFlags(rhs); }
    friend constexpr inline bool operator!=(QFlags lhs, Enum rhs) noexcept
    { return lhs != QFlags(rhs); }
    friend constexpr inline bool operator==(Enum lhs, QFlags rhs) noexcept
    { return QFlags(lhs) == rhs; }
    friend constexpr inline bool operator!=(Enum lhs, QFlags rhs) noexcept
    { return QFlags(lhs) != rhs; }


private:
    using const_initializer_iter = typename std::initializer_list<Enum>::const_iterator;
    constexpr static inline Int initializer_list_helper(const_initializer_iter it, const_initializer_iter end) noexcept {
        return (it == end ? Int(0) : (Int(*it) | initializer_list_helper(it + 1, end)));
    }

private:
    Int i;
};

#ifndef Q_MOC_RUN
#define Q_DECLARE_FLAGS(Flags, Enum) \
typedef QFlags<Enum> Flags;
#endif

#define Q_DECLARE_OPERATORS_FOR_FLAGS(Flags) \
constexpr inline QFlags<Flags::enum_type> operator|(Flags::enum_type f1, Flags::enum_type f2) noexcept \
{ return QFlags<Flags::enum_type>(f1) | f2; }\
constexpr inline QFlags<Flags::enum_type> operator|(Flags::enum_type f1, QFlags<Flags::enum_type> f2) noexcept \
{ return f2 | f1; }                          \
constexpr inline QFlags<Flags::enum_type> operator&(Flags::enum_type f1, Flags::enum_type f2) noexcept \
{ return QFlags<Flags::enum_type>(f1) & f2; } \
constexpr inline QFlags<Flags::enum_type> operator&(Flags::enum_type f1, QFlags<Flags::enum_type> f2) noexcept \
{ return f2 & f1; } \
constexpr inline void operator+(Flags::enum_type f1, Flags::enum_type f2) noexcept = delete;           \
constexpr inline void operator+(Flags::enum_type f1, QFlags<Flags::enum_type> f2) noexcept = delete;   \
constexpr inline void operator+(int f1, QFlags<Flags::enum_type> f2) noexcept = delete;                \
constexpr inline void operator-(Flags::enum_type f1, Flags::enum_type f2) noexcept = delete; \
constexpr inline void operator-(Flags::enum_type f1, QFlags<Flags::enum_type> f2) noexcept = delete; \
constexpr inline void operator-(int f1, QFlags<Flags::enum_type> f2) noexcept = delete;                \
constexpr inline void operator+(int f1, Flags::enum_type f2) noexcept = delete; \
constexpr inline void operator+(Flags::enum_type f1, int f2) noexcept = delete; \
constexpr inline void operator-(int f1, Flags::enum_type f2) noexcept = delete; \
constexpr inline void operator-(Flags::enum_type f1, int f2) noexcept = delete;                        \
constexpr inline QIncompatibleFlag operator|(Flags::enum_type f1, int f2) noexcept                    \
{ return QIncompatibleFlag(int(f1) | f2); }

QT_END_NAMESPACE

#endif //QFLAGS_H
