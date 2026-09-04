/****************************************************************************
** Meta object code from reading C++ file 'MTProtoSession.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/core/MTProtoSession.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MTProtoSession.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Telegram__Core__MTProtoSession[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      22,       // signalCount

 // signals: signature, parameters, type, tag, flags
      51,   32,   31,   31, 0x05,
      81,   77,   31,   31, 0x05,
     114,  101,   31,   31, 0x05,
     148,  140,   31,   31, 0x05,
     189,  164,   31,   31, 0x05,
     230,  224,   31,   31, 0x05,
     280,  253,   31,   31, 0x05,
     327,  322,   31,   31, 0x05,
     407,  355,   31,   31, 0x05,
     475,   31,   31,   31, 0x05,
     518,  504,   31,   31, 0x05,
     557,   31,   31,   31, 0x05,
     607,  584,   31,   31, 0x05,
     645,  637,   31,   31, 0x05,
     697,  681,   31,   31, 0x05,
     771,  740,   31,   31, 0x05,
     830,  806,   31,   31, 0x05,
     899,  873,   31,   31, 0x05,
     967,  948,   31,   31, 0x05,
    1023, 1010,   31,   31, 0x05,
    1083, 1070,   31,   31, 0x05,
    1135, 1120,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
    1167,   31,   31,   31, 0x08,
    1190,   31,   31,   31, 0x08,
    1223, 1216,   31,   31, 0x08,
    1252,  224,   31,   31, 0x08,
    1278,   31,   31,   31, 0x08,
    1292,   31,   31,   31, 0x08,
    1311,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Telegram__Core__MTProtoSession[] = {
    "Telegram::Core::MTProtoSession\0\0"
    "newState,stateText\0stateChanged(int,QString)\0"
    "log\0logMessage(QString)\0authKeyIdHex\0"
    "authKeyGenerated(quint64)\0newDcId\0"
    "dcMigrated(int)\0country,thisDc,nearestDc\0"
    "nearestDcReceived(QString,int,int)\0"
    "error\0errorOccurred(QString)\0"
    "phoneCodeHash,type,timeout\0"
    "authSentCodeReceived(QString,QString,int)\0"
    "hint\0authPasswordNeeded(QString)\0"
    "userId,accessHash,firstName,lastName,username,phone\0"
    "authSuccessReceived(qint64,quint64,QString,QString,QString,QString)\0"
    "authSignUpRequiredReceived()\0token,expires\0"
    "authLoginTokenReceived(QByteArray,int)\0"
    "authLoginSuccessReceived()\0"
    "errorCode,errorMessage\0"
    "rpcErrorReceived(int,QString)\0dialogs\0"
    "dialogsReceived(QList<QVariantMap>)\0"
    "peerId,messages\0"
    "historyReceived(qint64,QList<QVariantMap>)\0"
    "peerId,randomId,messageId,date\0"
    "messageSent(qint64,qint64,int,int)\0"
    "peerId,peerType,message\0"
    "newMessageReceived(qint64,int,QVariantMap)\0"
    "userId,bio,username,phone\0"
    "userFullReceived(qint64,QString,QString,QString)\0"
    "bio,username,phone\0"
    "myProfileReceived(QString,QString,QString)\0"
    "userId,chats\0"
    "commonChatsReceived(qint64,QList<QVariantMap>)\0"
    "peerId,bytes\0peerPhotoReceived(qint64,QByteArray)\0"
    "reqMsgId,bytes\0fileReceived(qint64,QByteArray)\0"
    "onTransportConnected()\0onTransportDisconnected()\0"
    "packet\0onPacketReceived(QByteArray)\0"
    "onTransportError(QString)\0onPingTimer()\0"
    "onReconnectTimer()\0onMigrateTimer()\0"
};

void Telegram::Core::MTProtoSession::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MTProtoSession *_t = static_cast<MTProtoSession *>(_o);
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->authKeyGenerated((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 3: _t->dcMigrated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->nearestDcReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->authSentCodeReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 7: _t->authPasswordNeeded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->authSuccessReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< quint64(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const QString(*)>(_a[6]))); break;
        case 9: _t->authSignUpRequiredReceived(); break;
        case 10: _t->authLoginTokenReceived((*reinterpret_cast< const QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->authLoginSuccessReceived(); break;
        case 12: _t->rpcErrorReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 13: _t->dialogsReceived((*reinterpret_cast< const QList<QVariantMap>(*)>(_a[1]))); break;
        case 14: _t->historyReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QList<QVariantMap>(*)>(_a[2]))); break;
        case 15: _t->messageSent((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 16: _t->newMessageReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QVariantMap(*)>(_a[3]))); break;
        case 17: _t->userFullReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 18: _t->myProfileReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 19: _t->commonChatsReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QList<QVariantMap>(*)>(_a[2]))); break;
        case 20: _t->peerPhotoReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 21: _t->fileReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 22: _t->onTransportConnected(); break;
        case 23: _t->onTransportDisconnected(); break;
        case 24: _t->onPacketReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 25: _t->onTransportError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 26: _t->onPingTimer(); break;
        case 27: _t->onReconnectTimer(); break;
        case 28: _t->onMigrateTimer(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Telegram::Core::MTProtoSession::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Telegram::Core::MTProtoSession::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Telegram__Core__MTProtoSession,
      qt_meta_data_Telegram__Core__MTProtoSession, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Telegram::Core::MTProtoSession::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Telegram::Core::MTProtoSession::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Telegram::Core::MTProtoSession::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Telegram__Core__MTProtoSession))
        return static_cast<void*>(const_cast< MTProtoSession*>(this));
    return QObject::qt_metacast(_clname);
}

int Telegram::Core::MTProtoSession::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void Telegram::Core::MTProtoSession::stateChanged(int _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Telegram::Core::MTProtoSession::logMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Telegram::Core::MTProtoSession::authKeyGenerated(quint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Telegram::Core::MTProtoSession::dcMigrated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Telegram::Core::MTProtoSession::nearestDcReceived(const QString & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Telegram::Core::MTProtoSession::errorOccurred(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Telegram::Core::MTProtoSession::authSentCodeReceived(const QString & _t1, const QString & _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Telegram::Core::MTProtoSession::authPasswordNeeded(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Telegram::Core::MTProtoSession::authSuccessReceived(qint64 _t1, quint64 _t2, const QString & _t3, const QString & _t4, const QString & _t5, const QString & _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Telegram::Core::MTProtoSession::authSignUpRequiredReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void Telegram::Core::MTProtoSession::authLoginTokenReceived(const QByteArray & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Telegram::Core::MTProtoSession::authLoginSuccessReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void Telegram::Core::MTProtoSession::rpcErrorReceived(int _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void Telegram::Core::MTProtoSession::dialogsReceived(const QList<QVariantMap> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void Telegram::Core::MTProtoSession::historyReceived(qint64 _t1, const QList<QVariantMap> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void Telegram::Core::MTProtoSession::messageSent(qint64 _t1, qint64 _t2, int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void Telegram::Core::MTProtoSession::newMessageReceived(qint64 _t1, int _t2, const QVariantMap & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void Telegram::Core::MTProtoSession::userFullReceived(qint64 _t1, const QString & _t2, const QString & _t3, const QString & _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void Telegram::Core::MTProtoSession::myProfileReceived(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void Telegram::Core::MTProtoSession::commonChatsReceived(qint64 _t1, const QList<QVariantMap> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void Telegram::Core::MTProtoSession::peerPhotoReceived(qint64 _t1, const QByteArray & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void Telegram::Core::MTProtoSession::fileReceived(qint64 _t1, const QByteArray & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}
QT_END_MOC_NAMESPACE
