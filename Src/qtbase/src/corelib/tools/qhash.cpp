//
// Created by Yujie Zhao on 2023/8/4.
//
#include "qhash.h"
#include "qhashfunctions.h"
#include <QtCore/qatomic.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>
#include <QtCore/qendian.h>
#include <QtCore/qbitarray.h>
#include <QtCore/private/qrandom_p.h>

QT_BEGIN_NAMESPACE

#if QT_POINTER_SIZE == 4
static inline uint murmurhash(const void *key, uint len, uint seed) noexcept {
    return 0;
}
#else
static inline uint64_t murmurhash(const void *key, uint64_t len, uint64_t seed) noexcept
{
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const unsigned char *data = reinterpret_cast<const unsigned char *>(key);
    const unsigned char *end = data + (len & ~7ul);

    while (data != end) {
        uint64_t k;
        memcpy(&k, data, sizeof(uint64_t));

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;

        data += 8;
    }

    len &= 7;
    if (len) {
        // handle the last few bytes of input
        size_t k = 0;
        end += len;

        while (data != end) {
            k <<= 8;
            k |= *data;
            ++data;
        }
        h ^= k;
        h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}
#endif

#if QT_POINTER_SIZE == 8
#define cROUNDS 1
#define dROUNDS 2

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define SIPROUND                                                               \
  do {                                                                         \
    v0 += v1;                                                                  \
    v1 = ROTL(v1, 13);                                                         \
    v1 ^= v0;                                                                  \
    v0 = ROTL(v0, 32);                                                         \
    v2 += v3;                                                                  \
    v3 = ROTL(v3, 16);                                                         \
    v3 ^= v2;                                                                  \
    v0 += v3;                                                                  \
    v3 = ROTL(v3, 21);                                                         \
    v3 ^= v0;                                                                  \
    v2 += v1;                                                                  \
    v1 = ROTL(v1, 17);                                                         \
    v1 ^= v2;                                                                  \
    v2 = ROTL(v2, 32);                                                         \
  } while (0)


static uint64_t siphash(const uint8_t *in, uint64_t inlen, const uint64_t seed)
{
    /* "somepseudorandomlygeneratedbytes" */
    uint64_t v0 = 0x736f6d6570736575ULL;
    uint64_t v1 = 0x646f72616e646f6dULL;
    uint64_t v2 = 0x6c7967656e657261ULL;
    uint64_t v3 = 0x7465646279746573ULL;
    uint64_t b;
    uint64_t k0 = seed;
    uint64_t k1 = seed ^ inlen;
    int i;
    const uint8_t *end = in + (inlen & ~7ULL);
    const int left = inlen & 7;
    b = inlen << 56;
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;

    for (; in != end; in += 8) {
        uint64_t m = qFromUnaligned<uint64_t>(in);
        v3 ^= m;

        for (i = 0; i < cROUNDS; ++i)
            SIPROUND;

        v0 ^= m;
    }


#if defined(Q_CC_GNU) && Q_CC_GNU >= 700
    QT_WARNING_DISABLE_GCC("-Wimplicit-fallthrough")
#endif
    switch (left) {
        case 7:
            b |= ((uint64_t)in[6]) << 48;
        case 6:
            b |= ((uint64_t)in[5]) << 40;
        case 5:
            b |= ((uint64_t)in[4]) << 32;
        case 4:
            b |= ((uint64_t)in[3]) << 24;
        case 3:
            b |= ((uint64_t)in[2]) << 16;
        case 2:
            b |= ((uint64_t)in[1]) << 8;
        case 1:
            b |= ((uint64_t)in[0]);
            break;
        case 0:
            break;
    }

    v3 ^= b;

    for (i = 0; i < cROUNDS; ++i)
        SIPROUND;

    v0 ^= b;

    v2 ^= 0xff;

    for (i = 0; i < dROUNDS; ++i)
        SIPROUND;

    b = v0 ^ v1 ^ v2 ^ v3;
    return b;
}
#else

#endif

static QBasicAtomicInteger<size_t> qt_qhash_seed = Q_BASIC_ATOMIC_INITIALIZER(0);

enum HashCreationMode {
    Initial,
    Reseed
};

static size_t getEnvironmentSeed(bool &ok) {
#pragma message("getEnvironmentSeed : qEnvironmentVariableIntValue 未实现")
    auto seed = qEnvironmentVariableIntValue("QT_HASH_SEED", &ok);
    return seed;
}

static size_t qt_create_qhash_seed(HashCreationMode mode) {
    size_t seed = 0;
    bool ok;
    seed = getEnvironmentSeed(ok);
    if (ok) {
        if (seed) {
        }
        seed = 1;
    }
    else if (mode == Initial) {
        auto data = qt_initial_random_value();
        seed = data.data[0] ^ data.data[1];
    }
    else {
        Q_ASSERT(false);
//        if (sizeof(seed) > sizeof(uint)) {
//            seed = QRandomGenerator::system()->generate64();
//        } else {
//            seed = QRandomGenerator::system()->generate();
//        }
    }
    return seed;
}

static size_t qt_initialize_qhash_seed() {
    size_t theirSeed;   //无锁编程，如果被其他线程设置了，直接使用其他线程的
    size_t ourSeed = qt_create_qhash_seed(Initial);
    if (qt_qhash_seed.testAndSetRelaxed(0, ourSeed, theirSeed)) {
        return ourSeed;
    }
    return theirSeed;
}

QHashSeed QHashSeed::globalSeed() noexcept
{
    size_t seed = qt_qhash_seed.loadRelaxed();
    if (Q_UNLIKELY(seed == 0)) {
        seed = qt_initialize_qhash_seed();
    }
    return { seed - 1 };  //zhaoyujie TODO 为什么要-1 ？
}

size_t qHash(double key, size_t seed) noexcept {
    key += 0.0;  //-0.0 to 0.0
    if constexpr(sizeof(double) == sizeof(size_t)) {
        size_t k;
        memcpy(&k, &key, sizeof(double));
        return QHashPrivate::hash(key, seed);
    }
    else {
        return murmurhash(&key, sizeof(key), seed);
    }
}

size_t qHashBits(const void *p, size_t size, size_t seed) noexcept {
    if (size <= QT_POINTER_SIZE) {
        return murmurhash(p, size, seed);
    }
    else {
        return siphash(reinterpret_cast<const uchar *>(p), size, seed);
    }
}

size_t qHash(const QByteArray &key, size_t seed) noexcept {
    return qHashBits(key.constData(), size_t(key.size()), seed);
}

size_t qHash(const QByteArrayView &key, size_t seed) noexcept {
    return qHashBits(key.constData(), size_t(key.size()), seed);
}

size_t qHash(QStringView key, size_t seed) noexcept {
    return qHashBits(key.data(), key.size()*sizeof(QChar), seed);
}

size_t qHash(const QBitArray &bitArray, size_t seed) noexcept
{
    qsizetype m = bitArray.d.size() - 1;
    size_t result = qHashBits(reinterpret_cast<const uchar *>(bitArray.d.constData()), size_t(qMax(0, m)), seed);

    // deal with the last 0 to 7 bits manually, because we can't trust that
    // the padding is initialized to 0 in bitArray.d
    qsizetype n = bitArray.size();
    if (n & 0x7)
        result = ((result << 4) + bitArray.d.at(m)) & ((1 << n) - 1);
    return result;
}

size_t qHash(QLatin1String key, size_t seed) noexcept
{
    return qHashBits(reinterpret_cast<const uchar *>(key.data()), size_t(key.size()), seed);
}

QT_END_NAMESPACE