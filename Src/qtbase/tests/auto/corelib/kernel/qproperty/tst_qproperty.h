//
// Created by Yujie Zhao on 2023/12/20.
//

#ifndef TST_QPROPERTY_H
#define TST_QPROPERTY_H

#include <QObject>
#include <QProperty>
#include <private/qproperty_p.h>

class tst_qproperty : public QObject
{
public:
    void registerInvokeMethods() {
        REGISTER_OBJECT_INVOKE_METHOD(functorBinding)
        REGISTER_OBJECT_INVOKE_METHOD(basicDependencies)
        REGISTER_OBJECT_INVOKE_METHOD(multipleDependencies)
        REGISTER_OBJECT_INVOKE_METHOD(bindingWithDeletedDependency)
        REGISTER_OBJECT_INVOKE_METHOD(dependencyChangeDuringDestruction)
        REGISTER_OBJECT_INVOKE_METHOD(recursiveDependency)
        REGISTER_OBJECT_INVOKE_METHOD(bindingAfterUse)
        REGISTER_OBJECT_INVOKE_METHOD(bindingFunctionDtorCalled)
        REGISTER_OBJECT_INVOKE_METHOD(switchBinding)
        REGISTER_OBJECT_INVOKE_METHOD(avoidDependencyAllocationAfterFirstEval)
        REGISTER_OBJECT_INVOKE_METHOD(boolProperty)
        REGISTER_OBJECT_INVOKE_METHOD(takeBinding)
        REGISTER_OBJECT_INVOKE_METHOD(stickyBinding)
        REGISTER_OBJECT_INVOKE_METHOD(replaceBinding)
        REGISTER_OBJECT_INVOKE_METHOD(changeHandler)
        REGISTER_OBJECT_INVOKE_METHOD(propertyChangeHandlerApi)
        REGISTER_OBJECT_INVOKE_METHOD(subscribe)
        REGISTER_OBJECT_INVOKE_METHOD(changeHandlerThroughBindings)
        REGISTER_OBJECT_INVOKE_METHOD(dontTriggerDependenciesIfUnchangedValue)
        REGISTER_OBJECT_INVOKE_METHOD(bindingSourceLocation)
        REGISTER_OBJECT_INVOKE_METHOD(bindingError)
        REGISTER_OBJECT_INVOKE_METHOD(bindingLoop)
        REGISTER_OBJECT_INVOKE_METHOD(realloc)
        REGISTER_OBJECT_INVOKE_METHOD(changePropertyFromWithinChangeHandler)
        REGISTER_OBJECT_INVOKE_METHOD(changePropertyFromWithinChangeHandlerThroughDependency)
        REGISTER_OBJECT_INVOKE_METHOD(settingPropertyValueDoesRemoveBinding)
        REGISTER_OBJECT_INVOKE_METHOD(genericPropertyBinding)
        REGISTER_OBJECT_INVOKE_METHOD(genericPropertyBindingBool)
        REGISTER_OBJECT_INVOKE_METHOD(setBindingFunctor)
        REGISTER_OBJECT_INVOKE_METHOD(multipleObservers)
        REGISTER_OBJECT_INVOKE_METHOD(arrowAndStarOperator)
        REGISTER_OBJECT_INVOKE_METHOD(notifiedProperty)
        REGISTER_OBJECT_INVOKE_METHOD(typeNoOperatorEqual)
        REGISTER_OBJECT_INVOKE_METHOD(bindingValueReplacement)
        REGISTER_OBJECT_INVOKE_METHOD(quntypedBindableApi)
        REGISTER_OBJECT_INVOKE_METHOD(readonlyConstQBindable)
        REGISTER_OBJECT_INVOKE_METHOD(qobjectBindableManualNotify)
        REGISTER_OBJECT_INVOKE_METHOD(qobjectBindableSignalTakingNewValue)
        REGISTER_OBJECT_INVOKE_METHOD(testNewStuff)
        REGISTER_OBJECT_INVOKE_METHOD(qobjectObservers)
        REGISTER_OBJECT_INVOKE_METHOD(compatBindings)
        REGISTER_OBJECT_INVOKE_METHOD(metaProperty)
        REGISTER_OBJECT_INVOKE_METHOD(modifyObserverListWhileIterating)
        REGISTER_OBJECT_INVOKE_METHOD(compatPropertyNoDobuleNotification)
        REGISTER_OBJECT_INVOKE_METHOD(compatPropertySignals)
        REGISTER_OBJECT_INVOKE_METHOD(noFakeDependencies)
        REGISTER_OBJECT_INVOKE_METHOD(bindablePropertyWithInitialization)
        REGISTER_OBJECT_INVOKE_METHOD(noDoubleNotification)
        REGISTER_OBJECT_INVOKE_METHOD(groupedNotifications)
        REGISTER_OBJECT_INVOKE_METHOD(groupedNotificationConsistency)
        REGISTER_OBJECT_INVOKE_METHOD(uninstalledBindingDoesNotEvaluate)
        REGISTER_OBJECT_INVOKE_METHOD(notify)
        REGISTER_OBJECT_INVOKE_METHOD(bindableInterfaceOfCompatPropertyUsesSetter)
        REGISTER_OBJECT_INVOKE_METHOD(selfBindingShouldNotCrash)
        REGISTER_OBJECT_INVOKE_METHOD(qpropertyAlias)
    }

    void functorBinding();
    void basicDependencies();
    void multipleDependencies();
    void bindingWithDeletedDependency();
    void dependencyChangeDuringDestruction();
    void recursiveDependency();
    void bindingAfterUse();
    void bindingFunctionDtorCalled();
    void switchBinding();
    void avoidDependencyAllocationAfterFirstEval();
    void boolProperty();
    void takeBinding();
    void stickyBinding();
    void replaceBinding();
    void changeHandler();
    void propertyChangeHandlerApi();
    void subscribe();
    void changeHandlerThroughBindings();
    void dontTriggerDependenciesIfUnchangedValue();
    void bindingSourceLocation();
    void bindingError();
    void bindingLoop();
    void realloc();
    void changePropertyFromWithinChangeHandler();
    void changePropertyFromWithinChangeHandlerThroughDependency();
    void settingPropertyValueDoesRemoveBinding();
    void genericPropertyBinding();
    void genericPropertyBindingBool();
    void setBindingFunctor();
    void multipleObservers();
    void arrowAndStarOperator();
    void notifiedProperty();
    void typeNoOperatorEqual();
    void bindingValueReplacement();
    void quntypedBindableApi();
    void readonlyConstQBindable();
    void qobjectBindableManualNotify();
    void qobjectBindableSignalTakingNewValue();
    void testNewStuff();
    void qobjectObservers();
    void compatBindings();
    void metaProperty();

    void modifyObserverListWhileIterating();
    void compatPropertyNoDobuleNotification();
    void compatPropertySignals();

    void noFakeDependencies();

    void bindablePropertyWithInitialization();
    void noDoubleNotification();
    void groupedNotifications();
    void groupedNotificationConsistency();
    void uninstalledBindingDoesNotEvaluate();

    void notify();

    void bindableInterfaceOfCompatPropertyUsesSetter();

    void selfBindingShouldNotCrash();

    void qpropertyAlias();
};

class ChangeDuringDtorTester : public QObject
{
Q_OBJECT
    Q_PROPERTY(int prop READ prop WRITE setProp BINDABLE bindableProp)

public:
    void setProp(int i) { m_prop = i;}
    int prop() const { return m_prop; }
    QBindable<int> bindableProp() { return &m_prop; }
private:
    Q_OBJECT_COMPAT_PROPERTY(ChangeDuringDtorTester, int, m_prop, &ChangeDuringDtorTester::setProp)
};

class BindingLoopTester : public QObject
{
Q_OBJECT
    Q_PROPERTY(int eagerProp READ eagerProp WRITE setEagerProp BINDABLE bindableEagerProp)
    Q_PROPERTY(int eagerProp2 READ eagerProp2 WRITE setEagerProp2 BINDABLE bindableEagerProp2)
public:
    BindingLoopTester(QProperty<int> *i, QObject *parent = nullptr)
        : QObject(parent) {
        eagerData.setBinding(Qt::makePropertyBinding([&](){
            return eagerData2.value() + i->value();
        }));
        eagerData2.setBinding(Qt::makePropertyBinding([&](){
            return eagerData.value() + 1;
        }));
        i->setValue(42);
    }
    BindingLoopTester() {}

    int eagerProp() {
        return eagerData.value();
    }
    void setEagerProp(int i) {
        eagerData.setValue(i); eagerData.notify();
    }
    QBindable<int> bindableEagerProp() {
        return QBindable<int>(&eagerData);
    }

    int eagerProp2() {
        return eagerData2.value();
    }
    void setEagerProp2(int i) {
        eagerData2.setValue(i); eagerData2.notify();
    }
    QBindable<int> bindableEagerProp2() {
        return QBindable<int>(&eagerData2);
    }

    Q_OBJECT_COMPAT_PROPERTY(BindingLoopTester, int, eagerData, &BindingLoopTester::setEagerProp)
    Q_OBJECT_COMPAT_PROPERTY(BindingLoopTester, int, eagerData2, &BindingLoopTester::setEagerProp2)
};

class ReallocTester : public QObject
{
    /*
     * This class and the realloc test rely on the fact that the internal property hashmap has an
     * initial capacity of 8 and a load factor of 0.5. Thus, it is necessary to cause actions which
     * allocate 5 different QPropertyBindingData
     * */
Q_OBJECT
    Q_PROPERTY(int prop1 READ prop1 WRITE setProp1 BINDABLE bindableProp1)
    Q_PROPERTY(int prop2 READ prop2 WRITE setProp2 BINDABLE bindableProp2)
    Q_PROPERTY(int prop3 READ prop3 WRITE setProp3 BINDABLE bindableProp3)
    Q_PROPERTY(int prop4 READ prop4 WRITE setProp4 BINDABLE bindableProp4)
    Q_PROPERTY(int prop5 READ prop5 WRITE setProp5 BINDABLE bindableProp5)
public:
    ReallocTester(QObject *parent = nullptr) : QObject(parent) {}


#define GEN(N) \
    int prop##N() {return propData##N.value();} \
    void setProp##N(int i) { if (i == propData##N) return; propData##N.setValue(i); propData##N.notify(); } \
    QBindable<int> bindableProp##N() {return QBindable<int>(&propData##N);} \
    Q_OBJECT_COMPAT_PROPERTY(ReallocTester, int, propData##N, &ReallocTester::setProp##N)
    GEN(1)
    GEN(2)
    GEN(3)
    GEN(4)
    GEN(5)
#undef GEN
};

struct ClassWithNotifiedProperty : public QObject
{
    QList<int> recordedValues;

    void callback() {
        recordedValues << property.value();
    }
    int getProp() { return 0; }

    Q_OBJECT_BINDABLE_PROPERTY(ClassWithNotifiedProperty, int, property, &ClassWithNotifiedProperty::callback);
};

class MyQObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int foo READ foo WRITE setFoo BINDABLE bindableFoo NOTIFY fooChanged)
    Q_PROPERTY(int bar READ bar WRITE setBar BINDABLE bindableBar NOTIFY barChanged)
    Q_PROPERTY(int read READ read)
    Q_PROPERTY(int computed READ computed STORED false)
    Q_PROPERTY(int compat READ compat WRITE setCompat NOTIFY compatChanged)

signals:
    void fooChanged(int newFoo);
    void barChanged();
    void compatChanged();

public slots:
    void fooHasChanged() { fooChangedCount++; }
    void barHasChanged() { barChangedCount++; }
    void compatHasChanged() { compatChangedCount++; }

public:
    int foo() const { return fooData.value(); }
    void setFoo(int i) { fooData.setValue(i); }
    int bar() const { return barData.value(); }
    void setBar(int i) { barData.setValue(i); }
    int read() const { return readData.value(); }
    int computed() const { return readData.value(); }
    int compat() const { return compatData; }
    void setCompat(int i)
    {
        if (compatData == i)
            return;
        // implement some side effect and clamping
        ++setCompatCalled;
        if (i < 0)
            i = 0;
        compatData.setValue(i);
        compatData.notify();
        emit compatChanged();
    }

    QBindable<int> bindableFoo() { return QBindable<int>(&fooData); }
    const QBindable<int> bindableFoo() const { return QBindable<int>(&fooData); }
    QBindable<int> bindableBar() { return QBindable<int>(&barData); }
    QBindable<int> bindableRead() { return QBindable<int>(&readData); }
    QBindable<int> bindableComputed() { return QBindable<int>(&computedData); }
    QBindable<int> bindableCompat() { return QBindable<int>(&compatData); }

public:
    int fooChangedCount = 0;
    int barChangedCount = 0;
    int compatChangedCount = 0;
    int setCompatCalled = 0;

    Q_OBJECT_BINDABLE_PROPERTY(MyQObject, int, fooData, &MyQObject::fooChanged);
    Q_OBJECT_BINDABLE_PROPERTY(MyQObject, int, barData, &MyQObject::barChanged);
    Q_OBJECT_BINDABLE_PROPERTY(MyQObject, int, readData);
    Q_OBJECT_COMPUTED_PROPERTY(MyQObject, int, computedData, &MyQObject::computed);
    Q_OBJECT_COMPAT_PROPERTY(MyQObject, int, compatData, &MyQObject::setCompat)
};

struct ReallocObject : QObject {
    ReallocObject()
    { v.setBinding([this] { return x.value() + y.value() + z.value(); }); }
    Q_OBJECT_BINDABLE_PROPERTY(ReallocObject, int, v)
    Q_OBJECT_BINDABLE_PROPERTY(ReallocObject, int, x)
    Q_OBJECT_BINDABLE_PROPERTY(ReallocObject, int, y)
    Q_OBJECT_BINDABLE_PROPERTY(ReallocObject, int, z)
};

class FakeDependencyCreator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int prop1 READ prop1 WRITE setProp1 NOTIFY prop1Changed BINDABLE bindableProp1)
    Q_PROPERTY(int prop2 READ prop2 WRITE setProp2 NOTIFY prop2Changed BINDABLE bindableProp2)
    Q_PROPERTY(int prop3 READ prop3 WRITE setProp3 NOTIFY prop3Changed BINDABLE bindableProp3)

signals:
    void prop1Changed();
    void prop2Changed();
    void prop3Changed();

public:
    void setProp1(int val) { prop1Data.setValue(val); prop1Data.notify();}
    void setProp2(int val) { prop2Data.setValue(val); prop2Data.notify();}
    void setProp3(int val) { prop3Data.setValue(val); prop3Data.notify();}

    int prop1() { return prop1Data; }
    int prop2() { return prop2Data; }
    int prop3() { return prop3Data; }

    QBindable<int> bindableProp1() { return QBindable<int>(&prop1Data); }
    QBindable<int> bindableProp2() { return QBindable<int>(&prop2Data); }
    QBindable<int> bindableProp3() { return QBindable<int>(&prop3Data); }

private:
    Q_OBJECT_COMPAT_PROPERTY(FakeDependencyCreator, int, prop1Data, &FakeDependencyCreator::setProp1, &FakeDependencyCreator::prop1Changed);
    Q_OBJECT_COMPAT_PROPERTY(FakeDependencyCreator, int, prop2Data, &FakeDependencyCreator::setProp2, &FakeDependencyCreator::prop2Changed);
    Q_OBJECT_COMPAT_PROPERTY(FakeDependencyCreator, int, prop3Data, &FakeDependencyCreator::setProp3, &FakeDependencyCreator::prop3Changed);
};

class PropertyAdaptorTester : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int foo READ foo WRITE setFoo NOTIFY fooChanged)
    Q_PROPERTY(int foo1 READ foo WRITE setFoo)

signals:
    void dummySignal1();
    void dummySignal2();
    void dummySignal3();
    void dummySignal4();
    void dummySignal5();
    void dummySignal6();
    void dummySignal7();
    void dummySignal8();
    void fooChanged(int newFoo);

public slots:
    void fooHasChanged() { fooChangedCount++; }

public:
    int foo() const { return fooData; }
    void setFoo(int i)
    {
        if (i != fooData) {
            fooData = i;
            fooChanged(fooData);
        }
    }

public:
    int fooData = 0;
    int fooChangedCount = 0;
};

//struct ThreadSafetyTester : public QObject
//{
//    Q_OBJECT
//public:
//    ThreadSafetyTester(QObject *parent = nullptr) : QObject(parent) {}
//
//    Q_INVOKABLE bool hasCorrectStatus() const
//    {
//        return qGetBindingStorage(this)->status({}) == QtPrivate::getBindingStatus({});
//    }
//
//    Q_INVOKABLE bool bindingTest()
//    {
//        QProperty<QString> name(u"inThread"_s);
//        bindableObjectName().setBinding([&]() -> QString { return name; });
//        name = u"inThreadChanged"_s;
//        const bool nameChangedCorrectly = objectName() == name;
//        bindableObjectName().takeBinding();
//        return nameChangedCorrectly;
//    }
//};

struct CustomType
{
    CustomType() = default;
    CustomType(int val) : value(val) { }
    CustomType(int val, int otherVal) : value(val), anotherValue(otherVal) { }
    CustomType(const CustomType &) = default;
    CustomType(CustomType &&) = default;
    ~CustomType() = default;
    CustomType &operator=(const CustomType &) = default;
    CustomType &operator=(CustomType &&) = default;
    bool operator==(const CustomType &other) const
    {
        return (value == other.value) && (anotherValue == other.anotherValue);
    }

    int value = 0;
    int anotherValue = 0;
};

class PropertyWithInitializationTester : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int prop1 READ prop1 WRITE setProp1 NOTIFY prop1Changed BINDABLE bindableProp1)
    Q_PROPERTY(CustomType prop2 READ prop2 WRITE setProp2 BINDABLE bindableProp2)
    Q_PROPERTY(CustomType prop3 READ prop3 WRITE setProp3 BINDABLE bindableProp3)
signals:
    void prop1Changed();

public:
    PropertyWithInitializationTester(QObject *parent = nullptr) : QObject(parent) { }

    int prop1() { return prop1Data.value(); }
    void setProp1(int i) { prop1Data = i; }
    QBindable<int> bindableProp1() { return QBindable<int>(&prop1Data); }

    CustomType prop2() { return prop2Data.value(); }
    void setProp2(CustomType val) { prop2Data = val; }
    QBindable<CustomType> bindableProp2() { return QBindable<CustomType>(&prop2Data); }

    CustomType prop3() { return prop3Data.value(); }
    void setProp3(CustomType val) { prop3Data.setValue(val); }
    QBindable<CustomType> bindableProp3() { return QBindable<CustomType>(&prop3Data); }

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(PropertyWithInitializationTester, int, prop1Data, 5,
    &PropertyWithInitializationTester::prop1Changed)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(PropertyWithInitializationTester, CustomType, prop2Data,
            CustomType(5))
    Q_OBJECT_COMPAT_PROPERTY_WITH_ARGS(PropertyWithInitializationTester, CustomType, prop3Data,
                                       &PropertyWithInitializationTester::setProp3,
                                       CustomType(10, 20))
};


class CompatPropertyTester : public QObject
{
Q_OBJECT
    Q_PROPERTY(int prop1 READ prop1 WRITE setProp1 BINDABLE bindableProp1)
    Q_PROPERTY(int prop2 READ prop2 WRITE setProp2 NOTIFY prop2Changed BINDABLE bindableProp2)
    Q_PROPERTY(int prop3 READ prop3 WRITE setProp3 NOTIFY prop3Changed BINDABLE bindableProp3)
public:
    CompatPropertyTester(QObject *parent = nullptr) : QObject(parent) { }

    int prop1() {return prop1Data.value();}
    void setProp1(int i) { if (i == prop1Data) return; prop1Data.setValue(i); prop1Data.notify(); }
    QBindable<int> bindableProp1() {return QBindable<int>(&prop1Data);}

    int prop2() { return prop2Data.value(); }
    void setProp2(int i)
    {
        if (i == prop2Data)
            return;
        prop2Data.setValue(i);
        prop2Data.notify();
    }
    QBindable<int> bindableProp2() { return QBindable<int>(&prop2Data); }

    int prop3() { return prop3Data.value(); }
    void setProp3(int i)
    {
        if (i == prop3Data)
            return;
        prop3Data.setValue(i);
        prop3Data.notify();
    }
    QBindable<int> bindableProp3() { return QBindable<int>(&prop3Data); }

signals:
    void prop2Changed(int value);
    void prop3Changed();

private:
    Q_OBJECT_COMPAT_PROPERTY(CompatPropertyTester, int, prop1Data, &CompatPropertyTester::setProp1)
    Q_OBJECT_COMPAT_PROPERTY(CompatPropertyTester, int, prop2Data, &CompatPropertyTester::setProp2,
                             &CompatPropertyTester::prop2Changed)
    Q_OBJECT_COMPAT_PROPERTY_WITH_ARGS(CompatPropertyTester, int, prop3Data,
                                       &CompatPropertyTester::setProp3,
                                       &CompatPropertyTester::prop3Changed, 1)
};



#endif //TST_QPROPERTY_H
