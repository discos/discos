// $Id: LedDisplay.cpp,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

#include "TW_LedDisplay.h"
#include <math.h>

using namespace TW;

CLedDisplay::CLedDisplay(const WORD& num) : CFrameComponent(), m_ledNumber(num)
{
	m_styleOn=new CStyle[m_ledNumber];
	m_styleOff=new CStyle[m_ledNumber];
	m_charactersOn=new char[m_ledNumber];
	m_charactersOff=new char[m_ledNumber];
	for (int i=0;i<m_ledNumber;i++) {
		m_charactersOn[i]='@';
		m_charactersOff[i]='o';
		m_styleOff[i].setColorPair(CColorPair::GREEN_BLACK);
		m_styleOn[i].setColorPair(CColorPair::RED_BLACK);
	}
	CFrameComponent::setValue("0");
	CFrameComponent::setWidth(m_ledNumber);
	CFrameComponent::setHeight(1);
	CFrameComponent::setHAlign(CFrameComponent::MIDDLE);
	CFrameComponent::setWAlign(CFrameComponent::CENTER);
	m_orientation=VERTICAL;
	m_interleave=0;
}

CLedDisplay::~CLedDisplay()
{
	if (m_styleOff!=NULL) delete[] m_styleOff;
	if (m_styleOn!=NULL) delete[] m_styleOn;
	if (m_charactersOn!=NULL) delete[] m_charactersOn;
	if (m_charactersOff!=NULL) delete[] m_charactersOff;		
}

void CLedDisplay::setLedStyle(const WORD& pos,const CStyle& styleOn,const CStyle& styleOff,const char& chOn,const char& chOff)
{
	if (pos<m_ledNumber) {
		m_charactersOn[pos]=chOn;
		m_charactersOff[pos]=chOff;
		m_styleOff[pos]=styleOff;
		m_styleOn[pos]=styleOn;
	}
}

void CLedDisplay::setOrientation(const TOrientation& h)
{
	m_orientation=h;
	if (m_orientation==VERTICAL) {
		setWidth(m_ledNumber);
		setHeight(1);
	}
	else {
		setWidth(1);
		setHeight(m_ledNumber);	
	}
}

WORD CLedDisplay::draw()
{
	WORD dimension;
	CPoint newPosition;
	IRA::CString tmp;
	unsigned long long int value;
	unsigned long long int mask;
	bool isOn;
	char *endPtr;
	//const char *nPtr;
	if (!CFrameComponent::getMainFrame()) return 4;
	if (!CFrameComponent::getEnabled()) return 1;
	if (!CFrameComponent::getMainFrame()->insideBorders(getPosition())) return 2;
	value=strtoull((const char*)getValue(),&endPtr,10);
	if ((value==0) && (endPtr==(const char*)getValue())) {
		return 5;
	}
	if (m_orientation==VERTICAL) dimension=CFrameComponent::getEffectiveWidth();
	else dimension=CFrameComponent::getEffectiveHeight();
	for (WORD i=0;i<dimension;i++) {
		if (m_orientation==VERTICAL) newPosition=CPoint(i*(m_interleave+1),0);
		else newPosition=CPoint(0,i*(m_interleave+1));
		mask=(unsigned long long int)pow(2,i);
		isOn=(value&mask)==mask;
		if (isOn) {
			if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+getPosition(),m_styleOn[i],m_charactersOn[i])) return 3;
		}
		else {
			if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+getPosition(),m_styleOff[i],m_charactersOff[i])) return 3;
		}
	}
	return 0;
}
