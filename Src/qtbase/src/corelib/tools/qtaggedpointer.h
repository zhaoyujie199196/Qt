//
// Created by Yujie Zhao on 2023/11/30.
//

#ifndef QTAGGEDPOINTER_H
#define QTAGGEDPOINTER_H

#include <QtCore/qglobal.h>
#include <QtCore/qalgorithms.h>
#include <QtCore/qmath.h>
#include <QtCore/qtypeinfo.h>

QT_BEGIN_NAMESPACE

/*
 * 利用malloc出来的指针的最后一位为0的特性，存储一些小数据
 * https://www.zhihu.com/question/40636241
 * */

namespace QtPrivate {
    constexpr quint8 nextByteSize(quint8 bits) {
        return (bits + 7) / 8;
    }

    template <typename T>
    struct TagInfo {
        static constexpr size_t alignment = alignof(T);
        static_assert((alignment & (alignment - 1)) == 0);

        static constexpr quint8 tagBits = QtPrivate::qConstexprCountTrailingZeroBits(alignment);
        static_assert(tagBits > 0);

        static constexpr size_t tagSize = QtPrivate::qConstexprNextPowerOfTwo(nextByteSize(tagBits));
        static_assert(tagSize < sizeof(quintptr));

        using TagType = typename QIntegerForSize<tagSize>::Unsigned ;
    };
}

template <typename T, typename Tag = typename QtPrivate::TagInfo<T>::TagType>
class QTaggedPointer
{
public:
    using Type = T;
    using TagType = Tag;

    static constexpr quintptr tagMask() { return QtPrivate::TagInfo<T>::alignment - 1; }
    static constexpr quintptr pointerMask() { return ~tagMask(); }

    constexpr QTaggedPointer() noexcept : d(0), p(nullptr) {}
    constexpr QTaggedPointer(std::nullptr_t) noexcept : QTaggedPointer() {}

    explicit QTaggedPointer(T *pointer, Tag tag = Tag()) noexcept
        : d(quintptr(pointer) | quintptr(tag))
        , p(pointer)
    {
        static_assert(sizeof(Type *) == sizeof(QTaggedPointer));
        assert((quintptr(pointer) & tagMask()) == 0);
        assert((static_cast<typename QtPrivate::TagInfo<T>::TagType>(tag) & pointerMask()) == 0);
    }

    Type &operator *() const noexcept
    {
        assert(data());
        return *data();
    }

    Type *operator->() const noexcept
    {
        return data();
    }

    explicit operator bool() const noexcept
    {
        return !isNull();
    }

    QTaggedPointer &operator=(T *other) noexcept
    {
        d = reinterpret_cast<quintptr>(other) | (d & tagMask());
        p = other;
        return *this;
    }

    static constexpr Tag maximumTag() noexcept
    {
        return TagType(typename QtPrivate::TagInfo<T>::TagType(tagMask()));
    }

    void setTag(Tag tag) {
        Q_ASSERT((static_cast<typename QtPrivate::TagInfo<T>::TagType>(tag) & pointerMask()) == 0);
        d = (d & pointerMask()) | static_cast<quintptr >(tag);
    }

    Tag tag() const noexcept {
        return TagType(d & tagMask());
    }

    T *data() const noexcept {
        return reinterpret_cast<T *>(d & pointerMask());
    }

    bool isNull() const noexcept {
        return !data();
    }

    void swap(QTaggedPointer &other) noexcept
    {
        qSwap(d, other.d);
        qSwap(p, other.p);
    }

    //只比较了指针
    friend inline bool operator==(QTaggedPointer lhs, QTaggedPointer rhs) noexcept
    {
        return lhs.data() == rhs.data();
    }

    friend inline bool operator!=(QTaggedPointer lhs, QTaggedPointer rhs) noexcept
    {
        return lhs.data() != rhs.data();
    }

    friend inline bool operator==(QTaggedPointer lhs, std::nullptr_t) noexcept
    {
        return lhs.isNull();
    }

    friend inline bool operator==(std::nullptr_t, QTaggedPointer rhs) noexcept
    {
        return rhs.isNull();
    }

    friend inline bool operator!=(QTaggedPointer lhs, std::nullptr_t) noexcept
    {
        return !lhs.isNull();
    }

    friend inline bool operator!=(std::nullptr_t, QTaggedPointer rhs) noexcept
    {
        return !rhs.isNull();
    }

    friend inline bool operator!(QTaggedPointer ptr) noexcept
    {
        return !ptr.data();
    }

    friend inline void swap(QTaggedPointer &p1, QTaggedPointer &p2) noexcept
    {
        p1.swap(p2);
    }


protected:
    quintptr d;
    Type *p;    //方便调试
};

template <typename T, typename Tag>
constexpr inline std::size_t qHash(QTaggedPointer<T, Tag> p, std::size_t seed = 0) noexcept
{
    Q_ASSERT(false);
    return 0;
}

template <typename T, typename Tag>
class QTypeInfo<QTaggedPointer<T, Tag>> : public QTypeInfoMerger<QTaggedPointer<T, Tag>, quintptr>
{
};



QT_END_NAMESPACE

#endif //QTAGGEDPOINTER_H
