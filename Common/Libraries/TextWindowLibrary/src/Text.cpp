// $Id: Text.cpp,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

#include "TW_Text.h"

using namespace TW;

CText::CText() : CFrameComponent()
{
}

CText::~CText()
{
}

WORD CText::draw()
{
	WORD width,height;
	bool done;
	CPoint newPosition;
	IRA::CString tmp;
	WORD iter,step;
	WORD newX,newY;	
	if (!CFrameComponent::getMainFrame()) return 4;
	if (!getEnabled()) return 1;
	if (!CFrameComponent::getMainFrame()->insideBorders(getPosition())) return 2;
	height=CFrameComponent::getEffectiveHeight();
	width=CFrameComponent::getEffectiveWidth();
	step=getValue().GetLength()/width;
	newY=CFrameComponent::getHeightAlignment(getHAlign(),step,height);
	iter=0;
	done=false;
	for (int i=0;i<height;i++) {
		if (!CFrameComponent::getMainFrame()->clearCanvas(getPosition()+CPoint(0,i),width,
		  CStyle(getStyle().getColorPair(),0))) return 3;
	}	
	while ((newY<height) && (!done)) {
		if (step>0) {
			tmp=getValue().Mid(iter*width,width);
		}
		else {
			tmp=getValue();
			done=true;
		}
		iter++;
		newX=CFrameComponent::getWidthAlignment(getWAlign(),tmp.GetLength(),width);
		newPosition=CPoint(newX,newY);
		if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+getPosition(),getStyle(),tmp)) return 3;
		newY++;
	}
	return 0;
}
