//
// Created by Yujie Zhao on 2023/5/16.
//
#include "qlocale.h"
#include "qlocale_p.h"
#include "qlocale_data_p.h"

QT_BEGIN_NAMESPACE

static const QLocaleData *const c_data = locale_data;

const QLocaleData *QLocaleData::c() {
    Q_ASSERT(locale_index[QLocale::C] == 0);
    return c_data;
}


QT_END_NAMESPACE