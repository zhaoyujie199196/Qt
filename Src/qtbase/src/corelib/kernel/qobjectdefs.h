//
// Created by Yujie Zhao on 2023/2/22.
//

#ifndef QOBJECTDEFS_H
#define QOBJECTDEFS_H

#include <QtCore/qtmetamacros.h>
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

namespace QtPrivate {
    class QMetaTypeInterface;
}

/*
 * 元对象
 * stringdata：{1, 2, 2, 5, "aabbbbb"}这种格式，前面存放索引和长度，真实数据放在后面
 * */
class QMetaMethod;
struct QMetaObject
{
    class Connection;  //信号槽连接的前置声明

    enum Call {
        InvokeMetaMethod,
        ReadProperty,
        WriteProperty,
        ResetProperty,
        CreateInstance,
        IndexOfMethod,
        RegisterPropertyMetaType,
        RegisterMethodArgumentMetaType,
        BindableProperty,
        CustomCall,
        ConstructInPlace,
    };
    //基类的QMetaObject
    struct SuperData {
        const QMetaObject *direct = nullptr;
        SuperData() = default;
        constexpr SuperData(std::nullopt_t) : direct(nullptr) {}
        constexpr SuperData(const QMetaObject *mo) : direct(mo) {}
        constexpr const QMetaObject *operator->() const { return operator const QMetaObject *(); }

        constexpr operator const QMetaObject *() const {
            return direct;
        }
        template <const QMetaObject &MO>
        static constexpr SuperData link() {
            return SuperData(QMetaObject::staticMetaObject<MO>());
        }
    };

    struct Data {   //data的内容都在moc文件中初始化
        SuperData superdata;  //父的元对象信息
        const uint *stringdata;  //字符串信息，所有的信号 / 槽 / 属性 名字都放在stringdata中.
        const uint *data;     //QMetaObjectPrivate指针
        typedef void (*StaticMetacallFunction)(QObject *, QMetaObject::Call, int, void **);
        StaticMetacallFunction static_metacall;
        const SuperData *relatedMetaObjects;
        const QtPrivate::QMetaTypeInterface *const *metaTypes;
        void *extradata;
    };

    const char *className() const;
    const QMetaObject *superClass() const;

    bool inherits(const QMetaObject *metaObject) const noexcept ;
    QObject *cast(QObject *obj) const {
        return const_cast<QObject *>(cast(const_cast<const QObject *>(obj)));
    }
    const QObject *cast(const QObject *obj) const;

    template <const QMetaObject &MO>
    static constexpr const QMetaObject *staticMetaObject() {
        return &MO;
    }

    static QByteArray normalizedType(const char *type);

    int propertyCount() const;

    int methodOffset() const;
    //property的偏移，前面的property都是父类的属性，自己的属性放在后面
    //排列方式： Class1Prop1——Class1Prop2——Class2Prop1——Class2Prop2——ThisClassProp
    int propertyOffset() const;

    //根据名字查找property的索引
    int indexOfProperty(const char *name) const;

    //获取property
    QMetaProperty property(int index) const;

    //调用方法
    int static_metacall(Call, int, void **) const;
    static int metacall(QObject *, Call, int, void **);

    //内部信号触发的方法
    // internal index-based signal activation
    static void activate(QObject *sender, int signal_index, void **argv) { Q_ASSERT(false); }
    static void activate(QObject *sender, const QMetaObject *, int local_signal_index, void **argv);
    static void activate(QObject *sender, int signal_offset, int local_signal_index, void **argv) { Q_ASSERT(false); }

    static QByteArray normalizedSignature(const char *method);

    static bool checkConnectArgs(const char *signal, const char *method);
    static bool checkConnectArgs(const QMetaMethod &signal, const QMetaMethod &method);

public:
    Data d;
};

/*
 * 信号槽连接
 * */
class QMetaObject::Connection
{
    friend class QObject;
    friend class QObjectPrivate;
    friend struct QMetaObject;
public:
    Connection() : d_ptr(nullptr) {  }
    ~Connection();
    explicit Connection(void *data) : d_ptr(data) {  }
    Connection(const Connection &other);
    Connection &operator=(const Connection &other);
    operator bool() const { return !!d_ptr; }
    Connection(Connection &&other) noexcept : d_ptr(qExchange(other.d_ptr, nullptr)) {}

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(Connection)
    void swap(Connection &other) noexcept { qSwap(d_ptr, other.d_ptr); }

private:
    void *d_ptr;   //QObjectPrivate::Connection *
};

namespace QtPrivate {
    //测试是否含有Q_OBJECT宏
    //测试方法为查看是否含有qt_metacall(QMetaObject::Call, int, void **)函数
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
