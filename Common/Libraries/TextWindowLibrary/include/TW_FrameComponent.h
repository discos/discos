/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_FrameComponent.h,v 1.2 2010-12-28 19:09:26 a.orlati Exp $																							                             */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/01/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 23/03/2007      Ported to ACS 6.0.2 in particular with respect to */ 
/*                                                    the new TAO Version.                               */      

#ifndef _TW_FRAMECOMPONENT_H
#define _TW_FRAMECOMPONENT_H


//#include "TW_Tools.h"
#include "TW_Frame.h"
#include <String.h>

namespace TW {
	
/**
 * This macro is a shortcut to sets some common property for a frame component object.
 * @param COMP pointer to frame component object
 * @param X horizontal coordinate of the first character of the label caption
 * @param Y vertical coordinate of the first character of the label caption	
 * @param W width of the label
 * @param H height of the label
 * @param COLORPAIR color pair used to draw the label
 * @param ATTRIB text attribute used to draw the label
 * @param OUTPUTCOMP the output component 
*/
#define _TW_SET_COMPONENT(COMP,X,Y,W,H,COLORPAIR,ATTRIB,OUTPUTCOMP) { \
	COMP->setPosition(TW::CPoint(X,Y)); \
	COMP->setWidth(W); \
	COMP->setHeight(H); \
	COMP->setStyle(TW::CStyle(COLORPAIR,ATTRIB)); \
	COMP->setOutputComponent((CFrameComponent *)OUTPUTCOMP); \
}

/**
 * This code is the basic interface that each control/component/widget must implement in order to be deployed in a frame.
 * It is very important to remember that each component must be linked to a frame(<i>CFrame</i>) before any other operation can take place.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CFrameComponent {
public:
	friend class CFrame;	
	/**
	 * <a name="Enum1"> List of all possible horizontal alignments </a>
	*/	
	enum TAlignW {
		CENTER,		 /*!< alignment in the center */
		LEFT,		    /*!< left alignment */
		RIGHT,		 /*!< right alignment */
	};
	
	/**
	 * <a name="Enum2"> List of all possible vertical alignments </a>
	*/		
	enum TAlignH {
		MIDDLE, 	/*!< alignment in the middle */
		DOWN,		/*!< down alignment */
		UP			/*!< up alignment */
	};
	
	/**
	 * Default Constructor.
	*/
	CFrameComponent();
	
	/**
	 * Destructor.
	*/
	virtual ~CFrameComponent();
	
	/**
	 * Sets the position property.
	 * @param pos <i>CPoint</i> class indicating the upper left spot of the component
	*/
	inline void setPosition(const CPoint& pos) { m_position=pos; }
	
	/**
	 * Gets the position of the component.
	 * @return the <i>CPoint</i> class indicating the upper left spot of the component
	*/
	inline const CPoint& getPosition() const { return m_position; }
	
	/**
	 * Sets the tag property. This value does not affect anything, it could be used freely.
	 * @param tag the new tag value.
	*/	
	inline void setTag(const DWORD& tag) { m_tag=tag; }
	
	/**
	 * Gets the tag property. 
	 * @return the present tag value.
	*/		
	inline const DWORD& getTag() const { return m_tag; }
	
	/**
	 * Sets the enabled property. If this property is false the component is not updated anymore.
	 * @param enab true to enable the component
	*/	
	inline void setEnabled(const bool& enab) { m_enabled=enab; }
	
	/**
	 * Gets the enabled property. 
	 * @return the present value of the enabled property
	*/		
	inline bool getEnabled() const { return m_enabled; }
	
	/**
	 * Sets the value property. This value represents the string that will be displayed by the component.
	 * @param val the new value.
	*/
	virtual void setValue(const IRA::CString& val);
	
	/**
	 * Gets the value property. 
	 * @return the present value property
	*/		
	inline const IRA::CString& getValue() const { return m_value; }
	
	/**
	 * Sets the style property. This will change the aspect of the component.
	 * @param style the new style.
	*/
	void setStyle(const CStyle &style) { m_style=style; }
	
	/**
	 * Gets the style property.
	 * @return the present style configuration.
	*/
	inline const CStyle& getStyle() const { return m_style; }

	/**
	 * Sets the width property. It represents the number of columns that the component occupies.
	 * @param w the new width of the component.
	*/
	void setWidth(const WORD& w);
	
	/**
	 * Gets the width property. 
	 * @return the present value of the width property
	*/		
	inline const WORD& getWidth() const { return m_width; }

	/**
	 * Sets the height property. It represents the number of rows that the component occupies.
	 * @param h the new height of the component.
	*/	
	void setHeight(const WORD& h);
	
	/**
	 * Gets the height property. 
	 * @return the present value of the height property
	*/		
	inline const WORD& getHeight() const { return m_height; }	

	/**
	 * Sets the HAlign property. It represents horizontal alignment for the component.
	 * @param h the new horizontal alignment, <a href="#Enum2"> see enum TAlignH. </a>
	*/
	void setHAlign(const TAlignH& h);
	
	/**
	 * Gets the HAlign property. 
	 * @return the present value of the HAlign property
	*/		
	inline const TAlignH& getHAlign() const { return m_alignHeight; }	

	/**
	 * Sets the WAlign property. It represents vertical alignment for the component.
	 * @param w the new vertical alignment, <a href="#Enum1"> see enum TAlignW. </a>
	*/	
	void setWAlign(const TAlignW& w);
	
	/**
	 * Gets the WAlign property. 
	 * @return the present value of the WAlign property
	*/		
	inline const TAlignW& getWAlign() const { return m_alignWidth; }
	
	/**
	 * Sets the outputComponent property. If sets (not NULL) this property links this component with another one
	 * so that errors or other messages generated by this component will be displayed by the associated component.
	 * @param comp the associated component.
	*/
	void setOutputComponent(CFrameComponent* comp=NULL) { m_outputComponent=comp; }
	
	/**	
	 * Gets the current outputComponent property. 
	 * @return the pointer to the associated property.
	*/
	inline CFrameComponent* getOutputComponent() const { return m_outputComponent; }
	
	/**
	 * Call this method to force a refresh of the component every time one property has changed. This method should be explicitally called
	 * if you want that a change is displayed.
	*/
	void Refresh();
	
protected:
	/** 
	 * This method must be implemented by all classes that inherites from this class in order to be instantiated. 
	 * This method must provide all drawing operations for the component. The returned integer, usally, has the following meaning:
	 * @arg \c 0 succesful operations.
	 * @arg \c 1 component is not enabled.
	 * @arg \c 2 the component is outside the frame borders.
	 * @arg \c 3 the parent frame is not visible or the drawing operation cannot suceed.
	 * @arg \c 4 the component does not have a parent frame.
	 * @arg \c 5 other 
	 * @return an integer, the value could vary on the particular implementation and it depends on the drawing operations results. 
	*/
	virtual WORD draw()=0;
	
	/**
	 * This method computes the effective height and width. If the component does not fit 
	 * perfectly inside the frame, the effective dimensions could be shorter than expected.
	 * @return the effective(usable) height of the component
	*/
	WORD getEffectiveHeight() const;

	/**
	 * This method returns the effective width of the component. If the component does not fit 
	 * perfectly inside the frame, the effective dimensions could be shorter than expected.
	 * @return the effective(usable) width of the component
	*/
	WORD getEffectiveWidth() const;
	
	/** 
	 * This methods is called by the main frame to activate the component. 
	 * When a component is active it can perform all its operation.
	 * Every components that inherits from <i>CFrameComponent</i> and overrides this method must call the base class function.	
	*/
	virtual void Activate();
	
	/** 
	 * This method is called by the main frame to de-activate the component. 
	 * When a component is not active it must be ready to chease all of its operation.
	 * Every components that inherits from <i>CFrameComponent</i> and overrides this method must call the base class function.	 
	*/	
	virtual void deActivate();
	
	/**
	 * This method can be used to determine if the component is alive. That means the component is usable, that means the component has been 
	 * activated and has been assigned to a parent window(frame).
	 * @return true is the object is ready.
	*/
	bool isAlive();
	
	/** 
	 * This method returns the reference to the main frame window of the component.
	 * @return a pointer to the parent window. A null means the main frame has not been set.
	*/
	CFrame *getMainFrame();
	
	/**
	 * This method is used to compute the Y offset (with respect to the upper left corner of the component) to be applied to obtain 
    * the right alignment.
	 * @param align the alignment that should be obtained
	 * @param rows number of rows that the text is expected to occupy
	 * @param height the effective (usable) height of the component
	 * @return the offeset to be applied to Y coordinate
	*/	
	static WORD getHeightAlignment(const TAlignH& align,const WORD& rows,const WORD& height);
	
	/**
	 * This method is used to compute the X offset (with respect to the upper left corner of the component) to be applied to obtain 
    * the right alignment.
	 * @param align the alignment that should be obtained
	 * @param cols number of rows that the text is expected to occupy
	 * @param width the effective (usable) width of the component
	 * @return the offeset to be applied to X coordinate	 
	*/		
	static WORD getWidthAlignment(const TAlignW& align,const WORD& cols,const WORD& width);
	
	/** 
	 * This method is used for debugging purposes. It allows to output a message in the current outputComponent if is already set.
	 * @param msg message to be output
	*/
	void outputMessage(const IRA::CString& msg);
	
private:
	/** Upper left spot of the component */
	CPoint m_position;
	/** Tag */
	DWORD m_tag;
	/** Component enabled or not */
	bool m_enabled;
	/** Display value */
	IRA::CString m_value;
	/** The Style */
	CStyle m_style;
	/** The width of the component */
	WORD m_width;
	/** The height of the component */
	WORD m_height;
	/** Horizontal alignment */
	TAlignH m_alignHeight;
	/** Vertical alignment */
	TAlignW m_alignWidth;
	/**	This component can be used to output extra messages or errors */
	CFrameComponent* m_outputComponent;
	/** Indicates wether the component has been activated or not */
	bool m_activated;
	/** Reference to the frame container */
	CFrame* m_frame;
};

}

#endif
