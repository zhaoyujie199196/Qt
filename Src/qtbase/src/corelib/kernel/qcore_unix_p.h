//
// Created by Yujie Zhao on 2023/10/17.
//

#ifndef QCORE_UNIX_P_H
#define QCORE_UNIX_P_H

#include <QtCore/private/qglobal_p.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef Q_OS_UNIX
#error "qcore_unix_p.h included on a non-Unix System"
#endif

#if defined(Q_PROCESSOR_X86_32) && defined(__GLIBC__)
Q_CORE_EXPORT int qt_open64(const char *pathname, int flags, mode_t);
#endif

#define EINTR_LOOP(var, cmd) \
    do {                         \
        var = cmd;                             \
    } while (var == -1 && errno == EINTR)

//安全打开文件描述符
static inline int qt_safe_open(const char *pathname, int flags, mode_t mode = 0777)
{
#ifdef O_CLOEXEC
    flags |= O_CLOEXEC;
#endif
    int fd;
    EINTR_LOOP(fd, ::open(pathname, flags, mode));
#ifndef O_CLOEXEC
    if (fd != -1)
        ::fcntl(fd, F_SETFD, FD_CLOEXEC);
#endif
    return fd;
}
#undef QT_OPEN
#define QT_OPEN qt_safe_open

//暗管关闭文件描述符
static inline int qt_safe_close(int fd)
{
    int ret;
    EINTR_LOOP(ret, ::close(fd));
    return ret;
}
#undef QT_CLOSE
#define QT_CLOSE qt_safe_close

static inline qint64 qt_safe_read(int fd, void *data, qint64 maxlen)
{
    qint64 ret = 0;
    EINTR_LOOP(ret, ::read(fd, data, maxlen));
    return ret;
}

#endif //QCORE_UNIX_P_H
