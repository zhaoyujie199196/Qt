//
// Created by Yujie Zhao on 2023/10/25.
//

#ifndef QMUTEX_H
#define QMUTEX_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QBasicMutex
{
public:
    void lock() {}
    void unlock() {}
};

template <typename Mutex>
class QMutexLocker
{
public:
    inline explicit QMutexLocker(Mutex *mutex) {

    }
    ~QMutexLocker() {
        unlock();
    }

    inline void unlock() noexcept  {

    }

    inline void relock() noexcept {

    }

    Mutex *mutex() const {
        return nullptr;
    }
};

QT_END_NAMESPACE

#endif //QMUTEX_H
