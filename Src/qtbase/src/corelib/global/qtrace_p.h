//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QTRACE_P_H
#define QTRACE_P_H

#include <QtCore/qglobal.h>
#include <QtCore/qscopeguard.h>

QT_BEGIN_NAMESPACE

#if defined(Q_TRACEPOINT) && !defined(QT_BOOTSTRAPPED)
#  define Q_HAS_TRACEPOINTS 1
#  define Q_TRACE(x, ...) QtPrivate::trace_ ## x(__VA_ARGS__)
#  define Q_TRACE_EXIT(x, ...) \
    const auto qTraceExit_ ## x ## __COUNTER__ = qScopeGuard([&]() { Q_TRACE(x, __VA_ARGS__); });
#  define Q_TRACE_SCOPE(x, ...) \
    Q_TRACE(x ## _entry, __VA_ARGS__); \
    Q_TRACE_EXIT(x ## _exit);
#  define Q_UNCONDITIONAL_TRACE(x, ...) QtPrivate::do_trace_ ## x(__VA_ARGS__)
#  define Q_TRACE_ENABLED(x) QtPrivate::trace_ ## x ## _enabled()
#else
#  define Q_HAS_TRACEPOINTS 0
#  define Q_TRACE(x, ...)
#  define Q_TRACE_EXIT(x, ...)
#  define Q_TRACE_SCOPE(x, ...)
#  define Q_UNCONDITIONAL_TRACE(x, ...)
#  define Q_TRACE_ENABLED(x) false
#endif // defined(Q_TRACEPOINT) && !defined(QT_BOOTSTRAPPED)

QT_END_NAMESPACE


#endif //QTRACE_P_H
