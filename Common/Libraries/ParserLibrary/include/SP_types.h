/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
//*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                			    when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/08/2008      Creation                                          */
/*Andrea Orlati(aorlati@ira.inaf.it)  15/04/2009       Added longSeq_type and doubleSeq_type       */
/*Andrea Orlati(aorlati@ira.inaf.it)  02/08/2010       added interval_type, declination_type and rightAscension_type       */
/*Andrea Orlati(aorlati@ira.inaf.it)  02/08/2010       included the compact way to provide sequences in case they are formed by identical values    */
/*Andrea Orlati(aorlati@ira.inaf.it)  10/10/2010       reviewed implementation of enum_type    */


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
#include "SP_typeConversion.h"

#define _SP_JOLLYCHARACTER '*'
#define _SP_JOLLYCHARACTER_REPLACEMENT "-1"

namespace SimpleParser {

#define _SP_WILDCARD_CLASS(NAME,REPLACE) \
class NAME { \
public: \
	static char *replace(const char *str) { \
		if (str[0]==_SP_JOLLYCHARACTER) { \
			char * out=new char[strlen(REPLACE)+1]; \
			strcpy(out,REPLACE); \
			return out; \
		} \
		else { \
			char *tmp=new char[strlen(str)+1]; \
			strcpy(tmp,str); \
			return tmp; \
		} \
	} \
}; \

_SP_WILDCARD_CLASS(_default_wildcard,_SP_JOLLYCHARACTER_REPLACEMENT);

class _no_wildcard
{
public:
	static char *replace(const char *str) {
		char *tmp=new char[strlen(str)+1];
		strcpy(tmp,str);
		return tmp;
	}
};

/**
 * This is the base class for all types that are known to the parser. New types can be easily added, just providing the converter class.
 * The converter class just have to implement the proper methods, <i>strToVal()</i> and <i>valToStr()</i>.
 * The template parameter is the corresponding c++ built-in type.
 */
template <class T,class CONVERTER,class WILDCARD=_default_wildcard> class basic_type
{
public:
	typedef const T& in_type;
	typedef T& out_type;
	typedef T ret_type;
	basic_type() : m_strVal(NULL)  {}
	basic_type(const basic_type& bt):  m_strVal(NULL) {
		//clone(bt.m_strVal);
		m_val=bt.m_val;
	}
	basic_type(const T& val):  m_strVal(NULL ) { m_strVal=m_conv.valToStr(val); m_val=val; }
	basic_type(const char *str) throw (ParserErrors::BadTypeFormatExImpl): m_strVal(NULL) {
		clone(str);
		try  {
			convert(); // check the string contains proper value
		}
		catch (ParserErrors::BadTypeFormatExImpl& ex) {
			if (m_strVal!=NULL) delete[] m_strVal;
			m_strVal=NULL;
			throw ex;
		}
	}
	virtual ~basic_type() {
		if (m_strVal!=NULL) delete[] m_strVal;
		m_strVal=NULL;
	}
	/**
	 * Initializes the type starting from another same-type object
	 */ 
	virtual const basic_type<T,CONVERTER,WILDCARD>& operator =( const basic_type& bt) {
		clone(bt.m_strVal);
		m_val=bt.m_val;
		return *this;
	}
	/**
	 * Initializes the type from the corresponding c++ build-in type
	 */
	virtual const basic_type<T,CONVERTER,WILDCARD>& operator =(const T& val) {
		if (m_strVal!=NULL) delete[] m_strVal;
		m_strVal=m_conv.valToStr(val);
		m_val=val;
		return *this;
	}
	/**
	 * Initializes the type starting from a string.
	 * @throw  ParserErrors::BadTypeFormatExImpl if the string cannot be converted into the specific type (@sa <i>strToVal()</i>
	 */
	virtual const basic_type<T,CONVERTER,WILDCARD>& operator =(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		clone(str);
		convert();
		return *this;
	}
	/**
	 * Converts the type into a string 
	 */
	virtual operator const char *() {
        if (m_strVal!=NULL) delete[] m_strVal;
         m_strVal=m_conv.valToStr(m_val);
         return m_strVal;
	}
	/**
	 * Thanks to this operator the type can be use as c++ built in type
	 */
	 virtual operator T&() {
		 return m_val;
	 }
protected:
	 char *m_strVal;
	 T m_val;
	 CONVERTER m_conv;
	/**
	 * Clone the strVal member from a source string.
	 */
	void clone(const char* src) {
		if (m_strVal!=NULL) delete[] m_strVal;
		m_strVal=new char[strlen(src)+1];
		strcpy(m_strVal,src);
	}
private:
	void convert() throw (ParserErrors::BadTypeFormatExImpl) {
		char *str=WILDCARD::replace(m_strVal);
		try {
			m_val=m_conv.strToVal(str);
		}
		catch (ParserErrors::BadTypeFormatExImpl& ex) {
			delete []str;
			throw ex;
		}
		delete []str;
	}
};

typedef basic_type<int,int_converter> int_type;
typedef basic_type<long,long_converter> long_type;
typedef basic_type<double,double_converter> double_type;
typedef basic_type<ACS::Time,time_converter> time_type;
typedef basic_type<ACS::TimeInterval,interval_converter> interval_type;
typedef basic_type<ACS::longSeq,longSeq_converter> longSeq_type;
typedef basic_type<ACS::doubleSeq,doubleSeq_converter> doubleSeq_type;
typedef basic_type<IRA::CString,longString_converter> longString_type;


class string_type: public virtual basic_type<char *,string_converter,_no_wildcard>
{
public:
        typedef const char * in_type;
        string_type() : basic_type<char *,string_converter,_no_wildcard>() {
                m_val=NULL;
        }
        string_type(const string_type& bt): basic_type<char *,string_converter,_no_wildcard>()  {
                m_val=NULL;
                string_type::clone(bt.m_val);
        }
        string_type(const char *src): basic_type<char *,string_converter,_no_wildcard>()  {
                 m_val=NULL;
                 string_type::clone(src);
        }
        virtual ~string_type() {
                if (m_val!=NULL) delete []m_val;
        }
        virtual const string_type& operator =( const string_type& bt) {
        	string_type::clone(bt.m_val);
            return *this;
        }
        virtual const string_type& operator =(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
        	string_type::clone(str);
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
};


// it is just a placeholder
class void_type: public virtual basic_type<int,class void_converter,_default_wildcard>
{
public:
        typedef const void *in_type;
        typedef void *out_type;
        typedef void ret_type;
        void_type(): basic_type<int,class void_converter,_default_wildcard>(), m_null("") {}
        virtual ~void_type() { }
        virtual operator const char *() {
                return (const char *) m_null;
        }
private:
        IRA::CString m_null;
        void_type(const void_type& bt); // disable copy constructor
        const void_type& operator =( const void_type& bt);
};

template <class ENUM_CONV,class ENUM=long,class WILDCARD=_default_wildcard> class enum_type : public virtual basic_type<ENUM,ENUM_CONV,WILDCARD>
{
public:
	enum_type() : basic_type<ENUM,ENUM_CONV,WILDCARD>() {}
	enum_type(const enum_type& bt): basic_type<ENUM,ENUM_CONV,WILDCARD>(bt)  { }
	enum_type(const ENUM& val): basic_type<ENUM,ENUM_CONV,WILDCARD>(val) {}
	enum_type(const char *str) throw (ParserErrors::BadTypeFormatExImpl) : basic_type<ENUM,ENUM_CONV,WILDCARD>(str) {}
	virtual ~enum_type() { }
	using basic_type<ENUM,ENUM_CONV,WILDCARD>::operator =;
};

/**
 * convert the format +/-ddd.ddd and normalize the result
 */
template <_sp_symbols OUT,class WILDCARD=_default_wildcard>class angle_type: public virtual basic_type<double,angle_converter<OUT>, WILDCARD>
{
public:
	angle_type() : basic_type<double,angle_converter<OUT>, WILDCARD>() {}
	angle_type(const angle_type& bt): basic_type<double,angle_converter<OUT>, WILDCARD>(bt)  { }
	angle_type(const double& val): basic_type<double,angle_converter<OUT>, WILDCARD>(val) {}
	angle_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double,angle_converter<OUT>, WILDCARD>(str) {}
	virtual ~angle_type() { }
	using basic_type<double,angle_converter<OUT>,WILDCARD>::operator =;
};

/**
 * convert the format +/-ddd.ddd'd' or dd:mm:ss.sss into radians or degrees, no normalization or range check
 */
template <_sp_symbols OUT,class WILDCARD=_default_wildcard>class angleOffset_type: public virtual basic_type<double,angleOffset_converter<OUT>,WILDCARD>
{
public:
	angleOffset_type() : basic_type<double,angleOffset_converter<OUT>,WILDCARD>() {}
	angleOffset_type(const angleOffset_type& bt): basic_type<double,angleOffset_converter<OUT>,WILDCARD>(bt)  { }
	angleOffset_type(const double& val): basic_type<double,angleOffset_converter<OUT>,WILDCARD>(val) {}
	angleOffset_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double,angleOffset_converter<OUT>,WILDCARD>(str) {	}
	virtual ~angleOffset_type() { }
	using basic_type<double,angleOffset_converter<OUT>,WILDCARD>::operator =;
};

/**
 * convert the format +/-dd:mm:ss.sss or +/-ddd.ddd'd' between ranges -90°...90° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical declination ranges
 */
template <_sp_symbols OUT,bool RANGECHECK,class WILDCARD=_default_wildcard>class declination_type:
	public virtual basic_type<double,declination_converter<OUT,RANGECHECK>, WILDCARD>
{
public:
	declination_type() : basic_type<double,declination_converter<OUT,RANGECHECK>, WILDCARD>() {}
	declination_type(const declination_type& bt): basic_type<double,declination_converter<OUT,RANGECHECK>, WILDCARD>(bt)  { }
	declination_type(const double& val): basic_type<double,declination_converter<OUT,RANGECHECK>, WILDCARD>(val) {}
	declination_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double,declination_converter<OUT,RANGECHECK>, WILDCARD>(str) {}
	virtual ~declination_type() { }
	using basic_type<double,declination_converter<OUT,RANGECHECK>, WILDCARD>::operator =;
};

/**
 * convert the format +/-hh:mm:ss.sss'h' 0<hh<24 or +/-ddd.ddd'd' or dd:mm:ss.sss  between ranges 0...360° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical right ascension ranges 
 */
template <_sp_symbols OUT,bool RANGECHECK,class WILDCARD=_default_wildcard>class rightAscension_type:
	public virtual basic_type<double,rightAscension_converter<OUT,RANGECHECK>, WILDCARD>
{
public:
	rightAscension_type() : basic_type<double,rightAscension_converter<OUT,RANGECHECK>, WILDCARD>() {}
	rightAscension_type(const rightAscension_type& bt): basic_type<double,rightAscension_converter<OUT,RANGECHECK>, WILDCARD>(bt)  { }
	rightAscension_type(const double& val): basic_type<double,rightAscension_converter<OUT,RANGECHECK>, WILDCARD>(val) {}
	rightAscension_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double,rightAscension_converter<OUT,RANGECHECK>, WILDCARD>(str) {}
	virtual ~rightAscension_type() { }
	using basic_type<double,rightAscension_converter<OUT,RANGECHECK>, WILDCARD>::operator =;
};

/**
 * convert the format +/-ddd.ddd'd' between ranges 0...360° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical azimuth ranges 
 */
template <_sp_symbols OUT,bool RANGECHECK,class WILDCARD=_default_wildcard>class azimuth_type:
	public virtual basic_type<double,azimuth_converter<OUT,RANGECHECK>,WILDCARD >
{
public:
	azimuth_type() : basic_type<double,azimuth_converter<OUT,RANGECHECK>,WILDCARD >() {}
	azimuth_type(const azimuth_type& bt): basic_type<double,azimuth_converter<OUT,RANGECHECK>,WILDCARD >(bt)  { }
	azimuth_type(const double& val): basic_type<double,azimuth_converter<OUT,RANGECHECK>,WILDCARD >(val) {}
	azimuth_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double,azimuth_converter<OUT,RANGECHECK>,WILDCARD >(str) {	}
	virtual ~azimuth_type() { }
	using basic_type<double,azimuth_converter<OUT,RANGECHECK>,WILDCARD >::operator =;
};

/**
 * convert the format +/-ddd.ddd'd' between ranges 0...90° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical elevation ranges 
 */
template <_sp_symbols OUT,bool RANGECHECK,class WILDCARD=_default_wildcard>class elevation_type:
	public virtual basic_type<double,elevation_converter<OUT,RANGECHECK>,WILDCARD>
{
public:
	elevation_type() : basic_type<double,elevation_converter<OUT,RANGECHECK>,WILDCARD>() {}
	elevation_type(const elevation_type& bt): basic_type<double,elevation_converter<OUT,RANGECHECK>,WILDCARD>(bt)  { }
	elevation_type(const double& val): basic_type<double,elevation_converter<OUT,RANGECHECK>,WILDCARD>(val) {}
	elevation_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double,elevation_converter<OUT,RANGECHECK>,WILDCARD>(str) {}
	virtual ~elevation_type() { }
	using basic_type<double,elevation_converter<OUT,RANGECHECK>,WILDCARD>::operator =;
};

/**
 * convert the format +/-ddd.ddd'd' or +/-dd:mm:ss.s between ranges 0...360° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical galactic latitude ranges 
 */
template <_sp_symbols OUT,bool RANGECHECK,class WILDCARD=_default_wildcard>class galacticLatitude_type:
	public virtual basic_type<double,galacticLatitude_converter<OUT,RANGECHECK>,WILDCARD>
{
public:
	galacticLatitude_type() : basic_type<double,galacticLatitude_converter<OUT,RANGECHECK>,WILDCARD>() {}
	galacticLatitude_type(const galacticLatitude_type& bt): basic_type<double,galacticLatitude_converter<OUT,RANGECHECK>,WILDCARD>(bt)  { }
	galacticLatitude_type(const double& val): basic_type<double,galacticLatitude_converter<OUT,RANGECHECK>,WILDCARD>(val) {}
	galacticLatitude_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double,galacticLatitude_converter<OUT,RANGECHECK>,WILDCARD>(str) {	}
	virtual ~galacticLatitude_type() { }
	using basic_type<double,galacticLatitude_converter<OUT,RANGECHECK>,WILDCARD>::operator =;
};

/**
 * convert the format +/-ddd.ddd'd' or +/-dd:mm:ss.s between ranges -90...90° 
 * OUT, control if the input/output is done in degrees or radians, rangeCheck allows to block inputs out of typical galactic longitude ranges 
 */
template <_sp_symbols OUT,bool RANGECHECK,class WILDCARD=_default_wildcard>class galacticLongitude_type:
	public virtual basic_type<double,galacticLongitude_converter<OUT,RANGECHECK>,WILDCARD>
{
public:
	galacticLongitude_type() : basic_type<double,galacticLongitude_converter<OUT,RANGECHECK>,WILDCARD>() {}
	galacticLongitude_type(const galacticLongitude_type& bt): basic_type<double,galacticLongitude_converter<OUT,RANGECHECK>,WILDCARD>(bt)  { }
	galacticLongitude_type(const double& val): basic_type<double,galacticLongitude_converter<OUT,RANGECHECK>,WILDCARD>(val) {}
	galacticLongitude_type(const char *str)  throw (ParserErrors::BadTypeFormatExImpl) : basic_type<double,galacticLongitude_converter<OUT,RANGECHECK>,WILDCARD>(str) {}
	virtual ~galacticLongitude_type() { }
	using basic_type<double,galacticLongitude_converter<OUT,RANGECHECK>,WILDCARD>::operator =;
};

};




/*template <_SP_PLACEHOLDER_FUNCTION_SIGNATURE=&_PHDefault>class void_type: public virtual basic_type<int,F>
{
public:
	typedef const void *in_type;
	typedef void *out_type;
	typedef void ret_type;
	void_type(): basic_type<int,&_PHDefault>(), m_null("") {}
	virtual ~void_type() { }
	virtual operator const char *() {
		return (const char *) m_null;
	}
protected:
	IRA::CString m_null;
	void_type(const void_type& bt); // disable copy constructor
	const void_type& operator =( const void_type& bt);
	char *valToStr(const int& val) { return 0;}
	int strToVal(const char*str)  { return 0;}
};*/



#endif /*_SP_TYPES_H_*/
