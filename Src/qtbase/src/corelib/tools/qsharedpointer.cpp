//
// Created by Yujie Zhao on 2023/8/15.
//
#include "qsharedpointer.h"
#include "qshareddata.h"
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

void QtSharedPointer::ExternalRefCountData::setQObjectShared(const QObject *, bool enable) {

}

void QtSharedPointer::ExternalRefCountData::checkQObjectShared(const QObject *) {
    if (strongref.loadRelaxed() < 0) {
        Q_ASSERT(false);
        //qWarning("QSharedPointer: cannot create a QSharedPointer from a QObject-tracking QWeakPointer");
    }
}

QtSharedPointer::ExternalRefCountData *QtSharedPointer::ExternalRefCountData::getAndRef(const QObject *) {
    //zhaoyujie TODO
    Q_ASSERT(false);
    return nullptr;
}

QT_END_NAMESPACE

