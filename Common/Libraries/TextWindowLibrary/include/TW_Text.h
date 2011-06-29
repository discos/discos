/*****************************************************************************************************  */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_Text.h,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $																							        */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/11/2006      Creation                                          */


#ifndef _TW_TEXT_H
#define _TW_TEXT_H

#include "TW_FrameComponent.h"

namespace TW {

/**
 * This class implements a text field. A text field is a read-only component that allows to display
 * any value included strings and numbers. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 	
*/	
class CText : protected virtual CFrameComponent {
public:
	/**
	 * Constructor. 
	*/
	CText();
	/**
     * Destructor.
	*/
	virtual ~CText();

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
	
protected:
	/**
	 * This method implements the pure virtual method of the <i>CFrameComponent</i> base class. This method contains all the instruction required to
	 * draw the component.
	*/
	WORD draw();
};

}


#endif
