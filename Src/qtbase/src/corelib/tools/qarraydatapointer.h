//
// Created by Yujie Zhao on 2023/3/8.
//

#ifndef QARRAYDATAPOINTER_H
#define QARRAYDATAPOINTER_H

#include "qarraydata.h"
#include "qarraydataops.h"
#include "QtCore/private/qcontainertools_impl.h"

QT_BEGIN_NAMESPACE

template <class T>
struct QArrayDataPointer
{
private:
    typedef QTypedArrayData<T> Data;
    typedef QArrayDataOps<T> DataOps;

public:
    enum {
        //is_arithmetic 检查类型是否是算数类型（整数类型和浮点类型）
        pass_parameter_by_value = std::is_arithmetic<T>::value || std::is_pointer<T>::value || std::is_enum<T>::value
    };
    typedef typename std::conditional<pass_parameter_by_value, T, const T &>::type parameter_type;

    constexpr QArrayDataPointer() noexcept
        : d(nullptr), ptr(nullptr), size(0)
    {}

    QArrayDataPointer(const QArrayDataPointer &other) noexcept
        : d(other.d), ptr(other.ptr), size(other.size)
    {
        ref();
    }

    constexpr QArrayDataPointer(Data *header, T *aData, qsizetype n = 0) noexcept
        : d(header), ptr(aData), size(n)
    {}

    explicit QArrayDataPointer(QPair<QTypedArrayData<T> *, T *> aData, qsizetype n = 0) noexcept
        : d(aData.first), ptr(aData.second), size(n)
    {}

    QArrayDataPointer &operator=(const QArrayDataPointer &other) noexcept
    {
        //直接构造了临时对象，用作swap，临时对象的西沟函数会尝试释放资源
        QArrayDataPointer tmp(other);
        this->swap(tmp);
        return *this;
    }

    QArrayDataPointer(QArrayDataPointer &&other) noexcept
        : d(other.d), ptr(other.ptr), size(other.size)
    {
        other.d = nullptr;
        other.ptr = nullptr;
        other.size = 0;
    }

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QArrayDataPointer)

    ~QArrayDataPointer()
    {
        //引用计数-1后为0，需要释放对象
        if (!deref()) {
            (*this)->destroyAll();
            Data::deallocate(d);
        }
    }

    DataOps &operator*() noexcept { return *static_cast<DataOps *>(this); }
    DataOps *operator->() noexcept { return static_cast<DataOps *>(this); }
    const DataOps &operator*() const noexcept { return *static_cast<const DataOps *>(this); }
    const DataOps *operator->() const noexcept { return static_cast<const DataOps *>(this); }

    bool isNull() const noexcept {return !ptr;}
    T *data() noexcept {return ptr;}
    const T *data() const noexcept {return ptr;}

    T *begin() noexcept {return data();}
    T *end() noexcept {return data() + size;}
    const T *begin() const noexcept {return data();}
    const T *end() const noexcept {return data() + size;}
    const T *constBegin() const noexcept {return data();}
    const T *constEnd() const noexcept {return data() + size;}

    friend bool operator==(const QArrayDataPointer &lhs, const QArrayDataPointer &rhs) noexcept
    {
        return lhs.data() == rhs.data() && lhs.size == rhs.size;
    }

    void swap(QArrayDataPointer &other) noexcept
    {
        qSwap(d, other.d);
        qSwap(ptr, other.ptr);
        qSwap(size, other.size);
    }

    void clear() noexcept(std::is_nothrow_destructible<T>::value)
    {
        //尝试释放交给析构tmp时进行
        QArrayDataPointer tmp;
        swap(tmp);
    }

    void detach(QArrayDataPointer *old = nullptr)
    {
        if (needsDetach()) {
            reallocateAndGrow(QArrayData::GrowsAtEnd, 0, old);
        }
    }

    void detachAndGrow(QArrayData::GrowthPosition where, qsizetype n, const T **data, QArrayDataPointer *old)
    {
        const bool detach = needsDetach();
        bool readjusted = false;
        if (!detach) {
            if (!n || (where == QArrayData::GrowsAtBeginning && freeSpaceAtBegin() >= n)
                || (where == QArrayData::GrowsAtEnd && freeSpaceAtEnd() >= n)) {
                return;  //不需要空间增长
            }
            readjusted = tryReadjustFreeSpace(where, n, data);
            Q_ASSERT(!readjusted
                     || (where == QArrayData::GrowsAtBeginning && freeSpaceAtBegin() > n)
                     || (where == QArrayData::GrowsAtEnd && freeSpaceAtEnd() >= n));
        }
        if (!readjusted) {
            reallocateAndGrow(where, n, old);
        }
    }

    bool tryReadjustFreeSpace(QArrayData::GrowthPosition pos, qsizetype n, const T **data = nullptr) {
        Q_ASSERT(!this->needsDetach());
        Q_ASSERT(n > 0);
        Q_ASSERT((pos == QArrayData::GrowsAtEnd && this->freeSpaceAtEnd() < n)
                 || (pos == QArrayData::GrowsAtBeginning && this->freeSpaceAtBegin() < n));
        const qsizetype capacity = this->constAllocatedCapacity();
        const qsizetype freeAtBegin = this->freeSpaceAtBegin();
        const qsizetype freeAtEnd = this->freeSpaceAtEnd();
        qsizetype dataStartOffset = 0;
        if (pos == QArrayData::GrowsAtEnd && freeAtBegin >= n && ((3 * this->size) < (2 * capacity))) {
//            Q_ASSERT(false);
        }
        else if (pos == QArrayData::GrowsAtBeginning && freeAtEnd >= n && ((3 * this->size) < capacity)) {
//            Q_ASSERT(false);
            dataStartOffset = n + qMax<int>(0, (capacity - this->size - n) / 2);
        }
        else {
            return false;
        }
        relocate(dataStartOffset - freeAtBegin, data);
        Q_ASSERT((pos == QArrayData::GrowsAtEnd && this->freeSpaceAtEnd() >= n)
                 || (pos == QArrayData::GrowsAtBeginning && this->freeSpaceAtBegin() >= n));
        return true;
    }

    void relocate(qsizetype offset, const T **data = nullptr) {
        T *res = this->ptr + offset;
        QtPrivate::q_relocate_overlap_n(this->ptr, this->size, res);
        if (data && QtPrivate::q_points_into_range(*data, this->begin(), this->end())) {
            *data += offset;
        }
        this->ptr = res;
    }

    void ref() noexcept { if (d) d->ref(); }
    bool deref() noexcept { return !d || d->deref(); }
    bool isMutable() const noexcept { return d; } //isMutable可变的？有什么作用?
    bool isShared() const noexcept { return !d || d->isShared(); }
    bool needsDetach() const noexcept { return !d || d->needsDetach(); }
    qsizetype detachCapacity(qsizetype newSize) const noexcept { return d ? d->detachCapacity(newSize) : newSize; }
    const typename Data::ArrayOptions flags() const noexcept  { return d ? d->flags : Data::ArrayOptionDefault; }
    void setFlag(typename Data::ArrayOptions f) noexcept  { Q_ASSERT(d); d->flags |= f; }
    void clearFlag(typename Data::ArrayOptions f) noexcept  { if (d) d->flags & !f; }

    Data *d_ptr() noexcept { return d; }
    void setBegin(T *begin) noexcept { ptr = begin; }

    qsizetype allocatedCapacity() noexcept { return d ? d->allocatedCapacity() : 0; }
    qsizetype constAllocatedCapacity() const noexcept {return d ? d->constAllocatedCapacity() : 0; }


    Q_NEVER_INLINE void reallocateAndGrow(QArrayData::GrowthPosition where, qsizetype n, QArrayDataPointer *old = nullptr)
    {
        if constexpr (QTypeInfo<T>::isRelocatable && alignof(T) <= alignof(std::max_align_t)) {
            if (where == QArrayData::GrowsAtEnd && !old && !needsDetach() && n > 0) {  //有原地址，不需要分离，可以直接本地构造
                (*this)->reallocate(constAllocatedCapacity() - freeSpaceAtEnd() + n, QArrayData::Grow);
                return;
            }
        }
        //分配内存
        QArrayDataPointer dp(allocateGrow(*this, n, where));
        if (n > 0) {
            Q_CHECK_PTR(dp.data());
        }
        if (where == QArrayData::GrowsAtBeginning) {
            Q_ASSERT(dp.freeSpaceAtBegin() >= n);
        }
        else {
            Q_ASSERT(dp.freeSpaceAtEnd() >= n);
        }
        //如果原来的容器中有数据，需要将原容器中的数据复制到新容器中
        if (size) {
            qsizetype toCopy = size;
            if (n < 0) {
                toCopy += n;
            }
            if (needsDetach() || old) {
                dp->copyAppend(begin(), begin() + toCopy);
            }
            else {
                dp->moveAppend(begin(), begin() + toCopy);
            }
            Q_ASSERT(dp.size == toCopy);
        }
        swap(dp);
        if (old) {
            old->swap(dp);
        }
    }

    qsizetype freeSpaceAtBegin() const noexcept
    {
        if (d == nullptr) {
            return 0;
        }
        auto ret = this->ptr - Data::dataStart(d, alignof(typename Data::AlignmentDummy));
        return ret;
    }

    qsizetype freeSpaceAtEnd() const noexcept
    {
        if (d == nullptr) {
            return 0;
        }
        auto ret = d->constAllocatedCapacity() - freeSpaceAtBegin() - this->size;
        return ret;
    }

    //分配增长的内存
    static QArrayDataPointer allocateGrow(const QArrayDataPointer &from, qsizetype n, QArrayData::GrowthPosition position)
    {
        qsizetype minCapacity = qMax(from.size, from.constAllocatedCapacity()) + n;
        minCapacity -= (position == QArrayData::GrowsAtEnd) ? from.freeSpaceAtEnd() : from.freeSpaceAtBegin();
        qsizetype capacity = from.detachCapacity(minCapacity);
        const bool grows = capacity > from.constAllocatedCapacity();
        auto [header, dataPtr] = Data::allocate(capacity, grows ? QArrayData::Grow : QArrayData::KeepSize);
        const bool valid = header != nullptr && dataPtr != nullptr;
        if (!valid) {
            return QArrayDataPointer(header, dataPtr);
        }
        if (position == QArrayData::GrowsAtBeginning) {  //插入在前面的话，分配的空余空间前后各一半
            dataPtr += (n + qMax<int>(0, (header->alloc - from.size - n) / 2));
        }
        else {
            dataPtr += from.freeSpaceAtBegin();
        }
        header->flags = from.flags();
        return QArrayDataPointer(header, dataPtr);
    }

public:
    Data *d;
    T *ptr;
    qsizetype size;
};

QT_END_NAMESPACE

#endif //QARRAYDATAPOINTER_H
