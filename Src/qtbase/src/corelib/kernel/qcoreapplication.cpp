//
// Created by Yujie Zhao on 2023/10/20.
//
#include "qcoreapplication.h"
#include "qcoreapplication_p.h"
#include <QtCore/private/qtrace_p.h>

QT_BEGIN_NAMESPACE

static bool doNotify(QObject *, QEvent *);

bool QCoreApplicationPrivate::notify_helper(QObject *receiver, QEvent *event)
{
    return receiver->event(event);
}

bool QCoreApplication::sendEvent(QObject *receiver, QEvent *event)
{
    Q_TRACE(QCoreApplication_sendEvent, receiver, event, event->type());
    return false;
}

bool QCoreApplication::notifyInternal2(QObject *receiver, QEvent *event)
{
    Q_ASSERT(false);
    return false;
}

bool QCoreApplication::notify(QObject *receiver, QEvent *event)
{
    Q_ASSERT(false);
    return doNotify(receiver, event);
}

static bool doNotify(QObject *receiver, QEvent *event)
{
    Q_ASSERT(false);
    return false;
}


QT_END_NAMESPACE
