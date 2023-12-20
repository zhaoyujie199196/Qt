//
// Created by Yujie Zhao on 2023/11/17.
//
#include "qproperty.h"
#include "qproperty_p.h"
#include <QtCore/qthread.h>
#include <QtCore/qvariant.h>
#include <QtCore/qbindingstorage.h>
#include <QtCore/qlogging.h>

QT_BEGIN_NAMESPACE

//thread_local: https://blog.csdn.net/hnyqzgdrz/article/details/130728921
static thread_local QBindingStatus bindingStatus;

struct QPropertyDelayedNotifications
{
    static constexpr inline auto PageSize = 4096;
    int ref = 0;
    QPropertyDelayedNotifications *next = nullptr;
    qsizetype used = 0;

    static constexpr qsizetype size = (PageSize - 3 * sizeof(void *) / sizeof(QPropertyProxyBindingData));
    QPropertyProxyBindingData delayedProperties[size];

    void addProperty(const QPropertyBindingData *bindingData, QUntypedPropertyData *propertyData) {
        Q_ASSERT(false);
    }

    void evaluateBinding(PendingBindingObserverList &bindingObservers, qsizetype index, QBindingStatus *status) {
        Q_ASSERT(false);
    }

    void notify(qsizetype index) {
        Q_ASSERT(false);
    }
};

QtPrivate::BindingEvaluationState::BindingEvaluationState(QPropertyBindingPrivate *binding, QBindingStatus *status)
    : binding(binding)
{
    Q_ASSERT(status);
    //将status中的currentBindEvaluatingBinding存储到previous中，然后设置成自身
    QBindingStatus *s = status;
    currentState = &s->currentlyEvaluatingBinding;
    previousState = *currentState;
    *currentState = this;
    binding->clearDependencyObservers();
}

QtPrivate::BindingEvaluationState::~BindingEvaluationState()
{
    //将QBindingStatus中的currentEvaluatingBinding设置成之前的状态
    *currentState = previousState;
}

QPropertyBindingError::QPropertyBindingError()
{
}

QPropertyBindingError::QPropertyBindingError(Type type, const QString &description)
{
    if (type != NoError) {
        d = new QPropertyBindingErrorPrivate;
        d->type = type;
        d->description = description;
    }
}

QPropertyBindingError::QPropertyBindingError(const QPropertyBindingError &other)
    : d(other.d)
{
}

QPropertyBindingError &QPropertyBindingError::operator=(const QPropertyBindingError &other)
{
    d = other.d;
    return *this;
}

QPropertyBindingError::QPropertyBindingError(QPropertyBindingError &&other)
    : d(std::move(other.d))
{
}

QPropertyBindingError &QPropertyBindingError::operator=(QPropertyBindingError &&other)
{
    d = std::move(other.d);
    return *this;
}

QPropertyBindingError::~QPropertyBindingError()
{
}

QPropertyBindingError::Type QPropertyBindingError::type() const
{
    if (!d) {
        return NoError;
    }
    return d->type;
}

QString QPropertyBindingError::description() const
{
    if (!d) {
        return QString();
    }
    return d->description;
}

QPropertyObserver::QPropertyObserver(ChangeHandler changeHandler)
{
    QPropertyObserverPointer d{this};
    d.setChangeHandler(changeHandler);
}

QPropertyObserver::QPropertyObserver(QUntypedPropertyData *data)
{
    Q_ASSERT(false);
    aliasData = data;
    next.setTag(ObserverIsAlias);
}

void QPropertyObserver::setSource(const QPropertyBindingData &property)
{
    QPropertyObserverPointer d{this};
    QPropertyBindingDataPointer propPrivate{&property};
    d.observerProperty(propPrivate);
}

QPropertyObserver::~QPropertyObserver()
{
    QPropertyObserverPointer d{this};
    d.unlink();
}

void QPropertyObserverPointer::observerProperty(QPropertyBindingDataPointer property)
{
    if (ptr->prev) {  //先从原链表中断开，在加入到新链表中
        unlink();
    }
    property.addObserver(ptr);
}

void QPropertyBindingDataPointer::addObserver(QPropertyObserver *observer)
{
    if (QPropertyBindingPrivate *b = binding())
    {
        observer->prev = &b->firstObserver.ptr;  //firstObserver中的ptr的地址
        observer->next = b->firstObserver.ptr;  //将next和firstObserver串联
        if (observer->next) {
            observer->next->prev = &observer->next;
        }
        b->firstObserver.ptr = observer;
    }
    else
    {
        //这段代码需要和QPropertyObserverPointer::unlink_common一起看
        auto &d = ptr->d_ref();
        Q_ASSERT(!(d & QPropertyBindingData::BindingBit));
        //当前的observer
        auto firstObserver = reinterpret_cast<QPropertyObserver *>(d);
        //observer的prev为QPropertyBindingData::d_ptr的内存地址
        observer->prev = reinterpret_cast<QPropertyObserver **>(&d);
        //将observer和当前的observer连接
        observer->next = firstObserver;
        if (observer->next) {
            //firstObserver->prev指向了observer->next的地址
            observer->next->prev = &observer->next;
        }
        d = reinterpret_cast<quintptr >(observer);
    }
}

void QPropertyObserverPointer::setBindingToNotify_unsafe(QPropertyBindingPrivate *binding)
{
    Q_ASSERT(ptr->next.tag() == QPropertyObserver::ObserverNotifiesBinding);
    ptr->binding = binding;
}

//检测不会出现自身监听自身的情况
void QPropertyObserverPointer::noSelfDependencies(QPropertyBindingPrivate *binding)
{
    auto observer = const_cast<QPropertyObserver *>(ptr);
    while (observer) {
        if (QPropertyObserver::ObserverTag(observer->next.tag()) == QPropertyObserver::ObserverNotifiesBinding) {
            if (observer->binding == binding) {
                qCritical("Property depends on itself !");
                break;
            }
        }
        observer = observer->next.data();
    }
}

void QPropertyObserverPointer::evaluateBindings(PendingBindingObserverList &bindingObservers, QBindingStatus *status)
{
    Q_ASSERT(status);
    auto observer = const_cast<QPropertyObserver *>(ptr);
    while (observer) {
        QPropertyObserver *next = observer->next.data();  //先存储next，下面的代码可能会改变next？
        //observer的链表中有时候会被插入Placeholder，需要跳过placeholder
        if (QPropertyObserver::ObserverTag(observer->next.tag()) == QPropertyObserver::ObserverNotifiesBinding) {
            auto bindingToEvaluate = observer->binding;
            QPropertyObserverNodeProtector protector(observer);
            QBindingObserverPtr bindingObserver(observer);
            if (bindingToEvaluate->evaluateRecursive_inline(bindingObservers, status)) {
                bindingObservers.push_back(std::move(bindingObserver));
            }
            next = protector.next();
        }
        observer = next;
    }
}

QPropertyBindingData::QPropertyBindingData(QPropertyBindingData &&other)
    : d_ptr(std::exchange(other.d_ptr, 0))
{
    QPropertyBindingDataPointer::fixupAfterMove(this);
}

QPropertyBindingData::~QPropertyBindingData()
{
    QPropertyBindingDataPointer d{this};
    if (isNotificationDelayed()) {
        Q_ASSERT(false);
        proxyData()->originalBindingData = nullptr;
    }
    //清理监听此BindingData的observer
    for (auto observer = d.firstObserver(); observer; ) {
        auto next = observer.nextObserver();
        observer.unlink();
        observer = next;
    }
    //清理此BindingData的observer
    if (auto binding = d.binding()) {
        binding->unlinkAndDeref();
    }
}

QUntypedPropertyBinding QPropertyBindingData::setBinding(const QUntypedPropertyBinding &binding,
                                                         QUntypedPropertyData *propertyDataPtr,
                                                         QtPrivate::QPropertyObserverCallback staticObserverCallback,
                                                         QtPrivate::QPropertyBindingWrapper guardCallback)
{
    QPropertyBindingPrivatePtr oldBinding;
    QPropertyBindingPrivatePtr newBinding = binding.d;

    QPropertyBindingDataPointer d{this};
    QPropertyObserverPointer observer;

    auto &data = d_ref();
    if (auto *existingBinding = d.binding()) {
        if (existingBinding == newBinding.data()) {  //要设置的binding就是当前binding
            return QUntypedPropertyBinding(static_cast<QPropertyBindingPrivate *>(oldBinding.data()));
        }
        if (existingBinding->isUpdating()) {  //正在对当前的binding求值，但是求值函数中又修改了绑定
            existingBinding->setError({QPropertyBindingError::BindingLoop, QStringLiteral("Binding set during binding evaluation!")});
            return QUntypedPropertyBinding(static_cast<QPropertyBindingPrivate *>(oldBinding.data()));
        }
        oldBinding = QPropertyBindingPrivatePtr(existingBinding);
        //需要将observer设置到新的bindingData中  TODO observer和bindingData分离的设计更好吧，搞个指针指向observer的数据结构就行了吧
        observer = static_cast<QPropertyBindingPrivate *>(oldBinding.data())->takeObservers();
        static_cast<QPropertyBindingPrivate *>(oldBinding.data())->unlinkAndDeref();  //oldBinding断开连接，尝试释放内存
        data = 0;
    }
    else {
        observer = d.firstObserver();
    }

    if (newBinding) {
        newBinding.data()->addRef();  //新的binding增加ref引用
        data = reinterpret_cast<quintptr>(newBinding.data());
        data |= BindingBit;   //将QPropertyBinding放到QProperty的BindingData中
        auto newBindingRaw = static_cast<QPropertyBindingPrivate *>(newBinding.data());
        newBindingRaw->setProperty(propertyDataPtr);  //设置bindingData的property
        if (observer) {
            //观测此Property的observer，挪到newBindingData中
            newBindingRaw->prependObserver(observer);
        }
        newBindingRaw->setStaticObserver(staticObserverCallback, guardCallback);

        //setBinding结束了，需要根据Binding计算QProperty的数据，在计算过程中，更新了Observer
        PendingBindingObserverList bindingObservers;
        newBindingRaw->evaluateRecursive(bindingObservers);
        //更新数据完毕，通知观察此QProperty的其他属性更新
        newBindingRaw->notifyNonRecursive(bindingObservers);
    }
    else if (observer) {
        d.setObservers(observer.ptr);
    }
    else {
        data = 0;
    }

    if (oldBinding) {
        static_cast<QPropertyBindingPrivate *>(oldBinding.data())->detachFromProperty();
    }
    return QUntypedPropertyBinding(static_cast<QPropertyBindingPrivate *>(oldBinding.data()));
}

//当前计算的QProperty中要监听此bindingData
void QPropertyBindingData::registerWithCurrentlyEvaluatingBinding_helper(QtPrivate::BindingEvaluationState *currentState) const
{
    QPropertyBindingDataPointer d{this};

    //防止重复计算
    if (currentState->alreadyCaptureProperties.contains(this)) {
        return;
    }
    else {
        currentState->alreadyCaptureProperties.push_back(this);
    }

    //从currentState的binding中分配observer
    QPropertyObserverPointer dependencyObserver = currentState->binding->allocateDependencyObserver();
    Q_ASSERT(QPropertyObserver::ObserverNotifiesBinding == 0);
    //observer设置所属的binding
    dependencyObserver.setBindingToNotify_unsafe(currentState->binding);
    //将observer添加到d的观察链表里
    d.addObserver(dependencyObserver.ptr);
}

void QPropertyBindingData::registerWithCurrentlyEvaluatingBinding() const
{
    auto currentState = bindingStatus.currentlyEvaluatingBinding;
    if (!currentState) {
        return;
    }
    registerWithCurrentlyEvaluatingBinding_helper(currentState);
}

void QPropertyBindingData::notifyObservers(QUntypedPropertyData *propertyDataPtr) const
{
    notifyObservers(propertyDataPtr, nullptr);
}

void QPropertyBindingData::notifyObservers(QUntypedPropertyData *propertyDataPtr, QBindingStorage *storage) const
{
    if (isNotificationDelayed()) {  //延迟通知？
        Q_ASSERT(false);
        return;
    }
    QPropertyBindingDataPointer d{this};
    //bindingObservers存放因为此数据变化而引起的其他变化了的监听
    //在更新过程中，使用了深度优先的策略，所以bindingObservers中越存放了观测observer的observer的数据
    PendingBindingObserverList bindingObservers;
    if (QPropertyObserverPointer observer = d.firstObserver()) {
        //notifyObserver_helper中更新observer的值
        if (notifyObserver_helper(propertyDataPtr, storage, observer, bindingObservers) == Evaluated) {
            //observer需要更新值的计算成功了，没有需要重新计算值的观察者，也算成功
            if (storage) {
                Q_ASSERT(false);
//                d = QPropertyBindingDataPointer { storage->bindingData(propertyDataPtr) };
            }
            if (QPropertyObserverPointer observer = d.firstObserver()) {
                observer.notify(propertyDataPtr);
            }
            for (auto &&bindingObserver: bindingObservers) {
                //监听者的数据有变化了, 继续通知
                bindingObserver.binding()->notifyNonRecursive();
            }
        }
    }
}

QPropertyBindingData::NotificationResult QPropertyBindingData::notifyObserver_helper(
        QUntypedPropertyData *propertyDataPtr, QBindingStorage *storage, QPropertyObserverPointer observer,
        PendingBindingObserverList &bindingObservers) const
{
    Q_UNUSED(storage);
    QBindingStatus *status = &bindingStatus;
    if (QPropertyDelayedNotifications *delay = status->groupUpdateData) {
        Q_ASSERT(false);  //延迟计算？
        delay->addProperty(this, propertyDataPtr);
        return Delayed;
    }

    observer.evaluateBindings(bindingObservers, status);
    return Evaluated;
}

//移除这个QProperty的监听其他QProperty的observer
void QPropertyBindingData::removeBinding_helper()
{
    QPropertyBindingDataPointer d{this};
    auto *existingBinding = d.binding();
    Q_ASSERT(existingBinding);
    if (existingBinding->isSticky()) {
        return;
    }
    auto observer = existingBinding->takeObservers();  //因为observer还有用，所以这里得使用take，免得被下面的unlinkAndDeref错误释放
    //将BindingData中的QPropertyBindingPrivate指针设置为空
    d_ref() = 0;  //bindingData被移除了，QPropertyBindingDataPointer中的d_ptr就存放observer
    if (observer) {
        d.setObservers(observer.ptr);
    }
    existingBinding->unlinkAndDeref();
}

void QPropertyBindingPrivatePtr::destroyAndFreeMemory()
{
    QPropertyBindingPrivate::destroyAndFreeMemory(static_cast<QPropertyBindingPrivate *>(d));
}

void QPropertyBindingPrivatePtr::reset(T *ptr) noexcept
{
    if (ptr == d) {
        return;
    }
    if (ptr) {
        ptr->ref++;
    }
    auto old = std::exchange(d, ptr);
    if (old && (--old->ref == 0)) {
        QPropertyBindingPrivate::destroyAndFreeMemory(static_cast<QPropertyBindingPrivate *>(d));
    }
}

QUntypedPropertyBinding::QUntypedPropertyBinding(QMetaType metaType, const BindingFunctionVTable *vtable,
                                                 void *function, const QPropertyBindingSourceLocation &location)
{
    auto size = QPropertyBindingPrivate::getSizeEnsuringAlignment() + vtable->size;
    //分配size个byte的空间，分配的内存在QPropertyBindingPrivatePtr的析构函数里做了销毁
    std::byte *mem = new std::byte[size];
    //在mem上执行构造函数。
    d = new(mem) QPropertyBindingPrivate(metaType, vtable, std::move(location));
    //在QPropertyBindingPrivate的地址后面存放了一个Callable的对象
    vtable->moveConstruct(mem + QPropertyBindingPrivate::getSizeEnsuringAlignment(), function);
}

QUntypedPropertyBinding::QUntypedPropertyBinding(QUntypedPropertyBinding &&other)
    : d(std::move(other.d))
{
}

QUntypedPropertyBinding::QUntypedPropertyBinding(const QUntypedPropertyBinding &other)
    : d(other.d)
{
}

QUntypedPropertyBinding &QUntypedPropertyBinding::operator=(const QUntypedPropertyBinding &other)
{
    d = other.d;
    return *this;
}

QUntypedPropertyBinding &QUntypedPropertyBinding::operator=(QUntypedPropertyBinding &&other)
{
    d = std::move(other.d);
    return *this;
}

QUntypedPropertyBinding::QUntypedPropertyBinding(QPropertyBindingPrivate *priv)
    : d(priv)
{
}

QUntypedPropertyBinding::~QUntypedPropertyBinding()
{
}

bool QUntypedPropertyBinding::isNull() const
{
    return !d;
}

QPropertyBindingError QUntypedPropertyBinding::error() const
{
    if (!d) {
        return QPropertyBindingError();
    }
    return static_cast<QPropertyBindingPrivate *>(d.get())->bindingError();
}

QMetaType QUntypedPropertyBinding::valueMetaType() const
{
    if (!d) {
        return QMetaType();
    }
    return static_cast<QPropertyBindingPrivate *>(d.get())->valueMetaType();
}

QPropertyBindingPrivate::~QPropertyBindingPrivate()
{
    if (firstObserver) {
        Q_ASSERT(false);
        firstObserver.unlink();  //zhaoyujie TODO 只需要断开firstObserver?
    }
    if (vtable->size) {
        vtable->destroy(reinterpret_cast<std::byte *>(this) + QPropertyBindingPrivate::getSizeEnsuringAlignment());
    }
}

//重新计算Property的值之前会调用此方法，用以重新进行observer的绑定
//不需要观察其他的Property了
void QPropertyBindingPrivate::clearDependencyObservers()
{
    for (size_t i = 0; i < qMin(dependencyObserverCount, inlineDependencyObservers.size()); ++i) {
        QPropertyObserverPointer p{ &inlineDependencyObservers[i] };
        p.unlink_fast();
    }
    if (heapObservers) {
        Q_ASSERT(false);
        heapObservers->clear();
    }
    dependencyObserverCount = 0;
}

QPropertyObserverPointer QPropertyBindingPrivate::allocateDependencyObserver_slow()
{
    ++dependencyObserverCount;
    if (!heapObservers) {
        heapObservers.reset(new std::vector<QPropertyObserver>());
    }
    Q_ASSERT(false);  //zhaoyujie TODO QPropertyObserverPointer和emplace_back的关系是啥样的。。。
    return { &heapObservers->emplace_back() };
}

void QPropertyBindingPrivate::unlinkAndDeref()
{
    clearDependencyObservers();  //清理属于此BindingData的observer
    propertyDataPtr = nullptr;   //unlink之后，已经不属于QProperty了
    if (--ref == 0) {
        destroyAndFreeMemory(this);    //引用-1，没有被其他引用的话，释放内存
    }
}

bool QPropertyBindingPrivate::evaluateRecursive(PendingBindingObserverList &bindingObservers, QBindingStatus *status)
{
    if (!status) {
        status = &bindingStatus;
    }
    return evaluateRecursive_inline(bindingObservers, status);
}

bool QPropertyBindingPrivate::evaluateRecursive_inline(PendingBindingObserverList &bindingObservers, QBindingStatus *status)
{
    if (updating) {  //如果正在更新QProperty的数据，触发了此函数，说明在更新数据时循环触发了
        error = QPropertyBindingError(QPropertyBindingError::BindingLoop);
        if (isQQmlPropertyBinding) {
            Q_ASSERT(false);
//            errorCallBack(this);
        }
        return false;
    }

    QPropertyBindingPrivatePtr keepAlive { this }; //调用了构造函数，构造函数里addRef，保证this不会被销毁
    QScopedValueRollback<bool> updateGuard(updating, true);  //将update设置为true，除了作用域再设置回原值
    //状态管理，并清理了属于当前BindingData的所有观察者，在计算的过程中要重新绑定Observer
    QtPrivate::BindingEvaluationState evaluationFrame(this, status);

    //bindingFunctor的地址紧跟在QPropertyBindingPrivate的地址之后，参见QUntypedPropertyBinding::QUntypedPropertyBinding函数
    auto bindingFunctor = reinterpret_cast<std::byte *>(this) + QPropertyBindingPrivate::getSizeEnsuringAlignment();
    bool changed = false;
    if (hasBindingWrapper) {
        changed = staticBindingWrapper(metaType, propertyDataPtr, {vtable, bindingFunctor});
    }
    else {
        //通过binding函数计算值
        changed = vtable->call(metaType, propertyDataPtr, bindingFunctor);
    }

    pendingNotify = pendingNotify || changed;
    if (!changed || !firstObserver) {
        return changed; //数据没有变化或者没有监听此QProperty的observer
    }
    firstObserver.noSelfDependencies(this);
    //如果计算结果有变化，继续通知观察此Property的其他Property，相当于深度遍历
    firstObserver.evaluateBindings(bindingObservers, status);
    return true;
}

void QPropertyBindingPrivate::notifyNonRecursive(const PendingBindingObserverList &bindingObservers)
{
    notifyNonRecursive();
    for (auto &&bindingObserver : bindingObservers) {
        bindingObserver.binding()->notifyNonRecursive();
    }
}

QPropertyBindingPrivate::NotificationState QPropertyBindingPrivate::notifyNonRecursive()
{
    //PendingNotify：不通知了，如果evaluate后的数据没有变化，则没必要通知。代码在外面给截断更好吧
    if (!pendingNotify) {
        return Delayed;
    }
    pendingNotify = false;
    Q_ASSERT(!updating);  //已经计算完毕了
    updating = true;  //自身的数据变化了，如果引起计算的observer中，又重新计算了此QProperty，则发生了绑定的错误
    if (firstObserver) {
        firstObserver.noSelfDependencies(this);
        firstObserver.notify(propertyDataPtr);
    }
    if (hasStaticObserver) {
        staticObserverCallback(propertyDataPtr);
    }
    updating = false;
    return Sent;
}

QPropertyBindingPrivate *QPropertyBindingPrivate::currentlyEvaluatingBinding()
{
    auto currentState = bindingStatus.currentlyEvaluatingBinding;
    return currentState ? currentState->binding : nullptr;
}

namespace PropertyAdaptorSlotObjectHelpers {
    void getter(const QUntypedPropertyData *d, void *value)
    {
        auto adaptor = static_cast<const QtPrivate::QPropertyAdaptorSlotObject *>(d);
        adaptor->bindingData().registerWithCurrentlyEvaluatingBinding();
        auto mt = adaptor->metaProperty().metaType();
        mt.destruct(value);
        mt.construct(value, adaptor->metaProperty().read(adaptor->object()).data());
    }

    void setter(QUntypedPropertyData *d, const void *value)
    {
        auto adaptor = static_cast<QtPrivate::QPropertyAdaptorSlotObject *>(d);
        adaptor->bindingData().removeBinding();
        adaptor->metaProperty().write(adaptor->object(),
                                      QVariant(adaptor->metaProperty().metaType(), value));
    }

    QUntypedPropertyBinding getBinding(const QUntypedPropertyData *d)
    {
        auto adaptor = static_cast<const QtPrivate::QPropertyAdaptorSlotObject *>(d);
        return QUntypedPropertyBinding(adaptor->bindingData().binding());
    }

    bool bindingWrapper(QMetaType type, QUntypedPropertyData *d,
                        QtPrivate::QPropertyBindingFunction binding, QUntypedPropertyData *temp,
                        void *value)
    {
        auto adaptor = static_cast<const QtPrivate::QPropertyAdaptorSlotObject *>(d);
        type.destruct(value);
        type.construct(value, adaptor->metaProperty().read(adaptor->object()).data());
        if (binding.vtable->call(type, temp, binding.functor)) {
            adaptor->metaProperty().write(adaptor->object(), QVariant(type, value));
            return true;
        }
        return false;
    }

    QUntypedPropertyBinding setBinding(QUntypedPropertyData *d, const QUntypedPropertyBinding &binding,
                                       QtPrivate::QPropertyBindingWrapper wrapper)
    {
        auto adaptor = static_cast<QtPrivate::QPropertyAdaptorSlotObject *>(d);
        return adaptor->bindingData().setBinding(binding, d, nullptr, wrapper);
    }

    void setObserver(const QUntypedPropertyData *d, QPropertyObserver *observer)
    {
        observer->setSource(static_cast<const QtPrivate::QPropertyAdaptorSlotObject *>(d)->bindingData());
    }
}

struct QBindingStorageData
{
    size_t size = 0;
    size_t used = 0;
    //Pair[] pairs;
};

struct QBindingStoragePrivate
{
    struct Pair {
        QUntypedPropertyData *data;
        QPropertyBindingData bindingData;
    };
    static_assert(alignof(Pair) == alignof(void *));
    static_assert(alignof(size_t) == alignof(void *));

    QBindingStorageData *&d;  //zhaoyujie TODO 这是什么意思？对指针的引用

    static inline Pair *pairs(QBindingStorageData *dd)
    {
        Q_ASSERT(dd);
        return reinterpret_cast<Pair *>(dd + 1);
    }

    void reallocate(size_t newSize)
    {
        Q_ASSERT(!d || newSize > d->size);
        size_t allocSize = sizeof(QBindingStorageData) + newSize * sizeof(Pair);
        void *nd = malloc(allocSize);
        memset(nd, 0, allocSize);
        QBindingStorageData *newData = new (nd) QBindingStorageData;
        newData->size = newSize;
        if (!d) {
            d = newData;
            return;
        }
        newData->used = d->used;
        Pair *p = pairs(d);
        for (size_t i = 0; i < d->size; ++i, ++p) {
            if (p->data) {
                Pair *pp = pairs(newData);
                Q_ASSERT(newData->size && (newData->size & (newData->size - 1)) == 0);  //size是2的次方
                size_t index = qHash(p->data) & (newData->size - 1);
                while (pp[index].data) {
                    ++index;
                    if (index == newData->size) {
                        index = 0;
                    }
                    new (pp + index) Pair{p->data, QPropertyBindingData(std::move(p->bindingData))};
                }
            }
        }
        free(p);
        d = newData;
    }

    QBindingStoragePrivate(QBindingStorageData *&_d) : d(_d) {}

    QPropertyBindingData *get(const QUntypedPropertyData *data)
    {
        Q_ASSERT(d);
        Q_ASSERT(d->size && (d->size & (d->size - 1)) == 0);
        size_t index = qHash(data) & (d->size - 1);
        Pair *p = pairs(d);
        while (p[index].data) {
            if (p[index].data == data) {
                return &p[index].bindingData;
            }
            ++index;
            if (index == d->size) {
                index = 0;
            }
        }
        return nullptr;
    }

    QPropertyBindingData *get(QUntypedPropertyData *data, bool create)
    {
        if (!d) {
            if (!create) {
                return nullptr;
            }
            reallocate(8);
        }
        else if (d->used * 2 >= d->size) {
            reallocate(d->size * 2);
        }
        Q_ASSERT(d->size && (d->size & (d->size - 1)) == 0);
        size_t index = qHash(data) & (d->size - 1);
        Pair *p = pairs(d);
        while (p[index].data) {
            if (p[index].data == data) {
                return &p[index].bindingData;
            }
            ++index;
            if (index == d->size) {
                index = 0;
            }
        }
        if (!create) {
            return nullptr;
        }
        ++d->used;
        new (p + index) Pair {data, QPropertyBindingData() };
        return &p[index].bindingData;
    }

    void destroy()
    {
        if (!d) {
            return;
        }
        Pair *p = pairs(d);
        for (size_t i = 0; i < d->size; ++i) {
            if (p->data) {
                p->~Pair();
            }
            ++p;
        }
        free(d);
    }
};

QBindingStorage::QBindingStorage()
{
    this->bindingStatus = &QT_PREPEND_NAMESPACE(bindingStatus);
    Q_ASSERT(this->bindingStatus);
}

QBindingStorage::~QBindingStorage()
{
    QBindingStoragePrivate(d).destroy();
}

void QBindingStorage::reinitAfterThreadMove()
{
    Q_ASSERT(false);
    this->bindingStatus = &QT_PREPEND_NAMESPACE(bindingStatus);
    Q_ASSERT(this->bindingStatus);
}

void QBindingStorage::clear()
{
    QBindingStoragePrivate(d).destroy();
    d = nullptr;
    this->bindingStatus = nullptr;
}

void QBindingStorage::registerDependency_helper(const QUntypedPropertyData *data) const
{
    Q_ASSERT(bindingStatus);
    QtPrivate::BindingEvaluationState *currentBinding;
    currentBinding = QT_PREPEND_NAMESPACE(bindingStatus).currentlyEvaluatingBinding;
    QUntypedPropertyData *dd = const_cast<QUntypedPropertyData *>(data);
    if (!currentBinding) {
        return;
    }
    auto storage = QBindingStoragePrivate(d).get(dd, true);
    if (!storage) {
        return;
    }
    storage->registerWithCurrentlyEvaluatingBinding(currentBinding);
}

QPropertyBindingData *QBindingStorage::bindingData_helper(const QUntypedPropertyData *data) const
{
    return QBindingStoragePrivate(d).get(data);
}

const QBindingStatus *QBindingStorage::status(QtPrivate::QBindingStatusAccessToken) const
{
    return bindingStatus;
}

QPropertyBindingData *QBindingStorage::bindingData_helper(QUntypedPropertyData *data, bool create)
{
    return QBindingStoragePrivate(d).get(data, create);
}

namespace QtPrivate {
    QPropertyAdaptorSlotObject::QPropertyAdaptorSlotObject(QObject *o, const QMetaProperty &p)
            : QSlotObjectBase(&impl), obj(o), metaProperty_(p) {
    }

    void QPropertyAdaptorSlotObject::impl(int which, QSlotObjectBase *this_, QObject *r, void **a, bool *ret) {
        auto self = static_cast<QPropertyAdaptorSlotObject *>(this_);
        switch (which) {
            case Destroy:
                delete self;
                break;
            case Call:
                if (!self->bindingData_.hasBinding())
                    self->bindingData_.notifyObservers(self);
                break;
            case Compare:
            case NumOperations:
                break;
        }
    }

    bool isPropertyInBindingWrapper(const QUntypedPropertyData *property)
    {
        if (const auto current = bindingStatus.currentCompatProperty) {
            return current->property == property;
        }
        return false;
    }

    void initBindingStatusThreadId()
    {
        bindingStatus.threadId = QThread::currentThreadId();
    }

    CompatPropertySafePoint::CompatPropertySafePoint(QBindingStatus *status, QUntypedPropertyData *property)
        : property(property)
    {
        currentState = &status->currentCompatProperty;
        previousState = *currentState;
        *currentState = this;

        currentlyEvaluatingBindingList = &bindingStatus.currentlyEvaluatingBinding;
        bindingState = *currentlyEvaluatingBindingList;
        *currentlyEvaluatingBindingList = nullptr;
    }

    QBindingStatus* getBindingStatus(QtPrivate::QBindingStatusAccessToken)
    {
        return &QT_PREPEND_NAMESPACE(bindingStatus);
    }
}



QT_END_NAMESPACE
