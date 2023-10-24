//
// Created by Yujie Zhao on 2023/10/20.
//
#include <QTest>
#include <QObject>

class tst_QObject : public QObject
{
public:
    void registerInvokeMethods() override {
        REGISTER_OBJECT_INVOKE_METHOD(test)
    }

    void test();
};

void tst_QObject::test() {
    QObject *obj = new QObject;
    obj->setProperty("objectName", "aaa");
    auto v = obj->property("objectName");
}

QTEST_APPLESS_MAIN(tst_QObject)