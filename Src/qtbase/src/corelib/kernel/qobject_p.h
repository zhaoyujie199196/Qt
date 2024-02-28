//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QOBJECT_P_H
#define QOBJECT_P_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/private/qobjectdefs_impl.h>
#include <QtCore/private/qproperty_p.h>

QT_BEGIN_NAMESPACE

class QVariant;
class QThreadData;

struct QDynamicMetaObjectData
{
    virtual ~QDynamicMetaObjectData() = default;
    virtual void objectDestroyed(QObject *) { delete this; }

    virtual QMetaObject *toDynamicMetaObject(QObject *) = 0;
    virtual int metaCall(QObject *, QMetaObject::Call, int _id, void **) = 0;

};

struct QSignalSpyCallbackSet
{
    typedef void (*BeginCallback)(QObject *caller, int signal_or_method_index, void **argv);
    typedef void (*EndCallback)(QObject *caller, int signal_or_method_index);

    BeginCallback signal_begin_callback;
    BeginCallback slot_begin_callback;
    EndCallback signal_end_callback;
    EndCallback slot_end_callback;
};

void Q_CORE_EXPORT qt_register_signal_spy_callbacks(QSignalSpyCallbackSet *callback_set);

extern Q_CORE_EXPORT QBasicAtomicPointer<QSignalSpyCallbackSet> qt_signal_spy_callback_set;

enum { QObjectPrivateVersion = QT_VERSION };

class Q_CORE_EXPORT QAbstractDeclarativeData
{
public:
    static void (*destroyed)(QAbstractDeclarativeData *, QObject *);
    static void (*signalEmitted)(QAbstractDeclarativeData *, QObject *, int, void **);
    static int  (*receivers)(QAbstractDeclarativeData *, const QObject *, int);
    static bool (*isSignalConnected)(QAbstractDeclarativeData *, const QObject *, int);
    static void (*setWidgetParent)(QObject *, QObject *); // Used by the QML engine to specify parents for widgets. Set by QtWidgets.
};

class Q_CORE_EXPORT QObjectPrivate : public QObjectData
{
public:
    Q_DECLARE_PUBLIC(QObject)
    typedef void (*StaticMetaCallFunction)(QObject *, QMetaObject::Call, int, void **);

    struct Connection;
    struct ConnectionData;
    struct ConnectionList;
    struct ConnectionOrSignalVector;
    struct SignalVector;
    struct Sender;
    struct TaggedSignalVector;
    using ConnectionDataPointer = QExplicitlySharedDataPointer<ConnectionData>;

    struct ExtraData
    {
        ExtraData(QObjectPrivate *ptr)
            : parent(ptr)
        {
        }

        inline void setObjectNameForwarder(const QString &name)
        {
            parent->q_func()->setObjectName(name);
        }

        inline void nameChangedForwarder(const QString &name)
        {
            emit parent->q_func()->objectNameChanged(name, QObject::QPrivateSignal());
        }

        QList<QByteArray> propertyNames;  //额外的自定义的property
        QList<QVariant> propertyValues;
        QList<int> runningTimes; //TODO
        QList<QPointer<QObject>> eventFilters;
        //定义objectName属性
        Q_OBJECT_COMPAT_PROPERTY(QObjectPrivate::ExtraData, QString, objectName,
                                 &QObjectPrivate::ExtraData::setObjectNameForwarder,
                                 &QObjectPrivate::ExtraData::nameChangedForwarder)
        QObjectPrivate *parent;
    };

    static QObjectPrivate *get(QObject *o) { return o->d_func(); }
    static const QObjectPrivate *get(const QObject *o) { return o->d_func(); }

    void ensureExtraData()
    {
        if (!extraData) {
            extraData = new ExtraData(this);
        }
    }

    void setParent_helper(QObject *o);
    void deleteChildren();

    int signalIndex(const char *signalName, const QMetaObject **meta = nullptr) const;

    inline void ensureConnectionData();
    inline void addConnection(int signal, Connection *c);
    static inline bool removeConnection(Connection *c);
    bool isSignalConnected(uint signalIndex, bool checkDeclarative = false) const;

    static QMetaObject::Connection connectImpl(const QObject *sender, int signal_index,
                                               const QObject *receiver, void **slot,
                                               QtPrivate::QSlotObjectBase *slotObj, int type,
                                               const int *types, const QMetaObject *senderMetaObject);

    inline bool isDeclarativeSignalConnected(uint signal_index) const;
    bool maybeSignalConnected(uint signalIndex) const;

public:
    //mutable关键字：https://liam.page/2017/05/25/the-mutable-keyword-in-Cxx/
    mutable ExtraData *extraData;
    QAtomicPointer<QThreadData> threadData;

    QAtomicPointer<ConnectionData> connections;

    union {
        QObject *currentChildBeingDeleted;    //正在被删除的子
        QAbstractDeclarativeData *declarativeData;  //声明式模块使用的额外数据
    };
};

inline bool QObjectPrivate::isDeclarativeSignalConnected(uint signal_index) const
{
    //TODO 为啥要判断isDeletingChildren
    return !isDeletingChildren && declarativeData && QAbstractDeclarativeData::isSignalConnected
        && QAbstractDeclarativeData::isSignalConnected(declarativeData, q_func(), signal_index);
}

inline const QBindingStorage *qGetBindingStorage(const QObjectPrivate *o)
{
    return &o->bindingStorage;
}

inline const QBindingStorage *qGetBindingStorage(QObjectPrivate *o)
{
    return &o->bindingStorage;
}

inline const QBindingStorage *qGetBindingStorage(const QObjectPrivate::ExtraData *ed)
{
    return &ed->parent->bindingStorage;
}

inline QBindingStorage *qGetBindingStorage(QObjectPrivate::ExtraData *ed)
{
    return &ed->parent->bindingStorage;
}


QT_END_NAMESPACE

#endif //QOBJECT_P_H
