// $Id: TW_PropertyText.i,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $


template <_TW_PROPERTYCOMPONENT_C>
CPropertyText<_TW_PROPERTYCOMPONENT_TL>::CPropertyText(PR property) : CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>(property)
{
}

template <_TW_PROPERTYCOMPONENT_C>
CPropertyText<_TW_PROPERTYCOMPONENT_TL>::~CPropertyText()
{
}

template <_TW_PROPERTYCOMPONENT_C>
WORD CPropertyText<_TW_PROPERTYCOMPONENT_TL>::draw()
{
	WORD width,height;
	bool done;
	CPoint newPosition;
	IRA::CString tmp;
	WORD iter,step;
	WORD newX,newY;	
	if (!CFrameComponent::getMainFrame()) return 4;
	if (!CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getEnabled()) return 1;
	if (!CFrameComponent::getMainFrame()->insideBorders(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition())) return 2;
	height=CFrameComponent::getEffectiveHeight();
	width=CFrameComponent::getEffectiveWidth();
	step=CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getValue().GetLength()/width;
	newY=CFrameComponent::getHeightAlignment(CPropertyComponent <_TW_PROPERTYCOMPONENT_TL>::getHAlign(),step,height);
	iter=0;
	done=false;
	for (int i=0;i<height;i++) {
		if (!CFrameComponent::getMainFrame()->clearCanvas(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition()+CPoint(0,i),width,
		  CStyle(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle().getColorPair(),0))) return 3;
	}	
	while ((newY<height) && (!done)) {
		if (step>0) {
			tmp=CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getValue().Mid(iter*width,width);
		}
		else {
			tmp=CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getValue();
			done=true;
		}
		iter++;
		newX=CFrameComponent::getWidthAlignment(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getWAlign(),tmp.GetLength(),width);
		newPosition=CPoint(newX,newY);
		if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition(),CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle(),tmp)) return 3;
		newY++;
	}
	return 0;
}
