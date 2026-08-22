/****************************************************************************
** Meta object code from reading C++ file 'mediacontroller.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/mediacontroller.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mediacontroller.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MediaController_t {
    uint offsetsAndSizes[124];
    char stringdata0[16];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[18];
    char stringdata5[6];
    char stringdata6[14];
    char stringdata7[7];
    char stringdata8[16];
    char stringdata9[8];
    char stringdata10[16];
    char stringdata11[13];
    char stringdata12[7];
    char stringdata13[14];
    char stringdata14[8];
    char stringdata15[12];
    char stringdata16[6];
    char stringdata17[12];
    char stringdata18[5];
    char stringdata19[16];
    char stringdata20[23];
    char stringdata21[8];
    char stringdata22[7];
    char stringdata23[13];
    char stringdata24[14];
    char stringdata25[8];
    char stringdata26[13];
    char stringdata27[5];
    char stringdata28[8];
    char stringdata29[13];
    char stringdata30[10];
    char stringdata31[8];
    char stringdata32[18];
    char stringdata33[6];
    char stringdata34[20];
    char stringdata35[14];
    char stringdata36[6];
    char stringdata37[7];
    char stringdata38[12];
    char stringdata39[5];
    char stringdata40[5];
    char stringdata41[9];
    char stringdata42[13];
    char stringdata43[13];
    char stringdata44[10];
    char stringdata45[9];
    char stringdata46[12];
    char stringdata47[11];
    char stringdata48[8];
    char stringdata49[14];
    char stringdata50[5];
    char stringdata51[17];
    char stringdata52[5];
    char stringdata53[7];
    char stringdata54[15];
    char stringdata55[9];
    char stringdata56[11];
    char stringdata57[8];
    char stringdata58[16];
    char stringdata59[9];
    char stringdata60[26];
    char stringdata61[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MediaController_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MediaController_t qt_meta_stringdata_MediaController = {
    {
        QT_MOC_LITERAL(0, 15),  // "MediaController"
        QT_MOC_LITERAL(16, 12),  // "readyChanged"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 5),  // "ready"
        QT_MOC_LITERAL(36, 17),  // "nowPlayingChanged"
        QT_MOC_LITERAL(54, 5),  // "title"
        QT_MOC_LITERAL(60, 13),  // "sourceChanged"
        QT_MOC_LITERAL(74, 6),  // "source"
        QT_MOC_LITERAL(81, 15),  // "positionChanged"
        QT_MOC_LITERAL(97, 7),  // "seconds"
        QT_MOC_LITERAL(105, 15),  // "durationChanged"
        QT_MOC_LITERAL(121, 12),  // "pauseChanged"
        QT_MOC_LITERAL(134, 6),  // "paused"
        QT_MOC_LITERAL(141, 13),  // "volumeChanged"
        QT_MOC_LITERAL(155, 7),  // "percent"
        QT_MOC_LITERAL(163, 11),  // "muteChanged"
        QT_MOC_LITERAL(175, 5),  // "muted"
        QT_MOC_LITERAL(181, 11),  // "idleChanged"
        QT_MOC_LITERAL(193, 4),  // "idle"
        QT_MOC_LITERAL(198, 15),  // "playlistChanged"
        QT_MOC_LITERAL(214, 22),  // "playlistEntriesChanged"
        QT_MOC_LITERAL(237, 7),  // "sources"
        QT_MOC_LITERAL(245, 6),  // "titles"
        QT_MOC_LITERAL(252, 12),  // "currentIndex"
        QT_MOC_LITERAL(265, 13),  // "statusMessage"
        QT_MOC_LITERAL(279, 7),  // "message"
        QT_MOC_LITERAL(287, 12),  // "errorMessage"
        QT_MOC_LITERAL(300, 4),  // "play"
        QT_MOC_LITERAL(305, 7),  // "enqueue"
        QT_MOC_LITERAL(313, 12),  // "loadPlaylist"
        QT_MOC_LITERAL(326, 9),  // "pathOrUrl"
        QT_MOC_LITERAL(336, 7),  // "replace"
        QT_MOC_LITERAL(344, 17),  // "playPlaylistIndex"
        QT_MOC_LITERAL(362, 5),  // "index"
        QT_MOC_LITERAL(368, 19),  // "removePlaylistIndex"
        QT_MOC_LITERAL(388, 13),  // "clearPlaylist"
        QT_MOC_LITERAL(402, 5),  // "pause"
        QT_MOC_LITERAL(408, 6),  // "resume"
        QT_MOC_LITERAL(415, 11),  // "togglePause"
        QT_MOC_LITERAL(427, 4),  // "stop"
        QT_MOC_LITERAL(432, 4),  // "next"
        QT_MOC_LITERAL(437, 8),  // "previous"
        QT_MOC_LITERAL(446, 12),  // "seekRelative"
        QT_MOC_LITERAL(459, 12),  // "seekAbsolute"
        QT_MOC_LITERAL(472, 9),  // "setVolume"
        QT_MOC_LITERAL(482, 8),  // "setMuted"
        QT_MOC_LITERAL(491, 11),  // "toggleMuted"
        QT_MOC_LITERAL(503, 10),  // "setShuffle"
        QT_MOC_LITERAL(514, 7),  // "enabled"
        QT_MOC_LITERAL(522, 13),  // "setRepeatMode"
        QT_MOC_LITERAL(536, 4),  // "mode"
        QT_MOC_LITERAL(541, 16),  // "setEqualizerBand"
        QT_MOC_LITERAL(558, 4),  // "band"
        QT_MOC_LITERAL(563, 6),  // "gainDb"
        QT_MOC_LITERAL(570, 14),  // "resetEqualizer"
        QT_MOC_LITERAL(585, 8),  // "shutdown"
        QT_MOC_LITERAL(594, 10),  // "connectIpc"
        QT_MOC_LITERAL(605, 7),  // "readIpc"
        QT_MOC_LITERAL(613, 15),  // "processFinished"
        QT_MOC_LITERAL(629, 8),  // "exitCode"
        QT_MOC_LITERAL(638, 25),  // "refreshObservedProperties"
        QT_MOC_LITERAL(664, 18)   // "drainBackendOutput"
    },
    "MediaController",
    "readyChanged",
    "",
    "ready",
    "nowPlayingChanged",
    "title",
    "sourceChanged",
    "source",
    "positionChanged",
    "seconds",
    "durationChanged",
    "pauseChanged",
    "paused",
    "volumeChanged",
    "percent",
    "muteChanged",
    "muted",
    "idleChanged",
    "idle",
    "playlistChanged",
    "playlistEntriesChanged",
    "sources",
    "titles",
    "currentIndex",
    "statusMessage",
    "message",
    "errorMessage",
    "play",
    "enqueue",
    "loadPlaylist",
    "pathOrUrl",
    "replace",
    "playPlaylistIndex",
    "index",
    "removePlaylistIndex",
    "clearPlaylist",
    "pause",
    "resume",
    "togglePause",
    "stop",
    "next",
    "previous",
    "seekRelative",
    "seekAbsolute",
    "setVolume",
    "setMuted",
    "toggleMuted",
    "setShuffle",
    "enabled",
    "setRepeatMode",
    "mode",
    "setEqualizerBand",
    "band",
    "gainDb",
    "resetEqualizer",
    "shutdown",
    "connectIpc",
    "readIpc",
    "processFinished",
    "exitCode",
    "refreshObservedProperties",
    "drainBackendOutput"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MediaController[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      41,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  260,    2, 0x06,    1 /* Public */,
       4,    1,  263,    2, 0x06,    3 /* Public */,
       6,    1,  266,    2, 0x06,    5 /* Public */,
       8,    1,  269,    2, 0x06,    7 /* Public */,
      10,    1,  272,    2, 0x06,    9 /* Public */,
      11,    1,  275,    2, 0x06,   11 /* Public */,
      13,    1,  278,    2, 0x06,   13 /* Public */,
      15,    1,  281,    2, 0x06,   15 /* Public */,
      17,    1,  284,    2, 0x06,   17 /* Public */,
      19,    0,  287,    2, 0x06,   19 /* Public */,
      20,    3,  288,    2, 0x06,   20 /* Public */,
      24,    1,  295,    2, 0x06,   24 /* Public */,
      26,    1,  298,    2, 0x06,   26 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      27,    1,  301,    2, 0x0a,   28 /* Public */,
      28,    1,  304,    2, 0x0a,   30 /* Public */,
      29,    2,  307,    2, 0x0a,   32 /* Public */,
      29,    1,  312,    2, 0x2a,   35 /* Public | MethodCloned */,
      32,    1,  315,    2, 0x0a,   37 /* Public */,
      34,    1,  318,    2, 0x0a,   39 /* Public */,
      35,    0,  321,    2, 0x0a,   41 /* Public */,
      36,    0,  322,    2, 0x0a,   42 /* Public */,
      37,    0,  323,    2, 0x0a,   43 /* Public */,
      38,    0,  324,    2, 0x0a,   44 /* Public */,
      39,    0,  325,    2, 0x0a,   45 /* Public */,
      40,    0,  326,    2, 0x0a,   46 /* Public */,
      41,    0,  327,    2, 0x0a,   47 /* Public */,
      42,    1,  328,    2, 0x0a,   48 /* Public */,
      43,    1,  331,    2, 0x0a,   50 /* Public */,
      44,    1,  334,    2, 0x0a,   52 /* Public */,
      45,    1,  337,    2, 0x0a,   54 /* Public */,
      46,    0,  340,    2, 0x0a,   56 /* Public */,
      47,    1,  341,    2, 0x0a,   57 /* Public */,
      49,    1,  344,    2, 0x0a,   59 /* Public */,
      51,    2,  347,    2, 0x0a,   61 /* Public */,
      54,    0,  352,    2, 0x0a,   64 /* Public */,
      55,    0,  353,    2, 0x0a,   65 /* Public */,
      56,    0,  354,    2, 0x08,   66 /* Private */,
      57,    0,  355,    2, 0x08,   67 /* Private */,
      58,    1,  356,    2, 0x08,   68 /* Private */,
      60,    0,  359,    2, 0x08,   70 /* Private */,
      61,    0,  360,    2, 0x08,   71 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   18,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QStringList, QMetaType::Int,   21,   22,   23,
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void, QMetaType::QString,   25,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString,    7,
    QMetaType::Bool, QMetaType::QString,    7,
    QMetaType::Bool, QMetaType::QString, QMetaType::Bool,   30,   31,
    QMetaType::Bool, QMetaType::QString,   30,
    QMetaType::Void, QMetaType::Int,   33,
    QMetaType::Void, QMetaType::Int,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::QString,   50,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   52,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   59,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MediaController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MediaController.offsetsAndSizes,
    qt_meta_data_MediaController,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MediaController_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MediaController, std::true_type>,
        // method 'readyChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'nowPlayingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'sourceChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'positionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'durationChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'pauseChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'volumeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'muteChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'idleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'playlistChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'playlistEntriesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'statusMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'errorMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'play'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'enqueue'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'loadPlaylist'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'loadPlaylist'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'playPlaylistIndex'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'removePlaylistIndex'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'clearPlaylist'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pause'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resume'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'togglePause'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'next'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'previous'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'seekRelative'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'seekAbsolute'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'setVolume'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setMuted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleMuted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setShuffle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setRepeatMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setEqualizerBand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'resetEqualizer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shutdown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectIpc'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readIpc'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'processFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'refreshObservedProperties'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'drainBackendOutput'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MediaController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MediaController *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->readyChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->nowPlayingChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->sourceChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->positionChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->durationChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 5: _t->pauseChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->volumeChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->muteChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->idleChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->playlistChanged(); break;
        case 10: _t->playlistEntriesChanged((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 11: _t->statusMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->errorMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: { bool _r = _t->play((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 14: { bool _r = _t->enqueue((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 15: { bool _r = _t->loadPlaylist((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 16: { bool _r = _t->loadPlaylist((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 17: _t->playPlaylistIndex((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->removePlaylistIndex((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 19: _t->clearPlaylist(); break;
        case 20: _t->pause(); break;
        case 21: _t->resume(); break;
        case 22: _t->togglePause(); break;
        case 23: _t->stop(); break;
        case 24: _t->next(); break;
        case 25: _t->previous(); break;
        case 26: _t->seekRelative((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 27: _t->seekAbsolute((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 28: _t->setVolume((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 29: _t->setMuted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 30: _t->toggleMuted(); break;
        case 31: _t->setShuffle((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 32: _t->setRepeatMode((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 33: _t->setEqualizerBand((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 34: _t->resetEqualizer(); break;
        case 35: _t->shutdown(); break;
        case 36: _t->connectIpc(); break;
        case 37: _t->readIpc(); break;
        case 38: _t->processFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 39: _t->refreshObservedProperties(); break;
        case 40: _t->drainBackendOutput(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MediaController::*)(bool );
            if (_t _q_method = &MediaController::readyChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(const QString & );
            if (_t _q_method = &MediaController::nowPlayingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(const QString & );
            if (_t _q_method = &MediaController::sourceChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(double );
            if (_t _q_method = &MediaController::positionChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(double );
            if (_t _q_method = &MediaController::durationChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(bool );
            if (_t _q_method = &MediaController::pauseChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(int );
            if (_t _q_method = &MediaController::volumeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(bool );
            if (_t _q_method = &MediaController::muteChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(bool );
            if (_t _q_method = &MediaController::idleChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (MediaController::*)();
            if (_t _q_method = &MediaController::playlistChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(const QStringList & , const QStringList & , int );
            if (_t _q_method = &MediaController::playlistEntriesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(const QString & );
            if (_t _q_method = &MediaController::statusMessage; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (MediaController::*)(const QString & );
            if (_t _q_method = &MediaController::errorMessage; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
    }
}

const QMetaObject *MediaController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MediaController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MediaController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MediaController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 41)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 41;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 41)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 41;
    }
    return _id;
}

// SIGNAL 0
void MediaController::readyChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MediaController::nowPlayingChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MediaController::sourceChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MediaController::positionChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MediaController::durationChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MediaController::pauseChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void MediaController::volumeChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void MediaController::muteChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void MediaController::idleChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void MediaController::playlistChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void MediaController::playlistEntriesChanged(const QStringList & _t1, const QStringList & _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void MediaController::statusMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void MediaController::errorMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
