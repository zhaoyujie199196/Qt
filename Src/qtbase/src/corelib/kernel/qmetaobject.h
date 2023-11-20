//
// Created by Yujie Zhao on 2023/6/1.
//

#ifndef QMETAOBJECT_H
#define QMETAOBJECT_H

#include "qobjectdefs.h"

QT_BEGIN_NAMESPACE

class QMetaObject;
class QObject;

class QByteArray;

class QMetaEnum
{
public:
};

//元属性
class QMetaProperty
{
    friend struct QMetaObject;
    friend struct QMetaObjectPrivate;
public:
    struct Data {  //moc文件中property在meta_data信息中的位置
        enum {
            Size = 5,  //一个Property占5位
        };
        uint name() const { return d[0]; }
        uint type() const { return d[1]; }   //property只允许接受一个参数
        uint flags() const { return d[2]; }
        uint notifyIndex() const { return d[3]; }
        uint revision() const { return d[4]; }

        int index(const QMetaObject *mobj) const;

        const uint *d;
    };

    QMetaProperty()
        : mobj(nullptr), data( {nullptr} )
    {
    }
    QMetaProperty(const QMetaObject *mobj, int index);
    //获取moc文件中定义的元信息中表示当前property的一段
    static Data getMetaPropertyData(const QMetaObject *mobj, int index);

    bool isReadable() const;
    bool isWriteable() const;

    //读取obj中的属性
    QVariant read(const QObject *obj) const;
    //写obj中的属性
    bool write(QObject *obj, const QVariant &value) const;

public:
    const QMetaObject *mobj;
    Data data;
//    QMetaEnum menum;

};

/*
 * 元方法
 * 元方法包括signal，slot，invoke方法
 * 排列顺序：signal slot invoke
 * */
class QMetaMethod
{
    friend class QMetaMethodPrivate;
    friend struct QMetaObject;
    friend struct QMetaObjectPrivate;
    friend class QObject;

    struct Data {
        enum { Size = 6 };
        uint name() const { return d[0]; }  //method名称索引
        uint argc() const { return d[1]; }  //参数个数
        uint parameters() const { return d[2]; }  //参数类型索引
        uint tag() const { return d[3]; }
        uint flags() const { return d[4]; }
        uint metaTypeOffset() const { return d[5]; } //zhaoyujie TODO 啥意思？
        bool operator==(const Data &other) const { return d == other.d; }

        const uint *d;  //method信息的起始指针
    };

public:
    enum MethodType {
        Method,  //zhaoyujie TODO
        Signal,  //信号函数
        Slot,    //槽函数
        Constructor  //构造函数
    };

    constexpr inline QMetaMethod() : mobj(nullptr), data({nullptr}) {}

    QByteArray methodSignature() const;
    inline bool isValid() const { return mobj != nullptr; }
    MethodType methodType() const;
    int relativeMethodIndex() const;
    int parameterCount() const;
    int methodIndex() const;

    inline const QMetaObject *enclosingMetaObject() const { return mobj; }

private:
    friend bool operator==(const QMetaMethod &m1, const QMetaMethod &m2) noexcept
    { return m1.data == m2.data; }
    friend bool operator!=(const QMetaMethod &m1, const QMetaMethod &m2) noexcept
    { return !(m1 == m2); }

    static QMetaMethod fromRelativeMethodIndex(const QMetaObject *mobj, int index);

private:
    const QMetaObject *mobj;
    Data data;
};
Q_DECLARE_TYPEINFO(QMetaMethod, Q_RELOCATABLE_TYPE);

//类的元信息
class QMetaClassInfo
{

};
Q_DECLARE_TYPEINFO(QMetaClassInfo, Q_RELOCATABLE_TYPE)

QT_END_NAMESPACE

#endif //QMETAOBJECT_H
