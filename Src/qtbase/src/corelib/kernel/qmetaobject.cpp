//
// Created by Yujie Zhao on 2023/6/1.
//
#include "qobjectdefs.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qvariant.h>
#include <QtCore/qobject.h>
#include "qobjectdefs.h"
#include "qmetaobject_p.h"
#include "qobject_p.h"

QT_BEGIN_NAMESPACE

static inline const QMetaObjectPrivate *priv(const uint *data)
{
    return reinterpret_cast<const QMetaObjectPrivate *>(data);
}

//根据index，获取到QMetaObject中的字符串信息
static inline const char *rawStringData(const QMetaObject *mo, int index)
{
    Q_ASSERT(priv(mo->d.data)->revision >= 7);
    //一条信息占两位：开始位置和长度
    uint offset = mo->d.stringdata[2 * index];
    return reinterpret_cast<const char *>(mo->d.stringdata) + offset;
}

static QByteArray normalizeTypeInternal(const char *t, const char *e)
{
    Q_ASSERT(false);
    return QByteArray();
//    int len = QtPrivate::qNormalizeType(t, e, nullptr);
//    if (len == 0) {
//        return QByteArray();
//    }
//    QByteArray result(len, Qt::Uninitialized);
//    len = QtPrivate::qNormalizeType(t, e, result.data());
//    Q_ASSERT(len == result.size());
//    return result;
}

QMetaProperty::QMetaProperty(const QMetaObject *mobj, int index)
    : mobj(mobj)
    , data(getMetaPropertyData(mobj, index))
{
    Q_ASSERT(index >= 0 && index < priv(mobj->d.data)->propertyCount);
    if (data.flags() & EnumOrFlag) {
        //zhaoyujie TODO 没明白这段代码什么意思
        Q_ASSERT(false);
    }
}

QMetaProperty::Data QMetaProperty::getMetaPropertyData(const QMetaObject *mobj, int index)
{
    //mobj->d.data： meta_object信息
    //propertyData: property开始的地方
    //index * Data::Size： 在property信息中的偏移
    return { mobj->d.data + priv(mobj->d.data)->propertyData + index * Data::Size };
}

int QMetaProperty::Data::index(const QMetaObject *mobj) const
{
    return (d - mobj->d.data - priv(mobj->d.data)->propertyData) / Size;
}

bool QMetaProperty::isReadable() const
{
    if (!mobj) {
        return false;
    }
    return data.flags() & Readable;
}

bool QMetaProperty::isWriteable() const
{
    if (!mobj) {
        return false;
    }
    return data.flags() & Writable;
}

/*
 * 读取object的属性，调用到moc文件的static_metacall方法
 * 返回值作为argv的第一个参数
 * */
QVariant QMetaProperty::read(const QObject *obj) const
{
    if (!obj || !mobj) {
        return QVariant();
    }
    int status = -1;
    QVariant value;
    void *argv[] = { nullptr, &value, &status };
    QMetaType t(mobj->d.metaTypes[data.index(mobj)]);
    if (t == QMetaType::fromType<QVariant>()) {  //返回值是QVariant
        argv[0] = &value;
    }
    else {
        value = QVariant(t, nullptr);
        argv[0] = value.data();
    }
    if (priv(mobj->d.data)->flags & PropertyAccessInStaticMetaCall && mobj->d.static_metacall) {
        Q_ASSERT(false);
//        mobj->d.static_metacall(const_cast<QObject *>(object), QMetaObject::ReadProperty, data.index(mobj), argv);
    }
    else {
        QMetaObject::metacall(const_cast<QObject *>(obj), QMetaObject::ReadProperty, data.index(mobj) + mobj->propertyOffset(), argv);
    }

    if (status != -1) {
        return value;
    }
    if (t != QMetaType::fromType<QVariant>() && argv[0] != value.data()) {
        return QVariant(t, argv[0]);
    }
    return value;
}

bool QMetaProperty::write(QObject *obj, const QVariant &value) const
{
    if (!obj || !isWriteable()) {
        return false;
    }
    QVariant v = value;
    QMetaType t(mobj->d.metaTypes[data.index(mobj)]);
    if (t != QMetaType::fromType<QVariant>() && t != v.metaType()) {
        //zhaoyujie TODO
        Q_ASSERT(false);
    }
    int status = -1;
    int flags = 0;
    void *argv[] = { nullptr, &v, &status, &flags };
    if (t == QMetaType::fromType<QVariant>()) {
        argv[0] = &v;
    }
    else {
        argv[0] = v.data();
    }
    if (priv(mobj->d.data)->flags & PropertyAccessInStaticMetaCall && mobj->d.static_metacall) {
        Q_ASSERT(false);  //zhaoyujie TODO 上面的判断什么意思？什么时候能够走到？
    }
    else {
        QMetaObject::metacall(obj, QMetaObject::WriteProperty, data.index(mobj) + mobj->propertyOffset(), argv);
    }
    return status;
}


QByteArray QMetaObject::normalizedType(const char *type)
{
    Q_ASSERT(false);  //zhaoyujie TODO
    return normalizeTypeInternal(type, type + qstrlen(type));
}

int QMetaObject::propertyCount() const
{
    int n = priv(d.data)->propertyCount;
    const QMetaObject *m = d.superdata.direct;
    while (m) {  //循环添加父的propertyCount
        n += priv(m->d.data)->propertyCount;
        m = m->d.superdata;
    }
    return n;
}

int QMetaObject::propertyOffset() const
{
    int offset = 0;
    const QMetaObject *m = d.superdata;
    while (m) {
        offset + priv(m->d.data)->propertyCount;
        m = m->d.superdata;
    }
    return offset;
}

int QMetaObject::indexOfProperty(const char *name) const
{
    const QMetaObject *m = this;
    while (m) {
        const QMetaObjectPrivate *d = priv(m->d.data);
        for (int i = 0; i < d->propertyCount; ++i) {
            const QMetaProperty::Data data = QMetaProperty::getMetaPropertyData(m, i);
            const char *prop = rawStringData(m, data.name());
            //zhaoyujie TODO 这里的比较为什么先比较0，再从1开始？不是直接比较就可以了吗？
            if (name[0] == prop[0] && strcmp(name + 1, prop + 1) == 0) {
                i += m->propertyOffset();
                return i;
            }
        }
        m = m->d.superdata;
    }
    Q_ASSERT(false);
    return -1;
}

QMetaProperty QMetaObject::property(int index) const
{
    int i = index;
    i -= propertyOffset();
    if (i < 0 && d.superdata) {  //索引小于0，说明是父类的属性
        return d.superdata->property(index);
    }
    if (i >= 0 && i < priv(d.data)->propertyCount) {
        return QMetaProperty(this, i);
    }
    return QMetaProperty();
}

int QMetaObject::static_metacall(Call cl, int idx, void **argv) const
{
    Q_ASSERT(priv(d.data)->revision >= 6);
    if (!d.static_metacall) {
        return 0;
    }
    d.static_metacall(nullptr, cl, idx, argv);
    return -1;
}

int QMetaObject::metacall(QObject *object, Call cl, int idx, void **argv)
{
    if (object->d_ptr->metaObject) {
        return object->d_ptr->metaObject->metaCall(object, cl, idx, argv);
    }
    else {
        return object->qt_metacall(cl, idx, argv);
    }
}


QT_END_NAMESPACE
