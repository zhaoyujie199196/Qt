//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QBYTEARRAY_H
#define QBYTEARRAY_H

#include <QtCore/qglobal.h>
#include "qbytearrayview.h"
#include "qbytearrayalgorithms.h"
#include "QtCore/qarraydatapointer.h"
#include "QtCore/qnamespace.h"
#include "QtCore/qcontainerfwd.h"

#ifndef QT5_NULL_STRINGS
#define QT5_NULL_STRINGS 1
#endif

QT_BEGIN_NAMESPACE

//QByteArrayData里面放的char类型？
using QByteArrayData = QArrayDataPointer<char>;
//Data指针为空，str指针有内容，全局静态区的str，不需要额外分配内存
//编译器优化，同样的裸字符串内存地址一样，其实是同一份数据
#define QByteArrayLiteral(str) (QByteArray(QByteArrayData(nullptr, const_cast<char *>(str), sizeof(str) - 1)))

class QByteArray
{
    friend class QString;
public:
    using DataPointer = QByteArrayData;
private:
    typedef QTypedArrayData<char> Data;
    DataPointer d;
    static const char _empty;  //TODO 这个设计是什么目的？

public:
    //base64编码 原理：https://zhuanlan.zhihu.com/p/146599482
    enum class Base64DecodingStatus {
        Ok,
        IllegalInputLength,
        IllegalCharacter,
        IllegalPadding,
    };

    enum Base64Option {
        Base64Encoding = 0,
        Base64UrlEncoding = 1,

        KeepTrailingEquals = 0,
        OmitTrailingEquals = 2,

        IgnoreBase64DecodingErrors = 0,
        AbortOnBase64DecodingErrors = 4,
    };
    Q_DECLARE_FLAGS(Base64Options, Base64Option)  //定义了内部的QFlags

public:
    typedef qsizetype size_type;
    typedef qptrdiff difference_type;
    typedef const char & const_reference;
    typedef char &reference;
    typedef char *pointer;
    typedef const char *const_pointer;
    typedef char value_type;

public:
    //base64编码 ：https://zhuanlan.zhihu.com/p/146599482
    class FromBase64Result;

public:
    inline constexpr QByteArray() noexcept;
    QByteArray(const char *, qsizetype size = -1) noexcept;
    QByteArray(qsizetype size, char c) noexcept;
    QByteArray(qsizetype size, Qt::Initialization) noexcept;
    QByteArray(const QByteArray &other) noexcept;
    ~QByteArray() {}

    QByteArray &operator=(const QByteArray &) noexcept;
    QByteArray &operator=(const char *str);

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QByteArray)

    inline void swap(QByteArray &other) noexcept
    { qSwap(d, other.d); }
    inline bool isEmpty() const { return size() == 0; }
    void resize(qsizetype size);
    QByteArray &fill(char c, qsizetype size = -1);
    inline qsizetype capacity() const { return qsizetype(d.constAllocatedCapacity()); }
    void reserve(qsizetype size);
    void squeeze();

    inline operator const char*() const { return data(); }
    inline operator const void*() const { return data(); }

    inline char *data()
    { detach(); Q_ASSERT(d.data()); return d.data(); }
    inline const char *data() const
    { return d.data(); }
    inline const char *constData() const
    { return data(); }
    inline void detach();
    inline bool isDetached() const { return !d->isShared(); }
    inline bool isSharedWith(const QByteArray &other) const
    { return data() == other.data() && size() == other.size(); }
    void clear() { d.clear(); }

    inline char at(qsizetype i) const
    { Q_ASSERT(size_t(i) < size_t(size())); return d.data()[i]; }
    inline char operator[](qsizetype i) const
    { Q_ASSERT(size_t(i) < size_t(size())); return d.data()[i]; }
    inline char &operator[](qsizetype i)
    { Q_ASSERT(i >= 0 && i < size()); return data()[i]; }
    char front() const { return at(0); }
    inline char &front() { return operator[](0); }
    char back() const { return at(size() - 1); }
    inline char &back() { return operator[](size() - 1); }

    QList<QByteArray> split(char sep) const;

    qsizetype indexOf(char c, qsizetype from = 0) const;
    qsizetype indexOf(QByteArrayView bv, qsizetype from = 0) const
    { return QtPrivate::findByteArray(qToByteArrayViewIgnoringNull(*this), from, bv); }

    qsizetype lastIndexOf(char c, qsizetype from = -1) const;
    qsizetype lastIndexOf(QByteArrayView bv, qsizetype from = 0) const
    { return QtPrivate::lastIndexOf(qToByteArrayViewIgnoringNull(*this), from, bv); }

    inline bool contains(char c) const
    { return indexOf(c) != -1; }
    inline bool contains(QByteArrayView bv) const
    { return indexOf(bv) != -1; }
    qsizetype count(char c) const;
    qsizetype count(QByteArrayView bv) const
    { return QtPrivate::count(qToByteArrayViewIgnoringNull(*this), bv); }

    inline int compare(QByteArrayView a, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;

    QByteArray left(qsizetype len) const;
    QByteArray right(qsizetype len) const;
    QByteArray mid(qsizetype index, qsizetype len = -1) const;

    QByteArray first(qsizetype n) const
    { Q_ASSERT(n >= 0); Q_ASSERT(n <= size()); return QByteArray(data(), n); }
    QByteArray last(qsizetype n) const
    { Q_ASSERT(n >= 0); Q_ASSERT(n <= size()); return QByteArray(data() + size() - n, n);}
    QByteArray sliced(qsizetype pos) const  //[pos, end());
    { Q_ASSERT(pos >= 0); Q_ASSERT(pos <= size()); return QByteArray(data() + pos, size() - pos); }
    QByteArray sliced(qsizetype pos, qsizetype n) const
    { Q_ASSERT(pos >= 0); Q_ASSERT(n >= 0); Q_ASSERT(size_t(pos) + size_t(n) <= size_t(size())); return QByteArray(data() + pos, n); }
    QByteArray chopped(qsizetype len) const  //截掉后面n个
    { Q_ASSERT(len >= 0); Q_ASSERT(len <= size()); return first(size() - len); }

    bool startsWith(QByteArrayView bv) const
    { return QtPrivate::startsWith(qToByteArrayViewIgnoringNull(*this), bv); }
    bool startsWith(char c) const
    { return size() > 0 && front() == c; }

    bool endsWith(char c) const { return size() > 0 && back() == c; }
    bool endsWith(QByteArrayView bv) const
    { return QtPrivate::endsWith(qToByteArrayViewIgnoringNull(*this), bv); }

    bool isUpper() const;  //只包含ascii大写字母
    bool isLower() const;  //只包含ascii小写字母

    void truncate(qsizetype pos);  //截断到pos的位置，pos不保留
    void chop(qsizetype n); //把后面的n个砍掉

    //不会改变数组原始数据，会复制一份数据。如果是右值或者没有引用，会直接清空原数据
    //普通对象与const &对象， 引用对象会调用 const &函数，调用到toLower_helper(const &)
    QByteArray toLower() const & { return toLower_helper(*this); }
    //右值调用 && 函数, 调用的到toLower_helper(&)
    QByteArray toLower() && { return toLower_helper(*this); }
    QByteArray toUpper() const & { return toUpper_helper(*this); }
    QByteArray toUpper() && { return toUpper_helper(*this); }
    QByteArray trimmed() const & { return trimmed_helper(*this); }
    QByteArray trimmed() && { return trimmed_helper(*this); }
    //移除前后的空白字符，使用单个空白字符替换中间的空白字符
    QByteArray simplified() const & { return simplified_helper(*this); }
    QByteArray simplified() && { return simplified_helper(*this); }

    //左对齐width位，不够位数使用fill填充，truncate是否截断
    QByteArray leftJustified(qsizetype width, char fill = ' ', bool truncate = false) const;
    QByteArray rightJustified(qsizetype width, char fill = ' ', bool truncate = false);

    QByteArray toBase64(Base64Options options = Base64Encoding) const;
    QByteArray toHex(char separator = '\0') const;
    QByteArray toPercentEncoding(const QByteArray &exclude = QByteArray(),
                                 const QByteArray &include = QByteArray(),
                                 char percent = '%') const;

    QByteArray &setNum(short, int base = 10);
    QByteArray &setNum(ushort, int base = 10);
    QByteArray &setNum(int, int base = 10);
    QByteArray &setNum(uint, int base = 10);
    QByteArray &setNum(long, int base = 10);
    QByteArray &setNum(ulong, int base = 10);
    QByteArray &setNum(qlonglong, int base = 10);
    QByteArray &setNum(qulonglong, int base = 10);
    QByteArray &setNum(float, char format = 'g', int precision = 6);
    QByteArray &setNum(double, char format = 'g', int precision = 6);
    QByteArray &setRawData(const char *a, qsizetype n);

    static QByteArray number(int, int base = 10);
    static QByteArray number(uint, int base = 10);
    static QByteArray number(long, int base = 10);
    static QByteArray number(ulong, int base = 10);
    static QByteArray number(qlonglong, int base = 10);
    static QByteArray number(qulonglong, int base = 10);
    static QByteArray number(double, char format = 'g', int precision = 6);

    //raw: 原始的，不管理data的内存，所以第一个参数为nullptr
    static QByteArray fromRawData(const char *data, qsizetype size)
    {
        return QByteArray(DataPointer(nullptr, const_cast<char *>(data), size));
    }

    //前插入
    QByteArray &prepend(char c)
    { return insert(0, QByteArrayView(&c, 1)); }
    QByteArray &prepend(qsizetype count, char c)
    { return insert(0, count, c); }
    QByteArray &prepend(const char *s)
    { return insert(0, QByteArrayView(s, qsizetype(qstrlen(s)))); }
    QByteArray &prepend(const char *s, qsizetype len)
    { return insert(0, QByteArrayView(s, len)); }
    QByteArray &prepend(const QByteArray &a);
    QByteArray &prepend(QByteArrayView a)
    { return insert(0, a); }

    //后插入
    QByteArray &append(char c);
    QByteArray &append(qsizetype count, char c)
    { return insert(size(), count, c); }
    QByteArray &append(const char *s)
    { return append(QByteArrayView(s, qsizetype(qstrlen(s)))); }
    QByteArray &append(const char *s, qsizetype len)
    { return append(QByteArrayView(s, len)); }
    QByteArray &append(const QByteArray &a);
    QByteArray &append(QByteArrayView a)
    { return insert(size(), a); }

    QByteArray &insert(qsizetype i, QByteArrayView data);
    QByteArray &insert(qsizetype i, const char *s)
    { return insert(i, QByteArrayView(s)); }
    QByteArray &insert(qsizetype i, const QByteArray &data)
    { return insert(i, QByteArrayView(data)); }
    QByteArray &insert(qsizetype i, qsizetype count, char c);
    QByteArray &insert(qsizetype i, char c)
    { return insert(i, QByteArrayView(&c, 1)); }
    //len可能会超出s的范围，不会做有效性判断
    QByteArray &insert(qsizetype i, const char *s, qsizetype len)
    { return insert(i, QByteArrayView(s, len)); }

    QByteArray &remove(qsizetype index, qsizetype len);
    template <typename Predicate>
    QByteArray &removeIf(Predicate pred)
    {
        QtPrivate::sequential_erase_if(*this, pred);
        return *this;
    }

    QByteArray &replace(qsizetype index, qsizetype len, const char *s, qsizetype alen)
    { return replace(index, len, QByteArrayView(s, alen)); }
    QByteArray &replace(qsizetype index, qsizetype len, QByteArrayView s);
    QByteArray &replace(char before, QByteArrayView after)
    { return replace(QByteArrayView(&before, 1), after); }
    QByteArray &replace(const char *before, qsizetype bsize, const char *after, qsizetype asize)
    { return replace(QByteArrayView(before, bsize), QByteArrayView(after, asize)); }
    QByteArray &replace(QByteArrayView before, QByteArrayView after);
    QByteArray &replace(char before, char after);

    QByteArray &operator+=(char c)
    { return append(c); }
    QByteArray &operator+=(const char *s)
    { return append(s); }
    QByteArray &operator+=(const QByteArray &a)
    { return append(a); }
    QByteArray &operator+=(QByteArrayView a)
    { return append(a); }

    //STL 适配接口
    typedef char *iterator;
    typedef const char *const_iterator;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    iterator begin() { return data(); }
    const_iterator begin() const { return data(); }
    const_iterator cbegin() const { return data(); }
    const_iterator constBegin() const { return data(); }
    iterator end() { return data() + size(); }
    const_iterator end() const { return data() + size(); }
    const_iterator cend() const { return data() + size(); }
    const_iterator constEnd() const { return data() + size(); }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    friend inline bool operator==(const QByteArray &a1, const QByteArray &a2) noexcept
    { return QByteArrayView(a1) == QByteArrayView(a2); }
    //a1，a2都被隐式转换成了QByteArrayView
    friend inline bool operator==(const QByteArray &a1, const char *a2) noexcept
    { return a2 ? QtPrivate::compareMemory(a1, a2) == 0 : a1.isEmpty(); }
    friend inline bool operator==(const char *a1, const QByteArray &a2) noexcept
    { return a1 ? QtPrivate::compareMemory(a1, a2) == 0 : a2.isEmpty(); }
    friend inline bool operator!=(const QByteArray &a1, const QByteArray &a2) noexcept
    { return !(a1 == a2); }
    friend inline bool operator!=(const QByteArray &a1, const char *a2) noexcept
    { return a2 ? QtPrivate::compareMemory(a1, a2) != 0 : !a1.isEmpty(); }
    friend inline bool operator!=(const char *a1, const QByteArray &a2) noexcept
    { return a1 ? QtPrivate::compareMemory(a1, a2) != 0 : !a2.isEmpty(); }

    inline qsizetype size() const { return d->size; }
    inline qsizetype count() const { return size(); }
    inline qsizetype length() const { return size(); }

    bool isNull() const { return d->isNull(); }

    iterator erase(const_iterator first, const_iterator last);

    inline DataPointer &data_ptr() { return d; }
    explicit inline QByteArray(const DataPointer &dd)
            : d(dd)
    {
    }

    QByteArray repeated(qsizetype times) const;

    static inline QByteArray fromStdString(const std::string &s)
    { return QByteArray(s.data(), s.size()); }
    inline std::string toStdString() const
    { return std::string(constData(), length()); }

    static FromBase64Result fromBase64Encoding(QByteArray &&base64, Base64Options options = Base64Encoding);
    static FromBase64Result fromBase64Encoding(const QByteArray &base64, Base64Options options = Base64Encoding);
    static QByteArray fromBase64(const QByteArray &base64, Base64Options options = Base64Encoding);
    static QByteArray fromHex(const QByteArray &hexEncoded);
    static QByteArray fromPercentEncoding(const QByteArray &pctEncoded, char percent = '%');


private:
    void createEmpty() noexcept {
        d = DataPointer::fromRawData(&_empty, 0);
    }
    void reallocData(qsizetype alloc, QArrayData::AllocationOption option);

    static QByteArray toLower_helper(const QByteArray &a);
    static QByteArray toLower_helper(QByteArray &a);
    static QByteArray toUpper_helper(const QByteArray &a);
    static QByteArray toUpper_helper(QByteArray &a);
    static QByteArray trimmed_helper(const QByteArray &a);
    static QByteArray trimmed_helper(QByteArray &a);
    static QByteArray simplified_helper(const QByteArray &a);
    static QByteArray simplified_helper(QByteArray &a);
};

Q_DECLARE_SHARED(QByteArray)

//operator +
inline const QByteArray operator+(const QByteArray &a1, const QByteArray &a2)
{ return QByteArray(a1) += a2; }
inline const QByteArray operator+(const QByteArray &a1, const char *a2)
{ return QByteArray(a1) += a2; }
inline const QByteArray operator+(const QByteArray &a1, char a2)
{ return QByteArray(a1) += a2; }
inline const QByteArray operator+(const char *a1, const QByteArray &a2)
{ return QByteArray(a1) += a2; }
inline const QByteArray operator+(char a1, const QByteArray &a2)
{ return QByteArray(&a1, 1) += a2; }

class QByteArray::FromBase64Result
{
public:
    QByteArray decoded;
    QByteArray::Base64DecodingStatus decodingStatus;

    void swap(QByteArray::FromBase64Result &other) noexcept
    {
        qSwap(decoded, other.decoded);
        qSwap(decodingStatus, other.decodingStatus);
    }

    explicit operator bool() const noexcept { return decodingStatus == QByteArray::Base64DecodingStatus::Ok; }

    friend inline bool operator==(const QByteArray::FromBase64Result &lhs, const QByteArray::FromBase64Result &rhs) noexcept
    {
        if (lhs.decodingStatus != rhs.decodingStatus) {
            return false;
        }
        //zhaoyujie TODO 如果不是ok状态，decoded可以不用相等？
        if (lhs.decodingStatus == QByteArray::Base64DecodingStatus::Ok && lhs.decoded != rhs.decoded) {
            return false;
        }
        return true;
    }

    friend inline bool operator!=(const QByteArray::FromBase64Result &lhs, const QByteArray::FromBase64Result &rhs) noexcept
    {
        return !(lhs == rhs);
    }
};
Q_DECLARE_SHARED(QByteArray::FromBase64Result)

inline constexpr QByteArray::QByteArray() noexcept
{
}

inline void QByteArray::detach() {
    if (d->needsDetach()) {
        reallocData(size(), QArrayData::KeepSize);
    }
}

inline int QByteArray::compare(QByteArrayView a, Qt::CaseSensitivity cs) const noexcept
{
    if (cs == Qt::CaseSensitive) {
        return QtPrivate::compareMemory(*this, a);
    }
    else {
        return qstrnicmp(data(), size(), a.data(), a.size());
    }
}

inline namespace QtLiterals {
    //用户自定义字面量语法，https://blog.csdn.net/K346K346/article/details/85322227
    inline QByteArray operator"" _qba(const char *str, size_t size) noexcept
    {
        return QByteArray(QByteArrayData(nullptr, const_cast<char *>(str), qsizetype(size)));
    }
}

QT_END_NAMESPACE

#endif //QBYTEARRAY_H
