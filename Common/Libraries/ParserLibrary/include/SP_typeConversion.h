/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                  when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/05/2011      Creation                                          */

#ifndef _SP_TYPES_CONVERSION_H_
#define _SP_TYPES_CONVERSION_H_

#include <AntennaDefinitionsS.h>
#include <ParserErrors.h>
#include <slamac.h>
#include <IRA>

namespace SimpleParser {

enum _sp_symbols {
	rad,
	deg
};

#define _SP_MULTI_ARGUMENT_SEPARATOR ';'
#define _SP_MULTI_ARGUMENT_COMPACT_SEP 'x'

class int_converter
{
public:
	int strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		int ret;
		if (sscanf(str,"%d",&ret)!=1) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"int_converter::strToVal()");
			ex.setExpectedType("Integer");
			throw ex;
		}
		return ret;
	}
	char *valToStr(const int& val) {
		char tmp[128];
		sprintf(tmp,"%d",val);
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
};

class void_converter
{
public:
	char *valToStr(const int& val) { return 0;}
	int strToVal(const char*str)  { return 0;}
};

class long_converter
{
public:
	long strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		long ret;
		if (sscanf(str,"%ld",&ret)!=1) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"long_converter::strToVal()");
			ex.setExpectedType("long");
			throw ex;
		}
		return ret;
	}
	char *valToStr(const long& val) {
		char tmp[128];
		sprintf(tmp,"%ld",val);
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
};

class double_converter
{
public:
	double strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		double ret;
		if (sscanf(str,"%lf",&ret)!=1) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"double_converter::strToVal()");
			ex.setExpectedType("double");
			throw ex;
		}
		return ret;
	}
	char *valToStr(const double& val) {
		char tmp[128];
		sprintf(tmp,"%lf",val);
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
};

class string_converter
{
public:
	char *strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		return const_cast<char *>(str) ;
	}
	char *valToStr(char * const & val) {
		char *c=new char [strlen(val)+1];
		strcpy(c,val);
		return c;
	}
};

template <_sp_symbols OUT> class angle_converter
{
public:
	char *valToStr(const double& val) {
		IRA::CString out("");
		double newVal;
		if (OUT==rad) newVal=val;
		else if (OUT==deg) newVal=val*DD2R;
		IRA::CIRATools::radToAngle(newVal,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double radians;
		if (!IRA::CIRATools::angleToRad(str,radians)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"angle_converter::strToVal()");
			throw ex;
		}
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

template <_sp_symbols OUT> class angleOffset_converter
{
public:
	char *valToStr(const double& val) {
		IRA::CString out("");
		double newVal;
		if (OUT==rad) newVal=val;
		else if (OUT==deg) newVal=val*DD2R;
		IRA::CIRATools::radToAngle(newVal,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double radians;
		if (!IRA::CIRATools::offsetToRad(str,radians)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"angleOffset_converter::strToVal()");
			throw ex;
		}
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

template <_sp_symbols OUT,bool RANGECHECK> class declination_converter
{
public:
	char *valToStr(const double& val) {
		IRA::CString out("");
		double newVal;
		if (OUT==rad) newVal=val;
		else if (OUT==deg) newVal=val*DD2R;
		IRA::CIRATools::radToSexagesimalAngle(newVal,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double radians;
		if (!IRA::CIRATools::declinationToRad(str,radians,RANGECHECK)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"declination_converter::strToVal()");
			throw ex;
		}
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

template <_sp_symbols OUT,bool RANGECHECK> class rightAscension_converter
{
public:
	char *valToStr(const double& val) {
		IRA::CString out("");
		double newVal;
		if (OUT==rad) newVal=val;
		else if (OUT==deg) newVal=val*DD2R;
		IRA::CIRATools::radToHourAngle(newVal,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double radians;
		if (!IRA::CIRATools::rightAscensionToRad(str,radians,RANGECHECK)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"rightAscension_type::strToVal()");
			throw ex;
		}
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

template <_sp_symbols OUT,bool RANGECHECK> class azimuth_converter
{
public:
	char *valToStr(const double& val) {
		IRA::CString out("");
		double newVal;
		if (OUT==rad) newVal=val;
		else if (OUT==deg) newVal=val*DD2R;
		IRA::CIRATools::radToAngle(newVal,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double radians;
		if (!IRA::CIRATools::azimuthToRad(str,radians,RANGECHECK)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"azimuth_converter::strToVal()");
			throw ex;
		}
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

template <_sp_symbols OUT,bool RANGECHECK> class elevation_converter
{
public:
	char *valToStr(const double& val) {
		IRA::CString out("");
		double newVal;
		if (OUT==rad) newVal=val;
		else if (OUT==deg) newVal=val*DD2R;
		IRA::CIRATools::radToAngle(newVal,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double radians;
		if (!IRA::CIRATools::elevationToRad(str,radians,RANGECHECK)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"elevation_converter::strToVal()");
			throw ex;
		}
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

template <_sp_symbols OUT,bool RANGECHECK> class galacticLatitude_converter
{
public:
	char *valToStr(const double& val) {
		IRA::CString out("");
		double newVal;
		if (OUT==rad) newVal=val;
		else if (OUT==deg) newVal=val*DD2R;
		IRA::CIRATools::radToAngle(newVal,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double radians;
		if (!IRA::CIRATools::galLatitudeToRad(str,radians,RANGECHECK)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"galacticLatitude_converter::strToVal()");
			throw ex;
		}
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

template <_sp_symbols OUT,bool RANGECHECK> class galacticLongitude_converter
{
public:
	char *valToStr(const double& val) {
		IRA::CString out("");
		double newVal;
		if (OUT==rad) newVal=val;
		else if (OUT==deg) newVal=val*DD2R;
		IRA::CIRATools::radToAngle(newVal,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double radians;
		if (!IRA::CIRATools::galLongitudeToRad(str,radians,RANGECHECK)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"galacticLongitude_converter::strToVal()");
			throw ex;
		}
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

class time_converter
{
public:
	char *valToStr(const ACS::Time& val) {
		IRA::CString out("");
		IRA::CIRATools::timeToStr(val,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	ACS::Time strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		ACS::Time ret;
		if (!IRA::CIRATools::strToTime(str,ret)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"time_converter::strToVal()");
			throw ex;
		}
		return ret;
	}
};

class interval_converter
{
public:
	char *valToStr(const ACS::TimeInterval& val) {
		IRA::CString out("");
		IRA::CIRATools::intervalToStr(val,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);
		return c;
	}
	ACS::TimeInterval strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		ACS::TimeInterval ret;
		if (!IRA::CIRATools::strToInterval(str,ret)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"interval_converter::strToVal()");
			throw ex;
		}
		return ret;
	}
};

class longSeq_converter
{
public:
	char *valToStr(const ACS::longSeq& val) {
		char tmp[2048];
		char single[32];
		char sep[2] = { _SP_MULTI_ARGUMENT_SEPARATOR ,0 };
		unsigned long len=val.length();
		strcpy(tmp,"");
		for (unsigned long i=0;i<len;i++) {
			sprintf(single,"%d",val[i]);
			strcat(tmp,single);
			if (i<len-1) strcat(tmp,sep);
		}
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
	ACS::longSeq strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		ACS::longSeq out;
		IRA::CString param(str);
		IRA::CString ret;
		int start=0;
		int xPos;
		CORBA::ULong index=0;
		long val;
		char *endptr, *token;
		if ((xPos=param.Find(_SP_MULTI_ARGUMENT_COMPACT_SEP))>0) {
			long longVal=param.Left(xPos).ToLong();
			long mult=param.Right(param.GetLength()-(xPos+1)).ToLong();
			out.length(mult);
			for (long j=0;j<mult;j++) {
				out[j]=longVal;
			}
		}
		else {
			while (IRA::CIRATools::getNextToken(param,start,_SP_MULTI_ARGUMENT_SEPARATOR,ret)) {
				errno=0;
				token=const_cast<char *>((const char *)ret);
				val=strtol(token,&endptr,10);
				if ((errno==ERANGE && (val==LONG_MAX || val==LONG_MIN)) || (errno != 0 && val == 0)) {
					_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"longSeq_converter::strToVal()");
					ex.setExpectedType("Long integer");
					throw ex;
				}
				if (endptr==token) {
					_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"longSeq_converter::strToVal()");
					ex.setExpectedType("Long integer");
					throw ex;
       			}
       			out.length(index+1);
       			out[index]=val;
       			index++;
			}
		}
		return out;
	}
};

class doubleSeq_converter
{
public:
	char *valToStr(const ACS::doubleSeq& val) {
		char tmp[2048];
		char single[32];
		char sep[2] = { _SP_MULTI_ARGUMENT_SEPARATOR ,0 };
		unsigned long len=val.length();
		strcpy(tmp,"");
		for (unsigned long i=0;i<len;i++) {
			sprintf(single,"%lf",val[i]);
			strcat(tmp,single);
			if (i<len-1) strcat(tmp,sep);
		}
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
	ACS::doubleSeq strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		ACS::doubleSeq out;
		IRA::CString param(str);
		IRA::CString ret;
		int start=0;
		int xPos;
		CORBA::ULong index=0;
		double val;
		char *endptr, *token;
		if ((xPos=param.Find(_SP_MULTI_ARGUMENT_COMPACT_SEP))>0) {
			double doubleVal=param.Left(xPos).ToDouble();
			long mult=param.Right(param.GetLength()-(xPos+1)).ToLong();
			out.length(mult);
			for (long j=0;j<mult;j++) {
				out[j]=doubleVal;
			}
		}
		else {
			while (IRA::CIRATools::getNextToken(param,start,_SP_MULTI_ARGUMENT_SEPARATOR,ret)) {
				errno=0;
				token=const_cast<char *>((const char *)ret);
				val=strtod(token,&endptr);
				if ((errno==ERANGE && (val==HUGE_VALF || val==HUGE_VALF)) || (errno != 0 && val == 0)) {
					_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"doubleSeq_converter::strToVal()");
					ex.setExpectedType("Double");
					throw ex;
				}
				if (endptr==token) {
					_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"doubleSeq_converter::strToVal()");
					ex.setExpectedType("Double");
					throw ex;
				}
				out.length(index+1);
				out[index]=val;
				index++;
			}
		}
		return out;
	}
};

class AntennaSection2String {
public:
	char *valToStr(const Antenna::TSections& val) {
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
	Antenna::TSections strToVal(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
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
	char *valToStr(const Antenna::TSystemEquinox& val) {
		char *c=new char[16];
		IRA::CString out;
		IRA::CIRATools::equinoxToStr(val,out);
		strcpy(c,(const char *)out);		
		return c;
	}
	Antenna::TSystemEquinox strToVal(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
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
	char *valToStr(const Antenna::TCoordinateFrame& val) {
		char *c=new char[16];
		IRA::CString out;
		IRA::CIRATools::coordinateFrameToStr(val,out);
		strcpy(c,(const char *)out);		
		return c;		
	}
	Antenna::TCoordinateFrame strToVal(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
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

/*class AntennaReferenceFrame2String {
public:
	static char *valToStr(Antenna::TFrame& val) {
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
};*/


};

#endif /*_SP_TYPES_CONVERSION_H_*/
