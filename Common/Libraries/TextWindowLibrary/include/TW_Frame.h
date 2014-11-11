/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_Frame.h,v 1.1.1.1 2007-05-04 09:53:28 a.orlati Exp $																							    */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 02/01/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 23/03/2007      Ported to ACS 6.0.2 in particular with respect to */ 
/*                                                    the new TAO Version.                               */      

#ifndef _TW_FRAME_H
#define _TW_FRAME_H

#include <acsThread.h>
#include <ncurses.h>
#include <Definitions.h>
#include <String.h>
#include "TW_Tools.h"
#include <vector>

namespace TW {
	
class CFrameComponent;
	
/**
 * This code implements the frame/panel/container of all objects/components. In order to open a new frame, normal call sequence is:
 * @arg \c 1 Crete the CFrame object.
 * @arg \c 2 Call method <i>initFrame()</i>
 * @arg \c 3 Call method <i>addComponent()</i> all the times it is needed
 * @arg \c 3 Call method <i>showFrame()</i>
 * @arg \c 4 Call method <i>writeCanvas()</i>
 * Every component added to the frame will be erased when the frame closes, so you don't have to call the <i>CFrameComponent</i> destructor directly.
 * if you chhos to use automatic objects (stack) for your component, please make sure that they will be deleted before their frame.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CFrame {
public:
	/**
  	 * Constructor. Initialize the frame.
	 * @param ul indicates the upper left corner of the frame
 	 * @param br indicates the bottom right corner of the frame
	 * @param ls character used to draw the left side of the frame
	 * @param rs character used to draw the right side of the frame
	 * @param us character used to draw the upper side of the frame
	 * @param bs character used to draw the bottom side of the frame
	*/
	CFrame(const CPoint& ul,const CPoint& br,char ls,char rs,char us,char bs);

	/**
	 * Destructor. Delete the frame and all the components added to the frame.
	*/
	~CFrame();

	/**
	 * This method must be called before any other function. It will initilize the screen and prepare the frame.
	*/
	void initFrame();

	/** 
	 * This method shows the frame and starts the drawing operation.
	*/
	void showFrame();
	
	/** 
	 * This method forces the update of all visual items inside the frame.
	*/
	void Refresh();

	/**
	 * Call this method to close the frame and to stop all drawing operation over this frame.
	*/
	void closeFrame();
	
	/**
	 * This is the destructor proxy. 
	 * Call this method to free al the resources allocated to the window.
	*/ 
	void Destroy();
	
	/**
	 * Call this function if you want to know the real width of the frame
	 * @return the number of columns in the frame
	*/
	inline WORD getWidth() const { return m_width; }
	
	/**
	 * Call this function if you want to know the real height of the frame
	 * @return the number of rows in the frame
	*/
	inline WORD getHeight() const { return m_height; }
	
	/**
	 * Sets the frame title. Call <i>Refresh()</i> if you want to force an update of the title field.
	 * @param title the new frame title.
	*/
	void setTitle(const IRA::CString& title) { m_title=title; }
	
	/**
	 * Sets the frame title style. Call <i>Refresh()</i> in order to display the title using the new style fashion.
	 * @param style the new frame title style.
	*/
	void setTitleStyle(const CStyle& style) { m_titleStyle=style; }
	
	/**
	 * @return the frame title
	*/ 
	inline IRA::CString getTitle() const { return m_title; }
	
	/**
	 * @return the frame title style
	*/ 	
	inline CStyle getTitleStyle() const { return m_titleStyle; }
	
	/**
	 * Call this function if you want to know the usable width of the frame
	 * @return the number of usable columns in the frame
	*/	
	inline WORD getUsableWidth() const { return m_width-(m_borderW*2); }
	
	/**
	 * Call this function if you want to know the usable height of the frame
	 * @return the number of usable rows in the frame
	*/
	inline WORD getUsableHeight() const { return m_height-(m_borderH*2); }
	
	/**
	 * Call this function to define a new color in the frame. Once a color is defined it can be used (inside the window and all its components), 
	 * by just referring to its identifier. New defined colors can be used in place or conjunction with the standard colors (<i>CColor::DefaultColor</i>).
	 * Stadard colors don't have to be defined. The number of user defined color pairs is 0..<i>getUserColorNumber()</i>-1. Not all terminals allows to change
	 * the color definition; the <i>colorReady()</i> member can be used to know if the terminal has this capability.
	 * @param color color that must be defined
 	 * @param red the ammount of red component (0..1000)
	 * @param green the ammount of red component (0..1000)
 	 * @param blue the ammount of red component (0..1000)	
	*/
	void defineColor(const CColor& color,short red,short green,short blue) const;

	/**
	 * This routine returns true if the terminal supports colors and can change thier definitions. 
	 * @return true if the color defintions is available.
	*/
	bool colorReady() const;
	
	/**
	 * Call this function to define a new color pair in the frame. Once a color pair is defined it can be used (inside the window and all its components), 
	 * by just referring to its identifier. New defined color pairs can be used in place or conjunction with the standard color pair (<i>CColorPair::DefaultPair</i>).
	 * Stadard colors don't have to be defined. The number of user defined color pairs is 1..<i>getUserColorPairNumber()</i>-1.
	 * @param pair color pair that must be defined
	 * @param fore foreground color
	 * @param back background color
	*/
	void defineColorPair(const CColorPair& pair,const CColor& fore,const CColor& back) const;

	/**
	 * Call this function if you want to know the maximum number of user defineable colors. The user defined color identifiers should land in the range
	 * between zero (included) and the value returned by this function (not included).
	 * @return then number of colors that can be defined by the user.
	*/
	
	short getUserColorNumber() const;
	/**
	 * Call this function if you want to know the maximum number of user defineable color pairs. The user defined pair identifiers should land in the range
	 * between zero (included) and the value returned by this function (not included).
	 * @return then number of color pairs that can be defined by the user.
	*/
	short getUserColorPairNumber() const;
	
	/**
	 * Call this function to add a componet to the frame. Every component must be added to a frame in order to be
	 * displayed in the screen. When the frame dies all the component assigned to the frame will be deactivated automatically.
	 * @param comp pointer to the frame component
	*/
	void addComponent(CFrameComponent *comp);
	
	/**
	 * Call this function to remove a component from the main frame. When a component is removed it cannot be displayed in the screen anymore (It is deactivated).
	 * @param comp pointer to the frame component, if the component was not added before, nothing happens.
	*/
	void removeComponent(CFrameComponent *comp);
	
	/**
	 * Call this method to write a text line inside the window. Generally you don't have to call this function directly, in fact
	 * it is usaully called by the <i>CFrameComponent::draw()</i> method. 
	 * if the text is truncated if it exceeds the frame limits.
	 * @param xy this indicates the row and the line where the first character of the text will be positioned(relative to the frame itself)
	 * @param style this is the style used to draw the text
	 * @param text this is the string that will be drawn
	 * @return true if the requested point for the drawing operation is inside the frame limits.
	*/ 
	bool writeCanvas(const CPoint& xy,const CStyle& style,const IRA::CString& text);
	
	/**
	 * Call this method to clear a text line inside the window. Generally you don't have to call this function directly. The text line is cleared by 
	 * overwriting the existing line with a sequence of spaces.
	 * @param xy indicates first character that will be cleared.
	 * @param cols the number of characters (included the first one) that will be cleared.
	 * @param style this is the style used to draw the text
	 * @return true if the requested point for the drawing operation is inside the frame limits.
	*/ 
	bool clearCanvas(const CPoint& xy,const WORD& cols, const CStyle& style);
	
	/**
	 * Call this method to read the next character typed by the user. Generally this function should not be called directly. 
	 * @param ch this parameter is used as output parameter to store the last typed character.
	 * @return false if no further character is pending.
	*/
	bool readCanvas(char &ch);
	
	/** 
	 * This method is used to check if a spot is inside the frame borders.
	 * @param p point to be checked (the coordinates are relative to the left upper corner of the frame)
	 * @return true if the spot is inside the borders, false otherwise
	*/
	bool insideBorders(const CPoint& p) const;
private:
	/** Upper left corner of the frame */
	CPoint m_upperLeft;
	/** Bottom right corner of the frame */
   CPoint m_bottomRight;
	char m_ls,m_rs,m_us,m_bs;
	/** Pointer to the NCURSES window */
	WINDOW* m_win;
	/** Frame height */
	WORD m_height;
	/** Frame width */
	WORD m_width;
	/** The width of the frame border */
	WORD m_borderW;
	/** The height of the frame border */
	WORD m_borderH;
	/** Frame title */
	IRA::CString m_title;
	/** Frame title style */
	CStyle m_titleStyle;
	/** This flag indicates if the frame is visible or not */
	bool m_show;
	/** List of components deployed inside the frame */
	std::list <DWORD> m_components;
	/** Frame mutex */
	ACE_Recursive_Thread_Mutex m_mutex;
	/** This flag is true when the frame is destroyed or is destroying, no drawings are then permitted */
	bool m_destroyed;
	/** This methods writes a sequence of character in the frame canvas */
	void Write(const CPoint& xy,const CStyle& style,const IRA::CString& text);
};
	
}


#endif
