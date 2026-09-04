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
      23,   14, // methods
      12,  129, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      51,   43,   42,   42, 0x05,
      78,   72,   42,   42, 0x05,
     102,   96,   42,   42, 0x05,
     130,   42,   42,   42, 0x05,
     185,  152,   42,   42, 0x05,
     224,   42,   42,   42, 0x05,
     248,  241,   42,   42, 0x05,
     273,   42,   42,   42, 0x05,

 // slots: signature, parameters, type, tag, flags
     297,  289,   42,   42, 0x0a,
     351,  335,   42,   42, 0x0a,
     413,  396,   42,   42, 0x0a,
     448,   42,   42,   42, 0x0a,
     492,  468,   42,   42, 0x0a,
     552,  537,   42,   42, 0x0a,
     575,   42,   42,   42, 0x0a,

 // methods: signature, parameters, type, tag, flags
     590,  241,   42,   42, 0x02,
     611,   42,   42,   42, 0x02,
     638,  628,   42,   42, 0x02,
     665,  152,   42,   42, 0x02,
     735,  702,   42,   42, 0x02,
     806,  768,   42,   42, 0x02,
     857,  847,   42,   42, 0x02,
     896,  892,   42,   42, 0x02,

 // properties: name, type, flags
     949,  919, 0x00095409,
     955,  919, 0x00095409,
     974,  969, 0x01495001,
     988,  984, 0x02495001,
    1009, 1001, 0x0a495103,
    1021, 1001, 0x0a495001,
    1046, 1039, 0x04495001,
    1061,  969, 0x01495001,
    1069,  984, 0x02495103,
    1082,  984, 0x02495001,
    1100,  984, 0x02495001,
    1120,  984, 0x02495001,

 // properties: notify_signal_id
       0,
       0,
       0,
       1,
       2,
       3,
       3,
       5,
       6,
       7,
       7,
       7,

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
    "canSendChanged()\0folder\0"
    "folderFilterChanged(int)\0countsUpdated()\0"
    "dialogs\0onDialogsReceived(QList<QVariantMap>)\0"
    "peerId,messages\0"
    "onHistoryReceived(qint64,QList<QVariantMap>)\0"
    "peerId,localPath\0onAvatarDownloaded(qint64,QString)\0"
    "onSessionRestored()\0peerId,peerType,message\0"
    "onNewMessageReceived(qint64,int,QVariantMap)\0"
    "messageId,date\0onMessageSent(int,int)\0"
    "retryDialogs()\0setFolderFilter(int)\0"
    "refreshDialogs()\0indexPath\0"
    "selectDialog(QVariantList)\0"
    "openChat(qint64,int,QString,quint64)\0"
    "peerType,peerIdStr,accessHashStr\0"
    "loadHistory(int,QString,QString)\0"
    "peerType,peerIdStr,accessHashStr,text\0"
    "sendMessage(int,QString,QString,QString)\0"
    "text,time\0addInitialMessage(QString,QString)\0"
    "msg\0logDiagnostic(QString)\0"
    "bb::cascades::GroupDataModel*\0model\0"
    "messagesModel\0bool\0isLoading\0int\0"
    "dialogsCount\0QString\0searchQuery\0"
    "selectedPeerTitle\0qint64\0selectedPeerId\0"
    "canSend\0folderFilter\0unreadGroupsCount\0"
    "unreadChannelsCount\0unreadTotalCount\0"
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
        case 5: _t->canSendChanged(); break;
        case 6: _t->folderFilterChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->countsUpdated(); break;
        case 8: _t->onDialogsReceived((*reinterpret_cast< const QList<QVariantMap>(*)>(_a[1]))); break;
        case 9: _t->onHistoryReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QList<QVariantMap>(*)>(_a[2]))); break;
        case 10: _t->onAvatarDownloaded((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 11: _t->onSessionRestored(); break;
        case 12: _t->onNewMessageReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QVariantMap(*)>(_a[3]))); break;
        case 13: _t->onMessageSent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 14: _t->retryDialogs(); break;
        case 15: _t->setFolderFilter((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->refreshDialogs(); break;
        case 17: _t->selectDialog((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 18: _t->openChat((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< quint64(*)>(_a[4]))); break;
        case 19: _t->loadHistory((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 20: _t->sendMessage((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 21: _t->addInitialMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 22: _t->logDiagnostic((*reinterpret_cast< const QString(*)>(_a[1]))); break;
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
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
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
        case 7: *reinterpret_cast< bool*>(_v) = canSend(); break;
        case 8: *reinterpret_cast< int*>(_v) = folderFilter(); break;
        case 9: *reinterpret_cast< int*>(_v) = unreadGroupsCount(); break;
        case 10: *reinterpret_cast< int*>(_v) = unreadChannelsCount(); break;
        case 11: *reinterpret_cast< int*>(_v) = unreadTotalCount(); break;
        }
        _id -= 12;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 4: setSearchQuery(*reinterpret_cast< QString*>(_v)); break;
        case 8: setFolderFilter(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 12;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 12;
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

// SIGNAL 5
void Telegram::Controllers::ChatListController::canSendChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void Telegram::Controllers::ChatListController::folderFilterChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Telegram::Controllers::ChatListController::countsUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}
QT_END_MOC_NAMESPACE
