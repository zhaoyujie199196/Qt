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
#include "qobjectdefs.h"
#include "qmetaobject_p.h"
#include "qobject_p.h"

QT_BEGIN_NAMESPACE

static inline const QMetaObjectPrivate *priv(const uint *data)
{
    return reinterpret_cast<const QMetaObjectPrivate *>(data);
}

static inline QLatin1String stringDataView(const QMetaObject *mo, int index)
{
    Q_ASSERT(priv(mo->d.data)->revision >= 7);
    //前面存储了索引与长度，后面存储了字符串的真实值
    uint offset = mo->d.stringdata[2 * index];
    uint length = mo->d.stringdata[2 * index + 1];
    const char *string = reinterpret_cast<const char *>(mo->d.stringdata) + offset;
    return QLatin1String(string, length);
}

static inline QByteArray stringData(const QMetaObject *mo, int index)
{
    const auto view = stringDataView(mo, index);
    return QByteArray::fromRawData(view.data(), view.size());
}

//根据index，获取到QMetaObject中的字符串信息
static inline const char *rawStringData(const QMetaObject *mo, int index)
{
    Q_ASSERT(priv(mo->d.data)->revision >= 7);
    //一条信息占两位：开始位置和长度
    uint offset = mo->d.stringdata[2 * index];
    return reinterpret_cast<const char *>(mo->d.stringdata) + offset;
}

static inline int typeFromTypeInfo(const QMetaObject *mo, uint typeInfo)
{
    if (!(typeInfo & IsUnresolvedType)) {
        return typeInfo;
    }
    return QMetaType::fromName(rawStringData(mo, typeInfo & TypeNameIndexMask)).id();
}

static inline QByteArray typeNameFromTypeInfo(const QMetaObject *mo, uint typeInfo)
{
    if (typeInfo & IsUnresolvedType) {
        return stringData(mo, typeInfo & TypeNameIndexMask);
    }
    else {
        return QMetaType(typeInfo).name();
    }
}

static QByteArray normalizeTypeInternal(const char *t, const char *e)
{
    int len = QtPrivate::qNormalizeType(t, e, nullptr);
    if (len == 0) {
        return QByteArray();
    }
    QByteArray result(len, Qt::Uninitialized);
    len = QtPrivate::qNormalizeType(t, e, result.data());
    Q_ASSERT(len == result.size());
    return result;
}

//zhaoyujie TODO 这里还需要看看
static void qRemoveWhitespace(const char *s, char *d)
{
    char last = 0;
    while (*s && is_space(*s)) {
        s++;
    }
    while (*s) {
        while (*s && !is_space(*s)) {
            last = *s;
            *d++ = *s++;
        }
        while (*s && is_space(*s)) {
            s++;
        }
        if (*s && ((is_ident_char(*s) && is_ident_char(last))
                   || ((*s == ':') && (last == '<')))) {
            last = ' ';
            *d++ = ' ';
        }
    }
    *d = '\0';
}

static char *qNormalizeType(char *d, int &templdepth, QByteArray &result)
{
    const char *t = d;
    while (*d && (templdepth || (*d != ',' && *d != ')'))) {
        if (*d == '<') {
            ++templdepth;
        }
        if (*d == '>') {
            --templdepth;
        }
        ++d;
    }
    if (strncmp("void)", t, d - t + 1) != 0) {
        result += normalizeTypeInternal(t, d);
    }
    else {
        Q_ASSERT(false);
    }
    return d;
}

class QMetaMethodPrivate : public QMetaMethod
{
public:
    static const QMetaMethodPrivate *get(const QMetaMethod *q)
    {
        return static_cast<const QMetaMethodPrivate *>(q);
    }

    inline QByteArray name() const;
    inline QByteArray signature() const;
    inline const QMetaObject *enclosingMetaObject() const { return mobj; }
    inline int typesDataIndex() const;  //参数数据类型的索引
    inline int parameterTypeInfo(int index) const;
    inline int parametersDataIndex() const;
    inline int ownMethodIndex() const;
    inline QByteArray tag() const;
    inline QList<QByteArray> parameterTypes() const;
};

QByteArray QMetaMethodPrivate::name() const
{
    return stringData(mobj, data.name());
}

QByteArray QMetaMethodPrivate::signature() const
{
    QByteArray result;
    result.reserve(256);
    result += name();
    result += '(';
    QList<QByteArray> argTypes = parameterTypes();
    for (auto i = 0; i < argTypes.size(); ++i) {
        if (i) {
            result += ',';
        }
        result += argTypes.at(i);
    }
    result += ')';
    return result;
}

int QMetaMethodPrivate::typesDataIndex() const
{
    return data.parameters();
}

int QMetaMethodPrivate::parameterTypeInfo(int index) const
{
    return mobj->d.data[parametersDataIndex() + index];
}

int QMetaMethodPrivate::parametersDataIndex() const
{
    return typesDataIndex() + 1;
}

int QMetaMethodPrivate::ownMethodIndex() const
{
    return (data.d - mobj->d.data - priv(mobj->d.data)->methodData) / Data::Size;
}

QByteArray QMetaMethodPrivate::tag() const
{
    Q_ASSERT(priv(mobj->d.data)->revision > 7);
    return stringData(mobj, data.tag());
}

QList<QByteArray> QMetaMethodPrivate::parameterTypes() const
{
    int argc = parameterCount();
    QList<QByteArray> list;
    list.reserve(argc);
    int paramsIndex = parametersDataIndex();
    for (int i = 0; i < argc; ++i) {
        list += typeNameFromTypeInfo(mobj, mobj->d.data[paramsIndex + i]);
    }
    return list;
}

QMetaProperty::QMetaProperty(const QMetaObject *mobj, int index)
    : mobj(mobj)
    , data(getMetaPropertyData(mobj, index))
{
    Q_ASSERT(index >= 0 && index < priv(mobj->d.data)->propertyCount);
    if (data.flags() & EnumOrFlag) {
        //zhaoyujie TODO 没明白这段代码什么意思
        Q_ASSERT(false);
    }
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

int QMetaProperty::relativePropertyIndex() const
{
    if (!mobj) {
        return -1;
    }
    return data.index(mobj);
}

QUntypedBindable QMetaProperty::bindable(QObject *object) const
{
    QUntypedBindable bindable;
    void *argv[1] { &bindable };
    mobj->metacall(object, QMetaObject::BindableProperty, data.index(mobj) + mobj->propertyOffset(), argv);
    return bindable;
}

QMetaProperty::Data QMetaProperty::getMetaPropertyData(const QMetaObject *mobj, int index)
{
    //mobj->d.data： meta_object信息
    //propertyData: property开始的地方
    //index * Data::Size： 在property信息中的偏移
    return { mobj->d.data + priv(mobj->d.data)->propertyData + index * Data::Size };
}

int QMetaProperty::Data::index(const QMetaObject *mobj) const
{
    return (d - mobj->d.data - priv(mobj->d.data)->propertyData) / Size;
}

bool QMetaProperty::isReadable() const
{
    if (!mobj) {
        return false;
    }
    return data.flags() & Readable;
}

bool QMetaProperty::isWriteable() const
{
    if (!mobj) {
        return false;
    }
    return data.flags() & Writable;
}

/*
 * 读取object的属性，调用到moc文件的static_metacall方法
 * 返回值作为argv的第一个参数
 * */
QVariant QMetaProperty::read(const QObject *obj) const
{
    if (!obj || !mobj) {
        return QVariant();
    }
    int status = -1;
    QVariant value;
    void *argv[] = { nullptr, &value, &status };
    QMetaType t(mobj->d.metaTypes[data.index(mobj)]);
    if (t == QMetaType::fromType<QVariant>()) {  //返回值是QVariant
        argv[0] = &value;
    }
    else {
        value = QVariant(t, nullptr);
        argv[0] = value.data();
    }
    if (priv(mobj->d.data)->flags & PropertyAccessInStaticMetaCall && mobj->d.static_metacall) {
        Q_ASSERT(false);
//        mobj->d.static_metacall(const_cast<QObject *>(object), QMetaObject::ReadProperty, data.index(mobj), argv);
    }
    else {
        QMetaObject::metacall(const_cast<QObject *>(obj), QMetaObject::ReadProperty, data.index(mobj) + mobj->propertyOffset(), argv);
    }

    if (status != -1) {
        return value;
    }
    if (t != QMetaType::fromType<QVariant>() && argv[0] != value.data()) {
        return QVariant(t, argv[0]);
    }
    return value;
}

bool QMetaProperty::write(QObject *obj, const QVariant &value) const
{
    if (!obj || !isWriteable()) {
        return false;
    }
    QVariant v = value;
    QMetaType t(mobj->d.metaTypes[data.index(mobj)]);
    if (t != QMetaType::fromType<QVariant>() && t != v.metaType()) {
        //zhaoyujie TODO
        Q_ASSERT(false);
    }
    int status = -1;
    int flags = 0;
    void *argv[] = { nullptr, &v, &status, &flags };
    if (t == QMetaType::fromType<QVariant>()) {
        argv[0] = &v;
    }
    else {
        argv[0] = v.data();
    }
    if (priv(mobj->d.data)->flags & PropertyAccessInStaticMetaCall && mobj->d.static_metacall) {
        Q_ASSERT(false);  //zhaoyujie TODO 上面的判断什么意思？什么时候能够走到？
    }
    else {
        QMetaObject::metacall(obj, QMetaObject::WriteProperty, data.index(mobj) + mobj->propertyOffset(), argv);
    }
    return status;
}

//从字符串中提取参数类型
static void argumentTypesFromString(const char *str, const char *end, QArgumentTypeArray &types)
{
    Q_ASSERT(str <= end);
    while (str != end) {
        if (!types.isEmpty()) {
            ++str;  //跳过逗号分隔符
        }
        const char *begin = str;
        int level = 0; //QMap<int, int>之类的层级
        while (str != end && (level > 0 || *str != ',')) {  //查找到逗号，提取中间的内容
            if (*str == '<') {
                ++level;
            }
            else if (*str == '>') {
                --level;
            }
            ++str;
        }
        QByteArray argType(begin, str - begin);
        argType.replace("QVector<", "QList<");
        types += QArgumentType(std::move(argType));
    }
}

/*
 * 解析如sig(const QString &, bool, int) 形式的信号槽写法
 * 方法名作为返回值，参数类型解析到types中
 * */
QByteArray QMetaObjectPrivate::decodeMethodSignature(const char *signature, QArgumentTypeArray &types)
{
    Q_ASSERT(signature != nullptr);
    const char *lparens = strchr(signature, '(');
    if (!lparens) {  //没有查找到()，返回
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

int QMetaObjectPrivate::indexOfMethod(const QMetaObject *m, const QByteArray &name, int argc,
                                      const QArgumentType *types) {
    int i = indexOfMethodRelative<0>(&m, name, argc, types);
    if (i >= 0) {
        i += m->methodOffset();
    }
    return i;
}

int QMetaObjectPrivate::indexOfSignalRelative(const QMetaObject **baseObject, const QByteArray &name, int argc,
                                              const QArgumentType *types)
{
    int i = indexOfMethodRelative<MethodSignal>(baseObject, name, argc, types);
    const QMetaObject *m = *baseObject;
    if (i >= 0 && m && m->d.superdata) {
        int conflict = indexOfMethod(m->d.superdata, name, argc, types);
        if (conflict >= 0) {
            //子类方法与父类方法冲突了
            Q_ASSERT(false);
        }
    }
    return i;
}

int QMetaObjectPrivate::indexOfSlotRelative(const QMetaObject **baseObject, const QByteArray &name, int argc,
                                            const QArgumentType *types) {
    return indexOfMethodRelative<MethodSlot>(baseObject, name, argc, types);
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

template <int MethodType>
inline int QMetaObjectPrivate::indexOfMethodRelative(const QMetaObject **baseObject, const QByteArray &name, int argc, const QArgumentType *types)
{
    for (const QMetaObject *m = *baseObject; m; m = m->d.superdata) {
        Q_ASSERT(priv(m->d.data)->revision >= 7);
        //查找signal，只查找信号，查找slot，会查找slot和invoke
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

bool QMetaObjectPrivate::methodMatch(const QMetaObject *m, const QMetaMethod &method, const QByteArray &name, int argc,
                                     const QArgumentType *types) {
    const auto &data = method.data;
    if (data.argc() != uint(argc)) {
        return false;
    }
    //zhaoyujie TODO stringdata中的数据和索引是怎么对应上的？
    if (stringData(m, data.name()) != name) {  //函数名称不同
        return false;
    }
    int paramsIndex = data.parameters() + 1;  //跳过returnType
    for (int i = 0; i < argc; ++i) {
        uint typeInfo = m->d.data[paramsIndex + i];
        if (types[i].type()) {  //如果有tyoeid类型，使用typeid比较
            if (types[i].type() != typeFromTypeInfo(m, typeInfo)) {
                return false;
            }
        }
        else {  //如果有typename，使用typename比较
            if (types[i].name() != typeNameFromTypeInfo(m, typeInfo)) {
                return false;
            }
        }
    }
    return true;
}

int QMetaObjectPrivate::originalClone(const QMetaObject *mobj, int local_method_index)
{
#pragma message("这个方法什么意思....")
    Q_ASSERT(local_method_index < get(mobj)->methodCount);
    while (QMetaMethod::fromRelativeMethodIndex(mobj, local_method_index).data.flags() & MethodCloned) {
        Q_ASSERT(local_method_index > 0);
        --local_method_index;
    }
    return local_method_index;
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

//检测连接参数是否匹配，信号的个数要不少于槽的个数，多余的参数会被截断丢弃
bool QMetaObjectPrivate::checkConnectArgs(int signalArgc, const QArgumentType *signalTypes, int methodArgc,
                                          const QArgumentType *methodTypes) {
    if (signalArgc < methodArgc) {  //信号的参数数量要>=槽的参数数量
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
    if (signal->parameterCount() < method->parameterCount()) {
        return false;
    }
    const QMetaObject *smeta = signal->enclosingMetaObject();
    const QMetaObject *rmeta = method->enclosingMetaObject();
    for (int i = 0; i < method->parameterCount(); ++i) {
        uint sourceTypeInfo = signal->parameterTypeInfo(i);
        uint targetTypeInfo = signal->parameterTypeInfo(i);
        //0x80000000 | 9 类型的，通过名字判断  zhaoyujie TODO 直接通过sourceTypeInfo判断不行吗?这里为什么要绕一个圈？
        Q_ASSERT(false);
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

static void computeOffsets(const QMetaObject *metaobject, int *signalOffset, int *methodOffset)
{
    *signalOffset = *methodOffset = 0;
    const QMetaObject *m = metaobject->d.superdata;
    while (m) {
        const QMetaObjectPrivate *d = QMetaObjectPrivate::get(m);
        *methodOffset += d->methodCount;
        *signalOffset += d->signalCount;
        m = m->d.superdata;
    }
}

void QMetaObjectPrivate::memberIndexes(const QObject *obj, const QMetaMethod &member, int *signalIndex,
                                       int *methodIndex) {
    *signalIndex = -1;
    *methodIndex = -1;
    if (!obj || !member.mobj) {
        return;
    }
    const QMetaObject *m = obj->metaObject();
    while (m != nullptr & m != member.mobj) {
        m = m->d.superdata;
    }
    if (!m) {  //method里放的元对象与obj的元对象不匹配
        return;
    }
    //method在这个object中的索引
    *signalIndex = *methodIndex = member.relativeMethodIndex();

    int signalOffset;
    int methodOffset;
    computeOffsets(m, &signalOffset, &methodOffset);

    *methodIndex += methodOffset;
    if (member.methodType() == QMetaMethod::Signal) {
        *signalIndex = originalClone(m, *signalIndex);
        *signalIndex += signalOffset;
    }
    else {
        *signalIndex = -1;
    }
}

static inline const char *objectClassName(const QMetaObject *m)
{
    return rawStringData(m, priv(m->d.data)->className);
}

const char *QMetaObject::className() const
{
    return objectClassName(this);
}

const QMetaObject *QMetaObject::superClass() const
{
    return d.superdata;
}

bool QMetaObject::inherits(const QMetaObject *metaObject) const noexcept
{
    const QMetaObject *m = this;
    do {
        if (metaObject == m) {
            return true;
        }
    } while ( (m = m->d.superdata) );
    return false;
}

const QObject *QMetaObject::cast(const QObject *obj) const
{
    return (obj && obj->metaObject()->inherits(this)) ? obj : nullptr;
}

QByteArray QMetaObject::normalizedType(const char *type)
{
    Q_ASSERT(false);  //zhaoyujie TODO
    return normalizeTypeInternal(type, type + qstrlen(type));
}

int QMetaObject::propertyCount() const
{
    int n = priv(d.data)->propertyCount;
    const QMetaObject *m = d.superdata.direct;
    while (m) {  //循环添加父的propertyCount
        n += priv(m->d.data)->propertyCount;
        m = m->d.superdata;
    }
    return n;
}

int QMetaObject::methodCount() const
{
    int n = priv(d.data)->methodCount;
    const QMetaObject *m = d.superdata;
    while (m) {
        n += priv(m->d.data)->methodCount;
        m = m->d.superdata;
    }
    return n;
}

int QMetaObject::methodOffset() const
{
    int offset = 0;
    const QMetaObject *m = d.superdata;
    while (m) {
        offset += priv(m->d.data)->methodCount;
        m = m->d.superdata;
    }
    return offset;
}

int QMetaObject::propertyOffset() const
{
    int offset = 0;
    const QMetaObject *m = d.superdata;
    while (m) {
        offset += priv(m->d.data)->propertyCount;
        m = m->d.superdata;
    }
    return offset;
}

int QMetaObject::indexOfProperty(const char *name) const
{
    const QMetaObject *m = this;
    while (m) {
        const QMetaObjectPrivate *d = priv(m->d.data);
        for (int i = 0; i < d->propertyCount; ++i) {
            const QMetaProperty::Data data = QMetaProperty::getMetaPropertyData(m, i);
            const char *prop = rawStringData(m, data.name());
            //zhaoyujie TODO 这里的比较为什么先比较0，再从1开始？不是直接比较就可以了吗？
            if (name[0] == prop[0] && strcmp(name + 1, prop + 1) == 0) {
                i += m->propertyOffset();
                return i;
            }
        }
        m = m->d.superdata;
    }
    Q_ASSERT(false);
    return -1;
}

QMetaMethod QMetaObject::method(int index) const
{
    int i = index;
    i -= methodOffset();
    if (i < 0 && d.superdata) {
        return d.superdata->method(index);
    }
    if (i >= 0 && i < priv(d.data)->methodCount) {
        return QMetaMethod::fromRelativeMethodIndex(this, i);
    }
    return QMetaMethod();
}

QMetaProperty QMetaObject::property(int index) const
{
    int i = index;
    i -= propertyOffset();
    if (i < 0 && d.superdata) {  //索引小于0，说明是父类的属性
        return d.superdata->property(index);
    }
    if (i >= 0 && i < priv(d.data)->propertyCount) {
        return QMetaProperty(this, i);
    }
    return QMetaProperty();
}

int QMetaObject::static_metacall(Call cl, int idx, void **argv) const
{
    Q_ASSERT(priv(d.data)->revision >= 6);
    if (!d.static_metacall) {
        return 0;
    }
    d.static_metacall(nullptr, cl, idx, argv);
    return -1;
}

int QMetaObject::metacall(QObject *object, Call cl, int idx, void **argv)
{
    if (object->d_ptr->metaObject) {
        return object->d_ptr->metaObject->metaCall(object, cl, idx, argv);
    }
    else {
        return object->qt_metacall(cl, idx, argv);
    }
}

QByteArray QMetaObject::normalizedSignature(const char *method)
{
    QByteArray result;
    if (!method || !*method) {
        return result;
    }
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

void QMetaObject::connectSlotsByName(QObject *o) {
    if (!o) {
        return;
    }
    const QMetaObject *mo = o->metaObject();
    Q_ASSERT(mo);

    QObjectList list = o->findChildren<QObject *>(QString());
    list << o;

    for (int i = 0; i < mo->methodCount(); ++i) {
        const QByteArray slotSignature = mo->method(i).methodSignature();
        const char *slot = slotSignature.constData();
        Q_ASSERT(slot);

        //必须以on_开头
        if (slot[0] != 'o' || slot[1] != 'n' || slot[2] != '_') {
            continue;
        }

        bool foundIt = false;
        for (int j = 0; j < list.count(); ++j) {
            const QObject *co = list.at(j);
            const QByteArray coName = co->objectName().toLatin1();
            //receiver的objectName必须符合"on_<objectName>_<signal>"的格式
            if (coName.isEmpty() || qstrncmp(slot + 3, coName.constData(), coName.size()) || slot[coName.size() + 3] != '_') {
                continue;
            }

            const char *signal = slot + coName.size() + 4;

            //符合on_<objectName>_<signal>格式
            const QMetaObject *smeta;
            int sigIndex = co->d_func()->signalIndex(signal, &smeta);
            if (sigIndex < 0) {
                QList<QByteArray> compatibleSignals;
                const QMetaObject *smo = co->metaObject();
                int sigLen = int(qstrlen(signal)) - 1;
                for (int k = QMetaObjectPrivate::absoluteSignalCount(smo) - 1; k >= 0; --k) {
                    const QMetaMethod method = QMetaObjectPrivate::signal(smo, k);
                    if (!qstrncmp(method.methodSignature().constData(), signal, sigLen)) {
                        smeta = method.enclosingMetaObject();
                        sigIndex = k;
                        compatibleSignals.prepend(method.methodSignature());
                    }
                }
                if (compatibleSignals.size() > 1) {
//                    qCWarning(lcConnectSlotsByName) << "QMetaObject::connectSlotsByName: Connecting slot" << slot
//                                                                                                          << "with the first of the following compatible signals:" << compatibleSignals;
                }
            }
            if (sigIndex < 0) {
                continue;
            }

            if (Connection(QMetaObjectPrivate::connect(co, sigIndex, smeta, o, i))) {
                foundIt = true;
                break;
            }
        }
        if (foundIt) {
            while (mo->method(i + 1).attributes() & QMetaMethod::Cloned) {
                ++i;
            }
        }
        else if (!(mo->method(i).attributes() & QMetaMethod::Cloned)) {
            int iParen = slotSignature.indexOf('(');
            int iLastUnderscore = slotSignature.lastIndexOf('_', iParen - 1);
            if (iLastUnderscore > 3) {
                qCWarning(lcConnectSlotsByName,
                          "QMetaObject::connectSlotsByName: No matching signal for %s", slot)
            }
        }
    }
}

bool QMetaObject::checkConnectArgs(const char *signal, const char *method)
{
    Q_ASSERT(false);
    const char *s1 = signal;
    const char *s2 = method;
    //跳过函数名，只比较参数
    while (*s1++ != '(') {
    }
    while (*s2++ != '(' ) {
    }

    if (*s2 == ')' || qstrcmp(s1, s2) == 0) {
        return true;
    }
    const auto s1len = qstrlen(s1);
    const auto s2len = qstrlen(s2);
    //signal的参数允许比method多，比较signal参数的个数
    if (s2len < s1len && strncmp(s1, s2, s2len - 1) == 0 && s1[s2len - 1] == ',') {
        return true;
    }
    return false;
}

bool QMetaObject::checkConnectArgs(const QMetaMethod &signal, const QMetaMethod &method)
{
    Q_ASSERT(false);
    return false;
//    return QMetaObjectPrivate::checkConnectArgs(
//                QMetaObjectPrivate::get(&signal),
//                QMetaObjectPrivate::get(&method);
//            );
}

QMetaObject::Connection::Connection(const QMetaObject::Connection &other)
        : d_ptr(other.d_ptr)
{
    if (d_ptr) {
        static_cast<QObjectPrivate::Connection *>(d_ptr)->ref();
    }
}

QMetaObject::Connection &QMetaObject::Connection::operator=(const QMetaObject::Connection &other)
{
    if (other.d_ptr != d_ptr) {
        if (d_ptr) {
            static_cast<QObjectPrivate::Connection *>(d_ptr)->deref();
        }
        d_ptr = other.d_ptr;
        if (d_ptr) {
            static_cast<QObjectPrivate::Connection *>(d_ptr)->ref();
        }
    }
    return *this;
}

QMetaObject::Connection::~Connection()
{
    if (d_ptr) {
        static_cast<QObjectPrivate::Connection *>(d_ptr)->deref();
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

QByteArray QMetaMethod::methodSignature() const
{
    if (!mobj) {
        return QByteArray();
    }
    return QMetaMethodPrivate::get(this)->signature();
}

QMetaMethod::MethodType QMetaMethod::methodType() const
{
    if (!mobj) {
        return QMetaMethod::Method;
    }
    return static_cast<QMetaMethod::MethodType >((data.flags() & MethodTypeMask) >> 2);
}

int QMetaMethod::relativeMethodIndex() const
{
    if (!mobj) {
        return -1;
    }
    return QMetaMethodPrivate::get(this)->ownMethodIndex();
}

int QMetaMethod::parameterCount() const
{
    if (!mobj) {
        return 0;
    }
    return data.argc();
}

int QMetaMethod::methodIndex() const
{
    if (!mobj) {
        return -1;
    }
    return QMetaMethodPrivate::get(this)->ownMethodIndex() + mobj->methodOffset();
}

int QMetaMethod::attributes() const {
    if (!mobj) {
        return false;
    }
    return data.flags() >> 4;
}

QT_END_NAMESPACE
