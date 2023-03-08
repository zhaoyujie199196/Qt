#ifndef QTEST_H
#define QTEST_H

#include <QtTest/qtestcase.h>
#include <iostream>

QT_BEGIN_NAMESPACE

namespace QtTest {

#define QTEST_APPLESS_MAIN(Classname) \
int main(int argc, char *argv[]) {    \
    std::cout<<"test "<<#Classname<<"  begin..."<<std::endl;\
    Classname object;                 \
    object.registerInvokeMethods();   \
    int ret = QTest::qExec(&object);  \
    std::cout<<"test "<<#Classname<<"  finish..."<<std::endl; \
    return ret; \
}

}

#endif //QTEST_H