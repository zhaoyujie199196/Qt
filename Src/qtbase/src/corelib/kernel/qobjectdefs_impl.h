//
// Created by Yujie Zhao on 2023/11/1.
//

#ifndef QOBJECTDEFS_IMPL_H
#define QOBJECTDEFS_IMPL_H

#include <QtCore/qnamespace.h>
#include <QtCore/private/qfunctiontools_impl.h>

QT_BEGIN_NAMESPACE

class QObject;
class QObjectPrivate;
class QMetaMethod;
class QByteArray;

namespace QtPrivate {

//移除 & / const &
template<typename T>
struct RemoveRef {
    typedef T Type;
};
template<typename T>
struct RemoveRef<T &> {
    typedef T Type;
};
template<typename T>
struct RemoveConstRef {
    typedef T Type;
};
template<typename T>
struct RemoveConstRef<const T &> {
    typedef T Type;
};


template<typename ObjPrivate>
inline void assertObjectType(QObjectPrivate *d);

template<typename Obj>
inline void assertObjectType(QObject *o) {
    // ensure all three compile
    [[maybe_unused]] auto staticcast = [](QObject *obj) { return static_cast<Obj *>(obj); };
    [[maybe_unused]] auto qobjcast = [](QObject *obj) { return Obj::staticMetaObject.cast(obj); };
#ifdef __cpp_rtti
    [[maybe_unused]] auto dyncast = [](QObject *obj) { return dynamic_cast<Obj *>(obj); };
    auto cast = dyncast;
#else
    auto cast = qobjcast;
#endif
    Q_ASSERT_X(cast(o), Obj::staticMetaObject.className(),
               "Called object is not of the correct type (class destructor may have already run)");
}

/*
 * 参数信息的链表  List<int, double, QObject *>
 * */
template<typename... Ts>
struct List {
    static const size_t size = sizeof...(Ts);  //参数个数
};

template<typename Head, typename... Tail>
struct List<Head, Tail...> {
    static constexpr size_t size = 1 + sizeof...(Tail);
    typedef Head Car;
    typedef List<Tail...> Cdr;
};

//参数数量
template<typename T>
struct SizeOfList {
    static constexpr size_t value = 1;
};
//0模板参数
template<>
struct SizeOfList<List<>> {
    static constexpr size_t value = 0;
};
//多模板参数
template<typename ...Ts>
struct SizeOfList<List<Ts...>> {
    static constexpr size_t value = List<Ts...>::size;
};

//连接参数
template<typename, typename>
struct List_Append;

template<typename... L1, typename... L2>
struct List_Append<List<L1...>, List<L2...>> {
    typedef List<L1..., L2...> Value;
};

//萃取L左边N个参数类型
template<typename L, int N>
struct List_Left {
    typedef typename List_Append<List<typename L::Car>, typename List_Left<typename L::Cdr, N - 1>::Value>::Value Value;
};

template<typename L>
struct List_Left<L, 0> {
    typedef List<> Value;
};

/*
* 设置槽函数的返回值
*/
template<typename T>
struct ApplyReturnValue {
    void *data;

    explicit ApplyReturnValue(void *data_) : data(data_) {}
};

//重写了,操作符，U是void不调用，U不是void走赋值
template<typename T, typename U>
void operator,(T &&value, const ApplyReturnValue<U> &container) {
    if (container.data) {
        *reinterpret_cast<U *>(container.data) = std::forward<T>(value);
    }
}

template<typename T>
void operator,(T, const ApplyReturnValue<void> &) {}

/*
 * */
template<class T> using InvokeGenSeq = typename T::Type;

//IndexesList<0, 1, 2, 3 >
template<int...>
struct IndexesList {
    using Type = IndexesList;
};

//连接S1, S2， S1与S2都是IndexesList
template<int N, class S1, class S2>
struct ConcatSeqImpl;

template<int N, int... I1, int... I2>
struct ConcatSeqImpl<N, IndexesList<I1...>, IndexesList<I2...>> : IndexesList<I1..., (N + I2)...> {
};

template<int N, class S1, class S2>
using ConcatSeq = InvokeGenSeq<ConcatSeqImpl<N, S1, S2>>;

template<int N>
struct GenSeq;
template<int N> using makeIndexSequence = InvokeGenSeq<GenSeq<N>>;

template<int N>
struct GenSeq : ConcatSeq<N / 2, makeIndexSequence<N / 2>, makeIndexSequence<N - N / 2>> {
};

template<>
struct GenSeq<0> : IndexesList<> {
};
template<>
struct GenSeq<1> : IndexesList<0> {
};

template<int N>
struct Indexes {
    using Value = makeIndexSequence<N>;
};

/*
 * 方法调用的模板
 * */
template<typename, typename, typename, typename>
struct FunctorCall;

template<int... II, typename... SignalArgs, typename R, typename Function>
struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, Function> {
    static void call(Function &f, void **arg) {
        f((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))..., ApplyReturnValue<R>(arg[0]));
    }
};

template<int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet(Obj::*)(SlotArgs...)> {
    static void call(SlotRet (Obj::*f)(SlotArgs...), Obj *o, void **arg) {
        assertObjectType<Obj>(o);
        (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
    }
};

template<int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...) const> {
    static void call(SlotRet (Obj::*f)(SlotArgs...) const, Obj *o, void **arg) {
        assertObjectType<Obj>(o);
        (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
    }
};

template<int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...) noexcept> {
    static void call(SlotRet (Obj::*f)(SlotArgs...) noexcept, Obj *o, void **arg) {
        assertObjectType<Obj>(o);
        (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
    }
};

template<int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...) const noexcept> {
    static void call(SlotRet (Obj::*f)(SlotArgs...) const noexcept, Obj *o, void **arg) {
        assertObjectType<Obj>(o);
        (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
    }
};

//函数指针基础类  lambda表达式 / 仿函数走这个类
template<typename Func>
struct FunctionPointer {
    enum {
        ArgumentCount = -1,  //参数个数
        IsPointerToMemberFunction = false //是否是类成员函数
    };
};

//符合 Ret Object::Func(Args...)类型的函数指针
//类中的普通成员函数指针
template<class Obj, typename Ret, typename... Args>
struct FunctionPointer<Ret (Obj::*)(Args...)> {
    typedef Obj Object;
    typedef List<Args...> Arguments;  //参数类型
    typedef Ret ReturnType;           //返回值类型
    typedef Ret(Obj::*Function)(Args...);  //定义函数指针类型
    enum {
        ArgumentCount = sizeof...(Args),
        IsPointerToMemberFunction = true
    };

    template<typename SignalArgs, typename R>
    static void call(Function f, Obj *o, void **arg) {
        FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
    }
};

//符合 Ret Object::Func(Args...) const 类型的函数指针
//类中的const成员函数
template<class Obj, typename Ret, typename... Args>
struct FunctionPointer<Ret (Obj::*)(Args...) const> {
    typedef Obj Object;
    typedef List<Args...> Arguments;
    typedef Ret ReturnType;

    typedef Ret (Obj::*Function)(Args...) const;

    enum {
        ArgumentCount = sizeof...(Args),
        IsPointerToMemberFunction = true
    };

    template<typename SignalArgs, typename R>
    static void call(Function f, Obj *o, void **arg) {
        FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
    }
};

// 符合 Ret Func(Args...)
// 普通全局函数
template<typename Ret, typename... Args>
struct FunctionPointer<Ret (*)(Args...)> {
    typedef List<Args...> Arguments;
    typedef Ret ReturnType;

    typedef Ret (*Function)(Args...);

    enum {
        ArgumentCount = sizeof...(Args),
        IsPointerToMemberFunction = false
    };

    template<typename SignalArgs, typename R>
    static void call(Function f, void *, void **arg) {
        FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
    }
};

// 符合 Ret Obj::Func(Args...) noexcept
template<typename Obj, typename Ret, typename... Args>
struct FunctionPointer<Ret (Obj::*)(Args...) noexcept> {
    typedef Obj Object;
    typedef List<Args...> Arguments;
    typedef Ret ReturnType;

    typedef Ret(Obj::*Function)(Args...) noexcept;

    enum {
        ArgumentCount = sizeof...(Args),
        IsPointerToMemberFunction = true
    };

    template<typename SignalArgs, typename R>
    static void call(Function f, Obj *o, void **arg) {
        FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
    }
};

// 符合 Ret Obj::Func(Args...) const noexcept
template<class Obj, typename Ret, typename... Args>
struct FunctionPointer<Ret (Obj::*)(Args...) const noexcept> {
    typedef Obj Object;
    typedef List<Args...> Arguments;
    typedef Ret ReturnType;

    typedef Ret(Obj::*Function)(Args...) const noexcept;

    enum {
        ArgumentCount = sizeof...(Args),
        IsPointerToMemberFunction = true
    };

    template<typename SignalArgs, typename R>
    static void call(Function f, Obj *o, void **arg) {
        FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
    }
};

template<typename Ret, typename ...Args>
struct FunctionPointer<Ret(*)(Args...) noexcept> {
    typedef List<Args...> Arguments;
    typedef Ret ReturnType;

    typedef Ret(*Function)(Args...) noexcept;

    enum {
        ArgumentCount = sizeof...(Args),
        IsPointerToMemberFunction = false
    };

    template<typename SignalArgs, typename R>
    static void call(Function f, void *, void **arg) {
        FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
    }
};

/*
 * 萃取给定函数对象的上下文类型, 使用范围：
 * QObject(s, &QObject::sig1, r, &TestQObject::onSig1)
 * QObject(s, &QObject::sig1, r, [](int){})
 */
template<typename Func, typename = void>
struct ContextTypeForFunctor {
};

//lambda表达式或者全局函数，使用QObject做上下文
template<typename Func>
struct ContextTypeForFunctor<Func, std::enable_if_t<
        !std::disjunction_v<std::is_convertible<Func, const char *>,
                std::is_member_function_pointer<Func>
        >>> {
    using ContextType = QObject;
};

//类的成员函数，萃取函数所属类的类型
template<typename Func>
struct ContextTypeForFunctor<Func,
        std::enable_if_t<std::conjunction_v<std::negation<std::is_convertible<Func, const char *>>,
                std::is_member_function_pointer<Func>,
                std::is_convertible<typename QtPrivate::FunctionPointer<Func>::Object *, QObject *>
        >>> {
    using ContextType = typename QtPrivate::FunctionPointer<Func>::Object;
};

/*
 * 探测是否两个类型之间是否可以相互转换
 * 检测方式: To t[1] = { std::declval<From>() }
 *
 * double a = 1.0; float b[1] = { a }; 会报收窄错误
 * 在初始化列表中，将一个非常量表达式从double类型缩小转换为float类型
 * 这种转换是不允许的。这个错误通常在编译时发生。
 */
template <class T>
struct NarrowingDetector {
    T t[1];
};

template <typename From, typename To, typename Enable = void>
struct IsConvertibleWithoutNarrowing : std::false_type {};

template <typename From, typename To>
struct IsConvertibleWithoutNarrowing<From, To,
        std::void_t<decltype(NarrowingDetector<To>{ {std::declval<From>} })>
        > : std::true_type {};

/*
 * 检测From与To是否一样，或者从From可以转向To，且没有收窄
 * */
template <typename From, typename To, typename Enable = void>
struct AreArgumentsConvertibleWithoutNarrowingBase : std::false_type {};

template <typename From, typename To>
struct AreArgumentsConvertibleWithoutNarrowingBase<From, To,
        std::enable_if_t<
                std::disjunction_v<std::is_same<From, To>, IsConvertibleWithoutNarrowing<From, To>>
        >
> : std::true_type {};

/*
 * 检测参数类型是否匹配
 * */

//检测单个参数类型是否匹配
template <typename A1, typename A2>
struct AreArgumentsCompatible
{
    static int test(const std::remove_reference_t<A2> &);  //func1
    static char test(...);  //func2
    enum {
        //A1能转换成A2，则调用func1，否则会调用func2
        value = sizeof(test(std::declval<std::remove_reference_t<A1>>())) == sizeof(int)
    };
};
//A1为非引用，A2为引用，不能转换
template <typename A1, typename A2>
struct AreArgumentsCompatible<A1, A2&> { enum { value = false }; };
template <typename A>
struct AreArgumentsCompatible<A &, A &> { enum { value = true }; };

// void as a return value
template<typename A> struct AreArgumentsCompatible<void, A> { enum { value = true }; };
template<typename A> struct AreArgumentsCompatible<A, void> { enum { value = true }; };
template<> struct AreArgumentsCompatible<void, void> { enum { value = true }; };

//检测参数列表是否匹配
template <typename List1, typename List2>
struct CheckCompatibleArguments {
    enum { value = false };
};

//终止条件：两个空List
template <>
struct CheckCompatibleArguments<List<>, List<>> {
    enum { value = true };
};

//终止条件：信号参数List非空，槽参数List为空
template <typename List1>
struct CheckCompatibleArguments<List1, List<>> {
    enum { value = true };
};

// 两个参数列表都为非空
// 通过AreArgumentsCompatible比较第一个参数， 再用CheckCompatibleArguments比较随后的参数
template <typename Arg1, typename Arg2, typename... Tail1, typename... Tail2>
struct CheckCompatibleArguments<List<Arg1, Tail1...>, List<Arg2, Tail2...>> {
    enum {
        value = AreArgumentsCompatible<typename RemoveConstRef<Arg1>::Type, typename RemoveConstRef<Arg2>::Type>::value
                && CheckCompatibleArguments<List<Tail1...>, List<Tail2...>>::value
    };
};

/*
 * 获取信号函数与槽函数匹配的参数个数
 * 如果信号参数与槽函数不匹配，值为-1
 * */
template <typename Functor, typename ArgList>
struct ComputeFunctorArgumentCount;

template <typename Functor, typename ArgList, bool Done>
struct ComputeFunctorArgumentCountHelper {
    enum { Value = -1 };
};

template <typename Functor, typename First, typename ...ArgList>
struct ComputeFunctorArgumentCountHelper<Functor, List<First, ArgList...>, false>
        : ComputeFunctorArgumentCount<Functor, typename List_Left<List<First, ArgList...>, sizeof...(ArgList)>::Value>
{};

template <typename Functor, typename... ArgList>
struct ComputeFunctorArgumentCount<Functor, List<ArgList...>>
{
    template <typename F>
    static auto test(F f)->decltype(((f.operator()((std::declval<ArgList>())...)), int()));
    static char test(...);
    enum {
        Ok = sizeof(test(std::declval<Functor>())) == sizeof(int),
        Value = Ok ? int(sizeof...(ArgList)) : int(ComputeFunctorArgumentCountHelper<Functor, List<ArgList...>, Ok>::Value)
    };
};

/*
 * 获取lambda表达式 / 仿函数返回值
 * */
template <typename Functor, typename ArgList> struct FunctorReturnType;

template <typename Functor, typename... ArgList>
struct FunctorReturnType<Functor, List<ArgList...>> {
    typedef decltype(std::declval<Functor>().operator()((std::declval<ArgList>())...)) Value;
};

//调用lambda表达试 / 仿函数
template <typename Function, int N>
struct Functor
{
    template <typename SignalArgs, typename R>
    static void call(Function &f, void *, void **arg) {
        FunctorCall<typename Indexes<N>::Value, SignalArgs, R, Function>::call(f, arg);
    }
};

template <typename Func>
struct ZeroArgFunctor : Functor<Func, 0>
{
    using RetureType = decltype(std::declval<Func>()());
    using Function = RetureType(*)();
    enum {
        ArgumentCount = 0,
    };
    using Arguments = QtPrivate::List<>;
};

//lambda表达试或仿函数的argumentCount为-1，走ZeroArgFunctor，否则走函数指针
template <typename Func>
using Callable = std::conditional_t<FunctionPointer<std::decay_t<Func>>::ArgumentCount == -1,
                 ZeroArgFunctor<std::decay_t<Func>>,
                 FunctionPointer<std::decay_t<Func>>
                 >;

/*
 * 封装ComputeFunctorArgumentCount与CheckCompatibleArgument
 * 参数不匹配返回-1
 * */
template <typename Prototype, typename Functor>
inline constexpr std::enable_if_t<!std::disjunction_v<std::is_convertible<Prototype, const char *>,
                                                      std::is_same<std::decay_t<Prototype>, QMetaMethod>,
                                                      std::is_convertible<Functor, const char *>,
                                                      std::is_same<std::decay_t<Functor>, QMetaMethod>
                                                      >, int>
countMatchingArguments()
{
    using ExpectedArguments = typename QtPrivate::FunctionPointer<Prototype>::Arguments;
    using Actual = std::decay_t<Functor>;

    if constexpr (QtPrivate::FunctionPointer<Actual>::IsPointerToMemberFunction
                  || QtPrivate::FunctionPointer<Actual>::ArgumentCount >= 0) {
        using ActualArguments = typename QtPrivate::FunctionPointer<Actual>::Arguments;
        if constexpr (QtPrivate::CheckCompatibleArguments<ExpectedArguments , ActualArguments >::Value)
            return QtPrivate::FunctionPointer<Actual>::ArgumentCount;
        else
            return -1;
    }
    else {
        //lambda 或者 functor
        return QtPrivate::ComputeFunctorArgumentCount<Actual, ExpectedArguments >::Value;
    }
}

class QSlotObjectBase
{
    QAtomicInt m_ref = 1;
    typedef void (*ImplFn)(int which, QSlotObjectBase *this_, QObject *receiver, void **args, bool *ret);
    const ImplFn m_impl;

protected:
    enum Operation {
        Destroy,
        Call,
        Compare,
        NumOperations
    };

public:
    explicit QSlotObjectBase(ImplFn fn) : m_impl(fn) {}

    struct Deleter
    {
        void operator()(QSlotObjectBase *p) const noexcept
        {
            if (p) {
                p->destroyIfLastRef();
            }
        }
        static void cleanup(QSlotObjectBase *p) noexcept { Deleter{}(p); }
    };

    bool ref() noexcept { return m_ref.ref(); }
    inline void destroyIfLastRef() noexcept {
        if (!m_ref.deref()) {
            m_impl(Destroy, this, nullptr, nullptr, nullptr);
        }
    }

    inline bool compare(void **a) {
        bool ret = false;
        m_impl(Compare, this, nullptr, a, &ret);
        return ret;
    }

    inline void call(QObject *r, void **a) {
        m_impl(Call, this, r, a, nullptr);
    }

    bool isImpl(ImplFn fn) const { return m_impl == fn; }

protected:
    ~QSlotObjectBase() {}

private:
    Q_DISABLE_COPY_MOVE(QSlotObjectBase)
};

//unique_ptr第二个模板参数为删除器
using SlotObjUniquePtr = std::unique_ptr<QSlotObjectBase, QSlotObjectBase::Deleter>;

inline SlotObjUniquePtr copy(const SlotObjUniquePtr &other) noexcept
{
    if (other) {
        other->ref();
    }
    return SlotObjUniquePtr{other.get()};
}

class SlotObjSharedPtr {
    SlotObjUniquePtr obj;

public:
    SlotObjSharedPtr() noexcept = default;
    SlotObjSharedPtr(std::nullptr_t) noexcept : SlotObjSharedPtr() {}

    //std::move会将o置为空
    explicit SlotObjSharedPtr(SlotObjUniquePtr o)
        : obj(std::move(o))
    {}

    SlotObjSharedPtr(const SlotObjSharedPtr &other) noexcept
        : obj{ copy(other.obj) }
    {}

    SlotObjSharedPtr &operator=(const SlotObjSharedPtr &other) noexcept
    {
        auto copy = other;
        swap(copy);
        return *this;
    }

    SlotObjSharedPtr(SlotObjSharedPtr &&other) noexcept = default;
    SlotObjSharedPtr &operator=(SlotObjSharedPtr &&other) noexcept = default;
    ~SlotObjSharedPtr() = default;

    void swap(SlotObjSharedPtr &other) noexcept { obj.swap(other.obj); }

    auto get() const noexcept { return obj.get(); }
    auto operator->() const noexcept { return get(); }

    explicit operator bool() const noexcept { return bool(obj); }
};

template <typename Func, typename Args, typename R>
class QCallableObject : public QSlotObjectBase,
                        private QtPrivate::CompactStorage<std::decay_t<Func>>
{
    using FunctorValue = std::decay_t<Func>;
    using Storage = QtPrivate::CompactStorage<FunctorValue>;
    using FuncType = std::conditional_t<std::is_member_function_pointer_v<FunctorValue>,
                         QtPrivate::FunctionPointer<FunctorValue >,
                         QtPrivate::Functor<FunctorValue, Args::size>
                     >;
    static void impl(int which, QSlotObjectBase *this_, QObject *r, void **a, bool *ret)
    {
        const auto that = static_cast<QCallableObject *>(this_);
        switch (which) {
            case Destroy:
                delete that;
                break;
            case Call:
                if constexpr (std::is_member_function_pointer_v<FunctorValue>)
                    FuncType::template call<Args, R>(that->object(), static_cast<typename FuncType::Object *>(r), a);
                else
                    FuncType::template call<Args, R>(that->object(), r, a);
                break;
            case Compare:
                if constexpr (std::is_member_function_pointer_v<FunctorValue>) {
                    *ret = *reinterpret_cast<FunctorValue *>(a) == that->object();
                    break;
                }
                Q_ASSERT(false);
            case NumOperations:
                Q_UNUSED(ret);
        }
    }

public:
    explicit QCallableObject(Func &&f) : QSlotObjectBase(&impl), Storage{ std::move(f) } {}
    explicit QCallableObject(const Func &f) : QSlotObjectBase(&impl), Storage{ f } {}
};


template <typename Prototype, typename Functor>
static constexpr std::enable_if_t<QtPrivate::countMatchingArguments<Prototype, Functor>() >= 0,
        QtPrivate::QSlotObjectBase *>
        makeCallableObject(Functor &&func)
{
    using ExpectedSignature = QtPrivate::FunctionPointer<Prototype>;
    using ExpectedRetureType = typename ExpectedSignature::RetureType;
    using ExpectedArguments = typename ExpectedSignature::Arguments;

    using ActualSignature = QtPrivate::FunctionPointer<Functor>;
    constexpr int MatchingArgumentCount = QtPrivate::countMatchingArguments<Prototype, Functor>();
    using ActualArguments = typename QtPrivate::List_Left<ExpectedArguments , MatchingArgumentCount>::Value;

    static_assert(int(ActualSignature::ArgumentCount) <= int(ExpectedSignature::ArgumentCount));

    return new QtPrivate::QCallableObject<std::decay_t<Functor>, ActualArguments , ExpectedRetureType >(std::forward<Functor>(func));
}

template <typename Prototype, typename Functor, typename = void>
struct AreFunctionsCompatible : std::false_type {};

template <typename Prototype, typename Functor>
struct AreFunctionsCompatible<Prototype, Functor, std::enable_if_t <
        std::is_same_v<decltype(QtPrivate::makeCallableObject<Prototype>(std::forward<Functor>(std::declval<Functor>()))),
                       QtPrivate::QSlotObjectBase *>>
        > : std::true_type {};

template <typename Prototype, typename Functor>
inline constexpr bool AssertCompatibleFunctions()
{
    static_assert(AreFunctionsCompatible<Prototype, Functor>::value);
    return true;
}


}

QT_END_NAMESPACE

#endif //QOBJECTDEFS_IMPL_H
