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

//元方法
class QMetaMethod
{

};

//类的元信息
class QMetaClassInfo
{

};
Q_DECLARE_TYPEINFO(QMetaClassInfo, Q_RELOCATABLE_TYPE)

QT_END_NAMESPACE

#endif //QMETAOBJECT_H
