//
// Created by Yujie Zhao on 2024/2/19.
//

#ifndef QFUNCTIONALTOOLS_IMPL_H
#define QFUNCTIONALTOOLS_IMPL_H

#include <type_traits>
#include <utility>
#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

#define FOR_EACH_CVREF(op) \
    op(&)                  \
    op(const &)            \
    op(&&)                 \
    op(const &&)


namespace QtPrivate {
    namespace detail {

#define MAKE_GETTER(cvref) \
    constexpr Object cvref object() cvref noexcept \
    { return static_cast<Object cvref>(o); }

        template <typename Object, typename = void>
        struct StorageByValue
        {
            Object o;
            FOR_EACH_CVREF(MAKE_GETTER)
        };
#undef MAKE_GETTER

#define MAKE_GETTER(cvref) \
    constexpr Object cvref object() cvref noexcept \
    { return static_cast<Object cvref>(*this); }
        template <typename Object, typename Tag = void>
        struct StorageEmptyBaseClassOptimization : Object
        {
            StorageEmptyBaseClassOptimization(Object &&o)
                : Object(std::move(o))
            {}

            StorageEmptyBaseClassOptimization(const Object &o)
                : Object(o)
            {}

            FOR_EACH_CVREF(MAKE_GETTER)
        };
    }
#undef MAKE_GETTER

    template <typename Object, typename Tag = void>
    using CompactStorage = typename std::conditional_t<
            std::conjunction_v<
                    std::is_empty<Object>,
                    std::negation<std::is_final<Object>>
                    >,
                    detail::StorageEmptyBaseClassOptimization<Object, Tag>,
                    detail::StorageByValue<Object, Tag>
            >;
}

#undef FOR_EACH_CVREF

QT_END_NAMESPACE

#endif //QFUNCTIONALTOOLS_IMPL_H
