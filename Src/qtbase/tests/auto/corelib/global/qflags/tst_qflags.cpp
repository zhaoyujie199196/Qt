//
// Created by Yujie Zhao on 2023/3/7.
//
#include <QTest>
#include <QObject>
#include <QtCore/QTypeInfo>
#include <QtCore/QNameSpace>

class tst_QFlags: public QObject {
public:
    using QObject::QObject;
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(testFlag)
        REGISTER_OBJECT_INVOKE_METHOD(testFlagZeroFlag)
        REGISTER_OBJECT_INVOKE_METHOD(testFlagMultiBits)
        REGISTER_OBJECT_INVOKE_METHOD(testFlags)
        REGISTER_OBJECT_INVOKE_METHOD(testAnyFlag)
        REGISTER_OBJECT_INVOKE_METHOD(constExpr)
        REGISTER_OBJECT_INVOKE_METHOD(signedness)
        REGISTER_OBJECT_INVOKE_METHOD(classEnum)
        REGISTER_OBJECT_INVOKE_METHOD(initializerLists)
        REGISTER_OBJECT_INVOKE_METHOD(testSetFlags)
        REGISTER_OBJECT_INVOKE_METHOD(adl)
    }
    void testFlag() const;
    void testFlagZeroFlag() const;
    void testFlagMultiBits() const;
    void testFlags();
    void testAnyFlag();
    void constExpr();
    void signedness();
    void classEnum();
    void initializerLists();
    void testSetFlags();
    void adl();
};

void tst_QFlags::testFlag() const
{
    Qt::MouseButtons btn = Qt::LeftButton | Qt::RightButton;
    QVERIFY(btn.testFlag(Qt::LeftButton));
    QVERIFY(!btn.testFlag(Qt::MiddleButton));

    btn = {};
    QVERIFY(!btn.testFlag(Qt::LeftButton));
}

void tst_QFlags::testFlagZeroFlag() const
{
    {
        Qt::MouseButtons btn = Qt::LeftButton | Qt::RightButton;
        QVERIFY(!btn.testFlag(Qt::NoButton));
    }
    {
        QVERIFY(Qt::MouseButtons().testFlag(Qt::NoButton));
    }
    {
        Qt::MouseButtons btn = Qt::NoButton;
        QVERIFY(btn.testFlag(Qt::NoButton));
    }
}

void tst_QFlags::testFlagMultiBits() const
{
    {
        const Qt::WindowFlags onlyWindow(Qt::Window);
        QVERIFY(!onlyWindow.testFlag(Qt::Dialog));
    }
    {
        const Qt::WindowFlags hasDialog(Qt::Dialog);
        QVERIFY(hasDialog.testFlag(Qt::Dialog));
    }
}

void tst_QFlags::testFlags()
{
    using Int = Qt::TextInteractionFlags::Int;
    constexpr Int Zero(0);

    Qt::TextInteractionFlags flags;
    QCOMPARE(flags.toInt(), Zero);
    QVERIFY(flags.testFlags(flags));
    QVERIFY(Qt::TextInteractionFlags::fromInt(Zero).testFlags(flags));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByMouse));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testFlags(Qt::TextInteractionFlags::fromInt(Zero)));
    QVERIFY(flags.testFlags(Qt::TextInteractionFlags(Qt::TextSelectableByMouse) & ~Qt::TextSelectableByMouse));

    flags = Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard;
    QVERIFY(flags.toInt() != Zero);
    QVERIFY(flags.testFlags(flags));
    QVERIFY(flags.testFlags(Qt::TextSelectableByMouse));
    QVERIFY(flags.testFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByKeyboard | Qt::TextEditable));
    QVERIFY(!flags.testFlags(Qt::TextInteractionFlags()));
    QVERIFY(!flags.testFlags(Qt::TextInteractionFlags::fromInt(Zero)));
    QVERIFY(!flags.testFlags(Qt::TextEditable));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByMouse | Qt::TextEditable));
}

void tst_QFlags::testAnyFlag()
{
    Qt::TextInteractionFlags flags;
    QVERIFY(!flags.testAnyFlags(Qt::NoTextInteraction));
    QVERIFY(!flags.testAnyFlags(Qt::TextSelectableByMouse));
    QVERIFY(!flags.testAnyFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(!flags.testAnyFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(!flags.testAnyFlag(Qt::TextEditorInteraction));
    QVERIFY(!flags.testAnyFlag(Qt::TextBrowserInteraction));

    flags = Qt::TextSelectableByMouse;
    QVERIFY(!flags.testAnyFlags(Qt::NoTextInteraction));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByMouse));
    QVERIFY(!flags.testAnyFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testAnyFlag(Qt::TextEditorInteraction));
    QVERIFY(flags.testAnyFlags(Qt::TextBrowserInteraction));

    flags = Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard;
    QVERIFY(!flags.testAnyFlags(Qt::NoTextInteraction));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByMouse));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testAnyFlag(Qt::TextEditorInteraction));
    QVERIFY(flags.testAnyFlag(Qt::TextEditorInteraction));
    QVERIFY(flags.testAnyFlag(Qt::TextBrowserInteraction));
}

template <unsigned int N, typename T>
bool verifyConstExpr(T n) { return n == N; }

constexpr Qt::MouseButtons testRelaxedConstExpr()
{
    Qt::MouseButtons value;
    value = Qt::LeftButton | Qt::RightButton;
    value |= Qt::MiddleButton;
    value &= ~Qt::LeftButton;
    value ^= Qt::RightButton;
    return value;
}

void tst_QFlags::constExpr()
{
    Qt::MouseButtons btn = Qt::LeftButton | Qt::RightButton;
    switch(btn) {
        case Qt::LeftButton:
            QVERIFY(false);
            break;
        case Qt::RightButton:
            QVERIFY(false);
            break;
        case (Qt::LeftButton | Qt::RightButton):
            QVERIFY(true);
            break;
        default:
            QVERIFY(false);
    }
    QVERIFY(verifyConstExpr<uint((Qt::LeftButton | Qt::RightButton) & Qt::LeftButton)>(Qt::LeftButton));
    QVERIFY(verifyConstExpr<uint((Qt::LeftButton | Qt::RightButton) & Qt::MiddleButton)>(0));
    QVERIFY(verifyConstExpr<uint((Qt::LeftButton | Qt::RightButton) | Qt::MiddleButton)>(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton));
    QVERIFY(verifyConstExpr<uint(~(Qt::LeftButton | Qt::RightButton))>(~(Qt::LeftButton | Qt::RightButton)));
    QVERIFY(verifyConstExpr<uint(Qt::MouseButtons(Qt::LeftButton) ^ Qt::RightButton)>(Qt::LeftButton ^ Qt::RightButton));
    QVERIFY(verifyConstExpr<uint(Qt::MouseButtons(0))>(0));
    QVERIFY(verifyConstExpr<uint(Qt::MouseButtons(Qt::RightButton) & 0xff)>(Qt::RightButton));
    QVERIFY(verifyConstExpr<uint(Qt::MouseButtons(Qt::RightButton) | 0xff)>(0xff));

    QVERIFY(!verifyConstExpr<Qt::RightButton>(~Qt::MouseButtons(Qt::LeftButton)));
    QVERIFY(verifyConstExpr<uint(testRelaxedConstExpr())>(Qt::MiddleButton));
}

void tst_QFlags::signedness()
{
    static_assert((std::is_unsigned<typename std::underlying_type<Qt::MouseButton>::type>::value ==
                   std::is_unsigned<Qt::Alignment::Int>::value));

    static_assert((std::is_signed<typename std::underlying_type<Qt::AlignmentFlag>::type>::value ==
                   std::is_signed<Qt::Alignment::Int>::value));
}

enum class MyStrictEnum {
    StrictZero,
    StrictOne,
    StrictTwo,
    StrictFour = 4
};
Q_DECLARE_FLAGS( MyStrictFlags, MyStrictEnum )
Q_DECLARE_OPERATORS_FOR_FLAGS( MyStrictFlags )

enum class MyStrictNoOpEnum {
    StrictZero,
    StrictOne,
    StrictTwo,
    StrictFour = 4
};
Q_DECLARE_FLAGS( MyStrictNoOpFlags, MyStrictNoOpEnum )

static_assert(!QTypeInfo<MyStrictFlags>::isComplex);
static_assert( QTypeInfo<MyStrictFlags>::isRelocatable);
static_assert(!QTypeInfo<MyStrictFlags>::isPointer);

void tst_QFlags::classEnum()
{
    MyStrictEnum e1 = MyStrictEnum::StrictOne;
    MyStrictEnum e2 = MyStrictEnum::StrictTwo;

    MyStrictFlags f1(MyStrictEnum::StrictOne);
    QCOMPARE(f1, 1);

    MyStrictFlags f2(e2);
    QCOMPARE(f2, 2);

    MyStrictFlags f0;
    QCOMPARE(f0, 0);

    MyStrictFlags f3(e2 | e1);
    QCOMPARE(f3, 3);

    QVERIFY(f3.testFlag(MyStrictEnum::StrictOne));
    QVERIFY(!f1.testFlag(MyStrictEnum::StrictTwo));

    QVERIFY(!f0);

    QCOMPARE(f3 & int(1), 1);
    QCOMPARE(f3 & uint(1), 1);
    QCOMPARE(f3 & MyStrictEnum::StrictOne, 1);

    MyStrictFlags aux;
    aux = f3;
    aux &= int(1);
    QCOMPARE(aux, 1);

    aux = f3;
    aux &= uint(1);
    QCOMPARE(aux, 1);

    aux = f3;
    aux &= MyStrictEnum::StrictOne;
    QCOMPARE(aux, 1);

    aux = f3;
    aux &= f1;
    QCOMPARE(aux, 1);

    aux = f3 ^ f3;
    QCOMPARE(aux, 0);

    aux = f3 ^ f1;
    QCOMPARE(aux, 2);

    aux = f3 ^ f0;
    QCOMPARE(aux, 3);

    aux = f3 ^ MyStrictEnum::StrictOne;
    QCOMPARE(aux, 2);

    aux = f3 ^ MyStrictEnum::StrictZero;
    QCOMPARE(aux, 3);

    aux = f3;
    aux ^= f3;
    QCOMPARE(aux, 0);

    aux = f3;
    aux ^= f1;
    QCOMPARE(aux, 2);

    aux = f3;
    aux ^= f0;
    QCOMPARE(aux, 3);

    aux = f3;
    aux ^= MyStrictEnum::StrictOne;
    QCOMPARE(aux, 2);

    aux = f3;
    aux ^= MyStrictEnum::StrictZero;
    QCOMPARE(aux, 3);

    aux = f1 | f2;
    QCOMPARE(aux, 3);

    aux = MyStrictEnum::StrictOne | MyStrictEnum::StrictTwo;
    QCOMPARE(aux, 3);

    aux = f1;
    aux |= f2;
    QCOMPARE(aux, 3);

    aux = MyStrictEnum::StrictOne;
    aux |= MyStrictEnum::StrictTwo;
    QCOMPARE(aux, 3);

    aux = ~f1;
    QCOMPARE(aux, -2);
}

void tst_QFlags::initializerLists()
{
    Qt::MouseButtons bts = { Qt::LeftButton, Qt::RightButton };
    QVERIFY(bts.testFlag(Qt::LeftButton));
    QVERIFY(bts.testFlag(Qt::RightButton));
    QVERIFY(!bts.testFlag(Qt::MiddleButton));

    MyStrictNoOpFlags flags = {MyStrictNoOpEnum::StrictOne, MyStrictNoOpEnum::StrictFour};
    QVERIFY(flags.testFlag(MyStrictNoOpEnum::StrictOne));
    QVERIFY(flags.testFlag(MyStrictNoOpEnum::StrictFour));
    QVERIFY(!flags.testFlag(MyStrictNoOpEnum::StrictTwo));
}

void tst_QFlags::testSetFlags()
{
    Qt::MouseButtons btn = Qt::NoButton;

    btn.setFlag(Qt::LeftButton);
    QVERIFY(btn.testFlag(Qt::LeftButton));
    QVERIFY(!btn.testFlag(Qt::MiddleButton));

    btn.setFlag(Qt::LeftButton, false);
    QVERIFY(!btn.testFlag(Qt::LeftButton));
    QVERIFY(!btn.testFlag(Qt::MiddleButton));

    MyStrictFlags flags;
    flags.setFlag(MyStrictEnum::StrictOne);
    flags.setFlag(MyStrictEnum::StrictTwo, true);
    QVERIFY(flags.testFlag(MyStrictEnum::StrictOne));
    QVERIFY(flags.testFlag(MyStrictEnum::StrictTwo));
    QVERIFY(!flags.testFlag(MyStrictEnum::StrictFour));

    flags.setFlag(MyStrictEnum::StrictTwo, false);
    QVERIFY(flags.testFlag(MyStrictEnum::StrictOne));
    QVERIFY(!flags.testFlag(MyStrictEnum::StrictTwo));
    QVERIFY(!flags.testFlag(MyStrictEnum::StrictFour));
}

namespace SomeNS {
    enum Foo {
        Foo_A = 1 << 0,
        Foo_B = 1 << 1,
        Foo_C = 1 << 2
    };
    Q_DECLARE_FLAGS(Foos, Foo)
    Q_DECLARE_OPERATORS_FOR_FLAGS(Foos)

    Qt::Alignment alignment()
    {
        return Qt::AlignLeft | Qt::AlignTop;
    }
}

void tst_QFlags::adl()
{
    SomeNS::Foos f1 = SomeNS::Foo_B | SomeNS::Foo_C;
    QVERIFY(f1 & SomeNS::Foo_B);
    QVERIFY(!(f1 & SomeNS::Foo_A));
    QCOMPARE(SomeNS::alignment(), Qt::AlignLeft | Qt::AlignTop);
}

enum MyEnum {
    Zero,
    One,
    Two,
    Four = 4
};

Q_DECLARE_FLAGS(MyFlags, MyEnum)
Q_DECLARE_OPERATORS_FOR_FLAGS(MyFlags)

static_assert(!QTypeInfo<MyFlags>::isComplex);
static_assert(QTypeInfo<MyFlags>::isRelocatable);
static_assert(!QTypeInfo<MyFlags>::isPointer);

QTEST_APPLESS_MAIN(tst_QFlags)