//
// Created by Yujie Zhao on 2023/10/23.
//
#include "qthread_p.h"
#include "qthread.h"

QT_BEGIN_NAMESPACE

static QThreadData *data = nullptr;

QThreadData *QThreadData::current(bool createIfNecessary) {
    if (!data && createIfNecessary) {
        data = new QThreadData;
        //zhaoyujie TODO
    }
    return data;
}

void QThreadData::ref() {
    _ref.ref();
    Q_ASSERT(_ref.loadRelaxed() != 0);
}

void QThreadData::deref() {
    if (!_ref.deref()) {
        delete this;
    }
}

QT_END_NAMESPACE

