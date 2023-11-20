//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QCOREEVENT_H
#define QCOREEVENT_H

#include <QtCore/qnamespace.h>
#include <QtCore/QByteArray>

QT_BEGIN_NAMESPACE

#define Q_EVENT_DISABLE_COPY(Class) \
protected:                          \
    Class(const Class &) = default;     \
    Class(Class &&) = delete;       \
    Class &operator=(const Class &other) = default; \
    Class &operator=(Class &&) = delete;\



class QEvent
{
    Q_EVENT_DISABLE_COPY(QEvent)
    friend class QCoreApplication;
public:
    enum Type {
        None = 0,   //非法事件
        Timer = 1,  //时间
        ThreadChange = 22,  //QObject的线程改变
        MetaCall = 43,  //meta call 时间
        DeferredDelete = 52,  //延迟删除
        ChildAdded = 68,   //新建子,
        ChildPolished = 69,  //TODO
        ChildRemoved = 71, //删除子
        DynamicPropertyChange = 170,  //QObject的setProperty / property导致的动态属性变化
        User = 1000,  //用户自注册的事件id
        MaxUsr = 65535
    };
    QEvent(Type type);
    virtual ~QEvent() {}
    //这种写法有点奇技淫巧，QEvent的积累指针clone后返回的是指向子类的基类指针，使用时需要转换
    virtual QEvent *clone() const;

    inline Type type() const { return static_cast<Type>(t); }

protected:
    Type t;
};

/*
 * 动态属性变化
 * QObject的动态属性是放在ExtraData中的属性
 * */
class QDynamicPropertyChangeEvent : public QEvent
{
    Q_EVENT_DISABLE_COPY(QDynamicPropertyChangeEvent)
public:
    explicit QDynamicPropertyChangeEvent(const QByteArray &name);
    ~QDynamicPropertyChangeEvent();

    inline QByteArray propertyName() const { return n; }
    QDynamicPropertyChangeEvent *clone() const override {
        return new QDynamicPropertyChangeEvent(*this);
    }

protected:
    QByteArray n;
};

class Q_CORE_EXPORT QTimerEvent : public QEvent
{
    Q_EVENT_DISABLE_COPY(QTimerEvent)
public:
    explicit QTimerEvent(int timerId);
    ~QTimerEvent();
    int timerId() const { return id; }

    QTimerEvent *clone() const override { return new QTimerEvent(*this); }

protected:
    int id;
};

class QObject;
class Q_CORE_EXPORT QChildEvent : public QEvent
{
Q_EVENT_DISABLE_COPY(QChildEvent);
public:
    QChildEvent(Type type, QObject *child);
    ~QChildEvent();
    QObject *child() const { return c; }
    bool added() const { return type() == ChildAdded; }
    bool polished() const { return type() == ChildPolished; }
    bool removed() const { return type() == ChildRemoved; }

    QChildEvent *clone() const override { return new QChildEvent(*this); }

protected:
    QObject *c;
};

class Q_CORE_EXPORT QDeferredDeleteEvent : public QEvent
{
Q_EVENT_DISABLE_COPY(QDeferredDeleteEvent);
public:
    explicit QDeferredDeleteEvent();
    ~QDeferredDeleteEvent();
    int loopLeven() const { return level; }

    QDeferredDeleteEvent *clone() const override {
        return new QDeferredDeleteEvent(*this);
    }

private:
    int level;
    friend class QCoreApplication;
};


QT_END_NAMESPACE

#endif //QCOREEVENT_H
