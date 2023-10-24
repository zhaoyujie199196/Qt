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

public:
    enum Type {
        None = 0,   //非法事件
        DynamicPropertyChange = 170,  //QObject的setProperty / property导致的动态属性变化
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


QT_END_NAMESPACE

#endif //QCOREEVENT_H
