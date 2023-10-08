//
// Created by Yujie Zhao on 2023/3/3.
//

#ifndef QREADWRITELOCK_H
#define QREADWRITELOCK_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QReadWriteLock
{
public:
    enum RecursionMode {
        NonRecursive,
        Recursive
    };

    explicit QReadWriteLock(RecursionMode recursionMode = NonRecursive) {}
    ~QReadWriteLock() {}

    void lockForRead() {}
    bool tryLockForRead() {return true;}
    bool tryLockForRead(int timeout) {return true;}

    void lockForWrite() {}
    bool tryLockForWrite() { return true;}
    bool tryLockForWrite(int timeout) {return true;}

    void unlock() {}
};

class QWriteLocker
{
public:
    inline QWriteLocker(QReadWriteLock *readWriteLock) {}
    QWriteLocker() = default;
    ~QWriteLocker() = default;
};

class QReadLocker
{
public:
    inline QReadLocker(QReadWriteLock *readWriteLock) {}
    QReadLocker() = default;
    ~QReadLocker() = default;
};

QT_END_NAMESPACE

#endif //QREADWRITELOCK_H
