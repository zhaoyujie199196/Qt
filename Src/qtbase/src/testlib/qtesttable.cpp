#include <QtTest/private/qtesttable_p.h>
#include <QtTest/qtestassert.h>
#include <QtTest/qtestdata.h>
#include <vector>

QT_BEGIN_NAMESPACE

class QTestTablePrivate {
public:
    struct Element {
        Element() = default;
        Element(const char *n, int t) : name(n), type(t) {}
        const char *name = nullptr;
        int type = 0;
    };

    using ElementList = std::vector<Element>;
    ElementList elementList;

    using DataList = std::vector<QTestData *>;
    DataList dataList;

    void addColumn(int elemType, const char *elemName) {
        elementList.push_back(Element(elemName, elemType));
    }
    void addRow(QTestData *data) {
        dataList.push_back(data);
    }

    static QTestTable *currentTestTable;
    static QTestTable *globalTestTable;
};

QTestTable *QTestTablePrivate::currentTestTable = nullptr;
QTestTable *QTestTablePrivate::globalTestTable = nullptr;

QTestTable::QTestTable()
{
    d = new QTestTablePrivate;
    QTestTablePrivate::currentTestTable = this;
}

QTestTable::~QTestTable()
{
    for (auto data : d->dataList) {
        delete data;
        d->dataList.clear();
    }
    QTestTablePrivate::currentTestTable = nullptr;
    delete d;
}

void QTestTable::addColumn(int type, const char *name)
{
    QTEST_ASSERT(type);
    QTEST_ASSERT(name);
    d->addColumn(type, name);
}

int QTestTable::elementCount() const
{
    return d->elementList.size();
}

QTestData *QTestTable::newData(const char *dataTag)
{
    QTestData *data = new QTestData(dataTag, this);
    d->addRow(data);
    return data;
}

int QTestTable::dataCount() const
{
    return d->dataList.size();
}

QTestData *QTestTable::testData(int index) const
{
    Q_ASSERT(index >= 0 && index < dataCount());
    return d->dataList[index];
}

int QTestTable::elementTypeId(int index) const
{
    if (index < d->elementList.size()) {
        return d->elementList[index].type;
    }
    else {
        QTEST_ASSERT(false);
        return -1;
    }
}

int QTestTable::indexOf(const char *dataTag) const
{
    auto &elemList = d->elementList;
    using Elem = QTestTablePrivate::Element;
    auto it = std::find_if(elemList.begin(), elemList.end(), [dataTag](const Elem &element) {
        return std::strcmp(dataTag, element.name) == 0;
    });
    return it == elemList.end() ? -1 : (it - elemList.begin());
}

QTestTable *QTestTable::currentTestTable()
{
    return QTestTablePrivate::currentTestTable;
}

QTestTable *QTestTable::globalTestTable()
{
    return QTestTablePrivate::globalTestTable;
}

QT_END_NAMESPACE