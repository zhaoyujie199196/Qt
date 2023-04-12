//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QBYTEARRAY_H
#define QBYTEARRAY_H

#include <QtCore/qglobal.h>
#include "qbytearrayalgorithms.h"

QT_BEGIN_NAMESPACE

class QByteArray
{
public:
    typedef qsizetype size_type;
    typedef qptrdiff difference_type;
    typedef const char & const_reference;
    typedef char &reference;
    typedef char *pointer;
    typedef const char *const_pointer;
    typedef char value_type;


private:
    static const char _empty;
};


QT_END_NAMESPACE

#endif //QBYTEARRAY_H
