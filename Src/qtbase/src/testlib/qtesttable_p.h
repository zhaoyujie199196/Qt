#ifndef QTEST_TABLE_P_H
#define QTEST_TABLE_P_H

#include <QtCore/qglobal.h>
#include <QtTest/qtestglobal.h>

QT_BEGIN_NAMESPACE

class QTestTablePrivate;
class QTestData;
class Q_TESTLIB_EXPORT QTestTable
{
public:
    QTestTable();
    ~QTestTable();
    void addColumn(int type, const char *name);
    int elementCount() const;
    QTestData *newData(const char *dataTag);
    int dataCount() const;
    QTestData *testData(int index) const;
    int elementTypeId(int index) const;
    int indexOf(const char *dataTag) const;

    static QTestTable *globalTestTable();
    static QTestTable *currentTestTable();
    static void clearGlobalTestTable();

private:
    Q_DISABLE_COPY(QTestTable)
    QTestTablePrivate *d;
};

QT_END_NAMESPACE

#endif //QTEST_TABLE_P_H