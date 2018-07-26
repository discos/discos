#ifndef OBSERVATORYCLIENT_H
#define OBSERVATORYCLIENT_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: ObservatoryTextClient.h,v 1.10 2010-08-19 08:57:43 a.orlati Exp $                                                                                            */   
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                  when            What                                            */
/* Andrea Orlati(a.orlati@ira.inaf.it)  14/02/2007      Creation                                        */
/* Andrea Orlati(a.orlati@ira.inaf.it)  14/02/2007      Ported to ACS 6.0.2                             */
/* Andrea Orlati(a.orlati@ira.inaf.it)  24/09/2007      Ported to ACS 6.0.4                             */
/* Andrea Orlati(a.orlati@ira.inaf.it)  06/04/2009      Connect to Component as default component           */
/* Andrea Orlati(a.orlati@ira.inaf.it)  04/08/2010      command from user input removed, a part from "exit"     */
/* Andrea Orlati(a.orlati@ira.inaf.it)  19/08/2010      displayed interface attrribute "observatoryName" */

#include <ObservatoryC.h>
#include <maciSimpleClient.h>
#include <baciThread.h>
#include <ClientErrors.h>
#include <IRA>
#include <TW_Tools.h>
#include <TW_Frame.h>
#include <TW_PropertyText.h>
#include <TW_Label.h>
#include <TW_InputCommand.h>
#include <TW_PropertyStatusBox.h>

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
#define MAXFIELDNUMBER 10
/* *************************************************************************** */

/* define the main frame dimension */
#define WINDOW_WIDTH 73      /*columns */
#define WINDOW_HEIGHT 17     /* rows */  
/* ******************************* */

/* define the routine name here, to be used for logging */
#define MODULE_NAME "observatoryTui"   /* the name of this module (filename without .cpp) */
/* **************************************************** */

/* define the application title and style */
#define APPLICATION_TITLE "Observatory"        /* the title of the main frame */
#define TITLE_COLOR_PAIR CColorPair::RED_BLACK /* the title color and background */
#define TITLE_STYLE CStyle::BOLD|CStyle::UNDERLINE /* the title style */
/* ************************************* */

/* define name and component interface */
//#define COMPONENT_NAME "ANTENNA/Observatory"              /* the name of the istantiation of the component */
#define COMPONENT_INTERFACE_TPYE "IDL:alma/Antenna/Observatory:1.0"  /* the type of the interface */
#define COMPONENT_IDL_MODULE Antenna				/* the IDL module that contains the component interface */
#define COMPONENT_IDL_INTERFACE Observatory  /* the IDL interface of the component */
#define COMPONENT_SMARTPOINTER Observatory_var /* the component type */
/* ********************************** */

/* define user input command style */
#define USER_INPUT_COLOR_PAIR BLUE_GRAY    /* user input color and background */
#define USER_INPUT_STYLE CStyle::NORMAL    /* user input style */
/* ******************************* */

/* define output field look and feel */
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
