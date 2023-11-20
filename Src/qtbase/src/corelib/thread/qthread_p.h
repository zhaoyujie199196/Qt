//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QTHREAD_P_H
#define QTHREAD_P_H

#include "QtCore/qglobal.h"

QT_BEGIN_NAMESPACE

class QThreadData
{
public:
    class FlaggedDebugSignatures {
    public:
        bool contains(const char *method) const {
            return false;
        }
    };

    //当前线程
    static QThreadData *current(bool createIfNecessary = true);

    void ref();
    void deref();

private:
    QAtomicInt _ref;

public:
    QAtomicPointer<void> threadId = QAtomicPointer<void>(nullptr);
    FlaggedDebugSignatures flaggedSignatures;
};

QT_END_NAMESPACE

#endif //QTHREAD_P_H
