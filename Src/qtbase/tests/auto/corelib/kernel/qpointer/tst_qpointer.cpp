//
// Created by Yujie Zhao on 2023/8/31.
//
#include <QPointer>
#include <QObject>
#include <QTest>

class tst_QPointer : public QObject {
public:
    using QObject::QObject;
    inline tst_QPointer *me() const {
        return const_cast<tst_QPointer *>(this);
    }

    void registerInvokeMethods() override {
        //zhaoyujie TODO
//        REGISTER_OBJECT_INVOKE_METHOD(constructors)
//        REGISTER_OBJECT_INVOKE_METHOD(destructor)
//        REGISTER_OBJECT_INVOKE_METHOD(assigment_operators)
//        REGISTER_OBJECT_INVOKE_METHOD(equality_operators)
//        REGISTER_OBJECT_INVOKE_METHOD(swap)
//        REGISTER_OBJECT_INVOKE_METHOD(isNull)
//        REGISTER_OBJECT_INVOKE_METHOD(dereference_operators)
//        REGISTER_OBJECT_INVOKE_METHOD(disconnect)
    }

    void constructors();
    void destructor();
    void assigment_operators();
    void equality_operators();
    void swap();
    void isNull();
    void dereference_operators();
    void disconnect();
//    void castDuringDestruction();
//    void threadSafety();
//
//    void qvariantCast();
//    void constPointer();
//    void constQPointer();
};

void tst_QPointer::constructors() {
    QPointer<QObject> p1;
    QPointer<QObject> p2(this);
    QPointer<QObject> p3(p2);
    QCOMPARE(p1, QPointer<QObject>(0));
    QCOMPARE(p2, QPointer<QObject>(this));
    QCOMPARE(p3, QPointer<QObject>(this));
}

void tst_QPointer::destructor() {
    // Make two QPointer's to the same object
    QObject *object = new QObject;
    QPointer<QObject> p1 = object;
    QPointer<QObject> p2 = object;
    // Check that they point to the correct object
    QCOMPARE(p1, QPointer<QObject>(object));
    QCOMPARE(p2, QPointer<QObject>(object));
    QCOMPARE(p1, p2);
    // Destroy the guarded object
    delete object;
    // Check that both pointers were zeroed
    QCOMPARE(p1, QPointer<QObject>(0));
    QCOMPARE(p2, QPointer<QObject>(0));
    QCOMPARE(p1, p2);
}

void tst_QPointer::assigment_operators() {
    QPointer<QObject> p1;
    QPointer<QObject> p2;

    // Test assignment with a QObject-derived object pointer
    p1 = this;
    p2 = p1;
    QCOMPARE(p1, QPointer<QObject>(this));
    QCOMPARE(p2, QPointer<QObject>(this));
    QCOMPARE(p1, QPointer<QObject>(p2));

    // Test assignment with a null pointer
    p1 = nullptr;
    p2 = p1;
    QCOMPARE(p1, QPointer<QObject>(0));
    QCOMPARE(p2, QPointer<QObject>(0));
    QCOMPARE(p1, QPointer<QObject>(p2));

    // Test assignment with a real QObject pointer
    QObject *object = new QObject;

    p1 = object;
    p2 = p1;
    QCOMPARE(p1, QPointer<QObject>(object));
    QCOMPARE(p2, QPointer<QObject>(object));
    QCOMPARE(p1, QPointer<QObject>(p2));

    // Test assignment with the same pointer that's already guarded
    p1 = object;
    p2 = p1;
    QCOMPARE(p1, QPointer<QObject>(object));
    QCOMPARE(p2, QPointer<QObject>(object));
    QCOMPARE(p1, QPointer<QObject>(p2));

    // Cleanup
    delete object;
}

void tst_QPointer::equality_operators() {
    QPointer<QObject> p1;
    QPointer<QObject> p2;

    QVERIFY(p1 == p2);

    QObject *object = nullptr;
#ifndef QT_NO_WIDGETS
//    QWidget *widget = nullptr;
#endif

    p1 = object;
    QVERIFY(p1 == p2);
    QVERIFY(p1 == object);
    p2 = object;
    QVERIFY(p2 == p1);
    QVERIFY(p2 == object);

    p1 = this;
    QVERIFY(p1 != p2);
    p2 = p1;
    QVERIFY(p1 == p2);

    // compare to zero
    p1 = nullptr;
    QVERIFY(p1 == 0);
    QVERIFY(0 == p1);
    QVERIFY(p2 != 0);
    QVERIFY(0 != p2);
    QVERIFY(p1 == nullptr);
    QVERIFY(nullptr == p1);
    QVERIFY(p2 != nullptr);
    QVERIFY(nullptr != p2);
    QVERIFY(p1 == object);
    QVERIFY(object == p1);
    QVERIFY(p2 != object);
    QVERIFY(object != p2);
#ifndef QT_NO_WIDGETS
//    QVERIFY(p1 == widget);
//    QVERIFY(widget == p1);
//    QVERIFY(p2 != widget);
//    QVERIFY(widget != p2);
#endif
}

void tst_QPointer::swap() {
    QPointer<QObject> c1, c2;
    {
        QObject o;
        c1 = &o;
        QVERIFY(c2.isNull());
        QCOMPARE(c1.data(), &o);
        c1.swap(c2);
        QVERIFY(c1.isNull());
        QCOMPARE(c2.data(), &o);
    }
    QVERIFY(c1.isNull());
    QVERIFY(c2.isNull());
}

void tst_QPointer::isNull() {
    QPointer<QObject> p1;
    QVERIFY(p1.isNull());
    p1 = this;
    QVERIFY(!p1.isNull());
    p1 = nullptr;
    QVERIFY(p1.isNull());
}

void tst_QPointer::dereference_operators() {
    QPointer<tst_QPointer> p1 = this;
    QPointer<tst_QPointer> p2;

    // operator->() -- only makes sense if not null
    QObject *object = p1->me();
    QCOMPARE(object, this);

    // operator*() -- only makes sense if not null
    QObject &ref = *p1;
    QCOMPARE(&ref, this);

    // operator T*()
    QCOMPARE(static_cast<QObject *>(p1), this);
    QCOMPARE(static_cast<QObject *>(p2), static_cast<QObject *>(0));

    // data()
    QCOMPARE(p1.data(), this);
    QCOMPARE(p2.data(), static_cast<QObject *>(0));
}

void tst_QPointer::disconnect() {
    // Verify that pointer remains guarded when all signals are disconencted.
//    QPointer<QObject> p1 = new QObject;
//    QVERIFY(!p1.isNull());
//    p1->disconnect();
//    QVERIFY(!p1.isNull());
//    delete static_cast<QObject *>(p1);
//    QVERIFY(p1.isNull());
}

QTEST_APPLESS_MAIN(tst_QPointer)

