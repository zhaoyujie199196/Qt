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

#define QMETATYPE_CONVERTER(To, From, assign_and_return) \
    case makePair(QMetaType::To, QMetaType::From):       \
        if (onlyCheck) { \
            return true;                                     \
        }                                                \
        {                                                \
            const From &source = *static_cast<const From *>(from); \
            To &result = *static_cast<To *>(to);         \
            assign_and_return \
        }

#define QMETATYPE_CONVERTER_ASSIGN(To, From) \
    QMETATYPE_CONVERTER(To, From, result = To(source); return true;)

}

QT_END_NAMESPACE

#endif //QMETATYPE_P_H
