//
// Created by Yujie Zhao on 2023/2/24.
//

#include "qobject.h"

QT_BEGIN_NAMESPACE

void QObject::registerInvokeMethod(const std::string &key, const InvokeMethod &func)
{
    m_invokeMethodMap[key] = std::move(func);
}

QT_END_NAMESPACE