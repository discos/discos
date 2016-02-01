/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: SP_functor.h,v 1.4 2010-09-13 16:41:21 a.orlati Exp $											*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                			    when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/08/2008      Creation                                          */

#ifndef _SP_FUNCTOR_H_
#define _SP_FUNCTOR_H_

#define _SP_FUNCTOR CFunctor<OBJ>

namespace SimpleParser {

enum _sp_const_function {
	constant,
	non_constant
};

template<_sp_const_function cond,typename ifConst,typename ifNonConst> struct conditional {
	typedef ifConst type; 
};

template<typename ifConst,typename ifNonConst> struct conditional<non_constant,ifConst,ifNonConst> {
	typedef ifNonConst type; 
};

/**
 * This template is used to specialize types into output or input/output parameters
 */
template <class TYPE> class O
{
public: 
	typedef typename TYPE::out_type parameter_type;
	typedef TYPE argument;
	enum { 
		isOutput=1
	}; 
	enum {
		isInput=0
	};
};

/**
 * This template is used to specialize type into input parameters
 */
template <class TYPE> class I
{
public:
	typedef typename TYPE::in_type parameter_type;
	typedef TYPE argument;
	enum {
		isOutput=0
	}; 
	enum {
		isInput=1
	};	
};

/**
 * This template is used to specialize type into input/output parameters
 */
template <class TYPE> class IO
{
public:
	typedef typename TYPE::out_type parameter_type;
	typedef TYPE argument;
	enum {
		isOutput=1
	};
	enum {
		isInput=1
	};	
};

/**
 * This template is used to specialize type into input parameters
 */
template <class TYPE> class RET
{
public:
	typedef typename TYPE::ret_type return_type;
	typedef TYPE argument;
};

/**
 * This is a simple function container all function inherits from this class
 */
class CBaseFunction
{
public:
	CBaseFunction(WORD ar) : m_arity(ar) {  }

	virtual ~CBaseFunction() { }
	/**
	 * This function returns back the total number of parameters (output/input) that the function can manage.
	 */
	WORD arity() const { return m_arity; }
protected:
	WORD m_arity;
};

/**
 * This is the special function wrapper that contains the method to execute the function.
 */
template<class OBJ> class CFunctor : public  CBaseFunction
{
public:
	CFunctor(OBJ *object,WORD ar):   CBaseFunction(ar) , m_obj(object) { };
	virtual ~CFunctor() { };
	/**
	 * This pure virtual method allows the owner of this class to call the function without knowing the details and the signature of the
	 * function itself.
	 * @param params parameters of the function as strings, the number is not fixed but depends on the particular implementation of
	 * @param parLen number of string parameters
	 * the function. 
	*/
	virtual void call(IRA::CString *params,const WORD& parLen)=0;
	/**
	 * This function can be used to get back the return value of the function (if not void) and then the value of the output parameters.
	 * Values of input parameters are ignored.
	 * @param params array that contains the values to be returned, if the function is not void the first value is the return value of the
	 *                function. 
	 * @return the number of value returned.
	 */
	virtual WORD get(IRA::CString *params)=0;

protected:
	OBJ *m_obj;
};

};

#endif /*_SP_FUNCTOR_H_*/

