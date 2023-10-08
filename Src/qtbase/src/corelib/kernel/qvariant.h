//
// Created by Yujie Zhao on 2023/9/26.
//

#ifndef QVARIANT_H
#define QVARIANT_H

#include <QtCore/qglobal.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qcontainerfwd.h>

QT_BEGIN_NAMESPACE

class QByteArray;
class QString;
class QLatin1String;

template <typename T>
inline T qvariant_cast(const QVariant &);

class QVariant {
    template <class T>
    friend inline T qvariant_cast(const QVariant &);
public:
    enum Type {
        Invalid = QMetaType::UnknownType,
        Bool = QMetaType::Bool,
        Int = QMetaType::Int,
        UInt = QMetaType::UInt,
        LongLong = QMetaType::LongLong,
        ULongLong = QMetaType::ULongLong,
        Double = QMetaType::Double,
        Char = QMetaType::QChar,
        Map = QMetaType::QVariantMap,
        List = QMetaType::QVariantList,
        String = QMetaType::QString,
//        StringList = QMetaType::QStringList,
        ByteArray = QMetaType::QByteArray,
        BitArray = QMetaType::QBitArray,
//        Date = QMetaType::QDate,
//        Time = QMetaType::QTime,
//        DateTime = QMetaType::QDateTime,
//        Url = QMetaType::QUrl,
//        Locale = QMetaType::QLocale,
//        Rect = QMetaType::QRect,
//        RectF = QMetaType::QRectF,
//        Size = QMetaType::QSize,
//        SizeF = QMetaType::QSizeF,
//        Line = QMetaType::QLine,
//        LineF = QMetaType::QLineF,
//        Point = QMetaType::QPoint,
//        PointF = QMetaType::QPointF,
//        RegularExpression = QMetaType::QRegularExpression,
//        Hash = QMetaType::QVariantHash,
//        EasingCurve = QMetaType::QEasingCurve,
//        Uuid = QMetaType::QUuid,
//        ModelIndex = QMetaType::QModelIndex,
//        PersistentModelIndex = QMetaType::QPersistentModelIndex,
//        LastCoreType = QMetaType::LastCoreType,

//        Font = QMetaType::QFont,
//        Pixmap = QMetaType::QPixmap,
//        Brush = QMetaType::QBrush,
//        Color = QMetaType::QColor,
//        Palette = QMetaType::QPalette,
//        Image = QMetaType::QImage,
//        Polygon = QMetaType::QPolygon,
//        Region = QMetaType::QRegion,
//        Bitmap = QMetaType::QBitmap,
//        Cursor = QMetaType::QCursor,
//        KeySequence = QMetaType::QKeySequence,
//        Pen = QMetaType::QPen,
//        TextLength = QMetaType::QTextLength,
//        TextFormat = QMetaType::QTextFormat,
//        Transform = QMetaType::QTransform,
//        Matrix4x4 = QMetaType::QMatrix4x4,
//        Vector2D = QMetaType::QVector2D,
//        Vector3D = QMetaType::QVector3D,
//        Vector4D = QMetaType::QVector4D,
//        Quaternion = QMetaType::QQuaternion,
//        PolygonF = QMetaType::QPolygonF,
//        Icon = QMetaType::QIcon,
//        LastGuiType = QMetaType::LastGuiType,

//        SizePolicy = QMetaType::QSizePolicy,

        UserType = QMetaType::User,
        LastType = 0xffffffff // need this so that gcc >= 3.4 allocates 32 bits for Type
    };

    struct PrivateShared
    {
    private:
        inline PrivateShared() : ref(1) {}
    public:
        static PrivateShared *create(const QtPrivate::QMetaTypeInterface *type) {
            Q_ASSERT(type);
            //实际数据放在PrivateShared数据之后，根据align等计算偏移地址
            size_t size = type->size;
            size_t align = type->alignment;
            auto testSize = sizeof(PrivateShared);

            size += sizeof(PrivateShared);
            if (align > sizeof(PrivateShared)) {
                size += align - sizeof(PrivateShared);
            }
            void *data = operator new(size);
            auto *ps = new (data) QVariant::PrivateShared();
            //根据align获取offset的偏移地址
            ps->offset = int(((quintptr(ps) + sizeof(PrivateShared) + align - 1) & ~(align - 1)) - quintptr(ps));
            return ps;
        }

        const void *data() const { return reinterpret_cast<const unsigned char *>(this) + offset; }
        void *data() { return reinterpret_cast<unsigned char *>(this) + offset; }
        static void free(PrivateShared *p) {
            p->~PrivateShared();
            operator delete(p);
        }
    public:
        alignas(8) QAtomicInt ref;  //zhaoyujie TODO 按照8字节对齐有什么作用？
        int offset;
    };

    struct Private
    {
        static constexpr size_t MaxInternalSize = 3 * sizeof(void *);  //data的容量

        template <size_t S>
        static constexpr bool FitsInInternalSize = S <= MaxInternalSize;  //是否在容量内。在容量内直接使用data

        template <typename T>
        static constexpr bool CanUseInternalSpace = (QTypeInfo<T>::isRelocatable && FitsInInternalSize<sizeof(T)> && alignof(T) <= sizeof(double));

        static constexpr bool canUseInternalSpace(const QtPrivate::QMetaTypeInterface *type) {
            Q_ASSERT(type);
            return QMetaType::TypeFlags(type->flags) & QMetaType::RelocatableType &&
                    size_t(type->size) <= MaxInternalSize && size_t(type->alignment) <= alignof(double);
        }

        union {
            uchar data[MaxInternalSize] = {};  //zhaoyujie TODO 这里为什么是MaxInternalSize
            PrivateShared *shared;
            double _forAlignment;  //这个_forAlignment有什么作用？
        } data;
        //is_shared, is_null, packedType 共用了8位。因为指针的地址是8的倍数，所以可以右移两位压缩存放到packedType中
        quintptr is_shared : 1;
        quintptr is_null: 1;
        quintptr packedType : sizeof(QMetaType) * 8 - 2;

        constexpr Private() noexcept
            : is_shared(false), is_null(true), packedType(0)
        {}
        explicit Private(QMetaType type) noexcept : is_shared(false), is_null(false)
        {
            quintptr mt = quintptr(type.iface());
            Q_ASSERT((mt & 0x3) == 0);
            packedType = mt >> 2;
        }

        explicit Private(const QtPrivate::QMetaTypeInterface *iface) noexcept ;

        const void *storage() const {
            return is_shared ? data.shared->data() : &data.data;
        }

        template <class T>
        const T &get() const {
            return *static_cast<const T *>(storage());
        }

        template <class T>
        void set(const T &t) {
            auto p = CanUseInternalSpace<T> ? &data.data : data.shared->data();
            *static_cast<T *>(p) = t;
        }

        inline const QtPrivate::QMetaTypeInterface *typeInterface() const {
            return reinterpret_cast<const QtPrivate::QMetaTypeInterface *>(packedType << 2);
        }

        inline QMetaType type() const {
            return QMetaType(typeInterface());
        }

        inline int typeId() const {
            return type().id();
        }
    };

    QVariant() noexcept : d() {}
    ~QVariant();
    explicit QVariant(QMetaType type, const void *copy = nullptr);
    QVariant(const QVariant &other);
    QVariant(int i) noexcept;
    QVariant(uint ui) noexcept;
    QVariant(qlonglong ll) noexcept;
    QVariant(qulonglong ull) noexcept;
    QVariant(bool b) noexcept;
    QVariant(double d) noexcept;
    QVariant(float f) noexcept;
    QVariant(const char *str) noexcept;
    QVariant(const QByteArray &bytearray) noexcept;
    QVariant(const QBitArray &bitarray) noexcept;
    QVariant(const QString &string) noexcept;
    QVariant(QLatin1String string) noexcept;
//    QVariant(const QStringList &stringList);
    QVariant(QChar qchar) noexcept;
//    QVariant(QData data);
//    QVariant(QTime time);
//    QVariant(const QDateTime &datetime);
    QVariant(const QList<QVariant> &list) noexcept;
    QVariant(const QMap<QString, QVariant> &map) noexcept;
//    QVariant(const QHash<QString, QVariant> &hash);
//    QVariant(const QSize &size);
//    QVariant(const QSizeF &size);
//    QVariant(const QPoint &pt);
//    QVariant(const QPointF &pt);
//    QVariant(const QLine &line);
//    QVariant(const QLineF &line);
//    QVariant(const QRect &rect);
//    QVariant(const QRectF &rect);
//    QVariant(const QLocale &locale);
//    QVariant(const QRegularExpression &re);
//    QVariant(const QEasingCurve &easing);
//    QVariant(const QUuid &uuid);
//    QVariant(const QUrl &url);
//    QVariant(const QJsonValue &jsonValue);
//    QVariant(const QJsonObject &jsonObject);
//    QVariant(const QJsonArray &jsonArray);
//    QVariant(const QJsonDocument &jsonDocument);
//    QVariant(const QModelIndex &modelIndex);
//    QVariant(const QPersistentModelIndex &modelIndex);

    QVariant &operator=(const QVariant &other);
    inline QVariant(QVariant &&other) noexcept : d(other.d) {
        other.d = Private();
    }

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QVariant)

    inline void swap(QVariant &other) noexcept  { qSwap(d, other.d); }

    int userType() const { return typeId(); }
    int typeId() const { return metaType().id(); }

    const char *typeName() const;
    QMetaType metaType() const;

    void *data();
    const void *constData() const { return d.storage(); }

    bool isValid() const;
    bool isNull() const;

    void clear();
    void detach();
    bool isDetached();

    int toInt(bool *ok = nullptr) const;
    uint toUInt(bool *ok = nullptr) const;
    qlonglong toLongLong(bool *ok = nullptr) const;
    qulonglong toULongLong(bool *ok = nullptr) const;
    bool toBool() const;
    double toDouble(bool *ok = nullptr) const;
    float toFloat(bool *ok = nullptr) const;
    qreal toReal(bool *ok = nullptr) const;
    QByteArray toByteArray() const;
    QBitArray toBitArray() const;
    QString toString() const;
//    QStringList toStringList() const;
    QChar toChar() const;
//    QDate toDate() const;
//    QTime toTime() const;
//    QDateTime toDateTime() const;
    QList<QVariant> toList() const;
    QMap<QString, QVariant> toMap() const;
//    QHash<QString, QVariant> toHash() const;

//    QPoint toPoint() const;
//    QPointF toPointF() const;
//    QRect toRect() const;
//    QSize toSize() const;
//    QSizeF toSizeF() const;
//    QLine toLine() const;
//    QLineF toLineF() const;
//    QRectF toRectF() const;
//    QLocale toLocale() const;
//    QRegularExpression toRegularExpression() const;
//    QEasingCurve toEasingCurve() const;
//    QUuid toUuid() const;
//    QUrl toUrl() const;
//    QJsonValue toJsonValue() const;
//    QJsonObject toJsonObject() const;
//    QJsonArray toJsonArray() const;
//    QJsonDocument toJsonDocument() const;
//    QModelIndex toModelIndex() const;
//    QPersistentModelIndex toPersistentModelIndex() const;

//    void load(QDataStream &ds);
//    void save(QDataStream &ds) const;

    explicit QVariant(Type type)
        : QVariant(QMetaType(int(type)))
    {}
    Type type() const
    {
        int type = d.typeId();
        return type >= QMetaType::User ? UserType : static_cast<Type>(type);
    }

    static const char *typeToName(int typeId) {
        return QMetaType(typeId).name();
    }
    static Type nameToType(const char *name) {
        int metaType = QMetaType::fromName(name).id();
        return metaType >= QMetaType::User ? UserType: static_cast<Type>(metaType);
    }

    template <class T>
    const T &get() const {
        return *static_cast<const T *>(d.storage());
    }

//    QDataStream &operator>>(QDataStream &s, QVariant &p);
//    QDataStream &operator<<(QDataStream &s, const QVariant &p);

    // 左值和右值都会走到这个函数。
    // 引用折叠：https://zhuanlan.zhihu.com/p/50816420
    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, QVariant>>>
    void setValue(T &&value) {
        using VT = std::decay_t<T>;
        QMetaType metaType = QMetaType::fromType<VT>();
        if (isDetached() && d.type() == metaType) {
            //数据类型一样，直接调用了T::operator=(T &&)
            *reinterpret_cast<VT *>(const_cast<void *>(constData())) = std::forward<T>(value);
        }
        else {
            *this = QVariant::fromValue<VT>(std::forward<T>(value));
        }
    }
    void setValue(const QVariant &value) {
        *this = value;
    }
    void setValue(QVariant &&value)
    {
        *this = std::move(value);
    }

    template <typename T>
    inline T value() const
    {
        return qvariant_cast<T>(*this);
    }

    bool canConvert(QMetaType targetType) const;
    bool convert(QMetaType type);
    bool canConvert(int targetTypeId) const { return canConvert(QMetaType(targetTypeId)); }
    bool convert(int targetTypeId) { return convert(QMetaType(targetTypeId)); }

    bool canView(QMetaType targetType) const
    {
        return QMetaType::canView(d.type(), targetType);
    }
    template <typename T>
    inline T view() {
        T t{};
        QMetaType::view(metaType(), data(), QMetaType::fromType<T>(), &t);
        return t;
    }
    template <typename T>
    bool canView() {
        return canView(QMetaType::fromType<T>());
    }

    template <typename T>
    static inline auto fromValue(const T &value)-> std::enable_if_t<std::is_copy_constructible_v<T>, QVariant>
    {
        return QVariant(QMetaType::fromType<T>(), std::addressof(value));
    }

    template <typename... Types>
    static inline QVariant fromStdVariant(const std::variant<Types...> &value)
    {
        Q_ASSERT(false);
        if (value.valueless_by_exception()) {
            return QVariant();
        }
        return std::visit([](const auto &arg) {
            return fromValue(arg);
        }, value);
    }

    friend inline bool operator==(const QVariant &a, const QVariant &b)
    { return a.equals(b); }
    friend inline bool operator!=(const QVariant &a, const QVariant &b)
    { return !a.equals(b); }

protected:
    void create(int tyoe, const void *copy);
    void create(QMetaType type, const void *copy);
    bool equals(const QVariant &other) const;

private:
    Private d;
};

template <typename T>
inline T qvariant_cast(const QVariant &v) {
    QMetaType targetType = QMetaType::fromType<T>();
    if (v.d.type() == targetType) {
        return v.d.get<T>();
    }
    if constexpr (std::is_same_v<T, std::remove_const_t<std::remove_pointer_t<T>> const *>) {
        using nonConstT = std::remove_const_t<std::remove_pointer_t<T>> *;
        QMetaType nonConstTargetType = QMetaType::fromType<nonConstT>();
        if (v.d.type() == nonConstTargetType) {
            return v.d.get<nonConstT>();
        }
    }
    T t{};
    QMetaType::convert(v.metaType(), v.constData(), targetType, &t);
    return t;
}

template <>
inline QVariant qvariant_cast<QVariant>(const QVariant &v) {
    if (v.metaType().id() == QMetaType::QVariant) {
        return *reinterpret_cast<const QVariant *>(v.constData());
    }
    return v;
}

QT_END_NAMESPACE

#endif //QVARIANT_H
