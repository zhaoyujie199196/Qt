//
// Created by Yujie Zhao on 2023/2/24.
//

#include "qtestrunner_p.h"
#include "qtesttable_p.h"
#include "qtestresult_p.h"

QT_BEGIN_NAMESPACE
using namespace QTest;

#define DATA_SUFFIX "_data"

QTestRunner::QTestRunner(QObject *object) noexcept
    : m_object(object)
{
    Q_ASSERT(object);
}

int QTestRunner::exec()
{
    init();
    auto ret = run();
    cleanup();
    return ret;
}

void QTestRunner::init()
{
}

int QTestRunner::run()
{
    const auto &functionMap = m_object->getInvokeMethodMap();

    auto isTestDataFunc=[](const std::string &funcName)->bool {
        return funcName.ends_with(DATA_SUFFIX);
    };
    auto getTestDataFunc=[&functionMap](const std::string &funcName)->QObject::InvokeMethod {
        auto testDataFuncName = funcName + DATA_SUFFIX;
        auto it = functionMap.find(testDataFuncName);
        if (it == functionMap.end()) {
            return nullptr;
        }
        else {
            return it->second;
        }
    };

    for (auto it = functionMap.begin(); it != functionMap.end(); ++it) {
        const std::string &funcName = it->first;
        if (isTestDataFunc(funcName)) {
            continue;
        }
        auto testDataFunc = getTestDataFunc(funcName);
        if (!testDataFunc) {
            it->second();
        }
        else {
            QTestTable table;
            testDataFunc();
            for (size_t i = 0; i < table.dataCount(); ++i)
            {
                auto testData = table.testData(i);
                QTestResult::setCurrentTestData(testData);
                it->second();
                QTestResult::setCurrentTestData(nullptr);
            }
        }
    }
    return 0;
}

void QTestRunner::cleanup()
{

}


QT_END_NAMESPACE