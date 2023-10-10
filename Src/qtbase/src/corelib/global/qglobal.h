//
// Created by Yujie Zhao on 2023/2/6.
//

#ifndef QGLOBAL_H
#define QGLOBAL_H

#include <assert.h>
#include <stdint.h>
#include <cstddef>
#include <type_traits>
#include <cmath>

#include <QtCore/qconfig.h>
#include <QtCore/qtcore-config.h>

#include "qcompilerdetection.h"
#include "qprocessordetection.h"

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

//禁止内联
#ifdef Q_CC_MSVC
#  define Q_NEVER_INLINE __declspec(noinline)
#  define Q_ALWAYS_INLINE __forceinline
#elif defined(Q_CC_GNU)
#  define Q_NEVER_INLINE __attribute__((noinline))
#  define Q_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#  define Q_NEVER_INLINE
#  define Q_ALWAYS_INLINE inline
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

#define QT_CONFIG(feature) (1/QT_FEATURE_##feature == 1)

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

#define QT_TRY try
#define QT_CATCH(A) catch(A)
#define QT_THROW(A) throw A

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
    Class &operator=(Class &&other) noexcept {               \
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

namespace QTypeTraits {
    namespace detail {
        template <typename T, typename U,
                typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> &&
                        std::is_floating_point_v<T> == std::is_floating_point_v<U> &&
                                std::is_signed_v<T> == std::is_signed_v<U> &&
                                        !std::is_same_v<T, bool> && !std::is_same_v<U, bool> &&
                                                !std::is_same_v<T, char> && !std::is_same_v<U, char>>>
                                                struct Promoted
                                                {
                                                    using type = decltype(T() + U());
                                                };

    };
    template <typename T, typename U>
    using Promoted = typename detail::Promoted<T, U>::type;
}

template <typename T>
constexpr inline const T &qMin(const T &a, const T &b) { return (a < b) ? a : b; }
template <typename T, typename U>
const inline QTypeTraits::Promoted<T, U> qMin(const T &a, const U &b)
{
    using P = QTypeTraits::Promoted<T, U>;
    P _a = a;
    P _b = b;
    return (_a < _b) ? _a : _b;
}

template <typename T>
constexpr inline const T &qMax(const T &a, const T &b) { return (a < b) ? b : a; }
template <typename T, typename U>
constexpr inline QTypeTraits::Promoted<T, U> qMax(const T &a, const U &b)
{
    using P = QTypeTraits::Promoted<T, U>;
    P _a = a;
    P _b = b;
    return (_a < _b) ? _b : _a;
}

//Bound: 边界
template <typename T>
constexpr inline const T &qBound(const T &min, const T &val, const T &max)
{ return qMax(min, qMin(max, val)); }


constexpr inline int qRound(double d)
{ return d >= 0.0 ? int(d + 0.5) : int(d - 0.5); }
constexpr inline int qRound(float d)
{ return d >= 0.0f ? int(d + 0.5f) : int(d - 0.5f); }
constexpr inline qint64 qRound64(double d)
{ return d >= 0.0 ? qint64(d + 0.5) : qint64(d - 0.5); }
constexpr inline qint64 qRound64(float d)
{ return d >= 0.0f ? qint64(d + 0.5f) : qint64(d - 0.5f); }

template <typename T>
const inline const T qAbs(const T &t)
{ return abs(t); }

template <typename T>
const inline const T qFloor(const T &t)
{ return std::floor(t); }

template <typename T>
const inline const T qCeil(const T &t)
{ return std::ceil(t); }

// printf格式检查
// enable gcc warnings for printf-style functions
#if defined(Q_CC_GNU) && !defined(__INSURE__)
#  if defined(Q_CC_MINGW) && !defined(Q_CC_CLANG)
#    define Q_ATTRIBUTE_FORMAT_PRINTF(A, B) \
         __attribute__((format(gnu_printf, (A), (B))))
#  else
#    define Q_ATTRIBUTE_FORMAT_PRINTF(A, B) \
         __attribute__((format(printf, (A), (B))))
#  endif
#else
#  define Q_ATTRIBUTE_FORMAT_PRINTF(A, B)
#endif

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  ifdef QT_STATIC
#    error "Both QT_SHARED and QT_STATIC defined, please make up your mind"
#  endif
#  ifndef QT_SHARED
#    define QT_SHARED
#  endif
#  if defined(QT_BUILD_CORE_LIB)
#    define Q_CORE_EXPORT Q_DECL_EXPORT
#  else
#    define Q_CORE_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_CORE_EXPORT
#endif

#include <QtCore/qtypeinfo.h>

#include <QtCore/qflags.h>

#include <QtCore/qatomic.h>

#endif //QGLOBAL_H
