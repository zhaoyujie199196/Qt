//
// Created by Yujie Zhao on 2023/2/16.
//

#include <QtCore/qglobal.h>
#include <tuple>
#include <variant>
#include <optional>

#ifndef QTYPEINFO_H
#define QTYPEINFO_H

QT_BEGIN_NAMESPACE

/*
 * isComplex: 是否为复杂类型
 * 如果一个类型是复杂类型，他的对象在创建和销毁是需要调用相应的成员函数来完成对象的构造与析构
 * 复杂类型通常是指包含有构造函数、析构函数、拷贝构造函数、移动构造函数等成员函数的类型
 * */

/*
 * isRelocatable: 是否可重定位
 * 可重定位类型是指对象在被移动到不同的内存地址时，其数据成员不会受到影响
 * 如果一个类型是可重定位的，那么其对象可以被移动到不同的内存地址而不需要进行任何特殊处理。
 * */

/*
 * 定义inline constexpr布尔变量qIsRelocatable，使用两个类型特征std::is_trivially_copyable_v和std::is_trivially_destructible_v来确定一个类型T是否是可重定位的。
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
 *
 * 平凡类型：https://blog.csdn.net/lr_shadow/article/details/122980868
 */

/*
 * std::declval的功能：返回某个类型T的右值引用，不管该类型是否有默认构造函数或者该类型是否可以创建对象
 * c++模板进阶指南：https://zhuanlan.zhihu.com/p/21314708
 * */

template <typename T>
class QTypeInfo {
public:
    enum {
        isPointer = std::is_pointer_v<T>,  //是否是指针类型
        isIntegral = std::is_integral_v<T>,  //是否是数值类型
        isComplex = !std::is_trivial_v<T>,  //是否是平凡类型
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

template <class T, class ...Ts>
class QTypeInfoMerger
{
    static_assert(sizeof...(Ts) > 0);
public:
    static constexpr bool isComplex = ((QTypeInfo<Ts>::isComplex) || ...);
    static constexpr bool isRelocatable = ((QTypeInfo<Ts>::isRelocatable) && ...);
    static constexpr bool isPointer = false;
    static constexpr bool isIntegral = false;
};

#define Q_DECLARE_MOVABLE_CONTAINER(CONTAINER) \
template <typename ...T>                       \
class QTypeInfo<CONTAINER<T...>>               \
{                                              \
public:                                        \
    enum {                                     \
        isPointer = false,                     \
        isIntegral = false,                    \
        isComplex = true,                      \
        isRelocatable = true                   \
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

//前置声明
template <typename T> class QFlags;
//对QFLags做特化模板
template <typename T>
Q_DECLARE_TYPEINFO_BODY(QFlags<T>, Q_PRIMITIVE_TYPE)

/*
 * 定义特殊的共享的类型Q_DECLARE_SHARED
 * 类型必须包含swap成员，定义在Qt的命名空间下
 * */
#define Q_DECLARE_SHARED_IMPL(TYPE, FLAGS) \
    Q_DECLARE_TYPEINFO(TYPE, FLAGS);       \
    inline void swap(TYPE &value1, TYPE &value2) \
        noexcept(noexcept(value1.swap(value2)))  \
   { value1.swap(value2); }
#define Q_DECLARE_SHARED(TYPE) Q_DECLARE_SHARED_IMPL(TYPE, Q_RELOCATABLE_TYPE)
#define Q_DECLARE_SHARED_NOT_MOVABLE_UNTIL_QT6(TYPE) Q_DECLARE_SHARED_IMPL(TYPE, Q_RELOCATABLE_TYPE)

namespace QTypeTraits
{
    namespace detail {
        template <typename, typename = void>
        struct is_container : std::false_type {};
        template <typename T>
        struct is_container<T, std::void_t<
                typename T::value_type,
                std::is_convertible<decltype(std::declval<T>().begin() != std::declval<T>().end()), bool>
                >> : std::true_type {};

        //是否有 < 操作符
        template <typename, typename = void>
        struct has_operator_less_than : std::false_type {};
        template <typename T>
        struct has_operator_less_than<T, std::void_t<decltype(bool(std::declval<const T &>() < std::declval<const T &>()))>>
                : std::true_type {};
        template <typename T, bool = is_container<T>::value>
        struct expand_operator_less_than_container;
        template <typename T>
        struct expand_operator_less_than_tuple;

        template <typename T>
        using expand_operator_less_than = expand_operator_less_than_container<T>;

        template <typename T, bool>
        struct expand_operator_less_than_container : expand_operator_less_than_tuple<T> {};
        template <typename T>
        struct expand_operator_less_than_container<T, true> : std::conjunction <
                std::disjunction <
                    std::is_same<T, typename T::value_type>,
                    expand_operator_less_than<typename T::value_type>
                >, expand_operator_less_than_tuple<T>
                > {};
        template <typename ...T>
        using expand_operator_less_than_recursive = std::conjunction<expand_operator_less_than<T>...>;
        template <typename T>
        struct expand_operator_less_than_tuple : has_operator_less_than<T> {};
        template <typename T1, typename T2>
        struct expand_operator_less_than_tuple<std::pair<T1, T2>> : expand_operator_less_than_recursive<T1, T2> {};
        template <typename ...T>
        struct expand_operator_less_than_tuple<std::tuple<T...>> : expand_operator_less_than_recursive<T...> {};
        template <typename ...T>
        struct expand_operator_less_than_tuple<std::variant<T...>> : expand_operator_less_than_recursive<T...> {};

        //含有==操作符
        template <typename, typename = void>
        struct has_operator_equal : std::false_type {};  //默认为false
        //没有==操作符不会导致编译失败：https://zhuanlan.zhihu.com/p/21314708
        //SFINAE技术，Substitution failure is not an error
        //T虽然不能匹配这个这个类，但是可以匹配上面的std::false_type类型的类
        template <typename T>
        struct has_operator_equal<T, std::void_t<decltype(bool(std::declval<const T&>() == std::declval<const T&>()))>>
                : std::true_type {};

        template<typename T, bool = is_container<T>::value>
        struct expand_operator_equal_container;
        template<typename T>
        struct expand_operator_equal_tuple;

        // the entry point for the public method
        template<typename T>
        using expand_operator_equal = expand_operator_equal_container<T>;

        // if T isn't a container check if it's a tuple like object
        template<typename T, bool>
        struct expand_operator_equal_container : expand_operator_equal_tuple<T> {};
        // if T::value_type exists, check first T::value_type, then T itself
        template<typename T>
        struct expand_operator_equal_container<T, true> :
                std::conjunction<
                        std::disjunction<
                                std::is_same<T, typename T::value_type>, // avoid endless recursion
                                expand_operator_equal<typename T::value_type>
                        >, expand_operator_equal_tuple<T>> {};

        // recursively check the template arguments of a tuple like object
        template<typename ...T>
        using expand_operator_equal_recursive = std::conjunction<expand_operator_equal<T>...>;

        template<typename T>
        struct expand_operator_equal_tuple : has_operator_equal<T> {};
        template<typename T>
        struct expand_operator_equal_tuple<std::optional<T>> : has_operator_equal<T> {};
        template<typename T1, typename T2>
        struct expand_operator_equal_tuple<std::pair<T1, T2>> : expand_operator_equal_recursive<T1, T2> {};
        template<typename ...T>
        struct expand_operator_equal_tuple<std::tuple<T...>> : expand_operator_equal_recursive<T...> {};
        template<typename ...T>
        struct expand_operator_equal_tuple<std::variant<T...>> : expand_operator_equal_recursive<T...> {};
    }
    //struct TestOpeartorEqual {
    //    friend inline bool operator==(const TestOpeartorEqual &lhs, const TestOpeartorEqual &rhs) {
    //        return true;
    //    }
    //};
    //TestOpeartorEqual中写了operator==, 编译通过，没有重写，编译失败
    //qDebug()<<bool(std::declval<const TestOpeartorEqual &>() == std::declval<const TestOpeartorEqual &>());

    //是否含有operator== 操作符号
    template<typename T>
    struct has_operator_equal : detail::expand_operator_equal<T> {};
    template<typename T>
    inline constexpr bool has_operator_equal_v = has_operator_equal<T>::value;

    template <typename T>
    struct has_operator_less_than : detail::expand_operator_less_than<T> {};
    template <typename T>
    inline constexpr bool has_operator_less_than_v = has_operator_less_than<T>::value;

    template <typename Container, typename T>
    using has_operator_equal_container = std::disjunction<std::is_base_of<Container, T>, QTypeTraits::has_operator_equal<T>>;

    template <typename ...T>
    using compare_eq_result = std::enable_if_t<std::conjunction_v<QTypeTraits::has_operator_equal<T>...>, bool>;

    template <typename Container, typename ...T>
    using compare_eq_result_container = std::enable_if_t<std::conjunction_v<QTypeTraits::has_operator_equal_container<Container, T>...>, bool>;

    template <typename Container, typename T>
    using has_operator_less_than_container = std::disjunction<std::is_base_of<Container, T>, QTypeTraits::has_operator_less_than<T>>;

    template <typename ...T>
    using compare_lt_result = std::enable_if_t<std::conjunction_v<QTypeTraits::has_operator_less_than<T>...>, bool>;

    template <typename Container, typename ...T>
    using compare_lt_result_container = std::enable_if_t<std::conjunction_v<QTypeTraits::has_operator_less_than_container<Container, T>...>, bool>;
}


QT_END_NAMESPACE

#endif //QTYPEINFO_H
