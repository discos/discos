// $Id: FrameComponent.cpp,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

#include "TW_FrameComponent.h"

using namespace TW;

CFrameComponent::CFrameComponent() 
{
	m_enabled=true;
	m_position=CPoint(0,0);
	m_tag=0;
   m_value.Empty();
	m_style=CStyle(CColorPair::WHITE_BLACK,CStyle::NORMAL);
	m_width=10;
	m_height=1;
	m_alignHeight=MIDDLE;
	m_alignWidth=LEFT;
	m_frame=NULL;
	m_activated=false;
	m_outputComponent=NULL;
	
}

CFrameComponent::~CFrameComponent()
{
	if (getMainFrame()!=NULL) {
		getMainFrame()->removeComponent((CFrameComponent*)this);
	}
}

void CFrameComponent::Refresh()
{
	if (isAlive()) draw();
}

void CFrameComponent::setValue(const IRA::CString& val)
{
	m_value=val;
}

void CFrameComponent::setWidth(const WORD& w)
{ 
	m_width=w;
}

void CFrameComponent::setHeight(const WORD& h)
{
	m_height=h;
}

void CFrameComponent::setHAlign(const TAlignH& h)
{ 
	m_alignHeight=h;
}

void CFrameComponent::setWAlign(const TAlignW& w)
{
	m_alignWidth=w;
}

WORD CFrameComponent::getEffectiveHeight() const
{
	WORD frameHeight;
	frameHeight=m_frame->getUsableHeight();
	if (m_position.getY()+m_height-1>=frameHeight) {
		 return frameHeight-m_position.getY();
	}
	else {
		return m_height;
	}
}

WORD CFrameComponent::getEffectiveWidth() const
{
	WORD frameWidth=m_frame->getUsableWidth();
	if (m_position.getX()+m_width-1>=frameWidth) {
		return frameWidth-m_position.getX();
	}
	else {
		return m_width;
	}
}

WORD CFrameComponent::getHeightAlignment(const TAlignH& align,const WORD& rows,const WORD& height)
{
	if (height>rows) {
		if (align==UP) {
			return 0;
		}
		else if (align==MIDDLE) {
			return int(float(height-rows)/2.0);
		}
		else { //DOWN
			return height-rows;
		}
	}
	else {
		return 0;
	}
}

WORD CFrameComponent::getWidthAlignment(const TAlignW& align,const WORD& cols,const WORD& width)
{
	if (width>cols) {
		if (align==LEFT) {
			return 0;
		}
		else if (align==CENTER) {
			return int(float(width-cols)/2.0);
		}
		else { //RIGHT
			return width-cols;
		}
	}
	else {
		return 0;
	}	
}

void CFrameComponent::outputMessage(const IRA::CString& msg)
{
	CFrameComponent *out;
	out=getOutputComponent();
	if (out!=NULL) {
		out->setValue(msg);
		out->Refresh();
	}
}

bool CFrameComponent::isAlive()
{
	return (m_activated && m_frame);
}

CFrame * CFrameComponent::getMainFrame()
{
	return m_frame;
}

void CFrameComponent::Activate()
{
	m_activated=true;
}

void CFrameComponent::deActivate()
{
	m_activated=false;
}
