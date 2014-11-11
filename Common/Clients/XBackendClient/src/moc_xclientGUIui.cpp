/****************************************************************************
** Meta object code from reading C++ file 'xclientGUIui.h'
**
** Created: Fri May 21 18:12:31 2010
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/xclientGUIui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'xclientGUIui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_xclientGUIui[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      18,   13,   14,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   44,   13,   13, 0x0a,
      64,   13,   13,   13, 0x0a,
      75,   13,   13,   13, 0x0a,
     102,   92,   87,   13, 0x0a,
     126,   13,   13,   13, 0x0a,
     134,   13,   13,   13, 0x0a,
     144,   13,   13,   13, 0x0a,
     155,   13,   13,   13, 0x0a,
     166,   13,   13,   13, 0x0a,
     178,   13,   13,   13, 0x0a,
     189,   13,   13,   13, 0x0a,
     212,   13,   13,   13, 0x0a,
     242,  235,   13,   13, 0x0a,
     261,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_xclientGUIui[] = {
    "xclientGUIui\0\0int\0setGUICatchAllErrorCode()\0"
    "N\0Rivisualizza(int)\0FileExit()\0"
    "OpenXplot()\0bool\0path,name\0"
    "OpenFile(char[],char[])\0Check()\0"
    "Integra()\0StartInt()\0AbortInt()\0"
    "Broadcast()\0Modifica()\0Successivo_activated()\0"
    "Precedente_activated()\0status\0"
    "UpdateStatus(char)\0changeGUICatchAllErrorCode()\0"
};

const QMetaObject xclientGUIui::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_xclientGUIui,
      qt_meta_data_xclientGUIui, 0 }
};

const QMetaObject *xclientGUIui::metaObject() const
{
    return &staticMetaObject;
}

void *xclientGUIui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_xclientGUIui))
	return static_cast<void*>(const_cast< xclientGUIui*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int xclientGUIui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { int _r = setGUICatchAllErrorCode();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: Rivisualizza((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: FileExit(); break;
        case 3: OpenXplot(); break;
        case 4: { bool _r = OpenFile((*reinterpret_cast< char(*)[]>(_a[1])),(*reinterpret_cast< char(*)[]>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: Check(); break;
        case 6: Integra(); break;
        case 7: StartInt(); break;
        case 8: AbortInt(); break;
        case 9: Broadcast(); break;
        case 10: Modifica(); break;
        case 11: Successivo_activated(); break;
        case 12: Precedente_activated(); break;
        case 13: UpdateStatus((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 14: changeGUICatchAllErrorCode(); break;
        }
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
int xclientGUIui::setGUICatchAllErrorCode()
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
    return _t0;
}
