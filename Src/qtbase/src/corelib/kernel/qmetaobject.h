//
// Created by Yujie Zhao on 2023/6/1.
//

#ifndef QMETAOBJECT_H
#define QMETAOBJECT_H

#include "qobjectdefs.h"
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class Q_CORE_EXPORT QMetaMethod
{
    friend struct QMetaObject;
    friend struct QMetaObjectPrivate;
    friend class QObject;
public:
    //moc中的data的形式
//signals: name, argc, parameters, tag, flags, initial metatype offsets
//         1,    1,    38,          2, 0x06,    2 /* Public */,
//         1,    0,    41,          2, 0x26,    4 /* Public | MethodCloned */,
//         3,    1,    42,          2, 0x06,    5 /* Public */,
    struct Data {
        enum { Size = 6 };

        uint name() const { return d[0]; }
        //参数个数
        uint argc() const { return d[1]; }
        //参数在metadata中的位置（d[2]是返回值类型的索引位置）
        uint parameters() const { return d[2]; }
        uint tag() const { return d[3]; }
        uint flags() const { return d[4]; }
        uint metaTypeOffset() const { return d[5]; }

        bool operator==(const Data &other) const { return d == other.d; }

        const uint *d;
    };

    enum MethodType {
        Method,
        Signal,
        Slot,
        Constructor
    };
    enum Attributes {
        Compatibility = 0x1,
        Cloned = 0x2,
        Scriptable = 0x4
    };
    enum Access {
        Private,
        Protected,
        Public
    };

    constexpr inline QMetaMethod() : mobj(nullptr), data({nullptr}) {}

    QByteArray methodSignature() const;
    MethodType methodType() const;
    QByteArray name() const;
    int returnType() const;
    QMetaType returnMetaType() const;
    int parameterCount() const;
    int parameterType(int index) const;
    QMetaType parameterMetaType(int index) const;
    //获取method在本QMetaObject中的索引
    int relativeMethodIndex() const;
    int methodIndex() const;
    int attributes() const;

    inline const QMetaObject *enclosingMetaObject() const { return mobj; }
    inline bool isValid() const { return mobj != nullptr; }

protected:
    friend bool operator==(const QMetaMethod &m1, const QMetaMethod &m2) noexcept
    { return m1.data == m2.data; }
    friend bool operator!=(const QMetaMethod &m1, const QMetaMethod &m2) noexcept
    { return !(m1 == m2); }

private:
    constexpr QMetaMethod(const QMetaObject *metaObject, const Data &data_)
        : mobj(metaObject), data(data_)
    {}

    static QMetaMethod fromRelativeMethodIndex(const QMetaObject *mobj, int index);
    static QMetaMethod fromRelativeConstructorIndex(const QMetaObject *mobj, int index);

protected:
    const QMetaObject *mobj;
    Data data;
};
Q_DECLARE_TYPEINFO(QMetaMethod, Q_RELOCATABLE_TYPE)

class Q_CORE_EXPORT QMetaEnum
{
    struct Data {
        enum {  Size = 5 };
        quint32 name() const { return d[0]; }
        quint32 alias() const { return d[1]; }
        quint32 flags() const { return d[2]; }
        quint32 keyCount() const { return static_cast<qint32 >(d[3]); }
        quint32 data() const { return d[4]; }
        int index(const QMetaObject *mobj) const;

        const uint *d;
    };
public:
    constexpr inline QMetaEnum()
        : mobj(nullptr), data( {nullptr} )
    {}

    const char *name() const;
    const char *enumName() const;
    QMetaType metaType() const;

    bool isFlag() const;
    bool isScoped() const;

    int keyCount() const;
    const char *key(int index) const;
    int value(int index) const;

    const char *scope() const;

    int keyToValue(const char *key, bool *ok = nullptr) const;
    const char *valueToKey(int value) const;
    int keysToValue(const char *keys, bool *ok = nullptr);
    QByteArray valueToKeys(int value) const;

    inline const QMetaObject *enclosingMetaObject() const { return mobj; }
    inline bool isValid() const { return name() != nullptr; }

    template <typename T>
    static QMetaEnum fromType()
    {
        static_assert(QtPrivate::IsQEnumHelper<T>::Value);
        const QMetaObject *metaObject = qt_getEnumMetaObject(T());
        const char *name = qt_getEnumName(T());
        return metaObject->enumerator(metaObject->indexOfEnumerator(name));
    }

private:
    QMetaEnum(const QMetaObject *mobj, int index);

    const QMetaObject *mobj;
    Data data;
    friend struct QMetaObject;
    friend struct QMetaObjectPrivate;
};


// QMetaProperty 在moc文件中
// properties: name, type, flags
//             11, QMetaType::Int, 0x00015103, uint(-1), 0,
class Q_CORE_EXPORT QMetaProperty
{
    friend struct QMetaObject;
    friend struct QMetaObjectPrivate;
public:
    const char *name() const;

    int userType() const { return typeId(); }
    int typeId() const { return metaType().id(); }
    int registerPropertyType() const { return typeId(); }
    QMetaType metaType() const;
    bool isWritable() const;
    bool isBindable() const;
    bool isResettable() const;
    bool isEnumType() const;

    int propertyIndex() const;

    QVariant read(const QObject *object) const;
    bool write(QObject *object, const QVariant &value) const;
    bool write(QObject *object, QVariant &&value) const;
    bool reset(QObject *object) const;

private:
    struct Data
    {
        enum { Size = 5 };
        uint name() const { return d[0]; }
        uint type() const { return d[1]; }
        uint flags() const { return d[2]; }
        uint notifyIndex() const { return d[3]; }
        uint revision() const { return d[4]; }

        int index(const QMetaObject *mobj) const;

        const uint *d;
    };

    QMetaProperty() : mobj(nullptr), data({nullptr}) {}
    QMetaProperty(const QMetaObject *mobj, int index);
    static Data getMetaPropertyData(const QMetaObject *mobj, int index);

private:
    const QMetaObject *mobj;
    Data data;
    QMetaEnum menum;
};

class Q_CORE_EXPORT QMetaClassInfo
{
    friend struct QMetaObject;
public:
    constexpr inline QMetaClassInfo() : mobj(nullptr), data({nullptr})
    {}
    const char *name() const;
    const char *value() const;
    inline const QMetaObject *enclosingMetaObject() const { return mobj; }

private:
    struct Data {
        enum { Size = 2 };

        uint name() const { return d[0]; }
        uint value() const { return d[1]; }

        const uint *d;
    };

private:
    const QMetaObject *mobj;
    Data data;
};

Q_DECLARE_TYPEINFO(QMetaClassInfo, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

#endif //QMETAOBJECT_H
