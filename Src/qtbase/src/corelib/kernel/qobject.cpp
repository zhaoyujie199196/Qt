//
// Created by Yujie Zhao on 2023/2/24.
//

#include "qobject.h"
#include "qcoreevent.h"
#include "qobject_p.h"
#include <QtCore/private/qthread_p.h>
#include <QtCore/qvariant.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

QMetaObject *QObjectData::dynamicMetaObject() const
{
    return metaObject->toDynamicMetaObject(q_ptr);
}

QObjectPrivate::~QObjectPrivate()
{
    delete extraData;
}

QObject::QObject(QObject *parent)
    : QObject(*new QObjectPrivate, parent)
{
}

QObject::QObject(QObjectPrivate &dd, QObject *parent)
    : d_ptr(&dd)
{
    Q_ASSERT(this != parent);
    Q_D(QObject);
    d_ptr->q_ptr = this;
    //和父的线程保持一致。如果没有父，使用当前线程
    auto threadData = (parent && !parent->thread()) ? parent->d_func()->threadData.loadRelaxed() : QThreadData::current();
    threadData->ref();
    d->threadData.storeRelaxed(threadData);
    if (parent) {
        Q_ASSERT(false);
    }
    //zhaoyujie TODO 添加QObject的钩子
//    if (Q_UNLIKELY(qtHookData[QHooks::AddQObject]))
//        reinterpret_cast<QHooks::AddQObjectCallback>(qtHookData[QHooks::AddQObject])(this);
//    Q_TRACE(QObject_ctor, this);

}

QObject::~QObject()
{
    //zhaoyujie TODO
}


QString QObject::objectName() const
{
//    Q_D(const QObject);
//    if (!d->extraData && QtPrivate::isAnyBindingEvaluating()) {
//        QObjectPrivate *dd = const_cast<QObjectPrivate *>(d);
//        dd->extraData = new QObjectPrivate::ExtraData(dd);
//    }
//    return d->extraData ? d->extraData->objectName : QString();
    return "";
}

void QObject::setObjectName(const QString &name)
{
//    Q_D(QObject);
//    d->ensureExtraData();
//    d->extraData->objectName.removeBindingUnlessInWrapper();
//    if (d->extraData->objectName != name) {
//        d->extraData->objectName.setValueBypassingBindings(name);
//        d->extraData->objectName.notify();
//    }
    int k = 0;
    k++;
}

bool QObject::event(QEvent *event)
{
    Q_ASSERT(false);
    return true;
}

bool QObject::eventFilter(QObject *, QEvent *)
{
    return false;
}


QVariant QObject::property(const char *name) const
{
    Q_D(const QObject);
    const QMetaObject *meta = metaObject();
    if (!name || !meta) {
        return QVariant();
    }
    int id = meta->indexOfProperty(name);
    if (id < 0) {  //没有通过Q_PROPERTY注册，查找动态属性
        if (!d->extraData) {
            return QVariant();
        }
        const int i = d->extraData->propertyNames.indexOf(name);
        return d->extraData->propertyValues.value(i);
    }
    else {
        QMetaProperty p = meta->property(id);
        if (!p.isReadable()) {
            Q_ASSERT(false);
        }
        return p.read(this);
    }
}

/*
 * setProperty先从metaObject中查找属性，如果找不到，就放在extraData（动态属性）中
 * 通过Q_PROPERTY注册的属性，设置成功返回true，否则都返回false
 * */
bool QObject::setProperty(const char *name, const QVariant &value)
{
    Q_D(QObject);
    const QMetaObject *meta = metaObject();
    if (!name || !meta) {
        return false;
    }
    int id = meta->indexOfProperty(name);
    if (id < 0) {
        if (!d->extraData) {
            d->extraData = new QObjectPrivate::ExtraData(d);
        }
        const int idx = d->extraData->propertyNames.indexOf(name);
        if (!value.isValid()) {  //value非法，从额外数据中移除
            if (idx == -1) {
                return false;
            }
            d->extraData->propertyNames.removeAt(idx);
            d->extraData->propertyValues.removeAt(idx);
        }
        else {
            if (idx == -1) {
                //value合法，extraData中没有此属性，将数据加入到extraData中
                d->extraData->propertyNames.append(name);
                d->extraData->propertyValues.append(value);
            }
            else {
                //数据类型相同，且数据相同无需设置，否则替换
                if (value.userType() == d->extraData->propertyValues.at(idx).userType()
                    && value == d->extraData->propertyValues.at(idx)) {
                    return false;
                }
                d->extraData->propertyValues[idx] = value;
            }
        }
        QDynamicPropertyChangeEvent ev(name);
        QCoreApplication::sendEvent(this, &ev);
        return false;
    }
    else {
        QMetaProperty p = meta->property(id);
        if (!p.isWriteable()) {
            Q_ASSERT(false);
        }
        return p.write(this, value);
    }
}

QThread *QObject::thread() const
{
    Q_ASSERT(false);
    return nullptr;
}

void QObject::registerInvokeMethod(const std::string &key, const InvokeMethod &func)
{
    m_invokeMethodMap.push_back({key, std::move(func)});
}


QT_END_NAMESPACE