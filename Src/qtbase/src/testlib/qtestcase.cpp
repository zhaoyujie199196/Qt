#include "qtestcase.h"
#include "qtestresult_p.h"
#include "qtestrunner_p.h"
#include <QtCore/qobject.h>
#include <QtTest/qtestassert.h>
#include <QtTest/private/qtesttable_p.h>
#include <QtTest/qtestdata.h>
#include <QtCore/QByteArray>

int QTest::qExec(QObject *object)
{
    QTestRunner runner(object);
    return runner.exec();
}

bool QTest::qVerify(bool statement, const char *statementStr, const char *description, const char *file, int line)
{
    return QTestResult::verify(statement, statementStr, description, file, line);
}

void QTest::addColumnInternal(int id, const char *name)
{
    QTestTable *table = QTestTable::currentTestTable();
    QTEST_ASSERT_X(table, "QTest::addColumn()", "Cannot add testdata outside of a _data slot.");
    table->addColumn(id, name);
}

void *QTest::qData(const char *tagName, int typeId) {
    return fetchData(QTestResult::currentTestData(), tagName, typeId);
}

void *QTest::fetchData(QTestData *data, const char *tagName, int typeId) {
    int index = data->parent()->indexOf(tagName);
    Q_ASSERT(index >= 0 && index < data->parent()->elementCount());
    return data->data(index);
}

QTestData &QTest::newRow(const char *dataTag)
{
    QTEST_ASSERT_X(dataTag, __FUNCTION__ , "Data tag cannot be null");
    QTestTable *table = QTestTable::currentTestTable();
    QTEST_ASSERT_X(table, __FUNCTION__ , "Cannot add testdata outside of a _data slot");
    QTEST_ASSERT_X(table->elementCount(), __FUNCTION__ , "Must add columns before add rows");
    return *table->newData(dataTag);
}

QTestData &QTest::addRow(const char *format, ...)
{
    QTEST_ASSERT_X(format, __FUNCTION__ , "Format string cannot be null");
    QTestTable *table = QTestTable::currentTestTable();
    QTEST_ASSERT_X(table, __FUNCTION__ , "Cannot add testdata outside of a _data slot");
    QTEST_ASSERT_X(table->elementCount(), __FUNCTION__ , "Must add columns before add rows");
    char buf[1024];

    va_list va;
    va_start(va, format);
    (void) qvsnprintf(buf, sizeof buf, format, va);
    buf[sizeof buf - 1] = '\0';
    va_end(va);
    return *table->newData(buf);
}

void *QTest::qElementData(const char *tagName, int metaTypeId)
{
    QTEST_ASSERT(tagName);
    QTestData *data = QTestResult::currentTestData();
    QTEST_ASSERT(data);
    QTEST_ASSERT(data->parent());

    int idx = data->parent()->indexOf(tagName);
    QTEST_ASSERT(idx != -1);
    QTEST_ASSERT(data->parent()->elementTypeId(idx) == metaTypeId);
    return data->data(data->parent()->indexOf(tagName));
}

bool QTest::compare_helper(bool success, const char *failMsg, char *val1, char *val2, const char *actual,
                           const char *expected, const char *file, int line)
{
    return QTestResult::compare(success, failMsg, val1, val2, actual, expected, file, line);
}

bool QTest::compare_string_helper(const char *t1, const char *t2, const char *actual, const char *expected,
                                  const char *file, int line) {
    return compare_helper(qstrcmp(t1, t2) == 0, "Compare strings are not the same",
                          toString(t1), toString(t2), actual, expected, file, line);
}

bool QTest::qCompare(QStringView t1, QStringView t2, const char *actual, const char *expected, const char *file, int line)
{
    return QTestResult::compare(t1 == t2, "Compared values are not the same", t1, t2, actual, expected, file, line);
}