// $Id: Tools.cpp,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

#include "TW_Tools.h"

using namespace TW;

/************** CColor ************************/

const BYTE CColor::DEFAULTCOLORNUMBER=8;
const BYTE CColor::DEFINABLECOLORS=COLORS-DEFAULTCOLORNUMBER;


CColor::CColor(const short& color) : m_userColor(color)
{
	switch (color) {
		case BLACK : {
			m_cursesColor=COLOR_BLACK;
			break;
		}
		case RED: {
			m_cursesColor=COLOR_RED;
			break;
		}
		case GREEN: {
			m_cursesColor=COLOR_GREEN;
			break;
		}
		case YELLOW: {
			m_cursesColor=COLOR_YELLOW;
			break;
		}
		case BLUE: {
			m_cursesColor=COLOR_BLUE;
			break;
		}
		case MAGENTA: {
			m_cursesColor=COLOR_MAGENTA;
			break;
		}
		case CYAN: {
			m_cursesColor=COLOR_CYAN;
			break;
		}
		case WHITE: {
			m_cursesColor=COLOR_WHITE;
			break;
		}
		default : {
			if ((m_userColor<0) || (m_userColor>=DEFINABLECOLORS)) {
				m_userColor=BLACK;
				m_cursesColor=COLOR_BLACK;
			}
			else {
				m_cursesColor=m_userColor+DEFAULTCOLORNUMBER;
			}
			break;
		}	
	}	
}

CColor::CColor(const CColor& color)
{
	m_userColor=color.m_userColor;
	m_cursesColor=color.m_cursesColor;
}

CColor::~CColor()
{
}

CColor::operator short() const
{
	return m_cursesColor;
}

const CColor& CColor::operator =(const CColor& src)
{
	m_userColor=src.m_userColor;
	m_cursesColor=src.m_cursesColor;
	return *(this);
}

/*************** CColorPair *************************/

const BYTE CColorPair::DEFINABLEPAIRS=COLOR_PAIRS-1-DEFAULTPAIRNUMBER;
const BYTE CColorPair::DEFAULTPAIRNUMBER=13;

CColorPair::CColorPair(const short& pair) : m_userPair(pair)
{
	if ((m_userPair<=-1) && (m_userPair>=-DEFAULTPAIRNUMBER)) {
		m_cursesPair=-m_userPair;		
	}
	else {
		if ((m_userPair<0) || (m_userPair>=DEFINABLEPAIRS)) {
			m_userPair=WHITE_BLACK;
			m_cursesPair=-m_userPair;
		}
		else {
			m_cursesPair=m_userPair+DEFAULTPAIRNUMBER+1;
		}
	}
}

CColorPair::CColorPair(const CColorPair& pair)
{
	m_userPair=pair.m_userPair;
	m_cursesPair=pair.m_cursesPair;
}

CColorPair::~CColorPair()
{
}

CColorPair::operator short() const
{
	return m_cursesPair;
}

const CColorPair& CColorPair::operator =(const CColorPair& src)
{
	m_userPair=src.m_userPair;
	m_cursesPair=src.m_cursesPair;
	return *(this);
}

/************* CPOINT *************************/

CPoint::CPoint() : m_x(0), m_y(0)
{
}

CPoint::CPoint(const int& x,const int& y) : m_x(x), m_y(y)
{
}

CPoint::CPoint(const CPoint& src) {
	m_x=src.m_x;
	m_y=src.m_y;
}

CPoint::~CPoint()
{
}

WORD CPoint::getX() const 
{
	return m_x;
}

WORD CPoint::getY() const
{
	return m_y;
}

const CPoint& CPoint::operator =(const CPoint& src)
{
	m_x=src.m_x;
	m_y=src.m_y;
	return *(this);
}

const CPoint& CPoint::operator +=(const CPoint & src)
{
	m_x=m_x+src.m_x;
	m_y=m_y+src.m_y;
	return *(this);
}

const CPoint& CPoint::operator -=(const CPoint & src)
{
	if (m_x>src.m_x) m_x=m_x-src.m_x;
	else m_x=src.m_x-m_x;
	if (m_y>src.m_y) m_y=m_y-src.m_y;
	else m_y=src.m_y-m_y;
	return *(this);
}

CPoint TW::operator +(const CPoint& s1,const CPoint& s2)
{
	CPoint Tmp;
	Tmp.m_x=s1.m_x+s2.m_x;
	Tmp.m_y=s1.m_y+s2.m_y;
	return Tmp;
}

CPoint TW::operator -(const CPoint& s1,const CPoint& s2)
{
	CPoint Tmp;
	if (s1.m_x>s2.m_x) Tmp.m_x=s1.m_x-s2.m_x;
	else Tmp.m_x=s2.m_x-s1.m_x;
	if (s1.m_y>s2.m_y) 	Tmp.m_y=s1.m_y-s2.m_y;
	else Tmp.m_y=s2.m_y-s1.m_y;
	return Tmp;
}

/************************** CSTYLE ****************/

CStyle::CStyle() : m_colorPair(CColorPair::WHITE_BLACK), m_textAttributes(0) 
{
}

CStyle::CStyle(const CColorPair& pair,const int& attrib): m_colorPair(pair), m_textAttributes(attrib)
{
}

CStyle::CStyle(const CStyle& style) : m_colorPair(style.m_colorPair), m_textAttributes(style.m_textAttributes)
{
}

CStyle::~CStyle()
{
}

const CStyle& CStyle::operator =(const CStyle& src)
{
	m_colorPair=src.m_colorPair;
	m_textAttributes=src.m_textAttributes;	
	return *this;
}

void CStyle::addAttribute(Attributes attrib)
{
	m_textAttributes=m_textAttributes|attrib;
}

void CStyle::removeAttribute(Attributes attrib)
{
	m_textAttributes=m_textAttributes&~attrib;	
}
