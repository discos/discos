/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: SP_typeConversion.h,v 1.4 2011-06-21 16:37:46 a.orlati Exp $											*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                  when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/05/2011      Creation                                          */

#ifndef _SP_TYPES_CONVERSION_H_
#define _SP_TYPES_CONVERSION_H_

#include <AntennaDefinitionsS.h>
#include <ParserErrors.h>
#include <IRA>

namespace SimpleParser {

class AntennaSection2String {
public:
	static char *valToStr(Antenna::TSections& val) {
		char *c=new char[16];
		if (val==Antenna::ACU_CW) {
			strcpy(c,"CW");
		}
		else if (val==Antenna::ACU_CCW) {
			strcpy(c,"CCW");
		}
		else {
			strcpy(c,"NEUTRAL");
		}
		return c;
	}
	static Antenna::TSections strToVal(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
		IRA::CString strVal(str);
		strVal.MakeUpper();
		if (strVal=="CW") {
			return Antenna::ACU_CW;
		}
		else if (strVal=="CCW") {
			return Antenna::ACU_CCW;
		}
		else if (strVal=="NEUTRAL") {
			return Antenna::ACU_NEUTRAL;
		}
		else if (strVal=="-1") {
			return Antenna::ACU_NEUTRAL;
		}
		else {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"AntennaSection2String::strToVal()");
			throw ex;
		}
	}
};

class AntennaEquinox2String {
public:
	static char *valToStr(Antenna::TSystemEquinox& val) {
		char *c=new char[16];
		IRA::CString out;
		IRA::CIRATools::equinoxToStr(val,out);
		strcpy(c,(const char *)out);		
		return c;
	}
	static Antenna::TSystemEquinox strToVal(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
		Antenna::TSystemEquinox eq;
		if (!IRA::CIRATools::strToEquinox(str,eq)) {		
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"AntennaEquinox2String::strToVal()");
			throw ex;			
		}
		else {
			return eq;
		}
	}
};

class AntennaFrame2String {
public:
	static char *valToStr(Antenna::TCoordinateFrame& val) {
		char *c=new char[16];
		IRA::CString out;
		IRA::CIRATools::coordinateFrameToStr(val,out);
		strcpy(c,(const char *)out);		
		return c;		
	}
	static Antenna::TCoordinateFrame strToVal(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
		Antenna::TCoordinateFrame frame;
		if (!IRA::CIRATools::strToCoordinateFrame(str,frame)) {		
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"AntennaFrame2String::strToVal()");
			throw ex;			
		}
		else {
			return frame;
		}
	}
};

};

#endif /*_SP_TYPES_CONVERSION_H_*/
