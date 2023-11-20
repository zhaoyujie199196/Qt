//
// Created by Yujie Zhao on 2023/11/17.
//
#include "qhooks_p.h"

QT_BEGIN_NAMESPACE

quintptr Q_CORE_EXPORT qtHookData[] = {
        3,  //hook data version
        QHooks::LastHookIndex, // size of qtHookData
        QT_VERSION,
        0, //Add QObject
        0, //Remove QObject
        0, //Startup
        21,  //TypeInformationVersion ?
};

QT_END_NAMESPACE