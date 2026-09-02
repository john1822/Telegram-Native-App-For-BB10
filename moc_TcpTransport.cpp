/****************************************************************************
** Meta object code from reading C++ file 'TcpTransport.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/network/TcpTransport.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TcpTransport.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Telegram__Network__TcpTransport[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x05,
      45,   32,   32,   32, 0x05,
      67,   60,   32,   32, 0x05,
     103,   94,   32,   32, 0x05,
     130,  126,   32,   32, 0x05,

 // slots: signature, parameters, type, tag, flags
     150,   32,   32,   32, 0x08,
     170,   32,   32,   32, 0x08,
     193,   32,   32,   32, 0x08,
     219,  213,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Telegram__Network__TcpTransport[] = {
    "Telegram::Network::TcpTransport\0\0"
    "connected()\0disconnected()\0packet\0"
    "packetReceived(QByteArray)\0errorMsg\0"
    "errorOccurred(QString)\0log\0"
    "logMessage(QString)\0onSocketConnected()\0"
    "onSocketDisconnected()\0onSocketReadyRead()\0"
    "error\0onSocketError(QAbstractSocket::SocketError)\0"
};

void Telegram::Network::TcpTransport::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TcpTransport *_t = static_cast<TcpTransport *>(_o);
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->packetReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onSocketConnected(); break;
        case 6: _t->onSocketDisconnected(); break;
        case 7: _t->onSocketReadyRead(); break;
        case 8: _t->onSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Telegram::Network::TcpTransport::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Telegram::Network::TcpTransport::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Telegram__Network__TcpTransport,
      qt_meta_data_Telegram__Network__TcpTransport, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Telegram::Network::TcpTransport::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Telegram::Network::TcpTransport::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Telegram::Network::TcpTransport::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Telegram__Network__TcpTransport))
        return static_cast<void*>(const_cast< TcpTransport*>(this));
    return QObject::qt_metacast(_clname);
}

int Telegram::Network::TcpTransport::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void Telegram::Network::TcpTransport::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Telegram::Network::TcpTransport::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Telegram::Network::TcpTransport::packetReceived(const QByteArray & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Telegram::Network::TcpTransport::errorOccurred(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Telegram::Network::TcpTransport::logMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
