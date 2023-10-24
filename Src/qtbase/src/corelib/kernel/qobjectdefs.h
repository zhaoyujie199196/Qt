//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QOBJECTDEFS_H
#define QOBJECTDEFS_H

#include <QtCore/qtmetamacros.h>
#include <QtCore/qbytearray.h>

QT_BEGIN_NAMESPACE

class QObject;
class QMetaProperty;

namespace QtPrivate {
    class QMetaTypeInterface;
}

//元对象
struct QMetaObject
{
    enum Call {
//        InvokeMetaMethod,
        ReadProperty,
        WriteProperty,
        ResetProperty,
        CreateInstance,
        IndexOfMethod,
        RegisterPropertyMetaType,
        RegisterMethodArgumentMetaType,
        BindableProperty,
        CustomCall,
        ConstructInPlace,
    };
    //基类的QMetaObject
    struct SuperData {
        const QMetaObject *direct = nullptr;
        SuperData() = default;
        constexpr SuperData(std::nullopt_t) : direct(nullptr) {}
        constexpr SuperData(const QMetaObject *mo) : direct(mo) {}
        constexpr const QMetaObject *operator->() const { return operator const QMetaObject *(); }

        constexpr operator const QMetaObject *() const { return direct; }
    };

    struct Data {   //data的内容都在moc文件中初始化
        SuperData superdata;  //父的元对象信息
        const uint *stringdata;  //字符串信息，所有的信号 / 槽 / 属性 名字都放在stringdata中
        const uint *data;     //QMetaObjectPrivate指针
        typedef void (*StaticMetacallFunction)(QObject *, QMetaObject::Call, int, void **);
        StaticMetacallFunction static_metacall;
        const SuperData *relatedMetaObjects;
        const QtPrivate::QMetaTypeInterface *const *metaTypes;
        void *extradata;
    };

    static QByteArray normalizedType(const char *type);

    int propertyCount() const;

    //property的偏移，前面的property都是父类的属性，自己的属性放在后面
    //排列方式： Class1Prop1——Class1Prop2——Class2Prop1——Class2Prop2——ThisClassProp
    int propertyOffset() const;

    //根据名字查找property的索引
    int indexOfProperty(const char *name) const;

    //获取property
    QMetaProperty property(int index) const;

    //调用fangfa
    int static_metacall(Call, int, void **) const;
    static int metacall(QObject *, Call, int, void **);

public:
    Data d;
};


QT_END_NAMESPACE

#endif //QOBJECTDEFS_H
