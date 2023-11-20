//
// Created by Yujie Zhao on 2023/10/20.
//

#ifndef QMETAMACROS_H
#define QMETAMACROS_H

#include <QtCore/qglobal.h>

#ifndef Q_MOC_OUTPUT_REVISION
#define Q_MOC_OUTPUT_REVISION 68
#endif

//zhaoyujie TODO 没太明白这段什么意思
#ifndef QT_ANNOTATE_CLASS  //QT_ANNOTATE_CLASS展开后为空
# define QT_ANNOTATE_CLASS(type, ...)
#endif
#ifndef QT_ANNOTATE_CLASS2
# define QT_ANNOTATE_CLASS2(type, a1, a2)
#endif
#ifndef QT_ANNOTATE_FUNCTION
# define QT_ANNOTATE_FUNCTION(x)
#endif
#ifndef QT_ANNOTATE_ACCESS_SPECIFIER
# define QT_ANNOTATE_ACCESS_SPECIFIER(x)
#endif

QT_BEGIN_NAMESPACE

#define QT_TR_FUNCTIONS

#define Q_OBJECT_NO_OVERRIDE_WARNING

#define Q_OBJECT_NO_ATTRIBUTES_WARNING

#define Q_DECL_HIDDEN_STATIC_METACALL

#define emit

/*
 * 声明Q_OBJECT宏
 * staticMetaObject: 类的元对象，在moc文件中实例化
 * metaObject() 获取累的元对象
 * qt_metacast zhaoyujie TODO 元对象转换 ？父子类之间可以转？
 * qt_static_metacall: 调用类中的方法
 * QPrivateSignal:  zhaoyujie TODO
 * QT_ANNOTATE_CLASS(qt_qobject, "")  zhaoyujie TODO
 * */

#define Q_OBJECT \
public:          \
    QT_WARNING_PUSH \
    Q_OBJECT_NO_OVERRIDE_WARNING \
    static const QMetaObject staticMetaObject;  \
    virtual const QMetaObject *metaObject() const;  \
    virtual void *qt_metacast(const char *);        \
    virtual int qt_metacall(QMetaObject::Call, int, void **);             \
    QT_TR_FUNCTIONS \
private: \
    Q_OBJECT_NO_ATTRIBUTES_WARNING \
    \
    Q_DECL_HIDDEN_STATIC_METACALL static void qt_static_metacall(QObject *, QMetaObject::Call, int, void **); \
    QT_WARNING_POP \
    struct QPrivateSignal {}; \
    QT_ANNOTATE_CLASS(qt_qobject, "")



//#define Q_INVOKABLE  QT_ANNOTATE_FUNCTION(qt_invokable)
#define Q_INVOKABLE
//#define Q_SCRIPTABLE QT_ANNOTATE_FUNCTION(qt_scriptable)
#define Q_SCRIPTABLE

#define Q_PROPERTY(...) QT_ANNOTATE_CLASS(qt_property, __VA_ARGS__)

#define slots QT_ANNOTATE_ACCESS_SPECIFIER(qt_slot)
#define signals public QT_ANNOTATE_ACCESS_SPECIFIER(qt_signal)
#define Q_SIGNALS signals
#define Q_SLOTS slots

#ifndef QT_MOC_COMPAT
#  define QT_MOC_COMPAT
#else
#  undef QT_MOC_COMPAT
#  define QT_MOC_COMPAT
#endif

QT_END_NAMESPACE

#endif //QMETAMACROS_H
