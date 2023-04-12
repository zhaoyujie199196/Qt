//
// Created by Yujie Zhao on 2023/2/28.
//

#ifndef QSTRING_H
#define QSTRING_H

#include "qlatin1string.h"

QT_BEGIN_NAMESPACE

class QString
{
public:
    typedef qsizetype size_type;
    typedef qptrdiff difference_type;
    typedef const QChar &const_reference;
    typedef QChar &reference;
    typedef QChar *pointer;
    typedef const QChar *const_pointer;
    typedef QChar value_type;

};

QT_END_NAMESPACE

#endif //QSTRING_H
