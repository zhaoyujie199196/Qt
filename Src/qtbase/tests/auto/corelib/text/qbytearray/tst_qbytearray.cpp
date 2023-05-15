//
// Created by Yujie Zhao on 2023/4/13.
//

#include <QObject>
#include <QTest>
#include <QByteArray>

class tst_QByteArray : public QObject {
public:
    using QObject::QObject;
    void registerInvokeMethods() override {

    }
};

QTEST_APPLESS_MAIN(tst_QByteArray)