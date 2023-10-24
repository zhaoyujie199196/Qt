//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QCOREAPPLICATION_H
#define QCOREAPPLICATION_H

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QEvent;
class QCoreApplication : public QObject
{
public:
    //通知事件
    virtual bool notify(QObject *, QEvent *);

    //发送事件给接收者（统一发送）
    static bool sendEvent(QObject *receiver, QEvent *event);

private:
    static bool notifyInternal2(QObject *receiver, QEvent *);
};

QT_END_NAMESPACE

#endif //QCOREAPPLICATION_H
