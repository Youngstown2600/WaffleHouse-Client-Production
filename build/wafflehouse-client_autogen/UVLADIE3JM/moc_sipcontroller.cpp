/****************************************************************************
** Meta object code from reading C++ file 'sipcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/sipcontroller.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sipcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_SipController_t {
    uint offsetsAndSizes[30];
    char stringdata0[14];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[20];
    char stringdata5[10];
    char stringdata6[13];
    char stringdata7[14];
    char stringdata8[16];
    char stringdata9[13];
    char stringdata10[5];
    char stringdata11[13];
    char stringdata12[3];
    char stringdata13[10];
    char stringdata14[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SipController_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SipController_t qt_meta_stringdata_SipController = {
    {
        QT_MOC_LITERAL(0, 13),  // "SipController"
        QT_MOC_LITERAL(14, 12),  // "stateChanged"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 15),  // "accountsChanged"
        QT_MOC_LITERAL(44, 19),  // "accountStateChanged"
        QT_MOC_LITERAL(64, 9),  // "accountId"
        QT_MOC_LITERAL(74, 12),  // "callsChanged"
        QT_MOC_LITERAL(87, 13),  // "sipLogChanged"
        QT_MOC_LITERAL(101, 15),  // "activityChanged"
        QT_MOC_LITERAL(117, 12),  // "activityLine"
        QT_MOC_LITERAL(130, 4),  // "line"
        QT_MOC_LITERAL(135, 12),  // "incomingCall"
        QT_MOC_LITERAL(148, 2),  // "id"
        QT_MOC_LITERAL(151, 9),  // "remoteUri"
        QT_MOC_LITERAL(161, 4)   // "poll"
    },
    "SipController",
    "stateChanged",
    "",
    "accountsChanged",
    "accountStateChanged",
    "accountId",
    "callsChanged",
    "sipLogChanged",
    "activityChanged",
    "activityLine",
    "line",
    "incomingCall",
    "id",
    "remoteUri",
    "poll"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SipController[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x06,    1 /* Public */,
       3,    0,   69,    2, 0x06,    2 /* Public */,
       4,    1,   70,    2, 0x06,    3 /* Public */,
       6,    0,   73,    2, 0x06,    5 /* Public */,
       7,    0,   74,    2, 0x06,    6 /* Public */,
       8,    0,   75,    2, 0x06,    7 /* Public */,
       9,    1,   76,    2, 0x06,    8 /* Public */,
      11,    3,   79,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      14,    0,   86,    2, 0x08,   14 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,    5,   12,   13,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject SipController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SipController.offsetsAndSizes,
    qt_meta_data_SipController,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SipController_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SipController, std::true_type>,
        // method 'stateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'accountsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'accountStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'callsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sipLogChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'activityChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'activityLine'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'incomingCall'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'poll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void SipController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SipController *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->stateChanged(); break;
        case 1: _t->accountsChanged(); break;
        case 2: _t->accountStateChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->callsChanged(); break;
        case 4: _t->sipLogChanged(); break;
        case 5: _t->activityChanged(); break;
        case 6: _t->activityLine((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->incomingCall((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 8: _t->poll(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SipController::*)();
            if (_t _q_method = &SipController::stateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SipController::*)();
            if (_t _q_method = &SipController::accountsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SipController::*)(const QString & );
            if (_t _q_method = &SipController::accountStateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SipController::*)();
            if (_t _q_method = &SipController::callsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SipController::*)();
            if (_t _q_method = &SipController::sipLogChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SipController::*)();
            if (_t _q_method = &SipController::activityChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SipController::*)(const QString & );
            if (_t _q_method = &SipController::activityLine; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (SipController::*)(const QString & , int , const QString & );
            if (_t _q_method = &SipController::incomingCall; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject *SipController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SipController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SipController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SipController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SipController::stateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SipController::accountsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SipController::accountStateChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SipController::callsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SipController::sipLogChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SipController::activityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SipController::activityLine(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void SipController::incomingCall(const QString & _t1, int _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
