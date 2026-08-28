/****************************************************************************
** Meta object code from reading C++ file 'oscarvoice.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/oscarvoice.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'oscarvoice.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_OscarVoiceSession_t {
    uint offsetsAndSizes[18];
    char stringdata0[18];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[14];
    char stringdata5[14];
    char stringdata6[7];
    char stringdata7[13];
    char stringdata8[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_OscarVoiceSession_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_OscarVoiceSession_t qt_meta_stringdata_OscarVoiceSession = {
    {
        QT_MOC_LITERAL(0, 17),  // "OscarVoiceSession"
        QT_MOC_LITERAL(18, 13),  // "statusChanged"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 7),  // "message"
        QT_MOC_LITERAL(41, 13),  // "errorOccurred"
        QT_MOC_LITERAL(55, 13),  // "activeChanged"
        QT_MOC_LITERAL(69, 6),  // "active"
        QT_MOC_LITERAL(76, 12),  // "captureReady"
        QT_MOC_LITERAL(89, 14)   // "datagramsReady"
    },
    "OscarVoiceSession",
    "statusChanged",
    "",
    "message",
    "errorOccurred",
    "activeChanged",
    "active",
    "captureReady",
    "datagramsReady"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_OscarVoiceSession[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,
       4,    1,   47,    2, 0x06,    3 /* Public */,
       5,    1,   50,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,   53,    2, 0x08,    7 /* Private */,
       8,    0,   54,    2, 0x08,    8 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject OscarVoiceSession::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_OscarVoiceSession.offsetsAndSizes,
    qt_meta_data_OscarVoiceSession,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_OscarVoiceSession_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<OscarVoiceSession, std::true_type>,
        // method 'statusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'activeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'captureReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'datagramsReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void OscarVoiceSession::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OscarVoiceSession *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->statusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->activeChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->captureReady(); break;
        case 4: _t->datagramsReady(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OscarVoiceSession::*)(const QString & );
            if (_t _q_method = &OscarVoiceSession::statusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (OscarVoiceSession::*)(const QString & );
            if (_t _q_method = &OscarVoiceSession::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (OscarVoiceSession::*)(bool );
            if (_t _q_method = &OscarVoiceSession::activeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *OscarVoiceSession::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OscarVoiceSession::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OscarVoiceSession.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int OscarVoiceSession::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void OscarVoiceSession::statusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OscarVoiceSession::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OscarVoiceSession::activeChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
