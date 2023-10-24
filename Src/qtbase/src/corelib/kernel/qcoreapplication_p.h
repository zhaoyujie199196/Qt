//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QCOREAPPLICATION_P_H
#define QCOREAPPLICATION_P_H

#include "qobject_p.h"

QT_BEGIN_NAMESPACE

class QCoreApplicationPrivate : public QObjectPrivate
{
public:
    static bool notify_helper(QObject *receiver, QEvent *event);
};

QT_END_NAMESPACE

#endif //QCOREAPPLICATION_P_H
