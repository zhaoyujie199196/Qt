//
// Created by Yujie Zhao on 2023/11/30.
//

#ifndef QBINDINGSTORAGE_H
#define QBINDINGSTORAGE_H

#include <QtCore/qglobal.h>
#include <QtCore/qnamespace.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
    struct BindingEvaluationState;
    struct QBindingStatusAccessToken;
    struct CompatPropertySafePoint;
}

struct QPropertyDelayedNotifications;
class QPropertyBindingData;
class QUntypedPropertyData;

struct QBindingStatus
{
    QtPrivate::BindingEvaluationState *currentlyEvaluatingBinding = nullptr;
    QtPrivate::CompatPropertySafePoint *currentCompatProperty = nullptr;
    Qt::HANDLE threadId = nullptr;
    QPropertyDelayedNotifications *groupUpdateData = nullptr;
};

namespace QtPrivate {
    QBindingStatus *getBindingStatus(QBindingStatusAccessToken);
}

struct QBindingStorageData;
class QBindingStorage
{
    mutable QBindingStorageData *d = nullptr;
    QBindingStatus *bindingStatus = nullptr;

    template <typename Class, typename T, auto Offset, auto Setter, auto Signal, auto Getter>
    friend class QObjectCompatProperty;
    friend class QObjectPrivate;
    friend class QPropertyBindingData;

public:
    QBindingStorage();
    ~QBindingStorage();
    bool isEmpty() { return !d; }
    bool isValid() const noexcept { return bindingStatus; }

    const QBindingStatus *status(QtPrivate::QBindingStatusAccessToken) const;

    void registerDependency(const QUntypedPropertyData *data) const
    {
        if (!bindingStatus || !bindingStatus->currentlyEvaluatingBinding) {
            return;
        }
        registerDependency_helper(data);
    }

    QPropertyBindingData *bindingData(const QUntypedPropertyData *data) const
    {
        if (!d) {
            return nullptr;
        }
        return bindingData_helper(data);
    }

    QPropertyBindingData *bindingData(QUntypedPropertyData *data, bool create)
    {
        if (!d && !create) {
            return nullptr;
        }
        return bindingData_helper(data, create);
    }

private:
    void reinitAfterThreadMove();
    void clear();
    void registerDependency_helper(const QUntypedPropertyData *data) const;
    QPropertyBindingData *bindingData_helper(const QUntypedPropertyData *data) const;
    QPropertyBindingData *bindingData_helper(QUntypedPropertyData *data, bool create);
};

QT_END_NAMESPACE

#endif //QBINDINGSTORAGE_H
