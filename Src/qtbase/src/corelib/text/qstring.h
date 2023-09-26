//
// Created by Yujie Zhao on 2023/2/28.
//

#ifndef QSTRING_H
#define QSTRING_H

#include "qlatin1string.h"
#include "qstringliteral.h"
#include "qbytearrayview.h"
#include "qbytearray.h"
#include "qstringalgorithms.h"
#include "qstringview.h"

QT_BEGIN_NAMESPACE

class QString
{
    typedef QTypedArrayData<char16_t> Data;
    friend class QByteArray;
    friend class QStringView;
public:
    typedef QStringPrivate DataPointer;

    typedef qsizetype size_type;
    typedef qptrdiff difference_type;
    typedef const QChar &const_reference;
    typedef QChar &reference;
    typedef QChar *pointer;
    typedef const QChar *const_pointer;
    typedef QChar value_type;

public:
    inline constexpr QString() noexcept;
    explicit QString(const QChar *unicode, qsizetype size = -1);
    QString(QChar c);
    QString(qsizetype size, QChar c);
    inline QString(QLatin1String latin1);
    QString(qsizetype size, Qt::Initialization);
    explicit QString(DataPointer &&dd) : d(std::move(dd)) {}

#ifdef __cpp_char8_t
    inline QString(const char8_t *str)
        : QString(fromUtf8(str))
    {
    }
#endif
    inline QString(const char *ch)
        : QString(fromUtf8(ch))
    {
    }
    inline QString(const QByteArray &ba)
        : QString(fromUtf8(ba))
    {
    }
    inline QString(const QString &) noexcept;
    inline QString(QString &&other) noexcept { qSwap(d, other.d); }
    inline ~QString();
    QString &operator=(QChar c);
    QString &operator=(const QString &) noexcept;
    QString &operator=(QLatin1String latin1);
    inline QString &operator=(const char *ch) { return (*this = fromUtf8(ch)); }
    inline QString &operator=(const QByteArray &a) { return (*this = fromUtf8(a)); }

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QString)

    inline void swap(QString &other) noexcept { qSwap(d, other.d); }
    inline qsizetype size() const { return d.size; }
    inline qsizetype count() const { return d.size; }
    inline qsizetype length() const { return d.size; }
    inline bool isEmpty() const { return d.size == 0; }

    inline bool isNull() const { return d->isNull(); }

    inline const QChar *unicode() const;
    inline QChar *data();
    inline const QChar *data() const;
    inline const QChar *constData() const;

    inline void detach();
    inline bool isDetached() const;
    inline bool isSharedWith(const QString &other) const { return d.isSharedWith(other.d); }
    void clear();

    inline const QChar at(qsizetype i) const;
    const QChar operator[](qsizetype i) const;
    QChar &operator[](qsizetype i);

    inline QChar front() const { return at(0); };
    inline QChar &front();
    inline QChar back() const { return at(size() - 1); }
    inline QChar &back();

    QString left(qsizetype n) const;
    QString right(qsizetype n) const;
    QString mid(qsizetype position, qsizetype n = -1) const;

    inline qsizetype capacity() const;
    inline void reserve(qsizetype size);
    inline void squeeze();

    QString repeated(qsizetype times) const;

    const ushort *utf16() const;

    void resize(qsizetype size);
    void resize(qsizetype size, QChar fillChar);
    QString &fill(QChar c, qsizetype size = -1);
    void truncate(qsizetype pos);
    void chop(qsizetype n);

    //utf8 -> QString(unicode)
    static QString fromUtf8(QByteArrayView utf8);
    Q_WEAK_OVERLOAD
    static inline QString fromUtf8(const QByteArray &ba) { return fromUtf8(QByteArrayView(ba)); }
    static inline QString fromUtf8(const char *utf8, qsizetype size)
    {
        return fromUtf8(QByteArrayView(utf8, !utf8 || size < 0 ? qstrlen(utf8) : size));
    }
    //latin -> QString
    static QString fromLatin1(QByteArrayView ba);
    Q_WEAK_OVERLOAD
    static inline QString fromLatin1(const QByteArray &ba) { return fromLatin1(QByteArrayView(ba)); }
    static inline QString fromLatin1(const char *str, qsizetype size)
    {
        return fromLatin1(QByteArrayView(str, !str|| size < 0 ? qstrlen(str) : size));
    }
    static QString fromRawData(const QChar *, qsizetype size);
    QString &setRawData(const QChar *, qsizetype size);
    QString &setUnicode(const QChar *, qsizetype size);

    //startsWith
    bool startsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QStringView s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept{ return QtPrivate::startsWith(*this, s, cs); }
    bool startsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    //endsWith
    bool endsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QStringView s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept { return QtPrivate::endsWith(*this, s, cs); }
    bool endsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    bool isUpper() const;
    bool isLower() const;

    QString leftJustified(qsizetype width, QChar fill = QLatin1Char(' '), bool trunc = false) const;
    QString rightJustified(qsizetype width, QChar fill = QLatin1Char(' '), bool trunc = false) const;

    //arg相关
    static QString vasprintf(const char *format, va_list ap) Q_ATTRIBUTE_FORMAT_PRINTF(1, 0);
    static QString asprintf(const char *format, ...) Q_ATTRIBUTE_FORMAT_PRINTF(1, 2);

    //indexOf
    qsizetype indexOf(QStringView s, qsizetype from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::findString(*this, from, s, cs); }
    qsizetype indexOf(QLatin1String s, qsizetype from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::findString(*this, from, s, cs); }
    qsizetype indexOf(QChar c, qsizetype from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::findString(*this, from, QStringView(&c, 1), cs); }
    qsizetype indexOf(const QString &s, qsizetype from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
    { return QtPrivate::findString(*this, from, QStringView(s), cs); }
    //contains
    inline bool contains(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return indexOf(s, 0, cs) != -1; }
    bool contains(QStringView s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return indexOf(s, 0, cs) != -1; }
    bool contains(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return indexOf(s, 0, cs) != -1; }
    inline bool contains(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return indexOf(QStringView(&c, 1), 0, cs) != -1; }
    //lastIndexOf
    qsizetype lastIndexOf(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
    { return lastIndexOf(s, size(), cs); }
    qsizetype lastIndexOf(const QString &s, qsizetype from, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
    { return QtPrivate::lastIndexOf(QStringView(*this), from, s, cs); }
    qsizetype lastIndexOf(QStringView s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return lastIndexOf(s, size(), cs); }
    qsizetype lastIndexOf(QStringView s, qsizetype from, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::lastIndexOf(*this, from, s, cs); }
    qsizetype lastIndexOf(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return lastIndexOf(s, size(), cs); }
    qsizetype lastIndexOf(QLatin1String s, qsizetype from, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::lastIndexOf(*this, from, s, cs); }
    qsizetype lastIndexOf(QChar c, qsizetype from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return QtPrivate::lastIndexOf(*this, from, QStringView(&c, 1), cs); }

    QString toLower() const & { return toLower_helper(*this); }
    QString toLower() && { return toLower_helper(*this); }
    QString toUpper() const & { return toUpper_helper(*this); }
    QString toUpper() && { return toUpper_helper(*this); }
    QString toCaseFolded() const & { return toCaseFolded_helper(*this); }
    QString toCaseFolded() && { return toCaseFolded_helper(*this); }
    QString trimmed() const & { return trimmed_helper(*this); }
    QString trimmed() && { return trimmed_helper(*this); }
    QString simplified() const & { return simplified_helper(*this); }
    QString simplified() && { return simplified_helper(*this); }

    //insert
    QString &insert(qsizetype i, QChar c);
    QString &insert(qsizetype i, const QChar *uc, qsizetype len);
    inline QString &insert(qsizetype i, const QString &s) { return insert(i, s.constData(), s.length()); }
    inline QString &insert(qsizetype i, QStringView v) { return insert(i, v.data(), v.length()); }
    QString &insert(qsizetype i, QLatin1String s);
    inline QString &insert(qsizetype i, const char *s) { return insert(i, QString::fromUtf8(s)); }
    inline QString &insert(qsizetype i, const QByteArray &s) { return insert(i, QString::fromUtf8(s)); }

    //append
    QString &append(QChar c);
    QString &append(const QChar *uc, qsizetype len);
    QString &append(const QString &s);
    inline QString &append(QStringView v) { return append(v.data(), v.length()); }
    QString &append(QLatin1String s);
    inline QString &append(const char *s) { return append(QString::fromUtf8(s)); }
    inline QString &append(const QByteArray &s) { return append(QString::fromUtf8(s)); }

    //prepend
    inline QString &prepend(QChar c) { return insert(0, c); }
    inline QString &prepend(const QChar *uc, qsizetype len) { return insert(0., uc, len); }
    inline QString &prepend(const QString &s) { return insert(0, s); }
    inline QString &prepend(QStringView v) { return prepend(v.data(), v.length()); }
    inline QString &prepend(QLatin1String s) { return insert(0, s); }
    inline QString &prepend(const char *s) { return prepend(QString::fromUtf8(s)); }
    inline QString &prepend(const QByteArray &s) { return prepend(QString::fromUtf8(s)); }

    //remove
    QString &remove(qsizetype i, qsizetype len);
    QString &remove(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &remove(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &remove(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    template <typename Predicate>
    QString &removeIf(Predicate pred) {
        QtPrivate::sequential_erase_if(*this, pred);
        return *this;
    }

    //replace
    QString &replace(qsizetype i, qsizetype len, QChar after);
    QString &replace(qsizetype i, qsizetype len, const QChar *s, qsizetype slen);
    QString &replace(qsizetype i, qsizetype len, const QString &after);
    QString &replace(QChar before, QChar after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QChar *before, qsizetype len, const QChar *after, qsizetype alen, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QLatin1String before, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QLatin1String before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QString &before, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QString &before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QChar c, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QChar c, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    //compare
    int compare(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    int compare(QLatin1String other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    int compare(QStringView s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept;
    inline int compare(QChar ch, Qt::CaseSensitivity cs = Qt::CaseSensitive) const noexcept
    { return compare(QStringView(&ch, 1), cs); }
    static inline int compare(const QString &s1, const QString &s2, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept
    { return s1.compare(s2, cs); }
    static inline int compare(const QString &s1, QLatin1String s2, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept
    { return s1.compare(s2); }
    static inline int compare(QLatin1String s1, const QString &s2, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept
    { return -s2.compare(s1, cs); }
    static inline int compare(const QString &s1, QStringView s2, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept
    { return s1.compare(s2, cs); }
    static inline int compare(QStringView s1, const QString &s2, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept
    { return -s2.compare(s1, cs); }

    void setNum(int i, int base = 10);
    static QString number(int, int base = 10);

//#if QT_CONFIG(regularexpression)
//    QString &replace(const QRegularExpression &re, const QString  &after);
//    inline QString &remove(const QRegularExpression &re)
//    { return replace(re, QString()); }
//#endif


#ifdef __cpp_char8_t
    Q_WEAK_OVERLOAD
    static inline QString fromUtf8(const char8_t *str)
    {
        return fromUtf8(reinterpret_cast<const char *>(str));
    }
    Q_WEAK_OVERLOAD
    static inline QString fromUtf8(const char8_t *str, qsizetype size)
    {
        return fromUtf8(reinterpret_cast<const char *>(str), size);
    }
#endif
    static QString fromUtf16(const char16_t *, qsizetype size = -1);
    static QString fromUcs4(const char32_t *, qsizetype size = -1);

    //std::string QString convert
    static inline QString fromStdString(const std::string &s);
    inline std::string toStdString() const;

    //QString => others
    QByteArray toLatin1() const & { return toLatin1_helper(*this); }
    QByteArray toLatin11() && { return toLatin1_helper_inplace(*this); }
    QByteArray toUtf8() const & { return toUtf8_helper(*this); }
    QByteArray toUtf8() && { return toUtf8_helper(*this); }
//    QByteArray toLocal8Bit() const &;
//    QByteArray toLocal8Bit() &&;

    float toFloat(bool *ok = nullptr) const;
    double toDouble(bool *ok = nullptr) const;

    // QString <> QString
    friend bool operator==(const QString &s1, const QString &s2) noexcept
    { return (s1.size() == s2.size()) && QtPrivate::compareStrings(s1, s2, Qt::CaseSensitive) == 0; }
    friend bool operator< (const QString &s1, const QString &s2) noexcept
    { return QtPrivate::compareStrings(s1, s2, Qt::CaseSensitive) < 0; }
    friend bool operator> (const QString &s1, const QString &s2) noexcept
    { return s2 < s1; }
    friend bool operator<=(const QString &s1, const QString &s2) noexcept
    { return !(s2 < s1); }
    friend bool operator>=(const QString &s1, const QString &s2) noexcept
    { return !(s1 < s2); }

    // QString <> char16_t*
    friend bool operator==(const QString &s1, const char16_t *s2) { return s1 == QStringView(s2); }
    friend bool operator< (const QString &s1, const char16_t *s2) { return s1 <  QStringView(s2); }
    friend bool operator!=(const QString &s1, const char16_t *s2) { return s1 != QStringView(s2); }
    friend bool operator> (const QString &s1, const char16_t *s2) { return s1 >  QStringView(s2); }
    friend bool operator<=(const QString &s1, const char16_t *s2) { return s1 <= QStringView(s2); }
    friend bool operator>=(const QString &s1, const char16_t *s2) { return s1 >= QStringView(s2); }
    // char16_t * <> QString
    friend bool operator==(const char16_t *s1, const QString &s2) { return s2 == s1; }
    friend bool operator!=(const char16_t *s1, const QString &s2) { return s2 != s1; }
    friend bool operator< (const char16_t *s1, const QString &s2) { return s2 >  s1; }
    friend bool operator> (const char16_t *s1, const QString &s2) { return s2 <  s1; }
    friend bool operator<=(const char16_t *s1, const QString &s2) { return s2 >= s1; }
    friend bool operator>=(const char16_t *s1, const QString &s2) { return s2 <= s1; }
    // QString <> QLatin1String
    friend bool operator==(const QString &s1, QLatin1String s2) noexcept { return (s1.size() == s2.size()) && QtPrivate::compareStrings(s1, s2, Qt::CaseSensitive) == 0; }
    friend bool operator< (const QString &s1, QLatin1String s2) noexcept { return QtPrivate::compareStrings(s1, s2, Qt::CaseSensitive) < 0; }
    friend bool operator> (const QString &s1, QLatin1String s2) noexcept { return QtPrivate::compareStrings(s1, s2, Qt::CaseSensitive) > 0; }
    friend bool operator!=(const QString &s1, QLatin1String s2) noexcept { return !(s1 == s2); }
    friend bool operator<=(const QString &s1, QLatin1String s2) noexcept { return !(s1 > s2); }
    friend bool operator>=(const QString &s1, QLatin1String s2) noexcept { return !(s1 < s2); }
    // QLatin1String <> QString
    friend bool operator==(QLatin1String s1, const QString &s2) noexcept { return s2 == s1; }
    friend bool operator< (QLatin1String s1, const QString &s2) noexcept { return s2 > s1; }
    friend bool operator> (QLatin1String s1, const QString &s2) noexcept { return s2 < s1; }
    friend bool operator!=(QLatin1String s1, const QString &s2) noexcept { return s2 != s1; }
    friend bool operator<=(QLatin1String s1, const QString &s2) noexcept { return s2 >= s1; }
    friend bool operator>=(QLatin1String s1, const QString &s2) noexcept { return s2 <= s1; }
    // QString <> nullptr
    friend bool operator==(const QString &s1, std::nullptr_t) noexcept { return s1.isEmpty(); }
    friend bool operator!=(const QString &s1, std::nullptr_t) noexcept { return !s1.isEmpty(); }
    friend bool operator< (const QString &  , std::nullptr_t) noexcept { return false; }
    friend bool operator> (const QString &s1, std::nullptr_t) noexcept { return !s1.isEmpty(); }
    friend bool operator<=(const QString &s1, std::nullptr_t) noexcept { return s1.isEmpty(); }
    friend bool operator>=(const QString &  , std::nullptr_t) noexcept { return true; }
    // nullptr <> QString
    friend bool operator==(std::nullptr_t, const QString &s2) noexcept { return s2 == nullptr; }
    friend bool operator!=(std::nullptr_t, const QString &s2) noexcept { return s2 != nullptr; }
    friend bool operator< (std::nullptr_t, const QString &s2) noexcept { return s2 >  nullptr; }
    friend bool operator> (std::nullptr_t, const QString &s2) noexcept { return s2 <  nullptr; }
    friend bool operator<=(std::nullptr_t, const QString &s2) noexcept { return s2 >= nullptr; }
    friend bool operator>=(std::nullptr_t, const QString &s2) noexcept { return s2 <= nullptr; }
    // QChar <> QString
    friend inline bool operator==(QChar lhs, const QString &rhs) noexcept { return rhs.size() == 1 && lhs == rhs.front(); }
    friend inline bool operator< (QChar lhs, const QString &rhs) noexcept { return compare_helper(&lhs, 1, rhs.data(), rhs.size()) < 0; }
    friend inline bool operator> (QChar lhs, const QString &rhs) noexcept { return compare_helper(&lhs, 1, rhs.data(), rhs.size()) > 0; }
    friend inline bool operator!=(QChar lhs, const QString &rhs) noexcept { return !(lhs == rhs); }
    friend inline bool operator<=(QChar lhs, const QString &rhs) noexcept { return !(lhs >  rhs); }
    friend inline bool operator>=(QChar lhs, const QString &rhs) noexcept { return !(lhs <  rhs); }
    // QString <> QChar
    friend inline bool operator==(const QString &lhs, QChar rhs) noexcept { return   rhs == lhs; }
    friend inline bool operator!=(const QString &lhs, QChar rhs) noexcept { return !(rhs == lhs); }
    friend inline bool operator< (const QString &lhs, QChar rhs) noexcept { return   rhs >  lhs; }
    friend inline bool operator> (const QString &lhs, QChar rhs) noexcept { return   rhs <  lhs; }
    friend inline bool operator<=(const QString &lhs, QChar rhs) noexcept { return !(rhs <  lhs); }
    friend inline bool operator>=(const QString &lhs, QChar rhs) noexcept { return !(rhs >  lhs); }


    //operator +=
    inline QString &operator+=(QChar c) { return append(c); }
    inline QString &operator+=(const QString &s) { return append(s); }
    inline QString &operator+=(QStringView v) { return append(v); }
    inline QString &operator+=(QLatin1String s) { return append(s); }
    inline QString &operator+=(const char *s) { return append(QString::fromUtf8(s)); }
    inline QString &operator+=(const QByteArray &s) { return append(QString::fromUtf8(s)); }

    typedef QChar *iterator;
    typedef const QChar *const_iterator;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    inline iterator begin();
    inline const_iterator begin() const;
    inline const_iterator cbegin() const;
    inline const_iterator constBegin() const;
    inline iterator end();
    inline const_iterator end() const;
    inline const_iterator cend() const;
    inline const_iterator constEnd() const;
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

public:
    inline DataPointer &data_ptr() { return d; }
    inline const DataPointer &data_ptr() const { return d; }

private:
    void reallocData(qsizetype alloc, QArrayData::AllocationOption option);
    void reallocGrowData(qsizetype n);

    static QByteArray toLatin1_helper(const QString &);
    static QByteArray toLatin1_helper_inplace(QString &);
    static QByteArray toUtf8_helper(const QString &);

    void replace_helper(size_t *indices, qsizetype nIndices, qsizetype blen, const QChar *after, qsizetype alen);

    static int compare_helper(const QChar *data1, qsizetype length1, const QChar *data2, qsizetype length2, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;
    static int compare_helper(const QChar *data1, qsizetype length1, const char *data2, qsizetype length2, Qt::CaseSensitivity cs = Qt::CaseSensitive) noexcept;

    static QString toLower_helper(const QString &str);
    static QString toLower_helper(QString &str);
    static QString toUpper_helper(const QString &str);
    static QString toUpper_helper(QString &str);
    static QString toCaseFolded_helper(const QString &str);
    static QString toCaseFolded_helper(QString &str);
    static QString trimmed_helper(const QString &str);
    static QString trimmed_helper(QString &str);
    static QString simplified_helper(const QString &str);
    static QString simplified_helper(QString &str);

private:
    static const char16_t _empty;
    DataPointer d;
};

//operator +
inline const QString operator+(const QString &s1, const QString &s2)
{ QString t(s1); t += s2; return t; }
inline const QString operator+(const QString &s1, QChar s2)
{ QString t(s1); t += s2; return t; }
inline const QString operator+(QChar s1, const QString &s2)
{ QString t(s1); t += s2; return t; }
inline const QString operator+(const QString &s1, const char *s2)
{ QString t = QString::fromUtf8(s2); t += s1; return t; }
inline const QString operator+(const char *s1, const QString &s2)
{ QString t = QString::fromUtf8(s1); t += s2; return t; }
inline const QString operator+(const QByteArray &ba, const QString &s)
{ QString t = QString::fromUtf8(ba); t += s; return t; }
inline const QString &operator+(const QString &s, const QByteArray &ba)
{ QString t = QString::fromUtf8(ba); t += s; return t; }

inline constexpr QString::QString() noexcept
{
}

inline QString::QString(QLatin1String latin1)
{
    *this = QString::fromLatin1(latin1.data(), latin1.size());
}

inline QString::QString(const QString &other) noexcept
    : d(other.d)
{
}

inline QString::~QString()
{
}

inline const QChar QString::at(qsizetype i) const
{
    Q_ASSERT(size_t(i) < size_t(size()));
    return QChar(d.data()[i]);
}

inline const QChar QString::operator[](qsizetype i) const
{
    Q_ASSERT(size_t(i) < size_t(size()));
    return QChar(d.data()[i]);
}

inline QChar &QString::operator[](qsizetype i)
{
    Q_ASSERT(size_t(i) < size_t(size()));
    return data()[i];   //data()中做了detach操作
}

inline const QChar *QString::unicode() const
{
    return data();
}

inline QChar *QString::data()
{
    detach();
    Q_ASSERT(d.data());
    return reinterpret_cast<QChar *>(d.data());
}

inline const QChar *QString::data() const
{
    return reinterpret_cast<const QChar *>(d.data());
}

inline const QChar *QString::constData() const
{
    return data();
}

inline std::string QString::toStdString() const
{
    return toUtf8().toStdString();
}

inline QString QString::fromStdString(const std::string &s)
{
    return fromUtf8(s.data(), int(s.size()));
}

inline void QString::detach()
{
    if (d->needsDetach()) {  //有可能本身就只被当前对象引用
        reallocData(d.size, QArrayData::KeepSize);
    }
}

inline bool QString::isDetached() const
{
    return !d->isShared();
}

inline void QString::clear()
{
    if (!isNull()) {
        *this = QString();
    }
}

inline qsizetype QString::capacity() const
{
    return qsizetype(d->constAllocatedCapacity());
}

inline void QString::reserve(qsizetype asize)
{
    if (d->needsDetach() || asize >= capacity() - d.freeSpaceAtBegin()) {
        reallocData(qMax(asize, size()), QArrayData::KeepSize);
    }
    if (d->constAllocatedCapacity()) {
        d->setFlag(Data::CapacityReserved);
    }
}

inline void QString::squeeze()
{
    if (!d.isMutable()) {
        return;
    }
    if (d->needsDetach() || size() < capacity()) {
        reallocData(d.size, QArrayData::KeepSize);
    }
    if (d->constAllocatedCapacity()) {
        d->clearFlag(Data::CapacityReserved);
    }
}

inline QString::iterator QString::begin() {
    detach();
    return reinterpret_cast<QChar *>(d.data());
}

inline QString::const_iterator QString::begin() const {
    return reinterpret_cast<const QChar *>(d.data());
}

inline QString::const_iterator QString::cbegin() const {
    return reinterpret_cast<const QChar *>(d.data());
}

inline QString::const_iterator QString::constBegin() const {
    return reinterpret_cast<const QChar *>(d.data());
}

inline QString::iterator QString::end() {
    detach();
    return reinterpret_cast<QChar *>(d.data() + d.size);
}

inline QString::const_iterator QString::end() const {
    return reinterpret_cast<const QChar *>(d.data() + d.size);
}

inline QString::const_iterator QString::cend() const {
    return reinterpret_cast<const QChar *>(d.data() + d.size);
}

inline QString::const_iterator QString::constEnd() const {
    return reinterpret_cast<const QChar *>(d.data() + d.size);
}


QT_END_NAMESPACE

#endif //QSTRING_H
