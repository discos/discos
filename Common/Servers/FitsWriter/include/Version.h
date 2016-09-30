#ifndef VERSION_H_
#define VERSION_H_

/* ************************************************************************************************************* */
/* DISCOS project                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                       */
/* Andrea Orlati (a.orlati@ira.inaf.it)   09/04/2015
*/

#define FITS_VERSION1 "V. 0.8"
#define FITS_VERSION2 "V.0.82"
#define FITS_VERSION3 "V.0.9"
#define FITS_VERSION4 "V.0.91"
#define FITS_VERSION5 "V.0.92"
#define FITS_VERSION6 "V.1.0"
#define FITS_VERSION7 "V.1.01"
#define FITS_VERSION8 "V.1.1"
#define FITS_VERSION9 "V.1.11"
#define FITS_VERSION10 "V.1.2"

#define CURRENT_VERSION FITS_VERSION9

#define DEFAULT_COMMENT CURRENT_VERSION" Created by  S. Righini, M. Bartolini  & A. Orlati"

#define HISTORY1 FITS_VERSION1" First output standard for Italian radiotelescopes"
#define HISTORY2 FITS_VERSION2" The tsys column in data table raplaced with the Tant table, it reports the tsys measurement for each input of each section"
#define HISTORY3 FITS_VERSION3" The section table has been splitted into two tables: sections and rf inputs table"
#define HISTORY4 FITS_VERSION4" Added the flux column in section table"
#define HISTORY5 FITS_VERSION5" SubScanType added as primary header keyword"
#define HISTORY6 FITS_VERSION6" Added new table to store position of subriflector e primary focus receivers: SERVO TABLE"
#define HISTORY7 FITS_VERSION7" New keywords in FEED TABLE header to describe derotator configuration"
#define HISTORY8 FITS_VERSION8" Summary.fits file included in order to describe the scan configuration"
#define HISTORY9 FITS_VERSION9" Added the keyword SIGNAL in main header of each sub scan fits"
#define HISTORY10 FITS_VERSION10" Keywords SScanLatOff,SScanLonOff,SScanOffFrame raplace all offset keywords in main header"


#endif /* VERSION_H_ */
