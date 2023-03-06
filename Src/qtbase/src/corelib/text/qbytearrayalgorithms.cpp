//
// Created by Yujie Zhao on 2023/2/22.
//

#include "qbytearrayalgorithms.h"
#include <string>
#include "qbytearrayview.h"

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

int QtPrivate::compareMemory(QByteArrayView lhs, QByteArrayView rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        int ret = memcmp(lhs.data(), rhs.data(), std::min(lhs.size(), rhs.size()));
        if (ret != 0) {
            return ret;
        }
    }
    return lhs.size() - rhs.size();
}

QT_END_NAMESPACE