/****************************************************************************
** Meta object code from reading C++ file 'plot.h'
**
** Created: Thu Dec 10 11:02:51 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/plot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_Plot[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,    6,    5,    5, 0x0a,
      60,   46,    5,    5, 0x0a,
     117,   97,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Plot[] = {
    "Plot\0\0x,y1,count\0setDamp(double*,double*,int)\0x,y1,y2,count\0"
    "setDamp(double*,double*,double*,int)\0x,y1,y2,y3,y4,count\0"
    "setDamp(double*,double*,double*,double*,double*,int)\0"
};

const QMetaObject Plot::staticMetaObject = {
    { &QwtPlot::staticMetaObject, qt_meta_stringdata_Plot,
      qt_meta_data_Plot, 0 }
};

const QMetaObject *Plot::metaObject() const
{
    return &staticMetaObject;
}

void *Plot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Plot))
	return static_cast<void*>(const_cast<Plot*>(this));
    return QwtPlot::qt_metacast(_clname);
}

int Plot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setDamp((*reinterpret_cast< double*(*)>(_a[1])),(*reinterpret_cast< double*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: setDamp((*reinterpret_cast< double*(*)>(_a[1])),(*reinterpret_cast< double*(*)>(_a[2])),(*reinterpret_cast< double*(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 2: setDamp((*reinterpret_cast< double*(*)>(_a[1])),(*reinterpret_cast< double*(*)>(_a[2])),(*reinterpret_cast< double*(*)>(_a[3])),(*reinterpret_cast< double*(*)>(_a[4])),(*reinterpret_cast< double*(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6]))); break;
        }
        _id -= 3;
    }
    return _id;
}
