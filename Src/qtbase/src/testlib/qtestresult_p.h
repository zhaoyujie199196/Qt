//
// Created by Yujie Zhao on 2023/2/8.
//

#ifndef QTESTRESULT_P_H
#define QTESTRESULT_P_H

#include <QtTest/qtestglobal.h>
#include <QtGlobal>

QT_BEGIN_NAMESPACE

class QTestData;
class Q_TESTLIB_EXPORT QTestResult
{
public:
    static bool verify(bool statement, const char *statementStr, const char *extraInfo, const char *file, int line);
    static QTestData *currentTestData();
    static void setCurrentTestData(QTestData *testData);
    static bool compare(bool success, const char *failMsg, char *val1, char *val2, const char *actual, const char *expected, const char *file, int line);
};

QT_END_NAMESPACE

#endif //QTESTRESULT_P_H
