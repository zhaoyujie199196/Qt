//
// Created by Yujie Zhao on 2023/2/22.
//
#include "qbytearray.h"
#include "qstringalgorithms_p.h"
#include "qlocale_p.h"
#include "QtCore/qvarlengtharray.h"

QT_BEGIN_NAMESPACE

const char QByteArray::_empty = '\0';

inline uchar asciiUpper(uchar c);

inline uchar asciiLower(uchar c);

qsizetype qFindByteArray(
        const char *haystack0, qsizetype haystackLen, qsizetype from,
        const char *needle0, qsizetype needleLen
        );

QByteArray::QByteArray(const char *data, qsizetype size) noexcept
{
    if (!data) {
        d = DataPointer();
    }
    else {
        if (size < 0) {
            size = qstrlen(data);
        }
        if (!size) {
            createEmpty();
        }
        else {
            d = DataPointer(Data::allocate(size), size);
            Q_CHECK_PTR(d.data());
            memcpy(d.data(), data, size);
            d.data()[size] = '\0';  //分配内存时最后留出了空间
        }
    }
}

QByteArray::QByteArray(qsizetype size, char c) noexcept {
    if (size <= 0) {
        createEmpty();
    }
    else {
        d = DataPointer(Data::allocate(size), size);
        Q_CHECK_PTR(d.data());
        memset(d.data(), c, size);
        d.data()[size] = '\0';
    }
}

QByteArray::QByteArray(qsizetype size, Qt::Initialization) noexcept {
    if (size <= 0) {
        createEmpty();
    }
    else {
        d = DataPointer(Data::allocate(size), size);
        Q_CHECK_PTR(d.data());
        d.data()[size] = '\0';
    }
}

QByteArray::QByteArray(const QByteArray &other) noexcept
    : d(other.d)
{

}

QByteArray &QByteArray::operator=(const QByteArray &other) noexcept {
    d = other.d;
    return *this;
}

QByteArray &QByteArray::operator=(const char *str){
    if (!str) {
        d.clear();  //空
    }
    else if (!*str) {
        createEmpty();
    }
    else {
        const qsizetype len = qsizetype(strlen(str));
        const auto capacityAtEnd = d->allocatedCapacity() - d.freeSpaceAtBegin();
        if (d->needsDetach() || len > capacityAtEnd || (len < d->size && len < (capacityAtEnd >> 1))) {
            reallocData(len, QArrayData::KeepSize);
        }
        memcpy(d.data(), str, len + 1);
        d.size = len;
    }
    return *this;
}

void QByteArray::reallocData(qsizetype alloc, QArrayData::AllocationOption option)
{
    if (!alloc) {  //size为0，直接使用默认
        d = DataPointer::fromRawData(&_empty, 0);
        return;
    }
    const bool cannotUseReallocate = d.freeSpaceAtBegin() > 0;  //如果头部有空隙，压缩空隙
    if (d->needsDetach() || cannotUseReallocate) {
        DataPointer dd(Data::allocate(alloc, option), qMin(alloc, d.size));
        Q_CHECK_PTR(dd.data());
        if (dd.size > 0) {
            ::memcpy(dd.data(), d.data(), dd.size);
        }
        dd.data()[dd.size] = 0;
        d = dd;
    }
    else {
        //不需要分离，头部没有空隙，尝试原地增减内存，提升效率
        d->reallocate(alloc, option);
    }
}

void QByteArray::resize(qsizetype size)
{
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

QByteArray &QByteArray::fill(char c, qsizetype size) {
    resize(size < 0 ? this->size() : size);
    if (this->size()) {
        memset(d.data(), c, this->size());
    }
    return *this;
}

void QByteArray::reserve(qsizetype asize)
{
    if (d->needsDetach() || asize > capacity() - d->freeSpaceAtBegin()) {
        reallocData(qMax<qsizetype>(size(), asize), QArrayData::KeepSize);
    }
    if (d->constAllocatedCapacity()) {
        d->setFlag(Data::CapacityReserved);
    }
}

void QByteArray::squeeze()
{
    if (!d.isMutable()) {
        return;
    }
    if (d->needsDetach() || size() < capacity()) {
        reallocData(size(), QArrayData::KeepSize);
    }
    if (d->constAllocatedCapacity()) {
        d->clearFlag(Data::CapacityReserved);
    }
}

qsizetype QByteArray::indexOf(char ch, qsizetype from) const
{
    return qToByteArrayViewIgnoringNull(*this).indexOf(ch, from);
}

qsizetype QByteArray::lastIndexOf(char ch, qsizetype from) const
{
    return qToByteArrayViewIgnoringNull(*this).lastIndexOf(ch, from);
}

static inline qsizetype countCharHelper(QByteArrayView haystack, char needle) noexcept
{
    qsizetype num = 0;
    for (char ch : haystack) {
        if (ch == needle) {
            ++num;
        }
    }
    return num;
}

qsizetype QByteArray::count(char ch) const
{
    return static_cast<int>(countCharHelper(*this, ch));
}

QByteArray QByteArray::left(qsizetype len) const
{
    if (len >= size()) {
        return *this;
    }
    if (len < 0) {
        len = 0;
    }
    return QByteArray(data(), len);
}

QByteArray QByteArray::right(qsizetype len) const
{
    if (len >= size()) {
        return *this;
    }
    if (len < 0) {
        len = 0;
    }
    return QByteArray(end() - len, len);
}

QByteArray QByteArray::mid(qsizetype pos, qsizetype len) const
{
    qsizetype p = pos;
    qsizetype l = len;
    using namespace QtPrivate;
    switch(QContainerImplHelper::mid(size(), &p, &l)) {
        case QContainerImplHelper::Null:
            return QByteArray();
        case QContainerImplHelper::Empty:  //zhaoyjjie TODO NULL 和 empty有啥区别
            return QByteArray(DataPointer::fromRawData(&_empty, 0));
        case QContainerImplHelper::Full:
            return *this;
        case QContainerImplHelper::Subset:
            return QByteArray(d.data() + p, l);
    }
    Q_ASSERT(false);
    return QByteArray();
}

static constexpr inline bool isUpperCaseAscii(char c)
{
    return c >= 'A' && c <= 'Z';
}

bool QByteArray::isUpper() const
{
    if (!isEmpty()) {
        return false;
    }
    const char *d = data();
    for (qsizetype i = 0, max = size(); i < max; ++i) {
        if (!isUpperCaseAscii(d[i])) {
            return false;
        }
    }
    return true;
}

static constexpr inline bool isLowerCaseAscii(char c)
{
    return c >= 'a' && c <= 'z';
}

bool QByteArray::isLower() const
{
    if (isEmpty()) {
        return false;
    }
    const char *d = data();
    for (qsizetype i = 0, max = size(); i < max; ++i) {
        if (!isLowerCaseAscii(d[i])) {
            return false;
        }
    }
    return true;
}

void QByteArray::truncate(qsizetype pos) {
    if (pos < size()) {
        resize(pos);
    }
}

void QByteArray::chop(qsizetype n) {
    if (n > 0) {
        resize(size() - n);
    }
}

template <typename T>
static QByteArray toCase_template(T &input, uchar(*lookup)(uchar)) {
    const char *orig_begin = input.constBegin();
    const char *firstBad = orig_begin;
    const char *e = input.constEnd();
    //查找第一个需要转换的字符位置
    for ( ; firstBad != e; ++firstBad) {
        uchar ch = uchar(*firstBad);
        uchar converted = lookup(ch);
        if (ch != converted) {
            break;
        }
    }

    //查找不到需要转换的字符，返回原串。
    //调用operator=的重载，const &, &会调用const &的版本， &&会调用&&的版本
    //右值会保证原字符数组中的内容被转移到返回结果中
    if (firstBad == e) {
        return std::move(input);
    }
    QByteArray s = std::move(input);
    char *b = s.begin();
    char *p = b + (firstBad - orig_begin);
    e = b + s.size();
    for ( ; p != e; ++p) {
        *p = char(lookup(uchar(*p)));
    }
    return s;
}

QByteArray QByteArray::toLower_helper(const QByteArray &a) {
    return toCase_template(a, asciiLower);
}

QByteArray QByteArray::toLower_helper(QByteArray &a) {
    return toCase_template(a, asciiLower);
}

QByteArray QByteArray::toUpper_helper(const QByteArray &a) {
    return toCase_template(a, asciiUpper);
}

QByteArray QByteArray::toUpper_helper(QByteArray &a) {
    return toCase_template(a, asciiUpper);
}

QByteArray QByteArray::trimmed_helper(const QByteArray &a) {
    Q_ASSERT(false);
    return QStringAlgorithms<const QByteArray>::trimmed_helper(a);
}

QByteArray QByteArray::trimmed_helper(QByteArray &a) {
    Q_ASSERT(false); //TODO &&如何销毁原数组内容
    return QStringAlgorithms<QByteArray>::trimmed_helper(a);
}

QByteArray QByteArray::simplified_helper(const QByteArray &a) {
    Q_ASSERT(false);
    return QStringAlgorithms<const QByteArray>::simplified_helper(a);
}

QByteArray QByteArray::simplified_helper(QByteArray &a) {
    Q_ASSERT(false);
    return QStringAlgorithms<QByteArray>::simplified_helper(a);
}

QByteArray QByteArray::leftJustified(qsizetype width, char fill, bool truncate) const {
    QByteArray result;
    qsizetype len = size();
    qsizetype padlen = width - len;
    if (padlen > 0) {
        result.resize(len + padlen);
        if (len) {
            memcpy(result.d.data(), data(), len);
        }
        memset(result.d.data() + len, fill, padlen);
    }
    else {
        if (truncate) {
            result = left(width);
        }
        else {
            result = *this;
        }
    }
    return result;
}

QByteArray QByteArray::rightJustified(qsizetype width, char fill, bool truncate) {
    QByteArray result;
    qsizetype len = size();
    qsizetype padlen = width - len;
    if (padlen > 0) {
        result.resize(len + padlen);
        if (len) {
            memcpy(result.d.data() + padlen, data(), len);
        }
        memset(result.d.data(), fill, padlen);
    }
    else {
        if (truncate) {
            result = left(width);
        }
        else {
            result = *this;
        }
    }
    return result;
}

QByteArray &QByteArray::prepend(const QByteArray &ba) {
    //zhaoyujie TODO 为了提升效率？
    if (size() == 0 && ba.size() > d.constAllocatedCapacity() && ba.d.isMutable()) {
        return (*this = ba);
    }
    return prepend(QByteArrayView(ba));
}

QByteArray &QByteArray::append(const QByteArray &ba)
{
    //zhaoyujie TODO 为了提升效率？
    if (size() == 0 && ba.size() > d->freeSpaceAtEnd() && ba.d.isMutable()) {
        return (*this = ba);
    }
    return append(QByteArrayView(ba));
}

QByteArray &QByteArray::append(char ch) {
    d.detachAndGrow(QArrayData::GrowsAtEnd, 1, nullptr, nullptr);
    d->copyAppend(1, ch);
    d.data()[d.size] = '\0';
    return *this;
}

QByteArray &QByteArray::insert(qsizetype i, qsizetype count, char ch)
{
    if (i < 0 || count <= 0) {  //插入位置检测
        return *this;
    }
    if (i >= d->size) {
        //插入位置 > size, 中间的补空格
        //QList中的处理不允许超出范围
        d.detachAndGrow(Data::GrowsAtEnd, (i - d.size) + count, nullptr, nullptr);
        Q_CHECK_PTR(d.data());
        d->copyAppend(i - d->size, ' ');
        d->copyAppend(count, ch);
        d.data()[d.size] = '\0';
        return *this;
    }
    else {
        d->insert(i, count, ch);
        d.data()[d.size] = '\0';
        return *this;
    }
}

QByteArray &QByteArray::insert(qsizetype i, QByteArrayView data)
{
    const char *str = data.data();
    qsizetype size = data.size();
    if (i < 0 || size <= 0) {
        return *this;
    }
    if (i >= d->size) {
        //防止插入自身，使用detached拷贝原始数据
        DataPointer detached{};
        d.detachAndGrow(Data::GrowsAtEnd, (i - d.size) + size, &str, &detached);
        Q_CHECK_PTR(d.data());
        d->copyAppend(i - d->size, ' ');
        d->copyAppend(str, str + size);
        d.data()[d.size] = '\0';
        return *this;
    }
    if (!d->needsDetach() && QtPrivate::q_points_into_range(str, d.data(), d.data() + d.size)) {
       // 需要插入的内容会被部分覆盖
        QVarLengthArray<char> a(str, str + size);
        return insert(i, a);
        return *this;
    }
    else {
        d->insert(i, str, size);
        d.data()[d.size] = '\0';
        return *this;
    }
}

QByteArray &QByteArray::remove(qsizetype pos, qsizetype len) {
    if (len <= 0 || pos < 0 || size_t(pos) >= size_t(size())) {
        return *this;
    }
    detach();
    if (pos + len > d->size) {
        len = d->size - pos;
    }
    d->erase(d.begin() + pos, len);
    d.data()[d.size] = '\0';
    return *this;
}

QByteArray &QByteArray::setNum(short n, int base)
{
    return setNum(qlonglong(n), base);
}

QByteArray &QByteArray::setNum(ushort n, int base)
{
    return setNum(qulonglong(n), base);
}

QByteArray &QByteArray::setNum(int n, int base)
{
    return setNum(qlonglong(n), base);
}

QByteArray &QByteArray::setNum(uint n, int base)
{
    return setNum(qulonglong(n), base);
}

QByteArray &QByteArray::setNum(long n, int base)
{
    return setNum(qlonglong(n), base);
}

QByteArray &QByteArray::setNum(ulong n, int base)
{
    return setNum(qlonglong(n), base);
}

static char *qulltoa2(char *p, qulonglong n, int base)
{
    if (base < 2 || base > 36)  //超过36，字母表示不了
    {
        Q_ASSERT(false);
        base = 10;
    }
    const char b = 'a' - 10;
    do {
        const int c = n % base;
        n /= base;
        *--p = c + (c < 10 ? '0' : b); //<0,以'0'为基准，>=0的部分，以'a'为基准
    }while (n);
    return p;
}

QByteArray &QByteArray::setNum(qlonglong n, int base)
{
    const int buffsize = 66; //66位足够了
    char buff[buffsize];
    char *p;
    if (n < 0) {
        //zhaoyujie TODO 为什么这个取负运算这么复杂？防止最大负数直接取负导致溢出吗
        p = qulltoa2(buff + buffsize, qulonglong(-(1 + n)) + 1, base);
        *--p = '-';
    }
    else {
        p= qulltoa2(buff + buffsize, qulonglong(n), base);
    }
    clear();
    append(p, buffsize - (p - buff));
    return *this;
}

QByteArray &QByteArray::setNum(qulonglong n, int base)
{
    const int buffsize = 66;
    char buff[buffsize];
    char *p = qulltoa2(buff + buffsize, n, base);

    clear();
    append(p, buffsize - (p - buff));
    return *this;
}

QByteArray &QByteArray::setNum(float n, char format, int precision)
{
    return setNum(double(n), format, precision);
}

QByteArray &QByteArray::setNum(double n, char format, int precision)
{
    Q_ASSERT(false);
    return *this;
//    QLocaleData::DoubleForm form = QLocaleData::DFDecimal;
//    uint flags = QLocaleData::ZeroPadExponent;
//
//    char lower = asciiLower(uchar(format));
//
//    if (format != lower) {  //zhaoyjjie TODO 啥意思。。。
//        flags |= QLocaleData::CapitalEorX;
//    }
//
//    switch (lower) {
//        case 'f':
//            form = QLocaleData::DFDecimal;
//            break;
//        case 'e':
//            form = QLocaleData::DFExponent;
//            break;
//        case 'g':
//            form = QLocaleData::DFSignificantDigits;
//            break;
//        default:
//            Q_ASSERT(false);
//            break;
//    }
//    *this = QLocaleData::c()->doubleToString(n, precision, form, -1, flags).toUtf8();
//    return *this;
}

QList<QByteArray> QByteArray::split(char sep) const
{
    QList<QByteArray> list;
    qsizetype start = 0;
    qsizetype end;
    while ((end = indexOf(sep, start)) != -1) {
        list.append(mid(start, end - start));
        start = end + 1;
    }
    list.append(mid(start));
    return list;
}


QT_END_NAMESPACE