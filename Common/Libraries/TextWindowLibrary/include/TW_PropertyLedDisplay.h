/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_PropertyLedDisplay.h,v 1.1.1.1 2007-05-04 09:53:28 a.orlati Exp $																							                             */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 11/10/2006      Creation                                          */

#ifndef _TW_PROPERTYLEDDISPLAY_H
#define _TW_PROPERTYLEDDISPLAY_H

#include "TW_PropertyComponent.h"

namespace TW {

/**
 * This class implements a property component as a led display composed by 1 up to 32 leds. The leds are turned on/off writing the value (<i>setValue()</i>) property.
 * The value is always interpreted as a bit field: the first led correspond to the first bit and so on. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 	
*/	
template <_TW_PROPERTYCOMPONENT_C> 	
class CPropertyLedDisplay : protected virtual CPropertyComponent <_TW_PROPERTYCOMPONENT_TL> {
public:
	
	/**
	 * <a name="Enum1"> List of all possible orientation </a>
	*/	
	enum TOrientation {
		VERTICAL,		 /*!< The display will be oriented vertically */
		HORIZONTAL		 /*!< The display will be oriented horizontally */
	};	

	/**
	 * Constructor. 
	 * @param property pointer to the ACS property that is associated to the created CLed object.
	 * @param num indicates the number of leds that will compose the led display; default value is 1.
	*/
	CPropertyLedDisplay(PR property,const WORD& num=1);

	/**
     * Destructor.
	*/
	virtual ~CPropertyLedDisplay();
	
	/**
	 * Sets the Orientation property. It represents orientation for the component.
	 * @param h the new horizontal alignment, <a href="#Enum1"> see enum TOrientation </a>
	*/
	void setOrientation(const TOrientation& h);
	
	/**
	 * Gets the Orientation property. 
	 * @return the present value of the Orientation property
	*/		
	inline TOrientation getOrientation() const { return m_orientation; }	

	/**
    * Sets the look and feel of one single led of the display both when the led is on and when is off.
	 * @param pos identifies the position (zero based) of the led: 0..N-1
    * @param styleOn style used to draw the led when it is turned on
    * @param styleOff style used to draw the led when it is turned off
	 * @param chOn character used to draw the led when On
	 * @param chOff character used to draw the led when Off
	*/
	void setLedStyle(const WORD& pos,const CStyle& styleOn,const CStyle& styleOff,const char& chOn='@',const char& chOff='o');
	
	using CFrameComponent::setPosition;
	using CFrameComponent::getPosition;
	using CFrameComponent::setTag;
	using CFrameComponent::getTag;
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
	/** Represents the number of leds that composes the display. */
	WORD m_ledNumber;

	/** Led styles when they are on */
	CStyle* m_styleOn;

	/** Led styles when they are off */
	CStyle* m_styleOff;

	/** Characters used to draw the leds when they are lighted */
	char* m_charactersOn;

	/** Characters used to draw the leds when they are turned off */
	char* m_charactersOff;

	/** Indicated the orientation of the component: vertical or horiziontal */
	TOrientation m_orientation;

	/**
	 * This method implements the pure virtual method of the <i>CFrameComponent</i> base class. It must contain all the instruction required to
	 * draw the component.
	*/
	WORD draw();

private:
};

#include "TW_PropertyLedDisplay.i"

}	
#endif
