/****************************************************************************
** Meta object code from reading C++ file 'AuthController.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/controllers/AuthController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AuthController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Telegram__Controllers__AuthController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      34,   14, // methods
      16,  184, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: signature, parameters, type, tag, flags
      45,   39,   38,   38, 0x05,
      73,   67,   38,   38, 0x05,
     106,  101,   38,   38, 0x05,
     141,  136,   38,   38, 0x05,
     178,  170,   38,   38, 0x05,
     207,  202,   38,   38, 0x05,
     241,  236,   38,   38, 0x05,
     265,  261,   38,   38, 0x05,
     295,   38,   38,   38, 0x05,
     320,  316,   38,   38, 0x05,
     352,  347,   38,   38, 0x05,
     386,  380,   38,   38, 0x05,

 // slots: signature, parameters, type, tag, flags
     413,   38,   38,   38, 0x0a,
     421,   67,   38,   38, 0x0a,
     453,  448,   38,   38, 0x0a,
     487,  478,   38,   38, 0x0a,
     511,   38,   38,   38, 0x0a,
     524,   38,   38,   38, 0x0a,
     541,   38,   38,   38, 0x0a,
     561,   38,   38,   38, 0x0a,
     577,   38,   38,   38, 0x0a,
     605,  586,   38,   38, 0x08,
     653,  640,   38,   38, 0x08,
     708,  681,   38,   38, 0x08,
     752,  202,   38,   38, 0x08,
     834,  782,   38,   38, 0x08,
     904,   38,   38,   38, 0x08,
     941,  927,   38,   38, 0x08,
     982,   38,   38,   38, 0x08,
    1011, 1003,   38,   38, 0x08,
    1052, 1029,   38,   38, 0x08,
    1084,  380,   38,   38, 0x08,
    1108,   38,   38,   38, 0x08,
    1143, 1124,   38,   38, 0x08,

 // properties: name, type, flags
    1192, 1188, 0x02495001,
    1210, 1202, 0x0a495001,
    1226, 1202, 0x0a495103,
    1238, 1202, 0x0a495001,
    1252, 1202, 0x0a495001,
    1265, 1188, 0x02495001,
    1277, 1202, 0x0a495001,
    1295, 1290, 0x01495001,
    1302, 1202, 0x0a495001,
    1316, 1202, 0x0a495001,
    1325, 1202, 0x0a495001,
    1336, 1202, 0x0a495001,
    1343, 1202, 0x0a495001,
    1353, 1202, 0x0a495001,
    1361, 1202, 0x0a495001,
    1372, 1202, 0x0a495001,

 // properties: notify_signal_id
       0,
       0,
       1,
       2,
       3,
       4,
       5,
       6,
       7,
       8,
       8,
       8,
       8,
       8,
       9,
      10,

       0        // eod
};

static const char qt_meta_stringdata_Telegram__Controllers__AuthController[] = {
    "Telegram::Controllers::AuthController\0"
    "\0state\0authStateChanged(int)\0phone\0"
    "phoneNumberChanged(QString)\0hash\0"
    "phoneCodeHashChanged(QString)\0type\0"
    "deliveryTypeChanged(QString)\0timeout\0"
    "codeTimeoutChanged(int)\0hint\0"
    "passwordHintChanged(QString)\0busy\0"
    "isBusyChanged(bool)\0msg\0"
    "statusMessageChanged(QString)\0"
    "userProfileChanged()\0url\0"
    "qrTokenUrlChanged(QString)\0path\0"
    "qrImagePathChanged(QString)\0error\0"
    "authErrorOccurred(QString)\0start()\0"
    "submitPhoneNumber(QString)\0code\0"
    "submitLoginCode(QString)\0password\0"
    "submitPassword(QString)\0resendCode()\0"
    "requestQrLogin()\0changePhoneNumber()\0"
    "cancelQrLogin()\0logout()\0newState,stateText\0"
    "onSessionStateChanged(int,QString)\0"
    "authKeyIdHex\0onAuthKeyGenerated(quint64)\0"
    "phoneCodeHash,type,timeout\0"
    "onAuthSentCodeReceived(QString,QString,int)\0"
    "onAuthPasswordNeeded(QString)\0"
    "userId,accessHash,firstName,lastName,username,phone\0"
    "onAuthSuccessReceived(qint64,quint64,QString,QString,QString,QString)\0"
    "onAuthSignUpRequired()\0token,expires\0"
    "onAuthLoginTokenReceived(QByteArray,int)\0"
    "onAuthLoginSuccess()\0newDcId\0"
    "onDcMigrated(int)\0errorCode,errorMessage\0"
    "onRpcErrorReceived(int,QString)\0"
    "onSessionError(QString)\0onQrPollTimer()\0"
    "bio,username,phone\0"
    "onMyProfileReceived(QString,QString,QString)\0"
    "int\0authState\0QString\0authStateString\0"
    "phoneNumber\0phoneCodeHash\0deliveryType\0"
    "codeTimeout\0passwordHint\0bool\0isBusy\0"
    "statusMessage\0userName\0userHandle\0"
    "userId\0userPhone\0userBio\0qrTokenUrl\0"
    "qrImagePath\0"
};

void Telegram::Controllers::AuthController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AuthController *_t = static_cast<AuthController *>(_o);
        switch (_id) {
        case 0: _t->authStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->phoneNumberChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->phoneCodeHashChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->deliveryTypeChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->codeTimeoutChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->passwordHintChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->isBusyChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->statusMessageChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->userProfileChanged(); break;
        case 9: _t->qrTokenUrlChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->qrImagePathChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->authErrorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->start(); break;
        case 13: _t->submitPhoneNumber((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->submitLoginCode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->submitPassword((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->resendCode(); break;
        case 17: _t->requestQrLogin(); break;
        case 18: _t->changePhoneNumber(); break;
        case 19: _t->cancelQrLogin(); break;
        case 20: _t->logout(); break;
        case 21: _t->onSessionStateChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 22: _t->onAuthKeyGenerated((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 23: _t->onAuthSentCodeReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 24: _t->onAuthPasswordNeeded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 25: _t->onAuthSuccessReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< quint64(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const QString(*)>(_a[6]))); break;
        case 26: _t->onAuthSignUpRequired(); break;
        case 27: _t->onAuthLoginTokenReceived((*reinterpret_cast< const QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 28: _t->onAuthLoginSuccess(); break;
        case 29: _t->onDcMigrated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: _t->onRpcErrorReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 31: _t->onSessionError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 32: _t->onQrPollTimer(); break;
        case 33: _t->onMyProfileReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Telegram::Controllers::AuthController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Telegram::Controllers::AuthController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Telegram__Controllers__AuthController,
      qt_meta_data_Telegram__Controllers__AuthController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Telegram::Controllers::AuthController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Telegram::Controllers::AuthController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Telegram::Controllers::AuthController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Telegram__Controllers__AuthController))
        return static_cast<void*>(const_cast< AuthController*>(this));
    return QObject::qt_metacast(_clname);
}

int Telegram::Controllers::AuthController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = authState(); break;
        case 1: *reinterpret_cast< QString*>(_v) = authStateString(); break;
        case 2: *reinterpret_cast< QString*>(_v) = phoneNumber(); break;
        case 3: *reinterpret_cast< QString*>(_v) = phoneCodeHash(); break;
        case 4: *reinterpret_cast< QString*>(_v) = deliveryType(); break;
        case 5: *reinterpret_cast< int*>(_v) = codeTimeout(); break;
        case 6: *reinterpret_cast< QString*>(_v) = passwordHint(); break;
        case 7: *reinterpret_cast< bool*>(_v) = isBusy(); break;
        case 8: *reinterpret_cast< QString*>(_v) = statusMessage(); break;
        case 9: *reinterpret_cast< QString*>(_v) = userName(); break;
        case 10: *reinterpret_cast< QString*>(_v) = userHandle(); break;
        case 11: *reinterpret_cast< QString*>(_v) = userId(); break;
        case 12: *reinterpret_cast< QString*>(_v) = userPhone(); break;
        case 13: *reinterpret_cast< QString*>(_v) = userBio(); break;
        case 14: *reinterpret_cast< QString*>(_v) = qrTokenUrl(); break;
        case 15: *reinterpret_cast< QString*>(_v) = qrImagePath(); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: setPhoneNumber(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 16;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Telegram::Controllers::AuthController::authStateChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Telegram::Controllers::AuthController::phoneNumberChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Telegram::Controllers::AuthController::phoneCodeHashChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Telegram::Controllers::AuthController::deliveryTypeChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Telegram::Controllers::AuthController::codeTimeoutChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Telegram::Controllers::AuthController::passwordHintChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Telegram::Controllers::AuthController::isBusyChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Telegram::Controllers::AuthController::statusMessageChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Telegram::Controllers::AuthController::userProfileChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void Telegram::Controllers::AuthController::qrTokenUrlChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Telegram::Controllers::AuthController::qrImagePathChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Telegram::Controllers::AuthController::authErrorOccurred(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_END_MOC_NAMESPACE
