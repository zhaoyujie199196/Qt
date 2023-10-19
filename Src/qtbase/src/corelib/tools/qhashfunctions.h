//
// Created by Yujie Zhao on 2023/8/4.
//

#ifndef QHASHFUNCTIONS_H
#define QHASHFUNCTIONS_H

#include <QtCore/qstring.h>
#include <QtCore/qpair.h>

#include <numeric>
#include <functional>

#if defined(Q_CC_MSVC)
#pragma warning ( push )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4127 )
#endif

QT_BEGIN_NAMESPACE

class QBitArray;
class QByteArray;
class QString;
class QLatin1String;

struct QHashSeed
{
    constexpr QHashSeed(size_t d = 0)
        : data(d)
    {}
    constexpr operator size_t() const noexcept { return data; }
    static QHashSeed globalSeed() noexcept;

private:
    size_t data;
};

namespace QHashPrivate {

    template <typename T, typename = void>
    constexpr inline bool HasQHashSingleArgOverload = false;
    template <typename T>
    constexpr inline bool HasQHashSingleArgOverload<T, std::enable_if_t<
            std::is_convertible_v<decltype(qHash(std::declval<const T &>())), size_t>
    >> = true;

    //对size_t做hash
    Q_DECL_CONST_FUNCTION constexpr size_t hash(size_t key, size_t seed) noexcept
    {
        key ^= seed;
        if constexpr (sizeof(size_t) == 4) {
            key ^= key >> 16;
            key *= UINT32_C(0x45d9f3b);
            key ^= key >> 16;
            key *= UINT32_C(0x45d9f3b);
            key ^= key >> 16;
            return key;
        } else {
            quint64 key64 = key;
            key64 ^= key64 >> 32;
            key64 *= UINT64_C(0xd6e8feb86659fd93);
            key64 ^= key64 >> 32;
            key64 *= UINT64_C(0xd6e8feb86659fd93);
            key64 ^= key64 >> 32;
            return size_t(key64);
        }
    }
}

template <typename T>
bool qHashEquals(const T &a, const T &b)
{
    return a == b;
}

//c++内置类型的qHashfangfa
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(char key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(uchar key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(signed char key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(ushort key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(short key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(uint key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(int key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(ulong key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(long key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(quint64 key, size_t seed = 0) noexcept {
    if constexpr (sizeof(quint64) > sizeof(size_t)) {
        key ^ (key >> 32);
    }
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(qint64 key, size_t seed = 0) noexcept {
    return qHash(quint64(key), seed);
}
Q_DECL_CONST_FUNCTION inline size_t qHash(float key, size_t seed = 0) noexcept {
    // ensure -0 gets mapped to 0
    key += 0.0f;
    uint k;
    memcpy(&k, &key, sizeof(float));
    return QHashPrivate::hash(k, seed);
}
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION size_t qHash(double key, size_t seed = 0) noexcept ;
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(wchar_t key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(char16_t key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(char32_t key, size_t seed = 0) noexcept {
    return QHashPrivate::hash(size_t(key), seed);
}
#ifdef __cpp_char8_t
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(char8_t key, size_t seed = 0) noexcept  {
    return QHashPrivate::hash(size_t(key), seed);
}
#endif
template <class T>
inline size_t qHash(const T *key, size_t seed = 0) noexcept  {
    return qHash(reinterpret_cast<quintptr>(key), seed);
}
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(std::nullptr_t, size_t seed = 0) noexcept  {
    return seed;
}

//一些Qt的类型
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(const QChar key, size_t seed = 0) noexcept {
    return qHash(key.unicode(), seed);
}
Q_CORE_EXPORT Q_DECL_PURE_FUNCTION size_t qHash(const QByteArray &key, size_t seed = 0) noexcept;
Q_CORE_EXPORT Q_DECL_PURE_FUNCTION size_t qHash(const QByteArrayView &key, size_t seed = 0) noexcept ;
Q_CORE_EXPORT Q_DECL_PURE_FUNCTION size_t qHash(QStringView key, size_t seed = 0) noexcept;
inline Q_DECL_PURE_FUNCTION size_t qHash(const QString &key, size_t seed = 0) noexcept {
    return qHash(QStringView{key}, seed);
}
Q_CORE_EXPORT Q_DECL_PURE_FUNCTION size_t qHash(const QBitArray &key, size_t seed = 0) noexcept;
Q_CORE_EXPORT Q_DECL_PURE_FUNCTION size_t qHash(QLatin1String key, size_t seed = 0) noexcept;
//Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(QKeyCombination key, size_t seed = 0) noexcept {
//    return qHash(key.toCombined(), seed);
//}
template <typename Enum>
Q_DECL_CONST_FUNCTION constexpr inline size_t qHash(QFlags<Enum> flags, size_t seed = 0) noexcept {
    return qHash(flags.toInt(), seed);
}

template <typename T, std::enable_if_t<QHashPrivate::HasQHashSingleArgOverload<T>, bool> = true>
size_t qHash(const T &t, size_t seed) noexcept(noexcept(qHash(t))) {
    return qHash(t) ^ seed;
}

namespace QtPrivate {

    struct QHashCombine {
        typedef size_t result_type;
        template <typename T>
        constexpr result_type operator()(size_t seed, const T &t) const noexcept {
            return seed ^ (qHash(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
        }
    };
}

QT_END_NAMESPACE

#endif //QHASHFUNCTIONS_H