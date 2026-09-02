/****************************************************************************
** Meta object code from reading C++ file 'DiagnosticController.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/controllers/DiagnosticController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DiagnosticController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Telegram__Controllers__DiagnosticController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       7,   94, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      45,   44,   44,   44, 0x05,
      65,   44,   44,   44, 0x05,
      84,   44,   44,   44, 0x05,
     100,   44,   44,   44, 0x05,
     117,   44,   44,   44, 0x05,
     131,   44,   44,   44, 0x05,
     151,   44,   44,   44, 0x05,

 // slots: signature, parameters, type, tag, flags
     187,  171,   44,   44, 0x08,
     219,  215,   44,   44, 0x08,
     254,  241,   44,   44, 0x08,
     307,  282,   44,   44, 0x08,
     350,  344,   44,   44, 0x08,

 // methods: signature, parameters, type, tag, flags
     375,   44,   44,   44, 0x02,
     393,   44,   44,   44, 0x02,
     405,   44,   44,   44, 0x02,
     419,   44,   44,   44, 0x02,

 // properties: name, type, flags
     439,  431, 0x0a495001,
     450,  431, 0x0a495001,
     460,  431, 0x0a495001,
     467,  431, 0x0a495001,
     475,  431, 0x0a495001,
     489,  484, 0x01495001,
     501,  484, 0x01495001,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,
       5,
       6,

       0        // eod
};

static const char qt_meta_stringdata_Telegram__Controllers__DiagnosticController[] = {
    "Telegram::Controllers::DiagnosticController\0"
    "\0statusTextChanged()\0authKeyIdChanged()\0"
    "dcInfoChanged()\0countryChanged()\0"
    "logsChanged()\0connectionChanged()\0"
    "encryptionChanged()\0state,stateText\0"
    "onStateChanged(int,QString)\0log\0"
    "onLogMessage(QString)\0authKeyIdHex\0"
    "onAuthKeyGenerated(quint64)\0"
    "country,thisDc,nearestDc\0"
    "onNearestDcReceived(QString,int,int)\0"
    "error\0onErrorOccurred(QString)\0"
    "startConnection()\0reconnect()\0"
    "sendTestRpc()\0clearLogs()\0QString\0"
    "statusText\0authKeyId\0dcInfo\0country\0"
    "logsText\0bool\0isConnected\0isEncrypted\0"
};

void Telegram::Controllers::DiagnosticController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DiagnosticController *_t = static_cast<DiagnosticController *>(_o);
        switch (_id) {
        case 0: _t->statusTextChanged(); break;
        case 1: _t->authKeyIdChanged(); break;
        case 2: _t->dcInfoChanged(); break;
        case 3: _t->countryChanged(); break;
        case 4: _t->logsChanged(); break;
        case 5: _t->connectionChanged(); break;
        case 6: _t->encryptionChanged(); break;
        case 7: _t->onStateChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 8: _t->onLogMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->onAuthKeyGenerated((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 10: _t->onNearestDcReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 11: _t->onErrorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->startConnection(); break;
        case 13: _t->reconnect(); break;
        case 14: _t->sendTestRpc(); break;
        case 15: _t->clearLogs(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Telegram::Controllers::DiagnosticController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Telegram::Controllers::DiagnosticController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Telegram__Controllers__DiagnosticController,
      qt_meta_data_Telegram__Controllers__DiagnosticController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Telegram::Controllers::DiagnosticController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Telegram::Controllers::DiagnosticController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Telegram::Controllers::DiagnosticController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Telegram__Controllers__DiagnosticController))
        return static_cast<void*>(const_cast< DiagnosticController*>(this));
    return QObject::qt_metacast(_clname);
}

int Telegram::Controllers::DiagnosticController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = statusText(); break;
        case 1: *reinterpret_cast< QString*>(_v) = authKeyId(); break;
        case 2: *reinterpret_cast< QString*>(_v) = dcInfo(); break;
        case 3: *reinterpret_cast< QString*>(_v) = country(); break;
        case 4: *reinterpret_cast< QString*>(_v) = logsText(); break;
        case 5: *reinterpret_cast< bool*>(_v) = isConnected(); break;
        case 6: *reinterpret_cast< bool*>(_v) = isEncrypted(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Telegram::Controllers::DiagnosticController::statusTextChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Telegram::Controllers::DiagnosticController::authKeyIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Telegram::Controllers::DiagnosticController::dcInfoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Telegram::Controllers::DiagnosticController::countryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Telegram::Controllers::DiagnosticController::logsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void Telegram::Controllers::DiagnosticController::connectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void Telegram::Controllers::DiagnosticController::encryptionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
