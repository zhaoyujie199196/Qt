//
// Created by Yujie Zhao on 2023/2/28.
//

#ifndef QARRAYDATA_H
#define QARRAYDATA_H

#include <QtCore/qglobal.h>
#include <QtCore/qflags.h>
#include <QtCore/qatomic.h>
#include <QtCore/qpair.h>

QT_BEGIN_NAMESPACE

//数组数据，定义了一些通用的方法与定义
struct QArrayData
{
    enum AllocationOption {
        Grow,
        KeepSize
    };
    enum GrowthPosition {
        GrowsAtEnd,
        GrowsAtBeginning
    };
    enum ArrayOption {
        ArrayOptionDefault = 0,
        CapacityReserved = 0x1  //TODO CapacityReserved有什么作用
    };
    Q_DECLARE_FLAGS(ArrayOptions, ArrayOption)

    QBasicAtomicInt ref_;  //zhaoyujie TODO 可直接使用QRefCount
    ArrayOptions flags;
    qsizetype alloc;

    qsizetype allocatedCapacity() noexcept { return alloc; }
    qsizetype constAllocatedCapacity() const noexcept { return alloc; }

    bool ref() noexcept
    {
        ref_.ref();
        return true;
    }

    //减1后如果为0，返回false，不为0返回true
    bool deref()
    {
        return ref_.deref();
    }

    bool isShared() const noexcept
    {
        return ref_.loadRelaxed() != 1;
    }

    /*
     * 用在修改之前。
     * Qt使用隐式共享技术，多个示例可以共享同一块数据区域。
     * 如果某个示例对数据进行了修改，需要判断数据有没有被共享。
     * 如果被共享了，需要先分离复制一份
     * */
    bool needsDetach() const noexcept {
        return ref_.loadRelaxed() > 1;
    }

    qsizetype detachCapacity(qsizetype newSize) const noexcept {
        if (flags & CapacityReserved && newSize < constAllocatedCapacity()) {
            return constAllocatedCapacity();
        }
        return newSize;
    }

    //分配capacity个大小的空间，包含QByteArray的数据头
    static void *allocate(QArrayData **pData, qsizetype objectSize, qsizetype alignment, qsizetype capacity, AllocationOption option = QArrayData::KeepSize);
    static void deallocate(QArrayData *data, qsizetype objectSize, qsizetype alignment) noexcept;
    static QPair<QArrayData *, void *> reallocateUnaligned(QArrayData *data, void *dataPointer, qsizetype objectSize, qsizetype newCapacity, AllocationOption option) noexcept;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QArrayData::ArrayOptions)

//带有类型的数组数据
template <class T>
struct QTypedArrayData : public QArrayData
{
    struct AlignmentDummy {
        QArrayData header;
        T data;
    };

    static QPair<QTypedArrayData *, T *> allocate(qsizetype capacity, AllocationOption option = QArrayData::KeepSize)
    {
        static_assert(sizeof(QTypedArrayData) == sizeof(QArrayData));
        QArrayData *d = nullptr;
        void *result = QArrayData::allocate(&d, sizeof(T), alignof(AlignmentDummy), capacity, option);
//#if __has_builtin(__builtin_assume_aligned)
//        result = __builtin_assume_aligned(result, Q_ALIGNOF(AlignmentDummy));
//#endif
        //d是QArrayData的日志，QArrayData中的数据数组的地址
        return qMakePair(static_cast<QTypedArrayData *>(d), static_cast<T *>(result));
    }

    static void deallocate(QArrayData *data) noexcept
    {
        static_assert(sizeof(QTypedArrayData) == sizeof(QArrayData));
        QArrayData::deallocate(data, sizeof(T), alignof(AlignmentDummy));
    }

    static T *dataStart(QArrayData *data, qsizetype alignment) noexcept
    {
        Q_ASSERT(alignment >= qsizetype(alignof(QArrayData)) && !(alignment & (alignment - 1)));
        void *start = reinterpret_cast<void *>((quintptr(data) + sizeof(QArrayData) + alignment - 1) & ~(alignment - 1));
        return static_cast<T *>(start);
    }

    static QPair<QTypedArrayData *, T *> reallocateUnaligned(QTypedArrayData *data, T *dataPointer, qsizetype capacity, AllocationOption option)
    {
        static_assert(sizeof(QTypedArrayData) == sizeof(QArrayData));
        QPair<QArrayData *, void *> pair = QArrayData::reallocateUnaligned(data, dataPointer, sizeof(T), capacity, option);
        return qMakePair(static_cast<QTypedArrayData *>(pair.first), static_cast<T *>(pair.second));
    }
};


namespace QtPrivate {
    struct QContainerImplHelper {
        enum CutResult {
            Null,
            Empty,
            Full,
            Subset
        };
        static constexpr CutResult mid(qsizetype originalLength, qsizetype *_position, qsizetype *_length) {
            qsizetype &position = *_position;
            qsizetype &length = *_length;
            //位置超出了范围
            if (position > originalLength) {
                position = 0;
                length = 0;
                return Null;
            }
            if (position < 0) {
                if (length < 0 || length + position >= originalLength) {
                    position = 0;
                    length = originalLength;
                    return Full;
                }
                if (length + position <= 0) {
                    position = length = 0;
                    return Null;
                }
                else {
                    length += position;
                    position = 0;
                }
            }
            else if (size_t(length) > size_t(originalLength - position)) {
                //使用size_t强转，length如果 < 0,也会走进这个分支
                length = originalLength - position;
            }
            if (position == 0 && length == originalLength) {
                return Full;
            }
            return length > 0 ? Subset : Empty;
        }
    };
}

QT_END_NAMESPACE

#endif //QARRAYDATA_H
