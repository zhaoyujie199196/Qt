//
// Created by Yujie Zhao on 2023/4/11.
//

#ifndef QSCOPEDVALUEROLLBACK_H
#define QSCOPEDVALUEROLLBACK_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

//实现变量值的回滚
/*
 * 引用可以相互传递
 * int a = 1;
 * int &b = a;
 * int &c = b;
 * c = 2;
 * 此时 a 的值为 2
 */
template <typename T>
class QScopedValueRollback
{
public:
    explicit constexpr QScopedValueRollback(T &var)
        : varRef(var), oldValue(var)
    {

    }
    explicit constexpr QScopedValueRollback(T &var, T value)
        : varRef(var), oldValue(qExchange(var, std::move(value)))
    {

    }

    ~QScopedValueRollback()
    {
        varRef = std::move(oldValue);
    }
    constexpr void commit()
    {
        oldValue = varRef;
    }

private:
    T &varRef;
    T oldValue;
};

QT_END_NAMESPACE

#endif //QSCOPEDVALUEROLLBACK_H
