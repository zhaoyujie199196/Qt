//
// Created by Yujie Zhao on 2023/10/8.
//
#include "qsize.h"

QT_BEGIN_NAMESPACE

void QSize::transpose() noexcept
{
    qSwap(wd, ht);
}

QSize QSize::scaled(const QSize &s, Qt::AspectRatioMode mode) const noexcept
{
    if (mode == Qt::IgnoreAspectRatio || wd == 0 || ht == 0) {
        return s;
    } else {
        bool useHeight;
        qint64 rw = qint64(s.ht) * qint64(wd) / qint64(ht);

        if (mode == Qt::KeepAspectRatio) {
            useHeight = (rw <= s.wd);
        } else { // mode == Qt::KeepAspectRatioByExpanding
            useHeight = (rw >= s.wd);
        }

        if (useHeight) {
            return QSize(rw, s.ht);
        } else {
            return QSize(s.wd,
                         qint32(qint64(s.wd) * qint64(ht) / qint64(wd)));
        }
    }
}

void QSizeF::transpose() noexcept
{
    qSwap(wd, ht);
}

QSizeF QSizeF::scaled(const QSizeF &s, Qt::AspectRatioMode mode) const noexcept
{
    if (mode == Qt::IgnoreAspectRatio || qIsNull(wd) || qIsNull(ht)) {
        return s;
    } else {
        bool useHeight;
        qreal rw = s.ht * wd / ht;

        if (mode == Qt::KeepAspectRatio) {
            useHeight = (rw <= s.wd);
        } else { // mode == Qt::KeepAspectRatioByExpanding
            useHeight = (rw >= s.wd);
        }

        if (useHeight) {
            return QSizeF(rw, s.ht);
        } else {
            return QSizeF(s.wd, s.wd * ht / wd);
        }
    }
}

QT_END_NAMESPACE