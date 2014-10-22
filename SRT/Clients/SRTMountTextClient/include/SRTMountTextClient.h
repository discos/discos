#ifndef SRTMOUNTTEXTCLIENT_H
#define SRTMOUNTTEXTCLIENT_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $id $                                                                                                 */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 07/05/2010      Creation*/

// quick guide: when needed, fill in all the parts bounded by *



#include <SRTMountC.h>


#include <ComponentErrors.h>

#include <AntennaErrors.h>


#include <maciSimpleClient.h>
#include <baciThread.h>
#include <ClientErrors.h>
#include <baci.h>
#include <IRA>
#include <TW_Tools.h>
#include <TW_Frame.h>
#include <TW_PropertyText.h>
#include <TW_Label.h>
#include <TW_InputCommand.h>
#include <TW_PropertyStatusBox.h>
#include <TW_PropertyBitField.h>
#include <TW_LedDisplay.h>

#define GRAY_COLOR 0
#define BLUE_GRAY 0
#define GRAY_BLUE 1
#define BLACK_RED 2
#define BLACK_GREEN 3
#define BLACK_YELLOW 4
#define BLACK_BLUE 5
#define BLACK_MAGENTA 6
#define BLACK_WHITE 7

#define WINDOW_WIDTH 100      /*columns */
#define WINDOW_HEIGHT 37     /* rows */

#define MODULE_NAME "SRTMountTextClient"   /* the name of this module (filename without .cpp) */


#define APPLICATION_TITLE "Mount"        /* the title of the main frame */
#define TITLE_COLOR_PAIR CColorPair::RED_BLACK /* the title color and background */
#define TITLE_STYLE CStyle::BOLD|CStyle::UNDERLINE /* the title style */
/* ************************************* */

/* define name and component interface */
/* ********************************** */
#define COMPONENT_IDL_MODULE Antenna	/* the IDL module that contains the component interface */
#define COMPONENT_IDL_INTERFACE SRTMount  /* the IDL interface of the component */
#define COMPONENT_SMARTPOINTER SRTMount_var /* the component type */
/* ********************************** */

/* define user input command style */
/* ******************************* */
#define USER_INPUT_COLOR_PAIR BLUE_GRAY    /* user input color and background */
#define USER_INPUT_STYLE CStyle::NORMAL    /* user input style */
/* ******************************* */

#define USE_OUTPUT_FIELD 1	/* 0 disable the output field */
#define OUTPUT_FIELD_COLOR_PAIR GRAY_BLUE  /* output field color and background */
#define OUTPUT_FIELD_STYLE CStyle::NORMAL  /* output field style */
#define OUTPUT_FIELD_HEIGHT 4				  /* output field height (number of rows) */

#define MAXFIELDNUMBER 10

/* to send the client to sleep for a certain ammount of time, setup this value (milliseconds) */
#define MAINTHREADSLEEPTIME 200

#define GUARDINTERVAL 5000

#define MOTORS_NUMBER 13


class CMotorsStatusCallback : public virtual POA_ACS::CBlongSeq
{
public:
	CMotorsStatusCallback() : m_guard(GUARDINTERVAL*1000) {
	}
	~CMotorsStatusCallback() {}
	void working (const ACS::longSeq& value,const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	void done (const ACS::longSeq& vlaue,const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	CORBA::Boolean negotiate (ACS::TimeInterval time_to_transmit, const ACS::CBDescOut &desc) throw (CORBA::SystemException) {
		return true;
	}
	void setControls(TW::CLedDisplay * control[]) { 
		m_control=control;
	}
private:
	TW::CLedDisplay* *m_control;
	IRA::CLogGuard m_guard;
};


#endif
