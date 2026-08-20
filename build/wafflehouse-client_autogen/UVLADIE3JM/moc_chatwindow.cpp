/****************************************************************************
** Meta object code from reading C++ file 'chatwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/chatwindow.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chatwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_ChatWindow_t {
    uint offsetsAndSizes[34];
    char stringdata0[11];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[7];
    char stringdata5[17];
    char stringdata6[8];
    char stringdata7[16];
    char stringdata8[22];
    char stringdata9[15];
    char stringdata10[17];
    char stringdata11[19];
    char stringdata12[18];
    char stringdata13[12];
    char stringdata14[16];
    char stringdata15[21];
    char stringdata16[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ChatWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ChatWindow_t qt_meta_stringdata_ChatWindow = {
    {
        QT_MOC_LITERAL(0, 10),  // "ChatWindow"
        QT_MOC_LITERAL(11, 19),  // "conversationClosing"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 11),  // "ChatWindow*"
        QT_MOC_LITERAL(44, 6),  // "window"
        QT_MOC_LITERAL(51, 16),  // "messageSubmitted"
        QT_MOC_LITERAL(68, 7),  // "message"
        QT_MOC_LITERAL(76, 15),  // "secureRequested"
        QT_MOC_LITERAL(92, 21),  // "secureStatusRequested"
        QT_MOC_LITERAL(114, 14),  // "trustRequested"
        QT_MOC_LITERAL(129, 16),  // "untrustRequested"
        QT_MOC_LITERAL(146, 18),  // "secureOffRequested"
        QT_MOC_LITERAL(165, 17),  // "fileSendRequested"
        QT_MOC_LITERAL(183, 11),  // "sendMessage"
        QT_MOC_LITERAL(195, 15),  // "setTransparency"
        QT_MOC_LITERAL(211, 20),  // "opacitySliderChanged"
        QT_MOC_LITERAL(232, 7)   // "percent"
    },
    "ChatWindow",
    "conversationClosing",
    "",
    "ChatWindow*",
    "window",
    "messageSubmitted",
    "message",
    "secureRequested",
    "secureStatusRequested",
    "trustRequested",
    "untrustRequested",
    "secureOffRequested",
    "fileSendRequested",
    "sendMessage",
    "setTransparency",
    "opacitySliderChanged",
    "percent"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ChatWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   80,    2, 0x06,    1 /* Public */,
       5,    2,   83,    2, 0x06,    3 /* Public */,
       7,    1,   88,    2, 0x06,    6 /* Public */,
       8,    1,   91,    2, 0x06,    8 /* Public */,
       9,    1,   94,    2, 0x06,   10 /* Public */,
      10,    1,   97,    2, 0x06,   12 /* Public */,
      11,    1,  100,    2, 0x06,   14 /* Public */,
      12,    1,  103,    2, 0x06,   16 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      13,    0,  106,    2, 0x08,   18 /* Private */,
      14,    0,  107,    2, 0x08,   19 /* Private */,
      15,    1,  108,    2, 0x08,   20 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    6,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   16,

       0        // eod
};

Q_CONSTINIT const QMetaObject ChatWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ChatWindow.offsetsAndSizes,
    qt_meta_data_ChatWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ChatWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ChatWindow, std::true_type>,
        // method 'conversationClosing'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ChatWindow *, std::false_type>,
        // method 'messageSubmitted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ChatWindow *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'secureRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ChatWindow *, std::false_type>,
        // method 'secureStatusRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ChatWindow *, std::false_type>,
        // method 'trustRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ChatWindow *, std::false_type>,
        // method 'untrustRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ChatWindow *, std::false_type>,
        // method 'secureOffRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ChatWindow *, std::false_type>,
        // method 'fileSendRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ChatWindow *, std::false_type>,
        // method 'sendMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setTransparency'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'opacitySliderChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void ChatWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ChatWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->conversationClosing((*reinterpret_cast< std::add_pointer_t<ChatWindow*>>(_a[1]))); break;
        case 1: _t->messageSubmitted((*reinterpret_cast< std::add_pointer_t<ChatWindow*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->secureRequested((*reinterpret_cast< std::add_pointer_t<ChatWindow*>>(_a[1]))); break;
        case 3: _t->secureStatusRequested((*reinterpret_cast< std::add_pointer_t<ChatWindow*>>(_a[1]))); break;
        case 4: _t->trustRequested((*reinterpret_cast< std::add_pointer_t<ChatWindow*>>(_a[1]))); break;
        case 5: _t->untrustRequested((*reinterpret_cast< std::add_pointer_t<ChatWindow*>>(_a[1]))); break;
        case 6: _t->secureOffRequested((*reinterpret_cast< std::add_pointer_t<ChatWindow*>>(_a[1]))); break;
        case 7: _t->fileSendRequested((*reinterpret_cast< std::add_pointer_t<ChatWindow*>>(_a[1]))); break;
        case 8: _t->sendMessage(); break;
        case 9: _t->setTransparency(); break;
        case 10: _t->opacitySliderChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ChatWindow* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ChatWindow* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ChatWindow* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ChatWindow* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ChatWindow* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ChatWindow* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ChatWindow* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ChatWindow* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ChatWindow::*)(ChatWindow * );
            if (_t _q_method = &ChatWindow::conversationClosing; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ChatWindow::*)(ChatWindow * , const QString & );
            if (_t _q_method = &ChatWindow::messageSubmitted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ChatWindow::*)(ChatWindow * );
            if (_t _q_method = &ChatWindow::secureRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ChatWindow::*)(ChatWindow * );
            if (_t _q_method = &ChatWindow::secureStatusRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ChatWindow::*)(ChatWindow * );
            if (_t _q_method = &ChatWindow::trustRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ChatWindow::*)(ChatWindow * );
            if (_t _q_method = &ChatWindow::untrustRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ChatWindow::*)(ChatWindow * );
            if (_t _q_method = &ChatWindow::secureOffRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ChatWindow::*)(ChatWindow * );
            if (_t _q_method = &ChatWindow::fileSendRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject *ChatWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChatWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ChatWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ChatWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void ChatWindow::conversationClosing(ChatWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ChatWindow::messageSubmitted(ChatWindow * _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ChatWindow::secureRequested(ChatWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ChatWindow::secureStatusRequested(ChatWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ChatWindow::trustRequested(ChatWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ChatWindow::untrustRequested(ChatWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ChatWindow::secureOffRequested(ChatWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ChatWindow::fileSendRequested(ChatWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
