#ifndef XBACKENDSCLIENT_H_
#define XBACKENDSCLIENT_H_

#include <QtGui/QMainWindow>
#include "xclientGUI.h"

#include "headResult.h"
#include "Specifiche.h"
#ifndef COMPONENTTEXTCLIENTTEMPLATE_H
#define COMPONENTTEXTCLIENTTEMPLATE_H


/* Add the component skeleton header file */
/* ************************************* */
#include <GenericBackendC.h>
/* ************************************* */

/* Add the component errors header file */
/* ************************************* */
#include <ComponentErrors.h>
#include <BackendsErrors.h>
/* ************************************* */

#include <maciSimpleClient.h>
#include <baciThread.h>
#include <ClientErrors.h>
#include <baci.h>
#include <IRA>
#include <TW_Tools.h>
#include <TW_Frame.h>
#include <TW_PropertyText.h>
#include <TW_Label.h>
#include <TW_PropertyLedDisplay.h>
#include <TW_PropertyBitField.h>
#include <TW_InputCommand.h>

#define GRAY_COLOR 0
#define BLUE_GRAY 0
#define GRAY_BLUE 1
#define BLACK_RED 2
#define BLACK_GREEN 3
#define BLACK_YELLOW 4
#define BLACK_BLUE 5
#define BLACK_MAGENTA 6
#define BLACK_WHITE 7

/* define the max number of arguments that can be passed to component methods */
/* ************************************************************************** */
#define MAXFIELDNUMBER 10
/* ************************************************************************** */

/* define the main frame dimension */
/* ******************************* */
#define WINDOW_WIDTH 79      /*columns */
#define WINDOW_HEIGHT 28     /* rows */  
/* ******************************* */

/* define the routine name here, to be used for logging */
/* **************************************************** */
#define MODULE_NAME "genericBackendGui"   /* the name of this module (filename without .cpp) */
/* **************************************************** */

/* define the application title and style */
/* ************************************* */
#define APPLICATION_TITLE "GenericBackend"        /* the title of the main frame */
#define TITLE_COLOR_PAIR CColorPair::RED_BLACK /* the title color and background */
#define TITLE_STYLE CStyle::BOLD|CStyle::UNDERLINE /* the title style */
/* ************************************* */

/* define name and component interface */
/* ********************************** */
//#define COMPONENT_NAME "BACKENDS/TotalPower"              /* the name of the istantiation of the component */
#define COMPONENT_IDL_MODULE Backends	/* the IDL module that contains the component interface */
#define COMPONENT_IDL_INTERFACE GenericBackend  /* the IDL interface of the component */
#define COMPONENT_SMARTPOINTER GenericBackend_var /* the component type */
/* ********************************** */

/* define user input command style */
/* ******************************* */
#define USER_INPUT_COLOR_PAIR BLUE_GRAY    /* user input color and background */
#define USER_INPUT_STYLE CStyle::NORMAL    /* user input style */
/* ******************************* */

/* define output field look and feel */
/* ********************************* */
#define USE_OUTPUT_FIELD 1	/* 0 disable the output field */
#define OUTPUT_FIELD_COLOR_PAIR GRAY_BLUE  /* output field color and background */
#define OUTPUT_FIELD_STYLE CStyle::NORMAL  /* output field style */
#define OUTPUT_FIELD_HEIGHT 4				  /* output field height (number of rows) */
/* ********************************* */

/* define this value (milliseconds) to set the log guard interval */
#define GUARDINTERVAL 1000
/* to send the client to sleep for a certain ammount of time, setup this value (milliseconds) */
#define MAINTHREADSLEEPTIME 250

#endif

class XBackendsClient
{
public:
	XBackendsClient(QWidget *parent = 0);
	virtual ~XBackendsClient();

private:
	//Ui_ClientClass ui;
};

#endif /*XBACKENDSCLIENT_H_*/
