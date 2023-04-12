//
// Created by Yujie Zhao on 2023/2/6.
//

#ifndef QGLOBAL_H
#define QGLOBAL_H

#include <assert.h>
#include <stdint.h>
#include <cstddef>
#include <type_traits>

#include "qcompilerdetection.h"

#define QT_PREPEND_NAMESPACE(name) ::name

#define QT_USE_NAMESPACE
#define QT_BEGIN_NAMESPACE
#define QT_END_NAMESPACE

#define QT_BEGIN_INCLUDE_NAMESPACE
#define QT_END_INCLUDE_NAMESPACE

#ifdef __cpp_conditional_explicit
#define Q_EXPLICIT explicit(false)
#else
#define Q_EXPLICIT
#endif

//禁止拷贝构造
#define Q_DISABLE_COPY(Class) \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

//禁止移动构造与拷贝构造
#define Q_DISABLE_COPY_MOVE(class) \
    Q_DISABLE_COPY(Class)          \
    Class(Class &&) = delete;      \
    Class &operator=(Class &&) = delete;


//将非const对象转变为const对象
template <typename T>
constexpr typename std::add_const<T>::type &qAsConst(T &t) noexcept {return t;}
//右值引用不允许使用
template <typename T>
void qAsConst(const T &&) = delete;

template <typename T, typename U = T>
constexpr T qExchange(T &t, U &&newValue)
{
    T old = std::move(t);
    t = std::forward<U>(newValue);
    return old;
}


//如果没有定义过QT_NO_FOREACH宏，则定义Q_FOREACH宏。
//Q_FOREACH会复制一份容器，如果是Qt的容器，容器里存放的是指针，效率比较高。如果是std的容器，则效率低下
#ifndef QT_NO_FOREACH
namespace QtPrivate {

template <typename T>
class QForeachContainer {
    Q_DISABLE_COPY(QForeachContainer)
public:
    QForeachContainer(const T &t)
        : c(t)
        , i(qAsConst(c).begin())
        , e(qAsConst(c).end()) {

    }

    QForeachContainer(T &&t)
        : c(std::move(t))
        , i(qAsConst(c).begin())
        , e(qAsConst(c).end()) {

    }

    QForeachContainer(QForeachContainer &&other)
        : c(std::move(other.c))
        , i(qAsConst(c).begin())
        , e(qAsConst(c).end())
        , control(std::move(other.control)) {

    }

    QForeachContainer &operator=(QForeachContainer &&other) {
        c = std::move(other.c);
        i = qAsConst(c).begin();
        e = qAsConst(c).end();
        control = std::move(other.control);
        return *this;
    }

    T c;
    typename T::const_iterator i, e;
    int control = 1;
};
template <typename T, typename = decltype(std::declval<T>.detach())>
inline void warnIfContainerIsNotShared(int) {}

template <typename T>
inline void warnIfContainerIsNotShared(...) {}

//std::decay<T> 为T应用左值到右值，数组到指针，函数指针的隐式转换，转换将移除类型T的cv限定符
template <typename T>
QForeachContainer<typename std::decay<T>::type> qMakeForeachContainer(T &&t)
{
    warnIfContainerIsNotShared<typename std::decay<T>::type>(0);
    return QForeachContainer<typename std::decay<T>::type>(std::forward<T>(t));
}
}

#define Q_FOREACH(variable, container) \
for (auto  _container_ = QtPrivate::qMakeForeachContainer(container); _container_.i != _container_.e; ++_container_.i) \
    if (variable = *_container_.i; false) {                                                                             \
    }                                  \
    else

#endif //QT_NO_FOREACH

//死循环
#define Q_FOREVER for(;;)

//定义foreach / forever关键词方便代码编写
#ifndef QT_NO_KEYWORDS
# ifndef QT_NO_FOREACH
#  ifndef foreach
#    define foreach Q_FOREACH
#  endif
# endif //QT_NO_FOREACH
# ifndef forever
#  define forever Q_FOREVER
# endif
#endif //QT_NO_KEYWORDS


#define Q_ASSERT(cond) assert(cond)

QT_BEGIN_NAMESPACE

typedef signed char qint8;
typedef unsigned char quint8;
typedef short qint16;
typedef unsigned short quint16;
typedef int qint32;
typedef unsigned int quint32;
typedef double qreal;

#ifdef __cplusplus
# define Q_INT64_C(c) static_cast<long long>(c ## LL)
# define Q_UINT64_C(c) static_cast<unsigned long long>(c ## LL)
#else
# define Q_INT64_C(c) ((long long)(c ## LL))
# define Q_UINT64_C(c) ((unsigned long long)(c ## LL))
#endif
typedef long long qint64;
typedef unsigned long long quint64;
typedef qint64 qlonglong;
typedef quint64 qulonglong;

#ifndef __cplusplus
typedef ptrdiff_t qptrdiff;
typedef ptrdiff_t qsizetype;
typedef ptrdiff_t qintptr;
typedef size_t quintptr;
#endif

#ifdef __cplusplus
template <int> struct QIntegerForSize;
template <>    struct QIntegerForSize<1> {typedef quint8 Unsigned;  typedef qint8 Signed;  };
template <>    struct QIntegerForSize<2> {typedef quint16 Unsigned; typedef qint16 Signed; };
template <>    struct QIntegerForSize<4> {typedef quint32 Unsigned; typedef qint32 Signed; };
template <>    struct QIntegerForSize<8> {typedef quint64 Unsigned; typedef qint64 Signed; };

#if defined(__GNUC__) && defined(__SIZEOF_INT128__)
template <>    struct QIntegerForSize<16>{__extension__ typedef unsigned __int128 Unsigned; __extension__ typedef __int128 Signed; };
#endif

//zhaoyujie TODO 这段代码啥意思？qptrdiff是qint8类型？
template <class T>
struct QIntegerForSizeof : QIntegerForSize<sizeof(T)>{ };
typedef QIntegerForSizeof<void *>::Unsigned quintptr;
typedef QIntegerForSizeof<void *>::Signed qptrdiff;
typedef qptrdiff qintptr;
using qsizetype = QIntegerForSizeof<std::size_t>::Signed;

#endif //__cplusplus

//定义一些unsigned 头文件为简略写法
QT_BEGIN_INCLUDE_NAMESPACE
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

QT_END_INCLUDE_NAMESPACE

QT_END_NAMESPACE

#ifdef Q_CC_MSVC
#define Q_NEVER_INLINE __declspec(noinline)
#define Q_ALWAYS_INLINE __forceinline
#elif defined(Q_CC_GNU)
#define Q_NEVER_INLINE __attribute((noinline))
#define Q_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define Q_NEVER_INLINE
#define Q_ALWAYS_INLINE inline
#endif

//TODO
#define QT_WARNING_PUSH
#define QT_WARNING_POP
#define QT_WARNING_DISABLE_MSVC(number)

void qt_assert(const char *assertion, const char *file, int line) noexcept;
void qt_assert_x(const char *where, const char *what, const char *file, int line) noexcept;

#define Q_UNUSED(x) (void)x;

#define QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(Class) \
    Class &operator=(Class &&other) noexcept {                    \
        Class moved(std::move(other));                            \
        swap(moved);                                              \
        return *this;                                             \
    }

#define QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(Class) \
    Class &operatpr=(Class &&other) noexcept {               \
        swap(other);                                         \
        return *this;                                        \
    }

template <typename T>
inline void qSwap(T &value1, T &value2)
{
    using std::swap;
    swap(value1, value2);
}

void qt_check_pointer(const char *, int) noexcept;

#define Q_CHECK_PTR(p) do { if (!(p)) qt_check_pointer(__FILE__, __LINE__); } while(false)

template <typename T>
constexpr inline const T &qMin(const T &a, const T &b) { return (a < b) ? a : b; }
template <typename T>
constexpr inline const T &qMax(const T &a, const T &b) { return (a < b) ? b : a; }
template <typename T>
//Bound: 边界
constexpr inline const T &qBound(const T &min, const T &val, const T &max)
{ return qMax(min, qMin(max, val)); }

#endif //QGLOBAL_H
