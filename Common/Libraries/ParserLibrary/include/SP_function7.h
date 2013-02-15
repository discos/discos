/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: SP_function7.h,v 1.4 2010-09-13 16:41:21 a.orlati Exp $											*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                			    when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/08/2008      Creation                                          */

#ifndef SP_FUNCTION7_H_
#define SP_FUNCTION7_H_

#include "SP_functor.h"

namespace SimpleParser {

template<class OBJ,_sp_const_function CONST_SPEC,class RT,class A0,class A1,class A2,class A3,class A4,class A5,class A6>
class function7: public _SP_FUNCTOR
{
public:
	typedef typename RET<RT>::return_type return_type;
	typedef typename RET<RT>::argument return_argument;
	typedef typename A0::argument arg0;
	typedef typename A0::parameter_type param0;
	typedef typename A1::argument arg1;
	typedef typename A1::parameter_type param1; 
	typedef typename A2::argument arg2;
	typedef typename A2::parameter_type param2;
	typedef typename A3::argument arg3;
	typedef typename A3::parameter_type param3;
	typedef typename A4::argument arg4;
	typedef typename A4::parameter_type param4;
	typedef typename A5::argument arg5;
	typedef typename A5::parameter_type param5; 
	typedef typename A6::argument arg6;
	typedef typename A6::parameter_type param6;
	
	typedef return_type (OBJ::*normal_func_type)(param0,param1,param2,param3,param4,param5,param6);
	typedef return_type (OBJ::*const_func_type)(param0,param1,param2,param3,param4,param5,param6) const;
	typedef typename conditional<CONST_SPEC,const_func_type,normal_func_type>::type func_type;
	
	function7(OBJ *object, func_type func) : _SP_FUNCTOR(object,7),m_func(func) {};
	return_type operator()(arg0& pp0,arg1& pp1,arg2& pp2,arg3& pp3,arg4& pp4,arg5& pp5,arg6& pp6) const throw (ACSErr::ACSbaseExImpl) {
		return (*_SP_FUNCTOR::m_obj.*m_func)( pp0,pp1,pp2,pp3,pp4,pp5,pp6); //can throw a generic exception
	}
	void call(IRA::CString *params,const WORD& parLen) throw (ParserErrors::ConversionErrorExImpl,ACSErr::ACSbaseExImpl) {
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
			if (A2::isInput) {
				if (pCount<parLen) {
					p2=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A3::isInput) {
				if (pCount<parLen) {
					p3=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A4::isInput) {
				if (pCount<parLen) {
					p4=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A5::isInput) {
				if (pCount<parLen) {
					p5=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A6::isInput) {
				if (pCount<parLen) {
					p6=(const char *)params[pCount];
					pCount++;
				}
			}			
		}
		catch (ParserErrors::BadTypeFormatExImpl& err) {
			_ADD_BACKTRACE(ParserErrors::ConversionErrorExImpl,impl,err,"function7::call()");
			throw impl;
		}
		catch (...) {
			_THROW_EXCPT(ParserErrors::ConversionErrorExImpl,"function7::call()");
		}
		retVal=(*_SP_FUNCTOR::m_obj.*m_func)(p0,p1,p2,p3,p4,p5,p6); //can throw a generic exception
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
		if (A2::isOutput) {
			params[n]=(const char*)p2;
			n++;
		}
		if (A3::isOutput) {
			params[n]=(const char*)p3;
			n++;
		}
		if (A4::isOutput) {
			params[n]=(const char*)p4;
			n++;
		}		
		if (A5::isOutput) {
			params[n]=(const char*)p5;
			n++;
		}		
		if (A6::isOutput) {
			params[n]=(const char*)p6;
			n++;
		}		
		return n;
	}
protected:
	func_type m_func;
	arg0 p0;
	arg1 p1;
	arg2 p2;
	arg3 p3;
	arg4 p4;
	arg5 p5;
	arg6 p6;
	return_argument retVal;
};

template<class OBJ,_sp_const_function CONST_SPEC,class A0,class A1,class A2,class A3,class A4,class A5,class A6> 
class function7<OBJ,CONST_SPEC,void_type,A0,A1,A2,A3,A4,A5,A6>: public _SP_FUNCTOR
{
public:
	typedef typename RET<void_type>::return_type return_type;
	typedef typename RET<void_type>::argument return_argument;
	typedef typename A0::argument arg0;
	typedef typename A0::parameter_type param0;
	typedef typename A1::argument arg1;
	typedef typename A1::parameter_type param1; 
	typedef typename A2::argument arg2;
	typedef typename A2::parameter_type param2;
	typedef typename A3::argument arg3;
	typedef typename A3::parameter_type param3;
	typedef typename A4::argument arg4;
	typedef typename A4::parameter_type param4;
	typedef typename A5::argument arg5;
	typedef typename A5::parameter_type param5; 
	typedef typename A6::argument arg6;
	typedef typename A6::parameter_type param6;
	
	typedef return_type (OBJ::*normal_func_type)(param0,param1,param2,param3,param4,param5,param6);
	typedef return_type (OBJ::*const_func_type)(param0,param1,param2,param3,param4,param5,param6) const;
	typedef typename conditional<CONST_SPEC,const_func_type,normal_func_type>::type func_type;
	
	function7(OBJ *object, func_type func) : _SP_FUNCTOR(object,7),m_func(func) {};
	void  operator()(arg0& pp0,arg1& pp1,arg2& pp2,arg3& pp3,arg4& pp4,arg5& pp5,arg6& pp6) const throw (ACSErr::ACSbaseExImpl) {
		(*_SP_FUNCTOR::m_obj.*m_func)( pp0,pp1,pp2,pp3,pp4,pp5,pp6); //can throw a generic exception
	}
	void call(IRA::CString *params,const WORD& parLen) throw (ParserErrors::ConversionErrorExImpl,ACSErr::ACSbaseExImpl) {
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
			if (A2::isInput) {
				if (pCount<parLen) {
					p2=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A3::isInput) {
				if (pCount<parLen) {
					p3=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A4::isInput) {
				if (pCount<parLen) {
					p4=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A5::isInput) {
				if (pCount<parLen) {
					p5=(const char *)params[pCount];
					pCount++;
				}
			}
			if (A6::isInput) {
				if (pCount<parLen) {
					p6=(const char *)params[pCount];
					pCount++;
				}
			}			
		}
		catch (ParserErrors::BadTypeFormatExImpl& err) {
			_ADD_BACKTRACE(ParserErrors::ConversionErrorExImpl,impl,err,"function7::call()");
			throw impl;
		}
		catch (...) {
			_THROW_EXCPT(ParserErrors::ConversionErrorExImpl,"function7::call()");
		}
		(*_SP_FUNCTOR::m_obj.*m_func)(p0,p1,p2,p3,p4,p5,p6); //can throw a generic exception
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
		if (A2::isOutput) {
			params[n]=(const char *)p2;
			n++;
		}
		if (A3::isOutput) {
			params[n]=(const char *)p3;
			n++;
		}
		if (A4::isOutput) {
			params[n]=(const char *)p4;
			n++;
		}
		if (A5::isOutput) {
			params[n]=(const char *)p5;
			n++;
		}
		if (A6::isOutput) {
			params[n]=(const char *)p6;
			n++;
		}
		return n;				
	}
protected:
	func_type m_func;
	arg0 p0;
	arg1 p1;
	arg2 p2;
	arg3 p3;
	arg4 p4;
	arg5 p5;
	arg6 p6;
};

};




#endif /*SP_FUNCTION7_H_*/
