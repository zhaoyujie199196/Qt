//
// Created by Yujie Zhao on 2023/10/20.
//
#include <QTest>
#include <QObject>
#include <QList>
#include "tst_qobjectHelper.h"

/*
 * 问题：
 * signalVector的index为什么从-1开始
 * orphan的机制
 * QObject中的各种锁和线程安全机制
 * QueueConnection与BlockQueueConnection的机制
 * QProperty用法
 * */

int ReceiverObject::sequence = 0;

class tst_QObject : public QObject
{
public:
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(test)
        REGISTER_OBJECT_INVOKE_METHOD(disconnect)
    }

    void test();
    void disconnect();
};

void tst_QObject::test() {

}

void tst_QObject::disconnect() {
    SenderObject s;
    ReceiverObject r1;
    ReceiverObject r2;

    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));

    connect(&s, SIGNAL(signal2()), &r1, SLOT(slot2()));
    connect(&s, SIGNAL(signal3()), &r1, SLOT(slot3()));
    connect(&s, SIGNAL(signal4()), &r1, SLOT(slot4()));

    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QVERIFY(r1.called(1));
    QVERIFY(r1.called(2));
    QVERIFY(r1.called(3));
    QVERIFY(r1.called(4));
    r1.reset();

    bool ret = QObject::disconnect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));

    s.emitSignal1();

    QVERIFY(!r1.called(1));
    r1.reset();

    QVERIFY(ret);
    ret = QObject::disconnect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));
    QVERIFY(!ret);

    QObject::disconnect(&s, 0, &r1, 0);

    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QVERIFY(!r1.called(2));
    QVERIFY(!r1.called(3));
    QVERIFY(!r1.called(4));
    r1.reset();

    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));
    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot2()));
    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot3()));
    connect(&s, SIGNAL(signal2()), &r1, SLOT(slot4()));

    //断开r1所有和s的signal1的连接
    QObject::disconnect(&s, SIGNAL(signal1()), &r1, 0);

    s.emitSignal1();
    s.emitSignal2();

    QVERIFY(!r1.called(1));
    QVERIFY(!r1.called(2));
    QVERIFY(!r1.called(3));
    QVERIFY(r1.called(4));
    r1.reset();
    // make sure all is disconnected again
    QObject::disconnect(&s, 0, &r1, 0);

    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));
    connect(&s, SIGNAL(signal1()), &r2, SLOT(slot1()));
    connect(&s, SIGNAL(signal2()), &r1, SLOT(slot2()));
    connect(&s, SIGNAL(signal2()), &r2, SLOT(slot2()));
    connect(&s, SIGNAL(signal3()), &r1, SLOT(slot3()));
    connect(&s, SIGNAL(signal3()), &r2, SLOT(slot3()));

    // 断开所有和s的signal1的连接
    QObject::disconnect(&s, SIGNAL(signal1()), 0, 0);
    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();

    QVERIFY(!r1.called(1));
    QVERIFY(!r2.called(1));
    QVERIFY(r1.called(2));
    QVERIFY(r2.called(2));
    QVERIFY(r1.called(2));
    QVERIFY(r2.called(2));

    r1.reset();
    r2.reset();

    //断开所有和s的连接
    QObject::disconnect(&s, 0, 0, 0);

    QVERIFY(!r1.called(2));
    QVERIFY(!r2.called(2));
    QVERIFY(!r1.called(2));
    QVERIFY(!r2.called(2));

}

QTEST_APPLESS_MAIN(tst_QObject)