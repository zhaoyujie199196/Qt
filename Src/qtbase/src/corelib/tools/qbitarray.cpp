//
// Created by Yujie Zhao on 2023/9/7.
//
#include "qbitarray.h"
#include <QtCore/QEndian>
#include "qalgorithms.h"

QT_BEGIN_NAMESPACE

QBitArray::QBitArray(qsizetype size, bool value)
    : d(size <= 0 ? 0 : 1 + (size + 7) / 8, Qt::Uninitialized){
    Q_ASSERT(size >= 0);
    if (size <= 0) {
        return;
    }
    uchar *c = reinterpret_cast<uchar *>(d.data());
    memset(c + 1, value ? 0xff : 0, d.size() - 1);
    *c = d.size() * 8 - size;
    if (value && size && size & 7) {
        *(c + 1 + size / 8) &= (1 << (size & 7)) - 1;
    }
}

qsizetype QBitArray::count(bool on) const {
    qsizetype numBits = 0;
    const quint8 *bits = reinterpret_cast<const quint8 *>(d.data()) + 1;
    const quint8 *const end = reinterpret_cast<const quint8 *>(d.end());

    while (bits + 7 <= end) {
        quint64  v = qFromUnaligned<quint64>(bits);
        bits += 8;
        numBits += qsizetype(qPopulationCount(v));
    }
    while (bits + 3 <= end) {
        quint32 v = qFromUnaligned<quint32>(bits);
        bits += 4;
        numBits += qsizetype(qPopulationCount(v));
    }
    while (bits + 1 < end) {
        quint16 v = qFromUnaligned<quint16>(bits);
        bits += 2;
        numBits += qsizetype(qPopulationCount(v));
    }
    if (bits < end) {
        numBits += qsizetype(qPopulationCount(bits[0]));
    }
    return on ? numBits : size() - numBits;
}

void QBitArray::resize(qsizetype size) {
    if (!size) {
        d.resize(0);
    }
    else {
        qsizetype s = d.size();
        d.resize(1 + (size + 7) / 8);
        uchar *c = reinterpret_cast<uchar *>(d.data());
        if (size > (s << 3)) {
            memset(c + s, 0, d.size() - s);
        }
        else if (size & 7) {
            *(c + 1 + size / 8) &= (1 << (size & 7)) - 1;
        }
        *c = d.size() * 8 - size;
    }
}

void QBitArray::fill(bool value, qsizetype begin, qsizetype end) {
    while (begin < end && begin & 0x7)
        setBit(begin++, value);
    qsizetype len = end - begin;
    if (len <= 0)
        return;
    qsizetype s = len & ~qsizetype(0x7);
    uchar *c = reinterpret_cast<uchar *>(d.data());
    memset(c + (begin >> 3) + 1, value ? 0xff : 0, s >> 3);
    begin += s;
    while (begin < end) {
        setBit(begin++, value);
    }
}

QBitArray QBitArray::fromBits(const char *data, qsizetype size) {
    QBitArray result;
    if (size == 0) {
        return result;
    }
    qsizetype nbytes = (size + 7) / 8;

    result.d = QByteArray(nbytes + 1, Qt::Uninitialized);
    char *bits = result.d.data();
    memcpy(bits + 1, data, nbytes);
    if (size & 7) {
        bits[nbytes] &= 0xffU >> (8 - (size & 7));
    }
    *bits = result.d.size() * 8 - size;
    return result;
}

quint32 QBitArray::toUInt32(QSysInfo::Endian endianness, bool *ok) const noexcept {
    const qsizetype s = size();
    if (s > 32) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    if (ok) {
        *ok = true;
    }
    quint32 factor = 1;
    quint32 total = 0;
    for (qsizetype i = 0; i < s; ++i, factor *= 2) {
        const auto index = endianness == QSysInfo::Endian::LittleEndian ? i : (s - i - 1);
        if (testBit(index)) {
            total += factor;
        }
    }
    return total;
}

QBitArray &QBitArray::operator&=(const QBitArray &other) {
    resize(qMax(size(), other.size()));
    uchar *a1 = reinterpret_cast<uchar *>(d.data() + 1);
    const uchar *a2 = reinterpret_cast<const uchar *>(other.d.constData() + 1);
    qsizetype n = other.d.size() - 1;
    qsizetype p = d.size() - 1 - n;
    while (n-- > 0) {
        *a1++ &= *a2++;
    }
    while (p-- > 0) {
        *a1++ = 0;
    }
    return *this;
}

QBitArray &QBitArray::operator|=(const QBitArray &other)
{
    resize(qMax(size(), other.size()));
    uchar *a1 = reinterpret_cast<uchar *>(d.data()) + 1;
    const uchar *a2 = reinterpret_cast<const uchar *>(other.d.constData()) + 1;
    qsizetype n = other.d.size() - 1;
    while (n-- > 0)
        *a1++ |= *a2++;
    return *this;
}

QBitArray &QBitArray::operator^=(const QBitArray &other)
{
    resize(qMax(size(), other.size()));
    uchar *a1 = reinterpret_cast<uchar *>(d.data()) + 1;
    const uchar *a2 = reinterpret_cast<const uchar *>(other.d.constData()) + 1;
    qsizetype n = other.d.size() - 1;
    while (n-- > 0)
        *a1++ ^= *a2++;
    return *this;
}

QBitArray QBitArray::operator~() const
{
    qsizetype sz = size();
    QBitArray a(sz);
    const uchar *a1 = reinterpret_cast<const uchar *>(d.constData()) + 1;
    uchar *a2 = reinterpret_cast<uchar *>(a.d.data()) + 1;
    qsizetype n = d.size() - 1;

    while (n-- > 0)
        *a2++ = ~*a1++;

    if (sz && sz % 8)
        *(a2 - 1) &= (1 << (sz % 8)) - 1;
    return a;
}

QBitArray operator&(const QBitArray &a1, const QBitArray &a2)
{
    QBitArray tmp = a1;
    tmp &= a2;
    return tmp;
}

QBitArray operator|(const QBitArray &a1, const QBitArray &a2)
{
    QBitArray tmp = a1;
    tmp |= a2;
    return tmp;
}

QBitArray operator^(const QBitArray &a1, const QBitArray &a2)
{
    QBitArray tmp = a1;
    tmp ^= a2;
    return tmp;
}

QDataStream &operator<<(QDataStream &out, const QBitArray &ba) {
    Q_ASSERT(false);
    return out;
}

QDataStream &operator>>(QDataStream &in, QBitArray &ba) {
    Q_ASSERT(false);
    return in;
}

QDebug operator<<(QDebug dbg, const QBitArray &array) {
    Q_ASSERT(false);
    return dbg;
}

QT_END_NAMESPACE