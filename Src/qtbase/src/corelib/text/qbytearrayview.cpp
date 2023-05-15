//
// Created by Yujie Zhao on 2023/4/25.
//
#include "qbytearrayview.h"
#include "QtCore/qbytearray.h"

QT_BEGIN_NAMESPACE

QByteArray QByteArrayView::toByteArray() const
{
    return QByteArray(data(), size());
}

QT_END_NAMESPACE
