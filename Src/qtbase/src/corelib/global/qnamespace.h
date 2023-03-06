//
// Created by Yujie Zhao on 2023/2/28.
//

#ifndef QNAMESPACE_H
#define QNAMESPACE_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

namespace Qt{
    //大小写敏感
    enum CaseSensitivity {
        CaseInsensitive,
        CaseSensitive
    };
}

QT_END_NAMESPACE

#endif //QNAMESPACE_H
