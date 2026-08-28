/****************************************************************************
** Meta object code from reading C++ file 'oscarbackend.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/oscarbackend.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'oscarbackend.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_OscarBackend_t {
    uint offsetsAndSizes[94];
    char stringdata0[13];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[8];
    char stringdata5[12];
    char stringdata6[26];
    char stringdata7[9];
    char stringdata8[10];
    char stringdata9[17];
    char stringdata10[17];
    char stringdata11[15];
    char stringdata12[8];
    char stringdata13[17];
    char stringdata14[7];
    char stringdata15[5];
    char stringdata16[22];
    char stringdata17[22];
    char stringdata18[6];
    char stringdata19[8];
    char stringdata20[20];
    char stringdata21[20];
    char stringdata22[6];
    char stringdata23[29];
    char stringdata24[5];
    char stringdata25[30];
    char stringdata26[9];
    char stringdata27[14];
    char stringdata28[27];
    char stringdata29[6];
    char stringdata30[20];
    char stringdata31[5];
    char stringdata32[5];
    char stringdata33[23];
    char stringdata34[10];
    char stringdata35[8];
    char stringdata36[20];
    char stringdata37[10];
    char stringdata38[14];
    char stringdata39[11];
    char stringdata40[11];
    char stringdata41[9];
    char stringdata42[11];
    char stringdata43[20];
    char stringdata44[21];
    char stringdata45[7];
    char stringdata46[26];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_OscarBackend_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_OscarBackend_t qt_meta_stringdata_OscarBackend = {
    {
        QT_MOC_LITERAL(0, 12),  // "OscarBackend"
        QT_MOC_LITERAL(13, 15),  // "presenceChanged"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 5),  // "state"
        QT_MOC_LITERAL(36, 7),  // "message"
        QT_MOC_LITERAL(44, 11),  // "idleSeconds"
        QT_MOC_LITERAL(56, 25),  // "serverCapabilitiesChanged"
        QT_MOC_LITERAL(82, 8),  // "features"
        QT_MOC_LITERAL(91, 9),  // "familyIds"
        QT_MOC_LITERAL(101, 16),  // "profileSupported"
        QT_MOC_LITERAL(118, 16),  // "maxProfileLength"
        QT_MOC_LITERAL(135, 14),  // "profileChanged"
        QT_MOC_LITERAL(150, 7),  // "profile"
        QT_MOC_LITERAL(158, 16),  // "userInfoReceived"
        QT_MOC_LITERAL(175, 6),  // "target"
        QT_MOC_LITERAL(182, 4),  // "info"
        QT_MOC_LITERAL(187, 21),  // "directoryInfoReceived"
        QT_MOC_LITERAL(209, 21),  // "lookupResultsReceived"
        QT_MOC_LITERAL(231, 5),  // "query"
        QT_MOC_LITERAL(237, 7),  // "results"
        QT_MOC_LITERAL(245, 19),  // "accountInfoReceived"
        QT_MOC_LITERAL(265, 19),  // "watcherListReceived"
        QT_MOC_LITERAL(285, 5),  // "users"
        QT_MOC_LITERAL(291, 28),  // "authorizationRequestReceived"
        QT_MOC_LITERAL(320, 4),  // "from"
        QT_MOC_LITERAL(325, 29),  // "authorizationResponseReceived"
        QT_MOC_LITERAL(355, 8),  // "accepted"
        QT_MOC_LITERAL(364, 13),  // "buddyAddedYou"
        QT_MOC_LITERAL(378, 26),  // "typingNotificationReceived"
        QT_MOC_LITERAL(405, 5),  // "event"
        QT_MOC_LITERAL(411, 19),  // "oscarNoticeReceived"
        QT_MOC_LITERAL(431, 4),  // "kind"
        QT_MOC_LITERAL(436, 4),  // "text"
        QT_MOC_LITERAL(441, 22),  // "featureOperationResult"
        QT_MOC_LITERAL(464, 9),  // "operation"
        QT_MOC_LITERAL(474, 7),  // "success"
        QT_MOC_LITERAL(482, 19),  // "voiceInviteReceived"
        QT_MOC_LITERAL(502, 9),  // "cookieHex"
        QT_MOC_LITERAL(512, 13),  // "remoteAddress"
        QT_MOC_LITERAL(526, 10),  // "remotePort"
        QT_MOC_LITERAL(537, 10),  // "sampleRate"
        QT_MOC_LITERAL(548, 8),  // "channels"
        QT_MOC_LITERAL(557, 10),  // "invitation"
        QT_MOC_LITERAL(568, 19),  // "voiceInviteAccepted"
        QT_MOC_LITERAL(588, 20),  // "voiceInviteCancelled"
        QT_MOC_LITERAL(609, 6),  // "reason"
        QT_MOC_LITERAL(616, 25)   // "legacyVoiceInviteReceived"
    },
    "OscarBackend",
    "presenceChanged",
    "",
    "state",
    "message",
    "idleSeconds",
    "serverCapabilitiesChanged",
    "features",
    "familyIds",
    "profileSupported",
    "maxProfileLength",
    "profileChanged",
    "profile",
    "userInfoReceived",
    "target",
    "info",
    "directoryInfoReceived",
    "lookupResultsReceived",
    "query",
    "results",
    "accountInfoReceived",
    "watcherListReceived",
    "users",
    "authorizationRequestReceived",
    "from",
    "authorizationResponseReceived",
    "accepted",
    "buddyAddedYou",
    "typingNotificationReceived",
    "event",
    "oscarNoticeReceived",
    "kind",
    "text",
    "featureOperationResult",
    "operation",
    "success",
    "voiceInviteReceived",
    "cookieHex",
    "remoteAddress",
    "remotePort",
    "sampleRate",
    "channels",
    "invitation",
    "voiceInviteAccepted",
    "voiceInviteCancelled",
    "reason",
    "legacyVoiceInviteReceived"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_OscarBackend[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      18,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,  122,    2, 0x06,    1 /* Public */,
       6,    4,  129,    2, 0x06,    5 /* Public */,
      11,    1,  138,    2, 0x06,   10 /* Public */,
      13,    2,  141,    2, 0x06,   12 /* Public */,
      16,    2,  146,    2, 0x06,   15 /* Public */,
      17,    2,  151,    2, 0x06,   18 /* Public */,
      20,    1,  156,    2, 0x06,   21 /* Public */,
      21,    1,  159,    2, 0x06,   23 /* Public */,
      23,    2,  162,    2, 0x06,   25 /* Public */,
      25,    3,  167,    2, 0x06,   28 /* Public */,
      27,    1,  174,    2, 0x06,   32 /* Public */,
      28,    2,  177,    2, 0x06,   34 /* Public */,
      30,    2,  182,    2, 0x06,   37 /* Public */,
      33,    3,  187,    2, 0x06,   40 /* Public */,
      36,    7,  194,    2, 0x06,   44 /* Public */,
      43,    6,  209,    2, 0x06,   52 /* Public */,
      44,    3,  222,    2, 0x06,   59 /* Public */,
      46,    2,  229,    2, 0x06,   63 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UInt,    3,    4,    5,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QStringList, QMetaType::Bool, QMetaType::Int,    7,    8,    9,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantMap,   14,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantMap,   14,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList,   18,   19,
    QMetaType::Void, QMetaType::QVariantMap,   15,
    QMetaType::Void, QMetaType::QStringList,   22,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   24,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::QString,   24,   26,    4,
    QMetaType::Void, QMetaType::QString,   24,
    QMetaType::Void, QMetaType::QString, QMetaType::UShort,   24,   29,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   31,   32,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::QString,   34,   35,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::UShort, QMetaType::Int, QMetaType::Int, QMetaType::QString,   24,   37,   38,   39,   40,   41,   42,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::UShort, QMetaType::Int, QMetaType::Int,   24,   37,   38,   39,   40,   41,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UShort,   24,   37,   45,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   24,   37,

       0        // eod
};

Q_CONSTINIT const QMetaObject OscarBackend::staticMetaObject = { {
    QMetaObject::SuperData::link<ChatBackend::staticMetaObject>(),
    qt_meta_stringdata_OscarBackend.offsetsAndSizes,
    qt_meta_data_OscarBackend,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_OscarBackend_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<OscarBackend, std::true_type>,
        // method 'presenceChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'serverCapabilitiesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'profileChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'userInfoReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>,
        // method 'directoryInfoReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>,
        // method 'lookupResultsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'accountInfoReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>,
        // method 'watcherListReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'authorizationRequestReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'authorizationResponseReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'buddyAddedYou'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'typingNotificationReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'oscarNoticeReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'featureOperationResult'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'voiceInviteReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'voiceInviteAccepted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'voiceInviteCancelled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'legacyVoiceInviteReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void OscarBackend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OscarBackend *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->presenceChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[3]))); break;
        case 1: _t->serverCapabilitiesChanged((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 2: _t->profileChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->userInfoReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 4: _t->directoryInfoReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 5: _t->lookupResultsReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[2]))); break;
        case 6: _t->accountInfoReceived((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 7: _t->watcherListReceived((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 8: _t->authorizationRequestReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->authorizationResponseReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 10: _t->buddyAddedYou((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->typingNotificationReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint16>>(_a[2]))); break;
        case 12: _t->oscarNoticeReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 13: _t->featureOperationResult((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 14: _t->voiceInviteReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<quint16>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[7]))); break;
        case 15: _t->voiceInviteAccepted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<quint16>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[6]))); break;
        case 16: _t->voiceInviteCancelled((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<quint16>>(_a[3]))); break;
        case 17: _t->legacyVoiceInviteReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OscarBackend::*)(const QString & , const QString & , quint32 );
            if (_t _q_method = &OscarBackend::presenceChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QStringList & , const QStringList & , bool , int );
            if (_t _q_method = &OscarBackend::serverCapabilitiesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & );
            if (_t _q_method = &OscarBackend::profileChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QVariantMap & );
            if (_t _q_method = &OscarBackend::userInfoReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QVariantMap & );
            if (_t _q_method = &OscarBackend::directoryInfoReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QStringList & );
            if (_t _q_method = &OscarBackend::lookupResultsReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QVariantMap & );
            if (_t _q_method = &OscarBackend::accountInfoReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QStringList & );
            if (_t _q_method = &OscarBackend::watcherListReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QString & );
            if (_t _q_method = &OscarBackend::authorizationRequestReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , bool , const QString & );
            if (_t _q_method = &OscarBackend::authorizationResponseReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & );
            if (_t _q_method = &OscarBackend::buddyAddedYou; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , quint16 );
            if (_t _q_method = &OscarBackend::typingNotificationReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QString & );
            if (_t _q_method = &OscarBackend::oscarNoticeReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , bool , const QString & );
            if (_t _q_method = &OscarBackend::featureOperationResult; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QString & , const QString & , quint16 , int , int , const QString & );
            if (_t _q_method = &OscarBackend::voiceInviteReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QString & , const QString & , quint16 , int , int );
            if (_t _q_method = &OscarBackend::voiceInviteAccepted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QString & , quint16 );
            if (_t _q_method = &OscarBackend::voiceInviteCancelled; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (OscarBackend::*)(const QString & , const QString & );
            if (_t _q_method = &OscarBackend::legacyVoiceInviteReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
    }
}

const QMetaObject *OscarBackend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OscarBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OscarBackend.stringdata0))
        return static_cast<void*>(this);
    return ChatBackend::qt_metacast(_clname);
}

int OscarBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ChatBackend::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void OscarBackend::presenceChanged(const QString & _t1, const QString & _t2, quint32 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OscarBackend::serverCapabilitiesChanged(const QStringList & _t1, const QStringList & _t2, bool _t3, int _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OscarBackend::profileChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void OscarBackend::userInfoReceived(const QString & _t1, const QVariantMap & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void OscarBackend::directoryInfoReceived(const QString & _t1, const QVariantMap & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void OscarBackend::lookupResultsReceived(const QString & _t1, const QStringList & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void OscarBackend::accountInfoReceived(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void OscarBackend::watcherListReceived(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void OscarBackend::authorizationRequestReceived(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void OscarBackend::authorizationResponseReceived(const QString & _t1, bool _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void OscarBackend::buddyAddedYou(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void OscarBackend::typingNotificationReceived(const QString & _t1, quint16 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void OscarBackend::oscarNoticeReceived(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void OscarBackend::featureOperationResult(const QString & _t1, bool _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void OscarBackend::voiceInviteReceived(const QString & _t1, const QString & _t2, const QString & _t3, quint16 _t4, int _t5, int _t6, const QString & _t7)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void OscarBackend::voiceInviteAccepted(const QString & _t1, const QString & _t2, const QString & _t3, quint16 _t4, int _t5, int _t6)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void OscarBackend::voiceInviteCancelled(const QString & _t1, const QString & _t2, quint16 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void OscarBackend::legacyVoiceInviteReceived(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
