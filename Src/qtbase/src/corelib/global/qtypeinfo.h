//
// Created by Yujie Zhao on 2023/2/16.
//

#ifndef QTYPEINFO_H
#define QTYPEINFO_H

#include <QtCore/qglobal.h>


QT_BEGIN_NAMESPACE

/*
 * 这段代码定义了一个inline constexpr布尔变量qIsRelocatable，它使用了C++标准库中的两个类型特征std::is_trivially_copyable_v和std::is_trivially_destructible_v来确定一个类型T是否是可重定位的。
   可重定位类型是指可以在内存中安全地移动而不需要任何特殊处理或引起未定义行为的类型。换句话说，如果一个类型是可重定位的，它可以在内存中高效地移动，例如通过被移动或复制到内存中的不同位置。
   qIsRelocatable的定义检查类型T是否同时是平凡复制和平凡析构的。如果一个类型是平凡复制的，那么它的复制构造函数和复制赋值运算符是平凡的，这意味着它们进行对象的位拷贝。如果一个类型是平凡析构的，那么它的析构函数是平凡的，这意味着它什么都不做。
   如果T同时满足平凡复制和平凡析构的条件，那么qIsRelocatable将是true。这意味着T被认为是可重定位的，可以在内存中安全地移动。如果这两个条件中有任何一个不满足，qIsRelocatable将是false，表示T被认为是不可重定位的。
 * */
template <typename T>
inline constexpr bool qIsRelocatable = std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>;

/*
 * std::is_trivial_v是一个类型特征，用于检查给定类型是否是平凡类型。一个平凡类型是一个类型，它是：
 * 1.标量类型（如整数、浮点数等）。
 * 2.可以通过位拷贝复制的聚合类型（即没有用户定义的构造函数、析构函数、拷贝构造函数和移动构造函数的类型）。
 * 3.可以通过位拷贝复制的枚举类型。
 * 4.可以通过位拷贝复制的指针类型。
 * 如果一个类型是平凡类型，则它可以安全地使用位拷贝和移动构造函数，以及其他要求类型是平凡的操作。
 */

template <typename T>
class QTypeInfo {
public:
    enum {
        isPointer = std::is_pointer_v<T>,  //是否是指针类型
        isIntegral = std::is_integral_v<T>, //是否是证书类型
        isComplex = !std::is_trivial_v<T>, //是否是平凡类型
        isRelocatable = qIsRelocatable<T>  //是否可重定位
    };
};

template <>
class QTypeInfo<void> {
public:
    enum {
        isPointer = false,
        isIntegral = false,
        isComplex = false,
        isRelocatable = false
    };
};

#define Q_DECLARE_MOVABLE_CONTAINER(CONTAINER) \
template <typename ...T>                       \
class QTypeInfo<CONTAINER<T...>>               \
{                                              \
public:                                    \
    enum {                                     \
        isPointer = false,                     \
        isIntegral = false,                    \
        isComplex = true,                      \
        isRelocatable = true\
    };                                         \
};

#undef Q_DECLARE_MOVABLE_CONTAINER

enum {
    Q_COMPLEX_TYPE = 0,
    Q_PRIMITIVE_TYPE = 0x1,
    Q_RELOCATABLE_TYPE = 0x2,
    Q_MOVABLE_TYPE = 0x2,
    Q_DUMMY_TYPE = 0x4
};

#define Q_DECLARE_TYPEINFO_BODY(TYPE, FLAGS) \
class QTypeInfo<TYPE>                        \
{                                            \
public:                                      \
    enum {                                   \
        isComplex = (((FLAGS) & Q_PRIMITIVE_TYPE) == 0) && !std::is_trivial_v<TYPE>, \
        isRelocatable = !isComplex || ((FLAGS) & Q_RELOCATABLE_TYPE) || qIsRelocatable<TYPE>, \
        isPointer = false,                   \
        isIntegral = std::is_integral<TYPE>::value, \
    };\
};

#define Q_DECLARE_TYPEINFO(TYPE, FLAGS) \
template <>                             \
Q_DECLARE_TYPEINFO_BODY(TYPE, FLAGS)

QT_END_NAMESPACE

#endif //QTYPEINFO_H
