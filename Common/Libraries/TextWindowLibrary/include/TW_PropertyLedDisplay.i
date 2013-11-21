// $Id: TW_PropertyLedDisplay.i,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

#include <math.h>

template <_TW_PROPERTYCOMPONENT_C>
CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_TL>::CPropertyLedDisplay(PR property,const WORD& num) : CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>(property), m_ledNumber(num)
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
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValue("0");
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setWidth(m_ledNumber);
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setHeight(1);
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setHAlign(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::MIDDLE);
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setWAlign(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::CENTER);
	m_orientation=VERTICAL;
}

template <_TW_PROPERTYCOMPONENT_C>
CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_TL>::~CPropertyLedDisplay()
{
	if (m_styleOff!=NULL) delete[] m_styleOff;
	if (m_styleOn!=NULL) delete[] m_styleOn;
	if (m_charactersOn!=NULL) delete[] m_charactersOn;
	if (m_charactersOff!=NULL) delete[] m_charactersOff;		
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_TL>::setLedStyle(const WORD& pos,const CStyle& styleOn,const CStyle& styleOff,const char& chOn,const char& chOff)
{
	if (pos<m_ledNumber) {
		m_charactersOn[pos]=chOn;
		m_charactersOff[pos]=chOff;
		m_styleOff[pos]=styleOff;
		m_styleOn[pos]=styleOn;
	}
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_TL>::setOrientation(const TOrientation& h)
{
	m_orientation=h;
	if (m_orientation==VERTICAL) {
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setWidth(m_ledNumber);
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setHeight(1);
	}
	else {
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setWidth(1);
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setHeight(m_ledNumber);	
	}
}

template <_TW_PROPERTYCOMPONENT_C>
WORD CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_TL>::draw()
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
	if (!CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getEnabled()) return 1;
	if (!CFrameComponent::getMainFrame()->insideBorders(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition())) return 2;
	value=strtoull((const char*)CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getValue(),&endPtr,10);
	if ((value==0) && (endPtr==(const char*)CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getValue())) {
		return 5;
	}
	if (m_orientation==VERTICAL) dimension=CFrameComponent::getEffectiveWidth();
	else dimension=CFrameComponent::getEffectiveHeight();
	for (WORD i=0;i<dimension;i++) {
		if (m_orientation==VERTICAL) newPosition=CPoint(i,0);
		else newPosition=CPoint(0,i);
		mask=(unsigned long long int)pow(2,i);
		isOn=(value&mask)==mask;
		if (isOn) {
			if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition(),m_styleOn[i],m_charactersOn[i])) return 3;
		}
		else {
			if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition(),m_styleOff[i],m_charactersOff[i])) return 3;
		}
	}
	return 0;
}
