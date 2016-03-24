#ifndef RECEIVERSMODULE_H_
#define RECEIVERSMODULE_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*  									         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                      when                     What                                                       */
/* Andrea Orlati(a.orlati@ira.inaf.it) 08/07/2014       Creation                                                  */

#include "ReceiversDefinitionsC.h"

namespace Receivers {

/**
 * Some utility method related to the definitions of the <i>Receivers</i> module
 */
class Definitions {
public:
	static bool map(const char *strMode,TRewindModes& mode) {
		if (strcasecmp(strMode,"AUTO")==0) {
			mode=RCV_AUTO_REWIND;
			return true;
		}
		if (strcasecmp(strMode,"MANUAL")==0) {
			mode=RCV_MANUAL_REWIND;
			return true;
		}
		if (strcasecmp(strMode,"UNDEFINED")==0) {
			mode=RCV_UNDEF_REWIND;
			return true;
		}
		else {
			return false;
		}
	}
	static const char *map(const TRewindModes& mode ) {
		if (mode==RCV_AUTO_REWIND) {
			return "AUTO";
		}
		if (mode==RCV_MANUAL_REWIND) {
			return "MANUAL";
		}
		else { //RCV_UNDEF_REWIND
			return "UNDEFINED";
		}
	}

	static bool map(const char *strMode,TDerotatorConfigurations& mode) {
		if (strcasecmp(strMode,"FIXED")==0) {
			mode=RCV_FIXED_DEROTCONF;
			return true;
		}
		if (strcasecmp(strMode,"BSC")==0) {
			mode=RCV_BSC_DEROTCONF;
			return true;
		}
		if (strcasecmp(strMode,"BSC_OPT")==0) {
			mode=RCV_BSC_OPTIMIZED_DEROTCONF;
			return true;
		}
		if (strcasecmp(strMode,"CUSTOM")==0) {
			mode=RCV_CUSTOM_DEROTCONF;
			return true;
		}
		if (strcasecmp(strMode,"CUSTOM_OPT")==0) {
			mode=RCV_CUSTOM_OPTIMIZED_DEROTCONF;
			return true;
		}
		if (strcasecmp(strMode,"UNDEFINED")==0) {
			mode=RCV_UNDEF_DEROTCONF;
			return true;
		}
		else {
			return false;
		}
	}

	static const char *map(const TDerotatorConfigurations& mode ) {
		if (mode==RCV_FIXED_DEROTCONF) {
			return "FIXED";
		}
		if (mode==RCV_BSC_DEROTCONF) {
			return "BSC";
		}
		if (mode==RCV_BSC_OPTIMIZED_DEROTCONF) {
			return "BSC_OPT";
		}
		if (mode==RCV_CUSTOM_DEROTCONF) {
			return "CUSTOM";
		}
		if (mode==RCV_CUSTOM_OPTIMIZED_DEROTCONF) {
			return "CUSTOM_OPT";
		}
		else { //RCV_UNDEF_DEROTCONF
			return "UNDEFINED";
		}
	}

	static bool map(const char *strMode,TPolarization& mode) {
		if (strcasecmp(strMode,"LEFT")==0) {
			mode=RCV_LCP;
			return true;
		}
		if (strcasecmp(strMode,"RIGHT")==0) {
			mode=RCV_RCP;
			return true;
		}
		if (strcasecmp(strMode,"VERTICAL")==0) {
			mode=RCV_VLP;
			return true;
		}
		if (strcasecmp(strMode,"HORIZONTAL")==0) {
			mode=RCV_HLP;
			return true;
		}
		else {
			return false;
		}
	}
	static const char *map(const TPolarization& mode ) {
		if (mode==RCV_LCP) {
			return "LEFT";
		}
		if (mode==RCV_RCP) {
			return "RIGHT";
		}
		if (mode==RCV_VLP) {
			return "VERTICAL";
		}
		else { //RCV_HLP
			return "HORIZONTAL";
		}
	}
};

};



#endif /* RECEIVERSMODULE_H_ */
