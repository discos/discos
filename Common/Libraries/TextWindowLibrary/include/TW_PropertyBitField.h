/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_PropertyBitField.h,v 1.2 2010-12-28 19:09:26 a.orlati Exp $																							                             */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 29/04/2009      Creation                                          */


#ifndef _TW_PROPERTYBITFIELD_H
#define _TW_PROPERTYBITFIELD_H

#include "TW_PropertyComponent.h"

namespace TW {

/**
 * This template class implements a control that can display a pattern as field of bits. The aspect of the single bits is controlled via the configuration DataBase
 * by setting the "whenSet" and "whenCleared" attributes of the pattern property. 
 * The control will try to display the bit description and the corresponding led, so it dimensions itself in order to fit everything. No user dimensioning
 * is permitted.
 * Differently from the <i>CPropertyLedDisplay</i> control this is ACS pattern specific and the look and feel is  exclusively controlled by the CDB.  
 * This object is only suitable fro ACS pattern properties. If other properties are given it will not compile.
*/
template <_TW_PROPERTYCOMPONENT_C> 	
class CPropertyBitField : protected virtual CPropertyComponent <_TW_PROPERTYCOMPONENT_TL> {
public:
	/**
	 * Constructor.
	 * The read of the "bitDescriptions", "whenSet", "whenCleared" fields from the ACS CDB is tried here. If it is failed the <i>getLastError()</i> 
	 * member will return the corresponding error message.
	 * @param property pointer to the ACS property (must the a pattern) that is associated to that control.
	*/
	CPropertyBitField(PR property);
	/**
    * Destructor.
	*/
	virtual ~CPropertyBitField();
	
	using CFrameComponent::setPosition;
	using CFrameComponent::getPosition;
	using CFrameComponent::setTag;
	using CFrameComponent::getTag;
	using CFrameComponent::setStyle;
	using CFrameComponent::getStyle;
	using CFrameComponent::getWidth;
	using CFrameComponent::getHeight;
	using CFrameComponent::setWidth;
	using CFrameComponent::setHeight;	
	using CFrameComponent::getWAlign;
	using CFrameComponent::setWAlign;
	using CFrameComponent::setOutputComponent;
	using CFrameComponent::getOutputComponent;
	using CFrameComponent::setEnabled;
	using CFrameComponent::getEnabled;
	using CFrameComponent::getValue;
	using CFrameComponent::setValue;
	using CFrameComponent::Refresh;		
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::installAutomaticMonitor;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setTriggerTime;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValueTrigger;
	using CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getLastError;	
	
	/**
	 * It allows to hide/show the description labels in the component. The labels are taken from the bitDescription attribute of the associated property
	 * @param val if the true the labels are shown, if false the labels are hidden.
	 */ 
	void showDescriptionLabels(bool val) { m_drawLabels=val; }
		
protected:
		/**
	 * This method implements the pure virtual method of the <i>CFrameComponent</i> base class. This method contains all the instruction required to
	 * draw the component.
	*/
	WORD draw();
	/**
	 * This a string sequence used to stores the bits description from the ACS CDB.
	*/
	ACS::stringSeq_var m_bitDescriptions;
	/**
	 * This is a string sequence that stores the led aspect when the corresponding bit is set
	 */
	ACS::ConditionSeq_var m_whenSet;
	/**
	 * This is a string sequence that stores the led aspect when the corresponding bit is cleared
	 */
	ACS::ConditionSeq_var m_whenCleared;
	/**
	 * Number of bits that the component is controlling
	 */
	WORD m_bitsNumber;
	/**
	 * Stores the maximum number of character of each of the bits label
	 */
	WORD m_maxLen;
	/**
	 * true if the component has to draw the description labels next to the leds. Deafult is true.
	 */
	bool m_drawLabels;
	/**
	 * Used internally to check that the cdb information has been collected succesfully
	 */
	bool m_cdbOk;
};

#include "TW_PropertyBitField.i"

}


#endif
