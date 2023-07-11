//
// Created by Yujie Zhao on 2023/6/7.
//
#include "qstringconverter.h"
#include "qstringconverter_p.h"

QT_BEGIN_NAMESPACE

enum { Endian = 0, Data = 1 };
//UTF-8编码中的特殊字符，字节顺序标记
static const uchar utf8bom[] = { 0xef, 0xbb, 0xbf };

#if defined(__SSE2__) && defined(QT_COMPILER_SUPPORTS_SSE2)
static inline bool simdEncodeAscii(uchar *&dst, const ushort *&nextAscii, const ushort *&src, const ushort *end)
{
    // do sixteen characters at a time
    for ( ; end - src >= 16; src += 16, dst += 16) {
#  ifdef __AVX2__
        __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src));
        __m128i data1 = _mm256_castsi256_si128(data);
        __m128i data2 = _mm256_extracti128_si256(data, 1);
#  else
        __m128i data1 = _mm_loadu_si128((const __m128i*)src);
        __m128i data2 = _mm_loadu_si128(1+(const __m128i*)src);
#  endif

        // check if everything is ASCII
        // the highest ASCII value is U+007F
        // Do the packing directly:
        // The PACKUSWB instruction has packs a signed 16-bit integer to an unsigned 8-bit
        // with saturation. That is, anything from 0x0100 to 0x7fff is saturated to 0xff,
        // while all negatives (0x8000 to 0xffff) get saturated to 0x00. To detect non-ASCII,
        // we simply do a signed greater-than comparison to 0x00. That means we detect NULs as
        // "non-ASCII", but it's an acceptable compromise.
        __m128i packed = _mm_packus_epi16(data1, data2);
        __m128i nonAscii = _mm_cmpgt_epi8(packed, _mm_setzero_si128());

        // store, even if there are non-ASCII characters here
        _mm_storeu_si128((__m128i*)dst, packed);

        // n will contain 1 bit set per character in [data1, data2] that is non-ASCII (or NUL)
        ushort n = ~_mm_movemask_epi8(nonAscii);
        if (n) {
            // find the next probable ASCII character
            // we don't want to load 32 bytes again in this loop if we know there are non-ASCII
            // characters still coming
            nextAscii = src + qBitScanReverse(n) + 1;

            n = qCountTrailingZeroBits(n);
            dst += n;
            src += n;
            return false;
        }
    }

    if (end - src >= 8) {
        // do eight characters at a time
        __m128i data = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src));
        __m128i packed = _mm_packus_epi16(data, data);
        __m128i nonAscii = _mm_cmpgt_epi8(packed, _mm_setzero_si128());

        // store even non-ASCII
        _mm_storel_epi64(reinterpret_cast<__m128i *>(dst), packed);

        uchar n = ~_mm_movemask_epi8(nonAscii);
        if (n) {
            nextAscii = src + qBitScanReverse(n) + 1;
            n = qCountTrailingZeroBits(n);
            dst += n;
            src += n;
            return false;
        }
    }

    return src == end;
}

static inline bool simdDecodeAscii(ushort *&dst, const uchar *&nextAscii, const uchar *&src, const uchar *end)
{
    // do sixteen characters at a time
    for ( ; end - src >= 16; src += 16, dst += 16) {
        __m128i data = _mm_loadu_si128((const __m128i*)src);

#ifdef __AVX2__
        const int BitSpacing = 2;
        // load and zero extend to an YMM register
        const __m256i extended = _mm256_cvtepu8_epi16(data);

        uint n = _mm256_movemask_epi8(extended);
        if (!n) {
            // store
            _mm256_storeu_si256((__m256i*)dst, extended);
            continue;
        }
#else
        const int BitSpacing = 1;

        // check if everything is ASCII
        // movemask extracts the high bit of every byte, so n is non-zero if something isn't ASCII
        uint n = _mm_movemask_epi8(data);
        if (!n) {
            // unpack
            _mm_storeu_si128((__m128i*)dst, _mm_unpacklo_epi8(data, _mm_setzero_si128()));
            _mm_storeu_si128(1+(__m128i*)dst, _mm_unpackhi_epi8(data, _mm_setzero_si128()));
            continue;
        }
#endif

        // copy the front part that is still ASCII
        while (!(n & 1)) {
            *dst++ = *src++;
            n >>= BitSpacing;
        }

        // find the next probable ASCII character
        // we don't want to load 16 bytes again in this loop if we know there are non-ASCII
        // characters still coming
        n = qBitScanReverse(n);
        nextAscii = src + (n / BitSpacing) + 1;
        return false;

    }

    if (end - src >= 8) {
        __m128i data = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(src));
        uint n = _mm_movemask_epi8(data) & 0xff;
        if (!n) {
            // unpack and store
            _mm_storeu_si128(reinterpret_cast<__m128i *>(dst), _mm_unpacklo_epi8(data, _mm_setzero_si128()));
        } else {
            while (!(n & 1)) {
                *dst++ = *src++;
                n >>= 1;
            }

            n = qBitScanReverse(n);
            nextAscii = src + n + 1;
            return false;
        }
    }

    return src == end;
}

static inline const uchar *simdFindNonAscii(const uchar *src, const uchar *end, const uchar *&nextAscii)
{
#ifdef __AVX2__
    // do 32 characters at a time
    // (this is similar to simdTestMask in qstring.cpp)
    const __m256i mask = _mm256_set1_epi8(0x80);
    for ( ; end - src >= 32; src += 32) {
        __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src));
        if (_mm256_testz_si256(mask, data))
            continue;

        uint n = _mm256_movemask_epi8(data);
        Q_ASSUME(n);

        // find the next probable ASCII character
        // we don't want to load 32 bytes again in this loop if we know there are non-ASCII
        // characters still coming
        nextAscii = src + qBitScanReverse(n) + 1;

        // return the non-ASCII character
        return src + qCountTrailingZeroBits(n);
    }
#endif

    // do sixteen characters at a time
    for ( ; end - src >= 16; src += 16) {
        __m128i data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));

        // check if everything is ASCII
        // movemask extracts the high bit of every byte, so n is non-zero if something isn't ASCII
        uint n = _mm_movemask_epi8(data);
        if (!n)
            continue;

        // find the next probable ASCII character
        // we don't want to load 16 bytes again in this loop if we know there are non-ASCII
        // characters still coming
        nextAscii = src + qBitScanReverse(n) + 1;

        // return the non-ASCII character
        return src + qCountTrailingZeroBits(n);
    }

    // do four characters at a time
    for ( ; end - src >= 4; src += 4) {
        quint32 data = qFromUnaligned<quint32>(src);
        data &= 0x80808080U;
        if (!data)
            continue;

        // We don't try to guess which of the three bytes is ASCII and which
        // one isn't. The chance that at least two of them are non-ASCII is
        // better than 75%.
        nextAscii = src;
        return src;
    }
    nextAscii = end;
    return src;
}

// Compare only the US-ASCII beginning of [src8, end8) and [src16, end16)
// and advance src8 and src16 to the first character that could not be compared
static void simdCompareAscii(const char8_t *&src8, const char8_t *end8, const char16_t *&src16, const char16_t *end16)
{
    int bitSpacing = 1;
    qptrdiff len = qMin(end8 - src8, end16 - src16);
    qptrdiff offset = 0;
    uint mask = 0;

    // do sixteen characters at a time
    for ( ; offset + 16 < len; offset += 16) {
        __m128i data8 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src8 + offset));
#ifdef __AVX2__
        // AVX2 version, use 256-bit registers and VPMOVXZBW
        __m256i data16 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src16 + offset));

        // expand US-ASCII as if it were Latin1 and confirm it's US-ASCII
        __m256i datax8 = _mm256_cvtepu8_epi16(data8);
        mask = _mm256_movemask_epi8(datax8);
        if (mask)
            break;

        // compare Latin1 to UTF-16
        __m256i latin1cmp = _mm256_cmpeq_epi16(datax8, data16);
        mask = ~_mm256_movemask_epi8(latin1cmp);
        if (mask)
            break;
#else
        // non-AVX2 code
        __m128i datalo16 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src16 + offset));
        __m128i datahi16 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src16 + offset) + 1);

        // expand US-ASCII as if it were Latin1, we'll confirm later
        __m128i datalo8 = _mm_unpacklo_epi8(data8, _mm_setzero_si128());
        __m128i datahi8 = _mm_unpackhi_epi8(data8, _mm_setzero_si128());

        // compare Latin1 to UTF-16
        __m128i latin1cmplo = _mm_cmpeq_epi16(datalo8, datalo16);
        __m128i latin1cmphi = _mm_cmpeq_epi16(datahi8, datahi16);
        mask = _mm_movemask_epi8(latin1cmphi) << 16;
        mask |= ushort(_mm_movemask_epi8(latin1cmplo));
        mask = ~mask;
        if (mask)
            break;

        // confirm it was US-ASCII
        mask = _mm_movemask_epi8(data8);
        if (mask) {
            bitSpacing = 0;
            break;
        }
#endif
    }

    // helper for comparing 4 or 8 characters
    auto cmp_lt_16 = [&mask, &offset](int n, __m128i data8, __m128i data16) {
        // n = 4  ->  sizemask = 0xff
        // n = 8  ->  sizemask = 0xffff
        unsigned sizemask = (1U << (2 * n)) - 1;

        // expand as if Latin1
        data8 = _mm_unpacklo_epi8(data8, _mm_setzero_si128());

        // compare and confirm it's US-ASCII
        __m128i latin1cmp = _mm_cmpeq_epi16(data8, data16);
        mask = ~_mm_movemask_epi8(latin1cmp) & sizemask;
        mask |= _mm_movemask_epi8(data8);
        if (mask == 0)
            offset += n;
    };

    // do eight characters at a time
    if (mask == 0 && offset + 8 < len) {
        __m128i data8 = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(src8 + offset));
        __m128i data16 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src16 + offset));
        cmp_lt_16(8, data8, data16);
    }

    // do four characters
    if (mask == 0 && offset + 4 < len) {
        __m128i data8 = _mm_cvtsi32_si128(qFromUnaligned<quint32>(src8 + offset));
        __m128i data16 = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(src16 + offset));
        cmp_lt_16(4, data8, data16);
    }

    // correct the source pointers to point to the first character we couldn't deal with
    if (mask)
        offset += qCountTrailingZeroBits(mask) >> bitSpacing;
    src8 += offset;
    src16 += offset;
}
#elif defined(__ARM_NEON__)
static inline bool simdEncodeAscii(uchar *&dst, const ushort *&nextAscii, const ushort *&src, const ushort *end)
{
    return false;
//    uint16x8_t maxAscii = vdupq_n_u16(0x7f);
//    uint16x8_t mask1 = { 1,      1 << 2, 1 << 4, 1 << 6, 1 << 8, 1 << 10, 1 << 12, 1 << 14 };
//    uint16x8_t mask2 = vshlq_n_u16(mask1, 1);
//
//    // do sixteen characters at a time
//    for ( ; end - src >= 16; src += 16, dst += 16) {
//        // load 2 lanes (or: "load interleaved")
//        uint16x8x2_t in = vld2q_u16(src);
//
//        // check if any of the elements > 0x7f, select 1 bit per element (element 0 -> bit 0, element 1 -> bit 1, etc),
//        // add those together into a scalar, and merge the scalars.
//        uint16_t nonAscii = vaddvq_u16(vandq_u16(vcgtq_u16(in.val[0], maxAscii), mask1))
//                            | vaddvq_u16(vandq_u16(vcgtq_u16(in.val[1], maxAscii), mask2));
//
//        // merge the two lanes by shifting the values of the second by 8 and inserting them
//        uint16x8_t out = vsliq_n_u16(in.val[0], in.val[1], 8);
//
//        // store, even if there are non-ASCII characters here
//        vst1q_u8(dst, vreinterpretq_u8_u16(out));
//
//        if (nonAscii) {
//            // find the next probable ASCII character
//            // we don't want to load 32 bytes again in this loop if we know there are non-ASCII
//            // characters still coming
//            nextAscii = src + qBitScanReverse(nonAscii) + 1;
//
//            nonAscii = qCountTrailingZeroBits(nonAscii);
//            dst += nonAscii;
//            src += nonAscii;
//            return false;
//        }
//    }
//    return src == end;
}

static inline bool simdDecodeAscii(ushort *&dst, const uchar *&nextAscii, const uchar *&src, const uchar *end)
{
    return false;
//    // do eight characters at a time
//    uint8x8_t msb_mask = vdup_n_u8(0x80);
//    uint8x8_t add_mask = { 1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7 };
//    for ( ; end - src >= 8; src += 8, dst += 8) {
//        uint8x8_t c = vld1_u8(src);
//        uint8_t n = vaddv_u8(vand_u8(vcge_u8(c, msb_mask), add_mask));
//        if (!n) {
//            // store
//            vst1q_u16(dst, vmovl_u8(c));
//            continue;
//        }
//
//        // copy the front part that is still ASCII
//        while (!(n & 1)) {
//            *dst++ = *src++;
//            n >>= 1;
//        }
//
//        // find the next probable ASCII character
//        // we don't want to load 16 bytes again in this loop if we know there are non-ASCII
//        // characters still coming
//        n = qBitScanReverse(n);
//        nextAscii = src + n + 1;
//        return false;
//
//    }
//    return src == end;
}

static inline const uchar *simdFindNonAscii(const uchar *src, const uchar *end, const uchar *&nextAscii)
{
    return nullptr;
//    // The SIMD code below is untested, so just force an early return until
//    // we've had the time to verify it works.
//    nextAscii = end;
//    return src;
//
//    // do eight characters at a time
//    uint8x8_t msb_mask = vdup_n_u8(0x80);
//    uint8x8_t add_mask = { 1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7 };
//    for ( ; end - src >= 8; src += 8) {
//        uint8x8_t c = vld1_u8(src);
//        uint8_t n = vaddv_u8(vand_u8(vcge_u8(c, msb_mask), add_mask));
//        if (!n)
//            continue;
//
//        // find the next probable ASCII character
//        // we don't want to load 16 bytes again in this loop if we know there are non-ASCII
//        // characters still coming
//        nextAscii = src + qBitScanReverse(n) + 1;
//
//        // return the non-ASCII character
//        return src + qCountTrailingZeroBits(n);
//    }
//    nextAscii = end;
//    return src;
}

static void simdCompareAscii(const char8_t *&, const char8_t *, const char16_t *&, const char16_t *)
{
}
#else
static inline bool simdEncodeAscii(uchar *, const ushort *, const ushort *, const ushort *)
{
    return false;
}

static inline bool simdDecodeAscii(ushort *, const uchar *, const uchar *, const uchar *)
{
    return false;
}

static inline const uchar *simdFindNonAscii(const uchar *src, const uchar *end, const uchar *&nextAscii)
{
    nextAscii = end;
    return src;
}

static void simdCompareAscii(const char8_t *&, const char8_t *, const char16_t *&, const char16_t *)
{
}
#endif

void QStringConverter::State::clear()
{
    if (clearFn) {
        clearFn(this);
    }
    else {
        state_data[0] = state_data[1] = state_data[2] = state_data[3] = 0;
    }
    remainingChars = 0;
    invalidChars = 0;
    internalState = 0;
}

QByteArray QUtf8::convertFromUnicode(QStringView in)
{
    qsizetype len = in.size();
    QByteArray result(len * 3, Qt::Uninitialized);
    uchar *dst = reinterpret_cast<uchar *>(const_cast<char *>(result.constData()));
    const ushort *src = reinterpret_cast<const ushort *>(in.data());
    const ushort *const end = src + len;

    while (src != end) {
        const ushort *nextAscii = end;
        if (simdEncodeAscii(dst, nextAscii, src, end)) {
            break;
        }
        do {
            ushort u = *src++;
            int res = QUtf8Functions::toUtf8<QUtf8BaseTraits>(u, dst, src, end);
            if (res < 0) {
                *dst++ = '?';   //编码错误
            }
        } while (src < nextAscii);
    }
    result.truncate(dst - reinterpret_cast<uchar *>(const_cast<char *>(result.constData())));
    return result;
}

QByteArray QUtf8::convertFromUnicode(QStringView in, QStringConverterBase::State *state)
{
    QByteArray ba(3 * in.size() + 3, Qt::Uninitialized);
    char *end = convertFromUnicode(ba.data(), in, state);
    ba.truncate(end - ba.data());
    return ba;
}

char *QUtf8::convertFromUnicode(char *out, QStringView in, QStringConverter::State *state)
{
    Q_ASSERT(false);
    return nullptr;
}

QString QUtf8::convertToUnicode(QByteArrayView in)
{
    QString result(in.size(), Qt::Uninitialized);
    QChar *data = const_cast<QChar *>(result.constData());
    const QChar *end = convertToUnicode(data, in);
    result.truncate(end - data);
    return result;
}

//Utf8转Unicode
QChar *QUtf8::convertToUnicode(QChar *buffer, QByteArrayView in) noexcept
{
    ushort *dst = reinterpret_cast<ushort *>(buffer);
    const uchar *const start = reinterpret_cast<const uchar *>(in.data());
    const uchar *src = start;
    const uchar *end = src + in.size();

    // attempt to do a full decoding in SIMD
    const uchar *nextAscii = end;
    if (!simdDecodeAscii(dst, nextAscii, src, end)) {
        // at least one non-ASCII entry
        // check if we failed to decode the UTF-8 BOM; if so, skip it
        if (Q_UNLIKELY(src == start) && end - src >= 3 && Q_UNLIKELY(src[0] == utf8bom[0] && src[1] == utf8bom[1] && src[2] == utf8bom[2])) {
            src += 3;
        }

        while (src < end) {
            nextAscii = end;
            if (simdDecodeAscii(dst, nextAscii, src, end)) {
                break;
            }
            do {
                uchar b = *src++;
                int res = QUtf8Functions::fromUtf8<QUtf8BaseTraits>(b, dst, src, end);
                if (res < 0) {
                    // decoding error
                    *dst++ = QChar::ReplacementCharacter;
                }
            } while (src < nextAscii);
        }
    }

    return reinterpret_cast<QChar *>(dst);
}

QT_END_NAMESPACE
