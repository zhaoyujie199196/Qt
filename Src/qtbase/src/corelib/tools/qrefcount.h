//
// Created by Yujie Zhao on 2023/3/6.
//

#ifndef QREFCOUNT_H
#define QREFCOUNT_H

#include <QtCore/qatomic.h>

QT_BEGIN_NAMESPACE

/*
 * 基于原子操作的引用计数
 * zhaoyujie TODO atomic为 0 / -1 代表什么含义？
 * */

namespace QtPrivate
{
    class RefCount
    {
    public:
        inline bool ref() noexcept {
            int count = atomic.loadRelaxed();
            if (count != -1) {  //zhaoyujie TODO 这里的-1是用作static？
                atomic.ref();
            }
            return true;
        }

        inline bool deref() noexcept {
            int count = atomic.loadRelaxed();
            if (count == -1) {  //zhaoyujie TODO 这里的-1是用作static？
                return true;
            }
            return atomic.deref();  //如果-1后为0，返回true
        }

        bool isStatic() const noexcept {
            return atomic.loadRelaxed() == -1;
        }

        bool isShared() const noexcept {
            int count = atomic.loadRelaxed();
            return (count != 1) && (count != 0);
        }

        void initializeOwned() noexcept {atomic.storeRelaxed(1);}
        void initializeUnsharable() noexcept { atomic.storeRelease(0);}

        QBasicAtomicInt  atomic;
    };
}

QT_END_NAMESPACE

#endif //QREFCOUNT_H
