//
// Created by Yujie Zhao on 2023/7/21.
//

#ifndef QSYSINFO_H
#define QSYSINFO_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QString;
class QSysInfo {
public:
    enum Sizes {
        WordSize = (sizeof(void *) << 3),
    };

    enum Endian {
        BigEndian,
        LittleEndian,
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        ByteOrder = BigEndian,
#elif Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        ByteOrder = LittleEndian,
#else
#       error "Undefined byte order"
#endif
    };
};

QT_END_NAMESPACE

#endif //QSYSINFO_H
