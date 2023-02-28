//
// Created by Yujie Zhao on 2023/2/22.
//

#include "qbytearrayalgorithms.h"
#include <string>

QT_BEGIN_NAMESPACE

char *qstrdup(const char *src) {
    if (!src) {
        return nullptr;
    }
    char *dst = new char[strlen(src) + 1];
    return qstrcpy(dst, src);
}

char *qstrcpy(char *dst, const char *src) {
    return strcpy(dst, src);
}

QT_END_NAMESPACE