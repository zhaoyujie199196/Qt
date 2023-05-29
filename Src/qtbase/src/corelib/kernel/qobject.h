//
// Created by Yujie Zhao on 2023/2/24.
//

#ifndef QOBJECT_H
#define QOBJECT_H

#include <QtCore/qglobal.h>
#include <functional>
#include <map>
#include <string>
#include <vector>

QT_BEGIN_NAMESPACE

#define REGISTER_OBJECT_INVOKE_METHOD(functionName) \
    registerInvokeMethod(#functionName, [this](){ \
        this->functionName();                     \
    });

class QObject {
public:
    typedef std::function<void()> InvokeMethod;
    typedef std::vector<std::pair<std::string, InvokeMethod>> InvokeMethodMap;

    QObject() = default;
    virtual ~QObject() = default;
    //在实现Q_INVOKABLE之前先使用这种硬编码的方式
    virtual void registerInvokeMethods() {}
    const InvokeMethodMap &getInvokeMethodMap() const {return m_invokeMethodMap;}

protected:
    void registerInvokeMethod(const std::string &key, const InvokeMethod &func);

protected:
    InvokeMethodMap m_invokeMethodMap;
};

QT_END_NAMESPACE

#endif //QOBJECT_H
