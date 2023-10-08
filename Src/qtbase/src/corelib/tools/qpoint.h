//
// Created by Yujie Zhao on 2023/10/8.
//

#ifndef QPOINT_H
#define QPOINT_H

#include <QtCore/qnamespace.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qdebug.h>
#include <QtCore/qnumeric.h>

#if defined(Q_OS_DARWIN)
struct CGPoint;
#endif

QT_BEGIN_NAMESPACE

/*
 * 结构绑定语法：https://zhuanlan.zhihu.com/p/652817532
 * */

class QTransform;
class QPoint
{
public:
    friend class QTransform;
    constexpr QPoint() noexcept;
    constexpr QPoint(int xpos, int ypos) noexcept;

    constexpr inline bool isNull() const noexcept;
    constexpr inline int x() const noexcept;
    constexpr inline int y() const noexcept;
    constexpr inline void setX(int x) noexcept;
    constexpr inline void setY(int y) noexcept;

    //曼哈顿距离， |x| + |y|
    constexpr inline int manhattanLength() const;

    //x y对换
    constexpr QPoint transposed() const noexcept { return {yp, xp}; }

    constexpr inline int &rx() noexcept;
    constexpr inline int &ry() noexcept;

    constexpr inline QPoint &operator+=(const QPoint &p);
    constexpr inline QPoint &operator-=(const QPoint &p);

    constexpr inline QPoint &operator*=(float factor);
    constexpr inline QPoint &operator*=(double factor);
    constexpr inline QPoint &operator*=(int factor);

    constexpr inline QPoint &operator/=(qreal divisor);

    //点乘法
    constexpr static inline int dotProduct(const QPoint &p1, const QPoint &p2)
    { return p1.xp * p2.xp + p1.yp * p2.yp; }

    friend constexpr inline bool operator==(const QPoint &p1, const QPoint &p2) noexcept
    { return p1.xp == p2.xp && p1.yp == p2.yp; }
    friend constexpr inline bool operator!=(const QPoint &p1, const QPoint &p2) noexcept
    { return p1.xp != p2.xp || p1.yp != p2.yp; }

    friend constexpr inline QPoint operator+(const QPoint &p1, const QPoint &p2) noexcept
    { return QPoint(p1.xp + p2.xp, p1.yp + p2.yp); }
    friend constexpr inline QPoint operator-(const QPoint &p1, const QPoint &p2) noexcept
    { return QPoint(p1.xp - p2.xp, p1.yp - p2.yp); }

    //乘法
    friend constexpr inline QPoint operator*(const QPoint &p, float factor)
    { return QPoint(qRound(p.xp * factor), qRound(p.yp * factor)); }
    friend constexpr inline QPoint operator*(const QPoint &p, double factor)
    { return QPoint(qRound(p.xp * factor), qRound(p.yp * factor)); }
    friend constexpr inline QPoint operator*(const QPoint &p, int factor) noexcept
    { return QPoint(p.xp * factor, p.yp * factor); }
    friend constexpr inline QPoint operator*(float factor, const QPoint &p)
    { return QPoint(qRound(p.xp * factor), qRound(p.yp * factor)); }
    friend constexpr inline QPoint operator*(double factor, const QPoint &p)
    { return QPoint(qRound(p.xp * factor), qRound(p.yp * factor)); }
    friend constexpr inline QPoint operator*(int factor, const QPoint &p) noexcept
    { return QPoint(p.xp * factor, p.yp * factor); }

    //+号 / -号
    friend constexpr inline QPoint operator+(const QPoint &p) noexcept
    { return p; }
    friend constexpr inline QPoint operator-(const QPoint &p) noexcept
    { return QPoint(-p.xp, -p.yp); }
    friend constexpr inline QPoint operator/(const QPoint &p, qreal c)
    { return QPoint(qRound(p.xp / c), qRound(p.yp / c)); }

#if defined(Q_OS_DARWIN) || defined(Q_QDOC)
    [[nodiscard]] Q_CORE_EXPORT CGPoint toCGPoint() const noexcept;
#endif

private:
    int xp = 0;
    int yp = 0;

    template <std::size_t I,
            typename P,
            std::enable_if_t<(I < 2), bool> = true,
            std::enable_if_t<std::is_same_v<std::decay_t<P>, QPoint>, bool> = true>
    friend constexpr decltype(auto) get(P &&p) noexcept
    {
        if constexpr (I == 0)
            return (std::forward<P>(p).xp);
        else if constexpr (I == 1)
            return (std::forward<P>(p).yp);
    }
};
Q_DECLARE_TYPEINFO(QPoint, Q_PRIMITIVE_TYPE)

QDataStream &operator<<(QDataStream &s, const QPoint &p) { Q_ASSERT(false); return s; }
QDataStream &operator>>(QDataStream &s, QPoint &p) { Q_ASSERT(false); return s; }

constexpr inline QPoint::QPoint() noexcept : xp(0), yp(0) {}

constexpr inline QPoint::QPoint(int xpos, int ypos) noexcept : xp(xpos), yp(ypos) {}

constexpr inline bool QPoint::isNull() const noexcept
{
    return xp == 0 && yp == 0;
}

constexpr inline int QPoint::x() const noexcept
{
    return xp;
}

constexpr inline int QPoint::y() const noexcept
{
    return yp;
}

constexpr inline void QPoint::setX(int xpos) noexcept
{
    xp = xpos;
}

constexpr inline void QPoint::setY(int ypos) noexcept
{
    yp = ypos;
}

inline int constexpr QPoint::manhattanLength() const
{
    return qAbs(x()) + qAbs(y());
}

constexpr inline int &QPoint::rx() noexcept
{
    return xp;
}

constexpr inline int &QPoint::ry() noexcept
{
    return yp;
}

constexpr inline QPoint &QPoint::operator+=(const QPoint &p)
{
    xp += p.xp;
    yp += p.yp;
    return *this;
}

constexpr inline QPoint &QPoint::operator-=(const QPoint &p)
{
    xp -= p.xp;
    yp -= p.yp;
    return *this;
}

constexpr inline QPoint &QPoint::operator*=(float factor)
{
    xp = qRound(xp * factor);
    yp = qRound(yp * factor);
    return *this;
}

constexpr inline QPoint &QPoint::operator*=(double factor)
{
    xp = qRound(xp * factor);
    yp = qRound(yp * factor);
    return *this;
}

constexpr inline QPoint &QPoint::operator*=(int factor)
{
    xp = xp * factor;
    yp = yp * factor;
    return *this;
}

constexpr inline QPoint &QPoint::operator/=(qreal c)
{
    xp = qRound(xp / c);
    yp = qRound(yp / c);
    return *this;
}

QDebug operator<<(QDebug debug, const QPoint &p) { Q_ASSERT(false); return debug; }
size_t qHash(QPoint key, size_t seed = 0) noexcept { Q_ASSERT(false); return 0; }

class QPointF
{
public:
    friend class QTransform;
    constexpr QPointF() noexcept;
    constexpr QPointF(const QPoint &p) noexcept;
    constexpr QPointF(qreal xpos, qreal ypos) noexcept;

    constexpr inline qreal manhattanLength() const;

    inline bool isNull() const noexcept;

    constexpr inline qreal x() const noexcept;
    constexpr inline qreal y() const noexcept;
    constexpr inline void setX(qreal x) noexcept;
    constexpr inline void setY(qreal y) noexcept;

    constexpr QPointF transposed() const noexcept { return {yp, xp}; }

    constexpr inline qreal &rx() noexcept;
    constexpr inline qreal &ry() noexcept;

    constexpr inline QPointF &operator+=(const QPointF &p);
    constexpr inline QPointF &operator-=(const QPointF &p);
    constexpr inline QPointF &operator*=(qreal c);
    constexpr inline QPointF &operator/=(qreal c);

    constexpr static inline qreal dotProduct(const QPointF &p1, const QPointF &p2)
    {
        return p1.xp * p2.xp + p1.yp * p2.yp;
    }

    QT_WARNING_PUSH
    QT_WARNING_DISABLE_FLOAT_COMPARE
    friend constexpr inline bool operator==(const QPointF &p1, const QPointF &p2)
    {
        return ((!p1.xp || !p2.xp) ? qFuzzyIsNull(p1.xp - p2.xp) : qFuzzyCompare(p1.xp, p2.xp))
               && ((!p1.yp || !p2.yp) ? qFuzzyIsNull(p1.yp - p2.yp) : qFuzzyCompare(p1.yp, p2.yp));
    }
    friend constexpr inline bool operator!=(const QPointF &p1, const QPointF &p2)
    {
        return !(p1 == p2);
    }
    QT_WARNING_POP

    friend constexpr inline QPointF operator+(const QPointF &p1, const QPointF &p2)
    { return QPointF(p1.xp + p2.xp, p1.yp + p2.yp); }
    friend constexpr inline QPointF operator-(const QPointF &p1, const QPointF &p2)
    { return QPointF(p1.xp - p2.xp, p1.yp - p2.yp); }
    friend constexpr inline QPointF operator*(const QPointF &p, qreal c)
    { return QPointF(p.xp * c, p.yp * c); }
    friend constexpr inline QPointF operator*(qreal c, const QPointF &p)
    { return QPointF(p.xp * c, p.yp * c); }
    friend constexpr inline QPointF operator+(const QPointF &p)
    { return p; }
    friend constexpr inline QPointF operator-(const QPointF &p)
    { return QPointF(-p.xp, -p.yp); }
    friend constexpr inline QPointF operator/(const QPointF &p, qreal divisor)
    {
        Q_ASSERT(divisor < 0 || divisor > 0);
        return QPointF(p.xp / divisor, p.yp / divisor);
    }

    constexpr QPoint toPoint() const;

#if defined(Q_OS_DARWIN)
    [[nodiscard]] Q_CORE_EXPORT static QPointF fromCGPoint(CGPoint point) noexcept;
    [[nodiscard]] Q_CORE_EXPORT CGPoint toCGPoint() const noexcept;
#endif

private:
    qreal xp = 0.0;
    qreal yp = 0.0;

    //结构绑定语法： auto[x, y] = p;
    //https://zhuanlan.zhihu.com/p/652817532
    template <std::size_t I,
            typename P,
            std::enable_if_t<(I < 2), bool> = true,
            std::enable_if_t<std::is_same_v<std::decay_t<P>, QPointF>, bool> = true>
    friend constexpr decltype(auto) get(P &&p) noexcept
    {
        if constexpr (I == 0)
            return (std::forward<P>(p).xp);
        else if constexpr (I == 1)
            return (std::forward<P>(p).yp);
    }
};

Q_DECLARE_TYPEINFO(QPointF, Q_PRIMITIVE_TYPE);

size_t qHash(QPointF, size_t seed = 0) = delete;

QDataStream &operator<<(QDataStream &s, const QPointF &p) { Q_ASSERT(false); return s; }
QDataStream &operator>>(QDataStream &s, QPointF &p) { Q_ASSERT(false); return s; }

constexpr inline QPointF::QPointF() noexcept : xp(0), yp(0) { }

constexpr inline QPointF::QPointF(qreal xpos, qreal ypos) noexcept : xp(xpos), yp(ypos) { }

constexpr inline QPointF::QPointF(const QPoint &p) noexcept : xp(p.x()), yp(p.y()) { }

constexpr inline qreal QPointF::manhattanLength() const
{
    return qAbs(x()) + qAbs(y());
}

inline bool QPointF::isNull() const noexcept
{
    return qFuzzyIsNull(xp) && qFuzzyIsNull(yp);
}

constexpr inline qreal QPointF::x() const noexcept
{
    return xp;
}

constexpr inline qreal QPointF::y() const noexcept
{
    return yp;
}

constexpr inline void QPointF::setX(qreal xpos) noexcept
{
    xp = xpos;
}

constexpr inline void QPointF::setY(qreal ypos) noexcept
{
    yp = ypos;
}

constexpr inline qreal &QPointF::rx() noexcept
{
    return xp;
}

constexpr inline qreal &QPointF::ry() noexcept
{
    return yp;
}

constexpr inline QPointF &QPointF::operator+=(const QPointF &p)
{
    xp += p.xp;
    yp += p.yp;
    return *this;
}

constexpr inline QPointF &QPointF::operator-=(const QPointF &p)
{
    xp -= p.xp;
    yp -= p.yp;
    return *this;
}

constexpr inline QPointF &QPointF::operator*=(qreal c)
{
    xp *= c;
    yp *= c;
    return *this;
}

constexpr inline QPointF &QPointF::operator/=(qreal divisor)
{
    Q_ASSERT(divisor > 0 || divisor < 0);
    xp /= divisor;
    yp /= divisor;
    return *this;
}

constexpr inline QPoint QPointF::toPoint() const
{
    return QPoint(qRound(xp), qRound(yp));
}

QDebug operator<<(QDebug d, const QPointF &p) { Q_ASSERT(false); return d; }

//zhaoyujie TODO
//namespace std {
//    template <>
//    class tuple_size<QT_PREPEND_NAMESPACE(QPoint)> : public integral_constant<size_t, 2> {};
//    template <>
//    class tuple_element<0, QT_PREPEND_NAMESPACE(QPoint)> { public: using type = int; };
//    template <>
//    class tuple_element<1, QT_PREPEND_NAMESPACE(QPoint)> { public: using type = int; };
//
//    template <>
//    class tuple_size<QT_PREPEND_NAMESPACE(QPointF)> : public integral_constant<size_t, 2> {};
//    template <>
//    class tuple_element<0, QT_PREPEND_NAMESPACE(QPointF)> { public: using type = QT_PREPEND_NAMESPACE(qreal); };
//    template <>
//    class tuple_element<1, QT_PREPEND_NAMESPACE(QPointF)> { public: using type = QT_PREPEND_NAMESPACE(qreal); };
//}

QT_END_NAMESPACE

#endif //QPOINT_H
