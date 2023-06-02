//
// Created by Yujie Zhao on 2023/2/16.
//

#include "qtestdata.h"
#include <QtTest/private/qtesttable_p.h>
#include <QtCore/qbytearray.h>
#include <QtTest/qtestassert.h>
#include <QtCore/QMetaType>
#include <QtCore/qlatin1string.h>
#include <QtCore/qobjectdefs.h>

QT_BEGIN_NAMESPACE

struct QTestDataPrivate {
    char *tag = nullptr;
    //存放指针的数组
    void **data = nullptr;
    QTestTable * parent = nullptr;
    int dataCount = 0;
};

QTestData::QTestData(const char *tag, QTestTable *parent)
{
    d = new QTestDataPrivate;
    //这里需要复制tag，不然销毁char *时，这里会变成野指针
    d->tag = qstrdup(tag);
    //数量为table的列数量
    d->data = new void *[parent->elementCount()];
    d->parent = parent;
}

QTestData::~QTestData()
{
    delete [] d->tag;
    delete [] d->data;
    delete d;
}

void QTestData::append(int type, const void *data)
{
    QTEST_ASSERT(d->dataCount < d->parent->elementCount());
    int expectedType = d->parent->elementTypeId(d->dataCount);
    if (expectedType != type) {
        Q_ASSERT(false);
    }
    d->data[d->dataCount] = QMetaType(type).create(data);
    ++d->dataCount;
}

QTestTable *QTestData::parent() const
{
    return d->parent;
}

void *QTestData::data(int index) const
{
    Q_ASSERT(index >= 0 && index < d->dataCount);
    return d->data[index];
}

QT_END_NAMESPACE
