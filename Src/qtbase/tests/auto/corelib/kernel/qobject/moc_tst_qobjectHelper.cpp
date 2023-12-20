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
struct qt_meta_stringdata_AutoConnectSender_t {
    const uint offsetsAndSize[26];
    char stringdata0[125];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_AutoConnectSender_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_AutoConnectSender_t qt_meta_stringdata_AutoConnectSender = {
    {
QT_MOC_LITERAL(0, 17), // "AutoConnectSender"
QT_MOC_LITERAL(18, 14), // "signalNoParams"
QT_MOC_LITERAL(33, 0), // ""
QT_MOC_LITERAL(34, 16), // "signalWithParams"
QT_MOC_LITERAL(51, 1), // "i"
QT_MOC_LITERAL(53, 6), // "string"
QT_MOC_LITERAL(60, 16), // "signalManyParams"
QT_MOC_LITERAL(77, 2), // "i1"
QT_MOC_LITERAL(80, 2), // "i2"
QT_MOC_LITERAL(83, 2), // "i3"
QT_MOC_LITERAL(86, 5), // "onoff"
QT_MOC_LITERAL(92, 17), // "signalManyParams2"
QT_MOC_LITERAL(110, 14) // "signalLoopBack"

    },
    "AutoConnectSender\0signalNoParams\0\0"
    "signalWithParams\0i\0string\0signalManyParams\0"
    "i1\0i2\0i3\0onoff\0signalManyParams2\0"
    "signalLoopBack"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AutoConnectSender[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x06,    1 /* Public */,
       3,    1,   57,    2, 0x06,    2 /* Public */,
       3,    2,   60,    2, 0x06,    4 /* Public */,
       6,    5,   65,    2, 0x06,    7 /* Public */,
       6,    6,   76,    2, 0x06,   13 /* Public */,
      11,    5,   89,    2, 0x06,   20 /* Public */,
      12,    0,  100,    2, 0x06,   26 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    4,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Bool,    7,    8,    9,    5,   10,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    7,    8,    9,    5,   10,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Bool,    7,    8,    9,    5,   10,
    QMetaType::Void,

       0        // eod
};

void AutoConnectSender::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AutoConnectSender *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signalNoParams(); break;
        case 1: _t->signalWithParams((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->signalWithParams((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->signalManyParams((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5]))); break;
        case 4: _t->signalManyParams((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[6]))); break;
        case 5: _t->signalManyParams2((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5]))); break;
        case 6: _t->signalLoopBack(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AutoConnectSender::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectSender::signalNoParams)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AutoConnectSender::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectSender::signalWithParams)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AutoConnectSender::*)(int , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectSender::signalWithParams)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AutoConnectSender::*)(int , int , int , QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectSender::signalManyParams)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (AutoConnectSender::*)(int , int , int , QString , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectSender::signalManyParams)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (AutoConnectSender::*)(int , int , int , QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectSender::signalManyParams2)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (AutoConnectSender::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectSender::signalLoopBack)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject AutoConnectSender::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AutoConnectSender.offsetsAndSize,
    qt_meta_data_AutoConnectSender,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_AutoConnectSender_t
, QtPrivate::TypeAndForceComplete<AutoConnectSender, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *AutoConnectSender::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AutoConnectSender::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AutoConnectSender.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AutoConnectSender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void AutoConnectSender::signalNoParams()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AutoConnectSender::signalWithParams(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AutoConnectSender::signalWithParams(int _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AutoConnectSender::signalManyParams(int _t1, int _t2, int _t3, QString _t4, bool _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void AutoConnectSender::signalManyParams(int _t1, int _t2, int _t3, QString _t4, bool _t5, bool _t6)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void AutoConnectSender::signalManyParams2(int _t1, int _t2, int _t3, QString _t4, bool _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void AutoConnectSender::signalLoopBack()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
struct qt_meta_stringdata_AutoConnectReceiver_t {
    const uint offsetsAndSize[34];
    char stringdata0[304];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_AutoConnectReceiver_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_AutoConnectReceiver_t qt_meta_stringdata_AutoConnectReceiver = {
    {
QT_MOC_LITERAL(0, 19), // "AutoConnectReceiver"
QT_MOC_LITERAL(20, 24), // "on_Sender_signalLoopBack"
QT_MOC_LITERAL(45, 0), // ""
QT_MOC_LITERAL(46, 14), // "signalNoParams"
QT_MOC_LITERAL(61, 22), // "signal_with_underscore"
QT_MOC_LITERAL(84, 24), // "on_Sender_signalNoParams"
QT_MOC_LITERAL(109, 26), // "on_Sender_signalWithParams"
QT_MOC_LITERAL(136, 26), // "on_Sender_signalManyParams"
QT_MOC_LITERAL(163, 27), // "on_Sender_signalManyParams2"
QT_MOC_LITERAL(191, 12), // "slotLoopBack"
QT_MOC_LITERAL(204, 26), // "on_Receiver_signalNoParams"
QT_MOC_LITERAL(231, 34), // "on_Receiver_signal_with_under..."
QT_MOC_LITERAL(266, 1), // "o"
QT_MOC_LITERAL(268, 2), // "on"
QT_MOC_LITERAL(271, 3), // "on_"
QT_MOC_LITERAL(275, 12), // "on_something"
QT_MOC_LITERAL(288, 15) // "on_child_signal"

    },
    "AutoConnectReceiver\0on_Sender_signalLoopBack\0"
    "\0signalNoParams\0signal_with_underscore\0"
    "on_Sender_signalNoParams\0"
    "on_Sender_signalWithParams\0"
    "on_Sender_signalManyParams\0"
    "on_Sender_signalManyParams2\0slotLoopBack\0"
    "on_Receiver_signalNoParams\0"
    "on_Receiver_signal_with_underscore\0o\0"
    "on\0on_\0on_something\0on_child_signal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AutoConnectReceiver[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  128,    2, 0x06,    1 /* Public */,
       3,    0,  129,    2, 0x06,    2 /* Public */,
       4,    0,  130,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,  131,    2, 0x0a,    4 /* Public */,
       6,    1,  132,    2, 0x0a,    5 /* Public */,
       6,    0,  135,    2, 0x2a,    7 /* Public | MethodCloned */,
       6,    2,  136,    2, 0x0a,    8 /* Public */,
       7,    0,  141,    2, 0x0a,   11 /* Public */,
       7,    5,  142,    2, 0x0a,   12 /* Public */,
       7,    6,  153,    2, 0x0a,   18 /* Public */,
       8,    5,  166,    2, 0x0a,   25 /* Public */,
       9,    0,  177,    2, 0x0a,   31 /* Public */,
      10,    0,  178,    2, 0x0a,   32 /* Public */,
      11,    0,  179,    2, 0x0a,   33 /* Public */,
      12,    0,  180,    2, 0x09,   34 /* Protected */,
      13,    0,  181,    2, 0x09,   35 /* Protected */,
      14,    0,  182,    2, 0x09,   36 /* Protected */,
      15,    0,  183,    2, 0x09,   37 /* Protected */,
      16,    0,  184,    2, 0x09,   38 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Bool,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    2,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Bool,    2,    2,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AutoConnectReceiver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AutoConnectReceiver *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_Sender_signalLoopBack(); break;
        case 1: _t->signalNoParams(); break;
        case 2: _t->signal_with_underscore(); break;
        case 3: _t->on_Sender_signalNoParams(); break;
        case 4: _t->on_Sender_signalWithParams((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->on_Sender_signalWithParams(); break;
        case 6: _t->on_Sender_signalWithParams((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->on_Sender_signalManyParams(); break;
        case 8: _t->on_Sender_signalManyParams((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5]))); break;
        case 9: _t->on_Sender_signalManyParams((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[6]))); break;
        case 10: _t->on_Sender_signalManyParams2((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5]))); break;
        case 11: _t->slotLoopBack(); break;
        case 12: _t->on_Receiver_signalNoParams(); break;
        case 13: _t->on_Receiver_signal_with_underscore(); break;
        case 14: _t->o(); break;
        case 15: _t->on(); break;
        case 16: _t->on_(); break;
        case 17: _t->on_something(); break;
        case 18: _t->on_child_signal(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AutoConnectReceiver::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectReceiver::on_Sender_signalLoopBack)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AutoConnectReceiver::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectReceiver::signalNoParams)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AutoConnectReceiver::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoConnectReceiver::signal_with_underscore)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject AutoConnectReceiver::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AutoConnectReceiver.offsetsAndSize,
    qt_meta_data_AutoConnectReceiver,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_AutoConnectReceiver_t
, QtPrivate::TypeAndForceComplete<AutoConnectReceiver, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *AutoConnectReceiver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AutoConnectReceiver::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AutoConnectReceiver.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AutoConnectReceiver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void AutoConnectReceiver::on_Sender_signalLoopBack()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AutoConnectReceiver::signalNoParams()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AutoConnectReceiver::signal_with_underscore()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
