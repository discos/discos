/*****************************************************************************************************  */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_Label.h,v 1.1.1.1 2007-05-04 09:53:28 a.orlati Exp $																							        */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 11/04/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 24/10/2006      Changed to protected inhritance                   */


#ifndef _TW_LABEL_H
#define _TW_LABEL_H

#include "TW_FrameComponent.h"

namespace TW {
	
/**
 * This macro is a shortcut to create an add a CLabel to the frame.
 * @param CAPTION labal caption
 * @param X horizontal coordinate of the first character of the label caption
 * @param Y vertical coordinate of the first character of the label caption	
 * @param W width of the label
 * @param H height of the label
 * @param COLORPAIR Color pair used to draw the label
 * @param ATTRIB text attribute used to draw the label
 * @param FRAME Frame object that will host the label
*/
#define _TW_ADD_LABEL(CAPTION,X,Y,W,H,COLORPAIR,ATTRIB,FRAME) { \
	TW::CLabel *__dummy=new TW::CLabel(CAPTION); \
	__dummy->setWidth(W); \
	__dummy->setHeight(H); \
	__dummy->setPosition(TW::CPoint(X,Y)); \
	__dummy->setStyle(TW::CStyle(COLORPAIR,ATTRIB)); \
	FRAME.addComponent((CFrameComponent*)__dummy); \
}

/**
 * This class implements a component as a label. A label is a static text that can be placed inside the frame.
 * Style, position, alignment can be selected.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 	
*/	
class CLabel : protected virtual CFrameComponent
{
public:
	/**
 	 * Constructor. 
	 * @param caption this is the text of the label. It can also be changed by the <i>setValue()</i> method.
	*/
	CLabel(IRA::CString caption);
	/**
     * Destructor.
	*/
	virtual ~CLabel();

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
	using CFrameComponent::setValue;
	using CFrameComponent::getValue;
	using CFrameComponent::Refresh;
	using CFrameComponent::setOutputComponent;
	using CFrameComponent::getOutputComponent;	
	
protected:
	/**
	 * This method implements the pure virtual method of the <i>CFrameComponent</i> base class. This method contains all the instruction required to
	 * draw the component.
	*/
	WORD draw();
};

}

#endif
