/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_PropertyComponent.h,v 1.4 2010-04-09 12:37:07 a.orlati Exp $									  */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 18/01/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 10/09/2007      Modified the installAutomaticMonitor in order to  */
/*                                                    set up the log guard interval into the callback   */
/* Andrea Orlati(aorlati@ira.inaf.it) 09/04/2010      Added a type parameter list for strings, needed to fix a problem related to monitor creation  */

#ifndef _TW_PROPERTYCOMPONENT_H
#define _TW_PROPERTYCOMPONENT_H

#include "TW_FrameComponent.h"
#include "TW_Callbacks.h"
#include <String.h>
#include <baciS.h>
#include <baci.h>
#include <IRATools.h>

namespace TW {

/**
 * Use these macro to quickly pass the type parameters to the template if you plan to use standard ACS string type
 * This macro is due to the call to <i>create_monitor()</i> of string properies does not return a Monitorstring but a Monitor  
 * This is for Read-Only properties.
*/
#define _TW_PROPERTYCOMPONENT_T_ROSTRING ACS::ROstring_ptr,ACS::Monitorstring,ACS::Monitor_var,_TW_CBstring,ACS::CBstring_var

/**
 * Use these macro to quickly pass the type parameters to the template if you plan to use standard ACS string type
 * This macro is due to the call to <i>create_monitor()</i> of string properies does not return a Monitorstring but a Monitor*   
 * This is for Read-Write properties.
*/
#define _TW_PROPERTYCOMPONENT_T_RWSTRING ACS::RWstring_ptr,ACS::Monitorstring,ACS::Monitor_var,_TW_CBstring,ACS::CBstring_var

/**
 * Use these macro to quickly pass the type parameters to the template if you plan to use standard ACS types and bluilt-in callbacks objects. 
 * This is for Read-Write properties.
 * @param PROP the property type mnemonic. It could be: double, long, longLong, uLongLong or pattern
*/
#define _TW_PROPERTYCOMPONENT_T_RW(PROP) ACS::RW##PROP##_ptr,ACS::Monitor##PROP,ACS::Monitor##PROP##_var,_TW_CB##PROP,ACS::CB##PROP##_var
	
/**
 * Use these macro to quickly pass the type parameters to the template if you plan to use standard ACS types and bluilt-in callbacks objects.
 * This is for Read-Only properties.
 * @param PROP the property type mnemonic. It could be: double, long, longLong, uLongLong or pattern
*/	
#define _TW_PROPERTYCOMPONENT_T_RO(PROP) ACS::RO##PROP##_ptr,ACS::Monitor##PROP,ACS::Monitor##PROP##_var,_TW_CB##PROP,ACS::CB##PROP##_var

/**
 * Use these macro to quickly pass the type parameters to the template if you plan to use standard ACS types and bluilt-in callbacks objects. 
 * This is for Read-Write sequence properties.
 * @param PROP the property type mnemonic. It could be: double, long
*/	
#define _TW_SEQPROPERTYCOMPONENT_T_RW(PROP) ACS::RW##PROP##Seq_ptr,ACS::Monitor##PROP,ACS::Monitor##PROP##_var,_TW_CB##PROP##Seq,ACS::CB##PROP##Seq_var

/**
 * Use these macro to quickly pass the type parameters to the template if you plan to use standard ACS types and bluilt-in callbacks objects. 
 * This is for Read-Only sequence properties.
 * @param PROP the property type mnemonic. It could be: double, long
*/	
#define _TW_SEQPROPERTYCOMPONENT_T_RO(PROP) ACS::RO##PROP##Seq_ptr,ACS::Monitor##PROP,ACS::Monitor##PROP##_var,_TW_CB##PROP##Seq,ACS::CB##PROP##Seq_var

#define _TW_PROPERTYCOMPONENT_C class PR,class M,class MV,class C,class CV
#define _TW_PROPERTYCOMPONENT_TL PR,M,MV,C,CV

/**
 * This template class represents the controls that are linked against ACS component properties and display thier values according to the property type.
 * This class does not provide an implementation of base class method <i>draw()</i> so cannot be instantiated directly.
 * The ACS property value can be updated/displayed in 2 ways:
 * @arg \c 1 Manually calling the <i>CFrameComponent::setValue()</i> every time it is needed.
 * @arg \c 2 Installing an automatic monitor by calling the <i>installAutomaticMonitor()</i> method once and for all. In this case the callback specified by 
 *           C argument will be used.
 * The list of types of the template is:
 * @arg \c PR ACS property class pointer
 * @arg \c M ACS monitor class
 * @arg \c MV ACS monitor class smartpointer (CORBA object)
 * @arg \c C user callback class
 * @arg \c CV ACS callback class smartpointer (CORBA object)
 * The following example illustrates and clarifies the usage of this class. Please notice that 
 * it will not compile because this class is pure virtual...you need to provide an implementation of draw() first:
 * <pre>
 *		ACS::ROdouble_var prop1;
 *    ACS::ROlong_var prop2;
 *      ..............
 *		//create the propertyComponent object for a read-only double property:
 *		CPropertyComponent<TW_PROPERTYCOMPONENT_T_RO(double)> comp1(prop1.in());
 *		comp1.installAutomaticMonitor();
 *		CPropertyComponent<ACS::ROlong_ptr,ACS::Monitorlong,ACS::Monitorlong_var,MyCBlong,ACS::CBlong_var> comp2(prop2.in());
 *    comp1.installAutomaticMonitor();
 * </pre>
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
template <_TW_PROPERTYCOMPONENT_C> 
class CPropertyComponent : public virtual CFrameComponent {
public:

	/**
	 * Constructor.
	 * @param property pointer to the ACS property that will be associated to the component.
	*/
	CPropertyComponent(PR property);
	
	/**
    * Destructor.
	*/
	virtual ~CPropertyComponent();
	
	/**
	 * This function should be used in case an error occured and it would be necessary to know more details about the cause of the problem. 
    * @return a message that describes the last occured error.
	*/
	inline const IRA::CString& getLastError() const { return m_LastError; }
	
	/** 
	 * Call this method to install a monitor over the property, using the <i>CStandardCallback</i> callback or a derived class.
	 * @param logInterval this parameter is used to change the value (milliseconds) of the log guard inside the <i>CStandardCallback</i> callback. 
	 *        An error when reading the values became automatically a log, so in order to prevent to flood the system with a huge ammount of logs
	 *        the callback is capable to filter them out. @sa <i>IRA::CLogGaurd</i>. A value of zero diable this feature.   
	 * @return false if an error occurs, in this case use the <i>getLastError()</i> function in order to get the corrisponding error message
	*/
	bool installAutomaticMonitor(const DWORD& logInterval=0);
	
	/**
	 * Sets the gap between two monitor triggering. A value of zero disables the monitor even if a value_trigger was set.
  	 * @param millisec the trigger time in milliseconds.
	*/ 
	void setTriggerTime(DWORD millisec);
	
	/** 
	 * This method enstablishes a value change monitor. That is to say the monitor is triggered every time the property value changes.
	 * @param delta if the value of the property changes by a "delta" value the monitor callback is invoked
	 * @param enable true enables the value change, false disables it.
	*/
	void setValueTrigger(double delta,bool enable=true);

	/** 
	 * This method enstablishes a value change monitor. That is to say the monitor is triggered every time the property value changes.
	 * @param delta if the value of the property changes by a "delta" value the monitor callback is invoked
	 * @param enable true enables the value change, false disables it.
	*/
	void setValueTrigger(long delta,bool enable=true);

	/** 
	 * This method enstablishes a value change monitor. That is to say the monitor is triggered every time the property value changes.
	 * @param delta if the value of the property changes by a "delta" value the monitor callback is invoked
	 * @param enable true enables the value change, false disables it.
	*/
	void setValueTrigger(long long delta,bool enable=true);
	
	/** 
	 * This method enstablishes a value change monitor. That is to say the monitor is triggered every time the property value changes.
	 * @param delta if the value of the property changes by a "delta" value the monitor callback is invoked
	 * @param enable true enables the value change, false disables it.
	*/
	void setValueTrigger(unsigned long long delta,bool enable=true);

	/**
	 * This method sets the function which is in charge of formatting the data incoming from the automatic monitor.
	 * @param func pointer to the formatting function. if NULL the default function will be used.
	 * @param arg this is the argument that will be passed to the formatting function.
	*/
	void setFormatFunction(CCommonCallback::TFormatFunction func=NULL,const void* arg=NULL);

protected: 
	IRA::CString m_LastError;
	/** Ovverriden method deActivate */
	virtual void deActivate();
	/** Ovverriden method Activate */
	virtual void Activate();
private:
	PR m_property_p;
	MV m_monitor_p;
	CV m_callback_p;
	C m_callback;
	/** Indicates if an automatic monitor has been installed over the associated property */
	bool m_installed;
};

#include "TW_PropertyComponent.i"

}

#endif
