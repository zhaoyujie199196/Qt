//
// Created by Yujie Zhao on 2023/6/1.
//
#include "qobjectdefs.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qvariant.h>
#include <QtCore/qobject.h>
#include <QtCore/qmutex.h>
#include <QtCore/private/qthread_p.h>
#include <QtCore/private/qtrace_p.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/private/qtools_p.h>
#include "qobjectdefs.h"
#include "qmetaobject_p.h"
#include "qobject_p.h"

QT_BEGIN_NAMESPACE

static inline bool is_ident_char(char s)
{
    return isAsciiLetterOrNumber(s) || s == '_';
}

static inline bool is_space(char s)
{
    return (s == ' ' || s == '\t');
}

static void qRemoveWhitespace(const char *s, char *d)
{
    char last = 0;
    while (*s && is_space(*s))
        s++;
    while (*s) {
        while (*s && !is_space(*s))
            last = *d++ = *s++;
        while (*s && is_space(*s))
            s++;
        if (*s && ((is_ident_char(*s) && is_ident_char(last))
                   || ((*s == ':') && (last == '<')))) {
            last = *d++ = ' ';
        }
    }
    *d = '\0';
}

static QByteArray normalizeTypeInternal(const char *t, const char *e)
{
    int len = QtPrivate::qNormalizeType(t, e, nullptr);
    if (len == 0)
        return QByteArray();
    QByteArray result(len, Qt::Uninitialized);
    len = QtPrivate::qNormalizeType(t, e, result.data());
    Q_ASSERT(len == result.size());
    return result;
}

//从参数字符串中解析出参数列表
static void argumentTypesFromString(const char *str, const char *end, QArgumentTypeArray &types)
{
    Q_ASSERT(str <= end);
    while (str != end) {
        if (!types.isEmpty())
            ++str; // Skip comma
        const char *begin = str;
        int level = 0;
        while (str != end && (level > 0 || *str != ',')) {
            if (*str == '<')
                ++level;
            else if (*str == '>')
                --level;
            ++str;
        }
        QByteArray argType(begin, str - begin);
        argType.replace("QVector<", "QList<");
        types += QArgumentType(std::move(argType));
    }
}

//获取QMetaObject中string索引为index的字符串
static inline const char *rawStringData(const QMetaObject *mo, int index)
{
    uint offset = mo->d.stringdata[2 * index];
    return reinterpret_cast<const char *>(mo->d.stringdata) + offset;
}

static inline QLatin1String stringDataView(const QMetaObject *mo, int index)
{
    uint offset = mo->d.stringdata[2 * index];
    uint length = mo->d.stringdata[2 * index + 1];
    const char *str = reinterpret_cast<const char *>(mo->d.stringdata) + offset;
    return QLatin1String(str, qsizetype(length));
}

static inline QByteArray stringData(const QMetaObject *mo, int index)
{
    const auto view = stringDataView(mo, index);
    return QByteArray::fromRawData(view.data(), view.size());
}

static inline int typeFromTypeInfo(const QMetaObject *mo, uint typeinfo)
{
    if (!(typeinfo & IsUnresolvedType)) {
        return typeinfo;
    }
    return QMetaType::fromName(rawStringData(mo, typeinfo & TypeNameIndexMask)).id();
}

static inline QByteArray typeNameFromTypeInfo(const QMetaObject *mo, uint typeinfo)
{
    //判断是否是QMetaType注册的类型，如果是，使用QMetaType中的信息，否则信息存放在moc文件的stringdata字符串中
    if (typeinfo & IsUnresolvedType) {
        return stringData(mo, typeinfo & TypeNameIndexMask);
    }
    else {
        return QMetaType(typeinfo).name();
    }
}

//内部使用的private方法，直接使用继承没有使用代理
//可以直接调用基类的方法，不需要重新定义接口来转发调用基类的方法
class QMetaMethodPrivate : public QMetaMethodInvoker
{
public:
    static const QMetaMethodPrivate *get(const QMetaMethod *q)
    {
        return static_cast<const QMetaMethodPrivate *>(q);
    }

    //获取函数签名，函数签名带上参数
    inline QByteArray signature() const;
    inline QByteArray name() const;
    //参数信息
    inline int returnType() const;
    inline int parameterType(int index) const;
    inline int parameterCount() const;
    inline int typesDataIndex() const;
    inline int parametersDataIndex() const;
    inline uint parameterTypeInfo(int index) const;
    inline QByteArray parameterTypeName(int index) const;
    inline QList<QByteArray> parameterTypes() const;
    inline QList<QByteArray> parameterNames() const;
    inline int ownMethodIndex() const;
    inline const QtPrivate::QMetaTypeInterface *returnMetaTypeInterface() const;
    inline const QtPrivate::QMetaTypeInterface *const *parameterMetaTypeInterfaces() const;

private:
    void checkMethodMetaTypeConsistency(const QtPrivate::QMetaTypeInterface *iface, int index) const;
    QMetaMethodPrivate() = default;
};

static inline const QMetaObjectPrivate *priv(const uint *data)
{
    return reinterpret_cast<const QMetaObjectPrivate *>(data);
}

//查找克隆方法的原方法
//带有默认参数的函数，在moc中会生成多个函数，生成的函数带有MethodCloned标志，原方法不带此标志
int QMetaObjectPrivate::originalClone(const QMetaObject *mobj, int local_method_index)
{
    Q_ASSERT(local_method_index < get(mobj)->methodCount);
    while (QMetaMethod::fromRelativeMethodIndex(mobj, local_method_index).data.flags() & MethodCloned) {
        Q_ASSERT(local_method_index > 0);
        --local_method_index;
    }
    return local_method_index;
}

QByteArray QMetaObjectPrivate::decodeMethodSignature(const char *signature, QArgumentTypeArray &types)
{
    Q_ASSERT(signature != nullptr);
    //strchr：查找第一个出现的字符并返回字符后面的字符串位置
    const char *lparens = strchr(signature, '(');
    if (!lparens) {
        return QByteArray();
    }
    const char *rparens = strrchr(lparens + 1, ')');
    if (!rparens || *(rparens + 1)) {
        return QByteArray();
    }
    int nameLength = lparens - signature;
    argumentTypesFromString(lparens + 1, rparens, types);
    return QByteArray::fromRawData(signature, nameLength);
}

template <int MethodType>
inline int QMetaObjectPrivate::indexOfMethodRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types)
{
    for (const QMetaObject *m = *baseObject; m; m = m->d.superdata)
    {
        Q_ASSERT(priv(m->d.data)->revision >= 7);
        int i = (MethodType == MethodSignal) ? (priv(m->d.data)->signalCount - 1) : (priv(m->d.data)->methodCount - 1);
        const int end = (MethodType == MethodSlot) ? (priv(m->d.data)->signalCount) : 0;
        for (; i >= end; --i) {
            auto data = QMetaMethod::fromRelativeMethodIndex(m, i);
            if (methodMatch(m, data, name, argc, types)) {
                *baseObject = m;
                return i;
            }
        }
    }
    return -1;
}

int QMetaObjectPrivate::indexOfSignalRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types)
{
    int i = indexOfMethodRelative<MethodSignal>(baseObject, name, argc, types);
//#ifndef QT_NO_DEBUG
//    const QMetaObject *m = *baseObject;
//    if (i >= 0 && m && m->d.superdata) {
//        int conflict = indexOfMethod(m->d.superdata, name, argc, types);
//        if (conflict >= 0) {
//            QMetaMethod conflictMethod = m->d.superdata->method(conflict);
//            qWarning("QMetaObject::indexOfSignal: signal %s from %s redefined in %s",
//                     conflictMethod.methodSignature().constData(),
//                     objectClassName(m->d.superdata), objectClassName(m));
//        }
//    }
//#endif
    return i;
}

int QMetaObjectPrivate::indexOfSlotRelative(const QMetaObject **m, const QByteArray &name, int argc, const QArgumentType *types)
{
    return indexOfMethodRelative<MethodSlot>(m, name, argc, types);
}

QMetaMethod QMetaObjectPrivate::signal(const QMetaObject *m, int signal_index)
{
    if (signal_index < 0) {
        return QMetaMethod();
    }
    Q_ASSERT(m != nullptr);
    int i = signal_index;
    i -= signalOffset(m);
    if (i < 0 && m->d.superdata) {
        return signal(m->d.superdata, signal_index);
    }

    if (i >= 0 && i < priv(m->d.data)->signalCount) {
        return QMetaMethod::fromRelativeMethodIndex(m, i);
    }
    return QMetaMethod();
}

int QMetaObjectPrivate::indexOfSignal(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types)
{
    int i = indexOfSignalRelative(&m, name, argc, types);
    if (i >= 0) {
        i += m->methodOffset();
    }
    return i;
}

int QMetaObjectPrivate::indexOfSlot(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types)
{
    int i = indexOfSlotRelative(&m, name, argc, types);
    if (i >= 0) {
        i += m->methodOffset();
    }
    return i;
}

int QMetaObjectPrivate::indexOfMethod(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types)
{
    int i = indexOfMethodRelative<0>(&m, name, argc, types);
    if (i > 0) {
        i += m->methodOffset();
    }
    return i;
}

int QMetaObjectPrivate::indexOfConstructor(const QMetaObject *m, const QByteArray &name, int argc, const QArgumentType *types)
{
    for (int i = priv(m->d.data)->constructorCount - 1; i >= 0; --i) {
        const QMetaMethod method = QMetaMethod::fromRelativeConstructorIndex(m, i);
        if (methodMatch(m, method, name, argc, types)) {
            return i;
        }
    }
    return -1;
}

bool QMetaObjectPrivate::methodMatch(const QMetaObject *m, const QMetaMethod &method, const QByteArray &name, int argc, const QArgumentType *types)
{
    const QMetaMethod::Data &data = method.data;
    auto priv = QMetaMethodPrivate::get(&method);
    //比较参数个数
    if (priv->parameterCount() != argc) {
        return false;
    }
    //比较函数名字
    if (stringData(m, data.name()) != name) {
        return false;
    }

    //检查参数类型是否相同
    const QtPrivate::QMetaTypeInterface * const *ifaces = priv->parameterMetaTypeInterfaces();
    int paramsIndex = data.parameters() + 1;
    for (int i = 0; i < argc; ++i) {
        uint typeInfo = m->d.data[paramsIndex + 1];
        if (int id = types[i].type()) {
            if (id == QMetaType(ifaces[i]).id()) {
                continue;
            }
            if (id != typeFromTypeInfo(m, typeInfo)) {
                return false;
            }
        }
        else {
            if (types[i].name() == QMetaType(ifaces[i]).name())
                continue;
            if (types[i].name() != typeNameFromTypeInfo(m, typeInfo))
                return false;
        }
    }
    return true;
}

bool QMetaObjectPrivate::checkConnectArgs(int signalArgc, const QArgumentType *signalTypes, int methodArgc, const QArgumentType *methodTypes)
{
    if (signalArgc < methodArgc) {
        return false;
    }
    for (int i = 0; i < methodArgc; ++i) {
        if (signalTypes[i] != methodTypes[i]) {
            return false;
        }
    }
    return true;
}

bool QMetaObjectPrivate::checkConnectArgs(const QMetaMethodPrivate *signal, const QMetaMethodPrivate *method)
{
    if (signal->methodType() != QMetaMethod::Signal) {
        return false;
    }
    //TODO 没有考虑method的默认参数？
    if (signal->parameterCount() < method->parameterCount()) {
        return false;
    }
    const QMetaObject *smeta = signal->enclosingMetaObject();
    const QMetaObject *rmeta = method->enclosingMetaObject();
    for (int i = 0; i < method->parameterCount(); ++i) {
        uint sourceTypeInfo = signal->parameterTypeInfo(i);
        uint targetTypeInfo = method->parameterTypeInfo(i);
        if ((sourceTypeInfo & IsUnresolvedType) || (targetTypeInfo & IsUnresolvedType)) {
            QByteArray sourceName = typeNameFromTypeInfo(smeta, sourceTypeInfo);
            QByteArray targetName = typeNameFromTypeInfo(rmeta, targetTypeInfo);
            if (sourceName != targetName) {
                return false;
            }
        }
        else {
            int sourceType = typeFromTypeInfo(smeta, sourceTypeInfo);
            int targetType = typeFromTypeInfo(rmeta, targetTypeInfo);
            if (sourceType != targetType) {
                return false;
            }
        }
    }
    return true;
}

int QMetaObjectPrivate::absoluteSignalCount(const QMetaObject *m)
{
    Q_ASSERT(m != nullptr);
    int n = priv(m->d.data)->signalCount;
    for (m = m->d.superdata; m; m = m->d.superdata) {
        n += priv(m->d.data)->signalCount;
    }
    return n;
}

static inline const char *objectClassName(const QMetaObject *m)
{
    return rawStringData(m, priv(m->d.data)->className);
}

const char *QMetaObject::className() const
{
    return objectClassName(this);
}

bool QMetaObject::inherits(const QMetaObject *metaObject) const noexcept
{
    const QMetaObject *m = this;
    while (m) {
        if (metaObject == m) {
            return true;
        }
        m = m->d.superdata;
    }
    return false;
}

QObject *QMetaObject::cast(QObject *obj) const
{
    return const_cast<QObject *>(cast(const_cast<const QObject *>(obj)));
}

const QObject *QMetaObject::cast(const QObject *obj) const
{
    return (obj && obj->metaObject()->inherits(this)) ? obj : nullptr;
}

#define GET_OFFSET_FUNC(member) \
int offset = 0;                 \
const QMetaObject *m = d.superdata; \
while (m) {                     \
    offset += priv(m->d.data)->member; \
    m = m->d.superdata;\
}                               \
return offset;


int QMetaObject::methodOffset() const
{
    GET_OFFSET_FUNC(methodCount)
}

int QMetaObject::enumeratorOffset() const
{
    GET_OFFSET_FUNC(enumeratorCount)
}

int QMetaObject::propertyOffset() const
{
    GET_OFFSET_FUNC(propertyCount)
}

int QMetaObject::classInfoOffset() const
{
    GET_OFFSET_FUNC(classInfoCount)
}

#undef GET_OFFSET_FUNC


QMetaProperty QMetaObject::property(int index) const
{
    int i = index;
    i -= propertyOffset();
    if (i < 0 && d.superdata) {
        return d.superdata->property(index);
    }
    if (i >= 0 && i < priv(d.data)->propertyCount) {
        return QMetaProperty(this, i);
    }
    return QMetaProperty();
}

QMetaMethod QMetaObject::method(int index) const
{
    int i = index;
    i -= methodOffset();
    if (i < 0 && d.superdata)
        return d.superdata->method(index);

    if (i >= 0 && i < priv(d.data)->methodCount)
        return QMetaMethod::fromRelativeMethodIndex(this, i);
    return QMetaMethod();
}

QMetaEnum QMetaObject::enumerator(int index) const
{
    int i = index;
    i -= enumeratorOffset();
    if (i < 0 && d.superdata)
        return d.superdata->enumerator(index);

    if (i >= 0 && i < priv(d.data)->enumeratorCount)
        return QMetaEnum(this, i);
    return QMetaEnum();
}

QMetaClassInfo QMetaObject::classInfo(int index) const
{
    int i = index;
    i -= classInfoOffset();
    if (i < 0 && d.superdata)
        return d.superdata->classInfo(index);

    QMetaClassInfo result;
    if (i >= 0 && i < priv(d.data)->classInfoCount) {
        result.mobj = this;
        result.data = { d.data + priv(d.data)->classInfoData + i * QMetaClassInfo::Data::Size };
    }
    return result;
}

int QMetaObject::constructorCount() const
{
    return priv(d.data)->constructorCount;
}

#define GET_COUNT_FUNC(member) \
int n = priv(d.data)->member;  \
const QMetaObject *m = d.superdata; \
while (m) {                    \
    n += priv(m->d.data)->member;  \
    m = m->d.superdata;                           \
}                              \
return n;

int QMetaObject::methodCount() const
{
    GET_COUNT_FUNC(methodCount)
}

int QMetaObject::enumeratorCount() const
{
    GET_COUNT_FUNC(enumeratorCount)
}

int QMetaObject::propertyCount() const
{
    GET_COUNT_FUNC(propertyCount)
}

int QMetaObject::classInfoCount() const
{
    GET_COUNT_FUNC(classInfoCount)
}

#undef GET_COUNT_FUNC

int QMetaObject::indexOfProperty(const char *name) const
{
    const QMetaObject *m = this;
    while (m) {
        const QMetaObjectPrivate *d = priv(m->d.data);
        for (int i = 0; i < d->propertyCount; ++i) {
            const QMetaProperty::Data data = QMetaProperty::getMetaPropertyData(m, i);
            const char *prop = rawStringData(m, data.name());
            if (strcmp(name, prop) == 0) {
                i += m->propertyOffset();
                return i;
            }
        }
        m = m->d.superdata;
    }
    if (priv(this->d.data)->flags & DynamicMetaObject) {
        //TODO 创建Property
        Q_ASSERT(false);
    }
    return -1;
}

bool QMetaObject::checkConnectArgs(const char *signal, const char *method)
{
    const char *s1 = signal;
    const char *s2 = method;
    while (*s1++ != '(') {}
    while (*s2++ != '(') {}

    //完全匹配
    if (*s2 == ')' || qstrcmp(s1, s2) == 0) {
        return true;
    }
    const auto s1len = qstrlen(s1);
    const auto s2len = qstrlen(s2);
    //method的参数比signal少
    if (s2len < s1len && strncmp(s1, s2, s2len - 1) == 0 && s1[s2len - 1] == ',') {
        return true;
    }
    return false;
}

bool QMetaObject::checkConnectArgs(const QMetaMethod &signal, const QMetaMethod &method)
{
    auto s = QMetaMethodPrivate::get(&signal);
    auto m = QMetaMethodPrivate::get(&method);
    return QMetaObjectPrivate::checkConnectArgs(s, m);
}

static char *qNormalizeType(char *d, int &templdepth, QByteArray &result)
{
    const char *t = d;
    while (*d && (templdepth
                  || (*d != ',' && *d != ')'))) {
        if (*d == '<')
            ++templdepth;
        if (*d == '>')
            --templdepth;
        ++d;
    }
    // "void" should only be removed if this is part of a signature that has
    // an explicit void argument; e.g., "void foo(void)" --> "void foo()"
    if (strncmp("void)", t, d - t + 1) != 0)
        result += normalizeTypeInternal(t, d);

    return d;
}

QByteArray QMetaObject::normalizedSignature(const char *method)
{
    QByteArray result;
    if (!method || !*method)
        return result;
    int len = int(strlen(method));
    QVarLengthArray<char> stackbuf(len + 1);
    char *d = stackbuf.data();
    qRemoveWhitespace(method, d);

    result.reserve(len);

    int argdepth = 0;
    int templdepth = 0;
    while (*d) {
        if (argdepth == 1) {
            d = qNormalizeType(d, templdepth, result);
            if (!*d) //most likely an invalid signature.
                break;
        }
        if (*d == '(')
            ++argdepth;
        if (*d == ')')
            --argdepth;
        result += *d++;
    }

    return result;
}

QByteArray QMetaObject::normalizedType(const char *type)
{
    return normalizeTypeInternal(type, type + qstrlen(type));
}

int QMetaObject::static_metacall(Call c1, int idx, void **argv) const
{
    Q_ASSERT(priv(d.data)->revision >= 6);
    if (!d.static_metacall) {
        return 0;
    }
    d.static_metacall(nullptr, c1, idx, argv);
    return -1;
}

int QMetaObject::metacall(QObject *object, Call call, int idx, void **argv)
{
    if (object->d_ptr->metaObject) {
        return object->d_ptr->metaObject->metaCall(object, call, idx, argv);
    }
    else {
        return object->qt_metacall(call, idx, argv);
    }
}

QMetaProperty::QMetaProperty(const QMetaObject *mobj, int index)
    : mobj(mobj)
    , data(getMetaPropertyData(mobj, index))
{
    Q_ASSERT(index >= 0 && index < priv(mobj->d.data)->propertyCount);

    if (!(data.flags() & EnumOrFlag)) {
        return;
    }
    //TODO 对枚举做特殊处理
    Q_ASSERT(false);
}



int QMetaProperty::Data::index(const QMetaObject *mobj) const
{
    return (d - mobj->d.data - priv(mobj->d.data)->propertyData) / Size;
}

QMetaProperty::Data QMetaProperty::getMetaPropertyData(const QMetaObject *mobj, int index)
{
    //起始地址 + property的偏移 + index的偏移
    return { mobj->d.data + priv(mobj->d.data)->propertyData + index * Data::Size };
}

QVariant QMetaProperty::read(const QObject *object) const
{
    if (!mobj || !object) {
        return QVariant();
    }
    int status = -1;
    QVariant value;
    void *argv[] = { nullptr, &value, &status };
    QMetaType t(mobj->d.metaTypes[data.index(mobj)]);
    if (t == QMetaType::fromType<QVariant>()) {
        argv[0] = value.data();
    }
    else {
        value = QVariant(t, nullptr);
        argv[0] = value.data();
    }
    if (priv(mobj->d.data)->flags & PropertyAccessInStaticMetaCall && mobj->d.static_metacall) {
        mobj->d.static_metacall(const_cast<QObject *>(object), QMetaObject::ReadProperty, data.index(mobj), argv);
    }
    else {
        //TODO 不是从static_metacall调用
        Q_ASSERT(false);
    }
    if (status != -1) {
        return value;
    }
    Q_ASSERT(false);
    if (t != QMetaType::fromType<QVariant>() && argv[0] != value.data()) {
        return QVariant(t, argv[0]);
    }
    return value;
}

bool QMetaProperty::write(QObject *object, const QVariant &value) const
{
    if (!object || !isWritable()) {
        return false;
    }
    //构建右值
    return write(object, QVariant(value));
}

bool QMetaProperty::write(QObject *object, QVariant &&v) const
{
    if (!object || !isWritable()) {
        return false;
    }
    QMetaType t(mobj->d.metaTypes[data.index(mobj)]);
    if (t != QMetaType::fromType<QVariant>() && t != v.metaType()) {
        if (isEnumType() && !t.metaObject() && v.metaType().id() == QMetaType::QString) {
            //对枚举 / QString做处理 TODO
            Q_ASSERT(false);
        }
        else if (!v.isValid()) {   //v非法，尝试重置或者晴空
            if (isResettable()) {
                return reset(object);
            }
            v = QVariant(t, nullptr);
        }
        else if (!v.convert(t)) {  //数据之间不可以互相转换
            return false;
        }
    }

    int status = -1;  //status之类的都被使用到
    int flags = 0;
    void *argv[] = { nullptr, &v, &status, &flags };
    if (t == QMetaType::fromType<QVariant>()) {
        argv[0] = &v;
    }
    else {
        argv[0] = v.data();
    }
    if (priv(mobj->d.data)->flags & PropertyAccessInStaticMetaCall && mobj->d.static_metacall) {
        mobj->d.static_metacall(object, QMetaObject::WriteProperty, data.index(mobj), argv);
    }
    else {
        //TODO
        Q_ASSERT(false);
    }
    return status;
}

bool QMetaProperty::reset(QObject *object) const
{
    if (!object || !mobj || !isResettable()) {
        return false;
    }
    void *argv[] = {nullptr};
    //zhaoyujie TODO PropertyAccessInStaticMetaCall是什么作用？
    if (priv(mobj->d.data)->flags & PropertyAccessInStaticMetaCall && mobj->d.static_metacall) {
        Q_ASSERT(false);
        mobj->d.static_metacall(object, QMetaObject::ResetProperty, data.index(mobj), argv);
    }
    else {
        Q_ASSERT(false);
        QMetaObject::metacall(object, QMetaObject::ResetProperty, data.index(mobj) + mobj->propertyOffset(), argv);
    }
    return true;
}

QMetaMethod::MethodType QMetaMethod::methodType() const
{
    if (!mobj) {
        return QMetaMethod::Method;
    }
    auto flag = data.flags() & MethodTypeMask >> 2;
    return (QMetaMethod::MethodType)flag;
}

QByteArray QMetaMethod::methodSignature() const
{
    if (!mobj) {
        return QByteArray();
    }
    return QMetaMethodPrivate::get(this)->signature();
}

QByteArray QMetaMethod::name() const
{
    if (!mobj) {
        return QByteArray();
    }
    return QMetaMethodPrivate::get(this)->name();
}

int QMetaMethod::returnType() const
{
    return returnMetaType().id();
}

QMetaType QMetaMethod::returnMetaType() const
{
    if (!mobj || methodType() == QMetaMethod::Constructor) {
        return QMetaType {};
    }
    auto mt = QMetaType(mobj->d.metaTypes[data.metaTypeOffset()]);
    if (mt.id() == QMetaType::UnknownType) {
        return QMetaType(QMetaMethodPrivate::get(this)->returnType());
    }
    else {
        return mt;
    }
}

int QMetaMethod::parameterCount() const
{
    if (!mobj) {
        return 0;
    }
    return QMetaMethodPrivate::get(this)->parameterCount();
}

int QMetaMethod::parameterType(int index) const
{
    return parameterMetaType(index).id();
}

QMetaType QMetaMethod::parameterMetaType(int index) const
{
    if (!mobj || index < 0) {
        return {};
    }
    auto priv = QMetaMethodPrivate::get(this);
    if (index >= priv->parameterCount()) {
        return {};
    }
    //constructor没有返回参数
    auto parameterOffset = index + (methodType() == QMetaMethod::Constructor ? 0 : 1);
    auto mt = QMetaType(mobj->d.metaTypes[data.metaTypeOffset() + parameterOffset]);
    if (mt.id() == QMetaType::UnknownType) {
        return QMetaType(QMetaMethodPrivate::get(this)->parameterType(index));
    }
    else {
        return mt;
    }
}

QMetaMethod QMetaMethod::fromRelativeMethodIndex(const QMetaObject *mobj, int index)
{
    Q_ASSERT(index >= 0 && index < priv(mobj->d.data)->methodCount);
    QMetaMethod m;
    m.mobj = mobj;
    m.data = { mobj->d.data + priv(mobj->d.data)->methodData + index * Data::Size };
    return m;
}

QMetaMethod QMetaMethod::fromRelativeConstructorIndex(const QMetaObject *mobj, int index)
{
    Q_ASSERT(index >= 0 && index < priv(mobj->d.data)->constructorCount);
    QMetaMethod m;
    m.mobj = mobj;
    m.data = { mobj->d.data + priv(mobj->d.data)->constructorData + index * Data::Size };
    return m;
}

int QMetaMethod::relativeMethodIndex() const
{
    if (!mobj) {
        return -1;
    }
    return QMetaMethodPrivate::get(this)->ownMethodIndex();
}

int QMetaMethod::methodIndex() const
{
    if (!mobj) {
        return -1;
    }
    return QMetaMethodPrivate::get(this)->ownMethodIndex() + mobj->methodOffset();
}

int QMetaMethod::attributes() const
{
    if (!mobj) {
        return false;
    }
    return data.flags() >> 4;
}

QByteArray QMetaMethodPrivate::signature() const
{
    QByteArray result;
    result.reserve(256);
    result += name();
    result += '(';
    QList<QByteArray> argTypes = parameterTypes();
    for (int i = 0; i < argTypes.size(); ++i) {
        if (i) {
            result += ',';
        }
        result += argTypes.at(i);
    }
    result += ')';
    return result;
}

int QMetaMethodPrivate::returnType() const
{
    return parameterType(-1);
}

int QMetaMethodPrivate::parameterType(int index) const
{
    return typeFromTypeInfo(mobj, parameterTypeInfo(index));
}

int QMetaMethodPrivate::parameterCount() const
{
    return data.argc();
}

QByteArray QMetaMethodPrivate::name() const
{
    return stringData(mobj, data.name());
}

int QMetaMethodPrivate::typesDataIndex() const
{
    return data.parameters();
}

int QMetaMethodPrivate::parametersDataIndex() const
{
    //参数索引
    //typesDataIndex是返回值的索引位置，后面紧跟参数索引位置
    return typesDataIndex() + 1;
}

QByteArray QMetaMethodPrivate::parameterTypeName(int index) const
{
    int paramsIndex = parametersDataIndex();
    return typeNameFromTypeInfo(mobj, mobj->d.data[paramsIndex + index]);
}

QList<QByteArray> QMetaMethodPrivate::parameterTypes() const
{
    int argc = parameterCount();
    QList<QByteArray> list;
    list.reserve(argc);
    int paramsIndex = parametersDataIndex();
    for (int i = 0; i < argc; ++i) {
        //paramsIndex+i 获取参数信息的索引
        //typeNameFromTypeInfo函数获取索引位置的类型名称
        list += typeNameFromTypeInfo(mobj, mobj->d.data[paramsIndex + i]);
    }
    return list;
}

int QMetaMethodPrivate::ownMethodIndex() const
{
    //通过地址来计算
    //mobj->d.data + priv(mobj->d.data)->methodData 为 method的起始地址
    return (data.d - (mobj->d.data + priv(mobj->d.data)->methodData)) / Data::Size;
}

inline void QMetaMethodPrivate::checkMethodMetaTypeConsistency(const QtPrivate::QMetaTypeInterface *iface, int index) const
{
    uint typeInfo = parameterTypeInfo(index);
    QMetaType mt(iface);
    if (iface) {
        if ((typeInfo & IsUnresolvedType) == 0) {
            Q_ASSERT(mt.id() == int(typeInfo & TypeNameIndexMask));
        }
        Q_ASSERT(mt.name());
    } else {
        // The iface can only be null for a parameter if that parameter is a
        // const-ref to a forward-declared type. Since primitive types are
        // never incomplete, we can assert it's not one of them.

#define ASSERT_NOT_PRIMITIVE_TYPE(TYPE, METATYPEID, NAME)           \
        Q_ASSERT(typeInfo != QMetaType::TYPE);
        QT_FOR_EACH_STATIC_PRIMITIVE_NON_VOID_TYPE(ASSERT_NOT_PRIMITIVE_TYPE)
#undef ASSERT_NOT_PRIMITIVE_TYPE

        Q_ASSERT(typeInfo != QMetaType::QObjectStar);
        // Prior to Qt 6.4 we failed to record void and void*
        if (priv(mobj->d.data)->revision >= 11) {
            Q_ASSERT(typeInfo != QMetaType::Void);
            Q_ASSERT(typeInfo != QMetaType::VoidStar);
        }
    }
}

const QtPrivate::QMetaTypeInterface *QMetaMethodPrivate::returnMetaTypeInterface() const
{
    Q_ASSERT(priv(mobj->d.data)->revision >= 7);
    if (methodType() == QMetaMethod::Constructor)
        return nullptr;         // constructors don't have return types

    const QtPrivate::QMetaTypeInterface *iface =  mobj->d.metaTypes[data.metaTypeOffset()];
    checkMethodMetaTypeConsistency(iface, -1);
    return iface;
}

const QtPrivate::QMetaTypeInterface * const *QMetaMethodPrivate::parameterMetaTypeInterfaces() const
{
    Q_ASSERT(priv(mobj->d.data)->revision >= 7);
    int offset = (methodType() == QMetaMethod::Constructor ? 0 : 1);
    const auto ifaces = &mobj->d.metaTypes[data.metaTypeOffset() + offset];

    for (int i = 0; i < parameterCount(); ++i)
        checkMethodMetaTypeConsistency(ifaces[i], i);

    return ifaces;
}

uint QMetaMethodPrivate::parameterTypeInfo(int index) const
{
    Q_ASSERT(priv(mobj->d.data)->revision >= 7);
    return mobj->d.data[parametersDataIndex() + index];
}

const char *QMetaProperty::name() const
{
    if (!mobj) {
        return nullptr;
    }
    return rawStringData(mobj, data.name());
}

QMetaType QMetaProperty::metaType() const
{
    if (!mobj) {
        return {};
    }
    return QMetaType(mobj->d.metaTypes[data.index(mobj)]);
}

int QMetaProperty::propertyIndex() const
{
    if (!mobj) {
        return -1;
    }
    return data.index(mobj) + mobj->propertyOffset();
}

#define GET_PROPERTY_ABLE_FUNC(flag) \
    if (!mobj) {            \
        return false;       \
    }                       \
    return data.flags() & flag;\


bool QMetaProperty::isWritable() const
{
    GET_PROPERTY_ABLE_FUNC(Writable)
}

bool QMetaProperty::isResettable() const
{
    GET_PROPERTY_ABLE_FUNC(Resettable)
}

bool QMetaProperty::isBindable() const
{
    GET_PROPERTY_ABLE_FUNC(Bindable)
}

bool QMetaProperty::isEnumType() const
{
    if (!mobj) {
        return false;
    }
    return (data.flags() & EnumOrFlag) && menum.name();
}

const char *QMetaEnum::name() const
{
    if (!mobj) {
        return nullptr;
    }
    return rawStringData(mobj, data.name());
}

const char *QMetaEnum::enumName() const
{
    if (!mobj) {
        return nullptr;
    }
    return rawStringData(mobj, data.alias());
}

QMetaType QMetaEnum::metaType() const
{
    if (!mobj) {
        return {};
    }

    const QMetaObjectPrivate *p = priv(mobj->d.data);
    return QMetaType(mobj->d.metaTypes[data.index(mobj) + p->propertyCount]);
}

int QMetaEnum::keyCount() const
{
    if (!mobj) {
        return 0;
    }
    return data.keyCount();
}

const char *QMetaEnum::key(int index) const
{
    if (!mobj) {
        return nullptr;
    }
    if (index >= 0 && index < int(data.keyCount())) {
        return rawStringData(mobj, mobj->d.data[data.data() + 2 * index]);
    }
    return nullptr;
}

int QMetaEnum::value(int index) const
{
    if (!mobj) {
        return 0;
    }
    if (index >= 0 && index < int(data.keyCount())) {
        return mobj->d.data[data.data() + 2 * index + 1];
    }
    return -1;
}

bool QMetaEnum::isFlag() const
{
    if (!mobj) {
        return false;
    }
    return data.flags() & EnumIsFlag;
}

//使用enum class注册的枚举(作用域枚举)
bool QMetaEnum::isScoped() const
{
    if (!mobj) {
        return false;
    }
    return data.flags() & EnumIsScoped;
}

const char *QMetaEnum::scope() const
{
    return mobj ? objectClassName(mobj) : nullptr;
}

int QMetaEnum::keyToValue(const char *key, bool *ok) const
{
    if (ok != nullptr) {
        *ok = false;
    }
    if (!mobj || !key) {
        return -1;
    }
    uint scope = 0;
    const char *qualified_key = key;
    const char *s = key + qstrlen(key);
    while (s > key && *s != ':') {
        --s;
    }
    if (s > key && *(s - 1) == ':') {
        scope = s - key - 1;
        key += scope + 2;
    }
    for (int i = 0; i < int (data.keyCount()); ++i) {
        const QByteArray className = stringData(mobj, priv(mobj->d.data)->className);
        if ((!scope || (className.size() == int(scope) && strncmp(qualified_key, className.constData(), scope) == 0))
            && strcmp(key, rawStringData(mobj, mobj->d.data[data.data() + 2*i])) == 0) {
            if (ok != nullptr)
                *ok = true;
            return mobj->d.data[data.data() + 2 * i + 1];
        }
    }
    return -1;
}

const char *QMetaEnum::valueToKey(int value) const
{
    if (!mobj) {
        return nullptr;
    }
    for (int i = 0; i < int(data.keyCount()); ++i) {
        if (value == (int)mobj->d.data[data.data() + 2 * i + 1]) {
            return rawStringData(mobj, mobj->d.data[data.data() + 2 * i]);
        }
    }
    return nullptr;
}

static auto parse_scope(QLatin1StringView qualifiedKey) noexcept
{
    struct R {
        std::optional<QLatin1StringView> scope;
        QLatin1StringView key;
    };
    const auto scopePos = qualifiedKey.lastIndexOf("::"_L1);
    if (scopePos < 0)
        return R{std::nullopt, qualifiedKey};
    else
        return R{qualifiedKey.first(scopePos), qualifiedKey.sliced(scopePos + 2)};
}

int QMetaEnum::keysToValue(const char *keys, bool *ok)
{
    Q_ASSERT(false);
    return 0;
}

namespace
{
    template <typename String, typename Container, typename Separator>
    void join_reversed(String &s, const Container &c, Separator sep)
    {
        if (c.empty()) {
            return;
        }
        qsizetype len = qsizetype(c.size()) - 1; // N - 1 separators
        for (auto &e : c) {
            len += qsizetype(e.size()); // N parts
        }
        s.reserve(len);
        bool first = true;
        for (auto rit = c.rbegin(), rend = c.rend(); rit != rend; ++rit) {
            const auto &e = *rit;
            if (!first) {
                s.append(sep);
            }
            first = false;
            s.append(e.data(), e.size());
        }
    }
} // unnamed namespace

QByteArray QMetaEnum::valueToKeys(int value) const
{
    QByteArray keys;
    if (!mobj) {
        return keys;
    }
    QVarLengthArray<QLatin1StringView, sizeof(int) * CHAR_BIT> parts;
    int v = value;
    for (int i = data.keyCount() - 1; i >= 0; --i) {
        int k = mobj->d.data[data.data() + 2 * i + 1];
        if ((k != 0 && (v & k) == k) || (k == value)) {
            v = v & ~k;
            parts.push_back(stringDataView(mobj, mobj->d.data[data.data() + 2 * i]));
        }
    }
    join_reversed(keys, parts, '|');;
    return keys;
}

QMetaEnum::QMetaEnum(const QMetaObject *mobj, int index)
    : mobj(mobj), data({ mobj->d.data + priv(mobj->d.data)->enumeratorData + index * Data::Size })
{
    Q_ASSERT(index >= 0 && index < priv(mobj->d.data)->enumeratorCount);
}

int QMetaEnum::Data::index(const QMetaObject *mobj) const
{
    return (d - mobj->d.data - priv(mobj->d.data)->enumeratorData) / Size;
}

#undef GET_PROPERTY_ABLE_FUNC

QT_END_NAMESPACE
