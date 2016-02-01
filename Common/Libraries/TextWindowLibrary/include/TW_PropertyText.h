/*****************************************************************************************************  */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_PropertyText.h,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $																							        */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/02/2006      Creation                                          */


#ifndef _TW_PROPERTYTEXT_H
#define _TW_PROPERTYTEXT_H

#include "TW_PropertyComponent.h"

namespace TW {

/**
 * This class implements a property component as a text field. A text field is a read-only component that allows to display
 * the value of a property formatted according specific formatting functions. THis class inherits from the <i>CPropertyComponent</i>
 * class so it can provides all its capabilities to handle ACS properties.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 	
*/	
template <_TW_PROPERTYCOMPONENT_C> 	
class CPropertyText : protected virtual CPropertyComponent <_TW_PROPERTYCOMPONENT_TL> {
public:
	/**
	 * Constructor. 
	 * @param property pointer to the ACS property that is associated to that text field.
	*/
	CPropertyText(PR property);
	/**
     * Destructor.
	*/
	virtual ~CPropertyText();

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
	using CFrameComponent::setHAlign;
	using CFrameComponent::getHAlign;
	using CFrameComponent::setWAlign;
	using CFrameComponent::getWAlign;
	using CFrameComponent::setOutputComponent;
	using CFrameComponent::getOutputComponent;
	using CFrameComponent::setEnabled;
	using CFrameComponent::getEnabled;
	using CFrameComponent::setValue;
	using CFrameComponent::getValue;
	using CFrameComponent::Refresh;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setFormatFunction;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::installAutomaticMonitor;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setTriggerTime;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValueTrigger;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getLastError;	
	
protected:
	/**
	 * This method implements the pure virtual method of the <i>CFrameComponent</i> base class. This method contains all the instruction required to
	 * draw the component.
	*/
	WORD draw();
};

#include "TW_PropertyText.i"

}


#endif
