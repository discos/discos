/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: SP_function2.h,v 1.4 2010-09-13 16:41:21 a.orlati Exp $											*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                			    when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/08/2008      Creation                                          */

#ifndef _SP_FUNCTION2_H_
#define _SP_FUNCTION2_H_

#include "SP_functor.h"

namespace SimpleParser {

template<class OBJ,_sp_const_function CONST_SPEC,class RT,class A0,class A1> class function2: public _SP_FUNCTOR
{
public:
	enum {arity=2};
	typedef typename RET<RT>::return_type return_type;
	typedef typename RET<RT>::argument return_argument;
	typedef typename A0::argument arg0;
	typedef typename A0::parameter_type param0;
	typedef typename A1::argument arg1;
	typedef typename A1::parameter_type param1; 
	typedef return_type (OBJ::*normal_func_type)(param0,param1);
	typedef return_type (OBJ::*const_func_type)(param0,param1) const;
	typedef typename conditional<CONST_SPEC,const_func_type,normal_func_type>::type func_type;
	
	function2(OBJ *object, func_type func) : _SP_FUNCTOR(object),m_func(func) {};
	return_type operator()(arg0& pp0,arg1& pp1) const throw (ACSErr::ACSbaseExImpl) {
		return (*_SP_FUNCTOR::m_obj.*m_func)( pp0,pp1 ); //can throw a generic exception
	}
	void call(IRA::CString *params,const WORD& parLen) throw (
			ParserErrors::ConversionErrorExImpl,ACSErr::ACSbaseExImpl) {
		WORD pCount=0;
		try {
			if (A0::isInput) {
				if (pCount<parLen) {
					p0=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A1::isInput) {
				if (pCount<parLen) {
					p1=(const char *)params[pCount];
					pCount++;
				}
			}			
		}
		catch (ParserErrors::BadTypeFormatExImpl& err) {
			_ADD_BACKTRACE(ParserErrors::ConversionErrorExImpl,impl,err,"function2::call()");
			throw impl;
		}
		catch (...) {
			_THROW_EXCPT(ParserErrors::ConversionErrorExImpl,"function2::call()");
		}
		retVal=(*_SP_FUNCTOR::m_obj.*m_func)(p0,p1); //can throw a generic exception
	}
	WORD get(IRA::CString *params)  {
		int n=1;
		params[0]=(const char *)retVal;
		if (A0::isOutput) {
			params[n]=(const char*)p0;
			n++;
		}
		if (A1::isOutput) {
			params[n]=(const char*)p1;
			n++;
		}
		return n;
	}
protected:
	func_type m_func;
	arg0 p0;
	arg1 p1;
	return_argument retVal;
};

template<class OBJ,_sp_const_function CONST_SPEC,class A0,class A1> class function2<OBJ,CONST_SPEC,void_type,A0,A1>: public _SP_FUNCTOR
{
public:
	enum {arity=2};
	typedef typename RET<void_type>::return_type return_type;
	typedef typename RET<void_type>::argument return_argument;
	typedef typename A0::argument arg0;
	typedef typename A0::parameter_type param0; 
	typedef typename A1::argument arg1;
	typedef typename A1::parameter_type param1; 
	typedef return_type (OBJ::*normal_func_type)(param0,param1);
	typedef return_type (OBJ::*const_func_type)(param0,param1) const;
	typedef typename conditional<CONST_SPEC,const_func_type,normal_func_type>::type func_type;
	
	function2(OBJ *object, func_type func) : _SP_FUNCTOR(object),m_func(func) {};
	void  operator()(arg0& pp0,arg1& pp1) const throw (ACSErr::ACSbaseExImpl) {
		(*_SP_FUNCTOR::m_obj.*m_func)( pp0,pp1 ); //can throw a generic exception
	}
	void call(IRA::CString *params,const WORD& parLen) throw (
			ParserErrors::ConversionErrorExImpl,ACSErr::ACSbaseExImpl) {
		WORD pCount=0;
		try {
			if (A0::isInput) {
				if (pCount<parLen) {
					p0=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A1::isInput) {
				if (pCount<parLen) {
					p1=(const char *)params[pCount];
					pCount++;
				}
			}			
		}
		catch (ParserErrors::BadTypeFormatExImpl& err) {
			_ADD_BACKTRACE(ParserErrors::ConversionErrorExImpl,impl,err,"function2::call()");
			throw impl;
		}
		catch (...) {
			_THROW_EXCPT(ParserErrors::ConversionErrorExImpl,"function2::call()");
		}
		(*_SP_FUNCTOR::m_obj.*m_func)(p0,p1); //can throw a generic exception
	}
	WORD get(IRA::CString *params)  {
		int n=0;
		if (A0::isOutput) {
			params[n]=(const char*)p0;
			n++;
		}
		if (A1::isOutput) {
			params[n]=(const char*)p1;
			n++;
		}		
		return n;		
	}
protected:
	func_type m_func;
	arg0 p0;
	arg1 p1;
};

};

#endif /*SP_FUNCTION2_H_*/
