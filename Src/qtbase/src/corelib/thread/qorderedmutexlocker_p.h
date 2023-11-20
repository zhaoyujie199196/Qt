//
// Created by Yujie Zhao on 2023/10/25.
//

#ifndef QORDEREDMUTEXLOCKER_P_H
#define QORDEREDMUTEXLOCKER_P_H

#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QBasicMutex;
class QOrderedMutexLocker
{
public:
    QOrderedMutexLocker(QBasicMutex *m1, QBasicMutex *m2)
    {
    }

    ~QOrderedMutexLocker() {}

    void lock() {}
    void unlock() {}
    static bool relock(QBasicMutex *mtx1, QBasicMutex *mtx2) {
        return false;
    }
    void dismiss() {}
};

class QBasicMutexLocker
{
public:
    inline explicit QBasicMutexLocker(QBasicMutex *m) {

    }

    inline void unlock() noexcept  {

    }

    inline void relock() noexcept {

    }
};

QT_END_NAMESPACE

#endif //QORDEREDMUTEXLOCKER_P_H
