/****************************************************************************
** MedicinaMount meta object code from reading C++ file 'MedicinaMount.h'
**
** Created: Tue Apr 26 17:26:31 2011
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../include/MedicinaMount.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MedicinaMount::className() const
{
    return "MedicinaMount";
}

QMetaObject *MedicinaMount::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MedicinaMount( "MedicinaMount", &MedicinaMount::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MedicinaMount::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MedicinaMount", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MedicinaMount::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MedicinaMount", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MedicinaMount::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QThread::staticMetaObject();
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_0 = {"setGUIAntennaMountStatusColor", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_1 = {"setGUIACUStatusColor", 1, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_2 = {"setGUIPositionAndErrorValue", 1, param_signal_2 };
    static const QUParameter param_signal_3[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_3 = {"setGUIACUTimeValue", 1, param_signal_3 };
    static const QUParameter param_signal_4[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_4 = {"setGUIAntennaSectionValue", 1, param_signal_4 };
    static const QUParameter param_signal_5[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_5 = {"setGUIServoSystemColors", 1, param_signal_5 };
    static const QUParameter param_signal_6[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_6 = {"setGUIAzimuthElevationServoStatusColors", 1, param_signal_6 };
    static const QUParameter param_signal_7[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_7 = {"setGUIAzimuthElevationOperatingModeColors", 1, param_signal_7 };
    static const QUParameter param_signal_8[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_8 = {"setGUIPropertyErrorCode", 1, param_signal_8 };
    static const QUParameter param_signal_9[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out }
    };
    static const QUMethod signal_9 = {"setGUICatchAllErrorCode", 1, param_signal_9 };
    static const QMetaData signal_tbl[] = {
	{ "setGUIAntennaMountStatusColor()", &signal_0, QMetaData::Public },
	{ "setGUIACUStatusColor()", &signal_1, QMetaData::Public },
	{ "setGUIPositionAndErrorValue()", &signal_2, QMetaData::Public },
	{ "setGUIACUTimeValue()", &signal_3, QMetaData::Public },
	{ "setGUIAntennaSectionValue()", &signal_4, QMetaData::Public },
	{ "setGUIServoSystemColors()", &signal_5, QMetaData::Public },
	{ "setGUIAzimuthElevationServoStatusColors()", &signal_6, QMetaData::Public },
	{ "setGUIAzimuthElevationOperatingModeColors()", &signal_7, QMetaData::Public },
	{ "setGUIPropertyErrorCode()", &signal_8, QMetaData::Public },
	{ "setGUICatchAllErrorCode()", &signal_9, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"MedicinaMount", parentObject,
	0, 0,
	signal_tbl, 10,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MedicinaMount.setMetaObject( metaObj );
    return metaObj;
}

void* MedicinaMount::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MedicinaMount" ) )
	return this;
    return QThread::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL setGUIAntennaMountStatusColor
int MedicinaMount::setGUIAntennaMountStatusColor()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUIACUStatusColor
int MedicinaMount::setGUIACUStatusColor()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUIPositionAndErrorValue
int MedicinaMount::setGUIPositionAndErrorValue()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUIACUTimeValue
int MedicinaMount::setGUIACUTimeValue()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 3 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUIAntennaSectionValue
int MedicinaMount::setGUIAntennaSectionValue()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 4 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUIServoSystemColors
int MedicinaMount::setGUIServoSystemColors()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 5 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUIAzimuthElevationServoStatusColors
int MedicinaMount::setGUIAzimuthElevationServoStatusColors()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 6 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUIAzimuthElevationOperatingModeColors
int MedicinaMount::setGUIAzimuthElevationOperatingModeColors()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 7 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUIPropertyErrorCode
int MedicinaMount::setGUIPropertyErrorCode()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 8 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

// SIGNAL setGUICatchAllErrorCode
int MedicinaMount::setGUICatchAllErrorCode()
{
    int something;
    if ( signalsBlocked() )
	return something;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 9 );
    if ( !clist )
	return something;
    QUObject o[1];
    static_QUType_int.set(o,something);
    activate_signal( clist, o );
    return static_QUType_int.get(o);
}

bool MedicinaMount::qt_invoke( int _id, QUObject* _o )
{
    return QThread::qt_invoke(_id,_o);
}

bool MedicinaMount::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: static_QUType_int.set(_o,setGUIAntennaMountStatusColor()); break;
    case 1: static_QUType_int.set(_o,setGUIACUStatusColor()); break;
    case 2: static_QUType_int.set(_o,setGUIPositionAndErrorValue()); break;
    case 3: static_QUType_int.set(_o,setGUIACUTimeValue()); break;
    case 4: static_QUType_int.set(_o,setGUIAntennaSectionValue()); break;
    case 5: static_QUType_int.set(_o,setGUIServoSystemColors()); break;
    case 6: static_QUType_int.set(_o,setGUIAzimuthElevationServoStatusColors()); break;
    case 7: static_QUType_int.set(_o,setGUIAzimuthElevationOperatingModeColors()); break;
    case 8: static_QUType_int.set(_o,setGUIPropertyErrorCode()); break;
    case 9: static_QUType_int.set(_o,setGUICatchAllErrorCode()); break;
    default:
	return QThread::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool MedicinaMount::qt_property( int id, int f, QVariant* v)
{
    return QThread::qt_property( id, f, v);
}

bool MedicinaMount::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
