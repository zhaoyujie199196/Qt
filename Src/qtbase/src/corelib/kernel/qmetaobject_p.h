//
// Created by Yujie Zhao on 2023/10/23.
//

#ifndef QMETAOBJECT_P_H
#define QMETAOBJECT_P_H

#include <QtCore/qglobal.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QBasicMutex;
Q_CORE_EXPORT int qMetaTypeTypeInternal(const char *);

//static constexpr auto qt_meta_stringdata_CLASSNamespaceTestQObjectSCOPETestQObjectENDCLASS = QtMocHelpers::stringData(
//        "NamespaceTestQObject::TestQObject",
//        "sig1",
//        "int&",
//);

/*
 *  不是MetaType中的定义的类型，参数类型的字符串存放在stringData中，使用IsUnresolvedType作为标记
 * */

enum MetaDataFlags {
    IsUnresolvedType = 0x80000000,
    TypeNameIndexMask = 0x7FFFFFFF,
    IsUnresolvedSignal = 0x70000000, //TODO
};

enum EnumFlags {
    EnumIsFlag = 0x1,
    EnumIsScoped = 0x2
};

//TODO 这个枚举没明白是干嘛的
enum MetaObjectFlag {
    DynamicMetaObject = 0x01,
    RequiresVariantMetaObject = 0x02,
    PropertyAccessInStaticMetaCall = 0x04
};
Q_DECLARE_FLAGS(MetaObjectFlags, MetaObjectFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(MetaObjectFlags)

enum PropertyFlags {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
    Alias = 0x00000010,
    // Reserved for future usage = 0x00000020,
    StdCppSet = 0x00000100,
    Constant = 0x00000400,
    Final = 0x00000800,
    Designable = 0x00001000,
    Scriptable = 0x00004000,
    Stored = 0x00010000,
    User = 0x00100000,
    Required = 0x01000000,
    Bindable = 0x02000000
};

enum MethodFlags {
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, // mask

    MethodMethod = 0x00,
    MethodSignal = 0x04,
    MethodSlot = 0x08,
    MethodConstructor = 0x0c,
    MethodTypeMask = 0x0c,

    MethodCompatibility = 0x10,  //兼容性方法，方法已经不再推荐使用，但是仍然保留在库中以支持旧的代码
    MethodCloned = 0x20,     //克隆的方法。方法有默认参数，将克隆出几个版本
    MethodScriptable = 0x40,
    MethodRevisioned = 0x80,

    MethodIsConst = 0x100, // no use case for volatile so far
};

class QMetaMethodPrivate;
//参数类型
class QArgumentType
{
public:
    QArgumentType(int type)
        : _type(type)
    {}
    QArgumentType(const QByteArray &name)
        : _type(qMetaTypeTypeInternal(name.constData()))
        , _name(name)
    {}
    QArgumentType()
        : _type(0)
    {}

    int type() const { return _type; }

    QByteArray name() const
    {
        if (_type && _name.isEmpty()) {
            const_cast<QArgumentType *>(this)->_name = QMetaType(_type).name();
        }
        return _name;
    }

    bool operator==(const QArgumentType &other) const
    {
        if (_type && other._type)
            return _type == other._type;
        else
            return name() == other.name();
    }
    bool operator!=(const QArgumentType &other) const
    {
        if (_type && other._type)
            return _type != other._type;
        else
            return name() != other.name();
    }

private:
    int _type;
    QByteArray _name;
};
Q_DECLARE_TYPEINFO(QArgumentType, Q_RELOCATABLE_TYPE)
typedef QVarLengthArray<QArgumentType, 10> QArgumentTypeArray;

class QMetaMethodInvoker : public QMetaMethod
{
public:
};

//static const uint qt_meta_data_QObject[] = {
//
//        // content:
//        10,       // revision
//        0,       // classname
//        0,    0, // classinfo
//        4,   14, // methods
//        1,   50, // properties
//        0,    0, // enums/sets
//        2,   55, // constructors
//        0,       // flags
//        3,       // signalCount
//
//        // signals: name, argc, parameters, tag, flags, initial metatype offsets
//        1,    1,   38,    2, 0x06,    2 /* Public */,
//        1,    0,   41,    2, 0x26,    4 /* Public | MethodCloned */,
//        3,    1,   42,    2, 0x06,    5 /* Public */,
//
//        // slots: name, argc, parameters, tag, flags, initial metatype offsets
//        5,    0,   45,    2, 0x0a,    7 /* Public */,
//
//        // signals: parameters
//        QMetaType::Void, QMetaType::QObjectStar,    2,
//        QMetaType::Void,
//        QMetaType::Void, QMetaType::QString,    4,
//
//        // slots: parameters
//        QMetaType::Void,
//
//        // constructors: parameters
//        0x80000000 | 2, QMetaType::QObjectStar,    6,
//        0x80000000 | 2,
//
//        // properties: name, type, flags
//        4, QMetaType::QString, 0x02015103, uint(2), 0,
//
//        // constructors: name, argc, parameters, tag, flags, initial metatype offsets
//        0,    1,   46,    2, 0x0e,    8 /* Public */,
//        0,    0,   49,    2, 0x2e,    9 /* Public | MethodCloned */,
//
//        0        // eod
//};

//moc文件中的qt_meta_data_XXX信息
struct QMetaObjectPrivate
{
    enum DisconnectType {
        DisconnectAll,
        DisconnectOne
    };

    int revision;  //版本
    int className;  //类名在qt_meta_stringdata_XXX_t中的位置
    int classInfoCount, classInfoData;  //TODO
    int methodCount, methodData;      //方法数量，方法信息在qt_meta_data_XXX中的索引
    int propertyCount, propertyData;  //属性数量，属性信息在qt_meta_data_XXX中的索引
    int enumeratorCount, enumeratorData;    //枚举数量，枚举信息在qt_meta_data_XXX中的索引
    int constructorCount, constructorData;  //构造函数数量，构造函数信息在qt_meta_data_XXX中的索引
    int flags;    //TODO
    int signalCount;  //信号数量

    static inline const QMetaObjectPrivate *get(const QMetaObject *methodobject)
    {
        //QMethodObject->d.data存放的信息可以直接转换成QMetaObjectPrivate，数据排列一样
        return reinterpret_cast<const QMetaObjectPrivate *>(methodobject->d.data);
    }

    static int originalClone(const QMetaObject *obj, int local_method_index);

    //提取函数签名的函数名与参数类型
    static QByteArray decodeMethodSignature(const char *signature, QArgumentTypeArray &types);

    static int indexOfSignal(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types);
    static int indexOfSlot(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types);
    static int indexOfMethod(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types);
    static int indexOfConstructor(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types);

    static int indexOfSignalRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types);
    static int indexOfSlotRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types);
    template <int methodType>
    static inline int indexOfMethodRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types);

    Q_CORE_EXPORT static QMetaMethod signal(const QMetaObject *m, int signal_index);

    static void memberIndexes(const QObject *obj, const QMetaMethod &member, int *signalIndex, int *methodIndex);

    static bool methodMatch(const QMetaObject *m, const QMetaMethod &method, const QByteArray &name, int argc, const QArgumentType *types);

    static inline int signalOffset(const QMetaObject *m)
    {
        Q_ASSERT(m != nullptr);
        int offset = 0;
        for (m = m->d.superdata; m; m = m->d.superdata) {
            offset += reinterpret_cast<const QMetaObjectPrivate *>(m->d.data)->signalCount;
        }
        return offset;
    }

    static bool checkConnectArgs(int signalArgc, const QArgumentType *signalTypes,
                                 int methodArgc, const QArgumentType *methodTypes);
    static bool checkConnectArgs(const QMetaMethodPrivate *signal, const QMetaMethodPrivate *method);

    static QObjectPrivate::Connection *connect(const QObject *sender, int singal_index, const QMetaObject *smeta,
                                               const QObject *receiver, int method_index_relative, const QMetaObject *rmeta = nullptr,
                                               int type = 0, int *types = nullptr);

    static bool disconnect(const QObject *sender, int signal_index, const QMetaObject *smeta,
                           const QObject *receiver, int method_index, void **slot,
                           DisconnectType = DisconnectAll);

    static inline bool disconnectHelper(QObjectPrivate::ConnectionData *connections, int signalIndex,
                                        const QObject *receiver, int method_index, void **slot,
                                        QBasicMutex *senderMutex, DisconnectType = DisconnectAll);

    static int absoluteSignalCount(const QMetaObject *m);

};

QT_END_NAMESPACE

#endif //QMETAOBJECT_P_H
