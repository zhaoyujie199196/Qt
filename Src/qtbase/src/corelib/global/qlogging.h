//
// Created by Yujie Zhao on 2023/10/7.
//

#ifndef QLOGGING_H
#define QLOGGING_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

enum QtMsgType {
    QtDebugMsg,
    QtWarningMsg,
    QtCriticalMsg,
    QtFatalMsg,
    QtInfoMsg,
    QtSystemMsg = QtCriticalMsg
};

class QMessageLogger
{
public:
    constexpr QMessageLogger() {}
    constexpr QMessageLogger(const char *file, int line, const char *function) {}
    constexpr QMessageLogger(const char *file, int line, const char *function, const char *category) {  }

    void debug(const char *msg, ...) {}
    void info(const char *msg, ...) {}
    void warning(const char *msg, ...) {}
    void critical(const char *msg, ...) {}
    void fatal(const char *msg, ...) {}
};

#if !defined(QT_MESSAGELOGCONTEXT) && !defined(QT_NO_MESSAGELOGCONTEXT)
#  if defined(QT_NO_DEBUG)
#    define QT_NO_MESSAGELOGCONTEXT
#  else
#    define QT_MESSAGELOGCONTEXT
#  endif
#endif

#ifdef QT_MESSAGELOGCONTEXT
#define QT_MESSAGELOG_FILE static_cast<const char *>(__FILE__)
  #define QT_MESSAGELOG_LINE __LINE__
  #define QT_MESSAGELOG_FUNC static_cast<const char *>(Q_FUNC_INFO)
#else
#define QT_MESSAGELOG_FILE nullptr
#define QT_MESSAGELOG_LINE 0
#define QT_MESSAGELOG_FUNC nullptr
#endif

#define qDebug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug
#define qInfo QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).info
#define qWarning QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).warning
#define qCritical QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).critical
#define qFatal QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).fatal

QT_END_NAMESPACE

#endif //QLOGGING_H
