/****************************************************************************
** Meta object code from reading C++ file 'xplot.h'
**
** Created: Fri May 21 18:12:31 2010
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/xplot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'xplot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_Xplot[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x0a,
      30,    6,    6,    6, 0x0a,
      57,   53,    6,    6, 0x0a,
      69,    6,    6,    6, 0x0a,
      80,    6,    6,    6, 0x0a,
      98,   95,    6,    6, 0x0a,
     126,    6,    6,    6, 0x0a,
     143,  139,    6,    6, 0x08,
     161,  157,    6,    6, 0x08,
     185,  182,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Xplot[] = {
    "Xplot\0\0Successivo_activated()\0"
    "Precedente_activated()\0tmp\0Visual(int)\0"
    "initPlot()\0plotDataZero()\0tp\0"
    "plotData(DataIntegrazione*)\0enableZoom()\0"
    "pos\0moved(QPoint)\0pol\0selected(QwtPolygon)\0"
    "on\0enableZoomMode(bool)\0"
};

const QMetaObject Xplot::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Xplot,
      qt_meta_data_Xplot, 0 }
};

const QMetaObject *Xplot::metaObject() const
{
    return &staticMetaObject;
}

void *Xplot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Xplot))
	return static_cast<void*>(const_cast< Xplot*>(this));
    return QWidget::qt_metacast(_clname);
}

int Xplot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: Successivo_activated(); break;
        case 1: Precedente_activated(); break;
        case 2: Visual((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: initPlot(); break;
        case 4: plotDataZero(); break;
        case 5: plotData((*reinterpret_cast< DataIntegrazione*(*)>(_a[1]))); break;
        case 6: enableZoom(); break;
        case 7: moved((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 8: selected((*reinterpret_cast< const QwtPolygon(*)>(_a[1]))); break;
        case 9: enableZoomMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 10;
    }
    return _id;
}
