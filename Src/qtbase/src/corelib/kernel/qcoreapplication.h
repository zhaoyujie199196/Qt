//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QCOREAPPLICATION_H
#define QCOREAPPLICATION_H

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

#define qApp QCoreApplication::instance()

class QEvent;
class QCoreApplication : public QObject
{
public:
    static QCoreApplication *instance() noexcept { return self; }

    //通知事件
    virtual bool notify(QObject *, QEvent *);

    // 事件相关： 发送事件给接收者（统一发送）
    // sendEvent会立即处理给定的事件
    static bool sendEvent(QObject *receiver, QEvent *event);
    // postEvent会讲事件放到等待队列中，当下一次Qt的主事件循环运行时才会处理
    static void postEvent(QObject *receiver, QEvent *event, int priority = Qt::NormalEventPriority);

private:
    static bool notifyInternal2(QObject *receiver, QEvent *);

private:
    static QCoreApplication *self;
};

QT_END_NAMESPACE

#endif //QCOREAPPLICATION_H
