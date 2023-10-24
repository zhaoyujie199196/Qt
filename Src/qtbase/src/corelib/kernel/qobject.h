//
// Created by Yujie Zhao on 2023/2/24.
//

#ifndef QOBJECT_H
#define QOBJECT_H

#include <QtCore/qglobal.h>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <QtCore/QVariant>
#include <QtCore/QList>
#include "qtmetamacros.h"

QT_BEGIN_NAMESPACE

#define REGISTER_OBJECT_INVOKE_METHOD(functionName) \
    registerInvokeMethod(#functionName, [this](){ \
        this->functionName();                     \
    });

class QEvent;
class QObjectPrivate;
class QObject;
class QThread;

struct QDynamicMetaObjectData;

typedef QList<QObject *> QObjectList;

class Q_CORE_EXPORT QObjectData
{
    Q_DISABLE_COPY(QObjectData)
public:
    QObjectData() = default;
    virtual ~QObjectData() {}
    QObject *q_ptr;  //q指针，objectData的owner指针
    QObject *parent; //父亲指针
    QObjectList children;  //子对象列表

    QDynamicMetaObjectData *metaObject;
    QMetaObject *dynamicMetaObject() const;

};

class QObject {
    Q_OBJECT
    //声明private指针
    Q_DECLARE_PRIVATE(QObject)
    Q_PROPERTY(QString objectName READ objectName WRITE setObjectName)
public:
    friend struct QMetaObject;
    friend struct QMetaObjectPrivate;

    typedef std::function<void()> InvokeMethod;
    typedef std::vector<std::pair<std::string, InvokeMethod>> InvokeMethodMap;

    Q_INVOKABLE explicit QObject(QObject *parent = nullptr);
    virtual ~QObject();


    QString objectName() const;
    void setObjectName(const QString &name);
//    QBindable<QString> QObject::bindableObjectName();

    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);

    //翻译
    static QString tr(const char *sourceText, const char * = nullptr, int = -1) {
        //zhaoyujie TODO windows怎么弄的
#if defined(Q_OS_WIN)
        Q_ASSERT(false);
#endif
        return QString::fromUtf8(sourceText);
    }

    //通过name设置属性
    bool setProperty(const char *name, const QVariant &value);
    QVariant property(const char *name) const;

    QThread *thread() const;

    //在实现Q_INVOKABLE之前先使用这种硬编码的方式
    virtual void registerInvokeMethods() {}

    const InvokeMethodMap &getInvokeMethodMap() const {return m_invokeMethodMap;}

protected:
    QObject(QObjectPrivate &dd, QObject *parent = nullptr);
    void registerInvokeMethod(const std::string &key, const InvokeMethod &func);

protected:
    std::unique_ptr<QObjectData> d_ptr;

    InvokeMethodMap m_invokeMethodMap;
};

QT_END_NAMESPACE

#endif //QOBJECT_H
