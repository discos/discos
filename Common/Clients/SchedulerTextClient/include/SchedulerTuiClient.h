#ifndef COMPONENTTEXTCLIENTTEMPLATE_H
#define COMPONENTTEXTCLIENTTEMPLATE_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati (a.orlati@ira.inaf.it)  09/08/13     Creation  */
/* Andrea Orlati (a.orlati@ira.inaf.it)  12/04/14     display for the restFrequency attribute  */

#include <SchedulerC.h>
#include <ManagmentDefinitionsC.h>

#include <ManagementErrors.h>

#include <ComponentErrors.h>


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
#include <TW_PropertyLedDisplay.h>
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

/* define the max number of arguments that can be passed to component methods */
/* ************************************************************************** */
#define MAXFIELDNUMBER 10
/* ************************************************************************** */

/* define the main frame dimension */
/* ******************************* */
#define WINDOW_WIDTH 60      /*columns */
#define WINDOW_HEIGHT 17     /* rows */
/* ******************************* */

/* **************************************************** */
#define MODULE_NAME "SchedulerTextClient"   /* the name of this module (filename without .cpp) */
/* **************************************************** */

/* define the application title and style */
/* ************************************* */
#define APPLICATION_TITLE "Scheduler"        /* the title of the main frame */
#define TITLE_COLOR_PAIR CColorPair::RED_BLACK /* the title color and background */
#define TITLE_STYLE CStyle::BOLD|CStyle::UNDERLINE /* the title style */
/* ************************************* */

/* define name and component interface */
/* ********************************** */
#define COMPONENT_INTERFACE_TPYE "IDL:alma/Management/Scheduler:1.0"       /* the type of the interface */
#define COMPONENT_IDL_MODULE Management	/* the IDL module that contains the component interface */
#define COMPONENT_IDL_INTERFACE Scheduler  /* the IDL interface of the component */
#define COMPONENT_SMARTPOINTER Scheduler_var /* the component type */
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
#define MAINTHREADSLEEPTIME 200

#endif
