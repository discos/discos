/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_StatusBox.h,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $																							                             */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/11/2006      Creation                                          */


#ifndef _TW_STATUSBOX_H
#define _TW_STATUSBOX_H


#include "TW_FrameComponent.h"
#include <map>

namespace TW {

		
#define _TW_STATUSBOX_C class EN
#define _TW_STATUSBOX_TL EN

/**
 * This template class inplements a status box. The box can have a fixed number of values or status. For each value a different drawing style and caption/description can be defined. 
 *	This template has one type:
 * @arg \c EN enumeration type that enlists all the possible values that the box can assume.
*/
template <_TW_STATUSBOX_C> 	
class CStatusBox : protected virtual CFrameComponent
{
public:
	/**
	 * Constructor.
	 * @param init initial status value
	*/
	CStatusBox(const EN& init);
	/**
    * Destructor.
	*/
	virtual ~CStatusBox();
	
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
};

#include "TW_StatusBox.i"

}

#endif
