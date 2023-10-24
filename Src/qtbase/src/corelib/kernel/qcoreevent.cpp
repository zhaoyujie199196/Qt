//
// Created by Yujie Zhao on 2023/10/20.
//
#include "qcoreevent.h"

QT_BEGIN_NAMESPACE

QEvent::QEvent(Type type)
    : t(type)
{

}

QEvent *QEvent::clone() const
{
    Q_ASSERT(false);
    return new QEvent(*this);
}

QDynamicPropertyChangeEvent::QDynamicPropertyChangeEvent(const QByteArray &name)
    : QEvent(QEvent::DynamicPropertyChange), n(name)
{

}

QDynamicPropertyChangeEvent::~QDynamicPropertyChangeEvent()
{

}

QT_END_NAMESPACE