//
// Created by Yujie Zhao on 2023/3/21.
//

#ifndef QCONTAINERTOOLS_IMPL_H
#define QCONTAINERTOOLS_IMPL_H

#include <QtCore/qglobal.h>
#include <algorithm>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
    //指针p是否在[b, e)之间
    template <typename T, typename Cmp = std::less<>>
    static constexpr bool q_points_into_range(const T *p, const T *b, const T *e, Cmp less = {}) noexcept {
        return !less(p, b) && less(p, e);
    }

    //将first数组移动到out中
    template <typename T, typename N>
    void q_uninitialized_relocate_n(T *first, N n, T *out)
    {
        if constexpr(QTypeInfo<T>::isRelocatable) {  //可以直接移动内存
            if (n != N(0)) {
                std::memmove(static_cast<void *>(out),
                             static_cast<const void *>(first),
                             n * sizeof(T));
            }
        }
        else {
            //调用移动拷贝，调用first的析构函数，但是没有销毁内存
            std::uninitialized_move_n(first, n, out);
            if constexpr(QTypeInfo<T>::isComplex) {
                std::destroy_n(first, n);
            }
        }
    }

    template <typename iterator, typename N>
    void q_relocate_overlap_n_left_move(iterator first, N n, iterator d_first) {
        Q_ASSERT(n);
        Q_ASSERT(d_first < first);
        using T = typename std::iterator_traits<iterator>::value_type;

        //std::addressof用来获取对象的地址，因为&可以被重载
        struct Destructor
        {
            iterator *iter;
            iterator end;
            iterator intermediate;

            Destructor(iterator &it) noexcept : iter(std::addressof(it)), end(it) {}
            void commit() noexcept { iter = std::addressof(end); }
            void freeze() noexcept {
                intermediate = *iter;
                iter = std::addressof(intermediate);
            }
            ~Destructor() noexcept {
                for (const int step = *iter < end ? 1 : -1; *iter != end;) {
                    std::advance(*iter, step);
                    (*iter)->~T();
                }
            }
        };
        Destructor destroyer(d_first);

        const iterator d_last = d_first + n;
        auto pair = std::minmax(d_last, first);
        iterator overlapBegin = pair.first;
        iterator overlapEnd = pair.second;
        while (d_first != overlapBegin) {
            //如果有noexcept修饰的移动构造，std::move_if_noexcept返回T &&。如果没有，返回T &
            new (std::addressof(*d_first)) T(std::move_if_noexcept(*first));
            ++d_first;
            ++first;
        }
        destroyer.freeze();
        while (d_first != d_last) {
            *d_first = std::move_if_noexcept(*first);
            ++d_first;
            ++first;
        }
        Q_ASSERT(d_first == destroyer.end + n);
        destroyer.commit();
        while (first != overlapEnd) {
            (--first)->~T();
        }
    }

    //将【first, first+n) 迁移至[d_first, d_first +n)，注意覆盖
    template <typename T, typename N>
    void q_relocate_overlap_n(T *first, N n, T *d_first)
    {
        static_assert(std::is_nothrow_destructible_v<T>, "This algorithm requires that T has a non-throwing destructor");
        if (n == N(0) || first == d_first || first == nullptr || d_first == nullptr) {
            return;
        }
        if constexpr (QTypeInfo<T>::isRelocatable) {
            //如果可以直接游动，直接使用memmove
            std::memmove(static_cast<void *>(d_first), static_cast<const void *>(first), n * sizeof(T));
        }
        else {
            if (d_first < first) { //从右往左移动
                q_relocate_overlap_n_left_move(first, n, d_first);
            }
            else {  //从左往右移动，使用反向迭代器
                auto rfirst = std::make_reverse_iterator(first + n);
                auto rd_first = std::make_reverse_iterator(d_first + n);
                q_relocate_overlap_n_left_move(rfirst, n, rd_first);
            }
        }
    }

    template <typename Iterator>
    using IfIsInputIterator = typename std::enable_if<
            std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::input_iterator_tag>::value,
            bool>::type;

    template <typename Iterator>
    using IfIsForwardIterator = typename std::enable_if<
            std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::forward_iterator_tag>::value,
            bool>::type;

    template <typename Iterator>
    using IfIsNotForwardIterator = typename std::enable_if<
            !std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::forward_iterator_tag>::value,
            bool>::type;

    template <typename Container, typename T>
    auto sequential_erase(Container &c, const T &t)
    {
        const auto cbegin = c.cbegin();
        const auto cend = c.cend();
        const auto t_it = std::find(cbegin, cend, t);
        auto result = std::distance(cbegin, t_it);
        if (result == c.size()) {  //如果找不到，std::find返回end(),distance是通过it++循环实现的，所以找不到时，distance为size
            return result - result; // 没有删除，这里如果直接return 0,将和下面的return result冲突，编译不过
        }
        const auto e = c.end();
        //std::next将迭代器往后移n位
        //std::remove并不会真正删除元素，只是将保留的元素往前移。it表示往后移的第一个等于t的元素
        auto nextIt = std::next(c.begin(), result);
        const auto it = std::remove(nextIt, e, t);
        result = std::distance(it, e);
        c.erase(it, e);  //真正删除元素在这一步
        return result;
    }

    template <typename Container, typename T>
    auto sequential_erase_with_copy(Container &c, const T &t)
    {
        using CopyProxy = std::conditional_t<std::is_copy_constructible_v<T>, T, const T &>;
        const T &tCopy = CopyProxy(t);  //zhaoyujie TODO 这里为什么要Copy？防止删除容器中的地址？
        return sequential_erase(c, tCopy);
    }

    template <typename Container, typename T>
    auto sequential_erase_one(Container &c, const T &t)
    {
        const auto cend = c.cend();
        const auto it = std::find(c.cbegin(), cend, t);
        if (it == cend) {
            return false;
        }
        c.erase(it);
        return true;
    }

    template <typename Container, typename Predicate>
    auto sequential_erase_if(Container &c, Predicate &pred)
    {
        const auto cbegin = c.cbegin();
        const auto cend = c.cend();
        const auto t_it = std::find_if(cbegin, cend, pred);
        auto result = std::distance(cbegin, t_it);
        if (result == c.size()) {
            return result - result;
        }
        const auto e = c.end();
        const auto it = std::remove_if(std::next(c.begin(), result), e, pred);
        result = std::distance(it, e);
        c.erase(it, e);
        return result;
    }

    template <typename Container, typename InputIterator, IfIsNotForwardIterator<InputIterator> = true>
    void reverseIfForwardIterator(Container *, InputIterator, InputIterator)
    {
    }

    template <typename Container, typename ForwardIterator, IfIsForwardIterator<ForwardIterator> = true>
    void reserveIfForwardIterator(Container *c, ForwardIterator f, ForwardIterator l)
    {
        c->reserve(static_cast<typename Container::size_type>(std::distance(f, l)));
    }
}

QT_END_NAMESPACE

#endif //QCONTAINERTOOLS_IMPL_H
