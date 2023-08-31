//
// Created by Yujie Zhao on 2023/8/9.
//

#ifndef TST_QSHAREDPOINTER_H
#define TST_QSHAREDPOINTER_H

#include <QObject>
#include <QSharedPointer>

class tst_QSharedPointer : public QObject
{
public:
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(initTestCase)
        REGISTER_OBJECT_INVOKE_METHOD(basics_data)
        REGISTER_OBJECT_INVOKE_METHOD(basics)
        REGISTER_OBJECT_INVOKE_METHOD(operators)
        REGISTER_OBJECT_INVOKE_METHOD(nullptrOps)
        REGISTER_OBJECT_INVOKE_METHOD(swap)
        REGISTER_OBJECT_INVOKE_METHOD(moveSemantics)
        REGISTER_OBJECT_INVOKE_METHOD(useOfForwardDeclared)
        REGISTER_OBJECT_INVOKE_METHOD(memoryManagement)
        REGISTER_OBJECT_INVOKE_METHOD(dropLastReferenceOfForwardDeclared)
        REGISTER_OBJECT_INVOKE_METHOD(nonVirtualDestructors)
        REGISTER_OBJECT_INVOKE_METHOD(lock)
        REGISTER_OBJECT_INVOKE_METHOD(downCast)
        REGISTER_OBJECT_INVOKE_METHOD(functionCallDownCast)
        REGISTER_OBJECT_INVOKE_METHOD(upCast)
        REGISTER_OBJECT_INVOKE_METHOD(qobjectWeakManagement)
        REGISTER_OBJECT_INVOKE_METHOD(weakQObjectFromSharedPointer)
        REGISTER_OBJECT_INVOKE_METHOD(objectCast)
        REGISTER_OBJECT_INVOKE_METHOD(objectCastStdSharedPtr)
        REGISTER_OBJECT_INVOKE_METHOD(differentPointers)
        REGISTER_OBJECT_INVOKE_METHOD(virtualBaseDifferentPointers)
        REGISTER_OBJECT_INVOKE_METHOD(dynamicCast)
        REGISTER_OBJECT_INVOKE_METHOD(dynamicCastDifferentPointers)
        REGISTER_OBJECT_INVOKE_METHOD(dynamicCastVirtualBase)
        REGISTER_OBJECT_INVOKE_METHOD(dynamicCastFailure)
        REGISTER_OBJECT_INVOKE_METHOD(dynamicCastFailureNoLeak)
        REGISTER_OBJECT_INVOKE_METHOD(constCorrectness)
        REGISTER_OBJECT_INVOKE_METHOD(customDeleter)
        REGISTER_OBJECT_INVOKE_METHOD(lambdaCustomDeleter)
        REGISTER_OBJECT_INVOKE_METHOD(customDeleterOnNullptr)
        REGISTER_OBJECT_INVOKE_METHOD(creating)
        REGISTER_OBJECT_INVOKE_METHOD(creatingCvQualified)
        REGISTER_OBJECT_INVOKE_METHOD(creatingVariadic)
        REGISTER_OBJECT_INVOKE_METHOD(creatingQObject)
        REGISTER_OBJECT_INVOKE_METHOD(mixTrackingPointerCode)
        REGISTER_OBJECT_INVOKE_METHOD(reentrancyWhileDestructing)
//        REGISTER_OBJECT_INVOKE_METHOD(map)
//        REGISTER_OBJECT_INVOKE_METHOD(hash)
//        REGISTER_OBJECT_INVOKE_METHOD(qvariantCast)
        REGISTER_OBJECT_INVOKE_METHOD(sharedFromThis)
        REGISTER_OBJECT_INVOKE_METHOD(constructorThrow)
        REGISTER_OBJECT_INVOKE_METHOD(overloads)
    }

    void initTestCase();
    void basics_data();
    void basics();
    void operators();
    void nullptrOps();
    void swap();
    void moveSemantics();
    void useOfForwardDeclared();
    void memoryManagement();
    void dropLastReferenceOfForwardDeclared();
    void nonVirtualDestructors();
    void lock();
    void downCast();
    void functionCallDownCast();
    void upCast();
    void qobjectWeakManagement();
    void weakQObjectFromSharedPointer();
    void objectCast();
    void objectCastStdSharedPtr();
    void differentPointers();
    void virtualBaseDifferentPointers();

    void dynamicCast();
    void dynamicCastDifferentPointers();
    void dynamicCastVirtualBase();
    void dynamicCastFailure();
    void dynamicCastFailureNoLeak();

    void constCorrectness();
    void customDeleter();
    void lambdaCustomDeleter();
    void customDeleterOnNullptr();

    void creating();
    void creatingCvQualified();
    void creatingVariadic();
    void creatingQObject();
    void mixTrackingPointerCode();
    void reentrancyWhileDestructing();
//    void map();
//    void hash();
//    void qvariantCast();
    void sharedFromThis();
    void constructorThrow();
    void overloads();
    void threadStressTest_data();
    void threadStressTest();
    void validConstructs();
};

extern int forwardDeclaredDestructorRunCount;
class ForwardDeclared;

#ifdef QT_NAMESPACE
namespace QT_NAMESPACE {
#endif
template <typename T> class QSharedPointer;
#ifdef QT_NAMESPACE
}
using namespace QT_NAMESPACE;
#endif

QSharedPointer<ForwardDeclared> *forwardPointer();

class Wrapper
{
public:
    QSharedPointer<int> ptr;
    Wrapper(const QSharedPointer<int> &);
    ~Wrapper();

    static Wrapper create();
};

#endif //TST_QSHAREDPOINTER_H
