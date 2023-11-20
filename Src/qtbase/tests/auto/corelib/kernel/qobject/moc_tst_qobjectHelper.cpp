/****************************************************************************
** Meta object code from reading C++ file 'tst_qobjectHelper.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "./tst_qobjectHelper.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tst_qobjectHelper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SenderObject_t {
    const uint offsetsAndSize[36];
    char stringdata0[143];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_SenderObject_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_SenderObject_t qt_meta_stringdata_SenderObject = {
    {
QT_MOC_LITERAL(0, 12), // "SenderObject"
QT_MOC_LITERAL(13, 7), // "signal1"
QT_MOC_LITERAL(21, 0), // ""
QT_MOC_LITERAL(22, 7), // "signal2"
QT_MOC_LITERAL(30, 7), // "signal3"
QT_MOC_LITERAL(38, 7), // "signal4"
QT_MOC_LITERAL(46, 7), // "signal5"
QT_MOC_LITERAL(54, 7), // "signal6"
QT_MOC_LITERAL(62, 7), // "signal7"
QT_MOC_LITERAL(70, 11), // "aPublicSlot"
QT_MOC_LITERAL(82, 7), // "invoke1"
QT_MOC_LITERAL(90, 8), // "sinvoke1"
QT_MOC_LITERAL(99, 7), // "invoke2"
QT_MOC_LITERAL(107, 8), // "sinvoke2"
QT_MOC_LITERAL(116, 7), // "invoke3"
QT_MOC_LITERAL(124, 4), // "hinz"
QT_MOC_LITERAL(129, 4), // "kunz"
QT_MOC_LITERAL(134, 8) // "sinboke3"

    },
    "SenderObject\0signal1\0\0signal2\0signal3\0"
    "signal4\0signal5\0signal6\0signal7\0"
    "aPublicSlot\0invoke1\0sinvoke1\0invoke2\0"
    "sinvoke2\0invoke3\0hinz\0kunz\0sinboke3"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SenderObject[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  116,    2, 0x06,    1 /* Public */,
       3,    0,  117,    2, 0x06,    2 /* Public */,
       4,    0,  118,    2, 0x06,    3 /* Public */,
       5,    0,  119,    2, 0x06,    4 /* Public */,
       6,    0,  120,    2, 0x16,    5 /* Public | MethodCompatibility */,
       7,    0,  121,    2, 0x06,    6 /* Public */,
       8,    2,  122,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,  127,    2, 0x0a,   10 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      10,    0,  128,    2, 0x02,   11 /* Public */,
      11,    0,  129,    2, 0x42,   12 /* Public | isScriptable */,
      12,    0,  130,    2, 0x11,   13 /* Protected | MethodCompatibility */,
      12,    1,  131,    2, 0x11,   14 /* Protected | MethodCompatibility */,
      13,    0,  134,    2, 0x51,   16 /* Protected | MethodCompatibility | isScriptable */,
      14,    2,  135,    2, 0x00,   17 /* Private */,
      14,    1,  140,    2, 0x20,   20 /* Private | MethodCloned */,
      14,    0,  143,    2, 0x20,   22 /* Private | MethodCloned */,
      17,    0,  144,    2, 0x40,   23 /* Private | isScriptable */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,

 // slots: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   15,   16,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SenderObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SenderObject *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal1(); break;
        case 1: _t->signal2(); break;
        case 2: _t->signal3(); break;
        case 3: _t->signal4(); break;
        case 4: _t->signal5(); break;
        case 5: _t->signal6(); break;
        case 6: _t->signal7((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->aPublicSlot(); break;
        case 8: _t->invoke1(); break;
        case 9: _t->sinvoke1(); break;
        case 10: _t->invoke2(); break;
        case 11: _t->invoke2((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->sinvoke2(); break;
        case 13: _t->invoke3((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 14: _t->invoke3((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->invoke3(); break;
        case 16: _t->sinboke3(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SenderObject::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SenderObject::signal1)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SenderObject::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SenderObject::signal2)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SenderObject::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SenderObject::signal3)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SenderObject::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SenderObject::signal4)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SenderObject::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SenderObject::signal5)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SenderObject::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SenderObject::signal6)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SenderObject::*)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SenderObject::signal7)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject SenderObject::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SenderObject.offsetsAndSize,
    qt_meta_data_SenderObject,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_SenderObject_t
, QtPrivate::TypeAndForceComplete<SenderObject, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

>,
    nullptr
} };


const QMetaObject *SenderObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SenderObject::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SenderObject.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SenderObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void SenderObject::signal1()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SenderObject::signal2()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SenderObject::signal3()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SenderObject::signal4()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SenderObject::signal5()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SenderObject::signal6()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SenderObject::signal7(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
struct qt_meta_stringdata_ReceiverObject_t {
    const uint offsetsAndSize[12];
    char stringdata0[40];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ReceiverObject_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ReceiverObject_t qt_meta_stringdata_ReceiverObject = {
    {
QT_MOC_LITERAL(0, 14), // "ReceiverObject"
QT_MOC_LITERAL(15, 5), // "slot1"
QT_MOC_LITERAL(21, 0), // ""
QT_MOC_LITERAL(22, 5), // "slot2"
QT_MOC_LITERAL(28, 5), // "slot3"
QT_MOC_LITERAL(34, 5) // "slot4"

    },
    "ReceiverObject\0slot1\0\0slot2\0slot3\0"
    "slot4"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ReceiverObject[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x0a,    1 /* Public */,
       3,    0,   39,    2, 0x0a,    2 /* Public */,
       4,    0,   40,    2, 0x0a,    3 /* Public */,
       5,    0,   41,    2, 0x0a,    4 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ReceiverObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ReceiverObject *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->slot1(); break;
        case 1: _t->slot2(); break;
        case 2: _t->slot3(); break;
        case 3: _t->slot4(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject ReceiverObject::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ReceiverObject.offsetsAndSize,
    qt_meta_data_ReceiverObject,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ReceiverObject_t
, QtPrivate::TypeAndForceComplete<ReceiverObject, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *ReceiverObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ReceiverObject::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ReceiverObject.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ReceiverObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
