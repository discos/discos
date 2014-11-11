#ifndef _COMMON_H_
#define _COMMON_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Common.h,v 1.1 2011-03-14 15:16:22 a.orlati Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when           What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 10/06/2010      Creation				*/                                 

#define SAMPLETYPE float
#define SAMPLESIZE sizeof(SAMPLETYPE)  //

#define MAX_SECTION_NUMBER 14
#define MAX_INPUT_NUMBER MAX_SECTION_NUMBER*2


#define DEFAULT_ATTENUATION 9.0
#define DEFAULT_BANDWIDTH 1000.0
#define DEFAULT_SAMPLE_RATE 2000.0  
//4 seconds integration
#define DEFAULT_INTEGRATION 4000  
#define DEFAULT_BINS_NUMBER 4096
#define DEFAULT_FREQUENCY 100.0;
#define DEFAULT_POLARIZATION Backends::BKND_FULL_STOKES

#define NOISE_APPLITUDE 1000

#define FLOW_NUMBER 1
#endif /*COMMON_H_*/
