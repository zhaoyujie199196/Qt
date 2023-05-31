//
// Created by Yujie Zhao on 2023/5/23.
//

#ifndef QVARLENGTHARRAY_H
#define QVARLENGTHARRAY_H

#include <QtCore/qglobal.h>
#include <QtCore/qtypeinfo.h>
#include "qcontainerfwd.h"

QT_BEGIN_NAMESPACE

// Prealloc默认为256，在qcontainerfwd.h文件中前置声明了
template <class T, qsizetype Prealloc>
class QVarLengthArray
{
    static_assert(std::is_nothrow_destructible_v<T>, "Types with throwing destructors are not supported in Qt containers.");

public:
    typedef qsizetype size_type;
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef qptrdiff difference_type;

    typedef T *iterator;
    typedef const T *const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
    QVarLengthArray() : QVarLengthArray(0) {}
    inline explicit QVarLengthArray(qsizetype size);

    inline QVarLengthArray(const QVarLengthArray<T, Prealloc> &other)
        : m_alloc(Prealloc), m_size(0), m_ptr(reinterpret_cast<T *>(m_array))
    {
        append(other.constData(), other.size());
    }

    QVarLengthArray(QVarLengthArray &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_alloc(other.m_alloc)
        , m_size(other.m_size)
        , m_ptr(other.m_ptr)
    {
        const auto otherInlineStorage = reinterpret_cast<T *>(other.m_array);
        if (m_ptr == otherInlineStorage) {  //other的数据存储在数组中，需要拷贝数组数据
            m_ptr = reinterpret_cast<T *>(m_array);
            QtPrivate::q_uninitialized_relocate_n(otherInlineStorage, m_size, m_ptr);
        }
        else {
            //没有存储在数组中，不需要处理
        }
        other.m_alloc = Prealloc;
        other.m_size = 0;
        other.m_ptr = otherInlineStorage;
    }

    QVarLengthArray(std::initializer_list<T> args)
        : QVarLengthArray(args.begin(), args.end())
    {}

    template <typename InputIterator, QtPrivate::IfIsInputIterator<InputIterator> = true>
    inline QVarLengthArray(InputIterator first, InputIterator last)
        : QVarLengthArray()
    {
        //如果是Forward迭代器，可以提供重复读操作，而Input迭代器只能读取一次。这里使用Forward先resize提高效率
        // https://blog.51cto.com/u_6220803/3198640
        QtPrivate::reserveIfForwardIterator(this, first, last);
        std::copy(first, last, std::back_inserter(*this));
    }

    ~QVarLengthArray() {
        if constexpr (QTypeInfo<T>::isComplex) {
            std::destroy_n(m_ptr, m_size);  //调用析构函数
        }
        if (m_ptr != reinterpret_cast<T *>(m_array)) {  //释放内存
            free(m_ptr);
        }
    }

    //拷贝  TODO 为什么拷贝需要写QVarLengthArray<T, Prealloc>，而移动不需要？
    inline QVarLengthArray<T, Prealloc> &operator=(const QVarLengthArray<T, Prealloc> &other)
    {
        if (this != &other) {
            clear();
            append(other.constData(), other.size());
        }
        return *this;
    }

    //移动
    QVarLengthArray &operator=(QVarLengthArray &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        clear();  //clear会释放自身内存
        Q_ASSERT(capacity() >= Prealloc);
        const auto otherInlineStorage = reinterpret_cast<T *>(other.m_array);
        if (other.m_ptr != otherInlineStorage) {  //如果不是内部数组的数据，直接交换就行，原内存的释放交给other进行
            m_alloc = std::exchange(other.m_alloc, Prealloc);
            m_ptr = std::exchange(other.m_ptr, otherInlineStorage);
        }
        else {
            QtPrivate::q_uninitialized_relocate_n(other.m_ptr, other.m_size, m_ptr);
        }
        m_size = std::exchange(other.m_size, 0);
        return *this;
    }

    QVarLengthArray<T, Prealloc> &operator=(std::initializer_list<T> list)
    {
        resize(qsizetype(list.size()));
        std::copy(list.begin(), list.end(), QT_MAKE_CHECKED_ARRAY_ITERATOR(this->begin(), this->size()));
        return *this;
    }

    inline qsizetype size() const { return m_size; }
    inline qsizetype count() const { return m_size; }
    inline qsizetype length() const { return m_size; }

    inline T &first()
    { Q_ASSERT(!isEmpty()); return *begin(); }
    inline const T &first() const
    { Q_ASSERT(!isEmpty()); return *begin(); }
    T &last()
    { Q_ASSERT(!isEmpty()); return *(end() - 1); }
    const T &last() const
    { Q_ASSERT(!isEmpty()); return *(end() - 1); }

    inline bool isEmpty() const { return m_size == 0; }
    inline void resize(qsizetype size);
    inline void clear() { resize(0); }
    inline void squeeze();

    inline qsizetype capacity() const { return m_alloc; }
    inline void reserve(qsizetype size);

    template <typename AT = T>
    inline qsizetype indexOf(const AT &t, qsizetype from = 0) const;
    template <typename AT = T>
    inline qsizetype lastIndexOf(const AT &t, qsizetype from = -1) const;
    template <typename AT = T>
    inline bool contains(const AT &t) const;

    inline T &operator[](qsizetype idx) {
        Q_ASSERT(idx >= 0 && idx < m_size);
        return m_ptr[idx];
    }
    inline const T &operator[](qsizetype idx) const {
        Q_ASSERT(idx >= 0 && idx < m_size);
        return m_ptr[idx];
    }

    inline const T &at(qsizetype idx) const { return operator[](idx); }

    T value(qsizetype i) const;
    T value(qsizetype i, const T &defaultValue) const;

    //支持append左值，右值，指针数组
    inline void append(const T &t)
    {
        if (m_size == m_alloc) {
            //这里需要拷贝一份，有可能在reallocate时被释放掉了
            //但是可以判断范围，提升效率。大多数情况下，这份拷贝不是必须的
            T copy(t);
            reallocate(m_size, m_size << 1);
            const qsizetype idx = m_size++;
            new(m_ptr + idx) T(std::move(copy));
        }
        else {
            const qsizetype idx = m_size++;
            new (m_ptr + idx) T(t);
        }
    }

    void append(T &&t) {
        if (m_size == m_alloc) {
            reallocate(m_size, m_size << 1);
        }
        const qsizetype idx = m_size++;
        new (m_ptr + idx) T(std::move(t));  //t需要使用std::move保持右值属性
    }

    //append指针数组
    void append(const T *buf, qsizetype size);
    inline QVarLengthArray<T, Prealloc> &operator<<(const T &t)
    { append(t); return *this; }
    inline QVarLengthArray<T, Prealloc> &operator<<(T &&t)
    { append(std::move(t)); return *this; }
    inline QVarLengthArray<T, Prealloc> &operator+=(const T &t)
    { append(t); return *this; }
    inline QVarLengthArray<T, Prealloc> &operator+=(T &&t)
    { append(std::move(t)); return *this; }

    void prepend(T &&t);
    void prepend(const T &t);
    void insert(qsizetype i, T &&t);
    void insert(qsizetype i, const T &t);
    void insert(qsizetype i, qsizetype n, const T &t);
    void replace(qsizetype i, const T &t);
    void remove(qsizetype i);
    void remove(qsizetype i, qsizetype n);
    void removeLast();
    template <typename AT = T>
    qsizetype removeAll(const AT &t);
    template <typename AT = T>
    bool removeOne(const AT &t);
    template <typename Predicate>
    qsizetype removeIf(Predicate pred);

    inline T *data() { return m_ptr; }
    inline const T *data() const { return m_ptr; }
    inline const T *constData() const { return m_ptr; }

    //iterators
    inline iterator begin() { return m_ptr; }
    inline const_iterator begin() const { return m_ptr; }
    inline const_iterator cbegin() const { return m_ptr; }
    inline const_iterator constBegin() const { return m_ptr; }
    inline iterator end() { return m_ptr + m_size; }
    inline const_iterator end() const { return m_ptr + m_size; }
    inline const_iterator cend() const { return m_ptr + m_size; }
    inline const_iterator constEnd() const { return m_ptr + m_size; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    iterator insert(const_iterator before, qsizetype n, const T &x);
    iterator insert(const_iterator before, T &&x);
    inline iterator insert(const_iterator before, const T &x)
    { return insert(before, 1, x); }

    iterator erase(const_iterator begin, const_iterator end);
    inline iterator erase(const_iterator pos)
    { return erase(pos, pos + 1); }

    //STL 适配接口
    inline bool empty() const { return isEmpty(); }
    inline void push_back(const T &t) { append(t); }
    void push_back(T &&t) { append(std::move(t)); }
    inline void pop_back() { removeLast(); }
    inline T &front() { return first(); }
    inline const T&front() const { return first(); }
    inline T &back() { return last(); }
    inline const T &back() const { return last(); }
    void shrink_to_fit() { squeeze(); }

    template <typename U = T, qsizetype Prealloc2 = Prealloc>
    friend QTypeTraits::compare_eq_result<U> operator==(const QVarLengthArray<T, Prealloc> &l, const QVarLengthArray<T, Prealloc2> &r) {
        if (l.size() != r.size()) {
            return false;
        }
        const T *rb = r.begin();
        const T *b = l.begin();
        const T *e = l.end();
        return std::equal(b, e, QT_MAKE_CHECKED_ARRAY_ITERATOR(rb, r.size()));
    }

    template <typename U = T, qsizetype Prealloc2 = Prealloc>
    friend QTypeTraits::compare_eq_result<U> operator!=(const QVarLengthArray<T, Prealloc> &l, const QVarLengthArray<T, Prealloc2> &r)
    {
        return !(l == r);
    }

    template <typename U = T, qsizetype Prealloc2 = Prealloc>
    friend QTypeTraits::compare_lt_result<U> operator<(const QVarLengthArray<T, Prealloc> &lhs, const QVarLengthArray<T, Prealloc2> &rhs) noexcept
    {
        //按字典顺序比较两个序列。底层最后调用的是成员的比较函数
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename U = T, qsizetype Prealloc2 = Prealloc>
    friend QTypeTraits::compare_lt_result<U> operator>(const QVarLengthArray<T, Prealloc> &lhs, const QVarLengthArray<T, Prealloc2> &rhs) noexcept
    {
        return rhs < lhs;
    }

    template <typename U = T, qsizetype Prealloc2 = Prealloc>
    friend QTypeTraits::compare_lt_result<U> operator<=(const QVarLengthArray<T, Prealloc> &lhs, const QVarLengthArray<T, Prealloc2> &rhs)
    {
        return !(lhs > rhs);
    }

    template <typename U = T, qsizetype Prealloc2 = Prealloc>
    friend QTypeTraits::compare_lt_result<U> operator>=(const QVarLengthArray<T, Prealloc> &lhs, const QVarLengthArray<T, Prealloc2> &rhs) noexcept
    {
        return !(lhs < rhs);
    }

private:
    void reallocate(qsizetype size, qsizetype alloc);
    bool isValidIterator(const const_iterator &i) const
    {
        const std::less<const T *>less = {};
        return !less(cend(), i) && !less(i, cbegin());
    }

private:
    qsizetype m_alloc;  //capacity
    qsizetype m_size;  //size
    //如果容量过大，使用ptr保存，否则使用array保存。有点骚啊，这操作是为了啥？
    T *m_ptr;
    std::aligned_storage_t<sizeof(T), alignof(T)> m_array[Prealloc];
};

template <class T, qsizetype Prealloc>
QVarLengthArray<T, Prealloc>::QVarLengthArray(qsizetype asize)
    : m_size(asize)
{
    Q_ASSERT(Prealloc > 0);
    Q_ASSERT(m_size >= 0);
    if (m_size > Prealloc) {
        m_ptr = reinterpret_cast<T *>(malloc(m_size * sizeof(T)));
        Q_CHECK_PTR(m_ptr);
        m_alloc = m_size;
    }
    else {
        m_ptr = reinterpret_cast<T *>(m_array);
        m_alloc = Prealloc;
    }
    if (QTypeInfo<T>::isComplex) {
        T *i = m_ptr + m_size;
        while (i != m_ptr) {
            new (--i) T;
        }
    }
}

template <class T, qsizetype Prealloc>
inline void QVarLengthArray<T, Prealloc>::reserve(qsizetype asize) {
    if (asize > m_alloc) {
        reallocate(m_size, asize);
    }
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::reallocate(qsizetype asize, qsizetype aalloc)
{
    Q_ASSERT(aalloc >= asize);
    Q_ASSERT(m_ptr);
    T *oldPtr = m_ptr;
    qsizetype oldSize = m_size;
    const qsizetype copySize = qMin(asize, oldSize);
    Q_ASSERT(copySize >= 0);
    if (aalloc != m_alloc) {
        if (aalloc > Prealloc) {
            T *newPtr = reinterpret_cast<T *>(malloc(aalloc * sizeof(T)));
            Q_CHECK_PTR(newPtr);
            m_ptr = newPtr;
            m_alloc = aalloc;
        }
        else {
            m_ptr = reinterpret_cast<T *>(m_array);
            m_alloc = Prealloc;
        }
        m_size = 0;
        if (!QTypeInfo<T>::isRelocatable) {
            while (m_size < copySize) {
                //先构造，再调用原内存块的析构
                new (m_ptr + m_size) T(std::move(*(oldPtr + m_size)));
                (oldPtr + m_size)->~T();
                m_size++;
            }
        }
        else {
            memcpy(static_cast<void *>(m_ptr), static_cast<const void *>(oldPtr), copySize * sizeof(T));
        }
    }
    m_size = copySize;

    if constexpr (QTypeInfo<T>::isComplex) {
        if (oldSize > asize) {
            std::destroy(oldPtr + asize, oldPtr + oldSize);
        }
    }
    if (oldPtr != reinterpret_cast<T *>(m_array) && oldPtr != m_ptr) {
        free(oldPtr);
    }
    if (QTypeInfo<T>::isComplex) {
        while (m_size < asize) {
            new (m_ptr + (m_size)) T;
            m_size++;
        }
    }
    else {
        m_size = asize;
    }
}

template <class T, qsizetype Prealloc>
inline void QVarLengthArray<T, Prealloc>::resize(qsizetype asize)
{
    reallocate(asize, qMax(asize, m_alloc));
}

template <class T, qsizetype Prealloc>
inline void QVarLengthArray<T, Prealloc>::squeeze()
{
    reallocate(m_size, m_size);
}

template <class T, qsizetype Prealloc>
template <typename AT>
inline qsizetype QVarLengthArray<T, Prealloc>::indexOf(const AT &t, qsizetype from) const
{
    if (from < 0) {
        from = qMax(from + m_size, qsizetype(0));
    }
    if (from < m_size) {
        T *n = m_ptr + from - 1;
        T *e = m_ptr + m_size;
        while (++n != e) {
            if (*n == t) {
                return n - m_ptr;
            }
        }
    }
    return -1;
}

template <class T, qsizetype Prealloc>
template <typename AT>
inline qsizetype QVarLengthArray<T, Prealloc>::lastIndexOf(const AT &t, qsizetype from) const
{
    if (from < 0) {
        from += m_size;  //如果加上size之后还是小于0，那么就会返回-1
    }
    else if (from >= m_size) {
        from = m_size - 1;
    }
    if (from >= 0) {
        T *b = m_ptr;
        T *n = m_ptr + from + 1;
        while (n != b) {
            if (*--n == t) {
                return n - b;
            }
        }
    }
    return -1;
}

template <class T, qsizetype Prealloc>
template <typename AT>
inline bool QVarLengthArray<T, Prealloc>::contains(const AT &t) const
{
    T *b = m_ptr;
    T *i = m_ptr + m_size;
    while (i != b) {
        if (*--i == t) {
            return true;
        }
    }
    return false;
}

template <class T, qsizetype Prealloc>
T QVarLengthArray<T, Prealloc>::value(qsizetype i) const
{
    if (i < 0 || i >= size()) {
        return T();
    }
    return at(i);
}

template <class T, qsizetype Prealloc>
T QVarLengthArray<T, Prealloc>::value(qsizetype i, const T &defaultValue) const
{
    if (i < 0 || i >= size()) {
        return defaultValue;
    }
    return at(i);
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::append(const T *buf, qsizetype size)
{
    Q_ASSERT(buf);
    if (size <= 0) {
        return;
    }
    const qsizetype asize = m_size + size;
    if (asize >= m_alloc) {
        reallocate(m_size, qMax(m_size * 2, asize));
    }
    if constexpr(QTypeInfo<T>::isComplex) { //复杂类型通常是指包含有构造函数、析构函数、拷贝构造函数、移动构造函数等成员函数的类型
        //需要走构造函数
        std::uninitialized_copy_n(buf, size, m_ptr + m_size);
    }
    else {
        //直接内存拷贝
        std::memcpy(static_cast<void *>(&m_ptr[m_size]), static_cast<const void *>(buf), size * sizeof(T));
    }
    m_size = asize;
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::prepend(T &&t)
{
    insert(cbegin(), std::move(t));
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::prepend(const T &t)
{
    insert(begin(), 1, t);
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::insert(qsizetype i, T &&t)
{
    Q_ASSERT(i >= 0 && i <= m_size);
    insert(begin() + i, std::move(t));
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::insert(qsizetype i, const T &t)
{
    Q_ASSERT(i >= 0 && i <= m_size);
    insert(begin() + i, 1, t);
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::insert(qsizetype i, qsizetype n, const T &t)
{
    Q_ASSERT(i >= 0 && i <= m_size);
    insert(begin() + i, n, t);
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::replace(qsizetype i, const T &t)
{
    Q_ASSERT(i >= 0 && i < m_size);
    //zhaoyujie TODO 可以判断下地址是否一样就行了吧？
    const T copy(t);  //这里为什么copy？
    data()[i] = copy;
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::remove(qsizetype i)
{
    Q_ASSERT(i >= 0 && i < m_size);
    erase(begin() + i, begin() + i + 1);
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::remove(qsizetype i, qsizetype n)
{
    Q_ASSERT(i >= 0 && n >= 0 && i + n <= m_size);
    erase(begin() + i, begin() + i + n);
}

template <class T, qsizetype Prealloc>
void QVarLengthArray<T, Prealloc>::removeLast()
{
    Q_ASSERT(m_size > 0);
    if constexpr (QTypeInfo<T>::isComplex) {
        m_ptr[m_size - 1].~T();
    }
    --m_size;
}

template <class T, qsizetype Prealloc>
template <typename AT>
qsizetype QVarLengthArray<T, Prealloc>::removeAll(const AT &t)
{
    return QtPrivate::sequential_erase_with_copy(*this, t);
}

template <class T, qsizetype Prealloc>
template <typename AT>
bool QVarLengthArray<T, Prealloc>::removeOne(const AT &t)
{
    return QtPrivate::sequential_erase_one(*this, t);
}

template <class T, qsizetype Prealloc>
template <typename Predicate>
qsizetype QVarLengthArray<T, Prealloc>::removeIf(Predicate pred)
{
    return QtPrivate::sequential_erase_if(*this, pred);
}

template <class T, qsizetype Prealloc>
typename QVarLengthArray<T, Prealloc>::iterator QVarLengthArray<T, Prealloc>::insert(const_iterator before, T &&t)
{
    qsizetype offset = qsizetype(before - m_ptr);
    reserve(m_size + 1);
    if (!QTypeInfo<T>::isRelocatable) {
        T *b = m_ptr + offset;
        T *i = m_ptr + m_size;
        T *j = i + 1;
        if (i != b) {  //循环后裔
            new (--j) T(std::move(*--i));
            while (i != b) {
                *--j = std::move(*--i);
            }
            *b = std::move(t);
        }
        else {  //直接末尾插入
            new (b) T(std::move(t));
        }
    }
    else {   //内存直接移动
        T *b = m_ptr + offset;
        memmove(static_cast<void *>(b + 1), static_cast<const void *>(b), (m_size - offset) * sizeof(T));
        new (b) T(std::move(t));
    }
    m_size += 1;
    return m_ptr + offset;
}

template <class T, qsizetype Prealloc>
typename QVarLengthArray<T, Prealloc>::iterator QVarLengthArray<T, Prealloc>::insert(const_iterator before, size_type n, const T &t)
{
    Q_ASSERT(isValidIterator(before));

    qsizetype offset = qsizetype(before - m_ptr);
    if (n != 0) {
        const T copy(t);
        resize(m_size + n);
        if (!QTypeInfo<T>::isRelocatable) {
            T *b = m_ptr + offset;
            T *j = m_ptr + m_size;
            T *i = j - n;
            while (i != b) {
                *--j = *--i;
            }
            i = b + n;
            while (i != b) {
                *--i = copy;
            }
        }
        else {
            T *b = m_ptr + offset;
            T *i = b + n;
            memmove(static_cast<void *>(i), static_cast<const void *>(b), (m_size - offset - n) * sizeof(T));
            while (i != b) {
                new (--i) T(copy);
            }
        }
    }
    return m_ptr + offset;
}

template <class T, qsizetype Prealloc>
typename QVarLengthArray<T, Prealloc>::iterator QVarLengthArray<T, Prealloc>::erase(const_iterator abegin, const_iterator aend)
{
    Q_ASSERT(isValidIterator(abegin));
    Q_ASSERT(isValidIterator(aend));

    qsizetype f = qsizetype(abegin - m_ptr);
    qsizetype l = qsizetype(aend - m_ptr);
    qsizetype n = l - f;

    if constexpr (QTypeInfo<T>::isComplex) {
        //std::move时，会调用operator=(T &&) 或 operator=(const T &)
        std::move(m_ptr + l, m_ptr + m_size, QT_MAKE_CHECKED_ARRAY_ITERATOR(m_ptr + f, m_size - f));
        std::destroy(m_ptr + m_size - n, m_ptr + m_size);
    }
    else {
        memmove(static_cast<void *>(m_ptr + f), static_cast<const void *>(m_ptr + l), (m_size - l) * sizeof(T));
    }
    m_size -= n;
    return m_ptr + f;
}

QT_END_NAMESPACE

#endif //QVARLENGTHARRAY_H
