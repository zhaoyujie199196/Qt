//
// Created by Yujie Zhao on 2023/10/23.
//

#ifndef QMETAOBJECT_P_H
#define QMETAOBJECT_P_H

#include <QtCore/qglobal.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

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

//zhaoyujie TODO 这些是什么意思？
enum MetaDataFlag {
    DynamicMetaObject = 0x01,
    RequiresVariantMetaObject = 0x02,
    PropertyAccessInStaticMetaCall = 0x04
};

struct QMetaObjectPrivate
{
//    enum { OutputRevision = 10 };
//    enum { IntsPerMethod = QMetaMethod::Data::Size };
//    enum { IntsPerEnum = QMetaEnum::Data::Size };
//    enum { IntsPerProperty = QMetaProperty::Data::Size };

    int revision;
    int className;
    int classInfoCount, classInfoData;
    int methodCount, methodData;
    int propertyCount, propertyData;  //propertyCount: property的数量  propertyData: property信息开始的位置
    int enumeratorCount, enumeratorData;
    int constructorCount, constructorData;
    int flags;
    int signalCount;
};

QT_END_NAMESPACE

#endif //QMETAOBJECT_P_H
