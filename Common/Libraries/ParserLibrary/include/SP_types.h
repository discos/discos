/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: SP_types.h,v 1.10 2010-10-14 12:20:46 a.orlati Exp $											*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                			    when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/08/2008      Creation                                          */
/*Andrea Orlati(aorlati@ira.inaf.it)  15/04/2009       Added longSeq_type and doubleSeq_type       */
/*Andrea Orlati(aorlati@ira.inaf.it)  02/08/2010       added interval_type, declination_type and rightAscension_type       */
/*Andrea Orlati(aorlati@ira.inaf.it)  02/08/2010       included the compact way to provide sequences in case they are formed by identical values    */
/*Andrea Orlati(aorlati@ira.inaf.it)  10/10/2010       reviweb implementation of enum_type    */


#ifndef _SP_TYPES_H_
#define _SP_TYPES_H_

#include <String.h>
#include <ParserErrors.h>
#include <Definitions.h>
#include <slamac.h>
#include <slalib.h>
#include <math.h>
#include <acstimeEpochHelper.h>
#include <IRATools.h>

#define _SP_MULTI_ARGUMENT_SEPARATOR ';'
#define _SP_MULTI_ARGUMENT_COMPACT_SEP 'x'

namespace SimpleParser {

enum _sp_symbols {
	rad,
	deg
};
/**
 * This is the base class for all types that are known to the parser. New types can be easily added, the only constraints is that
 * the new type must derive from this basic type and give an implementation of the abstract methods <i>strToVal()</i> and
 * <i>valToStr()</i>. Constructors must be explicitly implemented while copy operators can be directly inherited.  
 * The template parameter is the corresponding c++ built-in type. 
 */
template <class T> class basic_type
{
public:
	typedef const T& in_type;
	typedef T& out_type;
	typedef T ret_type;
	basic_type() : m_strVal(NULL) {}
	basic_type(const basic_type& bt): m_val(bt.m_val),  m_strVal(NULL) { 	}
	basic_type(const T& val): m_val(val), m_strVal(NULL ) { }
	basic_type(const char *str) throw (ParserErrors::BadTypeFormatExImpl): m_strVal(NULL) {}
	virtual ~basic_type() {
		if (m_strVal!=NULL) delete[] m_strVal;
	}
	/**
	 * Initializes the type starting from another same-type object
	 */ 
	virtual const basic_type<T>& operator =( const basic_type& bt) {
		m_val=bt.m_val;
		return *this;
	}
	/**
	 * Initializes the type from the corresponding c++ build-in type
	 */
	virtual const basic_type<T>& operator =(const T& val) {
		m_val=val;
		return *this;
	}
	/**
	 * Initializes the type starting from a string.
	 * @throw  ParserErrors::BadTypeFormatExImpl if the string cannot be converted into the specific type (@sa <i>strToVal()</i>
	 */
	virtual const basic_type<T>& operator =(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		m_val=strToVal(str);
		return *this;
	}	
	/**
	 * Converts the type into a string 
	 */
	virtual operator const char *() {
		if (m_strVal!=NULL) delete[] m_strVal;
		m_strVal=valToStr(m_val);
		return m_strVal;
	}
	/**
	 * Thanks to this operator the type can be use as c++ built in type
	 */
	 virtual operator T&() {
		 return m_val;
	 }
protected:
	T m_val;
	char *m_strVal;
	virtual T strToVal(const char*str)=0;
	virtual char *valToStr(T& val)=0;
};

class string_type: public virtual basic_type<char *>
{
public:
	typedef const char * in_type;
	string_type() : basic_type<char *>() {
		m_val=NULL;
	}
	string_type(const string_type& bt): basic_type<char *>()  {
		m_val=NULL;
		clone(bt.m_val);
	}
	string_type(const char *src): basic_type<char *>()  {
		 m_val=NULL;
		clone(src);
	}
	virtual ~string_type() {
		if (m_val!=NULL) delete []m_val;
	}
	virtual const string_type& operator =( const string_type& bt) {
		clone(bt.m_val);
		return *this;
	}
	virtual const string_type& operator =(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
		clone(str);
		return *this;
	}
	virtual operator const char *() const {
		return m_val;
	}
	 virtual operator char *&() {
		 return m_val;
	 }
private:
	void clone(const char* src) {
		if (m_val!=NULL) delete[] m_val;
		m_val=new char [strlen(src)+1];
		strcpy(m_val,src);	
	}
	char *strToVal(const char*str) { return NULL; }
	char *valToStr(char *& val) { return NULL;}
};

// in is just a placeholder
class void_type: public virtual basic_type<int>
{
public:
	typedef const void *in_type;
	typedef void *out_type;
	typedef void ret_type;
	void_type(): basic_type<int>(), m_null("") {}
	virtual ~void_type() { }
	virtual operator const char *() {
		return (const char *) m_null;
	}
private:
	IRA::CString m_null;
	void_type(const void_type& bt); // disable copy constructor
	const void_type& operator =( const void_type& bt);
	char *valToStr(int& val) { return 0;}
	int strToVal(const char*str)  { return 0;}
};

class int_type: public virtual basic_type<int>
{
public:
	int_type() : basic_type<int>() {}
	int_type(const int_type& bt): basic_type<int>(bt)  { }
	int_type(const int& val): basic_type<int>(val) {}
	int_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<int>(str) { 
		m_val=strToVal(str);
	}
	virtual ~int_type() { }
	using basic_type<int>::operator =;
protected:
	char *valToStr(int& val) {
		char tmp[128];
		sprintf(tmp,"%d",val);
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
	int strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		int ret;
		if (sscanf(str,"%d",&ret)!=1) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"int_type::strToVal()");
			ex.setExpectedType("Integer");
			throw ex;
		}
		return ret;
	}
};

class long_type: public virtual basic_type<long>
{
public:
	long_type() : basic_type<long>() {}
	long_type(const long_type& bt): basic_type<long>(bt)  { }
	long_type(const long& val): basic_type<long>(val) {}
	long_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<long>(str) { 
		m_val=strToVal(str);
	}
	virtual ~long_type() { }
	using basic_type<long>::operator =;
protected:
	char *valToStr(long& val) {
		char tmp[128];
		sprintf(tmp,"%ld",val);
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
	long strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		long ret;
		if (sscanf(str,"%ld",&ret)!=1) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"int_type::strToVal()");
			ex.setExpectedType("Long Integer");
			throw ex;
		}
		return ret;
	}
};

class double_type: public virtual basic_type<double>
{
public:
	double_type() : basic_type<double>() {}
	double_type(const double_type& bt): basic_type<double>(bt)  { }
	double_type(const double& val): basic_type<double>(val) {}
	double_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~double_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
		char tmp[128];
		sprintf(tmp,"%lf",val);
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
	double strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		double ret;
		if (sscanf(str,"%lf",&ret)!=1) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"double_type::strToVal()");
			ex.setExpectedType("Double");
			throw ex;
		}
		return ret;
	}
};

template <class ENUM_CONV,class ENUM=long>class enum_type: public virtual basic_type<ENUM>
{
public:
	enum_type() : basic_type<ENUM>() {}
	enum_type(const enum_type& bt): basic_type<ENUM>(bt)  { }
	enum_type(const ENUM& val): basic_type<ENUM>(val) {}
	enum_type(const char *str) throw (ParserErrors::BadTypeFormatExImpl) : basic_type<ENUM>(str) {
		basic_type<ENUM>::m_val=strToVal(str);
	}
	virtual ~enum_type() { }
	using basic_type<ENUM>::operator =;
protected:
	char *valToStr(ENUM& val) {
		return ENUM_CONV::valToStr(val);		
	}
	ENUM strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		return ENUM_CONV::strToVal(str);
	}	
};

/**
 * convert the format +/-ddd.ddd and normalize the result 
 */
template <_sp_symbols OUT>class angle_type: public virtual basic_type<double>
{
public:
	angle_type() : basic_type<double>() {}
	angle_type(const angle_type& bt): basic_type<double>(bt)  { }
	angle_type(const double& val): basic_type<double>(val) {}
	angle_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~angle_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
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
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"angle_type::strToVal()");
			throw ex;			
		}		
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;	
	}
};

/**
 * convert the format +/-ddd.ddd'd' or dd:mm:ss.sss into radians or degrees, no normalization or range check 
 */
template <_sp_symbols OUT>class angleOffset_type: public virtual basic_type<double>
{
public:
	angleOffset_type() : basic_type<double>() {}
	angleOffset_type(const angleOffset_type& bt): basic_type<double>(bt)  { }
	angleOffset_type(const double& val): basic_type<double>(val) {}
	angleOffset_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~angleOffset_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
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
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"angleOffset_type::strToVal()");
			throw ex;			
		}		
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;	
	}
};

/**
 * convert the format +/-dd:mm:ss.sss or +/-ddd.ddd'd' between ranges -90°...90° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical declination ranges
 */
template <_sp_symbols OUT,bool rangeCheck>class declination_type: public virtual basic_type<double>
{
public:
	declination_type() : basic_type<double>() {}
	declination_type(const declination_type& bt): basic_type<double>(bt)  { }
	declination_type(const double& val): basic_type<double>(val) {}
	declination_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~declination_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
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
		if (!IRA::CIRATools::declinationToRad(str,radians,rangeCheck)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"declination_type::strToVal()");
			throw ex;			
		}		
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

/**
 * convert the format +/-hh:mm:ss.sss'h' 0<hh<24 or +/-ddd.ddd'd' or dd:mm:ss.sss  between ranges 0...360° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical right ascension ranges 
 */
template <_sp_symbols OUT,bool rangeCheck>class rightAscension_type: public virtual basic_type<double>
{
public:
	rightAscension_type() : basic_type<double>() {}
	rightAscension_type(const rightAscension_type& bt): basic_type<double>(bt)  { }
	rightAscension_type(const double& val): basic_type<double>(val) {}
	rightAscension_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~rightAscension_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
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
		if (!IRA::CIRATools::rightAscensionToRad(str,radians,rangeCheck)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"rightAscension_type::strToVal()");
			throw ex;			
		}		
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

/**
 * convert the format +/-ddd.ddd'd' between ranges 0...360° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical azimuth ranges 
 */
template <_sp_symbols OUT,bool rangeCheck>class azimuth_type: public virtual basic_type<double>
{
public:
	azimuth_type() : basic_type<double>() {}
	azimuth_type(const azimuth_type& bt): basic_type<double>(bt)  { }
	azimuth_type(const double& val): basic_type<double>(val) {}
	azimuth_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~azimuth_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
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
		if (!IRA::CIRATools::azimuthToRad(str,radians,rangeCheck)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"azimuth_type::strToVal()");
			throw ex;			
		}		
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

/**
 * convert the format +/-ddd.ddd'd' between ranges 0...90° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical elevation ranges 
 */
template <_sp_symbols OUT,bool rangeCheck>class elevation_type: public virtual basic_type<double>
{
public:
	elevation_type() : basic_type<double>() {}
	elevation_type(const elevation_type& bt): basic_type<double>(bt)  { }
	elevation_type(const double& val): basic_type<double>(val) {}
	elevation_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~elevation_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
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
		if (!IRA::CIRATools::elevationToRad(str,radians,rangeCheck)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"elevation_type::strToVal()");
			throw ex;			
		}		
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

/**
 * convert the format +/-ddd.ddd'd' or +/-dd:mm:ss.s between ranges 0...360° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical galactic latitude ranges 
 */
template <_sp_symbols OUT,bool rangeCheck>class galacticLatitude_type: public virtual basic_type<double>
{
public:
	galacticLatitude_type() : basic_type<double>() {}
	galacticLatitude_type(const galacticLatitude_type& bt): basic_type<double>(bt)  { }
	galacticLatitude_type(const double& val): basic_type<double>(val) {}
	galacticLatitude_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~galacticLatitude_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
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
		if (!IRA::CIRATools::galLatitudeToRad(str,radians,rangeCheck)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"galacticLatitude_type::strToVal()");
			throw ex;			
		}		
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

/**
 * convert the format +/-ddd.ddd'd' or +/-dd:mm:ss.s between ranges -90...90° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical galactic longitude ranges 
 */
template <_sp_symbols OUT,bool rangeCheck>class galacticLongitude_type: public virtual basic_type<double>
{
public:
	galacticLongitude_type() : basic_type<double>() {}
	galacticLongitude_type(const galacticLongitude_type& bt): basic_type<double>(bt)  { }
	galacticLongitude_type(const double& val): basic_type<double>(val) {}
	galacticLongitude_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double>(str) {
		m_val=strToVal(str);
	}
	virtual ~galacticLongitude_type() { }
	using basic_type<double>::operator =;
protected:
	char *valToStr( double& val) {
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
		if (!IRA::CIRATools::galLongitudeToRad(str,radians,rangeCheck)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"galacticLongitude_type::strToVal()");
			throw ex;			
		}		
		if (OUT==rad) return radians;
		else if (OUT==deg) return radians*DR2D;
	}
};

/**
 * convert the format yyyy-doy-hh:mm.ss.sss
 */
class time_type: public virtual basic_type<ACS::Time>
{
public:
	time_type() : basic_type<ACS::Time>() {}
	time_type(const time_type& bt): basic_type<ACS::Time>(bt)  { }
	time_type(const ACS::Time& val): basic_type<ACS::Time>(val) {}
	time_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<ACS::Time>(str) { 
		m_val=strToVal(str);
	}
	virtual ~time_type() { }
	using basic_type<ACS::Time>::operator =;
protected:
	char *valToStr(ACS::Time& val) {
		IRA::CString out("");
		IRA::CIRATools::timeToStr(val,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);		                  		
		return c;
	}
	ACS::Time strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		ACS::Time ret;
		if (!IRA::CIRATools::strToTime(str,ret)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"time_type::strToVal()");
			throw ex;			
		}
		return ret;
	}
};

/**
 * convert the format ddd-hh:mm.ss.sss
 */
class interval_type: public virtual basic_type<ACS::TimeInterval>
{
public:
	interval_type() : basic_type<ACS::TimeInterval>() {}
	interval_type(const interval_type& bt): basic_type<ACS::TimeInterval>(bt)  { }
	interval_type(const ACS::Time& val): basic_type<ACS::TimeInterval>(val) {}
	interval_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<ACS::TimeInterval>(str) { 
		m_val=strToVal(str);
	}
	virtual ~interval_type() { }
	using basic_type<ACS::TimeInterval>::operator =;
protected:
	char *valToStr(ACS::TimeInterval& val) {
		IRA::CString out("");
		IRA::CIRATools::intervalToStr(val,out);
		char *c=new char [out.GetLength()+1];
		strcpy(c,(const char *)out);		                  		
		return c;
	}
	ACS::TimeInterval strToVal(const char*str) throw (ParserErrors::BadTypeFormatExImpl) {
		ACS::TimeInterval ret;
		if (!IRA::CIRATools::strToInterval(str,ret)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"interval_type::strToVal()");
			throw ex;			
		}
		return ret;
	}
};

class longSeq_type: public virtual basic_type<ACS::longSeq>
{
public:
	longSeq_type() : basic_type<ACS::longSeq>() {}
	longSeq_type(const longSeq_type& bt): basic_type<ACS::longSeq>(bt)  { }
	longSeq_type(const ACS::longSeq& val): basic_type<ACS::longSeq>(val) {}
	longSeq_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<ACS::longSeq>(str) { 
		m_val=strToVal(str);
	}
	virtual ~longSeq_type() { }
	using basic_type<ACS::longSeq>::operator =;
protected:
	char *valToStr(ACS::longSeq& val) {
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
					_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"longSeq_type::strToVal()");
					ex.setExpectedType("Long integer");
					throw ex;
				}
				if (endptr==token) {
					_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"longSeq_type::strToVal()");
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

class doubleSeq_type: public virtual basic_type<ACS::doubleSeq>
{
public:
	doubleSeq_type() : basic_type<ACS::doubleSeq>() {}
	doubleSeq_type(const doubleSeq_type& bt): basic_type<ACS::doubleSeq>(bt)  { }
	doubleSeq_type(const ACS::doubleSeq& val): basic_type<ACS::doubleSeq>(val) {}
	doubleSeq_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<ACS::doubleSeq>(str) { 
		m_val=strToVal(str);
	}
	virtual ~doubleSeq_type() { }
	using basic_type<ACS::doubleSeq>::operator =;
protected:
	char *valToStr(ACS::doubleSeq& val) {
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
					_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"doubleSeq_type::strToVal()");
					ex.setExpectedType("Double");
					throw ex;
				}
				if (endptr==token) {
					_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"doubleSeq_type::strToVal()");
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

};


#endif /*_SP_TYPES_H_*/
