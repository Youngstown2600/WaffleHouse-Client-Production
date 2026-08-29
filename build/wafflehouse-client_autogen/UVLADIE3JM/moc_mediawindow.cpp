/****************************************************************************
** Meta object code from reading C++ file 'mediawindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/mediawindow.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mediawindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MediaWindow_t {
    uint offsetsAndSizes[18];
    char stringdata0[12];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[17];
    char stringdata5[27];
    char stringdata6[25];
    char stringdata7[19];
    char stringdata8[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MediaWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MediaWindow_t qt_meta_stringdata_MediaWindow = {
    {
        QT_MOC_LITERAL(0, 11),  // "MediaWindow"
        QT_MOC_LITERAL(12, 12),  // "showAndRaise"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 14),  // "openMediaFiles"
        QT_MOC_LITERAL(41, 16),  // "openStreamDialog"
        QT_MOC_LITERAL(58, 26),  // "openInternetPlaylistDialog"
        QT_MOC_LITERAL(85, 24),  // "searchShoutcastDirectory"
        QT_MOC_LITERAL(110, 18),  // "openPlaylistDialog"
        QT_MOC_LITERAL(129, 18)   // "savePlaylistDialog"
    },
    "MediaWindow",
    "showAndRaise",
    "",
    "openMediaFiles",
    "openStreamDialog",
    "openInternetPlaylistDialog",
    "searchShoutcastDirectory",
    "openPlaylistDialog",
    "savePlaylistDialog"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MediaWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x0a,    1 /* Public */,
       3,    0,   57,    2, 0x0a,    2 /* Public */,
       4,    0,   58,    2, 0x0a,    3 /* Public */,
       5,    0,   59,    2, 0x0a,    4 /* Public */,
       6,    0,   60,    2, 0x0a,    5 /* Public */,
       7,    0,   61,    2, 0x0a,    6 /* Public */,
       8,    0,   62,    2, 0x0a,    7 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MediaWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MediaWindow.offsetsAndSizes,
    qt_meta_data_MediaWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MediaWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MediaWindow, std::true_type>,
        // method 'showAndRaise'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openMediaFiles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openStreamDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openInternetPlaylistDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'searchShoutcastDirectory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openPlaylistDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'savePlaylistDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MediaWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MediaWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->showAndRaise(); break;
        case 1: _t->openMediaFiles(); break;
        case 2: _t->openStreamDialog(); break;
        case 3: _t->openInternetPlaylistDialog(); break;
        case 4: _t->searchShoutcastDirectory(); break;
        case 5: _t->openPlaylistDialog(); break;
        case 6: _t->savePlaylistDialog(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *MediaWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MediaWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MediaWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MediaWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
