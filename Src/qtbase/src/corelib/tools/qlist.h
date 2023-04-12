//
// Created by Yujie Zhao on 2023/3/8.
//

#ifndef QLIST_H
#define QLIST_H

#include <QtCore/qglobal.h>
#include "qarraydata.h"
#include "qarraydataops.h"
#include "qarraydatapointer.h"

QT_BEGIN_NAMESPACE

template <typename T> class QList;

namespace QtPrivate {
    template <typename V, typename U> qsizetype indexOf(const QList<V> &list, const U &u, qsizetype from) noexcept;
    template <typename V, typename U> qsizetype lastIndexOf(const QList<V> &list, const U &u, qsizetype from) noexcept;
}

template <typename T> struct QListSpecialMethodsBase
{
protected:
    ~QListSpecialMethodsBase() = default;
    using Self = QList<T>;
    Self *self() { return static_cast<Self *>(this); }
    const Self *self() const { return static_cast<const Self *>(this); }

public:
    template <typename AT = T>
    qsizetype indexOf(const AT &t, qsizetype from = 0) const noexcept;
    template <typename AT = T>
    qsizetype lastIndexOf(const AT &t, qsizetype from = -1) const noexcept;

    template <typename AT = T>
    bool contains(const AT &t) const noexcept
    {
       return self()->indexOf(t) != -1;
    }
};

template <typename T>
struct QListSpecialMethods : QListSpecialMethodsBase<T>
{
protected:
    ~QListSpecialMethods() = default;
public:
    using QListSpecialMethodsBase<T>::indexOf;
    using QListSpecialMethodsBase<T>::lastIndexOf;
    using QListSpecialMethodsBase<T>::contains;
};

//template <typename T>
//class QVector : public QList<T> {
//};

template <typename T>
class QList : public QListSpecialMethods<T> {
    using Data = QTypedArrayData<T>;
    using DataOps = QArrayDataOps<T>;
    using DataPointer = QArrayDataPointer<T>;
    class DisableRValueRefs {};

    DataPointer d;

public:
    using Type = T;
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using size_type = qsizetype;
    using difference_type = qptrdiff;
    //pod类型为T，否则为 const T &
    using parameter_type = typename DataPointer::parameter_type;
    //如果是基础类型，直接使用const T&传递，可以不使用T &&传递
    using rvalue_ref = typename std::conditional<DataPointer::pass_parameter_by_value, DisableRValueRefs, T&&>::type;

    //迭代器
    class iterator {
        T *i = nullptr;
    public:
        using difference_type = qsizetype;
        using value_type = T;
        //zhaoyujie TODO
//#if __cplusplus >= 202002L && (!defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 11)
//        using iterator_category = std::contiguous_iterator_tag;
//        using element_type = value_type;
//#else
//        using iterator_category = std::random_access_iterator_tag;
//#endif
        using iterator_category = std::random_access_iterator_tag;
        using pointer = T *;
        using reference = T &;

        inline constexpr iterator() = default;
        inline iterator(T *n) : i(n) {}
        inline T &operator*() const { return *i; }
        inline T *operator->() const { return i; }
        inline T &operator[](qsizetype j) { return *(i + j); }
        inline constexpr bool operator==(iterator o) const { return i == o.i; }
        inline constexpr bool operator!=(iterator o) const { return i != o.i; }
        inline constexpr bool operator< (iterator other) const { return i < other.i; }
        inline constexpr bool operator<=(iterator other) const { return i <= other.i; }
        inline constexpr bool operator> (iterator other) const { return i > other.i; }
        inline constexpr bool operator>=(iterator other) const { return i >= other.i; }
        inline constexpr bool operator==(pointer p) const { return i == p; }
        inline constexpr bool operator!=(pointer p) const { return i != p; }
        //前置++操作，效率更高，返回引用
        inline iterator &operator++() { ++i; return *this; }
        //后置++操作，效率较低，返回拷贝
        inline iterator operator++(int) { T *n = i; ++i; return n; }
        inline iterator &operator--() { --i; return *this; }
        inline iterator operator--(int) { T *n = i; --i; return n; }
        inline iterator &operator+=(qsizetype j) { i += j; return *this; }
        inline iterator &operator-=(qsizetype j) { i -= j; return *this; }
        //左+，右+
        inline iterator operator+ (qsizetype j) const { return iterator(i + j); }
        friend inline iterator operator+(qsizetype j, iterator k) { return k + j; }
        inline iterator operator- (qsizetype j) const { return iterator(i - j); }
        inline qsizetype operator- (iterator j) const { return i - j.i; }
        //(T *)i 强转
        inline operator T*() const { return i; }
    };

    class const_iterator {
        const T *i = nullptr;
    public:
        using difference_type = qsizetype;
        using value_type = T;
        //#if __cplusplus >= 202002L && (!defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 11)
//        using iterator_category = std::contiguous_iterator_tag;
//        using element_type = value_type;
//#else
//        using iterator_category = std::random_access_iterator_tag;
//#endif
        using iterator_category = std::random_access_iterator_tag;
        using pointer = const T *;
        using reference = const T &;

        inline constexpr const_iterator() = default;
        inline const_iterator(const T *n) : i(n) {}
        inline constexpr const_iterator(iterator o) : i(o) {}
        inline const T &operator*() const { return *i; }
        inline const T *operator->() const { return i; }
        inline const T &operator[](qsizetype j) const { return *(i + j); }
        inline constexpr bool operator==(const_iterator o) const { return i == o.i; }
        inline constexpr bool operator!=(const_iterator o) const { return i != o.i; }
        inline constexpr bool operator< (const_iterator o) const { return i < o.i; }
        inline constexpr bool operator<=(const_iterator o) const { return i <= o.i; }
        inline constexpr bool operator> (const_iterator o) const { return i > o.i; }
        inline constexpr bool operator>=(iterator o) const { return i >= o.i; }
        inline constexpr bool operator==(iterator o) const { return i == o.i; }
        inline constexpr bool operator!=(iterator o) const { return i != o.i; }
        inline constexpr bool operator==(T *p) const { return i == p; }
        inline constexpr bool operator!=(T *p) const { return i != p; }
        inline const_iterator &operator++() { ++i; return *this; }
        inline const_iterator operator++(int) { const T *n = i; ++i; return n; }
        inline const_iterator &operator--() {--i; return *this; }
        inline const_iterator operator--(int) { const T *n = i; --i; return n; }
        inline const_iterator &operator+=(qsizetype n) { i +=n; return *this; }
        inline const_iterator &operator-=(qsizetype n) { i-=n; return *this; }
        inline const_iterator operator+(qsizetype n) const { return const_iterator(i + n); }
        inline const_iterator operator-(qsizetype n) const { return const_iterator(i - n); }
        friend inline constexpr const_iterator operator+(qsizetype n, const_iterator j) { return n + j; }
        inline qsizetype operator-(const_iterator j) const { return i - j.i; }
        //(const T *)i 强转
        inline operator const T *() const { return i; }
    };

    using Iterator = iterator;
    using ConstIterator = const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    QList() = default;

    QList(DataPointer dd) noexcept
        : d(dd)
    {

    }

    explicit QList(qsizetype size)
        : d (Data::allocate(size)) {
        if (size) {
            d->appendInitialize(size);
        }
    }
    QList(qsizetype size, parameter_type t)
        : d(Data::allocate(size)) {
        if (size) {
            d->copyAppend(size, t);
        }
    }

    inline QList(std::initializer_list<T> args)
        : d(Data::allocate(args.size()))
    {
        if (args.size()) {
            d->copyAppend(args.begin(), args.end());
        }
    }

    template <typename InputIterator, QtPrivate::IfIsInputIterator<InputIterator> = true>
    QList(InputIterator i1, InputIterator i2)
    {
        if constexpr (!std::is_convertible_v<typename std::iterator_traits<Iterator>::iterator_category, std::forward_iterator_tag>){
            Q_ASSERT(false);
            std::copy(i1, i2, std::back_inserter(*this));
        }
        else {
            const auto distance = std::distance(i1, i2);
            if (distance) {
                d = DataPointer(Data::allocate(distance));
                if constexpr (std::is_same_v<std::decay_t<InputIterator>, iterator> ||
                              std::is_same_v<std::decay_t<InputIterator>, const_iterator>) {
                    d->copyAppend(i1, i2);
                } else {
                    d->appendIteratorRange(i1, i2);
                }
            }
        }
    }

    QList<T> &operator=(std::initializer_list<T> args)
    {
        d = DataPointer(Data::allocate(args.size()));
        if (args.size()) {
            d->copyAppend(args.begin(), args.end());
        }
        return *this;
    }

    qsizetype size() const noexcept { return d->size; }
    qsizetype count() const noexcept { return size(); }
    qsizetype length() const noexcept { return size(); }
    qsizetype capacity() const noexcept { return qsizetype(d.constAllocatedCapacity()); }
    void reserve(qsizetype size);

    void append(parameter_type t) { emplaceBack(t); }
    void append(rvalue_ref t)
    {
        if constexpr (DataPointer::pass_parameter_by_value) {
            Q_ASSERT(false);
            Q_UNUSED(t)
        } else {
            emplaceBack(std::move(t));
        }
    }
    void append(const_iterator i1, const_iterator i2);
    void append(const QList<T> &l) { append(l.constBegin(), l.constEnd()); }
    void append(QList<T> &&l);

    void prepend(rvalue_ref t)
    {
        if constexpr(DataPointer::pass_parameter_by_value) {
            Q_ASSERT(false);
            Q_UNLIKELY(t);
        }
        else {
            emplaceFront(std::move(t));
        }
    }
    void prepend(parameter_type t) { emplaceFront(t); }

    //在末尾插入,注意这里使用了模板Args，不是只可以接受T类型
    template <typename... Args>
    inline reference emplaceBack(Args &&... args);

    template <typename ...Args>
    inline reference emplaceFront(Args &&... args);

    iterator insert(qsizetype i, parameter_type t)
    { return emplace(i, t); }
    iterator insert(const_iterator before, parameter_type t)
    {
        Q_ASSERT(isValidIterator(before));
        return insert(before, 1, t);
    }
    iterator insert(const_iterator before, qsizetype n, parameter_type t)
    {
        Q_ASSERT(isValidIterator(before));
        return insert(std::distance(constBegin(), before), n, t);
    }
    iterator insert(const_iterator before, rvalue_ref t)
    {
        Q_ASSERT(isValidIterator(before));
        return insert(std::distance(constBegin(), before), std::move(t));
    }
    iterator insert(qsizetype i, rvalue_ref t)
    {
        if constexpr (DataPointer::pass_parameter_by_value) {
            Q_UNUSED(i);
            Q_UNUSED(t);
            return end();
        }
        else {
            return emplace(i, std::move(t));
        }
    }
    iterator insert(qsizetype i, qsizetype n, parameter_type t) {
        Q_ASSERT(size_t(i) <= size_t(d->size));
        Q_ASSERT(n >= 0);
        if (Q_LIKELY(n)) {
            d->insert(i, n, t);
        }
        return d.begin() + i;
    }

    template <typename ...Args>
    iterator emplace(const_iterator before, Args&&... args)
    {
        Q_ASSERT(isValidIterator(before));
        return emplace(std::distance(constBegin(), before), std::forward<Args>(args)...);
    }
    template <typename ...Args>
    iterator emplace(qsizetype i, Args&& ... args)
    {
        Q_ASSERT(i >= 0 && i <= d->size);
        d->emplace(i, std::forward<Args>(args)...);
        return d.begin() + i;
    }

    void replace(qsizetype i, parameter_type t)
    {
        Q_ASSERT(i >= 0 && i < d->size);
        DataPointer oldData;
        d.detach(&oldData);
        d.data()[i] = t;
    }

    void replace(qsizetype i, rvalue_ref t)
    {
        if constexpr (DataPointer::pass_parameter_by_value) {
            Q_UNUSED(i);
            Q_UNUSED(t);
        } else {
            Q_ASSERT(i >= 0 && i < d->size);
            DataPointer oldData;
            d.detach(&oldData);
            d.data()[i] = std::move(t);
        }
    }

    void clear() {
        if (!size()) {
            return;
        }
        if (d.needsDetach()) {
            //直接分配空的swap下
            DataPointer detached(Data::allocate(d.allocatedCapacity()));
            d.swap(detached);
        }
        else {
            d->truncate(0);
        }
    }

    void swapItemsAt(qsizetype i, qsizetype j) {
        Q_ASSERT(i >= 0 && i < size() && j >= 0 && j < size());
        detach();
        qSwap(d->begin()[i], d->begin()[j]);
    }

    inline void push_back(parameter_type t) { append(t); }
    void push_back(rvalue_ref t) { append(std::move(t)); }
    inline void push_front(parameter_type t) { prepend(t); }
    void push_front(rvalue_ref t) { prepend(std::move(t)); }
    void pop_back() noexcept { removeLast(); }
    void pop_front() noexcept { removeFirst(); }

    iterator erase(const_iterator begin, const_iterator end);
    inline iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

    QList<T> mid(qsizetype pos, qsizetype len = -1) const;

    inline T &first()  { Q_ASSERT(!isEmpty()); return *begin(); }
    inline const T &first() const noexcept { Q_ASSERT(!isEmpty()); return *begin(); }
    inline const T &constFirst() const noexcept { Q_ASSERT(!isEmpty()); return *begin(); }
    inline T &last() { Q_ASSERT(!isEmpty()); return *(end() - 1); }
    inline const T &last() const noexcept { Q_ASSERT(!isEmpty()); return *(end() - 1); }
    inline const T &constLast() const noexcept { Q_ASSERT(!isEmpty()); return *(end() - 1); }
    QList<T> first(qsizetype n) const {
        Q_ASSERT(size_t(n) <= size_t(size()));
        return QList<T>(begin(), begin() + n);
    }
    QList<T> last(qsizetype n) const {
        Q_ASSERT(size_t(n) <= size_t(size()));
        return QList<T>(end() - n, end());
    }
    QList<T> sliced(qsizetype pos) const {
        Q_ASSERT(size_t(pos) <= size_t(size()));
        return QList<T>(begin() + pos, end());
    }
    QList<T> sliced(qsizetype pos, qsizetype n) const {
        Q_ASSERT(size_t(pos) <= size_t(size()));
        Q_ASSERT(n >= 0);
        Q_ASSERT(pos + n <= size());
        return QList<T>(begin() + pos, begin() + pos + n);
    }

    T takeAt(qsizetype i) { T t = std::move(*this)[i]; remove(i); return t; }

    void move(qsizetype from, qsizetype to)
    {
        Q_ASSERT(from >= 0 && from < size());
        Q_ASSERT(to >= 0 && to < size());
        if (from == to) {
            return;
        }
        detach();
        T *const b = d->begin();
        if (from < to) {
            std::rotate(b + from, b + from + 1, b + to + 1);
        }
        else {
            std::rotate(b + to, b + from, b + from + 1);
        }
    }

    inline bool startsWith(parameter_type t) const { return !isEmpty() && first() == t; }
    inline bool endsWith(parameter_type t) const { return !isEmpty() && last() == t; }

    const_reference at(qsizetype i) const noexcept
    {
        Q_ASSERT(size_t(i) < size_t(d.size));
        return data()[i];
    }

    reference operator[](qsizetype i)
    {
        Q_ASSERT(size_t(i) < size_t(d.size));
        detach();  //返回引用，需要先分离
        return data()[i];
    }
    const_reference operator[](qsizetype i) const noexcept { return at(i); }

    T value(qsizetype i) const { return value(i, T()); }
    T value(qsizetype i, parameter_type defaultValue) const {
        return size_t(i) < size_t(d->size) ? at(i) : defaultValue;
    }

    inline reference front() { return first(); }
    inline const_reference  front() const noexcept { return first(); }
    inline reference back() { return last(); }
    inline const_reference back() const noexcept { return last(); }
    void shrink_to_fit() { squeeze(); }

    void remove(qsizetype i, qsizetype n = 1);
    void removeFirst() noexcept;
    void removeLast() noexcept;
    void removeAt(qsizetype i) { remove(i); }
    template <typename AT = T>
    qsizetype removeAll(const AT &t)
    {
        return QtPrivate::sequential_erase_with_copy(*this, t);
    }
    template <typename AT = T>
    bool removeOne(const AT &t)
    {
        return QtPrivate::sequential_erase_one(*this, t);
    }

    template <typename Predicate>
    qsizetype removeIf(Predicate pred)
    {
        return QtPrivate::sequential_erase_if(*this, pred);
    }

    value_type takeFirst() { Q_ASSERT(!isEmpty()); value_type v = std::move(first()); d->eraseFirst(); return v; }
    value_type takeLast() { Q_ASSERT(!isEmpty()); value_type v = std::move(last()); d->eraseLast(); return v; }

    //写时拷贝
    void detach() { d.detach(); }
    bool isDetached() const noexcept { return !d->isShared(); }
    bool isSharedWith(const QList<T> &other) const { return d == other.d; }

    pointer data() { detach(); return d->data(); }  //调用者可以修改data，需要先分离
    const_pointer data() const noexcept { return d.data(); }
    const_pointer constData() const { return d.data(); }

    bool isEmpty() const { return d->size == 0; }

    void resize(qsizetype size)
    {
        resize_internal(size);
        if (size > this->size()) {
            d->appendInitialize(size);
        }
    }

    void resize(qsizetype size, parameter_type c) {
        resize_internal(size);
        if (size > this->size()) {
            d->copyAppend(size - this->size(), c);
        }
    }

    static QList<T> fromList(const QList<T> &list) noexcept { return list; }
    QList<T> toList() const noexcept { return *this; }
    static QList<T> fromVector(const QList<T> &vector) noexcept { return vector; }
    QList<T> toVector() const noexcept { return *this; }
    template <qsizetype N>
    static QList<T> fromReadOnlyData(const T (&t)[N]) noexcept
    {
        return QList<T>({nullptr, const_cast<T *>(t), N });
    }

    iterator begin() { detach(); return d->begin(); }
    iterator end() { detach(); return d->end(); }
    const_iterator begin() const noexcept { return d->constBegin(); }
    const_iterator end() const noexcept { return d->constEnd(); }
    const_iterator cbegin() const noexcept { return d->constBegin(); }
    const_iterator cend() const noexcept { return d->constEnd(); }
    const_iterator constBegin() const noexcept { return d->constBegin(); }
    const_iterator constEnd() const noexcept { return d->constEnd(); }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    void swap(QList<T> &other) noexcept { qSwap(d, other.d); }
    inline void squeeze();

    template <typename AT = T>
    qsizetype count(const AT &t) const noexcept
    {
        return qsizetype(std::count(data(), data() + d.size, t));
    }

    inline bool empty() const noexcept { return d->size == 0; }

    inline QList<T> &fill(parameter_type t, qsizetype newSize = -1);

    template <typename U = T>
    QTypeTraits::compare_eq_result_container<QList, U> operator==(const QList &other) const
    {
        if (size() != other.size()) {
            return false;
        }
        if (begin() == other.begin()) {
            return true;
        }
        return d->compare(begin(), other.begin(), size());
    }

    template <typename U = T>
    QTypeTraits::compare_eq_result_container<QList, U> operator !=(const QList &other) const
    { return !(*this == other); }

    template <typename U = T>
    QTypeTraits::compare_lt_result_container<QList, U> operator<(const QList &other) const noexcept
    {
        return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }
    template <typename U = T>
    QTypeTraits::compare_lt_result_container<QList, U> operator>(const QList &other) const noexcept
    {
        return other < *this;
    }
    template <typename U = T>
    QTypeTraits::compare_lt_result_container<QList, U> operator<=(const QList &other) const noexcept
    {
        return !(other < *this);
    }
    template <typename U = T>
    QTypeTraits::compare_lt_result_container<QList, U> operator>=(const QList &other) const noexcept
    {
        return !(*this < other);
    }

    QList<T> &operator+=(const QList<T> &l) { append(l); return *this; }
    QList<T> &operator+=(QList<T> &&l) { append(std::move(l)); return *this; }
    inline QList<T> operator+(const QList<T> &l) const
    { QList n = *this; n += l; return n; }
    inline QList<T> operator+(QList<T> &&l) const
    { QList n = *this; n += std::move(l); return n; }
    inline QList<T> operator+(parameter_type t)
    { append(t); return *this; }

    inline QList<T> &operator<<(parameter_type t)
    { append(t); return *this; }
    inline QList<T> &operator<<(const QList<T> &l)
    { *this += l; return *this; }
    inline QList<T> &operator<<(QList<T> &&l)
    { *this += std::move(l); return *this; }

    inline QList<T> &operator+=(rvalue_ref t)
    { append(std::move(t)); return *this; }
    inline QList<T> &operator<<(rvalue_ref t)
    { append(std::move(t)); return *this; }

protected:
    inline void resize_internal(qsizetype newSize);
    bool isValidIterator(const_iterator i) const
    {
        const std::less<const T *> less = {};
        return !less(d->end(), i) && !less(i, d->begin());
    }
};

//zhaoyujie TODO 原来是这么写的，这种函数
template <typename T>
template <typename... Args>
inline typename QList<T>::reference QList<T>::emplaceBack(Args &&...args) {
    d->emplace(d->size, std::forward<Args>(args)...);
    //end是data() + size
    return *(d.end() - 1);
}

template <typename T>
template <typename... Args>
inline typename QList<T>::reference QList<T>::emplaceFront(Args &&...args) {
    d->emplace(0, std::forward<Args>(args)...);
    return *d.begin();
}

template <typename T>
void QList<T>::reserve(qsizetype asize)
{
    if (asize <= capacity() - d.freeSpaceAtBegin()) {
        if (d.flags() & Data::CapacityReserved) {
            return;
        }
        if (!d.isShared()) {
            d.setFlag(Data::CapacityReserved);
            return;
        }
    }

    DataPointer detached(Data::allocate(qMax(asize, size())));
    detached->copyAppend(constBegin(), constEnd());
    if (detached.d_ptr()) {
        detached.setFlag(Data::CapacityReserved);
    }
    d.swap(detached);
}

template <typename T>
inline void QList<T>::append(const_iterator i1, const_iterator i2)
{
    d->growAppend(i1, i2);
}

template <typename T>
inline void QList<T>::append(QList<T> &&other)
{
    Q_ASSERT(&other != this);
    if (other.isEmpty()) {
        return;
    }
    if (other.d.needsDetach() || !std::is_nothrow_move_constructible_v<T>) {
        return append(other);
    }
    d.detachAndGrow(QArrayData::GrowsAtEnd, other.size(), nullptr, nullptr);
    Q_ASSERT(d.freeSpaceAtEnd() >= other.size());
    d->moveAppend(other.begin(), other.end());
}

template <typename T>
inline void QList<T>::resize_internal(qsizetype newSize) {
    Q_ASSERT(newSize >= 0);

    if (d.needsDetach() || newSize > capacity() - d.freeSpaceAtBegin()) {
        d.detachAndGrow(QArrayData::GrowsAtEnd, newSize - d.size, nullptr, nullptr);
    }
    else if (newSize < size()) {
        d->truncate(newSize);
    }
}

template <typename T>
inline void QList<T>::squeeze()
{
    if (!d.isMutable()) {
        return;
    }
    if (d.needsDetach() || size() < capacity()) {
        DataPointer detached(Data::allocate(size()));
        if (size()) {
            if (d.needsDetach()) {
                detached->copyAppend(constBegin(), constEnd());
            }
            else {
                detached->moveAppend(d.data(), d.data() + d.size);
            }
        }
        d.swap(detached);
    }
    d->clearFlag(Data::CapacityReserved);
}

template <typename T>
inline void QList<T>::remove(qsizetype i, qsizetype n)
{
    Q_ASSERT(size_t(i) + size_t(n) <= size_t(d->size));
    Q_ASSERT(n >= 0);

    if (n == 0) {
        return;
    }
    d.detach();
    d->erase(d->begin() + i, n);
}

template <typename T>
inline void QList<T>::removeFirst() noexcept {
    Q_ASSERT(!isEmpty());
    d.detach();
    d->eraseFirst();
}

template <typename T>
inline void QList<T>::removeLast() noexcept {
    Q_ASSERT(!isEmpty());
    d.detach();
    d->eraseLast();
}

template <typename T>
typename QList<T>::iterator QList<T>::erase(const_iterator abegin, const_iterator aend)
{
    Q_ASSERT(isValidIterator(abegin));
    Q_ASSERT(isValidIterator(aend));
    Q_ASSERT(aend >= abegin);

    qsizetype i = std::distance(constBegin(), abegin);
    qsizetype n = std::distance(abegin, aend);
    remove(i, n);
    return d.begin() + i;
}

template <typename T>
inline QList<T> &QList<T>::fill(parameter_type t, qsizetype newSize)
{
    if (newSize == -1) {
        newSize = size();
    }
    if (d.needsDetach() || newSize > capacity()) {
        DataPointer detached(Data::allocate(d->detachCapacity(newSize)));
        detached->copyAppend(newSize, t);
        d.swap(detached);
    }
    else {
        d->assign(d.begin(), d.begin() + qMin(size(), newSize), t);
        if (newSize > size()) {
            d->copyAppend(newSize - size(), t);
        }
        else if (newSize < size()) {
            d->truncate(newSize);
        }
    }
    return *this;
}

namespace QtPrivate {
    template<typename T, typename U>
    qsizetype indexOf(const QList<T> &vector, const U &u, qsizetype from) noexcept {
        if (from < 0) {
            from = qMax(from + vector.size(), qsizetype(0));
        }
        if (from < vector.size()) {
            auto n = vector.begin() + from - 1;
            auto e = vector.end();
            while (++n != e) {
                if (*n == u) {
                    return qsizetype(n - vector.begin());
                }
            }
        }
        return -1;
    }

    template<typename T, typename U>
    qsizetype lastIndexOf(const QList<T> &vector, const U &u, qsizetype from) noexcept {
        if (from < 0) {
            from += vector.size();
        } else if (from >= vector.size()) {
            from = vector.size() - 1;
        }
        if (from >= 0) {
            auto b = vector.begin();
            auto n = vector.begin() + from + 1;
            while (n != b) {
                if (*--n == u) {
                    return qsizetype(n - b);
                }
            }
        }
        return -1;
    }
}

template <typename T>
template <typename AT>
qsizetype QListSpecialMethodsBase<T>::indexOf(const AT &t, qsizetype from) const noexcept {
    return QtPrivate::indexOf(*self(), t, from);
}

template <typename T>
template <typename AT>
qsizetype QListSpecialMethodsBase<T>::lastIndexOf(const AT &t, qsizetype from) const noexcept
{
    return QtPrivate::lastIndexOf(*self(), t, from);
}

template <typename T>
inline QList<T> QList<T>::mid(qsizetype pos, qsizetype len) const
{
    qsizetype p = pos;
    qsizetype l = len;
    using namespace QtPrivate;
    switch(QContainerImplHelper::mid(d.size, &p, &l)) {
        case QContainerImplHelper::Null:
        case QContainerImplHelper::Empty:
            return QList();
        case QContainerImplHelper::Full:
            return *this;
        case QContainerImplHelper::Subset:
            break;
    }
    DataPointer copied(Data::allocate(l));
    copied->copyAppend(constBegin() + p, constBegin() + p + l);
    return *(QList<T>*)&copied;
}


QT_END_NAMESPACE

#endif //QLIST_H
