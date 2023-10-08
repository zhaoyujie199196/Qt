//
// Created by Yujie Zhao on 2023/10/8.
//
#include <QtCore/QPoint>
#include <QtTest/QTest>
#include <QObject>

class tst_QPoint : public QObject
{
public:
    using QObject::QObject;

    void registerInvokeMethods() {
        REGISTER_OBJECT_INVOKE_METHOD(isNull)
        REGISTER_OBJECT_INVOKE_METHOD(manhattanLength_data)
        REGISTER_OBJECT_INVOKE_METHOD(manhattanLength)
        REGISTER_OBJECT_INVOKE_METHOD(getSet_data)
        REGISTER_OBJECT_INVOKE_METHOD(getSet)
        REGISTER_OBJECT_INVOKE_METHOD(transposed)
        REGISTER_OBJECT_INVOKE_METHOD(rx)
        REGISTER_OBJECT_INVOKE_METHOD(ry)
        REGISTER_OBJECT_INVOKE_METHOD(operator_add_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_add)
        REGISTER_OBJECT_INVOKE_METHOD(operator_subtract_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_subtract)
        REGISTER_OBJECT_INVOKE_METHOD(operator_multiply_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_multiply)
        REGISTER_OBJECT_INVOKE_METHOD(operator_divide_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_divide)
        REGISTER_OBJECT_INVOKE_METHOD(dotProduct_data)
        REGISTER_OBJECT_INVOKE_METHOD(dotProduct)
        REGISTER_OBJECT_INVOKE_METHOD(operator_unary_plus_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_unary_plus)
        REGISTER_OBJECT_INVOKE_METHOD(operator_unary_minus_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_unary_minus)
        REGISTER_OBJECT_INVOKE_METHOD(operator_eq_data)
        REGISTER_OBJECT_INVOKE_METHOD(operator_eq)
        REGISTER_OBJECT_INVOKE_METHOD(stream_data)
        REGISTER_OBJECT_INVOKE_METHOD(stream)
        REGISTER_OBJECT_INVOKE_METHOD(structuredBinding)
    }

    void isNull();

    void manhattanLength_data();
    void manhattanLength();

    void getSet_data();
    void getSet();

    void transposed();

    void rx();
    void ry();

    void operator_add_data();
    void operator_add();

    void operator_subtract_data();
    void operator_subtract();

    void operator_multiply_data();
    void operator_multiply();

    void operator_divide_data();
    void operator_divide();

    void dotProduct_data();
    void dotProduct();

    void operator_unary_plus_data();
    void operator_unary_plus();

    void operator_unary_minus_data();
    void operator_unary_minus();

    void operator_eq_data();
    void operator_eq();

    void stream_data();
    void stream();

    void structuredBinding();
};

void tst_QPoint::isNull()
{
    QPoint point(0, 0);
    QVERIFY(point.isNull());
    ++point.rx();
    QVERIFY(!point.isNull());
    point.rx() -= 2;
    QVERIFY(!point.isNull());
}

void tst_QPoint::manhattanLength_data()
{
    QTest::addColumn<QPoint>("point");
    QTest::addColumn<int>("expected");

    QTest::newRow("(0, 0)") << QPoint(0, 0) << 0;
    QTest::newRow("(10, 0)") << QPoint(10, 0) << 10;
    QTest::newRow("(0, 10)") << QPoint(0, 10) << 10;
    QTest::newRow("(10, 20)") << QPoint(10, 20) << 30;
    QTest::newRow("(-10, -20)") << QPoint(-10, -20) << 30;
}

void tst_QPoint::manhattanLength()
{
    QFETCH(QPoint, point);
    QFETCH(int, expected);

    QCOMPARE(point.manhattanLength(), expected);
}

void tst_QPoint::getSet_data()
{
    QTest::addColumn<int>("i");

    QTest::newRow("0") << 0;
    QTest::newRow("INT_MIN") << INT_MIN;
    QTest::newRow("INT_MAX") << INT_MAX;
}

void tst_QPoint::getSet()
{
    QFETCH(int, i);

    QPoint point;
    point.setX(i);
    QCOMPARE(point.x(), i);

    point.setY(i);
    QCOMPARE(point.y(), i);
}

void tst_QPoint::transposed()
{
    QCOMPARE(QPoint(1, 2).transposed(), QPoint(2, 1));
}

void tst_QPoint::rx()
{
    const QPoint originalPoint(-1, 0);
    QPoint point(originalPoint);
    ++point.rx();
    QCOMPARE(point.x(), originalPoint.x() + 1);
}

void tst_QPoint::ry()
{
    const QPoint originalPoint(0, -1);
    QPoint point(originalPoint);
    ++point.ry();
    QCOMPARE(point.y(), originalPoint.y() + 1);
}

void tst_QPoint::operator_add_data()
{
    QTest::addColumn<QPoint>("point1");
    QTest::addColumn<QPoint>("point2");
    QTest::addColumn<QPoint>("expected");

    QTest::newRow("(0, 0) + (0, 0)") << QPoint(0, 0) << QPoint(0, 0) << QPoint(0, 0);
    QTest::newRow("(0, 9) + (1, 0)") << QPoint(0, 9) << QPoint(1, 0) << QPoint(1, 9);
    QTest::newRow("(INT_MIN, 0) + (1, 0)") << QPoint(INT_MIN, 0) << QPoint(1, 0) << QPoint(INT_MIN + 1, 0);
    QTest::newRow("(INT_MAX, 0) + (-1, 0)") << QPoint(INT_MAX, 0) << QPoint(-1, 0) << QPoint(INT_MAX - 1, 0);
}

void tst_QPoint::operator_add()
{
    QFETCH(QPoint, point1);
    QFETCH(QPoint, point2);
    QFETCH(QPoint, expected);

    QCOMPARE(point1 + point2, expected);
    point1 += point2;
    QCOMPARE(point1, expected);
}

void tst_QPoint::operator_subtract_data()
{
    QTest::addColumn<QPoint>("point1");
    QTest::addColumn<QPoint>("point2");
    QTest::addColumn<QPoint>("expected");

    QTest::newRow("(0, 0) - (0, 0)") << QPoint(0, 0) << QPoint(0, 0) << QPoint(0, 0);
    QTest::newRow("(0, 9) - (1, 0)") << QPoint(0, 9) << QPoint(1, 0) << QPoint(-1, 9);
    QTest::newRow("(INT_MAX, 0) - (1, 0)") << QPoint(INT_MAX, 0) << QPoint(1, 0) << QPoint(INT_MAX - 1, 0);
    QTest::newRow("(INT_MIN, 0) - (-1, 0)") << QPoint(INT_MIN, 0) << QPoint(-1, 0) << QPoint(INT_MIN - -1, 0);
}

void tst_QPoint::operator_subtract()
{
    QFETCH(QPoint, point1);
    QFETCH(QPoint, point2);
    QFETCH(QPoint, expected);

    QCOMPARE(point1 - point2, expected);
    point1 -= point2;
    QCOMPARE(point1, expected);
}

enum PrimitiveType {
    Int,
    Float, Double
};

Q_DECLARE_METATYPE(PrimitiveType)

void tst_QPoint::operator_multiply_data()
{
    QTest::addColumn<QPoint>("point");
    QTest::addColumn<double>("factorAsDouble");
    QTest::addColumn<PrimitiveType>("type");
    QTest::addColumn<QPoint>("expected");

    QTest::newRow("(0, 0) * 0.0") << QPoint(0, 0) << 0.0 << Double << QPoint(0, 0);
    QTest::newRow("(INT_MIN, 1) * 0.5") << QPoint(INT_MIN, 1) << 0.5 << Double << QPoint(qRound(INT_MIN * 0.5), 1);
    QTest::newRow("(INT_MAX, 2) * 0.5") << QPoint(INT_MAX, 2) << 0.5 << Double << QPoint(qRound(INT_MAX * 0.5), 1);

    QTest::newRow("(0, 0) * 0") << QPoint(0, 0) << 0.0 << Int << QPoint(0, 0);
    QTest::newRow("(INT_MIN + 1, 0) * -1") << QPoint(INT_MIN + 1, 0) << -1.0 << Int << QPoint((INT_MIN + 1) * -1, 0);
    QTest::newRow("(INT_MAX, 0) * -1") << QPoint(INT_MAX, 0) << -1.0 << Int << QPoint(INT_MAX  * -1, 0);

    QTest::newRow("(0, 0) * 0.0f") << QPoint(0, 0) << 0.0 << Float << QPoint(0, 0);
    QTest::newRow("(INT_MIN, 0) * -0.5f") << QPoint(INT_MIN, 0) << -0.5 << Float << QPoint(qRound(INT_MIN * -0.5f), 0);
}

template<typename T>
void multiplyTest(QPoint point, double factor, const QPoint &expected)
{
    T factorAsT = static_cast<T>(factor);

    QCOMPARE(point * factorAsT, expected);
    // Test with reversed argument version.
    QCOMPARE(factorAsT * point, expected);
    point *= factorAsT;
    QCOMPARE(point, expected);
}

void tst_QPoint::operator_multiply()
{
    QFETCH(QPoint, point);
    QFETCH(double, factorAsDouble);
    QFETCH(PrimitiveType, type);
    QFETCH(QPoint, expected);

    if (type == Int)
        multiplyTest<int>(point, factorAsDouble, expected);
    else if (type == Float)
        multiplyTest<float>(point, factorAsDouble, expected);
    else if (type == Double)
        multiplyTest<double>(point, factorAsDouble, expected);
}

void tst_QPoint::operator_divide_data()
{
    QTest::addColumn<QPoint>("point");
    QTest::addColumn<qreal>("divisor");
    QTest::addColumn<QPoint>("expected");

    QTest::newRow("(0, 0) / 1") << QPoint(0, 0) << qreal(1) << QPoint(0, 0);
    QTest::newRow("(0, 9) / 2") << QPoint(0, 9) << qreal(2) << QPoint(0, 5);
    QTest::newRow("(INT_MAX, 0) / 2") << QPoint(INT_MAX, 0) << qreal(2) << QPoint(qRound(INT_MAX / qreal(2)), 0);
    QTest::newRow("(INT_MIN, 0) / -1.5") << QPoint(INT_MIN, 0) << qreal(-1.5) << QPoint(qRound(INT_MIN / qreal(-1.5)), 0);
}

void tst_QPoint::operator_divide()
{
    QFETCH(QPoint, point);
    QFETCH(qreal, divisor);
    QFETCH(QPoint, expected);

    QCOMPARE(point / divisor, expected);
    point /= divisor;
    QCOMPARE(point, expected);
}


void tst_QPoint::dotProduct_data()
{
    QTest::addColumn<QPoint>("point1");
    QTest::addColumn<QPoint>("point2");
    QTest::addColumn<int>("expected");

    QTest::newRow("(0, 0) dot (0, 0)") << QPoint(0, 0) << QPoint(0, 0)<< 0;
    QTest::newRow("(10, 0) dot (0, 10)") << QPoint(10, 0) << QPoint(0, 10) << 0;
    QTest::newRow("(0, 10) dot (10, 0)") << QPoint(0, 10) << QPoint(10, 0) << 0;
    QTest::newRow("(10, 20) dot (-10, -20)") << QPoint(10, 20) << QPoint(-10, -20) << -500;
    QTest::newRow("(-10, -20) dot (10, 20)") << QPoint(-10, -20) << QPoint(10, 20) << -500;
}

void tst_QPoint::dotProduct()
{
    QFETCH(QPoint, point1);
    QFETCH(QPoint, point2);
    QFETCH(int, expected);

    QCOMPARE(QPoint::dotProduct(point1, point2), expected);
}

void tst_QPoint::operator_unary_plus_data()
{
    operator_unary_minus_data();
}

void tst_QPoint::operator_unary_plus()
{
    QFETCH(QPoint, point);
    // Should be a NOOP.
    QCOMPARE(+point, point);
}

void tst_QPoint::operator_unary_minus_data()
{
    QTest::addColumn<QPoint>("point");
    QTest::addColumn<QPoint>("expected");

    QTest::newRow("-(0, 0)") << QPoint(0, 0) << QPoint(0, 0);
    QTest::newRow("-(-1, 0)") << QPoint(-1, 0) << QPoint(1, 0);
    QTest::newRow("-(0, -1)") << QPoint(0, -1) << QPoint(0, 1);
    QTest::newRow("-(-INT_MAX, INT_MAX)") << QPoint(-INT_MAX, INT_MAX) << QPoint(INT_MAX, -INT_MAX);
}

void tst_QPoint::operator_unary_minus()
{
    QFETCH(QPoint, point);
    QFETCH(QPoint, expected);

    QCOMPARE(-point, expected);
}

void tst_QPoint::operator_eq_data()
{
    QTest::addColumn<QPoint>("point1");
    QTest::addColumn<QPoint>("point2");
    QTest::addColumn<bool>("expectEqual");

    QTest::newRow("(0, 0) == (0, 0)") << QPoint(0, 0) << QPoint(0, 0) << true;
    QTest::newRow("(-1, 0) == (-1, 0)") << QPoint(-1, 0) << QPoint(-1, 0) << true;
    QTest::newRow("(-1, 0) != (0, 0)") << QPoint(-1, 0) << QPoint(0, 0) << false;
    QTest::newRow("(-1, 0) != (0, -1)") << QPoint(-1, 0) << QPoint(0, -1) << false;
    QTest::newRow("(1, 99999) != (-1, 99999)") << QPoint(1, 99999) << QPoint(-1, 99999) << false;
    QTest::newRow("(INT_MIN, INT_MIN) == (INT_MIN, INT_MIN)") << QPoint(INT_MIN, INT_MIN) << QPoint(INT_MIN, INT_MIN) << true;
    QTest::newRow("(INT_MAX, INT_MAX) == (INT_MAX, INT_MAX)") << QPoint(INT_MAX, INT_MAX) << QPoint(INT_MAX, INT_MAX) << true;
}

void tst_QPoint::operator_eq()
{
    QFETCH(QPoint, point1);
    QFETCH(QPoint, point2);
    QFETCH(bool, expectEqual);

    bool equal = point1 == point2;
    QCOMPARE(equal, expectEqual);
    bool notEqual = point1 != point2;
    QCOMPARE(notEqual, !expectEqual);

    if (equal) {
        //zhaoyujie TODO
//        QCOMPARE(qHash(point1), qHash(point2));
    }
}

void tst_QPoint::stream_data()
{
    //zhaoyujie TODO
//    QTest::addColumn<QPoint>("point");
//
//    QTest::newRow("(0, 0)") << QPoint(0, 0);
//    QTest::newRow("(-1, 1)") << QPoint(-1, 1);
//    QTest::newRow("(1, -1)") << QPoint(1, -1);
//    QTest::newRow("(INT_MIN, INT_MAX)") << QPoint(INT_MIN, INT_MAX);
}

void tst_QPoint::stream()
{
    //zhaoyujie TODO
//    QFETCH(QPoint, point);
//
//    QBuffer tmp;
//    QVERIFY(tmp.open(QBuffer::ReadWrite));
//    QDataStream stream(&tmp);
//    // Ensure that stream returned is the same stream we inserted into.
//    QDataStream &insertionStreamRef(stream << point);
//    QVERIFY(&insertionStreamRef == &stream);
//
//    tmp.seek(0);
//    QPoint pointFromStream;
//    QDataStream &extractionStreamRef(stream >> pointFromStream);
//    QVERIFY(&extractionStreamRef == &stream);
//    QCOMPARE(pointFromStream, point);
}

void tst_QPoint::structuredBinding() {
    {
        QPoint p(1, 2);
        auto [x, y] = p;
        QCOMPARE(x, 1);
        QCOMPARE(y, 2);

        p.setX(42);
        QCOMPARE(x, 1);
        QCOMPARE(y, 2);

        p.setY(-123);
        QCOMPARE(x, 1);
        QCOMPARE(y, 2);
    }
    {
        QPoint p(1, 2);

        auto &[x, y] = p;
        QCOMPARE(x, 1);
        QCOMPARE(y, 2);

        x = 42;
        QCOMPARE(x, 42);
        QCOMPARE(p.x(), 42);
        QCOMPARE(p.rx(), 42);
        QCOMPARE(y, 2);
        QCOMPARE(p.y(), 2);
        QCOMPARE(p.ry(), 2);

        y = -123;
        QCOMPARE(x, 42);
        QCOMPARE(p.x(), 42);
        QCOMPARE(p.rx(), 42);
        QCOMPARE(y, -123);
        QCOMPARE(p.y(), -123);
        QCOMPARE(p.ry(), -123);

        p.setX(0);
        QCOMPARE(x, 0);
        QCOMPARE(p.x(), 0);
        QCOMPARE(p.rx(), 0);
        QCOMPARE(y, -123);
        QCOMPARE(p.y(), -123);
        QCOMPARE(p.ry(), -123);

        p.ry() = 10;
        QCOMPARE(x, 0);
        QCOMPARE(p.x(), 0);
        QCOMPARE(p.rx(), 0);
        QCOMPARE(y, 10);
        QCOMPARE(p.y(), 10);
        QCOMPARE(p.ry(), 10);
    }
}


QTEST_APPLESS_MAIN(tst_QPoint)