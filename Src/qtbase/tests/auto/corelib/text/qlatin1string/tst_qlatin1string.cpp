//
// Created by Yujie Zhao on 2023/3/1.
//

#include <iostream>
#include <QtTest/QTest>
#include <QString>
#include <QObject>
#include <QtCore/QByteArrayView>

struct QLatin1StringContainer {
    QLatin1String l1;
};
Q_DECLARE_TYPEINFO(QLatin1StringContainer, Q_RELOCATABLE_TYPE)
Q_DECLARE_METATYPE(QLatin1StringContainer)

class tst_QLatin1String : public QObject {
public:
    using QObject::QObject;
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(at)
        REGISTER_OBJECT_INVOKE_METHOD(arg)
        REGISTER_OBJECT_INVOKE_METHOD(midLeftRight)
        REGISTER_OBJECT_INVOKE_METHOD(nullString)
        REGISTER_OBJECT_INVOKE_METHOD(emptyString)
        REGISTER_OBJECT_INVOKE_METHOD(iterators)
        REGISTER_OBJECT_INVOKE_METHOD(relationalOperators_data)
        REGISTER_OBJECT_INVOKE_METHOD(relationalOperators)
    }

    void at();
    void arg() const {}
    void midLeftRight();
    void nullString();
    void emptyString();
    void iterators();
    void relationalOperators_data();
    void relationalOperators();
};

void tst_QLatin1String::at() {
    const QLatin1String l1("Hello World");
    QCOMPARE(l1.at(0), QLatin1Char('H'));
    QCOMPARE(l1.at(l1.size() - 1), QLatin1Char('d'));
    QCOMPARE(l1[0], QLatin1Char('H'));
    QCOMPARE(l1[l1.size() - 1], QLatin1Char('d'));
}

void tst_QLatin1String::midLeftRight() {
    const QLatin1String l1("Hello World");
    QCOMPARE(l1.mid(0), l1);
    QCOMPARE(l1.mid(0, l1.size()), l1);
    QCOMPARE(l1.left(l1.size()), l1);
    QCOMPARE(l1.right(l1.size()), l1);

    QCOMPARE(l1.mid(6), QLatin1String("World"));
    QCOMPARE(l1.mid(6, 5), QLatin1String("World"));
    QCOMPARE(l1.right(5), QLatin1String("World"));

    QCOMPARE(l1.mid(6, 1), QLatin1String("W"));
    QCOMPARE(l1.right(5).left(1), QLatin1String("W"));
    QCOMPARE(l1.left(5), QLatin1String("Hello"));
}

void tst_QLatin1String::nullString() {
    {
        QLatin1String l1;
        QCOMPARE(static_cast<const void*>(l1.data()), static_cast<const void*>(nullptr));
        QCOMPARE(l1.size(), 0);
        //zhaoyujie TODO
    }
    {
        const char *null = nullptr;
        QLatin1String l1(null);
        QCOMPARE(static_cast<const void*>(l1.data()), static_cast<const void*>(nullptr));
        QCOMPARE(l1.size(), 0);
        //zhaoyujie TODO
    }
}

void tst_QLatin1String::emptyString() {
    {
        const char *empty = "";
        QLatin1String l1(empty);
        QCOMPARE(static_cast<const void*>(l1.data()), static_cast<const void*>(empty));
        QCOMPARE(l1.size(), 0);
        //zhaoyujie TODO
    }
    {
        const char *notEmpty = "foo";
        QLatin1String l1(notEmpty, qsizetype(0));
        QCOMPARE(static_cast<const void*>(l1.data()), static_cast<const void*>(notEmpty));
        QCOMPARE(l1.size(), 0);
        //zhaoyujie TODO
    }
}

void tst_QLatin1String::iterators() {
    QLatin1String hello("hello");
    QLatin1String olleh("olleh");
    QVERIFY(std::equal(hello.begin(), hello.end(), olleh.rbegin()));
}

void tst_QLatin1String::relationalOperators_data() {
    QTest::addColumn<QLatin1StringContainer>("lhs");
    QTest::addColumn<int>("lhsOrderNumber");
    QTest::addColumn<QLatin1StringContainer>("rhs");
    QTest::addColumn<int>("rhsOrderNumber");
    struct Data {
        QLatin1String l1;
        int order;
    };
    Data data[] = {
            {QLatin1String(), 0},
            {QLatin1String(""), 0},
            {QLatin1String("a"), 1},
            {QLatin1String("aa"), 2},
            {QLatin1String("b"), 3}
    };

    for (Data *lhs = data; lhs != data + sizeof(data) / sizeof(*data); ++lhs) {
        for (Data *rhs = data; rhs != data + sizeof(data) / sizeof(*data); ++rhs) {
            QLatin1StringContainer l = {lhs->l1};
            QLatin1StringContainer r = {rhs->l1};
            auto &rowData = QTest::addRow("\"%s\" <> \"%s\"",
                          lhs->l1.data() ? lhs->l1.data() : "nullptr",
                          rhs->l1.data() ? rhs->l1.data() : "nullptr");
            rowData << l << lhs->order << r << rhs->order;
        }
    }
}

void tst_QLatin1String::relationalOperators() {
    QFETCH(QLatin1StringContainer, lhs);
    QFETCH(int, lhsOrderNumber);
    QFETCH(QLatin1StringContainer, rhs);
    QFETCH(int, rhsOrderNumber);

#define CHECK(op) \
    QCOMPARE(lhs.l1 op rhs.l1, lhsOrderNumber op rhsOrderNumber) \
    /*end*/

    CHECK(==);
    CHECK(!=);
    CHECK(< );
    CHECK(> );
    CHECK(<=);
    CHECK(>=);
#undef CHECK
}

QTEST_APPLESS_MAIN(tst_QLatin1String)
