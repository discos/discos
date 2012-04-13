/****************************************************************************
** Meta object code from reading C++ file 'XControlSystem.h'
**
** Created: Fri May 21 18:12:31 2010
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/XControlSystem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'XControlSystem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_XControlSystem[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      23,   16,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_XControlSystem[] = {
    "XControlSystem\0\0status\0upState(char)\0"
};

const QMetaObject XControlSystem::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_XControlSystem,
      qt_meta_data_XControlSystem, 0 }
};

const QMetaObject *XControlSystem::metaObject() const
{
    return &staticMetaObject;
}

void *XControlSystem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_XControlSystem))
	return static_cast<void*>(const_cast< XControlSystem*>(this));
    if (!strcmp(_clname, "POA_ACS::CBpattern"))
	return static_cast< POA_ACS::CBpattern*>(const_cast< XControlSystem*>(this));
    return QThread::qt_metacast(_clname);
}

int XControlSystem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: upState((*reinterpret_cast< char(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void XControlSystem::upState(char _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
