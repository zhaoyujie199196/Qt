/****************************************************************************
** Meta object code from reading C++ file 'qobject.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "qobject.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qobject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QObject_t {
    const uint offsetsAndSize[8];
    char stringdata0[27];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QObject_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QObject_t qt_meta_stringdata_QObject = {
    {
QT_MOC_LITERAL(0, 7), // "QObject"
QT_MOC_LITERAL(8, 0), // ""
QT_MOC_LITERAL(9, 6), // "parent"
QT_MOC_LITERAL(16, 10) // "objectName"

    },
    "QObject\0\0parent\0objectName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QObject[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   18, // properties
       0,    0, // enums/sets
       2,   23, // constructors
       0,       // flags
       0,       // signalCount

 // constructors: parameters
    0x80000000 | 1, QMetaType::QObjectStar,    2,
    0x80000000 | 1,

 // properties: name, type, flags
       3, QMetaType::QString, 0x00015103, uint(-1), 0,

 // constructors: name, argc, parameters, tag, flags, initial metatype offsets
       0,    1,   14,    1, 0x0e,    2 /* Public */,
       0,    0,   17,    1, 0x2e,    3 /* Public | MethodCloned */,

       0        // eod
};

void QObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { QObject *_r = new QObject((*reinterpret_cast< std::add_pointer_t<QObject*>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { QObject *_r = new QObject();
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<QObject *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->objectName(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<QObject *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setObjectName(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
}

const QMetaObject QObject::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_QObject.offsetsAndSize,
    qt_meta_data_QObject,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QObject_t
, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QObject, std::true_type>


, QtPrivate::TypeAndForceComplete<QObject *, std::false_type>
>,
    nullptr
} };


const QMetaObject *QObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QObject::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QObject.stringdata0))
        return static_cast<void*>(this);
    return nullptr;
}

int QObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
