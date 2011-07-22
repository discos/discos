/****************************************************************************
** MedicinaMountGUI meta object code from reading C++ file 'MedicinaMountGUIui.h'
**
** Created: Tue Apr 26 17:26:31 2011
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../include/MedicinaMountGUIui.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MedicinaMountGUI::className() const
{
    return "MedicinaMountGUI";
}

QMetaObject *MedicinaMountGUI::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MedicinaMountGUI( "MedicinaMountGUI", &MedicinaMountGUI::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MedicinaMountGUI::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MedicinaMountGUI", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MedicinaMountGUI::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MedicinaMountGUI", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MedicinaMountGUI::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUMethod slot_0 = {"Quit", 0, 0 };
    static const QUMethod slot_1 = {"Stow", 0, 0 };
    static const QUMethod slot_2 = {"UnStow", 0, 0 };
    static const QUMethod slot_3 = {"Stop", 0, 0 };
    static const QUMethod slot_4 = {"Rates", 0, 0 };
    static const QUMethod slot_5 = {"AzElMode", 0, 0 };
    static const QUMethod slot_6 = {"Preset", 0, 0 };
    static const QUMethod slot_7 = {"ProgramTrack", 0, 0 };
    static const QUMethod slot_8 = {"ResetFailures", 0, 0 };
    static const QUMethod slot_9 = {"SetTime", 0, 0 };
    static const QUMethod slot_10 = {"changeGUIAntennaMountStatusColor", 0, 0 };
    static const QUMethod slot_11 = {"changeGUIACUStatusColor", 0, 0 };
    static const QUMethod slot_12 = {"changeGUIPositionAndErrorValue", 0, 0 };
    static const QUMethod slot_13 = {"changeGUIACUTimeValue", 0, 0 };
    static const QUMethod slot_14 = {"changeGUIAntennaSectionValue", 0, 0 };
    static const QUMethod slot_15 = {"changeGUIServoSystemColors", 0, 0 };
    static const QUMethod slot_16 = {"changeGUIAzimuthElevationServoStatusColors", 0, 0 };
    static const QUMethod slot_17 = {"changeGUIAzimuthElevationOperatingModeColors", 0, 0 };
    static const QUMethod slot_18 = {"changeGUIPropertyErrorCode", 0, 0 };
    static const QUMethod slot_19 = {"changeGUICatchAllErrorCode", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "Quit()", &slot_0, QMetaData::Public },
	{ "Stow()", &slot_1, QMetaData::Public },
	{ "UnStow()", &slot_2, QMetaData::Public },
	{ "Stop()", &slot_3, QMetaData::Public },
	{ "Rates()", &slot_4, QMetaData::Public },
	{ "AzElMode()", &slot_5, QMetaData::Public },
	{ "Preset()", &slot_6, QMetaData::Public },
	{ "ProgramTrack()", &slot_7, QMetaData::Public },
	{ "ResetFailures()", &slot_8, QMetaData::Public },
	{ "SetTime()", &slot_9, QMetaData::Public },
	{ "changeGUIAntennaMountStatusColor()", &slot_10, QMetaData::Private },
	{ "changeGUIACUStatusColor()", &slot_11, QMetaData::Private },
	{ "changeGUIPositionAndErrorValue()", &slot_12, QMetaData::Private },
	{ "changeGUIACUTimeValue()", &slot_13, QMetaData::Private },
	{ "changeGUIAntennaSectionValue()", &slot_14, QMetaData::Private },
	{ "changeGUIServoSystemColors()", &slot_15, QMetaData::Private },
	{ "changeGUIAzimuthElevationServoStatusColors()", &slot_16, QMetaData::Private },
	{ "changeGUIAzimuthElevationOperatingModeColors()", &slot_17, QMetaData::Private },
	{ "changeGUIPropertyErrorCode()", &slot_18, QMetaData::Private },
	{ "changeGUICatchAllErrorCode()", &slot_19, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"MedicinaMountGUI", parentObject,
	slot_tbl, 20,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MedicinaMountGUI.setMetaObject( metaObj );
    return metaObj;
}

void* MedicinaMountGUI::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MedicinaMountGUI" ) )
	return this;
    if ( !qstrcmp( clname, "Ui_MedicinaMountGUI" ) )
	return (Ui_MedicinaMountGUI*)this;
    return QWidget::qt_cast( clname );
}

bool MedicinaMountGUI::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: Quit(); break;
    case 1: Stow(); break;
    case 2: UnStow(); break;
    case 3: Stop(); break;
    case 4: Rates(); break;
    case 5: AzElMode(); break;
    case 6: Preset(); break;
    case 7: ProgramTrack(); break;
    case 8: ResetFailures(); break;
    case 9: SetTime(); break;
    case 10: changeGUIAntennaMountStatusColor(); break;
    case 11: changeGUIACUStatusColor(); break;
    case 12: changeGUIPositionAndErrorValue(); break;
    case 13: changeGUIACUTimeValue(); break;
    case 14: changeGUIAntennaSectionValue(); break;
    case 15: changeGUIServoSystemColors(); break;
    case 16: changeGUIAzimuthElevationServoStatusColors(); break;
    case 17: changeGUIAzimuthElevationOperatingModeColors(); break;
    case 18: changeGUIPropertyErrorCode(); break;
    case 19: changeGUICatchAllErrorCode(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MedicinaMountGUI::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MedicinaMountGUI::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool MedicinaMountGUI::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
