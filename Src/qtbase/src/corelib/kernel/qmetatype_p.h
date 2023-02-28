//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QMETATYPE_P_H
#define QMETATYPE_P_H

#include <QtCore/qglobal.h>
#include "qmetatype.h"

QT_BEGIN_NAMESPACE

namespace QtMetaTypePrivate {
    template <typename T>
    struct TypeDefinition {
        static const bool IsAvailable = true;
    };

    template <typename T>
    static const QT_PREPEND_NAMESPACE(QtPrivate::QMetaTypeInterface) *getInterfaceFromType() {
        if constexpr (QtMetaTypePrivate::TypeDefinition<T>::IsAvailable) {
            return &QT_PREPEND_NAMESPACE(QtPrivate::QMetaTypeInterfaceWrapper)<T>::metaType;
        }
        return nullptr;
    }

#define QT_METATYPE_CONVERT_ID_TO_TYPE(MetaTypeName, MetaTypeId, RealName) \
    case QMetaType::MetaTypeName: \
        return QtMetaTypePrivate::getInterfaceFromType<RealName>();

}

QT_END_NAMESPACE

#endif //QMETATYPE_P_H
