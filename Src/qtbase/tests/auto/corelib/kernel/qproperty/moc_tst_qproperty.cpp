/****************************************************************************
** Meta object code from reading C++ file 'tst_qproperty.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "./tst_qproperty.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QProperty>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tst_qproperty.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ChangeDuringDtorTester_t {
    const uint offsetsAndSize[4];
    char stringdata0[28];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ChangeDuringDtorTester_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ChangeDuringDtorTester_t qt_meta_stringdata_ChangeDuringDtorTester = {
    {
QT_MOC_LITERAL(0, 22), // "ChangeDuringDtorTester"
QT_MOC_LITERAL(23, 4) // "prop"

    },
    "ChangeDuringDtorTester\0prop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ChangeDuringDtorTester[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x02015103, uint(-1), 0,

       0        // eod
};

void ChangeDuringDtorTester::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ChangeDuringDtorTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->prop(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ChangeDuringDtorTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setProp(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
        auto *_t = static_cast<ChangeDuringDtorTester *>(_o);
        (void)_t;
        switch (_id) {
        case 0: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp(); break;
        default: break;
        }
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject ChangeDuringDtorTester::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ChangeDuringDtorTester.offsetsAndSize,
    qt_meta_data_ChangeDuringDtorTester,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ChangeDuringDtorTester_t
, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<ChangeDuringDtorTester, std::true_type>



>,
    nullptr
} };


const QMetaObject *ChangeDuringDtorTester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChangeDuringDtorTester::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ChangeDuringDtorTester.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ChangeDuringDtorTester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_BindingLoopTester_t {
    const uint offsetsAndSize[6];
    char stringdata0[39];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_BindingLoopTester_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_BindingLoopTester_t qt_meta_stringdata_BindingLoopTester = {
    {
QT_MOC_LITERAL(0, 17), // "BindingLoopTester"
QT_MOC_LITERAL(18, 9), // "eagerProp"
QT_MOC_LITERAL(28, 10) // "eagerProp2"

    },
    "BindingLoopTester\0eagerProp\0eagerProp2"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BindingLoopTester[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x02015103, uint(-1), 0,
       2, QMetaType::Int, 0x02015103, uint(-1), 0,

       0        // eod
};

void BindingLoopTester::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<BindingLoopTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->eagerProp(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->eagerProp2(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<BindingLoopTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setEagerProp(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setEagerProp2(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
        auto *_t = static_cast<BindingLoopTester *>(_o);
        (void)_t;
        switch (_id) {
        case 0: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableEagerProp(); break;
        case 1: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableEagerProp2(); break;
        default: break;
        }
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject BindingLoopTester::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_BindingLoopTester.offsetsAndSize,
    qt_meta_data_BindingLoopTester,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_BindingLoopTester_t
, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<BindingLoopTester, std::true_type>



>,
    nullptr
} };


const QMetaObject *BindingLoopTester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BindingLoopTester::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BindingLoopTester.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BindingLoopTester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_ReallocTester_t {
    const uint offsetsAndSize[12];
    char stringdata0[44];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ReallocTester_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ReallocTester_t qt_meta_stringdata_ReallocTester = {
    {
QT_MOC_LITERAL(0, 13), // "ReallocTester"
QT_MOC_LITERAL(14, 5), // "prop1"
QT_MOC_LITERAL(20, 5), // "prop2"
QT_MOC_LITERAL(26, 5), // "prop3"
QT_MOC_LITERAL(32, 5), // "prop4"
QT_MOC_LITERAL(38, 5) // "prop5"

    },
    "ReallocTester\0prop1\0prop2\0prop3\0prop4\0"
    "prop5"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ReallocTester[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       5,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x02015103, uint(-1), 0,
       2, QMetaType::Int, 0x02015103, uint(-1), 0,
       3, QMetaType::Int, 0x02015103, uint(-1), 0,
       4, QMetaType::Int, 0x02015103, uint(-1), 0,
       5, QMetaType::Int, 0x02015103, uint(-1), 0,

       0        // eod
};

void ReallocTester::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ReallocTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->prop1(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->prop2(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->prop3(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->prop4(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->prop5(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ReallocTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setProp1(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setProp2(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setProp3(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setProp4(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setProp5(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
        auto *_t = static_cast<ReallocTester *>(_o);
        (void)_t;
        switch (_id) {
        case 0: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp1(); break;
        case 1: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp2(); break;
        case 2: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp3(); break;
        case 3: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp4(); break;
        case 4: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp5(); break;
        default: break;
        }
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject ReallocTester::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ReallocTester.offsetsAndSize,
    qt_meta_data_ReallocTester,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ReallocTester_t
, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<ReallocTester, std::true_type>



>,
    nullptr
} };


const QMetaObject *ReallocTester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ReallocTester::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ReallocTester.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ReallocTester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_MyQObject_t {
    const uint offsetsAndSize[28];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MyQObject_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MyQObject_t qt_meta_stringdata_MyQObject = {
    {
QT_MOC_LITERAL(0, 9), // "MyQObject"
QT_MOC_LITERAL(10, 10), // "fooChanged"
QT_MOC_LITERAL(21, 0), // ""
QT_MOC_LITERAL(22, 6), // "newFoo"
QT_MOC_LITERAL(29, 10), // "barChanged"
QT_MOC_LITERAL(40, 13), // "compatChanged"
QT_MOC_LITERAL(54, 13), // "fooHasChanged"
QT_MOC_LITERAL(68, 13), // "barHasChanged"
QT_MOC_LITERAL(82, 16), // "compatHasChanged"
QT_MOC_LITERAL(99, 3), // "foo"
QT_MOC_LITERAL(103, 3), // "bar"
QT_MOC_LITERAL(107, 4), // "read"
QT_MOC_LITERAL(112, 8), // "computed"
QT_MOC_LITERAL(121, 6) // "compat"

    },
    "MyQObject\0fooChanged\0\0newFoo\0barChanged\0"
    "compatChanged\0fooHasChanged\0barHasChanged\0"
    "compatHasChanged\0foo\0bar\0read\0computed\0"
    "compat"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyQObject[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       5,   58, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    6 /* Public */,
       4,    0,   53,    2, 0x06,    8 /* Public */,
       5,    0,   54,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,   55,    2, 0x0a,   10 /* Public */,
       7,    0,   56,    2, 0x0a,   11 /* Public */,
       8,    0,   57,    2, 0x0a,   12 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       9, QMetaType::Int, 0x02015103, uint(0), 0,
      10, QMetaType::Int, 0x02015103, uint(1), 0,
      11, QMetaType::Int, 0x00015001, uint(-1), 0,
      12, QMetaType::Int, 0x00005001, uint(-1), 0,
      13, QMetaType::Int, 0x00015103, uint(2), 0,

       0        // eod
};

void MyQObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyQObject *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->fooChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->barChanged(); break;
        case 2: _t->compatChanged(); break;
        case 3: _t->fooHasChanged(); break;
        case 4: _t->barHasChanged(); break;
        case 5: _t->compatHasChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MyQObject::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyQObject::fooChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MyQObject::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyQObject::barChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MyQObject::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyQObject::compatChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MyQObject *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->foo(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->bar(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->read(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->computed(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->compat(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<MyQObject *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFoo(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setBar(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setCompat(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
        auto *_t = static_cast<MyQObject *>(_o);
        (void)_t;
        switch (_id) {
        case 0: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableFoo(); break;
        case 1: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableBar(); break;
        default: break;
        }
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject MyQObject::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MyQObject.offsetsAndSize,
    qt_meta_data_MyQObject,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MyQObject_t
, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<MyQObject, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *MyQObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyQObject::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyQObject.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MyQObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MyQObject::fooChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MyQObject::barChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MyQObject::compatChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_FakeDependencyCreator_t {
    const uint offsetsAndSize[16];
    char stringdata0[80];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_FakeDependencyCreator_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_FakeDependencyCreator_t qt_meta_stringdata_FakeDependencyCreator = {
    {
QT_MOC_LITERAL(0, 21), // "FakeDependencyCreator"
QT_MOC_LITERAL(22, 12), // "prop1Changed"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 12), // "prop2Changed"
QT_MOC_LITERAL(49, 12), // "prop3Changed"
QT_MOC_LITERAL(62, 5), // "prop1"
QT_MOC_LITERAL(68, 5), // "prop2"
QT_MOC_LITERAL(74, 5) // "prop3"

    },
    "FakeDependencyCreator\0prop1Changed\0\0"
    "prop2Changed\0prop3Changed\0prop1\0prop2\0"
    "prop3"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FakeDependencyCreator[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       3,   35, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x06,    4 /* Public */,
       3,    0,   33,    2, 0x06,    5 /* Public */,
       4,    0,   34,    2, 0x06,    6 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::Int, 0x02015103, uint(0), 0,
       6, QMetaType::Int, 0x02015103, uint(1), 0,
       7, QMetaType::Int, 0x02015103, uint(2), 0,

       0        // eod
};

void FakeDependencyCreator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FakeDependencyCreator *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->prop1Changed(); break;
        case 1: _t->prop2Changed(); break;
        case 2: _t->prop3Changed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FakeDependencyCreator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FakeDependencyCreator::prop1Changed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FakeDependencyCreator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FakeDependencyCreator::prop2Changed)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FakeDependencyCreator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FakeDependencyCreator::prop3Changed)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<FakeDependencyCreator *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->prop1(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->prop2(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->prop3(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<FakeDependencyCreator *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setProp1(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setProp2(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setProp3(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
        auto *_t = static_cast<FakeDependencyCreator *>(_o);
        (void)_t;
        switch (_id) {
        case 0: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp1(); break;
        case 1: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp2(); break;
        case 2: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp3(); break;
        default: break;
        }
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

const QMetaObject FakeDependencyCreator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FakeDependencyCreator.offsetsAndSize,
    qt_meta_data_FakeDependencyCreator,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_FakeDependencyCreator_t
, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<FakeDependencyCreator, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *FakeDependencyCreator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FakeDependencyCreator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FakeDependencyCreator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FakeDependencyCreator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void FakeDependencyCreator::prop1Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FakeDependencyCreator::prop2Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void FakeDependencyCreator::prop3Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_PropertyAdaptorTester_t {
    const uint offsetsAndSize[30];
    char stringdata0[168];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_PropertyAdaptorTester_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_PropertyAdaptorTester_t qt_meta_stringdata_PropertyAdaptorTester = {
    {
QT_MOC_LITERAL(0, 21), // "PropertyAdaptorTester"
QT_MOC_LITERAL(22, 12), // "dummySignal1"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 12), // "dummySignal2"
QT_MOC_LITERAL(49, 12), // "dummySignal3"
QT_MOC_LITERAL(62, 12), // "dummySignal4"
QT_MOC_LITERAL(75, 12), // "dummySignal5"
QT_MOC_LITERAL(88, 12), // "dummySignal6"
QT_MOC_LITERAL(101, 12), // "dummySignal7"
QT_MOC_LITERAL(114, 12), // "dummySignal8"
QT_MOC_LITERAL(127, 10), // "fooChanged"
QT_MOC_LITERAL(138, 6), // "newFoo"
QT_MOC_LITERAL(145, 13), // "fooHasChanged"
QT_MOC_LITERAL(159, 3), // "foo"
QT_MOC_LITERAL(163, 4) // "foo1"

    },
    "PropertyAdaptorTester\0dummySignal1\0\0"
    "dummySignal2\0dummySignal3\0dummySignal4\0"
    "dummySignal5\0dummySignal6\0dummySignal7\0"
    "dummySignal8\0fooChanged\0newFoo\0"
    "fooHasChanged\0foo\0foo1"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PropertyAdaptorTester[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       2,   86, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    3 /* Public */,
       3,    0,   75,    2, 0x06,    4 /* Public */,
       4,    0,   76,    2, 0x06,    5 /* Public */,
       5,    0,   77,    2, 0x06,    6 /* Public */,
       6,    0,   78,    2, 0x06,    7 /* Public */,
       7,    0,   79,    2, 0x06,    8 /* Public */,
       8,    0,   80,    2, 0x06,    9 /* Public */,
       9,    0,   81,    2, 0x06,   10 /* Public */,
      10,    1,   82,    2, 0x06,   11 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    0,   85,    2, 0x0a,   13 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
      13, QMetaType::Int, 0x00015103, uint(8), 0,
      14, QMetaType::Int, 0x00015003, uint(-1), 0,

       0        // eod
};

void PropertyAdaptorTester::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PropertyAdaptorTester *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dummySignal1(); break;
        case 1: _t->dummySignal2(); break;
        case 2: _t->dummySignal3(); break;
        case 3: _t->dummySignal4(); break;
        case 4: _t->dummySignal5(); break;
        case 5: _t->dummySignal6(); break;
        case 6: _t->dummySignal7(); break;
        case 7: _t->dummySignal8(); break;
        case 8: _t->fooChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->fooHasChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PropertyAdaptorTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::dummySignal1)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PropertyAdaptorTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::dummySignal2)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PropertyAdaptorTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::dummySignal3)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PropertyAdaptorTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::dummySignal4)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PropertyAdaptorTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::dummySignal5)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (PropertyAdaptorTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::dummySignal6)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (PropertyAdaptorTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::dummySignal7)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (PropertyAdaptorTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::dummySignal8)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (PropertyAdaptorTester::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyAdaptorTester::fooChanged)) {
                *result = 8;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<PropertyAdaptorTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->foo(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->foo(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<PropertyAdaptorTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFoo(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setFoo(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject PropertyAdaptorTester::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PropertyAdaptorTester.offsetsAndSize,
    qt_meta_data_PropertyAdaptorTester,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_PropertyAdaptorTester_t
, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<PropertyAdaptorTester, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *PropertyAdaptorTester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PropertyAdaptorTester::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyAdaptorTester.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PropertyAdaptorTester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void PropertyAdaptorTester::dummySignal1()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PropertyAdaptorTester::dummySignal2()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PropertyAdaptorTester::dummySignal3()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void PropertyAdaptorTester::dummySignal4()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void PropertyAdaptorTester::dummySignal5()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void PropertyAdaptorTester::dummySignal6()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void PropertyAdaptorTester::dummySignal7()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void PropertyAdaptorTester::dummySignal8()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void PropertyAdaptorTester::fooChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
struct qt_meta_stringdata_PropertyWithInitializationTester_t {
    const uint offsetsAndSize[14];
    char stringdata0[76];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_PropertyWithInitializationTester_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_PropertyWithInitializationTester_t qt_meta_stringdata_PropertyWithInitializationTester = {
    {
QT_MOC_LITERAL(0, 32), // "PropertyWithInitializationTester"
QT_MOC_LITERAL(33, 12), // "prop1Changed"
QT_MOC_LITERAL(46, 0), // ""
QT_MOC_LITERAL(47, 5), // "prop1"
QT_MOC_LITERAL(53, 5), // "prop2"
QT_MOC_LITERAL(59, 10), // "CustomType"
QT_MOC_LITERAL(70, 5) // "prop3"

    },
    "PropertyWithInitializationTester\0"
    "prop1Changed\0\0prop1\0prop2\0CustomType\0"
    "prop3"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PropertyWithInitializationTester[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       3,   21, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   20,    2, 0x06,    4 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::Int, 0x02015103, uint(0), 0,
       4, 0x80000000 | 5, 0x0201510b, uint(-1), 0,
       6, 0x80000000 | 5, 0x0201510b, uint(-1), 0,

       0        // eod
};

void PropertyWithInitializationTester::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PropertyWithInitializationTester *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->prop1Changed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PropertyWithInitializationTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PropertyWithInitializationTester::prop1Changed)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<PropertyWithInitializationTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->prop1(); break;
        case 1: *reinterpret_cast< CustomType*>(_v) = _t->prop2(); break;
        case 2: *reinterpret_cast< CustomType*>(_v) = _t->prop3(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<PropertyWithInitializationTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setProp1(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setProp2(*reinterpret_cast< CustomType*>(_v)); break;
        case 2: _t->setProp3(*reinterpret_cast< CustomType*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
        auto *_t = static_cast<PropertyWithInitializationTester *>(_o);
        (void)_t;
        switch (_id) {
        case 0: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp1(); break;
        case 1: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp2(); break;
        case 2: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp3(); break;
        default: break;
        }
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

const QMetaObject PropertyWithInitializationTester::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PropertyWithInitializationTester.offsetsAndSize,
    qt_meta_data_PropertyWithInitializationTester,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_PropertyWithInitializationTester_t
, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<CustomType, std::true_type>, QtPrivate::TypeAndForceComplete<CustomType, std::true_type>, QtPrivate::TypeAndForceComplete<PropertyWithInitializationTester, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *PropertyWithInitializationTester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PropertyWithInitializationTester::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyWithInitializationTester.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PropertyWithInitializationTester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void PropertyWithInitializationTester::prop1Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_CompatPropertyTester_t {
    const uint offsetsAndSize[16];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_CompatPropertyTester_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_CompatPropertyTester_t qt_meta_stringdata_CompatPropertyTester = {
    {
QT_MOC_LITERAL(0, 20), // "CompatPropertyTester"
QT_MOC_LITERAL(21, 12), // "prop2Changed"
QT_MOC_LITERAL(34, 0), // ""
QT_MOC_LITERAL(35, 5), // "value"
QT_MOC_LITERAL(41, 12), // "prop3Changed"
QT_MOC_LITERAL(54, 5), // "prop1"
QT_MOC_LITERAL(60, 5), // "prop2"
QT_MOC_LITERAL(66, 5) // "prop3"

    },
    "CompatPropertyTester\0prop2Changed\0\0"
    "value\0prop3Changed\0prop1\0prop2\0prop3"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CompatPropertyTester[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       3,   30, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   26,    2, 0x06,    4 /* Public */,
       4,    0,   29,    2, 0x06,    6 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::Int, 0x02015103, uint(-1), 0,
       6, QMetaType::Int, 0x02015103, uint(0), 0,
       7, QMetaType::Int, 0x02015103, uint(1), 0,

       0        // eod
};

void CompatPropertyTester::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CompatPropertyTester *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->prop2Changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->prop3Changed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CompatPropertyTester::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CompatPropertyTester::prop2Changed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CompatPropertyTester::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CompatPropertyTester::prop3Changed)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CompatPropertyTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->prop1(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->prop2(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->prop3(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CompatPropertyTester *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setProp1(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setProp2(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setProp3(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
        auto *_t = static_cast<CompatPropertyTester *>(_o);
        (void)_t;
        switch (_id) {
        case 0: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp1(); break;
        case 1: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp2(); break;
        case 2: *static_cast<QUntypedBindable *>(_a[0]) = _t->bindableProp3(); break;
        default: break;
        }
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject CompatPropertyTester::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CompatPropertyTester.offsetsAndSize,
    qt_meta_data_CompatPropertyTester,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_CompatPropertyTester_t
, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<CompatPropertyTester, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *CompatPropertyTester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CompatPropertyTester::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CompatPropertyTester.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CompatPropertyTester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void CompatPropertyTester::prop2Changed(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CompatPropertyTester::prop3Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
