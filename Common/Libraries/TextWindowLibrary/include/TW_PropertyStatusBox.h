/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_PropertyStatusBox.h,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $																							                             */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 19/10/2006      Creation                                          */


#ifndef _TW_PROPERTYSTATUSBOX_H
#define _TW_PROPERTYSTATUSBOX_H


#include "TW_PropertyComponent.h"
#include <map>

namespace TW {

		
#define _TW_PROPERTYSTATUSBOX_C _TW_PROPERTYCOMPONENT_C,class EN
#define _TW_PROPERTYSTATUSBOX_TL _TW_PROPERTYCOMPONENT_TL,EN

/**
 * This template class inplements a status box. The box can get a fixed number of values; for each value a different drawing style and caption/description can be defined. This class inherits all
 * the types of the base class <i>CPropertyComponent</i>, plus: 
 * @arg \c EN enumeration type that enlists all the possible values that the box can assume.
 * This object is particularly suitable fro ACS enum and pettern properties.
*/
template <_TW_PROPERTYSTATUSBOX_C> 	
class CPropertyStatusBox : protected virtual CPropertyComponent <_TW_PROPERTYCOMPONENT_TL> {
public:
	/**
	 * Constructor.
	 * The read of the state descriptions from the ACS CDB is tried here. If it is failed the <i>getLastError()</i> member will return the 
	 * corresponding error message.
	 * @param property pointer to the ACS property that is associated to that box.
	 * @param init initial status value
	*/
	CPropertyStatusBox(PR property,const EN& init);
	/**
    * Destructor.
	*/
	virtual ~CPropertyStatusBox();
	
	/**
	 * This method is required to set the description and the style for each status.
	 * @param status indicates the value of the status for which you want to set the style and caption
	 * @param descr this is the caption that the control shows when the value property represent the status
	 * @apram style this is the style that is used to draw the control when the value property represent the status
	*/
	void setStatusLook(const EN& status,const IRA::CString& descr,const CStyle& style);
	
	/**
	 * This method is required to set the description for each status. The style used to draw the control is the default style, i.e. the style
	 * set using the member method <i>setStyle()</i> of the base class. 
	 * @param status indicates the value of the status for which you want to set the style and caption
	 * @param descr this is the caption that the control shows when the value property represent the status
	*/
	void setStatusLook(const EN& status,const IRA::CString& descr);

	/**
	 * This method is required to set the style for each status. The caption used to draw the control is taken from the ACS Configuration Data Base
	 * using the 'statesDescription' attribute of the associated property (it works only if the property is an ACS pattern property or an enum property)
	 * @param status indicates the value of the status for which you want to set the style and caption
	 * @apram style this is the style that is used to draw the control when the value property represent the status
	*/
	void setStatusLook(const EN& status,const CStyle& style);
	
	/**
	 * This method is required to set the default look and feel of the control for each status. The defaukt caption used to draw the control is taken from the ACS Configuration Data Base
	 * using the 'statesDescription' attribute of the associated property (it works only if the property is an ACS pattern property or an enum property). 
	 * While the default style is the style set using the member method <i>setStyle()</i> of the base class.
	 * @param status indicates the value of the status for which you want to set the style and caption
	 * @apram style this is the style that is used to draw the control when the value property represent the status
	*/
	void setStatusLook(const EN& status);
		
	/**
	 * This method is required to get the description and the style for one specific status.
	 * @param status indicates the value of the status for which you want to get the style and caption
	 * @param caption this is the caption associated to the requested status, if the status was not defined it returns an empty string
	 * @apram style this is the style that associated to requested status
	*/	
	void getStatusLook(const EN& status,IRA::CString& caption,CStyle& style);
	
	/**
	 * This method returns the description given the status.
	 * @param descr this is the status description 	 
	 * @param status this is the output argument that indicates the value of the status described by the first parameter
	 * @return true if the status description could be retrived, flase otherwise.
	*/		
	bool getStatusFromDescription(const IRA::CString& descr,EN& status);
	
	/**
	 * Sets the status value property. This value represents the string that will be displayed by the component.
	 * @param val the new value.
	*/
	void setValue(const IRA::CString& val);
	
	/**
	 * Sets the status value property. This value represents the new status of the component.
	 * @param val the new value.
	*/
	void setValue(const EN& val);
	

	using CFrameComponent::setPosition;
	using CFrameComponent::getPosition;
	using CFrameComponent::setTag;
	using CFrameComponent::getTag;
	using CFrameComponent::setStyle;
	using CFrameComponent::getStyle;
	using CFrameComponent::setWidth;
	using CFrameComponent::getWidth;
	using CFrameComponent::setHeight;
	using CFrameComponent::getHeight;
	using CFrameComponent::setOutputComponent;
	using CFrameComponent::getOutputComponent;
	using CFrameComponent::setEnabled;
	using CFrameComponent::getEnabled;
	using CFrameComponent::getValue;
	using CFrameComponent::Refresh;	
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setFormatFunction;	
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::installAutomaticMonitor;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setTriggerTime;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValueTrigger;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getLastError;	
	
protected:
	typedef struct {
		IRA::CString caption;
		EN status;
		CStyle style;
	} TElement;
	
	typedef std::map<EN,TElement> TMapTable;
	/**
	 * Represent the current status value of the component.
	*/
	EN m_statusValue;
	/** 
	 * This list enumerates all the possible status of the box.
	*/
	TMapTable m_valuesList;
	/**
	 * This method implements the pure virtual method of the <i>CFrameComponent</i> base class. This method contains all the instruction required to
	 * draw the component.
	*/
	WORD draw();
	/**
	 * This a string sequence used to stores the status description from the ACS CDB.
	*/
	ACS::stringSeq_var m_Descriptions;
};

#include "TW_PropertyStatusBox.i"

}

#endif
