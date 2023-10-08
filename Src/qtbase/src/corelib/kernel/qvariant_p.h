//
// Created by Yujie Zhao on 2023/9/26.
//

#ifndef QVARIANT_P_H
#define QVARIANT_P_H

#include "qvariant.h"

QT_BEGIN_NAMESPACE

template <class T>
inline void v_construct(QVariant::Private *x, const T &t)
{
    if constexpr (QVariant::Private::CanUseInternalSpace<T>) {
        new (&x->data) T(t);
        x->is_shared = false;
    }
    else {
        x->data.shared = QVariant::PrivateShared::create(QtPrivate::qMetaTypeInterfaceForType<T>());
        new (x->data.shared->data()) T(t);
        x->is_shared = true;
    }
}

QT_END_NAMESPACE

#endif //QVARIANT_P_H
