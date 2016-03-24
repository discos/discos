// $Id: TW_PropertyBitField.i,v 1.3 2010-12-28 19:09:26 a.orlati Exp $

template <_TW_PROPERTYCOMPONENT_C>
CPropertyBitField<_TW_PROPERTYCOMPONENT_TL>::CPropertyBitField(PR property) : CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>(property)
{
	int len;
	m_cdbOk=false;
	try {
		m_bitDescriptions=property->bitDescription();
		m_whenCleared=property->whenCleared();
		m_whenSet=property->whenSet();
	}
	catch (CORBA::SystemException &Ex) {
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::m_LastError.Format("Caught a CORBA System Exception, while trying to read CDB: %s:%d",Ex._name(),Ex.minor());
	}
	catch (...) {
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::m_LastError.Format("Caught an unknown exception, while trying to read CDB");
	}	
	m_bitsNumber=m_bitDescriptions->length();
	m_maxLen= strlen(m_bitDescriptions[(unsigned long)0]);
	for (unsigned long i=1;i<m_bitsNumber;i++) {
		len=strlen(m_bitDescriptions[i]);
		if (len>m_maxLen) m_maxLen=len;
	}
	showDescriptionLabels(true);
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValue("0");
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setWidth(m_maxLen+2);
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setHeight(m_bitsNumber);
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setHAlign(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::UP);
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setWAlign(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::LEFT);
	m_cdbOk=true;
}

template <_TW_PROPERTYCOMPONENT_C>
CPropertyBitField<_TW_PROPERTYCOMPONENT_TL>::~CPropertyBitField()
{
}

template <_TW_PROPERTYCOMPONENT_C>
WORD CPropertyBitField<_TW_PROPERTYCOMPONENT_TL>::draw()
{
	WORD width,height;		
	CPoint newPosition;
	WORD newX,newY;
	WORD bitPos;
	IRA::CString caption;	
	ACS::Condition shape;
	unsigned long long int value;
	char *endPtr;
	if (!m_cdbOk) return 5;
	if (!CFrameComponent::getMainFrame()) return 4;
	if (!CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getEnabled()) return 1;
	if (!CFrameComponent::getMainFrame()->insideBorders(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition())) return 2;
	height=CFrameComponent::getEffectiveHeight();
	width=CFrameComponent::getEffectiveWidth();
	// clear the canvas
	for (int i=0;i<height;i++) {
		if (!CFrameComponent::getMainFrame()->clearCanvas(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition()+CPoint(0,i),width,
		  CStyle(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle().getColorPair(),0))) return 3;
	}	
	newY=0;
	while ((newY<height) && (newY<m_bitsNumber)) {
		if (m_drawLabels) {
			caption=IRA::CString(m_bitDescriptions[static_cast<unsigned long>(newY)]);
			//fit the caption to the available space
			if (caption.GetLength()>width-2) {
				caption=caption.Left(width-2);
			}
			// need to give space to  the led
			newX=CFrameComponent::getWidthAlignment(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getWAlign(),caption.GetLength(),width-2);
			newPosition=CPoint(newX,newY);
			if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition(),
					CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle(),caption)) return 3;
		}
		//now draw the leds
		// get the current value as a integer
		value=strtoull((const char*)CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getValue(),&endPtr,10);
		if ((value==0) && (endPtr==(const char*)CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getValue())) {
			return 5;
		}
		bitPos=1 << (newY);
		if ((value&bitPos)==bitPos) { //bit is set
			shape=m_whenSet[newY];
		}
		else { // bit is cleared
			shape=m_whenCleared[newY];
		}
		CStyle ledStyle(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle());
		if (shape==ACS::RED) {
			ledStyle.setColorPair(CColorPair::RED_BLACK);
		}
		else if (shape==ACS::YELLOW) {
			ledStyle.setColorPair(CColorPair::YELLOW_BLACK);
		}
		else if (shape==ACS::GREEN) {
			ledStyle.setColorPair(CColorPair::GREEN_BLACK);
		}
		else if (shape==ACS::GREY) {
			ledStyle.setColorPair(CColorPair::WHITE_BLACK);
		}
		newPosition=CPoint(width-1,newY);
		if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition(),
				ledStyle,"O")) return 3;
		newY++;
	}
	return 0;
}
