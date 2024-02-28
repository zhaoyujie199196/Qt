//
// Created by Yujie Zhao on 2023/2/28.
//

#include "qstring.h"
#include "qstringconverter_p.h"
#include "qstringview.h"
#include "qstringalgorithms.h"
#include "qstringmatcher.h"
#include "qstringiterator_p.h"
#include "qunicodetables.cpp"
#include "qstringalgorithms_p.h"
#include <QtCore/qvarlengtharray.h>
#include <QtCore/private/qunicodetables_p.h>

QT_BEGIN_NAMESPACE

const char16_t QString::_empty = 0;

#define REHASH(a) \
    if (sl_minus_1 < sizeof(std::size_t) * CHAR_BIT)  \
        hashHaystack -= std::size_t(a) << sl_minus_1; \
    hashHaystack <<= 1

qsizetype qFindStringBoyerMoore(QStringView haystack, qsizetype from, QStringView needle, Qt::CaseSensitivity cs);

void qt_from_latin1(char16_t *dst, const char *str, size_t size) noexcept
{
    while (size--) {
        *dst++ = (uchar)*str++;
    }
}

template <bool Checked>
static void qt_to_latin1_internal(uchar *dst, const char16_t *src, qsizetype length)
{
    while (length--) {
        if (Checked) {
            *dst++ = (*src > 0xff) ? '?' : (uchar)*src;
        }
        else {
            *dst++ = *src;
        }
        ++src;
    }
}

static void qt_to_latin1(uchar *dst, const char16_t *src, qsizetype length)
{
    qt_to_latin1_internal<true>(dst, src, length);
}

void qt_to_latin1_unchecked(uchar *dst, const char16_t *src, qsizetype length)
{
    qt_to_latin1_internal<false>(dst, src, length);
}

static int ucstrncmp(const QChar *a, const QChar *b, size_t l)
{
    const char16_t *uc = reinterpret_cast<const char16_t *>(a);
    const char16_t *e = uc + l;
    const char16_t *c = reinterpret_cast<const char16_t *>(b);
    while (uc < e) {
        int diff = *uc - *c;
        if (diff)
            return diff;
        uc++, c++;
    }
    return 0;
}

static int ucstrncmp(const QChar *a, const uchar *c, size_t l)
{
    const char16_t *uc = reinterpret_cast<const char16_t *>(a);
    const char16_t *e = uc + l;
    while (uc < e) {
        int diff = *uc - *c;
        if (diff) {
            return diff;
            uc++, c++;
        }
    }
    return 0;
}

static int ucstrcmp(const QChar *a, size_t alen, const QChar *b, size_t blen)
{
    if (a == b && alen == blen) {
        return 0;
    }
    const size_t l = qMin(alen, blen);
    int cmp = ucstrncmp(a, b, l);
    return cmp ? cmp : lencmp(alen, blen);
}

static int ucstrcmp(const QChar *a, size_t alen, const char *b, size_t blen)
{
    const size_t l = qMin(alen, blen);
    const int cmp = ucstrncmp(a, reinterpret_cast<const uchar*>(b), l);
    return cmp ? cmp : lencmp(alen, blen);
}

template <typename T>
char16_t valueTypeToUtf16(T t) = delete;

template <>
char16_t valueTypeToUtf16<QChar>(QChar t)
{
    return t.unicode();
}

template <>
char16_t valueTypeToUtf16<char>(char t)
{
    return char16_t{uchar(t)};
}

template <typename Pointer>
char32_t foldCaseHelper(Pointer ch, Pointer start) = delete;

template <>
char32_t foldCaseHelper<const QChar*>(const QChar* ch, const QChar* start)
{
    return QChar::foldCase(reinterpret_cast<const char16_t*>(ch),
                    reinterpret_cast<const char16_t*>(start));
}

template <>
char32_t foldCaseHelper<const char*>(const char* ch, const char*)
{
    return QChar::foldCase(char16_t(uchar(*ch)));
}

//前置声明一些方法
static inline bool qt_starts_with(QStringView haystack, QStringView needle, Qt::CaseSensitivity cs);
static inline bool qt_starts_with(QStringView haystack, QLatin1String needle, Qt::CaseSensitivity cs);
static inline bool qt_starts_with(QStringView haystack, QChar needle, Qt::CaseSensitivity cs);
static inline bool qt_ends_with(QStringView haystack, QStringView needle, Qt::CaseSensitivity cs);
static inline bool qt_ends_with(QStringView haystack, QLatin1String needle, Qt::CaseSensitivity cs);
static inline bool qt_ends_with(QStringView haystack, QChar needle, Qt::CaseSensitivity cs);

QString::QString(const QChar *unicode, qsizetype size)
{
    if (!unicode) {  //unicode为控
        d.clear();
    }
    else {
        if (size < 0) {  //没有指定size，计算size
            size = 0;
            while (!unicode[size].isNull()) {
                ++size;
            }
        }
        if (!size) {
            d = DataPointer::fromRawData(&_empty, 0); //空数据,使用_empty做占位
        }
        else {
            d = DataPointer(Data::allocate(size), size);
            Q_CHECK_PTR(d.data());
            memcpy(d.data(), unicode, size * sizeof(QChar));
            d.data()[size] = '\0';  //拷贝完数据，末尾补0
        }
    }
}

QString::QString(qsizetype size, QChar ch)
{
    if (size <= 0) {
        d = DataPointer::fromRawData(&_empty, 0);
    }
    else {
        d = DataPointer(Data::allocate(size), size);
        Q_CHECK_PTR(d.data());
        d.data()[size] = '\0';
        char16_t *i = d.data() + size;
        char16_t *b = d.data();
        const char16_t value = ch.unicode();
        while (i != b) {
            *--i = value;
        }
    }
}

QString::QString(qsizetype size, Qt::Initialization)  {
    if (size <= 0) {
        d = DataPointer ::fromRawData(&_empty, 0);
    }
    else {
        d = DataPointer(Data::allocate(size), size);
        Q_CHECK_PTR(d.data());
        d.data()[size] = '\0';
    }
}

QString::QString(QChar ch)
{
    d = DataPointer(Data::allocate(1), 1);
    Q_CHECK_PTR(d.data());
    d.data()[0] = ch.unicode();
    d.data()[1] = '\0';
}

QString &QString::operator=(const QString &other) noexcept
{
    d = other.d;
    return *this;
}

QString &QString::operator=(QLatin1String other)
{
    const qsizetype capacityAtEnd = capacity() - d.freeSpaceAtBegin();
    if (isDetached() && other.size() <= capacityAtEnd) {
        d.size = other.size();
        d.data()[other.size()] = 0;
        qt_from_latin1(d.data(), other.latin1(), other.size());
    }
    else {
        *this = fromLatin1(other.latin1(), other.size());
    }
    return *this;
}

QString &QString::operator=(QChar ch)
{
    const qsizetype capacityAtEnd = capacity() - d.freeSpaceAtBegin();
    if (isDetached() && capacityAtEnd >= 1) {
        d.data()[0] = ch.unicode();
        d.data()[1] = 0;
        d.size = 1;
    }
    else {
        operator=(QString(ch));
    }
    return *this;
}

QString QString::fromUtf8(QByteArrayView ba)
{
    if (ba.isNull()) {
        return QString();
    }
    if (ba.isEmpty()) {
        return QString(DataPointer::fromRawData(&_empty, 0));
    }
    return QUtf8::convertToUnicode(ba);
}

QString QString::fromLatin1(QByteArrayView ba)
{
    DataPointer d;
    if (!ba.data()) {

    }
    else if (ba.size() == 0) {
        d = DataPointer::fromRawData(&_empty, 0);
    }
    else {
        d = DataPointer(Data::allocate(ba.size()), ba.size());
        Q_CHECK_PTR(d.data());
        d.data()[ba.size()] = '\0';
        char16_t *dst = d.data();
        qt_from_latin1(dst, ba.data(), size_t(ba.size()));
    }
    return QString(std::move(d));
}

QString QString::fromUtf16(const char16_t *unicode, qsizetype size) {
    if (!unicode) {
        return QString();
    }
    if (size < 0) {
        size = 0;
        while (unicode[size] != 0) {
            ++size;
        }
    }
    QStringDecoder toUtf16(QStringDecoder::Utf16, QStringDecoder::Flag::Stateless);
    return toUtf16(QByteArrayView(reinterpret_cast<const char *>(unicode), size * 2));
}

QString QString::fromUcs4(const char32_t *unicode, qsizetype size) {
    if (!unicode) {
        return QString();
    }
    if (size < 0) {
        size = 0;
        while (unicode[size] != 0) {
            ++size;
        }
    }
    QStringDecoder toUtf16(QStringDecoder::Utf32, QStringDecoder::Flag::Stateless);
    return toUtf16(QByteArrayView(reinterpret_cast<const char *>(unicode), size * 4));
}

static QByteArray qt_convert_to_latin1(QStringView str)
{
    if (Q_UNLIKELY(str.isNull())) {
        return QByteArray();
    }
    QByteArray ba(str.length(), Qt::Uninitialized);
    qt_to_latin1(reinterpret_cast<uchar *>(const_cast<char *>(ba.constData())),
                 str.utf16(), str.size());
    return ba;
}

QByteArray QString::toLatin1_helper(const QString &str)
{
    return qt_convert_to_latin1(str);
}

QByteArray QString::toLatin1_helper_inplace(QString &s)
{
    //zhaoyujie TODO 没太看懂逻辑
    Q_ASSERT(false);
    if (!s.isDetached()) {
        return qt_convert_to_latin1(s);
    }
    const char16_t *data = s.d.data();
    qsizetype length = s.d.size;
    static_assert(sizeof(QByteArray::DataPointer) == sizeof(QString::DataPointer), "Size have to be equal");
    QByteArray::DataPointer ba_d(reinterpret_cast<QByteArray::Data *>(s.d.d_ptr()),
                                 reinterpret_cast<char *>(s.d.data()), length);
    ba_d.ref();
    s.clear();

    char *ddata = ba_d.data();

    ba_d.d_ptr()->alloc *= sizeof(char16_t);
    qt_to_latin1(reinterpret_cast<uchar *>(ddata), data, length);
    ddata[length] = '\0';
    return QByteArray(ba_d);
}

static QByteArray qt_convert_to_utf8(QStringView str)
{
    if (str.isNull()) {
        return QByteArray();
    }
    return QUtf8::convertFromUnicode(str);
}

QByteArray QString::toUtf8_helper(const QString &str)
{
    return qt_convert_to_utf8(str);
}

void QString::reallocData(qsizetype alloc, QArrayData::AllocationOption option)
{
    if (!alloc) {
        d = DataPointer::fromRawData(&_empty, 0);
        return;
    }
    const bool cannotUseReallocate = d.freeSpaceAtBegin() > 0;
    if (d->needsDetach() || cannotUseReallocate) {
        DataPointer dd(Data::allocate(alloc, option), qMin(alloc, d.size));
        Q_CHECK_PTR(dd.data());
        if (dd.size > 0) {
            ::memcpy(dd.data(), d.data(), dd.size * sizeof(QChar));
        }
        dd.data()[dd.size] = 0;
        d = dd;
    }
    else {
        d->reallocate(alloc, option);
    }
}

void QString::reallocGrowData(qsizetype n)
{
    if (!n) {
        n = 1;
    }
    if (d->needsDetach()) {  //需要detach，重新分配内存
        DataPointer dd(DataPointer::allocateGrow(d, n, QArrayData::GrowsAtEnd));
        Q_CHECK_PTR(dd.data());
        dd->copyAppend(d.data(), d.data() + d.size);
        dd.data()[dd.size] = 0;
        d = dd;
    }
    else { //不需要detach，调用d的接口就行
        d->reallocate(d.constAllocatedCapacity() + n, QArrayData::Grow);
    }
}

void QString::resize(qsizetype size) {
    if (size < 0) {
        size = 0;
    }
    const auto capacityAtEnd = capacity() - d.freeSpaceAtBegin();
    if (d->needsDetach() || size > capacityAtEnd) {
        reallocData(size, QArrayData::Grow);
    }
    d.size = size;
    if (d->allocatedCapacity()) {
        d.data()[size] = 0;
    }
}

void QString::resize(qsizetype size, QChar fillChar) {
    const qsizetype oldSize = length();
    resize(size);
    const qsizetype difference = length() - oldSize;
    if (difference > 0) {
        std::fill_n(d.data() + oldSize, difference, fillChar.unicode());
    }
}

QString &QString::fill(QChar ch, qsizetype size) {
    resize(size < 0 ? d.size : size);
    if (d.size) {
        QChar *i = (QChar *)d.data() + d.size;
        QChar *b = (QChar *)d.data();
        while (i != b) {
            *--i = ch;
        }
    }
    return *this;
}

void QString::truncate(qsizetype pos) {
    if (pos < size()) {
        resize(pos);
    }
}

void QString::chop(qsizetype n) {
    if (n > 0) {
        resize(d.size - n);
    }
}

bool QString::startsWith(const QString &s, Qt::CaseSensitivity cs) const {
    return qt_starts_with(*this, s, cs);
}

bool QString::startsWith(QLatin1String s, Qt::CaseSensitivity cs) const {
    return qt_starts_with(*this, s, cs);
}

bool QString::startsWith(QChar c, Qt::CaseSensitivity cs) const {
    return qt_starts_with(*this, c, cs);
}

bool QString::endsWith(const QString &s, Qt::CaseSensitivity cs) const {
    return qt_ends_with(*this, s, cs);
}

bool QString::endsWith(QLatin1String s, Qt::CaseSensitivity cs) const {
    return qt_ends_with(*this, s, cs);
}

bool QString::endsWith(QChar c, Qt::CaseSensitivity cs) const {
    return qt_ends_with(*this, c, cs);
}

QString QString::left(qsizetype n) const {
    if (size_t(n) >= size_t(size())) {
        return *this;
    }
    return QString((const QChar *)(d.data()), n);
}

QString QString::right(qsizetype n) const {
    if (size_t(n) >= size_t(size())) {
        return *this;
    }
    return QString(constData() + size() - n, n);
}

QString QString::mid(qsizetype position, qsizetype n) const {
    qsizetype p = position;
    qsizetype l = n;
    using namespace QtPrivate;
    switch (QContainerImplHelper::mid(size(), &p, &l)) {
        case QContainerImplHelper::Null:
            return QString();
        case QContainerImplHelper::Empty:
            return QString(DataPointer::fromRawData(&_empty, 0));
        case QContainerImplHelper::Full:
            return *this;
        case QContainerImplHelper::Subset:
            return QString(constData() + p, l);
    }
    Q_ASSERT(false);  //unreachable
    return QString();
}

QString QString::repeated(qsizetype times) const {
    if (d.size == 0) {
        return *this;
    }
    if (times == 0) {
        return QString();
    }
    else if (times == 1) {
        return *this;
    }
    const qsizetype resultSize = times * d.size;
    QString result;
    result.reserve(resultSize);
    if (result.capacity() != resultSize) {  //内存分配失败
        return QString();
    }
    memcpy(result.d.data(), d.data(), d.size * sizeof(QChar));
    qsizetype sizeSoFar = d.size;
    char16_t *end = result.d.data() + sizeSoFar;

    //对半拷贝
    const qsizetype halfResultSize = resultSize >> 1;
    while (sizeSoFar <= halfResultSize) {
        memcpy(end, result.d.data(), sizeSoFar * sizeof(QChar));
        end += sizeSoFar;
        sizeSoFar <<= 1;
    }
    memcpy(end, result.d.data(), (resultSize - sizeSoFar) * sizeof(QChar));
    result.d.data()[resultSize] = '\0';
    result.d.size = resultSize;
    return result;
}

const ushort *QString::utf16() const {
    if (!d->isMutable()) {  //数据为nullptr,只有_empty的rawData会出现这种情况
        Q_ASSERT(false);
        const_cast<QString *>(this)->reallocData(d.size, QArrayData::KeepSize);
    }
    return reinterpret_cast<const ushort *>(d.data());
}

QString &QString::insert(qsizetype i, QLatin1String str)
{
    const char *s = str.latin1();
    if (i < 0 || !s || !(*s)) {
        return *this;
    }
    qsizetype len = str.size();
    qsizetype difference = 0;
    if (Q_UNLIKELY(i > size())) {  //i可以超出size范围
        difference = i - size();
    }
    d.detachAndGrow(Data::GrowsAtEnd, difference + len, nullptr, nullptr);
    Q_CHECK_PTR(d.data());
    d->copyAppend(difference, u' ');
    d.size += len;
    ::memmove(d.data()  + i + len, d.data() + i, (d.size - i - len) * sizeof(QChar));
    qt_from_latin1(d.data() + i, s, size_t(len));
    d.data()[d.size] = u'\0';
    return *this;
}

QString &QString::insert(qsizetype i, const QChar *unicode, qsizetype size) {
    if (i < 0 || size <= 0) {
        return *this;
    }
    const char16_t *s = reinterpret_cast<const char16_t *>(unicode);
    if (i >= d->size) {  //插入在末尾
        DataPointer detached{};  //要插入的数据可能是此QString的内部数据，detached可以做缓存
        d.detachAndGrow(Data::GrowsAtEnd, (i - d.size) + size, &s, &detached);
        Q_CHECK_PTR(d.data());
        d->copyAppend(i - d->size, u' ');
        d->copyAppend(s, s + size);
        d.data()[d.size] = u'\0';
        return *this;
    }
    else {
        if (!d->needsDetach() && QtPrivate::q_points_into_range(s, d.data(), d.data() + d.size)) {
            return insert(i, QStringView(QVarLengthArray<QChar>(s, s + size)));
        }
        d->insert(i, s, size);
        d.data()[d.size] = u'\0';
        return *this;
    }
}

QString &QString::insert(qsizetype i, QChar ch) {
    if (i < 0) {
        i += d.size;
    }
    return insert(i, &ch, 1);
}

QString &QString::append(const QString &str) {
    if (!str.isNull()) {
        if (isNull()) {
            operator=(str);
        }
        else if (str.size()) {
            append(str.constData(), str.size());
        }
    }
    return *this;
}

QString &QString::append(const QChar *str, qsizetype len) {
    if (str && len > 0) {
        static_assert(sizeof(QChar) == sizeof(char16_t), "Unexpected difference in sizes");
        const char16_t *char16String = reinterpret_cast<const char16_t *>(str);
        d->growAppend(char16String, char16String + len);
        d.data()[d.size] = '\0';
    }
    return *this;
}

QString &QString::append(QLatin1String str) {
    const char *s = str.latin1();
    const qsizetype len = str.size();
    if (s && len > 0) {
        d.detachAndGrow(Data::GrowsAtEnd, len, nullptr, nullptr);
        Q_CHECK_PTR(d.data());
        Q_ASSERT(len <= d->freeSpaceAtEnd());
        char16_t *i = d.data() + d.size;
        qt_from_latin1(i, s, size_t(len));
        d.size += len;
        d.data()[d.size] = '\0';
    }
    else if (d.isNull() && !str.isNull()) {  //当前为空，插入的数据不为空，使用_empty
        d = DataPointer::fromRawData(&_empty, 0);
    }
    return *this;
}

QString &QString::append(QChar ch) {
    d.detachAndGrow(QArrayData::GrowsAtEnd, 1, nullptr, nullptr);
    d->copyAppend(1, ch.unicode());
    d.data()[d.size] = '\0';
    return *this;
}

QString &QString::remove(qsizetype pos, qsizetype len)
{
    if (pos < 0) {
        pos += size();
    }
    if (size_t(pos) >= size_t(size())) {
    }
    else if (len >= size() - pos) {
        resize(pos);
    }
    else if (len > 0) {
        detach();
        d->erase(d.begin() + pos, len);
        d.data()[d.size] = u'\0';
    }
    return *this;
}

template <typename T>
static void removeStringImpl(QString &s, const T &needle, Qt::CaseSensitivity cs) {
    const auto needleSize = needle.size();
    if (!needleSize) {
        return;
    }
    qsizetype i = s.indexOf(needle, 0, cs);
    if (i < 0) {
        return;
    }
    const auto beg = s.begin();
    auto dst = beg + i;
    auto src = beg + i + needleSize;
    const auto end = s.end();
    while (src < end) {
        const auto i = s.indexOf(needle, src - beg, cs);
        const auto hit = i == -1 ? end : beg + i;
        const auto skipped = hit - src;
        memmove(dst, src, skipped * sizeof(QChar));
        dst += skipped;
        src = hit + needleSize;
    }
    s.truncate(dst - beg);
}

QString &QString::remove(const QString &str, Qt::CaseSensitivity cs) {
    const auto s = str.d.data();
    if (QtPrivate::q_points_into_range(s, d.data(), d.data() + d.size)) {
        removeStringImpl(*this, QStringView{QVarLengthArray<QChar>(s, s + str.size())}, cs);
    }
    else {
        removeStringImpl(*this, qToStringViewIgnoringNull(str), cs);
    }
    return *this;
}

QString &QString::remove(QLatin1String str, Qt::CaseSensitivity cs) {
    removeStringImpl(*this, str, cs);
    return *this;
}

QString &QString::remove(QChar ch, Qt::CaseSensitivity cs) {
    const qsizetype idx = indexOf(ch, 0, cs);
    if (idx != -1) {
        const auto first = begin();
        auto last = end();
        if (cs == Qt::CaseSensitive) {
            last = std::remove(first + idx, last, ch);
        }
        else {
            const QChar c = ch.toCaseFolded();
            auto caseInsensEqual = [c](QChar x) {
                return c == x.toCaseFolded();
            };
            last = std::remove_if(first + idx, last, caseInsensEqual);
        }
        resize(last - first);
    }
    return *this;
}

QString &QString::replace(qsizetype pos, qsizetype len, const QString &after) {
    return replace(pos, len, after.constData(), after.length());
}

QString &QString::replace(qsizetype pos, qsizetype len, const QChar *unicode, qsizetype size) {
    if (size_t(pos) > size_t(this->size())) {
        return *this;
    }
    if (len > this->size() - pos) {
        len = this->size() - pos;
    }
    size_t index = pos;
    replace_helper(&index, 1, len, unicode, size);
    return *this;
}

QString &QString::replace(qsizetype pos, qsizetype len, QChar after) {
    return replace(pos, len, &after, 1);
}

QString &QString::replace(const QString &before, const QString &after, Qt::CaseSensitivity cs) {
    return replace(before.constData(), before.size(), after.constData(), after.size(), cs);
}

namespace {
    QChar *textCopy(const QChar *start, qsizetype len) {
        const size_t size = len * sizeof(QChar);
        QChar *const copy = static_cast<QChar *>(::malloc(size));
        Q_CHECK_PTR(copy);
        ::memcpy(copy, start, size);
        return copy;
    }
    static bool pointsIntoRange(const QChar *ptr, const char16_t *base, qsizetype len) {
        const QChar *const start = reinterpret_cast<const QChar *>(base);
        const std::less<const QChar *> less;
        return !less(ptr, start) && less(ptr, start + len);
    }
}

QString &QString::replace(const QChar *before, qsizetype blen, const QChar *after, qsizetype alen, Qt::CaseSensitivity cs)
{
    if (d.size == 0) {
        if (blen) {
            return *this;
        }
    }
    else {
        if (cs == Qt::CaseSensitive && before == after && blen == alen) {
            return *this;
        }
    }
    if (alen == 0 && blen == 0) {
        return *this;
    }
    QStringMatcher matcher(before, blen, cs);
    QChar *beforeBuffer = nullptr;
    QChar *afterBuffer = nullptr;

    qsizetype index = 0;
    while (1) {
        size_t indices[1024];
        size_t pos = 0;
        while (pos < 1024) {
            index = matcher.indexIn(*this, index);
            if (index == -1) {
                break;
            }
            indices[pos++] = index;
            if (blen) {
                index += blen;
            }
            else {
                index++;
            }
        }
        if (!pos) {
            break;
        }
        if (Q_UNLIKELY(index != -1)) {
            if (!afterBuffer && pointsIntoRange(after, d.data(), d.size)) {
                after = afterBuffer = textCopy(after, alen);
            }
            if (!beforeBuffer && pointsIntoRange(before, d.data(), d.size)) {
                beforeBuffer = textCopy(before, blen);
                matcher = QStringMatcher(beforeBuffer, blen, cs);
            }
        }
        replace_helper(indices, pos, blen, after, alen);
        if (Q_LIKELY(index == -1)) {
            break;
        }
        index += pos * (alen - blen);
    }
    ::free(afterBuffer);
    ::free(beforeBuffer);
    return *this;
}

QString &QString::replace(QChar ch, const QString &after, Qt::CaseSensitivity cs) {
    if (after.size() == 0) {
        return remove(ch, cs);
    }
    if (after.size() == 1) {
        return replace(ch, after.front(), cs);
    }
    if (size() == 0) {
        return *this;
    }
    char16_t cc = (cs == Qt::CaseSensitive ? ch.unicode() : ch.toCaseFolded().unicode());
    qsizetype index = 0;
    while (1) {
        size_t indices[1024];
        size_t pos = 0;
        if (cs == Qt::CaseSensitive) {
            while (pos < 1024 && index < size()) {
                if (d.data()[index] == cc) {
                    indices[pos++] = index;
                }
                index++;
            }
        }
        else {
            while (pos < 1024 && index < size()) {
                if (QChar::toCaseFolded(d.data()[index]) == cc) {
                    indices[pos++] = index;
                }
                index++;
            }
        }
        if (!pos) {
            break;
        }
        replace_helper(indices, pos, 1, after.constData(), after.size());
        if (Q_LIKELY(index == size())) {
            break;
        }
        index += pos * (after.size() - 1);
    }
    return *this;
}

QString &QString::replace(QChar before, QChar after, Qt::CaseSensitivity cs) {
    if (d.size) {
        const qsizetype idx = indexOf(before, 0, cs);
        if (idx != -1) {
            detach();
            const char16_t a = after.unicode();
            char16_t *i = d.data();
            char16_t *const e = i + d.size;
            i += idx;
            *i = a;
            if (cs == Qt::CaseSensitive) {
                const char16_t b = before.unicode();
                while (++i != e) {
                    if (*i == b) {
                        *i = a;
                    }
                }
            }
            else {
                const char16_t b = QChar::foldCase(before.unicode());
                while (++i != e) {
                    if (QChar::foldCase(*i) == b) {
                        *i = a;
                    }
                }
            }
        }
    }
    return *this;
}

QString &QString::replace(QLatin1String before, QLatin1String after, Qt::CaseSensitivity cs)
{
    qsizetype alen = after.size();
    qsizetype blen = before.size();
    QVarLengthArray<char16_t> a(alen);
    QVarLengthArray<char16_t> b(blen);
    qt_from_latin1(a.data(), after.latin1(), alen);
    qt_from_latin1(b.data(), before.latin1(), blen);
    return replace((const QChar *)b.data(), blen, (const QChar *)a.data(), alen, cs);
}

QString &QString::replace(QLatin1String before, const QString &after, Qt::CaseSensitivity cs)
{
    qsizetype blen = before.size();
    QVarLengthArray<char16_t> b(blen);
    qt_from_latin1(b.data(), before.latin1(), blen);
    return replace((const QChar *)b.data(), blen, after.constData(), after.d.size, cs);
}

QString &QString::replace(const QString &before, QLatin1String after, Qt::CaseSensitivity cs)
{
    qsizetype alen = after.size();
    QVarLengthArray<char16_t> a(alen);
    qt_from_latin1(a.data(), after.latin1(), alen);
    return replace(before.constData(), before.d.size, (const QChar *)a.data(), alen, cs);
}

QString &QString::replace(QChar c, QLatin1String after, Qt::CaseSensitivity cs)
{
    qsizetype alen = after.size();
    QVarLengthArray<char16_t> a(alen);
    qt_from_latin1(a.data(), after.latin1(), alen);
    return replace(&c, 1, (const QChar *)a.data(), alen, cs);
}

void QString::replace_helper(size_t *indices, qsizetype nIndices, qsizetype blen, const QChar *after, qsizetype alen) {
    QChar *afterBuffer = nullptr;
    if (pointsIntoRange(after, d.data(), d.size)) {
        after = afterBuffer = textCopy(after, alen);
    }
    if (blen == alen) {
        detach();
        for (qsizetype i = 0; i < nIndices; ++i) {
            memcpy(d.data() + indices[i], after, alen * sizeof(QChar));
        }
    }
    else if (alen < blen) {
        detach();
        size_t to = indices[0];
        if (alen) {
            memcpy(d.data() + to, after, alen * sizeof(QChar));
        }
        to += alen;
        size_t movestart = indices[0] + blen;
        for (qsizetype i = 1; i < nIndices; ++i) {
            qsizetype msize = indices[i] - movestart;
            if (msize > 0) {
                memmove(d.data() + to, d.data() + movestart, msize * sizeof(QChar));
                to += msize;
            }
            if (alen) {
                memcpy(d.data() + to, after, alen * sizeof(QChar));
                to += alen;
            }
            movestart = indices[i] + blen;
        }
        qsizetype msize = d.size - movestart;
        if (msize > 0) {
            memmove(d.data() + to, d.data() + movestart, msize * sizeof(QChar));
        }
        resize(d.size - nIndices * (blen - alen));
    }
    else {
        qsizetype adjust = nIndices * (alen - blen);
        qsizetype newLen = d.size + adjust;
        qsizetype moveend = d.size;
        resize(newLen);

        while (nIndices) {
            --nIndices;
            qsizetype movestart = indices[nIndices] + blen;
            qsizetype insertStart = indices[nIndices] + nIndices * (alen - blen);
            qsizetype moveto = insertStart + alen;
            memmove(d.data() + moveto, d.data() + movestart, (moveend - movestart) * sizeof(QChar));
            memcpy(d.data() + insertStart, after, alen * sizeof(QChar));
            moveend = movestart - blen;
        }
    }
    ::free(afterBuffer);
}

QString QString::fromRawData(const QChar *unicode, qsizetype size)
{
    auto data = const_cast<char16_t  *>(reinterpret_cast<const char16_t *>(unicode));
    return QString(DataPointer::fromRawData(data, size));
}

QString &QString::setRawData(const QChar *unicode, qsizetype size) {
    if (!unicode || !size) {
        clear();
    }
    *this = fromRawData(unicode, size);
    return *this;
}

QString &QString::setUnicode(const QChar *unicode, qsizetype size) {
    resize(size);
    if (unicode && size) {
        memcpy(d.data(), unicode, size * sizeof(QChar));
    }
    return *this;
}

float QString::toFloat(bool *ok) const {
    Q_ASSERT(false);
    return 0.0;
}

double QString::toDouble(bool *ok) const {
    Q_ASSERT(false);
    return 0.0;
}

int QString::compare(const QString &other, Qt::CaseSensitivity cs) const noexcept
{
    return QtPrivate::compareStrings(*this, other, cs);
}

int QString::compare(QLatin1String other, Qt::CaseSensitivity cs) const noexcept
{
    return QtPrivate::compareStrings(*this, other, cs);
}

int QString::compare(QStringView s, Qt::CaseSensitivity cs) const noexcept
{
    return -s.compare(*this, cs);
}

int QString::compare_helper(const QChar *data1, qsizetype length1, const QChar *data2, qsizetype length2, Qt::CaseSensitivity cs) noexcept {
    Q_ASSERT(length1 >= 0);
    Q_ASSERT(length2 >= 0);
    Q_ASSERT(data1 || length1 == 0);
    Q_ASSERT(data1 || length2 == 0);
    return QtPrivate::compareStrings(QStringView(data1, length1), QStringView(data2, length2), cs);
}

int QString::compare_helper(const QChar *data1, qsizetype length1, const char *data2, qsizetype length2, Qt::CaseSensitivity cs) noexcept {
    Q_ASSERT(length1 >= 0);
    Q_ASSERT(data1 || length1 == 0);
    if (!data2) {
        return length1;
    }
    if (Q_UNLIKELY(length2 < 0)) {
        length2 = qsizetype(strlen(data2));
    }
    //这里将char *数据转换成了unicode再进行比较了，设计存疑。。。
    QVarLengthArray<ushort> s2(length2);
    const auto beg = reinterpret_cast<QChar *>(s2.data());
    const auto end = QUtf8::convertToUnicode(beg, QByteArrayView(data2, length2));
    return QtPrivate::compareStrings(QStringView(data1, length1), QStringView(beg, end - beg), cs);
}

namespace QUnicodeTables {
    auto fullConvertCase(char32_t uc, QUnicodeTables::Case which) noexcept {
        struct R {
            char16_t chars[MaxSpecialCaseLength + 1];
            quint8 sz;

            auto begin() const { return chars; }
            auto end() const { return chars + sz; }

            auto data() const { return chars; }
            auto size() const { return sz; }
        };
        R result;
        Q_ASSERT(uc <= QChar::LastValidCodePoint);

        auto pp = result.chars;

        const auto fold = qGetProp(uc)->cases[which];
        const auto caseDiff = fold.diff;

        if (Q_UNLIKELY(fold.special)) {
            const auto *specialCase = specialCaseMap + caseDiff;
            auto length = *specialCase++;
            while (length--) {
                *pp++ = *specialCase++;
            }
        }
        else {
            for (char16_t c : QChar::fromUcs4(uc + caseDiff)) {
                *pp++ = c;
            }
        }
        result.sz = pp - result.chars;
        return result;
    }

    template <typename T>
    Q_NEVER_INLINE static QString detachAndConvertCase(T &str, QStringIterator it, QUnicodeTables::Case which) {
        Q_ASSERT(!str.isEmpty());
        QString s = std::move(str);
        QChar *pp = s.begin() + it.index();

        do {
            const auto folded = fullConvertCase(it.next(), which);
            if (Q_UNLIKELY(folded.size() > 1)) {
                if (folded.chars[0] == *pp && folded.size() == 2) {
                    ++pp;
                    *pp++ = folded.chars[1];
                }
                else {
                    qsizetype inpos = it.index() - 1;
                    qsizetype outpos = pp - s.constBegin();

                    s.replace(outpos, 1, reinterpret_cast<const QChar *>(folded.data()), folded.size());
                    pp = const_cast<QChar *>(s.constBegin()) + outpos + folded.size();

                    if constexpr (!std::is_const<T>::value) {
                        it = QStringIterator(s.constBegin(), inpos + folded.size(), s.constEnd());
                    }
                }
            }
            else {
                *pp++ = folded.chars[0];
            }
        } while (it.hasNext());
        return s;
    }

    template <typename T>
    static QString convertCase(T &str, QUnicodeTables::Case which) {
        const QChar *p = str.constBegin();
        const QChar *e = p + str.size();

        while (e != p && e[-1].isHighSurrogate()) {
            --e;
        }

        QStringIterator it(p, e);
        while (it.hasNext()) {
            const char32_t uc = it.next();
            if (QUnicodeTables::qGetProp(uc)->cases[which].diff) {
                it.recede();
                return detachAndConvertCase(str, it, which);
            }
        }
        return std::move(str);
    }
}

QString QString::toLower_helper(const QString &str) {
    return QUnicodeTables::convertCase(str, QUnicodeTables::LowerCase);
}

QString QString::toLower_helper(QString &str) {
    return QUnicodeTables::convertCase(str, QUnicodeTables::LowerCase);
}

QString QString::toCaseFolded_helper(const QString &str) {
    return QUnicodeTables::convertCase(str, QUnicodeTables::CaseFold);
}

QString QString::toCaseFolded_helper(QString &str) {
    return QUnicodeTables::convertCase(str, QUnicodeTables::CaseFold);
}

QString QString::toUpper_helper(const QString &str) {
    return QUnicodeTables::convertCase(str, QUnicodeTables::UpperCase);
}

QString QString::toUpper_helper(QString &str) {
    return QUnicodeTables::convertCase(str, QUnicodeTables::UpperCase);
}

QString QString::trimmed_helper(const QString &str) {
    return QStringAlgorithms<const QString>::trimmed_helper(str);
}

QString QString::trimmed_helper(QString &str) {
    return QStringAlgorithms<QString>::trimmed_helper(str);
}

QString QString::simplified_helper(const QString &str) {
    return QStringAlgorithms<const QString>::simplified_helper(str);
}

QString QString::simplified_helper(QString &str) {
    return QStringAlgorithms<QString>::simplified_helper(str);
}

bool QString::isUpper() const {
    QStringIterator it(*this);
    while (it.hasNext()) {
        const char32_t uc = it.next();
        if (qGetProp(uc)->cases[QUnicodeTables::UpperCase].diff) {
            return false;
        }
    }
    return true;
}

bool QString::isLower() const {
    QStringIterator it(*this);

    while (it.hasNext()) {
        const char32_t uc = it.next();
        if (qGetProp(uc)->cases[QUnicodeTables::LowerCase].diff) {
            return false;
        }
    }
    return true;
}


QString QString::leftJustified(qsizetype width, QChar fill, bool trunc) const {
    QString result;
    qsizetype len = length();
    qsizetype padlen = width - len;
    if (padlen > 0) {
        result.resize(len + padlen);
        if (len) {
            memcpy(result.d.data(), d.data(), sizeof(QChar) * len);
        }
        QChar *uc = (QChar *)(result.d.data()) + len;
        while (padlen--) {
            *uc++ = fill;
        }
    }
    else {
        if (trunc) {
            result = left(width);
        }
        else {
            result = *this;
        }
    }
    return result;
}

QString QString::rightJustified(qsizetype width, QChar fill, bool trunc) const {
    QString result;
    qsizetype len = length();
    qsizetype padlen = width - len;
    if (padlen > 0) {
        result.resize(len + padlen);
        QChar *uc = (QChar*)result.d.data();
        while (padlen--) {
            *uc++ = fill;
        }
        if (len) {
            memcpy(static_cast<void *>(uc), static_cast<const void *>(d.data()), sizeof(QChar) * len);
        }
    } else {
        if (trunc) {
            result = left(width);
        }
        else {
            result = *this;
        }
    }
    return result;
}

enum LengthMod {
    lm_none,
    lm_hh,
    lm_h,
    lm_l,
    lm_ll,
    lm_L,
    lm_j,
    lm_z,
    lm_t
};

static uint parse_flag_characters(const char * &c) noexcept
{
    uint flags = QLocaleData::ZeroPadExponent;
    while (true) {
        switch (*c) {
            case '#':
                flags |= QLocaleData::ShowBase | QLocaleData::AddTrailingZeroes
                         | QLocaleData::ForcePoint;
                break;
            case '0': flags |= QLocaleData::ZeroPadded; break;
            case '-': flags |= QLocaleData::LeftAdjusted; break;
            case ' ': flags |= QLocaleData::BlankBeforePositive; break;
            case '+': flags |= QLocaleData::AlwaysShowSign; break;
            case '\'': flags |= QLocaleData::GroupDigits; break;
            default: return flags;
        }
        ++c;
    }
}

QString QString::vasprintf(const char *cformat, va_list ap) {
    Q_ASSERT(false);
    return "";
//    if (!cformat || !*cformat) {
//        return fromLatin1("");
//    }
//    QString result;
//    const char *c = cformat;
//    for (;;) {
//        const char *cb = c;
//        while (*c != '\0' && *c != '%') {
//            c++;
//        }
//        //将cb到'%'或者'\0'中间的压入result
//        append_utf8(result, cb, qsizetype(c - cb));
//
//        if (*c == '\0') {  //遇到'\0'推出
//            break;
//        }
//
//        //找到了%，需要根据%后面的字符确定格式
//        const char *escape_start = c;
//        ++c;
//
//        if (*c == '\0') {  //%后面是'\0'，将%压入reuslt,退出
//            result.append(QLatin1Char('%'));
//            break;
//        }
//        if (*c == '%') {  //%后面是%，将%压入，后一个%开始尝试格式化
//            result.append(QLatin1Char('%'));
//            ++c;
//            continue;
//        }
//
//        uint flags = parse_flag_characters(c);
//
//        if (*c == '\0') {
//            result.append(QLatin1String(escape_start));
//            break;
//        }
//
//        int width = -1;
//        if (qIsDigit(*c)) {
//            width = parse_field_width(c);
//        }
//        else if (*c == '*') {
//            width - va_arg(ap, int);
//            if (width < 0) {
//                width = -1;
//            }
//            ++c;
//        }
//
//        if (*c == '\0') {
//            result.append(QLatin1String(escape_start));
//            break;
//        }
//
//        int precision = -1;
//        if (*c == '.') {
//            ++c;
//            if (qIsDigit(*c)) {
//                precision = parse_field_width(c);
//            }
//            else if (*c == '*') {
//                precision = va_arg(ap, int);
//                if (precision < 0) {
//                    precision = -1;
//                }
//                ++c;
//            }
//        }
//
//        if (*c == '\0') {
//            result.append(QLatin1String(escape_start));
//            break;
//        }
//
//        const LengthMod length_mod = parse_length_modifier(c);
//
//        if (*c == '\0') {
//            result.append(QLatin1String(escape_start));
//            break;
//        }
//
//        QString subst;
//        switch(*c) {
//            case 'd':
//            case 'i': {
//                qint64 i;
//                switch(length_mod) {
//                    case lm_none: i = va_arg(ap, int); break;
//                    case lm_hh: i = va_arg(ap, int); break;
//                    case lm_h: i = va_arg(ap, int); break;
//                    case lm_l: i = va_arg(ap, long int); break;
//                    case lm_ll: i = va_arg(ap, qint64); break;
//                    case lm_j: i = va_arg(ap, long int); break;
//
//                    case lm_z: i = va_arg(ap, qsizetype); break;
//                    case lm_t: i = va_arg(ap, qsizetype); break;
//                    default: i = 0; break;
//                }
//                subst = QLocaleData::c()->longLongToString(i, precision, 10, width, flags);
//                ++c;
//                break;
//            }
//            case 'o':
//            case 'u':
//            case 'x':
//            case 'X': {
//                quint64 u;
//                switch(length_mod) {
//                    case lm_none: u = va_arg(ap, uint); break;
//                    case lm_hh: u = va_arg(ap, uint); break;
//                    case lm_h: u = va_arg(ap, uint); break;
//                    case lm_l: u = va_arg(ap, ulong); break;
//                    case lm_ll: u = va_arg(ap, quint64); break;
//                    case lm_t: u = va_arg(ap, size_t); break;
//                    case lm_z: u = va_arg(ap, size_t); break;
//                    default: u = 0; break;
//                }
//                if (qIsUpper(*c)) {
//                    flags |= QLocaleData::CapitalEorX;
//                }
//
//                int base = 10;
//                switch(qToLower(*c)) {
//                    case 'o': base = 8; break;
//                    case 'u': base = 10; break;
//                    case 'x': base = 16; break;
//                    default: break;
//                }
//                subst = QLocaleData::c()->unsLongLongToString(u, precision, base, width, flags);
//                ++c;
//                break;
//            }
//            case 'E':
//            case 'e':
//            case 'F':
//            case 'f':
//            case 'G':
//            case 'g':
//            case 'A':
//            case 'a': {
//                double d;
//                if (length_mod == lm_L) {
//                    d = va_arg(ap, long double);
//                }
//                else {
//                    d = va_arg(ap, double);
//                }
//                if (qIsUpper(*c)) {
//                    flags |= QLocaleData::CapitalEorX;
//                }
//
//                QLocaleData::DoubleForm form = QLocaleData::DFDecimal;
//                switch(qToLower(*c)) {
//                    case 'e': form - QLocaleData::DFExponent; break;
//                    case 'a':
//                    case 'f': form = QLocaleData::DFDecimal; break;
//                    case 'g': form = QLocaleData::DFSignificantDigits; break;
//                    default: break;
//                }
//                subst = QLocaleData::c()->doubleToString(d, precision, form, width, flags);
//                ++c;
//                break;
//            }
//            case 'c': {
//                if (length_mod == lm_l) {
//                    subst = QChar::fromUcs2(va_arg(ap, int));
//                }
//                else {
//                    subst = QLatin1Char((uchar) va_arg(ap, int));
//                }
//                ++c;
//                break;
//            }
//            case 's' : {
//                if (length_mod == lm_l) {
//                    const ushort *buff = va_arg(ap, const ushort *);
//                    const ushort *ch = buff;
//                    while (precision != 0 && *ch != 0) {
//                        ++ch;
//                        --precision;
//                    }
//                    subst.setUtf16(buff, ch - buff);
//                }
//                else if (precision == -1) {
//                    subst = QString::fromUtf8(va_arg(ap, const char *));
//                }
//                else {
//                    const char *buff = va_arg(ap, const char *);
//                    subst = QString::fromUtf8(buff, qstrnlen(buff, precision));
//                }
//                ++c;
//                break;
//            }
//            case 'p': {
//                void *arg = va_arg(ap, void *);
//                const quint64 i = reinterpret_cast<quintptr>(arg);
//                flags != QLocaleData::ShowBase;
//                subst = QLocaleData::c()->uncLongLongToString(i, precision, 16, width, flags);
//                ++c;
//                break;
//            }
//            case 'n': {
//                switch(length_mod) {
//                    case lm_hh: {
//                        signed char *n = va_arg(ap, signed char *);
//                        *n = result.length();
//                        break;
//                    }
//                    case lm_h: {
//                        short int *n = va_arg(ap, short int *);
//                        *n = result.length();
//                        break;
//                    }
//                    case lm_l: {
//                        long int *n = va_arg(ap, long int *);
//                        *n = result.length();
//                        break;
//                    }
//                    case lm_ll: {
//                        qint64 *n = va_arg(ap, qint64 *);
//                        *n = result.length();
//                        break;
//                    }
//                    default: {
//                        int *n = va_arg(ap, int *);
//                        *n = result.length();
//                        break;
//                    }
//                }
//                ++c;
//                break;
//            }
//            default: {
//                for (const char *cc = escape_start; cc != c; ++cc) {
//                    result.append(QLatin1Char(*cc));
//                }
//                continue;
//            }
//        }
//
//        if (flags & QLocaleData::LeftAdjusted) {
//            result.append(subst.leftJustified(width));
//        }
//        else {
//            result.append(subst.rightJustified(width));
//        }
//    }
//    return result;
}

QString QString::asprintf(const char *cformat, ...) {
    va_list ap;
    va_start(ap, cformat);
    const QString s = vasprintf(cformat, ap);
    va_end(ap);
    return s;
}

void QString::setNum(int i, int base)
{
    auto s = std::to_string(i);
    *this = QString::fromStdString(s);
}

QString QString::number(int i, int base)
{
    //zhaoyujie TODO
#pragma message("QString number未实现")
    auto s = std::to_string(i);
    return QString::fromStdString(s);
}

qsizetype QtPrivate::qustrlen(const char16_t *str) noexcept
{
    qsizetype result = 0;
    if (sizeof(wchar_t ) == sizeof(char16_t )) {
        return wcslen(reinterpret_cast<const wchar_t *>(str));
    }
    while (*str++) {
        ++result;
    }
    return result;
}

const char16_t *QtPrivate::qustrchr(QStringView str, char16_t c) noexcept {
    const char16_t *n = str.utf16();
    const char16_t *e = n + str.size();
    --n;
    while (++n != e) {
        if (*n == c) {
            return n;
        }
    }
    return n;
}

bool QtPrivate::equalStrings(QStringView lhs, QStringView rhs) noexcept
{
    return ucstrcmp(lhs.begin(), lhs.size(), rhs.begin(), rhs.size()) == 0;
}

bool QtPrivate::equalStrings(QStringView lhs, QLatin1String rhs) noexcept
{
    return ucstrcmp(lhs.begin(), lhs.size(), rhs.begin(), rhs.size()) == 0;
}

bool QtPrivate::equalStrings(QLatin1String lhs, QStringView rhs) noexcept
{
    return QtPrivate::equalStrings(rhs, lhs);
}

bool QtPrivate::equalStrings(QLatin1String lhs, QLatin1String rhs) noexcept
{
    return lhs.size() == rhs.size() && (!lhs.size() || qstrncmp(lhs.data(), rhs.data(), lhs.size()) == 0);
}

template<typename Haystack, typename Needle>
bool qt_starts_with_impl(Haystack haystack, Needle needle, Qt::CaseSensitivity cs) noexcept {
    if (haystack.isNull()) {
        return needle.isNull();
    }
    const auto haystackLen = haystack.size();
    const auto needleLen = needle.size();
    if (haystackLen == 0) {
        return needleLen == 0;
    }
    if (needleLen > haystackLen) {
        return false;
    }
    return QtPrivate::compareStrings(haystack.left(needleLen), needle, cs) == 0;
}

static inline bool qt_starts_with(QStringView haystack, QStringView needle, Qt::CaseSensitivity cs) {
    return qt_starts_with_impl(haystack, needle, cs);
}

static inline bool qt_starts_with(QStringView haystack, QLatin1String needle, Qt::CaseSensitivity cs) {
    return qt_starts_with_impl(haystack, needle, cs);
}

static inline bool qt_starts_with(QStringView haystack, QChar needle, Qt::CaseSensitivity cs) {
    if (haystack.size() == 0) {
        return false;
    }
    if (cs == Qt::CaseSensitive) {
        return haystack.front() == needle;
    }
    else {
        return QChar::foldCase(haystack.front()) == QChar::foldCase(needle);
    }
}

bool QtPrivate::startsWith(QStringView haystack, QStringView needle, Qt::CaseSensitivity cs) noexcept
{
    return qt_starts_with_impl(haystack, needle, cs);
}

bool QtPrivate::startsWith(QStringView haystack, QLatin1String needle, Qt::CaseSensitivity cs) noexcept
{
    return qt_starts_with_impl(haystack, needle, cs);
}

bool QtPrivate::startsWith(QLatin1String haystack, QStringView needle, Qt::CaseSensitivity cs) noexcept
{
    return qt_starts_with_impl(haystack, needle, cs);
}

bool QtPrivate::startsWith(QLatin1String haystack, QLatin1String needle, Qt::CaseSensitivity cs) noexcept {
    return qt_starts_with_impl(haystack, needle, cs);
}

template <typename Haystack, typename Needle>
bool qt_ends_with_impl(Haystack haystack, Needle needle, Qt::CaseSensitivity cs) noexcept {
    if (haystack.isNull()) {
        return needle.isNull();
    }
    const auto haystackLen = haystack.size();
    const auto needleLen = needle.size();
    if (haystackLen == 0) {
        return needleLen == 0;
    }
    if (haystackLen < needleLen) {
        return false;
    }
    return QtPrivate::compareStrings(haystack.right(needleLen), needle, cs) == 0;
}

static inline bool qt_ends_with(QStringView haystack, QStringView needle, Qt::CaseSensitivity cs) {
    return qt_ends_with_impl(haystack, needle, cs);
}

static inline bool qt_ends_with(QStringView haystack, QLatin1String needle, Qt::CaseSensitivity cs) {
    return qt_ends_with_impl(haystack, needle, cs);
}

static inline bool qt_ends_with(QStringView haystack, QChar needle, Qt::CaseSensitivity cs) {
    if (haystack.size() == 0) {
        return false;
    }
    if (cs == Qt::CaseSensitive) {
        return haystack.back() == needle;
    }
    else {
        return QChar::foldCase(haystack.back()) == QChar::foldCase(needle);
    }
}

bool QtPrivate::endsWith(QStringView haystack, QStringView needle, Qt::CaseSensitivity cs) noexcept
{
    return qt_ends_with_impl(haystack, needle, cs);
}

bool QtPrivate::endsWith(QStringView haystack, QLatin1String needle, Qt::CaseSensitivity cs) noexcept
{
    return qt_ends_with_impl(haystack, needle, cs);
}

bool QtPrivate::endsWith(QLatin1String haystack, QStringView needle, Qt::CaseSensitivity cs) noexcept
{
    return qt_ends_with_impl(haystack, needle, cs);
}

bool QtPrivate::endsWith(QLatin1String haystack, QLatin1String needle, Qt::CaseSensitivity cs) noexcept
{
    return qt_ends_with_impl(haystack, needle, cs);
}

static inline qsizetype qFindChar(QStringView str, QChar ch, qsizetype from, Qt::CaseSensitivity cs) noexcept {
    if (from < 0) {
        from = qMax(from + str.size(), qsizetype(0));
    }
    if (from < str.size()) {
        const char16_t *s = str.utf16();
        char16_t c = ch.unicode();
        const char16_t *n = s + from;
        const char16_t *e = s + str.size();
        if (cs == Qt::CaseSensitive) {
            n = QtPrivate::qustrchr(QStringView(n, e), c);
            if (n != e) {
                return n - s;
            }
        }
        else {
            c = QChar::foldCase(c);
            --n;
            while (++n != e) {
                if (QChar::foldCase(*n) == c) {
                    return n - s;
                }
            }
        }
    }
    return -1;
}

qsizetype QtPrivate::findString(QStringView haystack0, qsizetype from, QStringView needle0, Qt::CaseSensitivity cs) noexcept
{
    const qsizetype l = haystack0.size();
    const qsizetype sl = needle0.size();
    if (from < 0) {
        from += l;
    }
    //各种特殊case处理
    if (std::size_t(sl + from) > std::size_t(l)) {
        return -1;
    }
    if (sl == 0) {
        return from;
    }
    if (l == 0) {
        return -1;
    }
    if (sl == 1) {
        return qFindChar(haystack0, needle0[0], from, cs);
    }
    if (l > 500 && sl > 5) {
        return qFindStringBoyerMoore(haystack0, from, needle0, cs);
    }

    auto sv = [sl](const char16_t  *v) { return QStringView(v, sl); };

    const char16_t *needle = needle0.utf16();
    const char16_t *haystack = haystack0.utf16() + from;
    const char16_t *end = haystack0.utf16() + (l - sl);
    const std::size_t sl_minus_1 = sl - 1;
    std::size_t hashNeedle = 0;
    std::size_t hashHaystack = 0;
    qsizetype idx;

    if (cs == Qt::CaseSensitive) {
        for (idx = 0; idx < sl; ++idx) {
            hashNeedle = (hashNeedle << 1) + needle[idx];
            hashHaystack = (hashHaystack << 1) + haystack[idx];
        }
        hashHaystack -= haystack[sl_minus_1];
        while (haystack <= end) {
            hashHaystack += haystack[sl_minus_1];
            if (hashHaystack == hashNeedle && QtPrivate::compareStrings(needle0, sv(haystack), Qt::CaseSensitive) == 0) {
                return haystack - haystack0.utf16();
            }
            REHASH(*haystack);
            ++haystack;
        }
    }
    else {
        const char16_t *haystack_start = haystack0.utf16();
        for (idx = 0; idx < sl; ++idx) {
            hashNeedle = (hashNeedle << 1) + QChar::foldCase(needle + idx, needle);
            hashHaystack = (hashHaystack << 1) + QChar::foldCase(haystack + idx, haystack_start);
        }
        hashHaystack -= QChar::foldCase(haystack + sl_minus_1, haystack_start);
        while (haystack <= end) {
            hashHaystack += QChar::foldCase(haystack + sl_minus_1, haystack_start);
            if (hashHaystack == hashNeedle && QtPrivate::compareStrings(needle0, sv(haystack), Qt::CaseInsensitive) == 0) {
                return haystack - haystack0.utf16();
            }
            REHASH(QChar::foldCase(haystack, haystack_start));
            ++haystack;
        }
    }
    return -1;
}

qsizetype QtPrivate::findString(QStringView haystack, qsizetype from, QLatin1String needle, Qt::CaseSensitivity cs) noexcept
{
    if (haystack.size() < needle.size()) {
        return -1;
    }
    QVarLengthArray<char16_t> s(needle.size());
    qt_from_latin1(s.data(), needle.latin1(), needle.size());
    return QtPrivate::findString(haystack, from, QStringView(reinterpret_cast<const QChar *>(s.constData()), s.size()), cs);
}

qsizetype QtPrivate::findString(QLatin1String haystack, qsizetype from, QStringView needle, Qt::CaseSensitivity cs) noexcept
{
    if (haystack.size() < needle.size()) {
        return -1;
    }
    QVarLengthArray<char16_t> s(haystack.size());
    qt_from_latin1(s.data(), haystack.latin1(), haystack.size());
    auto t = reinterpret_cast<const QChar *>(s.constData());
    return QtPrivate::findString(QStringView(t, s.size()), from, needle, cs);
}

qsizetype QtPrivate::findString(QLatin1String haystack, qsizetype from, QLatin1String needle, Qt::CaseSensitivity cs) noexcept {
    if (haystack.size() < needle.size()) {
        return -1;
    }
    QVarLengthArray<char16_t> h(haystack.size());
    qt_from_latin1(h.data(), haystack.latin1(), haystack.size());
    QVarLengthArray<char16_t> n(needle.size());
    return QtPrivate::findString(QStringView(reinterpret_cast<const QChar *>(h.constData()), h.size()), from,
                                 QStringView(reinterpret_cast<const QChar *>(n.constData()), n.size()), cs);
}

bool QtPrivate::isLatin1(QLatin1String s) noexcept {
    return true;
}

bool QtPrivate::isLatin1(QStringView s) noexcept {
    const QChar *ptr = s.begin();
    const QChar *end = s.end();
    while (ptr != end) {
        if ((*ptr++).unicode() > 0xff) {
            return false;
        }
    }
    return true;
}

namespace {
    template <typename StringView>
    StringView qt_trimmed(StringView s) noexcept
    {
        auto begin = s.begin();
        auto end = s.end();
        QStringAlgorithms<const StringView>::trimmed_helper_positions(begin, end);
        return StringView{ begin, end };
    }
};

QLatin1String QtPrivate::trimmed(QLatin1String s) noexcept
{
    return qt_trimmed(s);
}

template <typename Haystack>
static inline qsizetype qLastIndexOf(Haystack haystack, QChar needle,
                                     qsizetype from, Qt::CaseSensitivity cs) noexcept
{
    if (haystack.size() == 0)
        return -1;
    if (from < 0)
        from += haystack.size();
    else if (std::size_t(from) > std::size_t(haystack.size()))
        from = haystack.size() - 1;
    if (from >= 0) {
        char16_t c = needle.unicode();
        const auto b = haystack.data();
        auto n = b + from;
        if (cs == Qt::CaseSensitive) {
            for (; n >= b; --n)
                if (valueTypeToUtf16(*n) == c)
                    return n - b;
        } else {
            c = QChar::foldCase(c);
            for (; n >= b; --n)
                if (QChar::foldCase(valueTypeToUtf16(*n)) == c)
                    return n - b;
        }
    }
    return -1;
}

template<typename Haystack, typename Needle>
static qsizetype qLastIndexOf(Haystack haystack0, qsizetype from,
                              Needle needle0, Qt::CaseSensitivity cs) noexcept
{
    const qsizetype sl = needle0.size();
    if (sl == 1)
        return qLastIndexOf(haystack0, needle0.front(), from, cs);

    const qsizetype l = haystack0.size();
    if (from < 0)
        from += l;
    if (from == l && sl == 0)
        return from;
    const qsizetype delta = l - sl;
    if (std::size_t(from) > std::size_t(l) || delta < 0)
        return -1;
    if (from > delta)
        from = delta;

    auto sv = [sl](const typename Haystack::value_type *v) { return Haystack(v, sl); };

    auto haystack = haystack0.data();
    const auto needle = needle0.data();
    const auto *end = haystack;
    haystack += from;
    const std::size_t sl_minus_1 = sl - 1;
    const auto *n = needle + sl_minus_1;
    const auto *h = haystack + sl_minus_1;
    std::size_t hashNeedle = 0, hashHaystack = 0;
    qsizetype idx;

    if (cs == Qt::CaseSensitive) {
        for (idx = 0; idx < sl; ++idx) {
            hashNeedle = (hashNeedle << 1) + valueTypeToUtf16(*(n - idx));
            hashHaystack = (hashHaystack << 1) + valueTypeToUtf16(*(h - idx));
        }
        hashHaystack -= valueTypeToUtf16(*haystack);

        while (haystack >= end) {
            hashHaystack += valueTypeToUtf16(*haystack);
            if (hashHaystack == hashNeedle
                && QtPrivate::compareStrings(needle0, sv(haystack), Qt::CaseSensitive) == 0)
                return haystack - end;
            --haystack;
            REHASH(valueTypeToUtf16(haystack[sl]));
        }
    } else {
        for (idx = 0; idx < sl; ++idx) {
            hashNeedle = (hashNeedle << 1) + foldCaseHelper(n - idx, needle);
            hashHaystack = (hashHaystack << 1) + foldCaseHelper(h - idx, end);
        }
        hashHaystack -= foldCaseHelper(haystack, end);

        while (haystack >= end) {
            hashHaystack += foldCaseHelper(haystack, end);
            if (hashHaystack == hashNeedle
                && QtPrivate::compareStrings(sv(haystack), needle0, Qt::CaseInsensitive) == 0)
                return haystack - end;
            --haystack;
            REHASH(foldCaseHelper(haystack + sl, end));
        }
    }
    return -1;
}

qsizetype QtPrivate::lastIndexOf(QStringView haystack, qsizetype from, QStringView needle, Qt::CaseSensitivity cs) noexcept
{
    return qLastIndexOf(haystack, from, needle, cs);
}

qsizetype QtPrivate::lastIndexOf(QStringView haystack, qsizetype from, QLatin1String needle, Qt::CaseSensitivity cs) noexcept
{
    return qLastIndexOf(haystack, from, needle, cs);
}

qsizetype QtPrivate::lastIndexOf(QLatin1String haystack, qsizetype from, QStringView needle, Qt::CaseSensitivity cs) noexcept
{
    return qLastIndexOf(haystack, from, needle, cs);
}

qsizetype QtPrivate::lastIndexOf(QLatin1String haystack, qsizetype from, QLatin1String needle, Qt::CaseSensitivity cs) noexcept
{
    return qLastIndexOf(haystack, from, needle, cs);
}


QT_END_NAMESPACE