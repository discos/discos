/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*  									         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                      when                     What                                                       */
/* Andrea Orlati(a.orlati@ira.inaf.it) 08/01/2015       Creation                                                  */

#ifndef MANAGEMENTMODULE_H_
#define MANAGEMENTMODULE_H_

#include "ManagmentDefinitionsC.h"

namespace Management {

/**
 * Some utility method related to the definitions of the <i>Management</i> module
 */
class Definitions {
public:
	static bool map(const char *strScan,TScanTypes& mode) {
		if (strcasecmp(strScan,"SIDEREAL")==0) {
			mode=Management::MNG_SIDEREAL;
			return true;
		}
		else if (strcasecmp(strScan,"SUN")==0) {
			mode=Management::MNG_SUN;
			return true;
		}
		else if (strcasecmp(strScan,"MOON")==0) {
			mode=Management::MNG_MOON;
			return true;
		}
		else if (strcasecmp(strScan,"SATELLITE")==0) {
			mode=Management::MNG_SATELLITE;
			return true;
		}
		else if (strcasecmp(strScan,"SOLARSYTEMBODY")==0) {
			mode=Management::MNG_SOLARSYTEMBODY;
			return true;
		}
		else if (strcasecmp(strScan,"OTF")==0) {
			mode=Management::MNG_OTF;
			return true;
		}
		else if (strcasecmp(strScan,"OTFC")==0) {
			mode=Management::MNG_OTFC;
			return true;
		}
		else if (strcasecmp(strScan,"PEAKER")==0) {
			mode=Management::MNG_PEAKER;
			return true;
		}
		else if (strcasecmp(strScan,"SKYDIP")==0) {
			mode=Management::MNG_SKYDIP;
			return true;
		}
		else {
			return false;
		}
	}
	static const char *map(const TScanTypes& scan) {
		if (scan==Management::MNG_SIDEREAL) {
			return "SIDEREAL";
		}
		if (scan==Management::MNG_SUN) {
			return "SUN";
		}
		if (scan==Management::MNG_MOON) {
			return "MOON";
		}
		if (scan==Management::MNG_SATELLITE) {
			return "SATELLITE";
		}
		if (scan==Management::MNG_SOLARSYTEMBODY) {
			return "SOLARSYTEMBODY";
		}
		if (scan==Management::MNG_OTF) {
			return "OTF";
		}
		if (scan==Management::MNG_OTFC) {
			return "OTFC";
		}
		if (scan==Management::MNG_PEAKER) {
			return "PEAKER";
		}
		else { //Management::MNG_SKYDIP
			return "SKYDIP";
		}
	}

	static bool map(const char *strScan,TSubScanSignal& mode) {
		if (strcasecmp(strScan,"SIGNAL")==0) {
			mode=Management::MNG_SIGNAL_SIGNAL;
			return true;
		}
		else if (strcasecmp(strScan,"REFERENCE")==0) {
			mode=Management::MNG_SIGNAL_REFERENCE;
			return true;
		}
		else if (strcasecmp(strScan,"NONE")==0) {
			mode=Management::MNG_SIGNAL_NONE;
			return true;
		}
		else {
			return false;
		}
	}

	static const char *map(const TSubScanSignal& scan) {
		if (scan==Management::MNG_SIGNAL_SIGNAL) {
			return "SIGNAL";
		}
		if (scan==Management::MNG_SIGNAL_REFERENCE) {
			return "REFERENCE";
		}
		else { //Management::MNG_SIGNAL_REFERENCE
			return "NONE";
		}
	}

};

};




#endif /* MANAGEMENTMODULE_H_ */
