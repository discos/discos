/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_Tools.h,v 1.7 2010-09-13 10:30:50 a.orlati Exp $																							                             */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/01/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 13/04/2006      CColor and CColorPair clases revisited            */

#ifndef _TW_TOOLS_H
#define _TW_TOOLS_H

#include <Definitions.h>
#include  <ncurses.h>


/**
 * @mainpage TextWindow Library API Documentation
 * @date 12/09/2010 
 * @version 1.3.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 7.0.2
 * @remarks gcc version is 4.1.2
 *
 * This library is a collection of classes that are assembled together in order to 
 * provide a basic and general tools for ACS client developers. This library allows to write
 * client using the text mode in the CURSES fashion. At present it permits to develop one modal 
 * dialog frame <i>CFrame</i> in which the programmer can deploy some components(widgets). Optionally the
 * widgets can be associated to the ACS property. The shape of the dialog can be changed and customized by tuning 
 * some parameters.
 * In order to use this library you have to include all the required headers and link against the ncurses library, switch -lncurses
*/
namespace TW {

class CFrame;
class CColorPair;

/** 
 * This class represents the color definitions. The user can define his own colors or use the one provided by default. Every color is distinguished by its
 * identifier. @sa the definition of a new color can be done using the <i>CFrame::defineColor()</i> method. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CColor {
	friend class CFrame;
	friend class CColorPair;
public:
	/**
	 * List of all default colors (no need to be defined).
	*/	
	enum DefaultColor {
		BLACK=-1,	   /*!< color black   */
		RED=-2,		   /*!< color red     */
		GREEN=-3,	   /*!< color green   */
		YELLOW=-4,   	/*!< color yellow  */
		BLUE=-5,	   	/*!< color blue    */
		MAGENTA=-6, 	/*!< color magenta */
		CYAN=-7,	      /*!< color cyan    */
		WHITE=-8	      /*!< color white   */
	};
	/**
	 * Constructor. It creates a color given its identifier.
	 * @param color the color identifier. If this value is not included in the range 0 >= X < <i>CFrame::getUserColorNumber()</i>, the BLACK color
	 *        will be assigned as defautl value.
	*/
	CColor(const short& color);
	/**
	 * Copy constructor.
	*/
	CColor(const CColor& color);
	/**
	 * Destructor.
	*/
	~CColor();
	/**
	 * Copy operator.
	*/
	const CColor& operator =(const CColor& src);
	/** 
	 * @return the current color identifier.
	*/
	inline short getColor() const { return m_userColor; }
protected:
	/**
	 * Cast operator.
	*/
	operator short() const;
private:
	static const BYTE DEFAULTCOLORNUMBER;
	static const BYTE DEFINABLECOLORS;
	short m_userColor;
	short m_cursesColor;
};

/** 
 * This class represents the color pair used during drawing operations. The user can define his own pair and/or use the ones provided by default. 
 * Every pair is distinguished by its identifier. @sa the definition of a new color can be done using the <i>CFrame::defineColorPair()</i> method.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CColorPair {
friend class CFrame;
public:
	/**
	 * List of all default colors pair (no need to be defined: the CFrame class will take charge of doing it). 
	*/		
	enum DefaultPair {
		WHITE_BLACK=-1,
		RED_BLACK=-2,
		GREEN_BLACK=-3,
		BLUE_BLACK=-4,
		YELLOW_BLACK=-5,
		MAGENTA_BLACK=-6,
		CYAN_BLACK=-7,
		WHITE_GREEN=-8,
		WHITE_BLUE=-9,
		WHITE_RED=-10,
		WHITE_YELLOW=-11,
		WHITE_CYAN=-12,
		WHITE_MAGENTA=-13,
	};
	/**
	 * Constructor. It creates a color pair given its identifier.
	 * @param pair the pair identifier. If this value is not included in the range  0 >= X < <i>CFrame::getUserColorPairNumber()</i>, BLACK_WHITE pair
	 * 		  will be assigned as defautl value.
	*/	
	CColorPair(const short& pair);
	/**
	 * Copy constructor.
	*/
	CColorPair(const CColorPair& pair);
	/**
	 * Destructor.
	*/
	~CColorPair();
	/**
	 * Copy operator.
	*/
	const CColorPair& operator =(const CColorPair& src);
	/** 
	 * @return the current color pair identifier.
	*/
	inline short getColorPair() const { return m_userPair; }
protected:
	/**
	 * Cast operator.
	*/
	operator short() const;	
private:
	static const BYTE DEFAULTPAIRNUMBER;
	static const BYTE DEFINABLEPAIRS;
	short m_userPair;
	short m_cursesPair;
};

/** 
 * This class is used to define the positions of the frames and widgets inside the screen. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CPoint {
public:
	friend class CFrame;
	/**
	 * Constructor.
	*/		
	CPoint();
	
	/**
	 * Constructor.
	 * @param x X coordinate of the point.
	 * @param y Y coordinate of the point.	
	*/	
	CPoint(const int& x,const int& y);
	
	/**
	 * Constructor.
	 * Copy constructor 
	 * @param src CPoint object to copy
	*/	
	CPoint(const CPoint& src);
	/**
	 * Destructor.
	*/
   virtual ~CPoint();
	
	/**
	 * It can be used to know the X coordinate.
	 * @return the X coordinate of the CPoint object.
	*/
	WORD getX() const;

	/**
	 * It can be used to know the X coordinate.
	 * @return the X coordinate of the CPoint object.
	*/
	WORD getY() const;
	
	/**
	 * Copy operator.
	*/
	const CPoint& operator =(const CPoint& src);
	
	/**
	 * Sum and assign operator.
	*/
	const CPoint& operator +=(const CPoint & src);
	
	/**
	 * Subtract and assign operator.
	*/
	const CPoint& operator -=(const CPoint & src);
	
	/**
	 * Sum operator.
	*/	
	friend CPoint operator +(const CPoint& s1,const CPoint& s2);
	
	/**
	 * Subtract operator.
	*/
	friend CPoint operator -(const CPoint& s1,const CPoint& s2);
private: 
	/** X coordinate */
	int m_x;
	/** Y coordinate */
	int m_y;
};

/** 
 * This class describes how each single character will be drawn. It is possible to define the color pair (foreground and background) and
 * the attributes. For example if the character will be bolded or underlined. Each single attribute can be toggled on or off.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CStyle {
public:
	friend class CFrame;
	/**
	 * List of all possible attributes. 
	*/
	enum Attributes {
		NORMAL=A_NORMAL,		   //Normal display (no highlight)
    	STANDOUT=A_STANDOUT,       //Best highlighting mode of the terminal
    	UNDERLINE=A_UNDERLINE,     //Underlining
    	REVERSE=A_REVERSE,         //Reverse video
    	BLINK=A_BLINK,             //Blinking
    	DIM=A_DIM,                 //Half bright
    	BOLD=A_BOLD,               //Extra bright or bold
    	PROTECT=A_PROTECT,         //Protected mode
    	INVIS=A_INVIS,             //Invisible or blank mode
    	ALTCHARSET=A_ALTCHARSET,   //Alternate character set
    	CHARTEXT=A_CHARTEXT        //Bit-mask to extract a character
	};
	
	/**
	 * Constructor.
	*/
	CStyle();
	
	/**
	 * Constructor.
	 * @param pair define the color pair of the current style.
	 * @param attrib define the attributes of the style, this field is bitwise-or’d of zero or more of the <i>Attributes</i> flags.
	*/
	CStyle(const CColorPair& pair,const int& attrib);

	/**
	 * Constructor.
    * Copy Constructor.
    * @param style CStyle object to be copied 
	*/	
	CStyle(const CStyle& style);
	
	/**
	 * Destructor.
	*/
	virtual ~CStyle();
	
	/**
	 * @return the color pair of the current style
	*/
	inline CColorPair getColorPair() const { return m_colorPair; }
	
	/**
    * Sets a new color pair.
	*/
	inline void setColorPair(const CColorPair& pair) { m_colorPair=pair; }

	/**
	 * @return the bitwise-or'd attributes.
	*/	
	inline int getAttrubutes() const { return m_textAttributes; }
	
	/**
    * Sets the new attributes set.
	*/
	inline void setAttributes(const int& attrib) { m_textAttributes=attrib; }
	
	/**
	 * Toggle an attribute on.
	 * @param attrib attribute to toggle on.
	*/
	void addAttribute(Attributes attrib);
	
	/**
	 * Toggle an attribute off.
	 * @param attrib attribute to toggle off.
	*/
	void removeAttribute(Attributes attrib);
	
	/**
	 * Copy operator.
	*/
	const CStyle& operator =(const CStyle& src);
private:
	/** Current color pair. */
	CColorPair m_colorPair;

	/** Current attributes (bitwise-or'd field) */
	int m_textAttributes;
};
	
}

#endif
