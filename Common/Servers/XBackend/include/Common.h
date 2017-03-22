#ifndef _COMMON_H_
#define _COMMON_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Common.h,v 1.29 2010/08/26 13:29:34 bliliana Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                           				     when           What                                              */
/* Liliana Branciforti(bliliana@arcetri.astro.it) 04/08/2009 		Creation								*/

//#define BKD_DEBUG 
#define DOPPIO

#ifdef DOPPIO 
	#define MAX_ADC_NUMBER 14 // CDB
#else 
	#define MAX_ADC_NUMBER 7 // CDB
#endif 

#ifdef COMPILE_TARGET_MED

#define DEFAULT_BINS 2048

#else

#define DEFAULT_BINS 2048

#endif


#define FLOW_NUMBER 1

#define MAX_GAIN 255 //Gain is in hardware units
#define MAX_DEFAULT_BEAM 7 // CDB
#define MAX_SECTION_NUMBER 32
#define DEFAULT_SECTION_NUMBER MAX_DEFAULT_BEAM // CDB
#define MAX_INPUT_NUMBER MAX_ADC_NUMBER // CDB

#define SAMPLETYPE double
#define SAMPLESIZE sizeof(SAMPLETYPE)  //

#define DEFAULT_MODE8BIT false // CDB

#define DEFAULT_GAIN 30.0//Gain is in hardware units
#define DEFAULT_BANDWIDTH 62.5 //MHz // CDB
#define DEFAULT_FREQUENCY 20.0 //MHz // CDB

#define DEFAULT_INTEGRATION 10  //10 seconds integration // CDB
#define DEFAULT_POLARIZATION true // CDB

#define MAX_BANDWIDTH 125.0 //MHz
#define MIN_BANDWIDTH 0.488 //MHz

#define MAX_ATTENUATION 50.0 //15.0 corrisponde ad un gain 0.8
#define MIN_ATTENUATION 0.0

#define MAX_FREQUENCY 125.0 
#define MIN_FREQUENCY 0.0 
#define ANALOG_FREQUENCY 125.0

#define DEFAULT_SAMPLERATE 125.0 //MHz // CDB

#endif /*COMMON_H_*/
