//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QOBJECT_P_H
#define QOBJECT_P_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QThreadData;

//zhaoyujie TODO 这段代码什么作用，好奇怪的代码。，。。
struct QAbstractDynamicMetaObject;
struct QDynamicMetaObjectData
{
    virtual ~QDynamicMetaObjectData() {}
    virtual void objectDestroyed(QObject *) {
        Q_ASSERT(false);
        delete this;
    }
    virtual QAbstractDynamicMetaObject *toDynamicMetaObject(QObject *) = 0;
    virtual int metaCall(QObject *, QMetaObject::Call, int _id, void **) = 0;
};

struct QAbstractDynamicMetaObject : public QDynamicMetaObjectData, public QMetaObject
{
    ~QAbstractDynamicMetaObject() {}
    QAbstractDynamicMetaObject *toDynamicMetaObject(QObject *) override { return this; }
    int metaCall(QObject *, QMetaObject::Call c, int _id, void **a) override {
        return metaCall(c, _id, a);
    }
    virtual int metaCall(QMetaObject::Call, int _id,void **) {
        return _id;
    }
};

class QObjectPrivate : public QObjectData
{
    Q_DECLARE_PUBLIC(QObject)
public:
    struct ExtraData {
        ExtraData(QObjectPrivate *ptr) : parent(ptr)
        {
        }

        QList<QByteArray> propertyNames;  //propertyNames与propertyValues为QObject的动态属性
        QList<QVariant> propertyValues;
        QString objectName;  //zhaoyujie TODO QObject的名称
        QObjectPrivate *parent = nullptr;  //zhaoyujie TODO 这个parent和QObjectData中的parent是不是重复了？
    };

    struct ConnectionOrSignalVector {

    };

    struct Connection : public ConnectionOrSignalVector {

    };

    struct ConnectionList {

    };

    struct Sender {

    };

    struct SignalVector : public ConnectionOrSignalVector {

    };

    struct ConnectionData {

    };

    void ensureExtraData() {
        if (!extraData) {
            extraData = new ExtraData(this);
        }
    }
    ~QObjectPrivate() override ;

public:
    mutable ExtraData *extraData;  //用户设置的其他属性
    QAtomicPointer<QThreadData> threadData;  //QObject所属线程

    QAtomicPointer<ConnectionData> connections;
};

QT_END_NAMESPACE

#endif //QOBJECT_P_H
