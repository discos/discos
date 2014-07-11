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

	static bool map(const char *strMode,TUpdateModes& mode) {
		if (strcasecmp(strMode,"FIXED")==0) {
			mode=RCV_FIXED_UPDATE;
			return true;
		}
		if (strcasecmp(strMode,"OPTIMIZED")==0) {
			mode=RCV_OPTIMIZED_UPDATE;
			return true;
		}
		if (strcasecmp(strMode,"DISABLED")==0) {
			mode=RCV_DISABLED_UPDATE;
			return true;
		}
		if (strcasecmp(strMode,"UNDEFINED")==0) {
			mode=RCV_UNDEF_UPDATE;
			return true;
		}
		else {
			return false;
		}
	}
	static const char *map(const TUpdateModes& mode ) {
		if (mode==RCV_FIXED_UPDATE) {
			return "AUTO";
		}
		if (mode==RCV_OPTIMIZED_UPDATE) {
			return "OPTIMIZED";
		}
		if (mode==RCV_DISABLED_UPDATE) {
			return "DISABLED";
		}
		else { //RCV_UNDEF_UPDATE
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
