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
	static bool map(const char *str,TReferenceFrame& frame) {
		if (strcasecmp(str,"TOPOCEN")==0) {
			frame=Antenna::ANT_TOPOCEN;
			return true;
		}
		else if (strcasecmp(str,"BARY")==0) {
			frame=Antenna::ANT_BARY;
			return true;
		}
		else if (strcasecmp(str,"LSRK")==0) {
			frame=Antenna::ANT_LSRK;
			return true;
		}
		else if (strcasecmp(str,"LSRD")==0) {
			frame=Antenna::ANT_LSRD;
			return true;
		}
		else if (strcasecmp(str,"GALCEN")==0) {
			frame=Antenna::ANT_GALCEN;
			return true;
		}
		else if (strcasecmp(str,"LGRP")==0) {
			frame=Antenna::ANT_LGROUP;
			return true;
		}
		else if (strcasecmp(str,"UNDEF")==0) {
			frame=Antenna::ANT_UNDEF_FRAME;
			return true;
		}
		else {
			return false;
		}
	};

	static const char *map(const TReferenceFrame& frame) {
		if (frame==Antenna::ANT_TOPOCEN) {
			return "TOPOCEN";
		}
		else if (frame==Antenna::ANT_BARY) {
			return "BARY";
		}
		else if (frame==Antenna::ANT_LSRK) {
			return "LSRK";
		}
		else if (frame==Antenna::ANT_LSRD) {
			return "LSRD";
		}
		else if (frame==Antenna::ANT_GALCEN) {
			return "GALCEN";
		}
		else if (frame==Antenna::ANT_LGROUP) {
			return "LGRP";
		}
		else { //if (frame==Antenna::ANT_UNDEF_FRAME) {
			return "UNDEF";
		}
	};

	static bool map(const char *str,TVradDefinition& frame) {
		if (strcasecmp(str,"RD")==0) {
			frame=Antenna::ANT_RADIO;
			return true;
		}
		else if (strcasecmp(str,"OP")==0) {
			frame=Antenna::ANT_OPTICAL;
			return true;
		}
		else if (strcasecmp(str,"Z")==0) {
			frame=Antenna::ANT_REDSHIFT;
			return true;
		}
		else if (strcasecmp(str,"UNDEF")==0) {
			frame=Antenna::ANT_UNDEF_DEF;
			return true;
		}
		else {
			return false;
		}
	};

	static const char *map(const TVradDefinition& frame) {
		if (frame==Antenna::ANT_RADIO) {
			return "RD";
		}
		else if (frame==Antenna::ANT_OPTICAL) {
			return "OP";
		}
		else if (frame==Antenna::ANT_REDSHIFT) {
			return "Z";
		}
		else { // if (frame==Antenna::ANT_UNDEF_DEF) {
			return "UNDEF";
		}
	};
};

};


#endif /* ANTENNAMODULE_H_ */


