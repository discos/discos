/****************************************************************************
** Meta object code from reading C++ file 'MedicinaActiveSurfaceCore.h'
**
** Created: Wed Dec 7 14:02:43 2022
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/MedicinaActiveSurfaceCore.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MedicinaActiveSurfaceCore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MedicinaActiveSurfaceCore[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      35,   31,   27,   26, 0x05,
      91,   74,   27,   26, 0x05,
     116,   26,   27,   26, 0x25,
     137,   74,   27,   26, 0x05,
     164,   26,   27,   26, 0x25,
     187,   74,   27,   26, 0x05,
     208,   26,   27,   26, 0x25,
     225,   26,   27,   26, 0x05,
     257,   26,   27,   26, 0x05,
     280,   26,   27,   26, 0x05,
     309,   26,   27,   26, 0x05,
     333,   26,   27,   26, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_MedicinaActiveSurfaceCore[] = {
    "MedicinaActiveSurfaceCore\0\0int\0,,,\0"
    "setGUIActuatorColor(int,int,bool,bool)\0"
    "callfromfunction\0setGUIAllActuators(bool)\0"
    "setGUIAllActuators()\0setGUIcircleORradius(bool)\0"
    "setGUIcircleORradius()\0setGUIActuator(bool)\0"
    "setGUIActuator()\0setGUIActuatorStatusEnblLabel()\0"
    "setGUIActuatorValues()\0"
    "setGUIActuatorStatusLabels()\0"
    "setGUIasStatusCode(int)\0"
    "setGUIasProfileCode(int)\0"
};

const QMetaObject MedicinaActiveSurfaceCore::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_MedicinaActiveSurfaceCore,
      qt_meta_data_MedicinaActiveSurfaceCore, 0 }
};

const QMetaObject *MedicinaActiveSurfaceCore::metaObject() const
{
    return &staticMetaObject;
}

void *MedicinaActiveSurfaceCore::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MedicinaActiveSurfaceCore))
        return static_cast<void*>(const_cast< MedicinaActiveSurfaceCore*>(this));
    return QThread::qt_metacast(_clname);
}

int MedicinaActiveSurfaceCore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { int _r = setGUIActuatorColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: { int _r = setGUIAllActuators((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: { int _r = setGUIAllActuators();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: { int _r = setGUIcircleORradius((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: { int _r = setGUIcircleORradius();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = setGUIActuator((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { int _r = setGUIActuator();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: { int _r = setGUIActuatorStatusEnblLabel();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: { int _r = setGUIActuatorValues();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 9: { int _r = setGUIActuatorStatusLabels();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 10: { int _r = setGUIasStatusCode((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 11: { int _r = setGUIasProfileCode((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
int MedicinaActiveSurfaceCore::setGUIActuatorColor(int _t1, int _t2, bool _t3, bool _t4)
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
    return _t0;
}

// SIGNAL 1
int MedicinaActiveSurfaceCore::setGUIAllActuators(bool _t1)
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, 2, _a);
    return _t0;
}

// SIGNAL 3
int MedicinaActiveSurfaceCore::setGUIcircleORradius(bool _t1)
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, 4, _a);
    return _t0;
}

// SIGNAL 5
int MedicinaActiveSurfaceCore::setGUIActuator(bool _t1)
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, 6, _a);
    return _t0;
}

// SIGNAL 7
int MedicinaActiveSurfaceCore::setGUIActuatorStatusEnblLabel()
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
    return _t0;
}

// SIGNAL 8
int MedicinaActiveSurfaceCore::setGUIActuatorValues()
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
    return _t0;
}

// SIGNAL 9
int MedicinaActiveSurfaceCore::setGUIActuatorStatusLabels()
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
    return _t0;
}

// SIGNAL 10
int MedicinaActiveSurfaceCore::setGUIasStatusCode(int _t1)
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
    return _t0;
}

// SIGNAL 11
int MedicinaActiveSurfaceCore::setGUIasProfileCode(int _t1)
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
    return _t0;
}
QT_END_MOC_NAMESPACE
