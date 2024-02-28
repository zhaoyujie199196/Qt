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
struct qt_meta_stringdata_DefaultArguments_t {
    const uint offsetsAndSize[28];
    char stringdata0[132];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_DefaultArguments_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_DefaultArguments_t qt_meta_stringdata_DefaultArguments = {
    {
QT_MOC_LITERAL(0, 16), // "DefaultArguments"
QT_MOC_LITERAL(17, 17), // "theOriginalSignal"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 15), // "theSecondSignal"
QT_MOC_LITERAL(52, 1), // "s"
QT_MOC_LITERAL(54, 14), // "theThirdSignal"
QT_MOC_LITERAL(69, 1), // "a"
QT_MOC_LITERAL(71, 1), // "b"
QT_MOC_LITERAL(73, 15), // "theFourthSignal"
QT_MOC_LITERAL(89, 8), // "QString&"
QT_MOC_LITERAL(98, 14), // "theFifthSignal"
QT_MOC_LITERAL(113, 8), // "QString*"
QT_MOC_LITERAL(122, 1), // "c"
QT_MOC_LITERAL(124, 7) // "theSlot"

    },
    "DefaultArguments\0theOriginalSignal\0\0"
    "theSecondSignal\0s\0theThirdSignal\0a\0b\0"
    "theFourthSignal\0QString&\0theFifthSignal\0"
    "QString*\0c\0theSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DefaultArguments[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    1 /* Public */,
       3,    1,   75,    2, 0x06,    2 /* Public */,
       3,    0,   78,    2, 0x26,    4 /* Public | MethodCloned */,
       5,    3,   79,    2, 0x06,    5 /* Public */,
       5,    2,   86,    2, 0x26,    9 /* Public | MethodCloned */,
       5,    1,   91,    2, 0x26,   12 /* Public | MethodCloned */,
       5,    0,   94,    2, 0x26,   14 /* Public | MethodCloned */,
       8,    1,   95,    2, 0x06,   15 /* Public */,
      10,    3,   98,    2, 0x06,   17 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      13,    1,  105,    2, 0x0a,   21 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Float, QMetaType::QString,    6,    7,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Float,    6,    7,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,    4,
    QMetaType::Void, 0x80000000 | 11, 0x80000000 | 9, QMetaType::QString,    6,    7,   12,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,

       0        // eod
};

void DefaultArguments::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DefaultArguments *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->theOriginalSignal(); break;
        case 1: _t->theSecondSignal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->theSecondSignal(); break;
        case 3: _t->theThirdSignal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->theThirdSignal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 5: _t->theThirdSignal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->theThirdSignal(); break;
        case 7: _t->theFourthSignal((*reinterpret_cast< std::add_pointer_t<QString&>>(_a[1]))); break;
        case 8: _t->theFifthSignal((*reinterpret_cast< std::add_pointer_t<QString*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString&>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 9: _t->theSlot((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DefaultArguments::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DefaultArguments::theOriginalSignal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DefaultArguments::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DefaultArguments::theSecondSignal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DefaultArguments::*)(int , float , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DefaultArguments::theThirdSignal)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DefaultArguments::*)(QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DefaultArguments::theFourthSignal)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (DefaultArguments::*)(QString * , QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DefaultArguments::theFifthSignal)) {
                *result = 8;
                return;
            }
        }
    }
}

const QMetaObject DefaultArguments::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DefaultArguments.offsetsAndSize,
    qt_meta_data_DefaultArguments,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_DefaultArguments_t
, QtPrivate::TypeAndForceComplete<DefaultArguments, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<float, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<float, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString *, std::false_type>, QtPrivate::TypeAndForceComplete<QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>


>,
    nullptr
} };


const QMetaObject *DefaultArguments::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DefaultArguments::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DefaultArguments.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DefaultArguments::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void DefaultArguments::theOriginalSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DefaultArguments::theSecondSignal(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 3
void DefaultArguments::theThirdSignal(int _t1, float _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 7
void DefaultArguments::theFourthSignal(QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void DefaultArguments::theFifthSignal(QString * _t1, QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
struct qt_meta_stringdata_NormalizeObject_t {
    const uint offsetsAndSize[172];
    char stringdata0[1471];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_NormalizeObject_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_NormalizeObject_t qt_meta_stringdata_NormalizeObject = {
    {
QT_MOC_LITERAL(0, 15), // "NormalizeObject"
QT_MOC_LITERAL(16, 17), // "uintPointerSignal"
QT_MOC_LITERAL(34, 0), // ""
QT_MOC_LITERAL(35, 5), // "uint*"
QT_MOC_LITERAL(41, 18), // "ulongPointerSignal"
QT_MOC_LITERAL(60, 6), // "ulong*"
QT_MOC_LITERAL(67, 22), // "constUintPointerSignal"
QT_MOC_LITERAL(90, 11), // "const uint*"
QT_MOC_LITERAL(102, 23), // "constUlongPointerSignal"
QT_MOC_LITERAL(126, 12), // "const ulong*"
QT_MOC_LITERAL(139, 12), // "structSignal"
QT_MOC_LITERAL(152, 6), // "Struct"
QT_MOC_LITERAL(159, 1), // "s"
QT_MOC_LITERAL(161, 11), // "classSignal"
QT_MOC_LITERAL(173, 5), // "Class"
QT_MOC_LITERAL(179, 1), // "c"
QT_MOC_LITERAL(181, 10), // "enumSignal"
QT_MOC_LITERAL(192, 4), // "Enum"
QT_MOC_LITERAL(197, 1), // "e"
QT_MOC_LITERAL(199, 19), // "structPointerSignal"
QT_MOC_LITERAL(219, 7), // "Struct*"
QT_MOC_LITERAL(227, 18), // "classPointerSignal"
QT_MOC_LITERAL(246, 6), // "Class*"
QT_MOC_LITERAL(253, 17), // "enumPointerSignal"
QT_MOC_LITERAL(271, 5), // "Enum*"
QT_MOC_LITERAL(277, 24), // "constStructPointerSignal"
QT_MOC_LITERAL(302, 13), // "const Struct*"
QT_MOC_LITERAL(316, 23), // "constClassPointerSignal"
QT_MOC_LITERAL(340, 12), // "const Class*"
QT_MOC_LITERAL(353, 22), // "constEnumPointerSignal"
QT_MOC_LITERAL(376, 11), // "const Enum*"
QT_MOC_LITERAL(388, 36), // "constStructPointerConstPointe..."
QT_MOC_LITERAL(425, 19), // "const Struct*const*"
QT_MOC_LITERAL(445, 35), // "constClassPointerConstPointer..."
QT_MOC_LITERAL(481, 18), // "const Class*const*"
QT_MOC_LITERAL(500, 34), // "constEnumPointerConstPointerS..."
QT_MOC_LITERAL(535, 17), // "const Enum*const*"
QT_MOC_LITERAL(553, 17), // "unsignedintSignal"
QT_MOC_LITERAL(571, 14), // "unsignedSignal"
QT_MOC_LITERAL(586, 18), // "unsignedlongSignal"
QT_MOC_LITERAL(605, 22), // "unsignedlonglongSignal"
QT_MOC_LITERAL(628, 21), // "unsignedlongintSignal"
QT_MOC_LITERAL(650, 19), // "unsignedshortSignal"
QT_MOC_LITERAL(670, 18), // "unsignedcharSignal"
QT_MOC_LITERAL(689, 13), // "typeRefSignal"
QT_MOC_LITERAL(703, 17), // "Template<Class&>&"
QT_MOC_LITERAL(721, 3), // "ref"
QT_MOC_LITERAL(725, 18), // "constTypeRefSignal"
QT_MOC_LITERAL(744, 22), // "Template<const Class&>"
QT_MOC_LITERAL(767, 18), // "typeConstRefSignal"
QT_MOC_LITERAL(786, 25), // "typePointerConstRefSignal"
QT_MOC_LITERAL(812, 20), // "constTemplateSignal1"
QT_MOC_LITERAL(833, 13), // "Template<int>"
QT_MOC_LITERAL(847, 20), // "constTemplateSignal2"
QT_MOC_LITERAL(868, 19), // "Template<const int>"
QT_MOC_LITERAL(888, 15), // "uintPointerSlot"
QT_MOC_LITERAL(904, 16), // "ulongPointerSlot"
QT_MOC_LITERAL(921, 20), // "constUintPointerSlot"
QT_MOC_LITERAL(942, 21), // "constUlongPointerSlot"
QT_MOC_LITERAL(964, 10), // "structSlot"
QT_MOC_LITERAL(975, 9), // "classSlot"
QT_MOC_LITERAL(985, 8), // "enumSlot"
QT_MOC_LITERAL(994, 17), // "structPointerSlot"
QT_MOC_LITERAL(1012, 16), // "classPointerSlot"
QT_MOC_LITERAL(1029, 15), // "enumPointerSlot"
QT_MOC_LITERAL(1045, 22), // "constStructPointerSlot"
QT_MOC_LITERAL(1068, 21), // "constClassPointerSlot"
QT_MOC_LITERAL(1090, 20), // "constEnumPointerSlot"
QT_MOC_LITERAL(1111, 34), // "constStructPointerConstPointe..."
QT_MOC_LITERAL(1146, 33), // "constClassPointerConstPointer..."
QT_MOC_LITERAL(1180, 32), // "constEnumPointerConstPointerSlot"
QT_MOC_LITERAL(1213, 8), // "uintSlot"
QT_MOC_LITERAL(1222, 15), // "unsignedintSlot"
QT_MOC_LITERAL(1238, 12), // "unsignedSlot"
QT_MOC_LITERAL(1251, 16), // "unsignedlongSlot"
QT_MOC_LITERAL(1268, 20), // "unsignedlonglongSlot"
QT_MOC_LITERAL(1289, 19), // "unsignedlongintSlot"
QT_MOC_LITERAL(1309, 17), // "unsignedshortSlot"
QT_MOC_LITERAL(1327, 16), // "unsignedcharSlot"
QT_MOC_LITERAL(1344, 11), // "typeRefSlot"
QT_MOC_LITERAL(1356, 16), // "constTypeRefSlot"
QT_MOC_LITERAL(1373, 16), // "typeConstRefSlot"
QT_MOC_LITERAL(1390, 23), // "typePointerConstRefSlot"
QT_MOC_LITERAL(1414, 18), // "constTemplateSlot1"
QT_MOC_LITERAL(1433, 18), // "constTemplateSlot2"
QT_MOC_LITERAL(1452, 18) // "constTemplateSlot3"

    },
    "NormalizeObject\0uintPointerSignal\0\0"
    "uint*\0ulongPointerSignal\0ulong*\0"
    "constUintPointerSignal\0const uint*\0"
    "constUlongPointerSignal\0const ulong*\0"
    "structSignal\0Struct\0s\0classSignal\0"
    "Class\0c\0enumSignal\0Enum\0e\0structPointerSignal\0"
    "Struct*\0classPointerSignal\0Class*\0"
    "enumPointerSignal\0Enum*\0"
    "constStructPointerSignal\0const Struct*\0"
    "constClassPointerSignal\0const Class*\0"
    "constEnumPointerSignal\0const Enum*\0"
    "constStructPointerConstPointerSignal\0"
    "const Struct*const*\0"
    "constClassPointerConstPointerSignal\0"
    "const Class*const*\0"
    "constEnumPointerConstPointerSignal\0"
    "const Enum*const*\0unsignedintSignal\0"
    "unsignedSignal\0unsignedlongSignal\0"
    "unsignedlonglongSignal\0unsignedlongintSignal\0"
    "unsignedshortSignal\0unsignedcharSignal\0"
    "typeRefSignal\0Template<Class&>&\0ref\0"
    "constTypeRefSignal\0Template<const Class&>\0"
    "typeConstRefSignal\0typePointerConstRefSignal\0"
    "constTemplateSignal1\0Template<int>\0"
    "constTemplateSignal2\0Template<const int>\0"
    "uintPointerSlot\0ulongPointerSlot\0"
    "constUintPointerSlot\0constUlongPointerSlot\0"
    "structSlot\0classSlot\0enumSlot\0"
    "structPointerSlot\0classPointerSlot\0"
    "enumPointerSlot\0constStructPointerSlot\0"
    "constClassPointerSlot\0constEnumPointerSlot\0"
    "constStructPointerConstPointerSlot\0"
    "constClassPointerConstPointerSlot\0"
    "constEnumPointerConstPointerSlot\0"
    "uintSlot\0unsignedintSlot\0unsignedSlot\0"
    "unsignedlongSlot\0unsignedlonglongSlot\0"
    "unsignedlongintSlot\0unsignedshortSlot\0"
    "unsignedcharSlot\0typeRefSlot\0"
    "constTypeRefSlot\0typeConstRefSlot\0"
    "typePointerConstRefSlot\0constTemplateSlot1\0"
    "constTemplateSlot2\0constTemplateSlot3"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NormalizeObject[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      60,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      29,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  374,    2, 0x06,    1 /* Public */,
       4,    1,  377,    2, 0x06,    3 /* Public */,
       6,    1,  380,    2, 0x06,    5 /* Public */,
       8,    1,  383,    2, 0x06,    7 /* Public */,
      10,    1,  386,    2, 0x06,    9 /* Public */,
      13,    1,  389,    2, 0x06,   11 /* Public */,
      16,    1,  392,    2, 0x06,   13 /* Public */,
      19,    1,  395,    2, 0x06,   15 /* Public */,
      21,    1,  398,    2, 0x06,   17 /* Public */,
      23,    1,  401,    2, 0x06,   19 /* Public */,
      25,    1,  404,    2, 0x06,   21 /* Public */,
      27,    1,  407,    2, 0x06,   23 /* Public */,
      29,    1,  410,    2, 0x06,   25 /* Public */,
      31,    1,  413,    2, 0x06,   27 /* Public */,
      33,    1,  416,    2, 0x06,   29 /* Public */,
      35,    1,  419,    2, 0x06,   31 /* Public */,
      37,    1,  422,    2, 0x06,   33 /* Public */,
      38,    1,  425,    2, 0x06,   35 /* Public */,
      39,    1,  428,    2, 0x06,   37 /* Public */,
      40,    1,  431,    2, 0x06,   39 /* Public */,
      41,    1,  434,    2, 0x06,   41 /* Public */,
      42,    1,  437,    2, 0x06,   43 /* Public */,
      43,    1,  440,    2, 0x06,   45 /* Public */,
      44,    1,  443,    2, 0x06,   47 /* Public */,
      47,    1,  446,    2, 0x06,   49 /* Public */,
      49,    1,  449,    2, 0x06,   51 /* Public */,
      50,    1,  452,    2, 0x06,   53 /* Public */,
      51,    1,  455,    2, 0x06,   55 /* Public */,
      53,    1,  458,    2, 0x06,   57 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      55,    1,  461,    2, 0x0a,   59 /* Public */,
      56,    1,  464,    2, 0x0a,   61 /* Public */,
      57,    1,  467,    2, 0x0a,   63 /* Public */,
      58,    1,  470,    2, 0x0a,   65 /* Public */,
      59,    1,  473,    2, 0x0a,   67 /* Public */,
      60,    1,  476,    2, 0x0a,   69 /* Public */,
      61,    1,  479,    2, 0x0a,   71 /* Public */,
      62,    1,  482,    2, 0x0a,   73 /* Public */,
      63,    1,  485,    2, 0x0a,   75 /* Public */,
      64,    1,  488,    2, 0x0a,   77 /* Public */,
      65,    1,  491,    2, 0x0a,   79 /* Public */,
      66,    1,  494,    2, 0x0a,   81 /* Public */,
      67,    1,  497,    2, 0x0a,   83 /* Public */,
      68,    1,  500,    2, 0x0a,   85 /* Public */,
      69,    1,  503,    2, 0x0a,   87 /* Public */,
      70,    1,  506,    2, 0x0a,   89 /* Public */,
      71,    1,  509,    2, 0x0a,   91 /* Public */,
      72,    1,  512,    2, 0x0a,   93 /* Public */,
      73,    1,  515,    2, 0x0a,   95 /* Public */,
      74,    1,  518,    2, 0x0a,   97 /* Public */,
      75,    1,  521,    2, 0x0a,   99 /* Public */,
      76,    1,  524,    2, 0x0a,  101 /* Public */,
      77,    1,  527,    2, 0x0a,  103 /* Public */,
      78,    1,  530,    2, 0x0a,  105 /* Public */,
      79,    1,  533,    2, 0x0a,  107 /* Public */,
      80,    1,  536,    2, 0x0a,  109 /* Public */,
      81,    1,  539,    2, 0x0a,  111 /* Public */,
      82,    1,  542,    2, 0x0a,  113 /* Public */,
      83,    1,  545,    2, 0x0a,  115 /* Public */,
      84,    1,  548,    2, 0x0a,  117 /* Public */,
      85,    1,  551,    2, 0x0a,  119 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 20,   12,
    QMetaType::Void, 0x80000000 | 22,   15,
    QMetaType::Void, 0x80000000 | 24,   18,
    QMetaType::Void, 0x80000000 | 26,   12,
    QMetaType::Void, 0x80000000 | 28,   15,
    QMetaType::Void, 0x80000000 | 30,   18,
    QMetaType::Void, 0x80000000 | 32,   12,
    QMetaType::Void, 0x80000000 | 34,   15,
    QMetaType::Void, 0x80000000 | 36,   18,
    QMetaType::Void, QMetaType::UInt,    2,
    QMetaType::Void, QMetaType::UInt,    2,
    QMetaType::Void, QMetaType::ULong,    2,
    QMetaType::Void, QMetaType::ULongLong,    2,
    QMetaType::Void, QMetaType::ULong,    2,
    QMetaType::Void, QMetaType::UShort,    2,
    QMetaType::Void, QMetaType::UChar,    2,
    QMetaType::Void, 0x80000000 | 45,   46,
    QMetaType::Void, 0x80000000 | 48,   46,
    QMetaType::Void, 0x80000000 | 48,   46,
    QMetaType::Void, 0x80000000 | 22,    2,
    QMetaType::Void, 0x80000000 | 52,    2,
    QMetaType::Void, 0x80000000 | 54,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 20,   12,
    QMetaType::Void, 0x80000000 | 22,   15,
    QMetaType::Void, 0x80000000 | 24,   18,
    QMetaType::Void, 0x80000000 | 26,   12,
    QMetaType::Void, 0x80000000 | 28,   15,
    QMetaType::Void, 0x80000000 | 30,   18,
    QMetaType::Void, 0x80000000 | 32,   12,
    QMetaType::Void, 0x80000000 | 34,   15,
    QMetaType::Void, 0x80000000 | 36,   18,
    QMetaType::Void, QMetaType::UInt,    2,
    QMetaType::Void, QMetaType::UInt,    2,
    QMetaType::Void, QMetaType::UInt,    2,
    QMetaType::Void, QMetaType::ULong,    2,
    QMetaType::Void, QMetaType::ULongLong,    2,
    QMetaType::Void, QMetaType::ULong,    2,
    QMetaType::Void, QMetaType::UShort,    2,
    QMetaType::Void, QMetaType::UChar,    2,
    QMetaType::Void, 0x80000000 | 45,    2,
    QMetaType::Void, 0x80000000 | 48,    2,
    QMetaType::Void, 0x80000000 | 48,    2,
    QMetaType::Void, 0x80000000 | 22,    2,
    QMetaType::Void, 0x80000000 | 52,    2,
    QMetaType::Void, 0x80000000 | 52,    2,
    QMetaType::Void, 0x80000000 | 54,    2,

       0        // eod
};

void NormalizeObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NormalizeObject *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->uintPointerSignal((*reinterpret_cast< std::add_pointer_t<uint*>>(_a[1]))); break;
        case 1: _t->ulongPointerSignal((*reinterpret_cast< std::add_pointer_t<ulong*>>(_a[1]))); break;
        case 2: _t->constUintPointerSignal((*reinterpret_cast< std::add_pointer_t<const uint*>>(_a[1]))); break;
        case 3: _t->constUlongPointerSignal((*reinterpret_cast< std::add_pointer_t<const ulong*>>(_a[1]))); break;
        case 4: _t->structSignal((*reinterpret_cast< std::add_pointer_t<Struct>>(_a[1]))); break;
        case 5: _t->classSignal((*reinterpret_cast< std::add_pointer_t<Class>>(_a[1]))); break;
        case 6: _t->enumSignal((*reinterpret_cast< std::add_pointer_t<Enum>>(_a[1]))); break;
        case 7: _t->structPointerSignal((*reinterpret_cast< std::add_pointer_t<Struct*>>(_a[1]))); break;
        case 8: _t->classPointerSignal((*reinterpret_cast< std::add_pointer_t<Class*>>(_a[1]))); break;
        case 9: _t->enumPointerSignal((*reinterpret_cast< std::add_pointer_t<Enum*>>(_a[1]))); break;
        case 10: _t->constStructPointerSignal((*reinterpret_cast< std::add_pointer_t<const Struct*>>(_a[1]))); break;
        case 11: _t->constClassPointerSignal((*reinterpret_cast< std::add_pointer_t<const Class*>>(_a[1]))); break;
        case 12: _t->constEnumPointerSignal((*reinterpret_cast< std::add_pointer_t<const Enum*>>(_a[1]))); break;
        case 13: _t->constStructPointerConstPointerSignal((*reinterpret_cast< std::add_pointer_t<const Struct*const*>>(_a[1]))); break;
        case 14: _t->constClassPointerConstPointerSignal((*reinterpret_cast< std::add_pointer_t<const Class*const*>>(_a[1]))); break;
        case 15: _t->constEnumPointerConstPointerSignal((*reinterpret_cast< std::add_pointer_t<const Enum*const*>>(_a[1]))); break;
        case 16: _t->unsignedintSignal((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1]))); break;
        case 17: _t->unsignedSignal((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1]))); break;
        case 18: _t->unsignedlongSignal((*reinterpret_cast< std::add_pointer_t<ulong>>(_a[1]))); break;
        case 19: _t->unsignedlonglongSignal((*reinterpret_cast< std::add_pointer_t<quint64>>(_a[1]))); break;
        case 20: _t->unsignedlongintSignal((*reinterpret_cast< std::add_pointer_t<ulong>>(_a[1]))); break;
        case 21: _t->unsignedshortSignal((*reinterpret_cast< std::add_pointer_t<ushort>>(_a[1]))); break;
        case 22: _t->unsignedcharSignal((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1]))); break;
        case 23: _t->typeRefSignal((*reinterpret_cast< std::add_pointer_t<Template<Class&>&>>(_a[1]))); break;
        case 24: _t->constTypeRefSignal((*reinterpret_cast< std::add_pointer_t<Template<const Class&>>>(_a[1]))); break;
        case 25: _t->typeConstRefSignal((*reinterpret_cast< std::add_pointer_t<Template<const Class&>>>(_a[1]))); break;
        case 26: _t->typePointerConstRefSignal((*reinterpret_cast< std::add_pointer_t<Class*>>(_a[1]))); break;
        case 27: _t->constTemplateSignal1((*reinterpret_cast< std::add_pointer_t<Template<int>>>(_a[1]))); break;
        case 28: _t->constTemplateSignal2((*reinterpret_cast< std::add_pointer_t<Template<const int>>>(_a[1]))); break;
        case 29: _t->uintPointerSlot((*reinterpret_cast< std::add_pointer_t<uint*>>(_a[1]))); break;
        case 30: _t->ulongPointerSlot((*reinterpret_cast< std::add_pointer_t<ulong*>>(_a[1]))); break;
        case 31: _t->constUintPointerSlot((*reinterpret_cast< std::add_pointer_t<const uint*>>(_a[1]))); break;
        case 32: _t->constUlongPointerSlot((*reinterpret_cast< std::add_pointer_t<const ulong*>>(_a[1]))); break;
        case 33: _t->structSlot((*reinterpret_cast< std::add_pointer_t<Struct>>(_a[1]))); break;
        case 34: _t->classSlot((*reinterpret_cast< std::add_pointer_t<Class>>(_a[1]))); break;
        case 35: _t->enumSlot((*reinterpret_cast< std::add_pointer_t<Enum>>(_a[1]))); break;
        case 36: _t->structPointerSlot((*reinterpret_cast< std::add_pointer_t<Struct*>>(_a[1]))); break;
        case 37: _t->classPointerSlot((*reinterpret_cast< std::add_pointer_t<Class*>>(_a[1]))); break;
        case 38: _t->enumPointerSlot((*reinterpret_cast< std::add_pointer_t<Enum*>>(_a[1]))); break;
        case 39: _t->constStructPointerSlot((*reinterpret_cast< std::add_pointer_t<const Struct*>>(_a[1]))); break;
        case 40: _t->constClassPointerSlot((*reinterpret_cast< std::add_pointer_t<const Class*>>(_a[1]))); break;
        case 41: _t->constEnumPointerSlot((*reinterpret_cast< std::add_pointer_t<const Enum*>>(_a[1]))); break;
        case 42: _t->constStructPointerConstPointerSlot((*reinterpret_cast< std::add_pointer_t<const Struct*const*>>(_a[1]))); break;
        case 43: _t->constClassPointerConstPointerSlot((*reinterpret_cast< std::add_pointer_t<const Class*const*>>(_a[1]))); break;
        case 44: _t->constEnumPointerConstPointerSlot((*reinterpret_cast< std::add_pointer_t<const Enum*const*>>(_a[1]))); break;
        case 45: _t->uintSlot((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1]))); break;
        case 46: _t->unsignedintSlot((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1]))); break;
        case 47: _t->unsignedSlot((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1]))); break;
        case 48: _t->unsignedlongSlot((*reinterpret_cast< std::add_pointer_t<ulong>>(_a[1]))); break;
        case 49: _t->unsignedlonglongSlot((*reinterpret_cast< std::add_pointer_t<quint64>>(_a[1]))); break;
        case 50: _t->unsignedlongintSlot((*reinterpret_cast< std::add_pointer_t<ulong>>(_a[1]))); break;
        case 51: _t->unsignedshortSlot((*reinterpret_cast< std::add_pointer_t<ushort>>(_a[1]))); break;
        case 52: _t->unsignedcharSlot((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1]))); break;
        case 53: _t->typeRefSlot((*reinterpret_cast< std::add_pointer_t<Template<Class&>&>>(_a[1]))); break;
        case 54: _t->constTypeRefSlot((*reinterpret_cast< std::add_pointer_t<Template<const Class&>>>(_a[1]))); break;
        case 55: _t->typeConstRefSlot((*reinterpret_cast< std::add_pointer_t<Template<const Class&>>>(_a[1]))); break;
        case 56: _t->typePointerConstRefSlot((*reinterpret_cast< std::add_pointer_t<Class*>>(_a[1]))); break;
        case 57: _t->constTemplateSlot1((*reinterpret_cast< std::add_pointer_t<Template<int>>>(_a[1]))); break;
        case 58: _t->constTemplateSlot2((*reinterpret_cast< std::add_pointer_t<Template<int>>>(_a[1]))); break;
        case 59: _t->constTemplateSlot3((*reinterpret_cast< std::add_pointer_t<Template<const int>>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NormalizeObject::*)(uint * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::uintPointerSignal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(ulong * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::ulongPointerSignal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const uint * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constUintPointerSignal)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const ulong * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constUlongPointerSignal)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Struct );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::structSignal)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Class );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::classSignal)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Enum );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::enumSignal)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Struct * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::structPointerSignal)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Class * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::classPointerSignal)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Enum * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::enumPointerSignal)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const Struct * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constStructPointerSignal)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const Class * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constClassPointerSignal)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const Enum * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constEnumPointerSignal)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const Struct * const * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constStructPointerConstPointerSignal)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const Class * const * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constClassPointerConstPointerSignal)) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const Enum * const * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constEnumPointerConstPointerSignal)) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::unsignedintSignal)) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(unsigned  );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::unsignedSignal)) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(unsigned long );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::unsignedlongSignal)) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(quint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::unsignedlonglongSignal)) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(unsigned long int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::unsignedlongintSignal)) {
                *result = 20;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(unsigned short );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::unsignedshortSignal)) {
                *result = 21;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(unsigned char );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::unsignedcharSignal)) {
                *result = 22;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Template<Class&> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::typeRefSignal)) {
                *result = 23;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(const Template<Class const&> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constTypeRefSignal)) {
                *result = 24;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Template<Class const&> const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::typeConstRefSignal)) {
                *result = 25;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Class * const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::typePointerConstRefSignal)) {
                *result = 26;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Template<int> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constTemplateSignal1)) {
                *result = 27;
                return;
            }
        }
        {
            using _t = void (NormalizeObject::*)(Template<const int> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NormalizeObject::constTemplateSignal2)) {
                *result = 28;
                return;
            }
        }
    }
}

const QMetaObject NormalizeObject::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_NormalizeObject.offsetsAndSize,
    qt_meta_data_NormalizeObject,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_NormalizeObject_t
, QtPrivate::TypeAndForceComplete<NormalizeObject, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<uint *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<ulong *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const uint *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const ulong *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Struct, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Class, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Enum, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Struct *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Class *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Enum *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Struct *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Class *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Enum *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Struct * const *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Class * const *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Enum * const *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned , std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned long, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<quint64, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned long int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned short, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Template<Class&> &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Template<Class const&> &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Template<Class const&> const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Class * const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Template<int>, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Template<const int>, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<uint *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<ulong *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const uint *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const ulong *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Struct, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Class, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Enum, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Struct *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Class *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Enum *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Struct *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Class *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Enum *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Struct * const *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Class * const *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Enum * const *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<uint, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned , std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned long, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<quint64, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned long int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned short, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Template<Class&> &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Template<const Class&> &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Template<Class const&> const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Class * const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Template<int> const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Template<int>, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const Template<const int>, std::false_type>


>,
    nullptr
} };


const QMetaObject *NormalizeObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NormalizeObject::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NormalizeObject.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NormalizeObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 60)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 60;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 60)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 60;
    }
    return _id;
}

// SIGNAL 0
void NormalizeObject::uintPointerSignal(uint * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NormalizeObject::ulongPointerSignal(ulong * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NormalizeObject::constUintPointerSignal(const uint * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NormalizeObject::constUlongPointerSignal(const ulong * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NormalizeObject::structSignal(Struct _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NormalizeObject::classSignal(Class _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void NormalizeObject::enumSignal(Enum _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void NormalizeObject::structPointerSignal(Struct * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void NormalizeObject::classPointerSignal(Class * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void NormalizeObject::enumPointerSignal(Enum * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void NormalizeObject::constStructPointerSignal(const Struct * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void NormalizeObject::constClassPointerSignal(const Class * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void NormalizeObject::constEnumPointerSignal(const Enum * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void NormalizeObject::constStructPointerConstPointerSignal(const Struct * const * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void NormalizeObject::constClassPointerConstPointerSignal(const Class * const * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void NormalizeObject::constEnumPointerConstPointerSignal(const Enum * const * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void NormalizeObject::unsignedintSignal(unsigned int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void NormalizeObject::unsignedSignal(unsigned  _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void NormalizeObject::unsignedlongSignal(unsigned long _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void NormalizeObject::unsignedlonglongSignal(quint64 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void NormalizeObject::unsignedlongintSignal(unsigned long int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void NormalizeObject::unsignedshortSignal(unsigned short _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void NormalizeObject::unsignedcharSignal(unsigned char _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 23
void NormalizeObject::typeRefSignal(Template<Class&> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void NormalizeObject::constTypeRefSignal(const Template<Class const&> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 25
void NormalizeObject::typeConstRefSignal(Template<Class const&> const & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 25, _a);
}

// SIGNAL 26
void NormalizeObject::typePointerConstRefSignal(Class * const & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 27
void NormalizeObject::constTemplateSignal1(Template<int> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 27, _a);
}

// SIGNAL 28
void NormalizeObject::constTemplateSignal2(Template<const int> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
