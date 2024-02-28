//
// Created by Yujie Zhao on 2024/2/20.
//

#ifndef QOBJECT_IMPL_H
#define QOBJECT_IMPL_H

#ifndef QOBJECT_H
#error Do not include qobject_impl.h directly
#endif

QT_BEGIN_NAMESPACE

namespace QtPrivate {

    /*
     * 根据List<>生成元数据的id数组
     * */
    template<typename ArgList>
    struct TypesAreDeclaredMetaType { enum { Value = false }; };
    template <>
    struct TypesAreDeclaredMetaType<List<>> { enum { Value = true }; };

    template <typename Arg, typename... Tail>
    struct TypesAreDeclaredMetaType<List<Arg, Tail...>> {
        enum {  Value = QMetaTypeId2<Arg>::Defined && TypesAreDeclaredMetaType<List<Tail...>>::Value };
    };

    template <typename ArgList, bool Declared = TypesAreDeclaredMetaType<ArgList>::Value>
    struct ConnectionTypes {
        static const int *types() { return nullptr; }
    };
    template <>
    struct ConnectionTypes<List<>, true> {
        static const int *types() { return nullptr; }
    };
    template <typename... Args>
    struct ConnectionTypes<List<Args...>, true> {
        static const int *types() {
            static const int t[sizeof...(Args) + 1] = {
                (QtPrivate::QMetaTypeIdHelper<Args>::qt_metatype_id())..., 0
            };
            return t;
        }
    };
}

QT_END_NAMESPACE

#endif //QOBJECT_IMPL_H
