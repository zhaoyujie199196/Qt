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
 * 在触发函数中删除了当前的Connection会出现什么情况？
 * QDynamicMetaObject是什么作用？
 * */

int ReceiverObject::sequence = 0;

class tst_QObject : public QObject
{
public:
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(test)
        REGISTER_OBJECT_INVOKE_METHOD(disconnect)
        REGISTER_OBJECT_INVOKE_METHOD(connectSlotsByName)
        REGISTER_OBJECT_INVOKE_METHOD(connectSignalsToSignalsWithDefaultArguments)
        REGISTER_OBJECT_INVOKE_METHOD(receivers)
//        REGISTER_OBJECT_INVOKE_METHOD(normalize)
//        REGISTER_OBJECT_INVOKE_METHOD(childDeletesItsSibling)
    }

    void test();
    void disconnect();
    void connectSlotsByName();
    void connectSignalsToSignalsWithDefaultArguments();
    void receivers();
//    void normalize();
//    void childDeletesItsSibling();
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

void tst_QObject::connectSlotsByName()
{
    AutoConnectReceiver receiver;
    receiver.setObjectName("Receiver");
    AutoConnectSender sender(&receiver);
    sender.setObjectName("Sender");

    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::connectSlotsByName: No matching signal for on_child_signal()");
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::connectSlotsByName: Connecting slot on_Sender_signalManyParams() with the first of the following compatible signals: QList(\"signalManyParams(int,int,int,QString,bool)\", \"signalManyParams(int,int,int,QString,bool,bool)\")");
    QMetaObject::connectSlotsByName(&receiver);

    receiver.called_slots.clear();
    sender.emitSignalNoParams();
    QCOMPARE(receiver.called_slots, QList<int>() << 1);

    receiver.called_slots.clear();
    sender.emitSignalWithParams(0);
    QCOMPARE(receiver.called_slots, QList<int>() << 2);

    receiver.called_slots.clear();
    sender.emitSignalWithParams(0, "string");
    QCOMPARE(receiver.called_slots, QList<int>() << 3);

    receiver.called_slots.clear();
    sender.emitSignalManyParams(1, 2, 3, "string", true);
    sender.emitSignalManyParams(1, 2, 3, "string", true, false);

    QCOMPARE(receiver.called_slots, QList<int>() << 4 << 5 << 6);

    receiver.called_slots.clear();
    sender.emitSignalManyParams2(1, 2, 3, "string", true);
    QCOMPARE(receiver.called_slots, QList<int>() << 7);

    receiver.called_slots.clear();
    sender.emitSignalLoopBack();
    QCOMPARE(receiver.called_slots, QList<int>() << 8);

    receiver.called_slots.clear();
    receiver.emitSignalNoParams();
    QCOMPARE(receiver.called_slots, QList<int>() << 9);

    receiver.called_slots.clear();
    receiver.emit_signal_with_underscore();
    QCOMPARE(receiver.called_slots, QList<int>() << 10);
}

void tst_QObject::connectSignalsToSignalsWithDefaultArguments()
{
    DefaultArguments o;
    connect(&o, SIGNAL(theOriginalSignal()), &o, SIGNAL(theSecondSignal()));
    connect(&o, SIGNAL(theSecondSignal(QString)), &o, SLOT(theSlot(QString)));
    QVERIFY( o.result.isEmpty() );
    o.emitTheSecondSignal();
    QCOMPARE(o.result, QString("secondDefault"));
    o.result = "Not called";
    o.emitTheOriginalSignal();
    QCOMPARE(o.result, QString("secondDefault"));
}

void tst_QObject::receivers()
{
    class Object : public QObject
    {
    public:
        int receivers(const char* signal) const {
            return QObject::receivers(signal);
        }
    };

    Object object;
    QCOMPARE(object.receivers(SIGNAL(destroyed())), 0);
    object.connect(&object, SIGNAL(destroyed()), SLOT(deleteLater()));
    QCOMPARE(object.receivers(SIGNAL(destroyed())), 1);
    object.connect(&object, SIGNAL(destroyed()), SLOT(deleteLater()));
    QCOMPARE(object.receivers(SIGNAL(destroyed())), 2);
    object.disconnect(SIGNAL(destroyed()), &object, SLOT(deleteLater()));
    QCOMPARE(object.receivers(SIGNAL(destroyed())), 0);
}
//
//void tst_QObject::normalize()
//{
//    NormalizeObject object;
//
//    // unsigned int -> uint, unsigned long -> ulong
//    QVERIFY(object.connect(&object,
//                           SIGNAL(uintPointerSignal(uint *)),
//                           SLOT(uintPointerSlot(uint *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(uintPointerSignal(unsigned int *)),
//                           SLOT(uintPointerSlot(uint *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(uintPointerSignal(uint *)),
//                           SLOT(uintPointerSlot(unsigned int *))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constUintPointerSignal(const uint *)),
//                    SLOT(constUintPointerSlot(const uint *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constUintPointerSignal(const unsigned int *)),
//                    SLOT(constUintPointerSlot(const uint *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constUintPointerSignal(const uint *)),
//                    SLOT(constUintPointerSlot(const unsigned int *))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(ulongPointerSignal(ulong *)),
//                           SLOT(ulongPointerSlot(ulong *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(ulongPointerSignal(unsigned long *)),
//                           SLOT(ulongPointerSlot(ulong *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(ulongPointerSignal(ulong *)),
//                           SLOT(ulongPointerSlot(unsigned long *))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constUlongPointerSignal(const ulong *)),
//                    SLOT(constUlongPointerSlot(const ulong *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constUlongPointerSignal(const unsigned long *)),
//                    SLOT(constUlongPointerSlot(const ulong *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constUlongPointerSignal(const ulong *)),
//                    SLOT(constUlongPointerSlot(const unsigned long *))));
//
//    // struct, class, and enum are optional
//    QVERIFY(object.connect(&object,
//                           SIGNAL(structSignal(struct Struct)),
//                    SLOT(structSlot(struct Struct))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(structSignal(Struct)),
//                           SLOT(structSlot(struct Struct))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(structSignal(struct Struct)),
//                    SLOT(structSlot(Struct))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(classSignal(class Class)),
//                    SLOT(classSlot(class Class))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(classSignal(Class)),
//                           SLOT(classSlot(class Class))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(classSignal(class Class)),
//                    SLOT(classSlot(Class))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(enumSignal(enum Enum)),
//                    SLOT(enumSlot(enum Enum))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(enumSignal(Enum)),
//                           SLOT(enumSlot(enum Enum))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(enumSignal(enum Enum)),
//                    SLOT(enumSlot(Enum))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(structPointerSignal(struct Struct *)),
//                    SLOT(structPointerSlot(struct Struct *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(structPointerSignal(Struct *)),
//                           SLOT(structPointerSlot(struct Struct *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(structPointerSignal(struct Struct *)),
//                    SLOT(structPointerSlot(Struct *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(classPointerSignal(class Class *)),
//                    SLOT(classPointerSlot(class Class *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(classPointerSignal(Class *)),
//                           SLOT(classPointerSlot(class Class *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(classPointerSignal(class Class *)),
//                    SLOT(classPointerSlot(Class *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(enumPointerSignal(enum Enum *)),
//                    SLOT(enumPointerSlot(enum Enum *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(enumPointerSignal(Enum *)),
//                           SLOT(enumPointerSlot(enum Enum *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(enumPointerSignal(enum Enum *)),
//                    SLOT(enumPointerSlot(Enum *))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerSignal(const struct Struct *)),
//                    SLOT(constStructPointerSlot(const struct Struct *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerSignal(const Struct *)),
//                    SLOT(constStructPointerSlot(const struct Struct *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerSignal(const struct Struct *)),
//                    SLOT(constStructPointerSlot(const Struct *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerSignal(const class Class *)),
//                    SLOT(constClassPointerSlot(const class Class *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerSignal(const Class *)),
//                    SLOT(constClassPointerSlot(const class Class *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerSignal(const class Class *)),
//                    SLOT(constClassPointerSlot(const Class *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerSignal(const enum Enum *)),
//                    SLOT(constEnumPointerSlot(const enum Enum *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerSignal(const Enum *)),
//                    SLOT(constEnumPointerSlot(const enum Enum *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerSignal(const enum Enum *)),
//                    SLOT(constEnumPointerSlot(const Enum *))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerSignal(struct Struct const *)),
//                    SLOT(constStructPointerSlot(struct Struct const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerSignal(Struct const *)),
//                    SLOT(constStructPointerSlot(struct Struct const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerSignal(struct Struct const *)),
//                    SLOT(constStructPointerSlot(Struct const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerSignal(class Class const *)),
//                    SLOT(constClassPointerSlot(class Class const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerSignal(Class const *)),
//                    SLOT(constClassPointerSlot(class Class const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerSignal(class Class const *)),
//                    SLOT(constClassPointerSlot(Class const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerSignal(enum Enum const *)),
//                    SLOT(constEnumPointerSlot(enum Enum const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerSignal(Enum const *)),
//                    SLOT(constEnumPointerSlot(enum Enum const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerSignal(enum Enum const *)),
//                    SLOT(constEnumPointerSlot(Enum const *))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerConstPointerSignal(const struct Struct * const *)),
//                    SLOT(constStructPointerConstPointerSlot(const struct Struct * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerConstPointerSignal(const Struct * const *)),
//                    SLOT(constStructPointerConstPointerSlot(const struct Struct * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerConstPointerSignal(const struct Struct * const *)),
//                    SLOT(constStructPointerConstPointerSlot(const Struct * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerConstPointerSignal(const class Class * const *)),
//                    SLOT(constClassPointerConstPointerSlot(const class Class * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerConstPointerSignal(const Class * const *)),
//                    SLOT(constClassPointerConstPointerSlot(const class Class * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerConstPointerSignal(const class Class * const *)),
//                    SLOT(constClassPointerConstPointerSlot(const Class * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerConstPointerSignal(const enum Enum * const *)),
//                    SLOT(constEnumPointerConstPointerSlot(const enum Enum * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerConstPointerSignal(const Enum * const *)),
//                    SLOT(constEnumPointerConstPointerSlot(const enum Enum * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerConstPointerSignal(const enum Enum * const *)),
//                    SLOT(constEnumPointerConstPointerSlot(const Enum * const *))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerConstPointerSignal(struct Struct const * const *)),
//                    SLOT(constStructPointerConstPointerSlot(struct Struct const * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerConstPointerSignal(Struct const * const *)),
//                    SLOT(constStructPointerConstPointerSlot(struct Struct const * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constStructPointerConstPointerSignal(struct Struct const * const *)),
//                    SLOT(constStructPointerConstPointerSlot(Struct const * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerConstPointerSignal(class Class const * const *)),
//                    SLOT(constClassPointerConstPointerSlot(class Class const * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerConstPointerSignal(Class const * const *)),
//                    SLOT(constClassPointerConstPointerSlot(class Class const * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constClassPointerConstPointerSignal(class Class const * const *)),
//                    SLOT(constClassPointerConstPointerSlot(Class const * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerConstPointerSignal(enum Enum const * const *)),
//                    SLOT(constEnumPointerConstPointerSlot(enum Enum const * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerConstPointerSignal(Enum const * const *)),
//                    SLOT(constEnumPointerConstPointerSlot(enum Enum const * const *))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constEnumPointerConstPointerSignal(enum Enum const * const *)),
//                    SLOT(constEnumPointerConstPointerSlot(Enum const * const *))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(unsignedintSignal(unsigned int)),
//                           SLOT(unsignedintSlot(unsigned int))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(unsignedSignal(unsigned)),
//                           SLOT(unsignedSlot(unsigned))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(unsignedSignal(unsigned)),
//                           SLOT(uintSlot(uint))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(unsignedlongSignal(unsigned long)),
//                           SLOT(unsignedlongSlot(unsigned long))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(unsignedlonglongSignal(quint64)),
//                           SLOT(unsignedlonglongSlot(quint64))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(unsignedlongintSignal(unsigned long int)),
//                           SLOT(unsignedlongintSlot(unsigned long int))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(unsignedshortSignal(unsigned short)),
//                           SLOT(unsignedshortSlot(unsigned short))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(unsignedcharSignal(unsigned char)),
//                           SLOT(unsignedcharSlot(unsigned char))));
//
//    // connect when original template signature and mixed usage of 'T<C const &> const &',
//    // 'const T<const C &> &', and 'T<const C &>'
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeRefSignal(Template<Class &> &)),
//                           SLOT(typeRefSlot(Template<Class &> &))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
//                    SLOT(constTypeRefSlot(const Template<const Class &> &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
//                    SLOT(constTypeRefSlot(const Template<Class const &> &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
//                    SLOT(constTypeRefSlot(Template<Class const &> const &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(Template<const Class &> const &)),
//                    SLOT(constTypeRefSlot(Template<Class const &> const &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(Template<Class const &> const &)),
//                    SLOT(constTypeRefSlot(Template<Class const &> const &))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
//                    SLOT(typeConstRefSlot(const Template<const Class &> &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
//                    SLOT(typeConstRefSlot(const Template<Class const &> &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
//                    SLOT(typeConstRefSlot(Template<Class const &> const &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(Template<const Class &> const &)),
//                    SLOT(typeConstRefSlot(Template<Class const &> const &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(constTypeRefSignal(Template<Class const &> const &)),
//                    SLOT(typeConstRefSlot(Template<Class const &> const &))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
//                    SLOT(constTypeRefSlot(const Template<const Class &> &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
//                    SLOT(constTypeRefSlot(const Template<Class const &> &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
//                    SLOT(constTypeRefSlot(Template<Class const &> const &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(Template<const Class &> const &)),
//                    SLOT(constTypeRefSlot(Template<Class const &> const &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(Template<Class const &> const &)),
//                    SLOT(constTypeRefSlot(Template<Class const &> const &))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
//                    SLOT(typeConstRefSlot(const Template<const Class &> &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
//                    SLOT(typeConstRefSlot(const Template<Class const &> &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
//                    SLOT(typeConstRefSlot(Template<Class const &> const &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(Template<const Class &> const &)),
//                    SLOT(typeConstRefSlot(Template<Class const &> const &))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typeConstRefSignal(Template<Class const &> const &)),
//                    SLOT(typeConstRefSlot(Template<Class const &> const &))));
//
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typePointerConstRefSignal(Class*const&)),
//                    SLOT(typePointerConstRefSlot(Class*const&))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typePointerConstRefSignal(Class*const&)),
//                    SLOT(typePointerConstRefSlot(Class*))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typePointerConstRefSignal(Class*)),
//                           SLOT(typePointerConstRefSlot(Class*const&))));
//    QVERIFY(object.connect(&object,
//                           SIGNAL(typePointerConstRefSignal(Class*)),
//                           SLOT(typePointerConstRefSlot(Class*))));
//
//    QVERIFY( connect(&object, SIGNAL(constTemplateSignal1(Template <int>)),
//                     &object , SLOT(constTemplateSlot1 (Template<int > )  ) ));
//    QVERIFY( connect(&object, SIGNAL(constTemplateSignal1(Template <int>)),
//                     &object , SLOT(constTemplateSlot2 (Template<int > )  ) ));
//    QVERIFY( connect(&object, SIGNAL(constTemplateSignal2(Template <const int>)),
//                     &object , SLOT(constTemplateSlot3(Template<int const > ) ) ));
//
//    //type does not match
//    QTest::ignoreMessage(QtWarningMsg, "QObject::connect: Incompatible sender/receiver arguments\n"
//                                       "        NormalizeObject::constTemplateSignal1(Template<int>) --> NormalizeObject::constTemplateSlot3(Template<const int>)");
//    QVERIFY(!connect(&object, SIGNAL(constTemplateSignal1(Template <int>)),
//                     &object , SLOT(constTemplateSlot3(Template<int const> ) ) ));
//}
//
//class SiblingDeleter : public QObject
//{
//public:
//    inline SiblingDeleter(QObject *sibling, QObject *parent)
//            : QObject(parent), sibling(sibling) {
//
//    }
//    inline virtual ~SiblingDeleter() {
//        delete sibling;
//    }
//
//private:
//    QPointer<QObject> sibling;
//};
//
//void tst_QObject::childDeletesItsSibling()
//{
//    QObject *commonParent = new QObject(0);
//    QPointer<QObject> child = new QObject(0);
//    QPointer<QObject> siblingDeleter = new SiblingDeleter(child, commonParent);
//    child->setParent(commonParent);
//    delete commonParent; // don't crash
//    QVERIFY(!child);
//    QVERIFY(!siblingDeleter);
//}

QTEST_APPLESS_MAIN(tst_QObject)