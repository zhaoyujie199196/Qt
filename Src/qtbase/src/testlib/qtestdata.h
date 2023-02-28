//
// Created by Yujie Zhao on 2023/2/16.
//

#ifndef QTESTDATA_H
#define QTESTDATA_H

#include <QtTest/qtestglobal.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_NAMESPACE

class QTestTable;
class QTestDataPrivate;
class Q_TESTLIB_EXPORT QTestData {
public:
    QTestData(const char *tag, QTestTable *parent);
    ~QTestData();
    void append(int type, const void *data);
    QTestTable *parent() const;
    void *data(int index) const;

private:
    QTestDataPrivate *d;
};

template <typename T>
QTestData &operator<<(QTestData &data, const T &value)
{
    data.append(qMetaTypeId<T>(), &value);
    return data;
}

QT_END_NAMESPACE


#endif //QTESTDATA_H
