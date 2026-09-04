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
      29,   14, // methods
      17,  159, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: signature, parameters, type, tag, flags
      51,   43,   42,   42, 0x05,
      78,   72,   42,   42, 0x05,
     102,   96,   42,   42, 0x05,
     130,   42,   42,   42, 0x05,
     185,  152,   42,   42, 0x05,
     224,   42,   42,   42, 0x05,
     248,  241,   42,   42, 0x05,
     273,   42,   42,   42, 0x05,
     289,   42,   42,   42, 0x05,

 // slots: signature, parameters, type, tag, flags
     314,  306,   42,   42, 0x0a,
     368,  352,   42,   42, 0x0a,
     430,  413,   42,   42, 0x0a,
     465,   42,   42,   42, 0x0a,
     509,  485,   42,   42, 0x0a,
     569,  554,   42,   42, 0x0a,
     592,   42,   42,   42, 0x0a,
     633,  607,   42,   42, 0x0a,
     697,  684,   42,   42, 0x0a,

 // methods: signature, parameters, type, tag, flags
     746,  241,   42,   42, 0x02,
     767,   42,   42,   42, 0x02,
     794,  784,   42,   42, 0x02,
     821,  152,   42,   42, 0x02,
     891,  858,   42,   42, 0x02,
     962,  924,   42,   42, 0x02,
    1013, 1003,   42,   42, 0x02,
    1052, 1048,   42,   42, 0x02,
    1085, 1080, 1075,   42, 0x02,
    1138, 1105,   42,   42, 0x02,
    1203, 1179,   42,   42, 0x22,

 // properties: name, type, flags
    1266, 1236, 0x00095409,
    1272, 1236, 0x00095409,
    1286, 1075, 0x01495001,
    1300, 1296, 0x02495001,
    1321, 1313, 0x0a495103,
    1333, 1313, 0x0a495001,
    1358, 1351, 0x04495001,
    1373, 1075, 0x01495001,
    1381, 1296, 0x02495103,
    1394, 1296, 0x02495001,
    1412, 1296, 0x02495001,
    1432, 1296, 0x02495001,
    1449, 1236, 0x00095409,
    1466, 1313, 0x0a495001,
    1477, 1313, 0x0a495001,
    1493, 1313, 0x0a495001,
    1506, 1313, 0x0a495001,

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
       0,
       8,
       8,
       8,
       8,

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
    "profileChanged()\0dialogs\0"
    "onDialogsReceived(QList<QVariantMap>)\0"
    "peerId,messages\0"
    "onHistoryReceived(qint64,QList<QVariantMap>)\0"
    "peerId,localPath\0onAvatarDownloaded(qint64,QString)\0"
    "onSessionRestored()\0peerId,peerType,message\0"
    "onNewMessageReceived(qint64,int,QVariantMap)\0"
    "messageId,date\0onMessageSent(int,int)\0"
    "retryDialogs()\0userId,bio,username,phone\0"
    "onUserFullReceived(qint64,QString,QString,QString)\0"
    "userId,chats\0"
    "onCommonChatsReceived(qint64,QList<QVariantMap>)\0"
    "setFolderFilter(int)\0refreshDialogs()\0"
    "indexPath\0selectDialog(QVariantList)\0"
    "openChat(qint64,int,QString,quint64)\0"
    "peerType,peerIdStr,accessHashStr\0"
    "loadHistory(int,QString,QString)\0"
    "peerType,peerIdStr,accessHashStr,text\0"
    "sendMessage(int,QString,QString,QString)\0"
    "text,time\0addInitialMessage(QString,QString)\0"
    "msg\0logDiagnostic(QString)\0bool\0path\0"
    "fileExists(QString)\0"
    "peerIdStr,accessHashStr,username\0"
    "loadUserProfile(QString,QString,QString)\0"
    "peerIdStr,accessHashStr\0"
    "loadUserProfile(QString,QString)\0"
    "bb::cascades::GroupDataModel*\0model\0"
    "messagesModel\0isLoading\0int\0dialogsCount\0"
    "QString\0searchQuery\0selectedPeerTitle\0"
    "qint64\0selectedPeerId\0canSend\0"
    "folderFilter\0unreadGroupsCount\0"
    "unreadChannelsCount\0unreadTotalCount\0"
    "commonChatsModel\0profileBio\0profileUsername\0"
    "profilePhone\0profileStatus\0"
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
        case 8: _t->profileChanged(); break;
        case 9: _t->onDialogsReceived((*reinterpret_cast< const QList<QVariantMap>(*)>(_a[1]))); break;
        case 10: _t->onHistoryReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QList<QVariantMap>(*)>(_a[2]))); break;
        case 11: _t->onAvatarDownloaded((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 12: _t->onSessionRestored(); break;
        case 13: _t->onNewMessageReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QVariantMap(*)>(_a[3]))); break;
        case 14: _t->onMessageSent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->retryDialogs(); break;
        case 16: _t->onUserFullReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 17: _t->onCommonChatsReceived((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QList<QVariantMap>(*)>(_a[2]))); break;
        case 18: _t->setFolderFilter((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->refreshDialogs(); break;
        case 20: _t->selectDialog((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 21: _t->openChat((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< quint64(*)>(_a[4]))); break;
        case 22: _t->loadHistory((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 23: _t->sendMessage((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 24: _t->addInitialMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 25: _t->logDiagnostic((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 26: { bool _r = _t->fileExists((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 27: _t->loadUserProfile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 28: _t->loadUserProfile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
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
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
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
        case 12: *reinterpret_cast< bb::cascades::GroupDataModel**>(_v) = commonChatsModel(); break;
        case 13: *reinterpret_cast< QString*>(_v) = profileBio(); break;
        case 14: *reinterpret_cast< QString*>(_v) = profileUsername(); break;
        case 15: *reinterpret_cast< QString*>(_v) = profilePhone(); break;
        case 16: *reinterpret_cast< QString*>(_v) = profileStatus(); break;
        }
        _id -= 17;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 4: setSearchQuery(*reinterpret_cast< QString*>(_v)); break;
        case 8: setFolderFilter(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 17;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 17;
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

// SIGNAL 8
void Telegram::Controllers::ChatListController::profileChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}
QT_END_MOC_NAMESPACE
