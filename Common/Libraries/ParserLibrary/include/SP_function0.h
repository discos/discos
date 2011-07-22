/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: SP_function0.h,v 1.4 2010-09-13 16:41:21 a.orlati Exp $											*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                			    when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/08/2008      Creation                                          */

#ifndef _SP_FUNCTION0_H_
#define _SP_FUNCTION0_H_

#include "SP_functor.h"

namespace SimpleParser {

template<class OBJ,_sp_const_function CONST_SPEC,class RT> class function0: public _SP_FUNCTOR
{
public:
	enum {arity=0};
	typedef typename RET<RT>::return_type return_type;
	typedef typename RET<RT>::argument return_argument;
	typedef return_type (OBJ::*const_func_type)() const;
	typedef return_type (OBJ::*normal_func_type)();
	typedef typename conditional<CONST_SPEC,const_func_type,normal_func_type>::type func_type;
	
	function0(OBJ *object, func_type func) : _SP_FUNCTOR(object),m_func(func) {};
	return_type operator()() const throw (ACSErr::ACSbaseExImpl) {
		return (*_SP_FUNCTOR::m_obj.*m_func)( ); //can throw a generic exception
	}
	void call(IRA::CString *params,const WORD& parLen) throw (
			ParserErrors::ConversionErrorExImpl,ACSErr::ACSbaseExImpl) {
		retVal=(*_SP_FUNCTOR::m_obj.*m_func)(); //can throw a generic exception
	}
	WORD get(IRA::CString *params)  {
		params[0]=(const char *)retVal;
		return 1;
	}
protected:
	func_type m_func;
	return_argument retVal;
};

template<class OBJ,_sp_const_function CONST_SPEC> class function0<OBJ,CONST_SPEC,void_type>: public _SP_FUNCTOR
{
public:
	enum {arity=0};
	typedef typename RET<void_type>::return_type return_type;
	typedef typename RET<void_type>::argument return_argument;
	typedef return_type (OBJ::*normal_func_type)();
	typedef return_type (OBJ::*const_func_type)() const;
	typedef typename conditional<CONST_SPEC,const_func_type,normal_func_type>::type func_type;
	
	function0(OBJ *object, func_type func) : _SP_FUNCTOR(object),m_func(func) {};
	void  operator()() const throw (ACSErr::ACSbaseExImpl) {
		(*_SP_FUNCTOR::m_obj.*m_func)(); //can throw a generic exception
	}
	void call(IRA::CString *params,const WORD& parLen) throw (
			ParserErrors::ConversionErrorExImpl,ACSErr::ACSbaseExImpl) {
		(*_SP_FUNCTOR::m_obj.*m_func)(); //can throw a generic exception
	}
	WORD get(IRA::CString *params)  {
		return 0;
	}
protected:
	func_type m_func;
};

};






#endif /*_SP_FUNCTION0_H_*/
