#ifndef QCOMPILERDETECTION_H
#define QCOMPILERDETECTION_H

//__attribute__((const)) 编译器优化，对结果做缓存，相同的输入直接返回缓存结果
//https://bbs.csdn.net/topics/390275079
#define Q_DECL_CONST_FUNCTION __attribute__((const))

//__builtin_expect https://blog.csdn.net/qq_22660775/article/details/89028258
#define Q_UNLIKELY(expr) __builtin_expect(!!(expr), false)

#endif //QCOMPILERDETECTION_H