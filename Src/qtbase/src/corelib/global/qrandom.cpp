//
// Created by Yujie Zhao on 2023/10/10.
//
#include "qrandom.h"
#include "qrandom_p.h"
#include <QtCore/qatomic.h>
#include <fcntl.h>

#ifdef Q_OS_UNIX
#include <QtCore/private/qcore_unix_p.h>
#endif

QT_BEGIN_NAMESPACE

template <typename Generator, typename FillBufferType, typename T>
static qsizetype callFillBuffer(FillBufferType f, T *v) {
    if constexpr (std::is_member_function_pointer_v<FillBufferType>) {
        return (Generator::self().*f)(v, sizeof(*v));
    } else {
        return f(v, sizeof(*v));
    }
}

struct QRandomGenerator::SystemGenerator {
#if QT_CONFIG(getentropy)
    static_assert(false);
#elif defined(Q_OS_UNIX)
    QBasicAtomicInt fdp1;
    int openDevice()
    {
        int fd = fdp1.loadAcquire() - 1;
        if (fd != -1) {
            return fd;
        }

        fd = qt_safe_open("/dev/urandom", O_RDONLY);
        if (fd == -1) {
            fd = qt_safe_open("/dev/random", O_RDONLY | O_NONBLOCK);
        }
        if (fd == -1) {
            fd = -2;
        }
        int opened_fdp1;
        if (fdp1.testAndSetOrdered(0, fd + 1, opened_fdp1)) {
            return fd;
        }
        if (fd >= 0) {
            qt_safe_close(fd);
        }
        return opened_fdp1 - 1;
    }

    static void closeDevice()
    {
        int fd = self().fdp1.loadRelaxed() - 1;
        if (fd >= 0) {
            qt_safe_close(fd);
        }
    }

    qsizetype fillBuffer(void *buffer, qsizetype count) {
        int fd = openDevice();
        if (Q_UNLIKELY(fd < 0)) {
            return 0;
        }
        qint64 n = qt_safe_read(fd, buffer, count);
        return qMax<qsizetype>(n, 0);
    }

    static SystemGenerator &self();


#elif defined(Q_OS_WIN)
    static_assert(false);
#endif
};

struct QRandomGenerator::SystemAndGlobalGenerators {
    SystemGenerator sys;

    static SystemAndGlobalGenerators *self() {
        static SystemAndGlobalGenerators g;
        return &g;
    }
};

inline QRandomGenerator::SystemGenerator &QRandomGenerator::SystemGenerator::self()
{
    return SystemAndGlobalGenerators::self()->sys;
}

QRandomGenerator::InitialRandomData qt_initial_random_value() noexcept
{
    QRandomGenerator::InitialRandomData v;
    for (int attempts = 16; attempts != 0; --attempts) {
        using Generator = QRandomGenerator::SystemGenerator;
        auto fillBuffer = &Generator::fillBuffer;
        if (callFillBuffer<Generator>(fillBuffer, &v) != sizeof(v)) {
            continue;
        }
        return v ;
    }

    return v;
#pragma message("qt_initial_random_value 未完成")
//    quint32 data[sizeof(v) / sizeof(quint32)];
//    fallback_fill(data, std::size(data));
//    memcpy(v.data, data, sizeof(v.data));
//    return v;
}

QT_END_NAMESPACE
