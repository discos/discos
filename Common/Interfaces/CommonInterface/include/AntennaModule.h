/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*  									         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                      when                     What                                                       */
/* Andrea Orlati(a.orlati@ira.inaf.it) 08/01/2015       Creation                                                  */

#ifndef ANTENNAMODULE_H_
#define ANTENNAMODULE_H_


#include "AntennaDefinitionsC.h"

namespace Antenna {
/**
 * Some utility method related to the definitions of the <i>Antenna</i> module
 */
class Definitions {
public:
	static bool map(const char *str,TsubScanGeometry& geometry) {
		if (strcasecmp(str,"LAT")==0) {
			geometry=Antenna::SUBSCAN_CONSTLAT;
			return true;
		}
		else if (strcasecmp(str,"LON")==0) {
			geometry=Antenna::SUBSCAN_CONSTLON;
			return true;
		}
		else if (strcasecmp(str,"GC")==0) {
			geometry=Antenna::SUBSCAN_GREATCIRCLE;
			return true;
		}
		else {
			return false;
		}
	}
	static const char *map(const TsubScanGeometry& scan) {
		if (scan==Antenna::SUBSCAN_CONSTLAT) {
			return "LAT";
		}
		if (scan==Antenna::SUBSCAN_CONSTLON) {
			return "LON";
		}
		else { // Antenna::SUBSCAN_GREATCIRCLE
			return "GC";
		}
	}
};

};


#endif /* ANTENNAMODULE_H_ */


