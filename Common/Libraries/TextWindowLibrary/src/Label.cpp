// $Id: Label.cpp,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

#include "TW_Label.h"

using namespace TW;


CLabel::CLabel(IRA::CString caption) : CFrameComponent()
{
	setValue(caption);
	setWidth(caption.GetLength());
}

CLabel::~CLabel()
{
}

WORD CLabel::draw()
{
	WORD width,height;
	bool done;
	CPoint newPosition;
	IRA::CString tmp;
	WORD iter,step;
	WORD newX,newY;	
	if (!getMainFrame()) return 4;
	if (!getMainFrame()->insideBorders(getPosition())) return 2;
	height=getEffectiveHeight();
	width=getEffectiveWidth();
	step=getValue().GetLength()/width;
	newY=CFrameComponent::getHeightAlignment(getHAlign(),step,height);
	iter=0;
	done=false;
	for (int i=0;i<height;i++) {
		if (!getMainFrame()->clearCanvas(getPosition()+CPoint(0,i),width,CStyle(getStyle().getColorPair(),0))) return 3;
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
		if (!getMainFrame()->writeCanvas(newPosition+getPosition(),getStyle(),tmp)) return 3;
		newY++;
	}
	return 0;
}
