//
// Created by Yujie Zhao on 2023/2/16.
//

#ifndef QTESTASSERT_H
#define QTESTASSERT_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

//注意cond周围的括号
#define QTEST_ASSERT(cond) do { if (!(cond)) qt_assert(#cond, __FILE__, __LINE__); } while (false)

#define QTEST_ASSERT_X(cond, where, what) do {if (!(cond)) qt_assert_x(where, what, __FILE__, __LINE__); } while (false)

QT_END_NAMESPACE

#endif //QTESTASSERT_H
