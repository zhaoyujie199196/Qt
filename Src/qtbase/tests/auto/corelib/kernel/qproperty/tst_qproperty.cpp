//
// Created by Yujie Zhao on 2023/11/27.
//
#include <QObject>
#include <QTest>
#include <QList>
#include <QProperty>
#include <QtCore/private/qproperty_p.h>
#include "tst_qproperty.h"

/*
 * QPropertyBindingPrivate内存什么时候释放？
 * 计算observer过程中修改了observer的list如何处理
 * observer的tag为什么要放在next中
 * */

struct DtorCounter
{
    static inline int counter = 0;
    bool shouldIncrement = false;
    ~DtorCounter() {
        if (shouldIncrement) {
            ++counter;
        }
    }
};


void tst_qproperty::functorBinding() {
    QProperty<int> property([]{
        return 42;
    });
    QCOMPARE(property.value(), int(42));

    property.setBinding([](){
        return 100;
    });
    QCOMPARE(property.value(), int(100));

    property.setBinding([](){
        return 50;
    });
    QCOMPARE(property.value(), int(50));
}

void tst_qproperty::basicDependencies() {
    QProperty<int> right(100);
    QProperty<int> left(Qt::makePropertyBinding(right));
    QCOMPARE(left.value(), int(100));
    right = 42;
    QCOMPARE(left.value(), int(42));
}

void tst_qproperty::multipleDependencies() {
    QProperty<int> firstDependency(1);
    QProperty<int> secondDependency(2);

    QProperty<int> sum;
    sum.setBinding([&]() {
        return firstDependency + secondDependency;
    });

    //first和second都只有sum一个监听者
    QCOMPARE(QPropertyBindingDataPointer::get(firstDependency).observerCount(), 1);
    QCOMPARE(QPropertyBindingDataPointer::get(secondDependency).observerCount(), 1);

    QCOMPARE(sum.value(), int(3));
    QCOMPARE(QPropertyBindingDataPointer::get(firstDependency).observerCount(), 1);
    QCOMPARE(QPropertyBindingDataPointer::get(secondDependency).observerCount(), 1);

    firstDependency = 10;

    QCOMPARE(sum.value(), int(12));
    QCOMPARE(QPropertyBindingDataPointer::get(firstDependency).observerCount(), 1);
    QCOMPARE(QPropertyBindingDataPointer::get(secondDependency).observerCount(), 1);

    secondDependency = 20;

    QCOMPARE(sum.value(), int(30));
    QCOMPARE(QPropertyBindingDataPointer::get(firstDependency).observerCount(), 1);
    QCOMPARE(QPropertyBindingDataPointer::get(secondDependency).observerCount(), 1);

    firstDependency = 1;
    secondDependency = 1;
    QCOMPARE(sum.value(), int(2));
    QCOMPARE(QPropertyBindingDataPointer::get(firstDependency).observerCount(), 1);
    QCOMPARE(QPropertyBindingDataPointer::get(secondDependency).observerCount(), 1);
}

void tst_qproperty::bindingWithDeletedDependency()
{
    QScopedPointer<QProperty<int>> dynamicProperty(new QProperty<int>(100));
    QProperty<int> staticProperty(1000);
    QProperty<bool> bindingReturnsDynamicProperty(false);

    QProperty<int> propertySelector([&]() {
        if (bindingReturnsDynamicProperty && !dynamicProperty.isNull())
            return dynamicProperty->value();
        else
            return staticProperty.value();
    });

    QCOMPARE(propertySelector.value(), staticProperty.value());
    bindingReturnsDynamicProperty = true;
    QCOMPARE(propertySelector.value(), dynamicProperty->value());
    //dynamicProperty销毁了，不会触发propertySelector重新计算
    dynamicProperty.reset();
    QCOMPARE(propertySelector.value(), 100);
    bindingReturnsDynamicProperty = false;
    QCOMPARE(propertySelector.value(), staticProperty.value());
}

//class ChangeDuringDtorTester : public QObject
//{
//    Q_OBJECT
//    Q_PROPERTY(int prop READ prop WRITE setProp BINDABLE bindableProp)
//
//public:
//    void setProp(int i) { m_prop = i;}
//    int prop() const { return m_prop; }
//    QBindable<int> bindableProp() { return &m_prop; }
//private:
//    Q_OBJECT_COMPAT_PROPERTY(ChangeDuringDtorTester, int, m_prop, &ChangeDuringDtorTester::setProp)
//};

void tst_qproperty::dependencyChangeDuringDestruction()
{
    //TODO Q_OBJECT_COMPAT_PROPERTY 写完之后看看这个是什么意思
}

void tst_qproperty::recursiveDependency()
{
    QProperty<int> first(1);
    QProperty<int> second;
    second.setBinding([&]() {
        return first.value();
    });
    QProperty<int> third;
    third.setBinding([&]() {
        return second.value();
    });
    QCOMPARE(third.value(), int(1));
    //first变化引起second变化，second变化引起third变化
    first = 2;
    QCOMPARE(third.value(), int(2));
}

void tst_qproperty::bindingAfterUse()
{
    QProperty<int> propWithBindingLater(1);

    QProperty<int> propThatUsesFirstProp;
    propThatUsesFirstProp.setBinding(Qt::makePropertyBinding(propWithBindingLater));

    QCOMPARE(propThatUsesFirstProp.value(), int(1));
    QCOMPARE(QPropertyBindingDataPointer::get(propWithBindingLater).observerCount(), 1);

    QProperty<int> injectedValue(42);
    propWithBindingLater.setBinding(Qt::makePropertyBinding(injectedValue));

    QCOMPARE(propThatUsesFirstProp.value(), int(42));
    QCOMPARE(QPropertyBindingDataPointer::get(propWithBindingLater).observerCount(), 1);
}

void tst_qproperty::bindingFunctionDtorCalled()
{
    DtorCounter dc;
    {
        //出了作用域，应该调用dc的析构函数
        QProperty<int> prop;
        prop.setBinding([dc]() mutable {
            dc.shouldIncrement = true;
            return 42;
        });
        QCOMPARE(prop.value(), 42);
    }
    QCOMPARE(DtorCounter::counter, 1);
}

void tst_qproperty::switchBinding()
{
    QProperty<int> first(1);
    QProperty<int> propWithChangingBinding;
    propWithChangingBinding.setBinding(Qt::makePropertyBinding(first));
    QCOMPARE(propWithChangingBinding.value(), 1);

    QProperty<int> output;
    output.setBinding(Qt::makePropertyBinding(propWithChangingBinding));
    QCOMPARE(output.value(), 1);

    QProperty<int> second(2);
    propWithChangingBinding.setBinding(Qt::makePropertyBinding(second));
    QCOMPARE(output.value(), 2);
}

void tst_qproperty::avoidDependencyAllocationAfterFirstEval()
{
    QProperty<int> firstDependency(1);
    QProperty<int> secondDependency(10);

    QProperty<int> propWithBinding([&]() {
        return firstDependency + secondDependency;
    });

    QCOMPARE(propWithBinding.value(), int(11));

    QVERIFY(QPropertyBindingDataPointer::get(propWithBinding).binding());
    QCOMPARE(QPropertyBindingDataPointer::get(propWithBinding).binding()->dependencyObserverCount, 2u);

    firstDependency = 100;
    QCOMPARE(propWithBinding.value(), int(110));
    QCOMPARE(QPropertyBindingDataPointer::get(propWithBinding).binding()->dependencyObserverCount, 2u);
}

void tst_qproperty::boolProperty()
{
    QProperty<bool> first(true);
    QProperty<bool> second(false);
    QProperty<bool> all([&]() {
        return first && second;
    });
    QCOMPARE(all.value(), false);
    second = true;
    QCOMPARE(all.value(), true);
}

void tst_qproperty::takeBinding()
{
    QPropertyBinding<int> existingBinding;
    QVERIFY(existingBinding.isNull());

    QProperty<int> first(100);
    QProperty<int> second(Qt::makePropertyBinding(first));

    QCOMPARE(second.value(), int(100));

    existingBinding = second.takeBinding();
    QVERIFY(!existingBinding.isNull());

    first = 10;
    QCOMPARE(second.value(), int(100));

    second = 25;
    QCOMPARE(second.value(), int(25));

    second.setBinding(existingBinding);
    QCOMPARE(second.value(), int(10));
    QVERIFY(!existingBinding.isNull());
}

void tst_qproperty::stickyBinding()
{
    QProperty<int> prop;
    QProperty<int> prop2 {2};
    prop.setBinding([&](){ return prop2.value(); });
    QCOMPARE(prop.value(), 2);
    //给Prop设置Sticky标识位，所以prop = 1这句代码也不会将bindingData移除
    auto privBinding = QPropertyBindingPrivate::get(prop.binding());
    privBinding->setSticky();
    prop = 1;
    QVERIFY(prop.hasBinding());
    QCOMPARE(prop.value(), 1);
    //prop仍然收到prop2的影响
    prop2 = 3;
    QCOMPARE(prop.value(), 3);
    //sticky为false，给prop设置值，会将bindingData干掉
    privBinding->setSticky(false);
    prop = 42;
    QVERIFY(!prop.hasBinding());
}

void tst_qproperty::replaceBinding()
{
    QProperty<int> first(100);
    QProperty<int> second(Qt::makePropertyBinding(first));

    QCOMPARE(second.value(), 100);

    auto constantBinding = Qt::makePropertyBinding([]() { return 42; });
    auto oldBinding = second.setBinding(constantBinding);
    QCOMPARE(second.value(), 42);

    second.setBinding(oldBinding);
    QCOMPARE(second.value(), 100);
}

void tst_qproperty::changeHandler()
{
    QProperty<int> testProperty(0);
    QList<int> recordedValues;
    QList<int> recordedValues2;

    {
        auto handler = testProperty.onValueChanged([&]() {
            recordedValues << testProperty;
        });

        auto handler2 = testProperty.onValueChanged([&](){
            recordedValues2 << testProperty;
        });

        testProperty = 1;
        testProperty = 2;
    }
    testProperty = 3;

    QCOMPARE(recordedValues.count(), 2);
    QCOMPARE(recordedValues.at(0), 1);
    QCOMPARE(recordedValues.at(1), 2);
}

void tst_qproperty::propertyChangeHandlerApi()
{
    int changeHandlerCallCount = 0;
    QPropertyChangeHandler handler([&changeHandlerCallCount]() {
        ++changeHandlerCallCount;
    });

    QProperty<int> source1;
    QProperty<int> source2;

    handler.setSource(source1);

    source1 = 100;
    QCOMPARE(changeHandlerCallCount, 1);

    handler.setSource(source2);
    source1 = 101;
    QCOMPARE(changeHandlerCallCount, 1);

    source2 = 200;
    QCOMPARE(changeHandlerCallCount, 2);
}

void tst_qproperty::subscribe()
{
    QProperty<int> testProperty(42);
    QList<int> recordedValues;

    {
        auto handler = testProperty.subscribe([&]() {
            recordedValues << testProperty;
        });

        testProperty = 1;
        testProperty = 2;
    }
    testProperty = 3;

    QCOMPARE(recordedValues.count(), 3);
    QCOMPARE(recordedValues.at(0), 42);
    QCOMPARE(recordedValues.at(1), 1);
    QCOMPARE(recordedValues.at(2), 2);
}

void tst_qproperty::changeHandlerThroughBindings()
{
    QProperty<bool> trigger(false);
    QProperty<bool> blockTrigger(false);
    QProperty<bool> condition([&]() {
        bool triggerValue = trigger;
        bool blockTriggerValue = blockTrigger;
        return triggerValue && !blockTriggerValue;
    });
    bool changeHandlerCalled = false;
    auto handler = condition.onValueChanged([&]() {
        changeHandlerCalled = true;
    });

    QVERIFY(!condition);
    QVERIFY(!changeHandlerCalled);

    trigger = true;

    QVERIFY(condition);
    QVERIFY(changeHandlerCalled);
    changeHandlerCalled = false;

    trigger = false;

    QVERIFY(!condition);
    QVERIFY(changeHandlerCalled);
    changeHandlerCalled = false;

    blockTrigger = true;

    QVERIFY(!condition);
    QVERIFY(!changeHandlerCalled);
}

void tst_qproperty::dontTriggerDependenciesIfUnchangedValue()
{
    QProperty<int> property(42);

    bool triggered = false;
    QProperty<int> observer([&]() {
        triggered = true;
        return property.value();
    });

    QCOMPARE(observer.value(), 42);
    QVERIFY(triggered);
    triggered = false;
    property = 42;
    QCOMPARE(observer.value(), 42);
    QVERIFY(!triggered);
}

void tst_qproperty::bindingSourceLocation()
{
#if defined(QT_PROPERTY_COLLECT_BINDING_LOCATION)
    auto bindingLine = QT_SOURCE_LOCATION_NAMESPACE::source_location::current().line() + 1;
    auto binding = Qt::makePropertyBinding([]() { return 42; });
    QCOMPARE(QPropertyBindingPrivate::get(binding)->sourceLocation().line, bindingLine);
#else
    QSKIP("Skipping this in the light of missing binding source location support");
#endif
}

void tst_qproperty::bindingError()
{
    QProperty<int> prop(1);
    prop.setBinding([]() -> int {
        QPropertyBindingError error(QPropertyBindingError::UnknownError, QLatin1String("my error"));
        QPropertyBindingPrivate::currentlyEvaluatingBinding()->setError(std::move(error));
        return 0;
    });
    QCOMPARE(prop.value(), 0);
    QCOMPARE(prop.binding().error().description(), QString("my error"));
}

void tst_qproperty::bindingLoop()
{
    QProperty<int> firstProp;

    QProperty<int> secondProp([&]() -> int {
        return firstProp.value();
    });

    QProperty<int> thirdProp([&]() -> int {
        return secondProp.value();
    });

    firstProp.setBinding([&]() -> int {
        return secondProp.value() + thirdProp.value();
    });

    thirdProp.setValue(10);
    QCOMPARE(firstProp.binding().error().type(), QPropertyBindingError::BindingLoop);


    {
        QProperty<int> i;
        BindingLoopTester tester(&i);
        QCOMPARE(tester.bindableEagerProp().binding().error().type(), QPropertyBindingError::BindingLoop);
        QCOMPARE(tester.bindableEagerProp2().binding().error().type(), QPropertyBindingError::BindingLoop);
    }
    {
        BindingLoopTester tester;
        auto handler = tester.bindableEagerProp().onValueChanged([&]() {
            tester.bindableEagerProp().setBinding([](){return 42;});
        });
        tester.bindableEagerProp().setBinding([]() {return 42;});
        QCOMPARE(tester.bindableEagerProp().binding().error().type(), QPropertyBindingError::BindingLoop);
        QCOMPARE(tester.bindableEagerProp().binding().error().description(), "Binding set during binding evaluation!");
    }
}

void tst_qproperty::realloc()
{

}

void tst_qproperty::changePropertyFromWithinChangeHandler()
{

}

void tst_qproperty::changePropertyFromWithinChangeHandlerThroughDependency()
{

}

void tst_qproperty::settingPropertyValueDoesRemoveBinding()
{

}

void tst_qproperty::genericPropertyBinding()
{
    QProperty<int> property;

    {
        //QProperty的类型和绑定函数的类型不匹配
        QUntypedPropertyBinding doubleBinding(QMetaType::fromType<double>(),
                                              [](const QMetaType &, void *) -> bool {
                                                  Q_ASSERT(false);
                                                  return true;
                                              }, QPropertyBindingSourceLocation());
        QVERIFY(!property.setBinding(doubleBinding));
    }

    QUntypedPropertyBinding intBinding(QMetaType::fromType<int>(),
                                       [](const QMetaType &metaType, void *dataPtr) -> bool {
                                           Q_ASSERT(metaType.id() == qMetaTypeId<int>());
                                           //QPropertyData中只有T这个元素，地址一样
                                           int *intPtr = reinterpret_cast<int*>(dataPtr);
                                           *intPtr = 100;
                                           return true;
                                       }, QPropertyBindingSourceLocation());

    QVERIFY(property.setBinding(intBinding));

    QCOMPARE(property.value(), 100);
}

void tst_qproperty::genericPropertyBindingBool()
{
    QProperty<bool> property;

    QVERIFY(!property.value());

    QUntypedPropertyBinding boolBinding(QMetaType::fromType<bool>(),
                                        [](const QMetaType &, void *dataPtr) -> bool {
                                            auto boolPtr = reinterpret_cast<bool *>(dataPtr);
                                            *boolPtr = true;
                                            return true;
                                        }, QPropertyBindingSourceLocation());
    QVERIFY(property.setBinding(boolBinding));

    QVERIFY(property.value());
}

void tst_qproperty::setBindingFunctor()
{
    QProperty<int> property;
    QProperty<int> injectedValue(100);
    // Make sure that this picks the setBinding overload that takes a functor and
    // moves it correctly.
    property.setBinding([&injectedValue]() { return injectedValue.value(); });
    injectedValue = 200;
    QCOMPARE(property.value(), 200);
}

void tst_qproperty::multipleObservers()
{

}

void tst_qproperty::arrowAndStarOperator()
{

}

void tst_qproperty::notifiedProperty()
{

}

void tst_qproperty::typeNoOperatorEqual()
{
    struct Uncomparable
    {
        int data = -1;
        bool changedCalled = false;

        Uncomparable(int value = 0)
                : data(value)
        {}
        Uncomparable(const Uncomparable &other)
        {
            data = other.data;
            changedCalled = false;
        }
        Uncomparable(Uncomparable &&other)
        {
            data = other.data;
            changedCalled = false;
            other.data = -1;
            other.changedCalled = false;
        }
        Uncomparable &operator=(const Uncomparable &other)
        {
            data = other.data;
            return *this;
        }
        Uncomparable &operator=(Uncomparable &&other)
        {
            data = other.data;
            changedCalled = false;
            other.data = -1;
            other.changedCalled = false;
            return *this;
        }
        bool operator==(const Uncomparable&) = delete;
        bool operator!=(const Uncomparable&) = delete;

        void changed()
        {
            changedCalled = true;
        }
    };

    Uncomparable u1 = { 13 };
    Uncomparable u2 = { 27 };

    QProperty<Uncomparable> p1;
    QProperty<Uncomparable> p2(Qt::makePropertyBinding(p1));

    QCOMPARE(p1.value().data, p2.value().data);
    p1.setValue(u1);
    QCOMPARE(p1.value().data, u1.data);
    QCOMPARE(p1.value().data, p2.value().data);
    p2.setValue(u2);
    QCOMPARE(p1.value().data, u1.data);
    QCOMPARE(p2.value().data, u2.data);

    QProperty<Uncomparable> p3(Qt::makePropertyBinding(p1));
    p1.setValue(u1);
    QCOMPARE(p1.value().data, p3.value().data);

//    QNotifiedProperty<Uncomparable, &Uncomparable::changed> np;
//    QVERIFY(np.value().data != u1.data);
//    np.setValue(&u1, u1);
//    QVERIFY(u1.changedCalled);
//    u1.changedCalled = false;
//    QCOMPARE(np.value().data, u1.data);
//    np.setValue(&u1, u1);
//    QVERIFY(u1.changedCalled);
}

void tst_qproperty::bindingValueReplacement()
{
//    Test test;
//    test.text = 0;
//    test.bindIconText(0);
//    test.iconText.setValue(&test, 42); // should not crash
//    QCOMPARE(test.iconText.value(), 42);
//    test.text = 1;
//    QCOMPARE(test.iconText.value(), 42);
}

void tst_qproperty::quntypedBindableApi()
{

}

void tst_qproperty::readonlyConstQBindable()
{

}

void tst_qproperty::qobjectBindableManualNotify()
{

}

void tst_qproperty::qobjectBindableSignalTakingNewValue()
{

}

void tst_qproperty::testNewStuff()
{

}

void tst_qproperty::qobjectObservers()
{

}

void tst_qproperty::compatBindings()
{

}

void tst_qproperty::metaProperty()
{

}

void tst_qproperty::modifyObserverListWhileIterating()
{

}

void tst_qproperty::compatPropertyNoDobuleNotification()
{

}

void tst_qproperty::compatPropertySignals()
{

}

void tst_qproperty::noFakeDependencies()
{

}

void tst_qproperty::bindablePropertyWithInitialization()
{

}

void tst_qproperty::noDoubleNotification()
{

}

void tst_qproperty::groupedNotifications()
{

}

void tst_qproperty::groupedNotificationConsistency()
{

}

void tst_qproperty::uninstalledBindingDoesNotEvaluate()
{

}

void tst_qproperty::notify()
{

}

void tst_qproperty::bindableInterfaceOfCompatPropertyUsesSetter()
{

}

void tst_qproperty::selfBindingShouldNotCrash()
{
    QProperty<int> i;
    //自己绑定了自身，应该报错
    i.setBinding([&](){
        return i + 1;
    });
    QVERIFY(i.binding().error().hasError());
}

void tst_qproperty::qpropertyAlias()
{

}


QTEST_APPLESS_MAIN(tst_qproperty)
