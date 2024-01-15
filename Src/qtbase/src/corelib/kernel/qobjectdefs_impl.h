//
// Created by Yujie Zhao on 2023/11/1.
//

#ifndef QOBJECTDEFS_IMPL_H
#define QOBJECTDEFS_IMPL_H

#include <QtCore/qnamespace.h>

QT_BEGIN_NAMESPACE

class QObject;

namespace QtPrivate {
    /*
     * 移除 &
     * */
    template <typename T> struct RemoveRef { typedef T Type; };
    template <typename T> struct RemoveRef<T &> { typedef T Type; };
    /*
     * 移除const &,只有const或者只有 &不被移除
     * */
    template <typename T> struct RemoveConstRef { typedef T Type; };
    template <typename T> struct RemoveConstRef<const T &>{ typedef T Type; };

    /*
     * 展开生成IndexesList类
     * */
    template <class T>
    using InvokeGenSeq = typename T::Type;

    //IndexesList类，里面存放了Type，类型指向自身，InvokeGenSeq最终会使用到IndexesList::Type
    template <int...>
    struct IndexesList {
        using Type = IndexesList;
    };

    //连接拓展IndexesList的模板
    template <int N, class S1, class S2>
    struct ConcatSeqImpl{
    };
    //模板符合IndexesList<I1>和IndexesList<I2>的，从IndexList<I1..., (N + I2)...>继承，基础类为模板展开
    template <int N, int ...I1, int ...I2>
    struct ConcatSeqImpl<N, IndexesList<I1...>, IndexesList<I2...>> : IndexesList<I1..., (N + I2)...> {
    };
    template <int N, class S1, class S2>
    using ConcatSeq = InvokeGenSeq<ConcatSeqImpl<N, S1, S2>>;

    template <int N>
    struct GenSeq;
    template <int N>
    using makeIndexSequence = InvokeGenSeq<GenSeq<N>>;
    //展开生成IndexesList<>
    template <int N>
    struct GenSeq : ConcatSeq<N / 2, makeIndexSequence<N / 2>, makeIndexSequence<N - N / 2>> {
    };
    //展开条件到0和1就终止
    template <>
    struct GenSeq<0> : IndexesList<> {};
    template <>
    struct GenSeq<1> : IndexesList<0> {};

    //使用接口
    template <int N>
    struct Indexes {
        using Value = makeIndexSequence<N>;
    };

    /*
     * 模板生成链式结构
     * */
    template <typename ...>
    struct List {
    };
    template <typename Head, typename... Tail>
    struct List<Head, Tail...> {
        typedef Head Car;
        typedef List<Tail...> Cdr;
    };

    //连接List
    template <typename, typename>
    struct List_Append;
    template <typename... L1, typename... L2>
    struct List_Append<List<L1...>, List<L2...>> {
        typedef List<L1..., L2...> Value;
    };
    //List的前N个节点，依次展开取Car，通过List_Append压到一起
    template <typename L, int N>
    struct List_Left {
        typedef typename List_Append<List<typename L::Car>, typename List_Left<typename L::Cdr, N - 1>::Value>::Value Value;
    };
    template <typename L>
    struct List_Left<L, 0> {
        typedef List<> Value;
    };
    //获取第N个节点
    template <typename L, int N>
    struct List_Select {
        typedef typename List_Select<typename L::Cdr, N - 1>::Value Value;
    };
    template <typename L>
    struct List_Select<L, 0> {
        typedef typename L::Car Value;
    };


    /*
     * 返回值赋值，发射的信号返回值为void不进行赋值，其他类型使用operator=进行赋值
     * 重载逗号操作符：
     * https://blog.csdn.net/SlowIsFastLemon/article/details/104245954
     * https://www.zhihu.com/question/55174509
     */
    template <typename T>
    struct ApplyReturnValue {
        void *data;
        explicit ApplyReturnValue(void *data_) : data(data_) {}
    };
    template <typename T, typename U>
    void operator,(T &&value, const ApplyReturnValue<U> &container) {
        if (container.data) {
            *reinterpret_cast<U *>(container.data) = std::forward<T>(value);
        }
    }
    template <typename T>
    void operator,(T, const ApplyReturnValue<void> &) {}


    //普通函数指针类，用在信号槽连接
    template <typename Func>
    struct FunctionPointer {  //仿函数与lambda表达式会走到这个FunctionPointer
        enum {
            ArgumentCount = -1,  //参数个数
            IsPointerToMemberFunction = false, //类的成员函数指针？
        };
    };

    //执行函数 将IndexesList展开，arg展开为参数，arg[0]为返回值
    //通用模板
    template <typename, typename, typename, typename> struct FunctorCall;

    //普通的函数，Function的返回类型和R可以不同
    template <int... II, typename... SignalArgs, typename R, typename Function>
    struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, Function> {
        static void call(Function &f, void **arg) {
            //ApplyReturnValue重写了逗号操作符
            f((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };
    //类成员函数指针
    template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
    struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet(Obj::*)(SlotArgs...)> {
        static void call(SlotRet (Obj::*f)(SlotArgs...), Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue <R>(arg[0]);
        }
    };
    //类的const成员函数，信号与槽的返回值可以不同
    template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
    struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet(Obj::*)(SlotArgs...) const> {
        static void call(SlotRet(Obj::*f)(SlotArgs...) const, Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };

#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
    //类的noexcept成员
    template <int ...II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
    struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet(Obj::*(SlotArgs...) noexcept)> {
        static void call(SlotRet(Obj::*f)(SlotArgs...) noexcept, Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };
    //类的const noexcept成员
    template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
    struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet(Obj::*)(SlotArgs...) const noexcept> {
        static void call(SlotRet (Obj::*f)(SlotArgs...) const noexcept, Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };
#endif

    /*
     * 函数指针
     * */
    //含有返回值Ret，含有变长参数Args，为类Obj的成员方法
    //FunctionPointer<void((Object::*)())> pointer;
    template <class Obj, typename Ret, typename ...Args>
    struct FunctionPointer<Ret(Obj::*)(Args...)> {
        typedef Obj Object;
        typedef List<Args...> Arguments;
        typedef Ret ReturnType;
        typedef Ret(Obj::*Function)(Args...);  //将方法名定义为Function
        enum {
            ArgumentCount = sizeof...(Args),  //获取参数个数
            IsPointerToMemberFunction = true,  //类的成员函数
        };
        template <typename SignalArgs, typename R>
        static void call(Function f, Obj *o, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
        }
    };

    //含有返回值Ret，含有变长参数Args，为类Obj的const成员方法
    //FunctionPointer<void((FunctionClass::*)() const)> pointer;
    template <class Obj, typename Ret, typename... Args>
    struct FunctionPointer<Ret(Obj:: *)(Args...) const> {
        typedef Obj Object;
        typedef List<Args...> Argument;
        typedef Ret ReturnType;
        typedef Ret(Obj::*Function)(Args...) const;
        enum {
            ArgumentCount = sizeof...(Args),
            IsPointerToMemberFunction = true,
        };
        template <typename SignalArgs, typename R>
        static void call(Function f, Obj *o, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
        }
    };

    //全局函数
    template <class Ret, typename... Args>
    struct FunctionPointer<Ret (*) (Args...)> {
        typedef List<Args...> Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function)(Args...);
        enum {
            ArgumentCount = sizeof...(Args),
            IsPointerToMemberFunction = false,
        };
        template <typename SignalArgs, typename R>
        static void call(Function f, void *, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
        }
    };

#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
    template <class Obj, typename Ret, typename... Args>
    struct FunctionPointer<Ret(Obj::*)(Args...) noexcept> {
        typedef Obj Object;
        typedef List<Args...> Arguments;
        typedef Ret ReturnType;
        typedef Ret(Obj::*Function)(Args...) noexcept;
        enum {
            ArgumentCount = sizeof...(Args),
            IsPointerToMemberFunction = true,
        };
        template <typename SignalArgs, typename R>
        static void call(Function f, Obj *o, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
        }
    };
    template <class Obj, typename Ret, typename... Args>
    struct FunctionPointer<Ret(Obj::*)(Args...) const noexcept> {
        typedef Obj Object;
        typedef List<Args...> Arguments;
        typedef Ret ReturnType;
        typedef Ret(Obj::*Function)(Args...) const noexcept;
        enum {
            ArgumentCount = sizeof...(Args),
            IsPointerToMemberFunction = true,
        };
        template <typename SignalArgs, typename R>
        static void call(Function f, Obj *o, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
        }
    };
    template <typename Ret, typename... Args>
    struct FunctionPointer<Ret(*)(Args...) noexcept> {
        typedef List<Args...> Arguments;
        typedef Ret ReturnType;
        typedef Ret(*Function)(Args...) noexcept;
        enum {
            ArgumentCount = sizeof...(Args),
            IsPointerToMemberFunction = false
        };
        template <typename SignalArgs, typename R>
        static void call(Function f, void *, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
        }
    };
#endif

    template <typename Function, int N>
    struct Functor {
        template <typename SignalArgs, typename R>
        static void call(Function &f, void *, void **arg) {
            FunctorCall<typename Indexes<N>::Value, SignalArgs, R, Function>::call(f, arg);
        }
    };
}

namespace QtPrivate {
    template <typename T>
    struct NarrowingDetector {
        T t[1];
    };
    template <typename From, typename To, typename Enable = void>
    struct IsConvertibleWithoutNarrowing : std::false_type
    {};

    template <typename From, typename To>
    struct IsConvertibleWithoutNarrowing<From, To,
            std::void_t< decltype( NarrowingDetector<To>{ {std::declval<From>()} } ) >
    > : std::true_type {};


    //参数转换
    template <typename From, typename To, typename Enable = void>
    struct AreArgumentsConvertibleWithoutNarrowingBase : std::false_type {};

    //From与To的类型一致或者From可以转换为To
    template <typename From, typename To>
    struct AreArgumentsConvertibleWithoutNarrowingBase<From, To,
            std::enable_if_t<std::disjunction_v<std::is_same<From, To>, IsConvertibleWithoutNarrowing<From, To>>>
            > : std::true_type {};

    //检测参数是否匹配，A1是否可以转换为A2
    template <typename A1, typename A2>
    struct AreArgumentsCompatible {
        static int test(const typename RemoveRef<A2>::Type &);
        static char test(...);
        static const typename RemoveRef<A1>::Type &dummy();

        enum {
            value = sizeof(test(dummy())) == sizeof(int),
        };
#ifdef QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
        using AreArgumentsConvertibleWithoutNarrowing = AreArgumentsConvertibleWithoutNarrowingBase<std::decay<A1>, std::decay<A2>>;
        static_assert(AreArgumentsConvertibleWithoutNarrowing::value);
#endif
    };

    //类型不同的引用，不能转换
    template<typename A1, typename A2> struct AreArgumentsCompatible<A1, A2&> { enum { value = false }; };
    template<typename A> struct AreArgumentsCompatible<A&, A&> { enum { value = true }; };
    // void as a return value
    template<typename A> struct AreArgumentsCompatible<void, A> { enum { value = true }; };
    template<typename A> struct AreArgumentsCompatible<A, void> { enum { value = true }; };
    template<> struct AreArgumentsCompatible<void, void> { enum { value = true }; };


    //普通参数模板，为false
    template <typename List1, typename List2>
    struct CheckCompatibleArguments {
        enum { value = false, };
    };
    //两个模板参数都为空
    template <>
    struct CheckCompatibleArguments<List<>, List<>> {
        enum { value = true, };
    };
    //第二个模板参数是空
    template <typename List1>
    struct CheckCompatibleArguments<List1, List<>> {
        enum { value = true  };
    };
    //循环展开检测第一个参数是否完全匹配，直到出现上面的终止条件
    template <typename Arg1, typename Arg2, typename... Tail1, typename... Tail2>
    struct CheckCompatibleArguments<List<Arg1, Tail1...>, List<Arg2, Tail2...>> {
        enum {
            value = AreArgumentsCompatible<typename RemoveConstRef<Arg1>::Type, typename RemoveConstRef<Arg2>::Type>::value
                    && CheckCompatibleArguments<List<Tail1...>, List<Tail2...>>::value,
        };
    };

    /*
     * 获取仿函数的参数与ArgList符合的个数，ArgList为信号的参数列表
     * 原理：依次取信号的前N个参数，知道能够正确通过模板测试  failure is not error，编译参数不符合的代码也不会报错
     */
    template <typename Functor, typename ArgList>
    struct ComputeFunctorArgumentCount;

    template <typename Functor, typename ArgList, bool Done>
    struct ComputeFunctorArgumentCountHelper {
        enum { Value = -1, };
    };
    template <typename Functor, typename First, typename... ArgList>
    struct ComputeFunctorArgumentCountHelper<Functor, List<First, ArgList...>, false>
            : ComputeFunctorArgumentCount<Functor,
                typename List_Left<List<First, ArgList...>, sizeof...(ArgList)>::Value> {};

    template <typename Functor, typename... ArgList>
    struct ComputeFunctorArgumentCount<Functor, List<ArgList...>>
    {
        template <typename D> static D dummy();
        template <typename F> static auto test(F f) -> decltype(((f.operator()((dummy<ArgList>())...)), int()));
        static char test(...);
        enum {
            Ok = sizeof(test(dummy<Functor>())) == sizeof(int),
            Value = Ok ? int(sizeof...(ArgList)) : int(ComputeFunctorArgumentCountHelper<Functor, List<ArgList...>, Ok>::Value),
        };
    };

    //获取仿函数的返回值
    template <typename Functor, typename ArgList>
    struct FunctorReturnType;
    template <typename Functor, typename ...ArgList>
    struct FunctorReturnType<Functor, List<ArgList...>> {
        template <typename D> static D dummy();
        typedef decltype(dummy<Functor>().operator()((dummy<ArgList>())...)) Value;
    };


    class QSlotObjectBase
    {
        typedef void (*ImplFn)(int which, QSlotObjectBase *this_, QObject *receiver, void **args, bool *ret);
    public:
        enum Operation {
            Destroy,
            Call,  //调用
            Compare,

            NumOperations
        };

        explicit QSlotObjectBase(ImplFn fn)
                : m_ref(1)
                , m_impl(fn) {
        }

        inline bool compare(void **a) {  //函数指针是否相同
            bool ret = false;
            m_impl(Compare, this, nullptr, a, &ret);
            return ret;
        }

        inline int ref() noexcept {
            return m_ref.ref();
        }

        inline void destroyIfLastRef() noexcept  {
            if (!m_ref.deref()) {
                m_impl(Destroy, this, nullptr, nullptr, nullptr);
            }
        }

        inline void call(QObject *r, void **a) {
            m_impl(Call, this, r, a, nullptr);
        }

        bool isImpl(ImplFn f) const { return m_impl == f; }

    protected:
        virtual ~QSlotObjectBase() {}
    private:
        Q_DISABLE_COPY_MOVE(QSlotObjectBase)
    private:
        QAtomicInt m_ref;
        const ImplFn m_impl;
    };

    //obj::func的形式
    template <typename Func, typename Args, typename R>
    class QSlotObject : public QSlotObjectBase
    {
        typedef QtPrivate::FunctionPointer<Func> FuncType;
        Func function;
        static void impl(int which, QSlotObjectBase *this_, QObject *r, void **a, bool *ret)
        {
            switch(which) {
                case Destroy:
                    delete static_cast<QSlotObject *>(this_);
                    break;
                case Call:
                    FuncType::template call<Args, R>(static_cast<QSlotObject *>(this_)->function, static_cast<typename FuncType::Object *>(r), a);
                    break;
                case Compare:
                    *ret = *reinterpret_cast<Func *>(a) == static_cast<QSlotObject *>(this_)->function;
                    break;
                case NumOperations:
                    Q_ASSERT(false);
                    break;
            }
        }

    public:
        explicit QSlotObject(Func f) : QSlotObjectBase(&impl), function(f)
        {}
    };

    //static全局函数
    template <typename Func, typename Args, typename R>
    class QStaticSlotObject : public QSlotObjectBase
    {
        typedef QtPrivate::FunctionPointer<Func> FuncType;
        Func function;
        static void impl(int which, QSlotObjectBase *this_, QObject *r, void **a, bool *ret)
        {
            switch(which) {
                case Destroy:
                    delete static_cast<QStaticSlotObject *>(this_);
                    break;
                case Call:
                    FuncType::template call<Args, R>(static_cast<QStaticSlotObject*>(this_)->function, r, a);
                    break;
                case Compare:
                    break; //不做比较，全局函数不提供断开链接方法
                case NumOperations:
                    Q_ASSERT(false);
                    break;
            }
        }
    public:
        explicit QStaticSlotObject(Func f) : QSlotObjectBase(&impl), function(f) {}
    };

    //仿函数 / lambda表达式
    template <typename Func, int N, typename Args, typename R>
    class QFunctorSlotObject : public QSlotObjectBase
    {
        typedef QtPrivate::Functor<Func, N> FuncType;
        Func function;
        static void impl(int which, QSlotObjectBase *this_, QObject *r, void **a, bool *ret)
        {
            switch(which) {
                case Destroy:
                    delete static_cast<QFunctorSlotObject *>(this_);
                    break;
                case Call:
                    FuncType::template call<Args, R>(static_cast<QFunctorSlotObject *>(this_)->function, r, a);
                    break;
                case Compare:
                    break;  //不做比较，仿函数和lambda表达式不提供断开链接方法
                case NumOperations:
                    Q_UNUSED(ret);
                    break;
            }
        }
    public:
        explicit QFunctorSlotObject(Func f) : QSlotObjectBase(&impl), function(std::move(f)) {}
    };
}

QT_END_NAMESPACE

#endif //QOBJECTDEFS_IMPL_H
