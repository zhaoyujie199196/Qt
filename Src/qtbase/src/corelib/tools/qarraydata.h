//
// Created by Yujie Zhao on 2023/2/28.
//

#ifndef QARRAYDATA_H
#define QARRAYDATA_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
    struct QContainerImplHelper {
        enum CutResult {
            Null,
            Empty,
            Full,
            Subset
        };
        static constexpr CutResult mid(qsizetype originalLength, qsizetype *_position, qsizetype *_length) {
            qsizetype &position = *_position;
            qsizetype &length = *_length;
            //位置超出了范围
            if (position > originalLength) {
                position = 0;
                length = 0;
                return Null;
            }
            if (position < 0) {
                if (length < 0 || length + position >= originalLength) {
                    position = 0;
                    length = originalLength;
                    return Full;
                }
                if (length + position <= 0) {
                    position = length = 0;
                    return Null;
                }
                else {
                    length += position;
                    position = 0;
                }
            }
            else if (length > originalLength - position) {
                length = originalLength - position;
            }
            if (position == 0 && length == originalLength) {
                return Full;
            }
            return length > 0 ? Subset : Empty;
        }
    };
}

QT_END_NAMESPACE

#endif //QARRAYDATA_H
