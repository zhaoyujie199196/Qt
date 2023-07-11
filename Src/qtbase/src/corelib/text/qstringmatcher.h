//
// Created by Yujie Zhao on 2023/6/20.
//

#ifndef QSTRINGMATCHER_H
#define QSTRINGMATCHER_H

#include <QtCore/qstring.h>
#include <QtCore/qstringview.h>

QT_BEGIN_NAMESPACE

class QStringMatcher
{
    void updateSkipTable();
public:
    QStringMatcher() = default;
    explicit QStringMatcher(const QString &pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QStringMatcher(const QChar *uc, qsizetype len, Qt::CaseSensitivity cs = Qt::CaseSensitive)
        : QStringMatcher(QStringView(uc, len), cs)
    {}
    QStringMatcher(QStringView pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QStringMatcher(const QStringMatcher &other);
    ~QStringMatcher();

    QStringMatcher &operator=(const QStringMatcher &other);

    void setPattern(const QString &pattern);
    void setCaseSensitivity(Qt::CaseSensitivity cs);

    qsizetype indexIn(const QString &str, qsizetype from = 0) const
    { return indexIn(QStringView(str), from); }
    qsizetype indexIn(const QChar *str, qsizetype length, qsizetype from = 0) const
    { return indexIn(QStringView(str, length), from); }
    qsizetype indexIn(QStringView str, qsizetype from = 0) const;
    QString pattern() const;
    inline Qt::CaseSensitivity caseSensitivity() const { return q_cs; }

private:
    Qt::CaseSensitivity q_cs = Qt::CaseSensitive;
    QString q_pattern;
    QStringView q_sv;
    uchar q_skiptable[256] = {};
};

QT_END_NAMESPACE

#endif //QSTRINGMATCHER_H
