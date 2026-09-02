/****************************************************************************
** Meta object code from reading C++ file 'ChatListController.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/controllers/ChatListController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChatListController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Telegram__Controllers__ChatListController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       7,   84, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      51,   43,   42,   42, 0x05,
      78,   72,   42,   42, 0x05,
     102,   96,   42,   42, 0x05,
     130,   42,   42,   42, 0x05,
     185,  152,   42,   42, 0x05,

 // slots: signature, parameters, type, tag, flags
     232,  224,   42,   42, 0x0a,
     286,  270,   42,   42, 0x0a,
     331,   42,   42,   42, 0x0a,

 // methods: signature, parameters, type, tag, flags
     351,   42,   42,   42, 0x02,
     378,  368,   42,   42, 0x02,
     405,  152,   42,   42, 0x02,
     475,  442,   42,   42, 0x02,
     546,  508,   42,   42, 0x02,
     597,  587,   42,   42, 0x02,

 // properties: name, type, flags
     662,  632, 0x00095409,
     668,  632, 0x00095409,
     687,  682, 0x01495001,
     701,  697, 0x02495001,
     722,  714, 0x0a495103,
     734,  714, 0x0a495001,
     759,  752, 0x04495001,

 // properties: notify_signal_id
       0,
       0,
       0,
       1,
       2,
       3,
       3,

       0        // eod
};

static const char qt_meta_stringdata_Telegram__Controllers__ChatListController[] = {
    "Telegram::Controllers::ChatListController\0"
    "\0loading\0loadingChanged(bool)\0count\0"
    "countChanged(int)\0query\0"
    "searchQueryChanged(QString)\0"
    "selectedPeerChanged()\0"
    "peerId,peerType,title,accessHash\0"
    "chatOpened(qint64,int,QString,quint64)\0"
    "dialogs\0onDialogsReceived(QList<QVariantMap>)\0"
    "peerId,messages\0"
    "onHistoryReceived(qint64,QList<QVariantMap>)\0"
    "onSessionRestored()\0refreshDialogs()\0"
    "indexPath\0selectDialog(QVariantList)\0"
    "openChat(qint64,int,QString,quint64)\0"
    "peerType,peerIdStr,accessHashStr\0"
    "loadHistory(int,QString,QString)\0"
    "peerType,peerIdStr,accessHashStr,text\0"
    "sendMessage(int,QString,QString,QString)\0"
    "text,time\0addInitialMessage(QString,QString)\0"
    "bb::cascades::GroupDataModel*\0model\0"
    "messagesModel\0bool\0isLoading\0int\0"
    "dialogsCount\0QString\0searchQuery\0"
    "selectedPeerTitle\0qint64\0selectedPeerId\0"
};

void Telegram::Controllers::ChatListController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChatListController *_t = static_cast<ChatListController *>(_o);
        switch (_id) {
        case 0: _t->loadingChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->countChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->searchQueryChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->selectedPeerChanged(); break;
        case 4: _t->chatOpened((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< quint64(*)>(_a[4]))); break;
        case 5: _t->onDialogsReceived((*reinterpret_cast< const QList<QVariantMap>(*)>(_a[1]))); break;
        case 6: _t->onHistoryReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QList<QVariantMap>(*)>(_a[2]))); break;
        case 7: _t->onSessionRestored(); break;
        case 8: _t->refreshDialogs(); break;
        case 9: _t->selectDialog((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 10: _t->openChat((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< quint64(*)>(_a[4]))); break;
        case 11: _t->loadHistory((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 12: _t->sendMessage((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 13: _t->addInitialMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Telegram::Controllers::ChatListController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Telegram::Controllers::ChatListController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Telegram__Controllers__ChatListController,
      qt_meta_data_Telegram__Controllers__ChatListController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Telegram::Controllers::ChatListController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Telegram::Controllers::ChatListController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Telegram::Controllers::ChatListController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Telegram__Controllers__ChatListController))
        return static_cast<void*>(const_cast< ChatListController*>(this));
    return QObject::qt_metacast(_clname);
}

int Telegram::Controllers::ChatListController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bb::cascades::GroupDataModel**>(_v) = model(); break;
        case 1: *reinterpret_cast< bb::cascades::GroupDataModel**>(_v) = messagesModel(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isLoading(); break;
        case 3: *reinterpret_cast< int*>(_v) = dialogsCount(); break;
        case 4: *reinterpret_cast< QString*>(_v) = searchQuery(); break;
        case 5: *reinterpret_cast< QString*>(_v) = selectedPeerTitle(); break;
        case 6: *reinterpret_cast< qint64*>(_v) = selectedPeerId(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 4: setSearchQuery(*reinterpret_cast< QString*>(_v)); break;
        }
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
void Telegram::Controllers::ChatListController::loadingChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Telegram::Controllers::ChatListController::countChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Telegram::Controllers::ChatListController::searchQueryChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Telegram::Controllers::ChatListController::selectedPeerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Telegram::Controllers::ChatListController::chatOpened(qint64 _t1, int _t2, const QString & _t3, quint64 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
