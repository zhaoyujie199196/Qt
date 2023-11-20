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
enum PropertyFlags {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
//    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
//    Alias = 0x00000010,
//    // Reserved for future usage = 0x00000020,
//    StdCppSet = 0x00000100,
//    Constant = 0x00000400,
//    Final = 0x00000800,
//    Designable = 0x00001000,
//    Scriptable = 0x00004000,
//    Stored = 0x00010000,
//    User = 0x00100000,
//    Required = 0x01000000,
//    Bindable = 0x02000000

};

enum MethodFlags {
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, // 全县的mask，最低两位

    MethodMethod = 0x00,  //方法
    MethodSignal = 0x04,  //信号
    MethodSlot = 0x08,    //槽函数
    MethodConstructor = 0x0c,
    MethodTypeMask = 0x0c,  //方法类型的mask，低三位～低四位

    MethodCompatibility = 0x10,   //兼容的
    MethodCloned = 0x20,
    MethodScriptable = 0x40,
    MethodRevisioned = 0x80,

    MethodIsConst = 0x100, // no use case for volatile so far
};

//zhaoyujie TODO 这些是什么意思？
enum MetaObjectFlag {
    DynamicMetaObject = 0x01,
    RequiresVariantMetaObject = 0x02,
    PropertyAccessInStaticMetaCall = 0x04
};
Q_DECLARE_FLAGS(MetaObjectFlags, MetaObjectFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(MetaObjectFlags)

//zhaoyujie TODO 这里是什么意思？
enum MetaDataFlags {
    IsUnresolvedType = 0x80000000,  //通过Q_DECLARE_TYPEINFO注册的类型，在moc文件中存储的是0x80000000 | 9
    TypeNameIndexMask = 0x7FFFFFFF,
    IsUnresolvedSignal = 0x70000000
};

//根据QMetaType的名字查找metaType的id
Q_CORE_EXPORT int qMetaTypeTypeInternal(const char *);

/*
 * 参数类型，只能使用QMetaType中注册过的类型
 * */
class QArgumentType
{
public:
    QArgumentType(int type) : _type(type) {}
    QArgumentType(const QByteArray &name)
        : _type(qMetaTypeTypeInternal(name.constData())), _name(name)
    {}
    QArgumentType() : _type(0) {}
    int type() const { return _type; }
    QByteArray name() const {
        if (_type && _name.isEmpty()) {
            const_cast<QArgumentType *>(this)->_name = QMetaType(_type).name();
        }
        return _name;
    }

    bool operator==(const QArgumentType &other) const {
        if (_type && other._type) {
            return _type == other._type;
        }
        else {
            return name() == other.name();
        }
    }

    bool operator!=(const QArgumentType &other) const {
        return !(*this == other);
    }

private:
    int _type;
    QByteArray _name;
};
Q_DECLARE_TYPEINFO(QArgumentType, Q_RELOCATABLE_TYPE);
typedef QVarLengthArray<QArgumentType, 10> QArgumentTypeArray;

class QMetaMethodPrivate;
struct QMetaObjectPrivate
{
//    enum { OutputRevision = 10 };
//    enum { IntsPerMethod = QMetaMethod::Data::Size };
//    enum { IntsPerEnum = QMetaEnum::Data::Size };
//    enum { IntsPerProperty = QMetaProperty::Data::Size };

    enum DisconnectType {
        DisconnectAll,
        DisconnectOne
    };

    int revision;
    int className;
    int classInfoCount, classInfoData;
    int methodCount, methodData;  //methodCount 顺序 signal, slot, invokable
    int propertyCount, propertyData;  //propertyCount: property的数量  propertyData: property信息开始的位置
    int enumeratorCount, enumeratorData;
    int constructorCount, constructorData;
    int flags;
    int signalCount;

    static inline const QMetaObjectPrivate *get(const QMetaObject *metaObject)
    {
        return reinterpret_cast<const QMetaObjectPrivate *>(metaObject->d.data);
    }

    static int originalClone(const QMetaObject *obj, int local_method_index);

    static QByteArray decodeMethodSignature(const char *signature, QArgumentTypeArray &types);
    //方法的索引
    static int indexOfMethod(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types);
    //查找信号的索引 baseObject要作为参数带出去
    static int indexOfSignalRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types);
    //查找槽函数的索引
    static int indexOfSlotRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types);
    //按名字查找方法  zhaoyujie TODO 这个模板的实现为什么可以放在cpp里
    template <int MethodType>
    static inline int indexOfMethodRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types);
    //方法与信息是否匹配
    static bool methodMatch(const QMetaObject *m, const QMetaMethod &method, const QByteArray &name, int argc, const QArgumentType *types);
    //连接
    static QObjectPrivate::Connection *connect(const QObject *sender, int signal_index, const QMetaObject *smeta,
                                               const QObject *receiver, int method_index_relative, const QMetaObject *rmeta = nullptr,
                                               int type = 0, int *types = nullptr);
    static bool disconnect(const QObject *sender, int signal_index, const QMetaObject *smeta,
                           const QObject *receiver, int method_index, void **slot,
                           DisconnectType type = DisconnectType::DisconnectAll);
    static inline bool disconnectHelper(QObjectPrivate::ConnectionData *connections, int signalindex,
                                        const QObject *receiver, int method_index, void **slot,
                                        QBasicMutex *senderMutex, DisconnectType = DisconnectAll);

    //获取信号
    static QMetaMethod signal(const QMetaObject *m, int signalIndex);
    static inline int signalOffset(const QMetaObject *m) {
        Q_ASSERT(m != nullptr);
        int offset = 0;
        for (m = m->d.superdata; m; m = m->d.superdata) {
            offset += reinterpret_cast<const QMetaObjectPrivate *>(m->d.data)->signalCount;
        }
        return offset;
    }

    //检测连接参数是否匹配
    static bool checkConnectArgs(int signalArgc, const QArgumentType *signalTypes,
                                 int methodArgc, const QArgumentType *methodTypes);
    static bool checkConnectArgs(const QMetaMethodPrivate *signal, const QMetaMethodPrivate *method);

    static void memberIndexes(const QObject *obj, const QMetaMethod &member, int *signalIndex, int *methodIndex);
};

static inline bool is_ident_char(char s)
{
    return ((s >= 'a' && s <= 'z')
            || (s >= 'A' && s <= 'Z')
            || (s >= '0' && s <= '9')
            || s == '_'
    );
}

static inline bool is_space(char s)
{
    return (s == ' ' || s == '\t');
}

QT_END_NAMESPACE

#endif //QMETAOBJECT_P_H
