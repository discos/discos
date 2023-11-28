/****************************************************************************
** Meta object code from reading C++ file 'MedicinaActiveSurfaceGUIui.h'
**
** Created: Wed Dec 7 14:02:43 2022
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/MedicinaActiveSurfaceGUIui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MedicinaActiveSurfaceGUIui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MedicinaActiveSurfaceGUI[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      36,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x0a,
      33,   25,   25,   25, 0x0a,
      40,   25,   25,   25, 0x0a,
      53,   25,   25,   25, 0x0a,
      62,   25,   25,   25, 0x0a,
      70,   25,   25,   25, 0x0a,
      77,   25,   25,   25, 0x0a,
      82,   25,   25,   25, 0x0a,
      89,   25,   25,   25, 0x0a,
      95,   25,   25,   25, 0x0a,
     104,   25,   25,   25, 0x0a,
     116,   25,   25,   25, 0x0a,
     125,   25,   25,   25, 0x0a,
     132,   25,   25,   25, 0x0a,
     140,   25,   25,   25, 0x0a,
     149,   25,   25,   25, 0x0a,
     162,   25,   25,   25, 0x0a,
     172,   25,   25,   25, 0x0a,
     182,   25,   25,   25, 0x0a,
     191,   25,   25,   25, 0x0a,
     200,   25,   25,   25, 0x0a,
     218,   25,   25,   25, 0x0a,
     230,   25,   25,   25, 0x0a,
     242,   25,   25,   25, 0x0a,
     260,  256,   25,   25, 0x08,
     319,  302,   25,   25, 0x08,
     347,   25,   25,   25, 0x28,
     371,  302,   25,   25, 0x08,
     401,   25,   25,   25, 0x28,
     427,  302,   25,   25, 0x08,
     451,   25,   25,   25, 0x28,
     471,   25,   25,   25, 0x08,
     506,   25,   25,   25, 0x08,
     532,   25,   25,   25, 0x08,
     564,   25,   25,   25, 0x08,
     591,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MedicinaActiveSurfaceGUI[] = {
    "MedicinaActiveSurfaceGUI\0\0Quit()\0"
    "move()\0correction()\0update()\0reset()\0"
    "stop()\0up()\0down()\0top()\0bottom()\0"
    "calibrate()\0calVer()\0stow()\0setup()\0"
    "refPos()\0recoverUSD()\0setupAS()\0"
    "startAS()\0stowAS()\0stopAS()\0"
    "setallactuators()\0setradius()\0setcircle()\0"
    "setactuator()\0,,,\0"
    "changeGUIActuatorColor(int,int,bool,bool)\0"
    "callfromfunction\0changeGUIAllActuators(bool)\0"
    "changeGUIAllActuators()\0"
    "changeGUIcircleORradius(bool)\0"
    "changeGUIcircleORradius()\0"
    "changeGUIActuator(bool)\0changeGUIActuator()\0"
    "changeGUIActuatorStatusEnblLabel()\0"
    "changeGUIActuatorValues()\0"
    "changeGUIActuatorStatusLabels()\0"
    "changeGUIasStatusCode(int)\0"
    "changeGUIasProfileCode(int)\0"
};

const QMetaObject MedicinaActiveSurfaceGUI::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MedicinaActiveSurfaceGUI,
      qt_meta_data_MedicinaActiveSurfaceGUI, 0 }
};

const QMetaObject *MedicinaActiveSurfaceGUI::metaObject() const
{
    return &staticMetaObject;
}

void *MedicinaActiveSurfaceGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MedicinaActiveSurfaceGUI))
        return static_cast<void*>(const_cast< MedicinaActiveSurfaceGUI*>(this));
    if (!strcmp(_clname, "Ui_MedicinaActiveSurfaceGUI"))
        return static_cast< Ui_MedicinaActiveSurfaceGUI*>(const_cast< MedicinaActiveSurfaceGUI*>(this));
    return QWidget::qt_metacast(_clname);
}

int MedicinaActiveSurfaceGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: Quit(); break;
        case 1: move(); break;
        case 2: correction(); break;
        case 3: update(); break;
        case 4: reset(); break;
        case 5: stop(); break;
        case 6: up(); break;
        case 7: down(); break;
        case 8: top(); break;
        case 9: bottom(); break;
        case 10: calibrate(); break;
        case 11: calVer(); break;
        case 12: stow(); break;
        case 13: setup(); break;
        case 14: refPos(); break;
        case 15: recoverUSD(); break;
        case 16: setupAS(); break;
        case 17: startAS(); break;
        case 18: stowAS(); break;
        case 19: stopAS(); break;
        case 20: setallactuators(); break;
        case 21: setradius(); break;
        case 22: setcircle(); break;
        case 23: setactuator(); break;
        case 24: changeGUIActuatorColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 25: changeGUIAllActuators((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: changeGUIAllActuators(); break;
        case 27: changeGUIcircleORradius((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: changeGUIcircleORradius(); break;
        case 29: changeGUIActuator((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 30: changeGUIActuator(); break;
        case 31: changeGUIActuatorStatusEnblLabel(); break;
        case 32: changeGUIActuatorValues(); break;
        case 33: changeGUIActuatorStatusLabels(); break;
        case 34: changeGUIasStatusCode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 35: changeGUIasProfileCode((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 36;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
