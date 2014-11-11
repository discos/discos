/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_Callbacks.h,v 1.7 2010-09-13 10:30:50 a.orlati Exp $																							                             */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 13/01/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 10/09/2007      added a log guard in the callback                 */
/* Andrea Orlati(aorlati@ira.inaf.it) 04/05/2008      added support for sequence properties        */
/* Andrea Orlati(aorlati@ira.inaf.it) 09/04/2010      fixed a bug in CFormatFunctions::defaultFormat that caused a seg fault when the associated property is a string  */
/* Andrea Orlati(aorlati@ira.inaf.it) 12/09/2010      reviwed implementation of the formatting functions: angleFormat and coordinateFormat  */


#ifndef _TW_CALLBACKS_H
#define _TW_CALLBACKS_H

#include "TW_FrameComponent.h"
#include <baci.h>
#include <IRATools.h>
#include <Definitions.h>
#include <LogFilter.h>

namespace TW {

class CCommonCallback;

/**
 * This class provides a collection of ready-to-use formatting functions.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
*/
class CFormatFunctions
{
friend class CCommonCallback;
public:
	/** 
 	* This function is the formatting function that converts from unsigned long long to date/time format. It should be used to format 
 	* values of <i>IRA::TIMEVALUE</i>.
	* It can be passed as argument of the <i>CCommonCallback::setFormatFunction()</i> method.
	* @param value BACIValue that must be formatted.
	* @param arg this parameter is ignored.
	* @return the string containing the formatted value.
	*/
	static IRA::CString dateTimeClockFormat(const baci::BACIValue& value,const void* arg);

	/** 
 	* This function is the formatting function that converts from unsigned long long to day+time format. It should be used to 
 	* format values of <i>IRA::TIMEDIFFERENCE</i>.
	* It can be passed as argument of the <i>CCommonCallback::setFormatFunction()</i> method.
	* @param value BACIValue that must be formatted.
	* @param arg this parameter is ignored.
	* @return the string containing the formatted value.
	*/
	static IRA::CString timeClockFormat(const baci::BACIValue& value,const void* arg);

	/** 
 	* This function is a formatting function that converts from unsigned long long to time format. It should be used to 
 	* format values of <i>IRA::TIMEDIFFERENCE</i>.
	* It can be passed as argument of the <i>CCommonCallback::setFormatFunction()</i> method.
	* @param value BACIValue that must be formatted.
	* @param arg this parameter is ignored.
	* @return the string containing the formatted value.
	*/	
	static IRA::CString clockFormat(const baci::BACIValue& value,const void* arg);
	
	/** 
 	* This function is a formatting function that formats an angle starting from a double. The angle can be formatted as
 	* angle(360°) or hour angle (24h) as well. The input value is radian value of the angle that must be formatted.  
	* It can be passed as argument of the <i>CCommonCallback::setFormatFunction()</i> method.
	* @param value BACIValue object that contains the double value of the angle, as radians.
	* @param arg if NULL the angle is formatted as +/-degrees:arcmin:arcsec.sss, otherwise if not NULL the angle if formatted 
	* according to the pattern hh:min:sec.sss
	* @return the string containing the formatted value.
	*/		
	static IRA::CString angleFormat(const baci::BACIValue& value,const void* arg);
	
	/** 
 	* This function is a formatting function that formats a coordinate given as an angle starting from a double value. The angle is formatted according  +/-ddd.ddd.
 	* The input value is a radian value.  
	* It can be passed as argument of the <i>CCommonCallback::setFormatFunction()</i> method.
	* @param value BACIValue object that contains the double value of the coordinate, as radians.
	* @param arg this parameter is ignored. 
	* @return the string containing the formatted value.
	*/		
	static IRA::CString coordinateFormat(const baci::BACIValue& value,const void* arg);

	/** 
 	* This function is a formatting function that formats the value as a floating point number. It can be passed as argument of the 
	* <i>CCommonCallback::setFormatFunction()</i> method.
	* @param value BACIValue that must be formatted.
	* @param arg if null this argument is ignored otherwise it should point to a character string that should contain the format directives in the 'printf' style.
	* @return the string containing the formatted value.
	*/
	static IRA::CString floatingPointFormat(const baci::BACIValue& value,const void* arg);

	/** 
 	* This function is a formatting function that formats the value as an integer number. It can be passed as argument of the 
	* <i>CCommonCallback::setFormatFunction()</i> method.
	* @param value BACIValue that must be formatted.
	* @param arg if null this argument is ignored otherwise it should point to a character string that should contain the format directives in the 'printf' style.
	* @return the string containing the formatted value.
	*/	
	static IRA::CString integerFormat(const baci::BACIValue& value,const void* arg);
private:
	/** 
	* This function is the default formatting function.
	* @param value BACIValue that must be formatted.
	* @param arg this is a placeholder.
	* @return the string containing the formatted value.	 
	*/
	static IRA::CString defaultFormat(const baci::BACIValue& value,const void* arg);
};
	
/**
 * This is a base class for all common callbacks. All the callback must inherits from this class in order to be used with the <i>CPropertyComponent</i> object.
 * The new callback class must also inherits from a baciCallback class and exposing the methods <i>working()</i>, <i>done()</i>, <i>negotiate()</i>. For that reason
 * you probably find more convignent to inherits from the <i>CStandardCallback</i> template class.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
*/	
class CCommonCallback
{
public:
	typedef IRA::CString (*TFormatFunction)(const baci::BACIValue&,const void*); 
	/**
	 * Constructor.
	 * @param isSequence if true the callback will handle the value coming back from the related property as a sequence of values
	*/
	CCommonCallback(const bool& isSequence=false);
	/**
	 * Destructor.
	*/
	virtual ~CCommonCallback();
	/**
	 * This method is used to check if the callback is enabled or not.
	 * @return true if the callback is stopped, that means it will not perform any operation
	*/
	bool isStopped() const;
	/** 
	 * This method starts the callback.
	*/
	void Start();
	/** 
	 * This method stops the callback.
	*/
	void Stop();
	/**
	 * Use this method to get the reference to the frame component in charge of displaying the value coming from the monitor.
	 * @return pointer to the frame component
	*/
	CFrameComponent *getFrameComponent() const;
	/**
     * Use this method to set the reference to the frame component in charge of displaying the value coming from the monitor.
	 * @param comp pointer to the frame component
    */
	void setFrameComponent(CFrameComponent* comp);
	/**
    * Use this method to set the function that is in charge of formatting the data.
	 * @param func pointer to the formatting function. The default is NULL, i.e. the default function will be used (@sa <i>DefaultFormat()</i>)
	 * @param arg this is the argument that will be passed to the formatting function.
    */	
	void setFormatFunction(TFormatFunction func=NULL,const void* arg=NULL);
		
protected:
	/** Pointer to the format function */
	TFormatFunction m_formatFunction;
	/** The argument passed to the installed formatting function */
	const void* m_formatArgument;
	
	template<typename T> long _tw_getTypeLength(const T& val) { return 1; }
	long _tw_getTypeLength(const char * val) { return 1; }													// added to match the string needs
	long _tw_getTypeLength(const ACS::longSeq& val) { return val.length(); }
	long _tw_getTypeLength(const ACS::doubleSeq& val) { return val.length(); }
	
	template<typename T> T _tw_getTypeValue(const T& val,const long& pos) { return val; }
	const char * _tw_getTypeValue(const char* val,const long& pos) { return val; }                         // added to match the string needs
	BACIdouble _tw_getTypeValue(const ACS::doubleSeq& val,const long& pos) { return (BACIdouble)val[pos]; }
	BACIlong _tw_getTypeValue(const ACS::longSeq& val,const long& pos) { return (BACIlong)val[pos]; }
	
private:
	/** Indicates wether the callback is enabled (allowed to perform the <i>working()</i> method) or not */
	bool m_started;
	/** Pointer to the frame component that displays the value of the property monitored using this callback */
	CFrameComponent* m_frameComponent;
	
protected:
	/** Indicates if the call back is used to monitor a normal property or a sequence property */
	bool m_isSequence;

};
	
/**
 * This is a standard ACS callback class usable together with the <i>CPropertyComponent</i> classes. You could also inherit from this class if you want special behaviours.
 * In this case the simpliest way is to override the <i>Action()</i> method. Another way to have special behaviour is to provide a new formatting function. (@sa 
 * <i>CPropertyComponent::setFormatFunction()</i>).
 * @arg \c CT CORBA type (it could be CORBA::Double,CORBA::Long.....const char * or ACS::pattern), this is the type of the value that this class should manage.
 * @arg \c POA This is the POA_ACS callback object that this class inherits from.
*/
template <class CT, class POA, bool ISSEQ = false> 
class CStandardCallback : public virtual POA, public CCommonCallback {
public:
	// added in order to support also sequences
	typedef typename IRA::conditional_type<ISSEQ,const CT&,CT>::type RCT; //redefined CORBA type
	/**
     * Constructor.
     * @param isSequence if true the callback will handle the value coming back from the related property as a sequence of values
 	*/
	CStandardCallback();

	/**
	 * Destructor.
	*/
	virtual ~CStandardCallback();

	/**
	 * This method is invoked every time the property monitor generates a new value. it internally calls the <i>Action()</i> method. 
     * This method should not be overriden.
	 * @param value this is the new property value
	 * @param c reference to the completion structure. Some information about the result fo the operation are reported
	 * @param desc callback description structure
	*/
	void working(RCT value,const ACSErr::Completion &c,const ACS::CBDescOut &desc) throw (CORBA::SystemException);

	/**
	 * This method is invoked when the monitor has finished its activity. This method should not be overriden.
	 * @param value this is the last property value
	 * @param c reference to the completion structure. Some information about the result of the operation are reported
	 * @param desc callback description structure
	*/		
	void done(RCT value,const ACSErr::Completion &c,const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	
	/**
     * This should be used to negotiate time out. Normally, the client returns true which means that the asynchronous request 
	 * should stay in the queue and that the client expects the callback notification when the call executes on the remote side.
	 * This capabilities is not usually needed.
	 * @param time_to_transmit timeout before the client will be notificated about the result of the operation
	 * @param desc callback description structure.				
	*/
	CORBA::Boolean negotiate (ACS::TimeInterval time_to_transmit,const ACS::CBDescOut &desc) throw (CORBA::SystemException) {
		return true;
	}
	
	/**
	 * This method changes the default value of the interval that the log guard will check before forwarding a log to the logger.
	 * The default value of this interval iz zero that means the guard is disabled. Call this method before starting the callback.
	 * @sa class <i>IRA::CLogGuard</i>
	 * @param interval new time interval in microseconds
	*/
	void setGuardInterval(const DDWORD& interval);
		
protected:
    /**
     * This the time interval set up in the log guard.
    */
	DDWORD m_guardInterval;
	/**
	 * Log Guard to prevent log flooding in the working method 
	*/
	IRA::CLogGuard m_workingGuard;

	/**
	 * This method is invoked by the <i>working()</i> method every time a new value is ready(with no errors) and the callback is not stopped. It performs all the 
	 * operation needed by this new value, i.e. calling the formatting function and the writing the value in the associated component. The easier way to provide 
	 * special behaviour is to inherits from this class and override this method. 
	 * @param value this is the new value.
	*/
	virtual void Action(RCT value);
};

typedef CStandardCallback<CORBA::Double,POA_ACS::CBdouble> _TW_CBdouble;
typedef CStandardCallback<CORBA::Long,POA_ACS::CBlong> _TW_CBlong;
typedef CStandardCallback<CORBA::LongLong,POA_ACS::CBlongLong> _TW_CBlongLong;
typedef CStandardCallback<CORBA::ULongLong,POA_ACS::CBuLongLong> _TW_CBuLongLong;
typedef CStandardCallback<const char*,POA_ACS::CBstring> _TW_CBstring;
typedef CStandardCallback<ACS::pattern,POA_ACS::CBpattern> _TW_CBpattern;
typedef CStandardCallback<ACS::doubleSeq,POA_ACS::CBdoubleSeq,true> _TW_CBdoubleSeq;
typedef CStandardCallback<ACS::longSeq,POA_ACS::CBlongSeq,true> _TW_CBlongSeq;

#include "TW_Callbacks.i"
	
}






#endif
