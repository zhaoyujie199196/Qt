#include "QtCore/QObject"
#include "QtTest/QTest"
#include "QtCore/QMargins"

class tst_QMargins : public QObject
{
public:
    using QObject::QObject;

    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(getSetCheck)
        REGISTER_OBJECT_INVOKE_METHOD(dataStreamCheck)
        REGISTER_OBJECT_INVOKE_METHOD(operators)
        REGISTER_OBJECT_INVOKE_METHOD(debugStreamCheck)
        REGISTER_OBJECT_INVOKE_METHOD(getSetCheckF)
        REGISTER_OBJECT_INVOKE_METHOD(dataStreamCheckF)
        REGISTER_OBJECT_INVOKE_METHOD(operatorsF)
        REGISTER_OBJECT_INVOKE_METHOD(debugStreamCheckF)
        REGISTER_OBJECT_INVOKE_METHOD(structuredBinding)
    }

    void getSetCheck();
    void dataStreamCheck();
    void operators();
    void debugStreamCheck();
    void getSetCheckF();
    void dataStreamCheckF();
    void operatorsF();
    void debugStreamCheckF();
    void structuredBinding();
};

void tst_QMargins::getSetCheck()
{
    QMargins margins;
    // int QMargins::width()
    // void QMargins::setWidth(int)
    margins.setLeft(0);
    QCOMPARE(0, margins.left());
    margins.setTop(INT_MIN);
    QCOMPARE(INT_MIN, margins.top());
    margins.setBottom(INT_MAX);
    QCOMPARE(INT_MAX, margins.bottom());
    margins.setRight(INT_MAX);
    QCOMPARE(INT_MAX, margins.right());

    margins = QMargins();
    QVERIFY(margins.isNull());
    margins.setLeft(5);
    margins.setRight(5);
    QVERIFY(!margins.isNull());
    QCOMPARE(margins, QMargins(5, 0, 5, 0));
}

void tst_QMargins::operators()
{
    const QMargins m1(12, 14, 16, 18);
    const QMargins m2(2, 3, 4, 5);

    const QMargins added = m1 + m2;
    QCOMPARE(added, QMargins(14, 17, 20, 23));
    QMargins a = m1;
    a += m2;
    QCOMPARE(a, added);

    const QMargins subtracted = m1 - m2;
    QCOMPARE(subtracted, QMargins(10, 11, 12, 13));
    a = m1;
    a -= m2;
    QCOMPARE(a, subtracted);

    QMargins h = m1;
    h += 2;
    QCOMPARE(h, QMargins(14, 16, 18, 20));
    h -= 2;
    QCOMPARE(h, m1);

    const QMargins doubled = m1 * 2;
    QCOMPARE(doubled, QMargins(24, 28, 32, 36));
    QCOMPARE(2 * m1, doubled);
    QCOMPARE(qreal(2) * m1, doubled);
    QCOMPARE(m1 * qreal(2), doubled);

    a = m1;
    a *= 2;
    QCOMPARE(a, doubled);
    a = m1;
    a *= qreal(2);
    QCOMPARE(a, doubled);

    const QMargins halved = m1 / 2;
    QCOMPARE(halved, QMargins(6, 7, 8, 9));

    a = m1;
    a /= 2;
    QCOMPARE(a, halved);
    a = m1;
    a /= qreal(2);
    QCOMPARE(a, halved);

    QCOMPARE(m1 + (-m1), QMargins());

    QMargins m3 = QMargins(10, 11, 12, 13);
    QCOMPARE(m3 + 1, QMargins(11, 12, 13, 14));
    QCOMPARE(1 + m3, QMargins(11, 12, 13, 14));
    QCOMPARE(m3 - 1, QMargins(9, 10, 11, 12));
    QCOMPARE(+m3, QMargins(10, 11, 12, 13));
    QCOMPARE(-m3, QMargins(-10, -11, -12, -13));
}

void tst_QMargins::debugStreamCheck()
{
    //zhaoyujie TODO
//    QMargins m(10, 11, 12, 13);
//    const QString expected = "QMargins(10, 11, 12, 13)";
//    QString result;
//    QDebug(&result).nospace() << m;
//    QCOMPARE(result, expected);
}

void tst_QMargins::dataStreamCheck()
{
    //zhaoyujie TODO
//    QByteArray buffer;
//
//    // stream out
//    {
//        QMargins marginsOut(0,INT_MIN,INT_MAX,6852);
//        QDataStream streamOut(&buffer, QIODevice::WriteOnly);
//        streamOut << marginsOut;
//    }
//
//    // stream in & compare
//    {
//        QMargins marginsIn;
//        QDataStream streamIn(&buffer, QIODevice::ReadOnly);
//        streamIn >> marginsIn;
//
//        QCOMPARE(marginsIn.left(), 0);
//        QCOMPARE(marginsIn.top(), INT_MIN);
//        QCOMPARE(marginsIn.right(), INT_MAX);
//        QCOMPARE(marginsIn.bottom(), 6852);
//    }
}

void tst_QMargins::getSetCheckF()
{
    QMarginsF margins;
    // int QMarginsF::width()
    // void QMarginsF::setWidth(int)
    margins.setLeft(1.1);
    QCOMPARE(1.1, margins.left());
    margins.setTop(2.2);
    QCOMPARE(2.2, margins.top());
    margins.setBottom(3.3);
    QCOMPARE(3.3, margins.bottom());
    margins.setRight(4.4);
    QCOMPARE(4.4, margins.right());

    margins = QMarginsF();
    QVERIFY(margins.isNull());
    margins.setLeft(5.5);
    margins.setRight(5.5);
    QVERIFY(!margins.isNull());
    QCOMPARE(margins, QMarginsF(5.5, 0.0, 5.5, 0.0));
}

void tst_QMargins::operatorsF()
{
    const QMarginsF m1(12.1, 14.1, 16.1, 18.1);
    const QMarginsF m2(2.1, 3.1, 4.1, 5.1);

    const QMarginsF added = m1 + m2;
    QCOMPARE(added, QMarginsF(14.2, 17.2, 20.2, 23.2));
    QMarginsF a = m1;
    a += m2;
    QCOMPARE(a, added);

    const QMarginsF subtracted = m1 - m2;
    QCOMPARE(subtracted, QMarginsF(10.0, 11.0, 12.0, 13.0));
    a = m1;
    a -= m2;
    QCOMPARE(a, subtracted);

    QMarginsF h = m1;
    h += 2.1;
    QCOMPARE(h, QMarginsF(14.2, 16.2, 18.2, 20.2));
    h -= 2.1;
    QCOMPARE(h, m1);

    const QMarginsF doubled = m1 * 2.0;
    QCOMPARE(doubled, QMarginsF(24.2, 28.2, 32.2, 36.2));
    QCOMPARE(2.0 * m1, doubled);
    QCOMPARE(m1 * 2.0, doubled);

    a = m1;
    a *= 2.0;
    QCOMPARE(a, doubled);

    const QMarginsF halved = m1 / 2.0;
    QCOMPARE(halved, QMarginsF(6.05, 7.05, 8.05, 9.05));

    a = m1;
    a /= 2.0;
    QCOMPARE(a, halved);

    QCOMPARE(m1 + (-m1), QMarginsF());

    QMarginsF m3 = QMarginsF(10.3, 11.4, 12.5, 13.6);
    QCOMPARE(m3 + 1.1, QMarginsF(11.4, 12.5, 13.6, 14.7));
    QCOMPARE(1.1 + m3, QMarginsF(11.4, 12.5, 13.6, 14.7));
    QCOMPARE(m3 - 1.1, QMarginsF(9.2, 10.3, 11.4, 12.5));
    QCOMPARE(+m3, QMarginsF(10.3, 11.4, 12.5, 13.6));
    QCOMPARE(-m3, QMarginsF(-10.3, -11.4, -12.5, -13.6));
}

void tst_QMargins::dataStreamCheckF()
{
    //zhaoyujie TODO
//    QByteArray buffer;
//
//    // stream out
//    {
//        QMarginsF marginsOut(1.1, 2.2, 3.3, 4.4);
//        QDataStream streamOut(&buffer, QIODevice::WriteOnly);
//        streamOut << marginsOut;
//    }
//
//    // stream in & compare
//    {
//        QMarginsF marginsIn;
//        QDataStream streamIn(&buffer, QIODevice::ReadOnly);
//        streamIn >> marginsIn;
//
//        QCOMPARE(marginsIn.left(), 1.1);
//        QCOMPARE(marginsIn.top(), 2.2);
//        QCOMPARE(marginsIn.right(), 3.3);
//        QCOMPARE(marginsIn.bottom(), 4.4);
//    }
}

void tst_QMargins::debugStreamCheckF()
{
    //zhaoyujie TODO
//    QMarginsF m(10.1, 11.2, 12.3, 13.4);
//    const QString expected = "QMarginsF(10.1, 11.2, 12.3, 13.4)";
//    QString result;
//    QDebug(&result).nospace() << m;
//    QCOMPARE(result, expected);
}

void tst_QMargins::structuredBinding()
{
    {
        QMargins m(1, 2, 3, 4);
        auto [left, top, right, bottom] = m;
        QCOMPARE(left, 1);
        QCOMPARE(top, 2);
        QCOMPARE(right, 3);
        QCOMPARE(bottom, 4);
    }
    {
        QMargins m(1, 2, 3, 4);
        auto &[left, top, right, bottom] = m;
        QCOMPARE(left, 1);
        QCOMPARE(top, 2);
        QCOMPARE(right, 3);
        QCOMPARE(bottom, 4);

        left = 10;
        top = 20;
        right = 30;
        bottom = 40;
        QCOMPARE(m.left(), 10);
        QCOMPARE(m.top(), 20);
        QCOMPARE(m.right(), 30);
        QCOMPARE(m.bottom(), 40);
    }
    {
        QMarginsF m(1.0, 2.0, 3.0, 4.0);
        auto [left, top, right, bottom] = m;
        QCOMPARE(left, 1.0);
        QCOMPARE(top, 2.0);
        QCOMPARE(right, 3.0);
        QCOMPARE(bottom, 4.0);
    }
    {
        QMarginsF m(1.0, 2.0, 3.0, 4.0);
        auto &[left, top, right, bottom] = m;
        QCOMPARE(left, 1.0);
        QCOMPARE(top, 2.0);
        QCOMPARE(right, 3.0);
        QCOMPARE(bottom, 4.0);

        left = 10.0;
        top = 20.0;
        right = 30.0;
        bottom = 40.0;
        QCOMPARE(m.left(), 10.0);
        QCOMPARE(m.top(), 20.0);
        QCOMPARE(m.right(), 30.0);
        QCOMPARE(m.bottom(), 40.0);
    }
}

QTEST_APPLESS_MAIN(tst_QMargins)