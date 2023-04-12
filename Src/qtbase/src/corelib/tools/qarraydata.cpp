//
// Created by Yujie Zhao on 2023/2/28.
//
#include "qarraydata.h"
#include "QtCore/qmath.h"
#include "QtCore/private/qtools_p.h"
#include "QtCore/private/qnumeric_p.h"
#include "QtCore/qstring.h"
#include "QtCore/qbytearray.h"

QT_BEGIN_NAMESPACE

//QArrayData八字节对齐，加了alignas(std::max_align_t)依然八字节对齐，不明白用处为何
struct alignas(std::max_align_t) AlignedQArrayData : QArrayData {

};

static qsizetype qCalculateBlockSize(qsizetype elementCount, qsizetype elementSize, qsizetype headerSize) noexcept
{
    Q_ASSERT(elementSize);
    size_t bytes;
    //_overflow的计算会考虑超出有效范围
    if (Q_UNLIKELY(mul_overflow(size_t(elementSize), size_t(elementCount), &bytes)) ||
            Q_UNLIKELY(add_overflow(bytes, size_t(headerSize), &bytes))) {
        return -1;
    }
    if (Q_UNLIKELY(qsizetype(bytes) < 0)) {
        return -1;
    }
    return qsizetype(bytes);
}

static CalculateGrowingBlockSizeResult qCalculateGrowingBlockSize(qsizetype elementCount, qsizetype elementSize, qsizetype headerSize) noexcept
{
    CalculateGrowingBlockSizeResult result = { qsizetype(-1), qsizetype(-1) };
    //计算容纳当前数据所需要的大小
    qsizetype bytes = qCalculateBlockSize(elementCount, elementSize, headerSize);
    if (bytes < 0) {
        return result;
    }
    //计算冗余空间
    size_t morebytes = static_cast<size_t>(qNextPowerOfTwo(quint64(bytes)));
    //计算是使用的无符号整数计算，换算成有符号可能会超出最大正数表示范围
    if (Q_UNLIKELY(qsizetype(morebytes) < 0)) {
        //zhaoyujie TODO 一半的增长也可能会超出范围啊，这里的逻辑到底啥意思。。。
        Q_ASSERT(false);
        bytes +=(morebytes - bytes) / 2;
    }
    else {
        bytes = qsizetype(morebytes);
    }
    result.elementCount = (bytes - headerSize) / elementSize;
    result.size = bytes;
    return result;
}

static inline qsizetype calculateBlockSize(qsizetype &capacity, qsizetype objectSize, qsizetype headerSize, QArrayData::AllocationOption option)
{
    if (option == QArrayData::Grow) {
        //容器可增长，内存按照2的n次方来分配
        auto r = qCalculateGrowingBlockSize(capacity, objectSize, headerSize);
        capacity = r.elementCount;
        return r.size;
    }
    else {
        //容器不可增长，需要多少就分配多少
        return qCalculateBlockSize(capacity, objectSize, headerSize);
    }
}

//存储数组尾部的'\0'  zhaoyujie TODO 添加一个byte就行了啊，为什么需要判断QChar
static inline qsizetype reserveExtraBytes(qsizetype allocSize)
{
    constexpr qsizetype extra = std::max(sizeof(QByteArray::value_type), sizeof(QString::value_type));
    if (Q_UNLIKELY(allocSize < 0)) {
        return -1;
    }
    if (Q_UNLIKELY(add_overflow(allocSize, extra, &allocSize))) {
        return -1;
    }
    return allocSize;
}

static QArrayData *allocateData(qsizetype allocSize)
{
    auto data = ::malloc(size_t(allocSize));
    QArrayData *header = static_cast<QArrayData *>(data);
    if (header) {
        header->ref_.storeRelaxed(1);
        header->flags = {};
        header->alloc = 0;
    }
    return header;
}

void *QArrayData::allocate(QArrayData **dptr, qsizetype objectSize, qsizetype alignment, qsizetype capacity,
                           AllocationOption option) {
    Q_ASSERT(dptr);
    //alignment得是2的次方 AlignmentDummy里面已经考虑了T和QArrayData，所以必然比alignof(QArrayData)大
    Q_ASSERT(alignment >= qsizetype(alignof(QArrayData)) && !(alignment & (alignment - 1)));
    if (capacity == 0) {  //zhaoyujie TODO
        *dptr = nullptr;
        return nullptr;
    }

    qsizetype headerSize = sizeof(AlignedQArrayData);
    const qsizetype headerAlignment = alignof(AlignedQArrayData);
    if (alignment > headerAlignment) {
        Q_ASSERT(false);   //zhaoyujie TODO 这个判断应该进不来，如果进来了再研究下
        headerSize += alignment - headerAlignment;
    }
    Q_ASSERT(headerSize > 0);
    //计算内存大小
    qsizetype allocSize = calculateBlockSize(capacity, objectSize, headerSize, option);
    allocSize = reserveExtraBytes(allocSize);
    if (Q_UNLIKELY(allocSize < 0)) {
        *dptr = nullptr;
        return nullptr;
    }

    QArrayData *header = allocateData(allocSize);
    void *data = nullptr;
    if (header) {
        //根据偏移计算出数据数组的地址
        data = QTypedArrayData<void>::dataStart(header, alignment);
        header->alloc = qsizetype(capacity);
    }
    *dptr = header;
    return data;
}

void QArrayData::deallocate(QArrayData *data, qsizetype objectSize, qsizetype alignment) noexcept
{
    //alignment是2的整数倍，且 >= alignof(QArrayData)（通过QArrayDataDummy）
    Q_ASSERT(alignment >= qsizetype(alignof(QArrayData)
             && !(alignment & alignment - 1)));
    //malloc deallocate算法
    ::free(data);
}

QPair<QArrayData *, void *> QArrayData::reallocateUnaligned(QArrayData *data, void *dataPointer, qsizetype objectSize,
                                                            qsizetype newCapacity, AllocationOption option) noexcept {
    Q_ASSERT(!data || !data->isShared());
    const qsizetype headerSize = sizeof(AlignedQArrayData);
    qsizetype allocSize = calculateBlockSize(newCapacity, objectSize, headerSize, option);
    if (Q_UNLIKELY(allocSize < 0)) {  //分配内存失败
        return qMakePair<QArrayData *, void *>(nullptr, nullptr);
    }

    const qptrdiff offset = dataPointer
            ? reinterpret_cast<char *>(dataPointer) - reinterpret_cast<char *>(data)
            : headerSize;
    Q_ASSERT(offset > 0);
    Q_ASSERT(offset < allocSize);

    allocSize = reserveExtraBytes(allocSize);
    if (Q_UNLIKELY(allocSize < 0)) {
        return qMakePair(data, dataPointer);
    }
    /*
     * realloc：https://blog.csdn.net/Azjrael/article/details/125966154
     * 先尝试在原地拓展，如果原地拓展失败，分配新内存，拷贝原地址的数据，释放原地址
     * */
    QArrayData *header = static_cast<QArrayData *>(::realloc(data, size_t(allocSize)));
    if (header) {
        header->alloc = newCapacity;
        dataPointer = reinterpret_cast<char *>(header) + offset;
    }
    else {
        dataPointer = nullptr;
    }
    return qMakePair(static_cast<QArrayData *>(header), dataPointer);

}

QT_END_NAMESPACE