//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QOBJECTDEFS_H
#define QOBJECTDEFS_H

#include <QtCore/qtmetamacros.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qbytearray.h>

QT_BEGIN_NAMESPACE

Q_CORE_EXPORT const char *qFlagLocation(const char *method);

//QLOCATION是什么用处？ zhaoyujie TODO
# define QLOCATION "\0" __FILE__ ":" QT_STRINGIFY(__LINE__)
# define METHOD(a)   qFlagLocation("0"#a QLOCATION)
# define SLOT(a)     qFlagLocation("1"#a QLOCATION)
# define SIGNAL(a)   qFlagLocation("2"#a QLOCATION)

// 函数成员类型
#define QMETHOD_CODE  0
#define QSLOT_CODE    1
#define QSIGNAL_CODE  2

class QObject;
class QMetaProperty;
class QMetaMethod;
class QMetaEnum;
class QMetaClassInfo;

namespace QtPrivate {
    class QMetaTypeInterface;
}

struct QMethodRawArguments
{
    void **arguments;
};

class Q_CORE_EXPORT QGenericArgument
{
public:
    inline QGenericArgument(const char *aName = nullptr, const void *aData = nullptr)
        : _data(aData), _name(aName)
    {}

    inline void *data() const { return const_cast<void *>(_data); }
    inline const char *name() const { return _name; }

private:
    const void *_data;
    const char *_name;
};

class Q_CORE_EXPORT QGenericReturnArgument : public QGenericArgument
{
public:
    inline QGenericReturnArgument(const char *aName = nullptr, void *aData = nullptr)
        : QGenericArgument(aName, aData)
    {}
};

template <class T>
class QArgument : public QGenericArgument
{
public:
    inline QArgument(const char *aName, const T &aData)
        : QGenericArgument(aName, static_cast<const void *>(&aData))
    {}
};

template <class T>
class QArgument<T &> : public QGenericArgument
{
public:
    inline QArgument(const char *aName, T &aData)
        : QGenericArgument(aName, static_cast<const void *>(&aData))
    {}
};

template <class T>
class QReturnArgument : public QGenericReturnArgument
{
public:
    inline QReturnArgument(const char *aName, T &aData)
        : QGenericReturnArgument(aName, static_cast<void *>(&aData))
    {}
};

struct Q_CORE_EXPORT QMetaObject
{
    class Connection;

    enum Call {
        InvokeMetaMethod,  //触发运行元方法
        ReadProperty,      //读属性
        WriteProperty,     //写属性
        ResetProperty,     //重置属性
        CreateInstance,    //创建实例
        IndexOfMethod,     //方法的索引
        RegisterPropertyMetaType,        //TODO
        RegisterMethodArgumentMetaType,  //TODO
        BindableProperty,  //TODO
        CustomCall,        //TODO
        ConstructInPlace,  //TODO
    };

    //QMetaObject的父信息
    struct SuperData {
        using Getter = const QMetaObject *(*)();
        const QMetaObject *direct;

        SuperData() = default;
        constexpr SuperData(std::nullptr_t) : direct(nullptr) {}
        constexpr SuperData(const QMetaObject *mo) : direct(mo) {}

        constexpr const QMetaObject *operator->() const { return operator const QMetaObject *(); }
        constexpr operator const QMetaObject *() const { return direct; }

        constexpr SuperData(Getter g) : direct(g()) {}
        template <const QMetaObject &MO>
        static constexpr SuperData link() {
            return SuperData(QMetaObject::staticMetaObject<MO>());
        }
    };

    struct Data {
        SuperData superdata;  //元对象的父
        const uint *stringdata;  //moc文件中的qt_meta_stringdata_XXX，存放字符串,字符串存储格式："QObject\0destroyed\0\0objectNameChanged\0objectName\0deleteLater\0parent"
        const uint *data;  //moc文件中的qt_meta_data_XXX，存放一些索引信息
        typedef void (*StaticMetacallFunction)(QObject *, QMetaObject::Call, int, void **);
        StaticMetacallFunction static_metacall;  //metacall方法
        const SuperData *relatedMetaObjects;  //TODO
        const QtPrivate::QMetaTypeInterface *const *metaTypes; //TODO
        void *extradata;  //TODO
    };
    const char *className() const;
    const QMetaObject *superClass() const { return d.superdata; }

    template <const QMetaObject &MO>
    static constexpr SuperData link() {
        return SuperData(QMetaObject::staticMetaObject<MO>());
    }
    template <const QMetaObject &MO>
    static constexpr const QMetaObject *staticMetaObject() {
        return &MO;
    }

    bool inherits(const QMetaObject *metaObject) const noexcept ;
    QObject *cast(QObject *obj) const;
    const QObject *cast(const QObject *obj) const;

    //自身的属性/方法/枚举等在所有信息中的偏移 (所有信息包含了父的相关信息)
    int methodOffset() const;
    int enumeratorOffset() const;
    int propertyOffset() const;
    int classInfoOffset() const;

    int constructorCount() const;
    int methodCount() const;
    int enumeratorCount() const;
    int propertyCount() const;
    int classInfoCount() const;

    int indexOfConstructor(const char *constructor) const;
    int indexOfMethod(const char *method) const;
    int indexOfSignal(const char *signal) const;
    int indexOfSlot(const char *slot) const;
    int indexOfEnumerator(const char *name) const;
    int indexOfProperty(const char *name) const;
    int indexOfClassInfo(const char *name) const;

    QMetaMethod constructor(int index) const;
    QMetaMethod method(int index) const;
    QMetaEnum enumerator(int index) const;
    QMetaProperty property(int index) const;
    QMetaClassInfo classInfo(int index) const;
    QMetaProperty userProperty() const;

    static QByteArray normalizedSignature(const char *method);
    static QByteArray normalizedType(const char *type);

    static bool checkConnectArgs(const char *signal, const char *method);
    static bool checkConnectArgs(const QMetaMethod &signal, const QMetaMethod &method);

    //发射信号后的激活函数
    static void activate(QObject *sender, int signalIndex, void **argv);
    static void activate(QObject *sender, const QMetaObject *, int local_signal_index, void **argv);
    static void activate(QObject *sender, int signal_offset, int local_signal_index, void **argv);

    int static_metacall(Call c1, int idx, void **argv) const;
    static int metacall(QObject *object, Call c1, int idx, void **argv);

    static void connectSlotsByName(QObject *p);

public:
    Data d;
};

//单个链接的外包
class Q_CORE_EXPORT QMetaObject::Connection
{
    void *d_ptr; //QObjectPrivate::Connection *
    explicit Connection(void *data) : d_ptr(data) {}
    friend class QObject;
    friend class QObjectPrivate;
    friend class QMetaObject;
    bool isConnected_helper() const;

public:
    Connection();
    Connection(const Connection &other);
    Connection &operator=(const Connection &other);
    ~Connection();
    Connection(Connection &&other) noexcept
            : d_ptr(std::exchange(other.d_ptr, nullptr))
    {}

    typedef void *Connection::*RestrictedBool;
    operator RestrictedBool() const
    {
        return d_ptr && isConnected_helper() ?  &Connection::d_ptr : nullptr;
    }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(Connection)
    void swap(Connection &other) noexcept
    {
        qt_ptr_swap(d_ptr, other.d_ptr);
    }
};

namespace QtPrivate {
    template <typename Object>
    struct HasQ_OBJECT_Macro {
        template <typename T>
        static char test(int (T::*)(QMetaObject::Call, int, void **));
        static int test(int (QObject::*)(QMetaObject::Call, int, void **));
        enum {
            Value = sizeof(test(&Object::qt_metacall)) == sizeof(int),
        };
    };
}



QT_END_NAMESPACE

#endif //QOBJECTDEFS_H
