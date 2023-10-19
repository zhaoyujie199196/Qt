//
// Created by Yujie Zhao on 2023/10/10.
//

#ifndef QRANDOM_H
#define QRANDOM_H

#include <QtCore/qalgorithms.h>
#include <algorithm>
#include <random>

/*
 * 随机数算法介绍：https://zhuanlan.zhihu.com/p/441297199
 * */

#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif

QT_BEGIN_NAMESPACE

class QRandomGenerator
{
public:
    struct InitialRandomData {
        //64位系统，quintptr占8个字节，相当于quintptr data[2]
        quintptr data[16 / sizeof(quintptr)];
    };

    struct SystemGenerator;  //前置声明
    struct SystemAndGlobalGenerators;

private:
    union Storage {

    };

    uint type;
    Storage storage;
};

QT_END_NAMESPACE

#endif //QRANDOM_H
