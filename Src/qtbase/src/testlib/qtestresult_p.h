//
// Created by Yujie Zhao on 2023/2/8.
//

#ifndef QTESTRESULT_P_H
#define QTESTRESULT_P_H

#include <QtTest/qtestglobal.h>
#include <QtGlobal>

QT_BEGIN_NAMESPACE

class QTestData;
class QStringView;
class Q_TESTLIB_EXPORT QTestResult
{
public:
    static bool verify(bool statement, const char *statementStr, const char *extraInfo, const char *file, int line);
    static QTestData *currentTestData();
    static void setCurrentTestData(QTestData *testData);
    static bool compare(bool success, const char *failMsg, char *val1, char *val2, const char *actual, const char *expected, const char *file, int line);
    static bool compare(bool success, const char *failMsg, QStringView val1, QStringView val2, const char *actual, const char *expected, const char *file, int line);
    static bool compare(bool success, const char *failureMsg, double val1, double val2, const char *actual, const char *expected, const char *file, int line);
    static bool compare(bool success, const char *failureMsg, float val1, float val2, const char *actual, const char *expected, const char *file, int line);
    static bool compare(bool success, const char *failureMsg, int val1, int val2, const char *actual, const char *expected, const char *file, int line);
};

QT_END_NAMESPACE

#endif //QTESTRESULT_P_H
