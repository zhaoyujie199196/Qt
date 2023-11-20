//
// Created by Yujie Zhao on 2023/10/23.
//

#ifndef QTHREAD_H
#define QTHREAD_H

#include <QtCore/qglobal.h>
#include <QtCore/qnamespace.h>

QT_BEGIN_NAMESPACE

class QThread
{
public:
    static Qt::HANDLE currentThreadId() noexcept;
    static QThread *currentThread();
};

QT_END_NAMESPACE

#endif //QTHREAD_H
