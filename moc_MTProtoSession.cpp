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
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      51,   32,   31,   31, 0x05,
     106,  102,   31,   31, 0x05,
     139,  126,   31,   31, 0x05,
     190,  165,   31,   31, 0x05,
     231,  225,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
     254,   31,   31,   31, 0x08,
     277,   31,   31,   31, 0x08,
     310,  303,   31,   31, 0x08,
     339,  225,   31,   31, 0x08,
     365,   31,   31,   31, 0x08,
     379,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Telegram__Core__MTProtoSession[] = {
    "Telegram::Core::MTProtoSession\0\0"
    "newState,stateText\0"
    "stateChanged(Telegram::Core::SessionState,QString)\0"
    "log\0logMessage(QString)\0authKeyIdHex\0"
    "authKeyGenerated(quint64)\0"
    "country,thisDc,nearestDc\0"
    "nearestDcReceived(QString,int,int)\0"
    "error\0errorOccurred(QString)\0"
    "onTransportConnected()\0onTransportDisconnected()\0"
    "packet\0onPacketReceived(QByteArray)\0"
    "onTransportError(QString)\0onPingTimer()\0"
    "onReconnectTimer()\0"
};

void Telegram::Core::MTProtoSession::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MTProtoSession *_t = static_cast<MTProtoSession *>(_o);
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< Telegram::Core::SessionState(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->authKeyGenerated((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 3: _t->nearestDcReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onTransportConnected(); break;
        case 6: _t->onTransportDisconnected(); break;
        case 7: _t->onPacketReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 8: _t->onTransportError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->onPingTimer(); break;
        case 10: _t->onReconnectTimer(); break;
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
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Telegram::Core::MTProtoSession::stateChanged(Telegram::Core::SessionState _t1, const QString & _t2)
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
void Telegram::Core::MTProtoSession::nearestDcReceived(const QString & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Telegram::Core::MTProtoSession::errorOccurred(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
