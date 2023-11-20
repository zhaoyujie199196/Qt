////
//// Created by Yujie Zhao on 2023/2/15.
////

#include "qmetatype.h"
#include "qmetatype_p.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <QtCore/qreadwritelock.h>
#include <QtCore/qvariant.h>
#include <QtCore/qmap.h>
#include <QtCore/qlist.h>
#include <QtCore/qreadwritelock.h>
#include <QtCore/qglobalstatic.h>
#include <QtCore/qlogging.h>
#include <thread>

QT_BEGIN_NAMESPACE

#define QT_ADD_STATIC_METATYPE(MetaTypeName, MetaTypeId, RealName) \
    { #RealName, sizeof(#RealName) - 1, MetaTypeId },

#define QT_ADD_STATIC_METATYPE_ALIASES_ITER(MetaTypeName, MetaTypeId, AliasingName, RealNameStr) \
    { RealNameStr, sizeof(RealNameStr) - 1, QMetaType::MetaTypeName },

static const struct {
    const char *typeName;
    int typeNameLength;
    int type;
} types[] = {
    QT_FOR_EACH_STATIC_TYPE(QT_ADD_STATIC_METATYPE)
    QT_FOR_EACH_STATIC_ALIAS_TYPE(QT_ADD_STATIC_METATYPE_ALIASES_ITER)
    QT_ADD_STATIC_METATYPE(_, QMetaTypeId2<qreal>::MetaType, qreal)
    { nullptr, 0, QMetaType::UnknownType }
};


//注册方法的模板容器类
struct QMetaTypeCustomRegistry
{
    QReadWriteLock lock;
    std::map<QByteArray, const QtPrivate::QMetaTypeInterface *> aliases;
    std::vector<const QtPrivate::QMetaTypeInterface *> registry;
    int firstEmpty = 0;

    //zhaoyujie TODO
    static QMetaTypeCustomRegistry* instance() {
        static QMetaTypeCustomRegistry s_instance;
        return &s_instance;
    }

    int registerCustomType(const QtPrivate::QMetaTypeInterface *ti) {
        QWriteLocker l();
        if (ti->typeId) {
            return ti->typeId;
        }
        QByteArray name(ti->name);
        auto ti2It = aliases.find(name);
        if(ti2It != aliases.end()) {;
            ti->typeId.storeRelaxed(ti2It->second->typeId.loadRelaxed());
            return ti2It->second->typeId;
        }
        else {
            aliases[name] = ti;
            int size = registry.size();
            while (firstEmpty < size && registry[firstEmpty]) {
                ++firstEmpty;
            }
            if (firstEmpty < size) {
                registry[firstEmpty] = ti;
                ++firstEmpty;
            }
            else {
                registry.push_back(ti);
                firstEmpty = registry.size();
            }
            ti->typeId = firstEmpty + QMetaType::User;
        }
        if (ti->legacyRegisterOp) {
            ti->legacyRegisterOp();
        }
        return ti->typeId;
    }

    const QtPrivate::QMetaTypeInterface *getCustomType(int id)
    {
        QReadLocker locker;
        return registry[id - QMetaType::User - 1];
    }
};

Q_GLOBAL_STATIC(QMetaTypeCustomRegistry, customTypeRegistry)

//注册转换方法
template <typename T, typename Key>
class QMetaTypeFunctionRegistry
{
public:
    ~QMetaTypeFunctionRegistry()
    {
        map.clear();
    }

    bool contains(Key k) const
    {
        return map.find(k) != map.end();
    }

    bool insertIfNotContains(Key key, const T &f)
    {
        if (contains(key)) {
            return false;
        }
        map.insert(key, f);
        return true;
    }

    const T *function(Key key) const
    {
        auto it = map.find(key);
        return it == map.end() ? nullptr : std::addressof(it->second);
    }

    void remove(int from, int to)
    {
        const Key k(from, to);
        auto it = map.find(k);
        if (it != map.end()) {
            map.erase(it);
        }
    }

private:
    //zhaoyujie TODO mutex
    std::map<Key, T> map;
};
typedef QMetaTypeFunctionRegistry<QMetaType::ConverterFunction, QPair<int, int> > QMetaTypeConverterRegistry;
Q_GLOBAL_STATIC(QMetaTypeConverterRegistry, customTypesConversionRegistry)

//static 只对正在编译的源代码文件
//const 表示不能修改里面的成员
static const struct QMetaTypeModuleHelper {
    const QtPrivate::QMetaTypeInterface *interfaceForType(int type) const {
        switch(type) {
            QT_FOR_EACH_STATIC_PRIMITIVE_TYPE(QT_METATYPE_CONVERT_ID_TO_TYPE)
            QT_FOR_EACH_STATIC_CORE_CLASS(QT_METATYPE_CONVERT_ID_TO_TYPE)
        default:
            return nullptr;
        }
    }

    static constexpr auto makePair(int from, int to) ->quint64 {
        return (quint64(from) << 32) + quint64 (to);
    }

    bool convert(const void *from, int fromTypeId, void *to, int toTypeId) const {
        Q_ASSERT(fromTypeId != toTypeId);
        //两个空指针可以互相转化，两个非空指针也可以互相转化
        bool onlyCheck = from == nullptr && to == nullptr;
        Q_ASSERT(onlyCheck || (bool(from) && bool(to)));

        using Char = char;
        using SChar = signed char;
        using UChar = unsigned char;
        using Short = short;
        using UShort = unsigned short;
        using Int = int;
        using UInt = unsigned int;
        using Long = long;
        using LongLong = qlonglong ;
        using ULong = unsigned long;
        using ULongLong = qulonglong;
        using Float = float;
        using Double = double;
        using Bool = bool;
        using Nullptr = std::nullptr_t;

#define QMETATYPE_CONVERTER_ASSIGN_DOUBLE(To, From) \
    QMETATYPE_CONVERTER(To, From, result = double(source); return true)

#define QMETATYPE_CONVERTER_ASSIGN_NUMBER(To, From) \
    QMetaType_CONVERTER(To, From, result = To::number(source); return true)

#define CONVERT_CBOR_AND_JSON(To)

#define INTEGRAL_CONVERTER(To) \
    QMETATYPE_CONVERTER_ASSIGN(To, Char); \
    QMETATYPE_CONVERTER_ASSIGN(To, UChar);\
    QMETATYPE_CONVERTER_ASSIGN(To, SChar);\
    QMETATYPE_CONVERTER_ASSIGN(To, Short);\
    QMETATYPE_CONVERTER_ASSIGN(To, UShort); \
    QMETATYPE_CONVERTER_ASSIGN(To, Int);  \
    QMETATYPE_CONVERTER_ASSIGN(To, UInt); \
    QMETATYPE_CONVERTER_ASSIGN(To, Long); \
    QMETATYPE_CONVERTER_ASSIGN(To, ULong);\
    QMETATYPE_CONVERTER_ASSIGN(To, LongLong); \
    QMETATYPE_CONVERTER_ASSIGN(To, ULongLong);\
    QMETATYPE_CONVERTER(To, Float, result = qRound64(source); return true;); \
    QMETATYPE_CONVERTER(To, Double, result = qRound64(source); return true;);\
    QMETATYPE_CONVERTER(To, QChar, result = source.unicode(); return true;); \
    QMETATYPE_CONVERTER(To, QString, Q_ASSERT(false); return false; );       \
    QMETATYPE_CONVERTER(To, QByteArray, Q_ASSERT(false); return false;);     \
    CONVERT_CBOR_AND_JSON(To)


        switch(makePair(toTypeId, fromTypeId)) {
            INTEGRAL_CONVERTER(Bool);
            INTEGRAL_CONVERTER(Char);
            INTEGRAL_CONVERTER(UChar);
            INTEGRAL_CONVERTER(SChar);
            INTEGRAL_CONVERTER(Short);
            INTEGRAL_CONVERTER(UShort);
            INTEGRAL_CONVERTER(Int);
            INTEGRAL_CONVERTER(UInt);
            INTEGRAL_CONVERTER(Long);
            INTEGRAL_CONVERTER(ULong);
            INTEGRAL_CONVERTER(LongLong);

            default:
                Q_ASSERT(false);
        }
        return false;
    }
};

QMetaTypeModuleHelper metatypeHelper;

static const QMetaTypeModuleHelper *qMetaTypeCoreHelper = &metatypeHelper;

static const QMetaTypeModuleHelper *qModuleHelperForType(int type) {
    if (type <= QMetaType::LastCoreType) {
        return qMetaTypeCoreHelper;  //内核数据
    }
//    if (type >= QMetaType::FirstGuiType && type <= QMetaType::LastGuiType) {
//        return qMetaTypeGuiHelper;
//         Q_ASSERT(false);
//         return nullptr;
//    }
//    if (type >= QMetaType::FirstWidgetsType && type <= QMetaType::LastWidgetsType) {
//        Q_ASSERT(false);
//        return nullptr;
//    }
    return nullptr;
}

static bool canConvertMetaObject(QMetaType fromType, QMetaType toType)
{
    //zhaoyujie TODO
    return false;
}

static const QtPrivate::QMetaTypeInterface *interfaceForType(int typeId) {
    const QtPrivate::QMetaTypeInterface *interface = nullptr;
    if (typeId >= QMetaType::User) {  //用户自己注册的类型
        auto reg = QMetaTypeCustomRegistry::instance();
        interface = reg->getCustomType(typeId);
    }
    else {
        auto moduleHelper = qModuleHelperForType(typeId);
        if (moduleHelper) {
            interface = moduleHelper->interfaceForType(typeId);
        }
    }
    if (!interface && typeId != QMetaType::UnknownType) {
        qWarning("Trying to construct an instance of an invalid type, type id: %i", typeId);
    }
    return interface;
}

QMetaType::QMetaType(int typeId)
    : QMetaType(interfaceForType(typeId))
{

}

//创建数据，使用默认构造
void *QMetaType::create(const void *copy) const {
    if (d_ptr && (copy ? !!d_ptr->copyCtr : !!d_ptr->defaultCtr)) {
        //定义了默认对齐的宏，new采用的是默认对齐的方案
        //在有些系统中，alignof(T)可能会大于__STDCPP_DEFAULT_NEW_ALIGNMENT__，如果不对齐，可能会产生一些性能方面的损耗
#ifdef __STDCPP_DEFAULT_NEW_ALIGNMENT__
        //operator new只分配内存，不会嗲用构造函数
        //std::align_val_t是枚举类型
        void *address = d_ptr->alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__ ?
                operator new(d_ptr->size, std::align_val_t(d_ptr->alignment)) :
                operator new(d_ptr->size);
#else
        void *address = operator new(d_ptr->size);
#endif
        return construct(address, copy);
    }
    else {
        Q_ASSERT(d_ptr && d_ptr->defaultCtr);
        return nullptr;
    }
}

void QMetaType::destory(void *data) const {
    //operator new只负责分配内存，不会调用构造函数
    //operator delete只释放内存，不会调用析构函数
    if (d_ptr) {
        //先尝试调用析构函数，然后调用delete释放data的内存
        if (d_ptr->dtor) {
            d_ptr->dtor(d_ptr, data);
        }
        if (d_ptr->alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            operator delete(data, std::align_val_t(d_ptr->alignment));
        }
        else {
            operator delete(data);
        }
    }
}

void *QMetaType::construct(void *address, const void *copy) const {
    if (!address) {
        return nullptr;
    }
    if (d_ptr) {
        if (copy) {
            d_ptr->copyCtr(d_ptr, address, copy);
            return address;
        }
        else if (!copy && d_ptr->defaultCtr){
            d_ptr->defaultCtr(d_ptr, address);
            return address;
        }
    }
    Q_ASSERT(false);
    return nullptr;
}

void QMetaType::destruct(void *data) const {
    if (!data) {
        return;
    }
    if (d_ptr && d_ptr->dtor) {
        d_ptr->dtor(d_ptr, data);
        return;
    }
}

bool QMetaType::equals(const void *lhs, const void *rhs) const {
    if (!lhs || !rhs) {
        return false;
    }
    if (d_ptr) {
        if (d_ptr->flags & QMetaType::IsPointer) {
            return *reinterpret_cast<const void * const *>(lhs) == *reinterpret_cast<const void * const *>(rhs);
        }
        if (d_ptr->equals) {
            return d_ptr->equals(d_ptr, lhs, rhs);
        }
        if (d_ptr->lessThan && !d_ptr->lessThan(d_ptr, lhs, rhs) && !d_ptr->lessThan(d_ptr, rhs, lhs)) {
            return true;
        }
    }
    return false;
}

int QMetaType::idHelper() const {
    Q_ASSERT(d_ptr);
    auto reg = QMetaTypeCustomRegistry::instance();
    if (reg) {
        return reg->registerCustomType(d_ptr);
    }
    return 0;
}

void QMetaType::registerNormalizedTypedef(const class QByteArray &normalizedTypeName, QMetaType metaType)
{
    if (!metaType.valid()) {
        return;
    }
    auto reg = QMetaTypeCustomRegistry::instance();
    if (reg) {
        QWriteLocker locker;
        auto it = reg->aliases.find(normalizedTypeName);
        if (it != reg->aliases.end()) {
            return;
        }
        reg->aliases[normalizedTypeName] = metaType.d_ptr;
    }
}

bool QMetaType::convert(QMetaType fromType, const void *from, QMetaType toType, void *to)
{
    if (!fromType.isValid() || !toType.isValid()) {
        return false;
    }
    if (fromType == toType) {
        //拷贝数据，先析构原数据，再执行拷贝操作
        fromType.destruct(to);
        fromType.construct(to, from);
        return true;
    }

    int fromTypeId = fromType.id();
    int toTypeId = toType.id();

    auto moduleHelper = qModuleHelperForType(qMax(fromTypeId, toTypeId));
    if (moduleHelper) {
        if (moduleHelper->convert(from, fromTypeId, to, toTypeId)) {
            return true;
        }
    }
//    const QMetaType::ConvertFunction * const f = customTypesConversionRegistry()->function(qMakePair(fromType, toTypeId));
//    if (f) {
//        return (*f)(from, to);
//    }
//    if (fromType.flags() & QMetaType::IsEnumeration) {
//        return convertFromEnum(fromType, from, toType, to);
//    }
//    if (toType.flags() & QMetaType::IsEnumeration) {
//        return convertToEnum(fromType, from, toType, tp);
//    }
//    if (toTypeId == Nullptr) {
//        *static_cast<std::nullptr_t *>(to) = nullptr;
//        if (fromType.flags() & QMetaType::IsPointer) {
//            if (from == nullptr) {
//                return true;
//            }
//        }
//    }

    Q_ASSERT(false);
    return false;
}

bool QMetaType::canConvert(QMetaType fromType, QMetaType toType)
{
    int fromTypeId = fromType.id();
    int toTypeId = toType.id();
    if (fromTypeId == UnknownType || toTypeId == UnknownType) {
        return false;
    }
    if (fromTypeId == toTypeId) {
        return true;
    }

    if (auto moduleHelper = qModuleHelperForType(qMax(fromTypeId, toTypeId))) {
        if (moduleHelper->convert(nullptr, fromTypeId, nullptr, toTypeId)) {
            return true;
        }
    }

    const ConverterFunction * const f = customTypesConversionRegistry()->function(qMakePair(fromTypeId, toTypeId));
    if (f) {
        return true;
    }

    //zhaoyujie TODO
//    if (toTypeId == qMetaTypeId<QSequentialIterable>())
//        return canConvertToSequentialIterable(fromType);
//
//    if (toTypeId == qMetaTypeId<QAssociativeIterable>())
//        return canConvertToAssociativeIterable(fromType);
//
//    if (toTypeId == QVariantList
//        && canConvert(fromType, QMetaType::fromType<QSequentialIterable>())) {
//        return true;
//    }
//
//    if ((toTypeId == QVariantHash || toTypeId == QVariantMap)
//        && canConvert(fromType, QMetaType::fromType<QAssociativeIterable>())) {
//        return true;
//    }

//    if (toTypeId == QVariantPair && hasRegisteredConverterFunction(
//            fromType, QMetaType::fromType<QtMetaTypePrivate::QPairVariantInterfaceImpl>())) {
//        return true;
//    }

    if (fromType.flags() & IsEnumeration) {  //枚举可以转换成字符串和数值
        if (toTypeId == QString || toTypeId == QByteArray) {
            return true;
        }
        return canConvert(QMetaType(LongLong), toType);
    }
    if (toType.flags() & IsEnumeration) {
        if (fromTypeId == QString || fromTypeId == QByteArray) {
            return true;
        }
        return canConvert(fromType, QMetaType(LongLong));
    }
    //指针转换成空指针
    if (toTypeId == Nullptr && fromType.flags() & IsPointer) {
        return true;
    }
    //元对象间可以互相转换
    if (canConvertMetaObject(fromType, toType)) {
        return true;
    }
    return false;
}

bool QMetaType::view(QMetaType fromType, void *from, QMetaType toType, void *to)
{
    Q_ASSERT(false);
    return false;
}

bool QMetaType::canView(QMetaType fromType, QMetaType toType)
{
    Q_ASSERT(false);
    return false;
//    int fromTypeId = fromType.id();
//    int toTypeId = toType.id();
//    if (fromTypeId == UnknownType || toTypeId == UnknownType) {
//        return false;
//    }
//    const mutableViewFunction *const f = customTypesMutableViewRegistry()->function(qMakePair(fromTypeId, toTypeId));
//    if (f) {
//        return true;
//    }
//    if (toTypeId == qMetaTypeId<QSequentialIterable>())
//        return canImplicitlyViewAsSequentialIterable(fromType);
//
//    if (toTypeId == qMetaTypeId<QAssociativeIterable>())
//        return canImplicitlyViewAsAssociativeIterable(fromType);
//
//    if (canConvertMetaObject(fromType, toType))
//        return true;
    return false;
}

static inline int qMetaTypeStaticType(const char *typeName, int length)
{
    int i = 0;
    while (types[i].typeName && ((length != types[i].typeNameLength)
                                 || memcmp(typeName, types[i].typeName, length)))
    {
        ++i;
    }
    return types[i].type;
}

static int qMetaTypeCustomType_unlocked(const char *typeName, int length)
{
    if (auto reg = customTypeRegistry()) {
        Q_ASSERT(!reg->lock.tryLockForWrite());
        auto it = reg->aliases.find(QByteArray(typeName, length));
        if (it != reg->aliases.end()) {
            return it->second->typeId;
        }
    }
    return QMetaType::UnknownType;
}

template <bool tryNormalizedType>
static inline int qMetaTypeTypeImpl(const char *typeName, int length)
{
    if (!length) {
        return QMetaType::UnknownType;
    }
    int type = qMetaTypeStaticType(typeName, length);
    if (type == QMetaType::UnknownType) {
        QReadLocker locker(&customTypeRegistry()->lock);
        type = qMetaTypeCustomType_unlocked(typeName, length);
        if ((type == QMetaType::UnknownType) && tryNormalizedType) {
            const QByteArray normalizedTypeName = QMetaObject::normalizedType(typeName);
            type = qMetaTypeStaticType(normalizedTypeName.constData(),
                                       normalizedTypeName.size());
            if (type == QMetaType::UnknownType) {
                type = qMetaTypeCustomType_unlocked(normalizedTypeName.constData(),
                                                    normalizedTypeName.size());
            }
        }
    }
    return type;
}

//根据typeName获取metaTypeId
Q_CORE_EXPORT int qMetaTypeTypeInternal(const char *typeName)
{
    return qMetaTypeTypeImpl<false>(typeName, int(qstrlen(typeName)));
}

QMetaType QMetaType::fromName(QByteArrayView typeName)
{
    return QMetaType(qMetaTypeTypeImpl<true>(typeName.data(), typeName.size()));
}

bool QMetaType::isRegistered(int type)
{
    return QMetaType(type).isRegistered();
}


QT_END_NAMESPACE
