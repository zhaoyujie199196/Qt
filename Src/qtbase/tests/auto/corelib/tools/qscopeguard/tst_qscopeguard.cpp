#include <QTest>
#include <QObject>
#include <QtCore/QScopeGuard>

class tst_QScopeGuard : public QObject
{
public:
    using QObject::QObject;

    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(construction)
        REGISTER_OBJECT_INVOKE_METHOD(constructionFromLvalue)
        REGISTER_OBJECT_INVOKE_METHOD(constructionFromRvalue)
        REGISTER_OBJECT_INVOKE_METHOD(leavingScope)
        REGISTER_OBJECT_INVOKE_METHOD(exceptions)
    }

    void construction();
    void constructionFromLvalue();
    void constructionFromRvalue();
    void leavingScope();
    void exceptions();
};

void func()
{
}

int intFunc()
{
    return 0;
}

int noDiscardFunc()
{
    return 0;
}

struct Callable
{
    Callable() {}
    Callable(const Callable &other)
    {
        Q_UNUSED(other);
        ++copied;
    }
    Callable(Callable &&other)
    {
        Q_UNUSED(other);
        ++moved;
    }
    void operator()() {}

    static int copied;
    static int moved;
    static void resetCounts()
    {
        copied = 0;
        moved = 0;
    }
};

int Callable::copied = 0;
int Callable::moved = 0;
static int s_globalState = 0;

void tst_QScopeGuard::construction()
{
#ifdef __cpp_deduction_guides
    QScopeGuard fromLambda([] { });
    QScopeGuard fromFunction(func);
    QScopeGuard fromFunctionPointer(&func);
    QScopeGuard fromNonVoidFunction(intFunc);
    QScopeGuard fromNoDiscardFunction(noDiscardFunc);
#ifndef __apple_build_version__
    QScopeGuard fromStdFunction{std::function<void()>(func)};
    std::function<void()> stdFunction(func);
    QScopeGuard fromNamedStdFunction(stdFunction);
#endif
#else
    QSKIP("This test requires C++17 Class Template Argument Deduction support enabled in the compiler.");
#endif
}

void tst_QScopeGuard::constructionFromLvalue()
{
#ifdef __cpp_deduction_guides
    Callable::resetCounts();
    {
        Callable callable;
        QScopeGuard guard(callable);
    }
    QCOMPARE(Callable::copied, 1);
    QCOMPARE(Callable::moved, 0);
    Callable::resetCounts();
    {
        Callable callable;
        auto guard = qScopeGuard(callable);
    }
    QCOMPARE(Callable::copied, 1);
    QCOMPARE(Callable::moved, 0);
#else
    QSKIP("This test requires C++17 Class Template Argument Deduction support enabled in the compiler.");
#endif
}

void tst_QScopeGuard::constructionFromRvalue()
{
#ifdef __cpp_deduction_guides
    Callable::resetCounts();
    {
        Callable callable;
        QScopeGuard guard(std::move(callable));
    }
    QCOMPARE(Callable::copied, 0);
    QCOMPARE(Callable::moved, 1);
    Callable::resetCounts();
    {
        Callable callable;
        auto guard = qScopeGuard(std::move(callable));
    }
    QCOMPARE(Callable::copied, 0);
    QCOMPARE(Callable::moved, 1);
#else
    QSKIP("This test requires C++17 Class Template Argument Deduction support enabled in the compiler.");
#endif
}

void tst_QScopeGuard::leavingScope()
{
    auto cleanup = qScopeGuard([] {
        s_globalState++;
        QCOMPARE(s_globalState, 3);
    });
    QCOMPARE(s_globalState, 0);

    {
        auto cleanup = qScopeGuard([] { s_globalState++; });
        QCOMPARE(s_globalState, 0);
    }

    QCOMPARE(s_globalState, 1);
    s_globalState++;
}

void tst_QScopeGuard::exceptions()
{
    s_globalState = 0;
    bool caught = false;
    QT_TRY
    {
        auto cleanup = qScopeGuard([] { s_globalState++; });
        QT_THROW(std::bad_alloc()); //if Qt compiled without exceptions this is noop
        s_globalState = 100;
    }
    QT_CATCH(...)
    {
        caught = true;
        QCOMPARE(s_globalState, 1);
    }

    QVERIFY((caught && s_globalState == 1) || (!caught && s_globalState == 101));
}

QTEST_APPLESS_MAIN(tst_QScopeGuard)