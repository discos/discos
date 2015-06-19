#ifndef _COMMON_H_
#define _COMMON_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Common.h,v 1.1 2011-05-31 12:19:38 amelis Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                           				     when           What                                              */
/* Liliana Branciforti(bliliana@arcetri.astro.it) 04/08/2009 		Creation								*/


//#define BKD_DEBUG 
//#define DOPPIO

#ifdef DOPPIO 
	#define HW_DOPPIO true
	#define DEFAULT_BINS 2048
	#define MAX_ADC_NUMBER 16
#else 
	#define HW_DOPPIO false
 	#define DEFAULT_BINS 4096
	#define MAX_ADC_NUMBER 8
#endif 

#define FLOW_NUMBER 1

#define MAX_GAIN 255 //Gain is in hardware units
#define MAX_DEFAULT_BEAM 7
#define MAX_SECTION_NUMBER MAX_ADC_NUMBER*2
#define DEFAULT_SECTION_NUMBER 4
#define MAX_INPUT_NUMBER MAX_ADC_NUMBER*2

#define SAMPLETYPE double
#define SAMPLESIZE sizeof(SAMPLETYPE)  //

#define DEFAULT_MODE8BIT true

#define DEFAULT_GAIN 30.0//Gain is in hardware units
#define DEFAULT_BANDWIDTH 62.5 //MHz
#define DEFAULT_FREQUENCY 125.0; //MHz

#define DEFAULT_INTEGRATION 10  //10 seconds integration

#define DEFAULT_POLARIZATION false

#define MAX_BANDWIDTH 125.0 //MHz
#define MIN_BANDWIDTH 0.488 //MHz

#define MAX_ATTENUATION 50.0//15.0 corrisponde ad un gain 0.8
#define MIN_ATTENUATION 0.0

#define MAX_FREQUENCY 250.0 //MHz//250.0MHz
#define MIN_FREQUENCY 125.0 //MHz//125.0MHz

#define MAX_SAMPLE_RATE 250.0 //MHz
#define DEFAULT_SAMPLERATE 125.0 //MHz

#endif /*COMMON_H_*/
