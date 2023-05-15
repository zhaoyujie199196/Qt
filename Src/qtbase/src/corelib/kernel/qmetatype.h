//
// Created by Yujie Zhao on 2023/2/15.
//

#ifndef QMETATYPE_H
#define QMETATYPE_H

#include <QtCore/qglobal.h>
#include <QtCore/qchar.h>
#include <QtCore/qatomic.h>
#include <array>
#include <string>
#include <string_view>
#include "qmetatypenormalizer_p.h"

QT_BEGIN_NAMESPACE

class QByteArrayView;
//zhaoyujie TODO QCborSimpleType的作用是什么
enum class QCborSimpleType : quint8;

//template类型的前置声明？
template <typename T>
struct QMetaTypeId2;

template <typename T>
inline constexpr int qMetaTypeId();

//基础类型的一些参数，用在其他的宏里面注册
#define QT_FOR_EACH_STATIC_PRIMITIVE_TYPE(F) \
    F(Void, 43, void)                        \
    F(Bool, 1, bool)                         \
    F(Int, 2, int)                           \
    F(UInt, 3, uint)                         \
    F(LongLong, 4, qlonglong)                \
    F(ULongLong, 5, qulonglong)              \
    F(Double, 6, double)                     \
    F(Long, 32, long)                        \
    F(Short, 33, short)                      \
    F(Char, 34, char)                        \
    F(Char16, 56, char16_t)                  \
    F(Char32, 57, char32_t)                  \
    F(ULong, 35, ulong)                      \
    F(UShort, 36, ushort)                    \
    F(UChar, 37, uchar)                      \
    F(Float, 38, float)                      \
    F(SChar, 40, signed char)                \
    F(Nullptr, 51, std::nullptr_t)           \
    F(QCborSimpleType, 52, QCborSimpleType)  \

#define QT_FOR_EACH_STATIC_PRIMITIVE_POINTER(F) \
    F(VoidStr, 31, void*)                       \

#define QT_FOR_EACH_STATIC_CORE_CLASS(F) \
    F(QChar, 7, QChar)                   \


#define QT_FOR_EACH_STATIC_TYPE(F) \
    QT_FOR_EACH_STATIC_PRIMITIVE_TYPE(F) \
    QT_FOR_EACH_STATIC_PRIMITIVE_POINTER(F) \
    QT_FOR_EACH_STATIC_CORE_CLASS(F)     \

#define QT_DEFINE_METATYPE_ID(TypeName, Id, Name) \
    TypeName = Id,

struct QMetaObject;

namespace QtPrivate
{
    template <typename T, typename = void>
    struct BuiltinMetaType : std::integral_constant<int, 0>
    {
    };
    template <typename T>
    struct BuiltinMetaType<T, std::enable_if_t<QMetaTypeId2<T>::IsBuiltin>>
            : std::integral_constant<int, QMetaTypeId2<T>::MetaType>
    {
    };

    class QMetaTypeInterface
    {
    public:
        ushort revision; //Qt 6.0中版本号为0。如果字段增加，版本号可以添加
        ushort alignment;
        uint size;
//        uint flags;
        mutable QBasicAtomicInt typeId;

//        using MetaObjectFn = const QMetaObject *(*)(const QMetaTypeInterface *);
//        MetaObjectFn metaObjectFn;
        const char *name;
        using DefaultCtrFn = void (*)(const QMetaTypeInterface *, void *);
        DefaultCtrFn defaultCtr;  //不带参数的默认构造函数
        using CopyCtrFn = void (*)(const QMetaTypeInterface *, void *, const void *);
        CopyCtrFn copyCtr;
//        using MoveCtrFn = void (*)(const QMetaTypeInterface *, void *, void *);
//        MoveCtrFn moveCtr;
        using DtorFn = void (*)(const QMetaTypeInterface *, void *);
        DtorFn dtor;  //析构函数，调用～T()
//        using EqualsFn = bool (*)(const QMetaTypeInterface *, const void *, const void *);
//        EqualsFn equals;
//        using LessThanFn = bool (*)(const QMetaTypeInterface *, const void *, const void *);
//        LessThanFn lessThan;
//        using DebugStreamFn = void (*)(const QMetaTypeInterface *, QDebug &, const void *);
//        DebugStreamFn debugStream;
//        using DataStreamOutFn = void (*)(const QMetaTypeInterface *, QDataStream &, const void *);
//        DataStreamOutFn dataStreamOut;
//        using DataStreamInFn = void (*)(const QMetaTypeInterface *, QDataStream &, void *);
//        DataStreamInFn dataStreamIn;
//
        using LegacyRegisterOp = void (*)();
        LegacyRegisterOp legacyRegisterOp;
    };

    template <typename T>
    class QMetaTypeForType {
    public:
        static constexpr decltype(typenameHelper<T>()) name = typenameHelper<T>();

        //默认构造函数
        static constexpr QMetaTypeInterface::DefaultCtrFn getDefaultCtr() {
            //std::is_default_constructible_v 是否有无参数构造函数
            if (std::is_default_constructible_v<T>) {
                return [](const QMetaTypeInterface *, void *address)->void {
                    //函数是void *类型，在address上原地构造
                    new(address) T();
                };
            }
            else {
                return nullptr;
            }
        }

        //拷贝 直接使用拷贝构造函数
        static constexpr QMetaTypeInterface::CopyCtrFn getCopyCtr()
        {
            if constexpr(std::is_copy_constructible_v<T>) {
                return [](const QMetaTypeInterface *, void *address, const void *other){
                    new (address) T(*reinterpret_cast<const T*>(other));
                };
            }
            else {
                return nullptr;
            }
        }

        //析构函数函数
        static constexpr QMetaTypeInterface::DtorFn getDtorCtr() {
            //如果是可以析构函数的，并且定义了析构函数，可以调用～T()
            if constexpr (std::is_destructible_v<T> && !std::is_trivially_destructible_v<T>) {
                return [](const QMetaTypeInterface *, void *address) {
                    reinterpret_cast<T *>(address)->~T();
                };
            }
            else {
                return nullptr;
            }
        }

        static constexpr QMetaTypeInterface::LegacyRegisterOp getLegacyRegister() {
            if constexpr (QMetaTypeId2<T>::Defined && !QMetaTypeId2<T>::IsBuiltIn) {
                return []() {
                    QMetaTypeId2<T>::qt_metatype_id();
                };
            }
            else {
                return nullptr;
            }
        }

        static constexpr const char *getName() {
            if constexpr (bool(QMetaTypeId2<T>::IsBuiltIn)) {
                return QMetaTypeId2<T>::nameAsArray.data();
            }
            else {
                return name.data();
            }
        }
    };

    template<typename T>
    struct QMetaTypeInterfaceWrapper {
        static constexpr QMetaTypeInterface metaType = {
                /*.revision=*/0,
                /*.alignment=*/alignof(T),
                /*.size=*/sizeof(T),
//                /*.flags=*/QMetaTypeTypeFlags<T>::Flags,
                /*.typeId=*/BuiltinMetaType<T>::value,
//                /*.metaObjectFn=*/ MetaObjectForType<T>::metaObjectFunction,
                /*.name=*/ QMetaTypeForType<T>::getName(),
                /*.defaultCtr=*/ QMetaTypeForType<T>::getDefaultCtr(),
                /*.copyCtr=*/ QMetaTypeForType<T>::getCopyCtr(),
//                /*.moveCtr=*/ QMetaTypeForType<T>::getMoveCtr(),
                /*.dtor=*/ QMetaTypeForType<T>::getDtorCtr(),
//                /*.equals=*/ QEqualityOperatorForType<T>::equals,
//                /*.lessThan=*/ QLessThanOperatorForType<T>::lessThan,
//                /*.debugStream=*/ QDebugStreamOperatorForType<T>::debugStream,
//                /*.dataStreamOut=*/ QDataStreamOperatorForType<T>::dataStreamOut,
//                /*.dataStreamIn=*/ QDataStreamOperatorForType<T>::dataStreamIn,
                /*.legacyRegisterOp=*/ QMetaTypeForType<T>::getLegacyRegister()
        };
    };

    template <>
    class QMetaTypeInterfaceWrapper<void>{
    public:
        static constexpr QMetaTypeInterface metaType = {
                /*.revision=*/ 0,
                /*.alignment=*/ 0,
                /*.size=*/ 0,
//                /*.flags=*/ 0,
                /*.typeId=*/ BuiltinMetaType<void>::value,
//                /*.metaObjectFn=*/ nullptr,
                /*.name=*/ "void",
                /*.defaultCtr=*/ nullptr,
                /*.copyCtr=*/ nullptr,
//                /*.moveCtr=*/ nullptr,
                /*.dtor=*/ nullptr,
//                /*.equals=*/ nullptr,
//                /*.lessThan=*/ nullptr,
//                /*.debugStream=*/ nullptr,
//                /*.dataStreamOut=*/ nullptr,
//                /*.dataStreamIn=*/ nullptr,
                /*.legacyRegisterOp=*/ nullptr
        };
    };

    template <typename T>
    constexpr const QMetaTypeInterface *qMetaTypeInterfaceForType()
    {
        using Ty = std::remove_cv_t<std::remove_reference_t<T>>;
        return &QMetaTypeInterfaceWrapper<Ty>::metaType;
    }
};

template <typename T>
struct QMetaTypeIdQObject
{
    enum {
        Defined = 0
    };
};

template <typename T>
struct QMetaTypeId : public QMetaTypeIdQObject<T>
{
};

template <typename T>
struct QMetaTypeId2
{
    using NameAsArrayType = void;
    enum { Defined = QMetaTypeId<T>::Defined, IsBuiltIn = false };
    static inline constexpr int qt_metatype_id() {return QMetaTypeId<T>::qt_metatype_id();}
};

class QMetaType {
public:
    enum Type {
        QT_FOR_EACH_STATIC_TYPE(QT_DEFINE_METATYPE_ID)

        FirstCoreType = Bool,
        LastCoreType = 58,  //zhaoyujie TODO
        QReal = sizeof(qreal) == sizeof(double) ? Double : Float,
        UnknownType = 0,
        User = 65536
    };

    explicit QMetaType(int typeId);
    explicit constexpr QMetaType(const QtPrivate::QMetaTypeInterface *d) : d_ptr(d) {}
    constexpr QMetaType() = default;

    static void registerNormalizedTypedef(const std::string &normalizedTypeName, QMetaType metaType);

    template <typename T>
    static constexpr QMetaType fromType() {
        return QMetaType(QtPrivate::qMetaTypeInterfaceForType<T>());
    }

    void *create(const void *copy = nullptr) const;
    void destory(void *data) const;
    void *construct(void *address, const void *copy = nullptr) const;

    int id(int = 0) const {
        if (d_ptr) {
            if (int id = d_ptr->typeId.loadRelaxed()) {
                return id;
            }
            return idHelper();
        }
        return 0;
    }

    inline const char *name() const { return d_ptr ? d_ptr->name : nullptr; }
    inline bool valid() const {return d_ptr;}
    inline bool isRegistered() const {return d_ptr;}

private:
    int idHelper() const;

private:
    const QtPrivate::QMetaTypeInterface *d_ptr = nullptr;
};

//特化模板，const &的的MetaType与基础类型保持一支
template <typename T>
struct QMetaTypeId2<const T&> : QMetaTypeId2<T> {
};

template <typename T>
struct QMetaTypeId2<T &> : QMetaTypeId2<T> {
    enum {Defined = 0 }; //通过traits技巧用以判断是否被定义
};

template <typename T>
int qRegisterNormalizedMetaType(const char *normalizedTypeName)
{
    const QMetaType metaType = QMetaType::fromType<T>();
    const int id = metaType.id();
//    QtPrivate::SequentialContainerTransformationHelper<T>::registerConverter();
//    QtPrivate::SequentialContainerTransformationhelper<T>::registerMutableView();
//    QtPrivate::AssociativeContainerTransformationHelper<T>::registerConverter();
//    QtPrivate::AssociativeContainerTransformationHelper<T>::registerMutableView();
//    QtPrivate::MetaTypePairHelper<T>::registerConverter();
//    QtPrivate::MetaTypeSmartPointerHelper<T>::registerConverter();
    if (normalizedTypeName != metaType.name()) {
        QMetaType::registerNormalizedTypedef(std::string(normalizedTypeName), metaType);
    }

    return id;
}


#ifndef Q_MOC_RUN
#define Q_DECLARE_METATYPE(TYPE) Q_DECLARE_METATYPE_IMPL(TYPE)
#define Q_DECLARE_METATYPE_IMPL(TYPE) \
    QT_BEGIN_NAMESPACE                \
    template <>                       \
    struct QMetaTypeId<TYPE>          \
    {                                 \
        enum{ Defined = 1 };          \
        static int qt_metatype_id()   \
        {                             \
            static QBasicAtomicInt metatype_id = Q_BASIC_ATOMIC_INITIALIZER(0); \
            if (const int id = metatype_id.loadAcquire()) {                     \
                return id;                          \
            }                         \
            const auto arr = QtPrivate::typenameHelper<TYPE>();                 \
            auto name = arr.data();   \
            if (QByteArrayView(name) == (#TYPE)) {                              \
                const int id = qRegisterNormalizedMetaType<TYPE>(name);         \
                metatype_id.storeRelease(id);                                   \
                return id; \
            }                         \
            else {                    \
                assert(false);        \
                return 0; \
            } \
        } \
    }; \
    QT_END_NAMESPACE                  \

#endif //Q_MOC_RUN

//注册builtin类型
#define Q_DECLARE_BUILTIN_METATYPE(TYPE, METATYPEID, NAME) \
    QT_BEGIN_NAMESPACE                                     \
    template<> struct QMetaTypeId2<NAME>                   \
    {                                                      \
        using NameAsArrayType = std::array<char, sizeof(#NAME)>; \
        enum { Defined = 1, IsBuiltIn = true, MetaType = METATYPEID }; \
        static inline constexpr int qt_metatype_id() {return METATYPEID; } \
        static constexpr NameAsArrayType nameAsArray = { #NAME };\
    };\
    QT_END_NAMESPACE


template <typename T>
inline constexpr int qMetaTypeId()
{
    if constexpr(bool(QMetaTypeId2<T>::IsBuiltIn)) {
        return QMetaTypeId2<T>::MetaType;
    }
    else {
        return QMetaType::fromType<T>().id();
    }
}

#undef QT_DEFINE_METATYPE_ID

QT_END_NAMESPACE

QT_FOR_EACH_STATIC_TYPE(Q_DECLARE_BUILTIN_METATYPE)


#endif //QMETATYPE_H
