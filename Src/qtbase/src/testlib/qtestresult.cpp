//
// Created by Yujie Zhao on 2023/2/8.
//

#include "qtestresult_p.h"
#include <stdio.h>
#include <QtCore/qglobal.h>
#include <QtCore/qobjectdefs.h>
#include <QtTest/qtestdata.h>
#include <QtCore/QStringView>

QT_BEGIN_NAMESPACE

namespace QTest {
    static QTestData *currentTestData = nullptr;
}

bool QTestResult::verify(bool statement, const char *statementStr, const char *extraInfo, const char *file, int line)
{
    //zhaoyujie TODO
    if (!statement) {
        printf("error : %s %d", file, line);
        Q_ASSERT(false);
        return false;
    }
    Q_ASSERT(statement);
    return statement;
}

QTestData *QTestResult::currentTestData()
{
    return QTest::currentTestData;
}

void QTestResult::setCurrentTestData(QTestData *testData)
{
    QTest::currentTestData = testData;
//    QTest::resetFailed();
//    if (data)
//        QTestLog::enterTestData(data);
}

bool QTestResult::compare(bool success, const char *failMsg, char *val1, char *val2, const char *actual,
                          const char *expected, const char *file, int line) {
    Q_ASSERT(success);
    delete []val1;
    delete []val2;
    return success;
}

bool QTestResult::compare(bool success, const char *failMsg, QStringView val1, QStringView val2, const char *actual,
                          const char *expected, const char *file, int line)
{
    Q_ASSERT(success);
    return success;
}

QT_END_NAMESPACE