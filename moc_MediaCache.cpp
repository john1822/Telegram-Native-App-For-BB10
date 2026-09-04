/****************************************************************************
** Meta object code from reading C++ file 'MediaCache.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/storage/MediaCache.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MediaCache.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Telegram__Storage__MediaCache[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      48,   31,   30,   30, 0x05,
      99,   81,   30,   30, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Telegram__Storage__MediaCache[] = {
    "Telegram::Storage::MediaCache\0\0"
    "peerId,localPath\0avatarDownloaded(qint64,QString)\0"
    "photoId,localPath\0photoDownloaded(qint64,QString)\0"
};

void Telegram::Storage::MediaCache::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MediaCache *_t = static_cast<MediaCache *>(_o);
        switch (_id) {
        case 0: _t->avatarDownloaded((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->photoDownloaded((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Telegram::Storage::MediaCache::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Telegram::Storage::MediaCache::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Telegram__Storage__MediaCache,
      qt_meta_data_Telegram__Storage__MediaCache, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Telegram::Storage::MediaCache::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Telegram::Storage::MediaCache::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Telegram::Storage::MediaCache::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Telegram__Storage__MediaCache))
        return static_cast<void*>(const_cast< MediaCache*>(this));
    return QObject::qt_metacast(_clname);
}

int Telegram::Storage::MediaCache::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Telegram::Storage::MediaCache::avatarDownloaded(qint64 _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Telegram::Storage::MediaCache::photoDownloaded(qint64 _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
