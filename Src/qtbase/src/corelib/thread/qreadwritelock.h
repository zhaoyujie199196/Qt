//
// Created by Yujie Zhao on 2023/3/3.
//

#ifndef QREADWRITELOCK_H
#define QREADWRITELOCK_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QWriteLocker
{
public:
    QWriteLocker() = default;
    ~QWriteLocker() = default;
};

class QReadLocker
{
public:
    QReadLocker() = default;
    ~QReadLocker() = default;
};

QT_END_NAMESPACE

#endif //QREADWRITELOCK_H
