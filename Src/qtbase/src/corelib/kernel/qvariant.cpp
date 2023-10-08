//
// Created by Yujie Zhao on 2023/9/26.
//
#include "qvariant.h"
#include "qvariant_p.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

static bool qIsNumericType(uint tp)
{
    static const qulonglong numericTypeBits =
            Q_UINT64_C(1) << QMetaType::QString |  //TODO 为什么这里有String类型？
            Q_UINT64_C(1) << QMetaType::Bool |
            Q_UINT64_C(1) << QMetaType::Double |
            Q_UINT64_C(1) << QMetaType::Float |
            Q_UINT64_C(1) << QMetaType::Char |
            Q_UINT64_C(1) << QMetaType::SChar |
            Q_UINT64_C(1) << QMetaType::UChar |
            Q_UINT64_C(1) << QMetaType::Short |
            Q_UINT64_C(1) << QMetaType::UShort |
            Q_UINT64_C(1) << QMetaType::Int |
            Q_UINT64_C(1) << QMetaType::UInt |
            Q_UINT64_C(1) << QMetaType::Long |
            Q_UINT64_C(1) << QMetaType::ULong |
            Q_UINT64_C(1) << QMetaType::LongLong |
            Q_UINT64_C(1) << QMetaType::ULongLong;
    return tp < (CHAR_BIT * sizeof numericTypeBits) ? numericTypeBits & (Q_UINT64_C(1) << tp) : false;
}

static bool qIsFloatingPoint(uint tp)
{
    return tp == QMetaType::Double || tp == QMetaType::Float;
}

static int normalizeLowerRanks(uint tp)
{
    static const qulonglong numericTypeBits =
            Q_UINT64_C(1) << QMetaType::Bool |
            Q_UINT64_C(1) << QMetaType::Char |
            Q_UINT64_C(1) << QMetaType::SChar |
            Q_UINT64_C(1) << QMetaType::UChar |
            Q_UINT64_C(1) << QMetaType::Short |
            Q_UINT64_C(1) << QMetaType::UShort;
    return numericTypeBits & (Q_UINT64_C(1) << tp) ? uint(QMetaType::Int) : tp;
}

static int normalizeLong(uint tp)
{
    const uint IntType = sizeof(long) == sizeof(int) ? QMetaType::Int : QMetaType::LongLong;
    const uint UIntType = sizeof(ulong) == sizeof(uint) ? QMetaType::UInt : QMetaType::ULongLong;
    if (tp == QMetaType::Long) {
        return IntType;
    }
    if (tp == QMetaType::ULong) {
        return UIntType;
    }
    return tp;
}

static qulonglong qMetaTypeUNumber(const QVariant::Private *d)
{
    switch(d->typeId()) {
        case QMetaType::UInt:
            return d->get<unsigned int>();
        case QMetaType::ULongLong:
            return d->get<qulonglong>();
        case QMetaType::UChar:
            return d->get<unsigned char>();
        case QMetaType::UShort:
            return d->get<unsigned short>();
        case QMetaType::ULong:
            return d->get<unsigned long>();
    }
    Q_ASSERT(false);
    return 0;
}

static qlonglong qMetaTypeNumber(const QVariant::Private *d)
{
    switch (d->typeId()) {
        case QMetaType::Int:
            return d->get<int>();
        case QMetaType::LongLong:
            return d->get<qlonglong>();
        case QMetaType::Char:
            return qlonglong(d->get<char>());
        case QMetaType::SChar:
            return qlonglong(d->get<signed char>());
        case QMetaType::Short:
            return qlonglong(d->get<short>());
        case QMetaType::Long:
            return qlonglong(d->get<long>());
        case QMetaType::Float:
            return qRound64(d->get<float>());
        case QMetaType::Double:
            return qRound64(d->get<double>());
//        case QMetaType::QJsonValue:
//        case QMetaType::QCborValue:
    }
    Q_ASSERT(false);
    return 0;
}

static qlonglong qConvertToNumber(const QVariant::Private *d, bool *ok, bool allowStringToBool = false)
{
    *ok = true;
    switch(uint(d->typeId())) {
        case QMetaType::QString: {
            Q_ASSERT(false); //TODO
            return 0;
        }
        case QMetaType::QChar:
            return d->get<QChar>().unicode();
        case QMetaType::QByteArray: {
            Q_ASSERT(false);
            return 0;
//            return d->get<QByteArray>().toLongLong(ok);
        }
        case QMetaType::Bool:
            return qlonglong(d->get<bool>());
//        case QMetaType::QCborValue:
//        case QMetaType::QJsonValue: {
//            Q_ASSERT(false);
//            return 0;
//        }
        case QMetaType::Double:
        case QMetaType::Int:
        case QMetaType::Char:
        case QMetaType::SChar:
        case QMetaType::Short:
        case QMetaType::Long:
        case QMetaType::Float:
        case QMetaType::LongLong:
            return qMetaTypeNumber(d);
        case QMetaType::ULongLong:
        case QMetaType::UInt:
        case QMetaType::UChar:
        case QMetaType::UShort:
        case QMetaType::ULong:
            return qlonglong(qMetaTypeUNumber(d));
    }

    QMetaType typeInfo = d->type();
    if (typeInfo.flags() & QMetaType::IsEnumeration || d->typeId() == QMetaType::QCborSimpleType) {
        switch (typeInfo.sizeOf()) {
            case 1:
                return d->get<signed char>();
            case 2:
                return d->get<short>();
            case 4:
                return d->get<int>();
            case 8:
                return d->get<qlonglong>();
        }
    }
    *ok = false;
    return Q_INT64_C(0);
}

static qreal qConvertToRealNumber(const QVariant::Private *d, bool *ok)
{
    *ok = true;
    switch(uint(d->typeId())) {
        case QMetaType::QString:
            Q_ASSERT(false);
            return 0;
        case QMetaType::Double:
            return qreal(d->get<double>());
        case QMetaType::Float:
            return qreal(d->get<float>());
        case QMetaType::ULongLong:
        case QMetaType::UInt:
        case QMetaType::UChar:
        case QMetaType::UShort:
        case QMetaType::ULong:
            return qreal(qMetaTypeUNumber(d));
//        case QMetaType::QCborValue:
//            return d->get<QCborValue>().toDouble();
//        case QMetaType::QJsonValue:
//            return d->get<QJsonValue>().toDouble();
        default:
            return qreal(qConvertToNumber(d, ok));
    }
}

static int numericTypePromotion(uint t1, uint t2)
{
    Q_ASSERT(qIsNumericType(t1));
    Q_ASSERT(qIsNumericType(t2));

    if ((t1 == QMetaType::Bool && t2 == QMetaType::QString) ||
        (t2 == QMetaType::Bool && t1 == QMetaType::QString)) {
        return QMetaType::Bool;
    }
    if (qIsFloatingPoint(t1) || qIsFloatingPoint(t2)) {
        return QMetaType::QReal;
    }
    t1 = normalizeLowerRanks(t1);
    t2 = normalizeLowerRanks(t2);

    t1 = normalizeLong(t1);
    t2 = normalizeLong(t2);

    if (t1 == QMetaType::ULongLong || t2 == QMetaType::ULongLong) {
        return QMetaType::ULongLong;
    }
    if (t1 == QMetaType::LongLong || t2 == QMetaType::LongLong) {
        return QMetaType::LongLong;
    }
    if (t1 == QMetaType::UInt || t2 == QMetaType::UInt) {
        return QMetaType::UInt;
    }
    return QMetaType::Int;
}

static bool integralEquals(uint promotedType, const QVariant::Private *d1, const QVariant::Private *d2)
{
    bool ok;
    qlonglong l1 = qConvertToNumber(d1, &ok, promotedType == QMetaType::Bool);
    if (!ok) {
        return false;
    }
    qlonglong l2 = qConvertToNumber(d2, &ok, promotedType == QMetaType::Bool);
    if (!ok) {
        return false;
    }
    if (promotedType == QMetaType::Bool) {
        return bool(l1) == bool(l2);
    }
    if (promotedType == QMetaType::Int) {
        return int(l1) == int(l2);
    }
    if (promotedType == QMetaType::UInt) {
        return uint(l1) == uint(l2);
    }
    if (promotedType == QMetaType::LongLong) {
        return l1 == l2;
    }
    if (promotedType == QMetaType::ULongLong) {
        return qulonglong(l1) == qulonglong(l2);
    }
    Q_ASSERT(false);
    return 0;
}

static bool numericEquals(const QVariant::Private *d1, const QVariant::Private *d2)
{
    uint promotedType = numericTypePromotion(d1->typeId(), d2->typeId());
    if (promotedType != QMetaType::QReal) {
        return integralEquals(promotedType, d1, d2);
    }
    bool ok;
    qreal r1 = qConvertToRealNumber(d1, &ok);
    if (!ok) {
        return false;
    }
    qreal r2 = qConvertToRealNumber(d2, &ok);
    if (!ok) {
        return false;
    }
    if (r1 == r2) {
        return true;
    }
    return false;
}

static bool canConvertMetaObject(QMetaType fromType, QMetaType toType)
{
    Q_ASSERT(false);
    return false;
}

static bool pointerEquals(const QVariant::Private *d1, const QVariant::Private *d2)
{
    return d1->get<QObject *>() == d2->get<QObject *>();
}

template <typename T>
inline T qNumVariantToHelper(const QVariant::Private &d, bool *ok) {
    QMetaType t= QMetaType::fromType<T>();
    if (ok) {
        *ok = true;
    }
    if (d.type() == t) {
        return d.get<T>();
    }

    T ret = 0;
    bool success = QMetaType::convert(d.type(), d.storage(), t, &ret);
    if (ok) {
        *ok = success;
    }
    return ret;
}

static void customClear(QVariant::Private *d) {
    auto iface = d->typeInterface();
    if (!iface) {
        return;
    }
    if (!d->is_shared) {
        if (iface->dtor) {
            iface->dtor(iface, &d->data);
        }
    }
    else {
        if (iface->dtor) {
            iface->dtor(iface, d->data.shared->data());
        }
        QVariant::PrivateShared::free(d->data.shared);
    }
}

static void customConstruct(QVariant::Private *d, const void *copy) {
    const QtPrivate::QMetaTypeInterface *iface = d->typeInterface();
    if (!(iface && iface->size)) {   //非法数据
        *d = QVariant::Private();
        return;
    }
    if (QVariant::Private::canUseInternalSpace(iface)) {
        Q_ASSERT(iface->copyCtr);
        Q_ASSERT(iface->defaultCtr);
        if (copy) {  //copy有数据执行拷贝构造，否则执行默认构造
            iface->copyCtr(iface, &d->data, copy);
        }
        else {
            iface->defaultCtr(iface, &d->data);
        }
        d->is_shared = false;
    }
    else {
        d->data.shared = QVariant::PrivateShared::create(iface);
        if (copy) {
            iface->copyCtr(iface, d->data.shared->data(), copy);
        }
        else {
            iface->defaultCtr(iface, d->data.shared->data());
        }
        d->is_shared = true;
    }
    d->is_null = !copy || QMetaType(iface) == QMetaType::fromType<std::nullptr_t>();
}

QVariant::~QVariant() {
    if ((d.is_shared && !d.data.shared->ref.deref()) || (!d.is_shared)) {
        customClear(&d);
    }
}

QVariant::QVariant(QMetaType type, const void *copy)
    : d(type)
{
    customConstruct(&d, copy);
}

QVariant::QVariant(const QVariant &p)
    : d(p.d)
{
    if (d.is_shared) {
        d.data.shared->ref.ref();
        return;
    }
    const QtPrivate::QMetaTypeInterface *iface = d.typeInterface();
    auto other = p.constData();
    if (iface) {
        if (other) {  //d的首地址是存放内存的成员
            iface->copyCtr(iface, &d, other);
        }
        else {
            iface->defaultCtr(iface, &d);
        }
    }
}

QVariant::QVariant(int val) noexcept
    : d(QMetaType::fromType<int>())
{
    d.set(val);
}

QVariant::QVariant(uint val) noexcept
    : d(QMetaType::fromType<uint>())
{
    d.set(val);
}

QVariant::QVariant(qlonglong val) noexcept
    : d(QMetaType::fromType<qlonglong >())
{
    d.set(val);
}

QVariant::QVariant(qulonglong val) noexcept
    : d(QMetaType::fromType<qulonglong >())
{
    d.set(val);
}

QVariant::QVariant(bool val) noexcept
    : d(QMetaType::fromType<bool>())
{
    d.set(val);
}

QVariant::QVariant(double val) noexcept
    : d(QMetaType::fromType<double>())
{
    d.set(val);
}

QVariant::QVariant(float val) noexcept
    : d(QMetaType::fromType<float>())
{
    d.set(val);
}

QVariant::QVariant(const char *str) noexcept
    : QVariant(QString::fromUtf8(str))
{
}

QVariant::QVariant(const QByteArray &val) noexcept
    : d(QMetaType::fromType<QByteArray>())
{
    v_construct<QByteArray>(&d, val);
}

QVariant::QVariant(const QBitArray &val) noexcept
    : d(QMetaType::fromType<QBitArray>())
{
    v_construct<QBitArray>(&d, val);
}

QVariant::QVariant(const QString &val) noexcept
    : d(QMetaType::fromType<QString>())
{
    v_construct<QString>(&d, val);
}

//QLatin1String转化成了QString
QVariant::QVariant(QLatin1String val) noexcept
    : d(QMetaType::fromType<QString>())
{
    v_construct<QString>(&d, val);
}

QVariant::QVariant(QChar val) noexcept
    : d(QMetaType::fromType<QChar>())
{
    v_construct<QString>(&d, val);
}

QVariant::QVariant(const QList<QVariant> &val) noexcept
    : d(QMetaType::fromType<QList<QVariant>>())
{
    v_construct<QVariantList>(&d, val);
}

QVariant::QVariant(const QMap<QString, QVariant> &val) noexcept
    : d(QMetaType::fromType<QMap<QString, QVariant>>())
{
    v_construct<QVariantMap>(&d, val);
}

QVariant &QVariant::operator=(const QVariant &other) {
    if (this == &other) {
        return *this;
    }
    clear();
    if (other.d.is_shared) {
        other.d.data.shared->ref.ref();
        d = other.d;
    }
    else {
        d = other.d;
        const QtPrivate::QMetaTypeInterface *iface = d.typeInterface();
        const void *data = other.constData();
        if (iface) {
            if (data) {
                iface->copyCtr(iface, &d, data);
            }
            else {
                Q_ASSERT(false);
                iface->defaultCtr(iface, &d);
            }
        }
    }
    return *this;
}

const char *QVariant::typeName() const
{
    return d.type().name();
}

QMetaType QVariant::metaType() const
{
    return d.type();
}

void *QVariant::data()
{
    detach();
    d.is_null = false;
    return const_cast<void *>(constData());
}

void QVariant::detach()
{
    //简单类型或者引用数为1，不需要分离
    if (!d.is_shared || d.data.shared->ref.loadRelaxed() == 1) {
        return;
    }
    Private dd(d.type());
    customConstruct(&dd, constData());  //根据原数据构造dd的数据
    if (!d.data.shared->ref.deref()) {
        customClear(&d);  //如果原数据没有被引用，clear掉
    }
    d.data.shared = dd.data.shared;  //将dd的shared指针赋值给d的指针，释放dd
}

bool QVariant::isDetached()
{
    //d是简单数据或者是没有被其他引用的复杂数据
    return !d.is_shared || d.data.shared->ref.loadRelaxed() == 1;
}

void QVariant::clear()
{
    if ((d.is_shared && !d.data.shared->ref.deref()) || (!d.is_shared)) {
        customClear(&d);
    }
    d = {};
}

bool QVariant::canConvert(QMetaType targetType) const
{
    return QMetaType::canConvert(d.type(), targetType);
}

bool QVariant::convert(QMetaType targetType)
{
    if (d.type() == targetType) {
        return targetType.isValid();
    }

    QVariant oldValue = *this;

    clear();
    create(targetType, nullptr);
    //不能转换的，直接清空了
    if (!oldValue.canConvert(targetType)) {
        return false;
    }
    //如果当前数据为空，并且目标不为空，返回false
    if (oldValue.d.is_null && oldValue.typeId() != QMetaType::Nullptr) {
        return false;
    }

    bool ok = QMetaType::convert(oldValue.d.type(), oldValue.constData(), targetType, data());
    d.is_null = !ok;
    return ok;
}

bool QVariant::isValid() const
{
    return d.type().isValid();
}

bool QVariant::isNull() const
{
    if (d.is_null || !metaType().isValid()) {
        return true;
    }
    if (metaType().flags() & QMetaType::IsPointer) {
        return d.get<void *>() == nullptr;
    }
    return false;
}

int QVariant::toInt(bool *ok) const {
    return qNumVariantToHelper<int>(d, ok);
}

uint QVariant::toUInt(bool *ok) const {
    return qNumVariantToHelper<uint>(d, ok);
}

qlonglong QVariant::toLongLong(bool *ok) const
{
    return qNumVariantToHelper<qlonglong>(d, ok);
}

qulonglong QVariant::toULongLong(bool *ok) const
{
    return qNumVariantToHelper<qulonglong>(d, ok);
}

bool QVariant::toBool() const {
    auto boolType = QMetaType::fromType<bool>();
    if (d.type() == boolType) {
        return d.get<bool>();
    }
    bool res = false;
    QMetaType::convert(d.type(), constData(), boolType, &res);
    return res;
}

double QVariant::toDouble(bool *ok) const
{
    return qNumVariantToHelper<double>(d, ok);
}

float QVariant::toFloat(bool *ok) const
{
    return qNumVariantToHelper<float>(d, ok);
}

qreal QVariant::toReal(bool *ok) const
{
    return qNumVariantToHelper<qreal>(d, ok);
}

QVariantList QVariant::toList() const {
    return qvariant_cast<QVariantList>(*this);
}

QMap<QString, QVariant> QVariant::toMap() const
{
    return qvariant_cast<QVariantMap>(*this);
}

void QVariant::create(int type, const void *copy)
{
    create(QMetaType(type), copy);
}

void QVariant::create(QMetaType metatype, const void *copy)
{
    d = Private(metatype);
    customConstruct(&d, copy);
}

bool QVariant::equals(const QVariant &v) const
{
    auto metaType = d.type();

    if (metaType != v.metaType()) {
        if (qIsNumericType(metaType.id()) && qIsNumericType(v.d.typeId())) {
            return numericEquals(&d, &v.d);
        }
        if (canConvertMetaObject(metaType, v.metaType())) {
            return pointerEquals(&d, &v.d);
        }
        return false;
    }
    if (!metaType.isValid()) {
        return true;
    }
    return metaType.equals(d.storage(), v.d.storage());
}

QT_END_NAMESPACE