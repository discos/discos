// $Id: Frame.cpp,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

#include "TW_Frame.h"
#include "TW_FrameComponent.h"

using namespace TW;

CFrame::CFrame(const CPoint& ul,const CPoint& br,char ls,char rs,char us,char bs): m_upperLeft(ul), m_bottomRight(br), m_ls(ls), m_rs(rs), m_us(us), m_bs(bs)
{
	CPoint Tmp;
	Tmp=m_upperLeft-m_bottomRight;
	// I need to take into account the borders of the window.
	m_borderW=m_borderH=1;
	m_height=Tmp.getY();
	m_width=Tmp.getX();
	m_show=false;
	m_win=NULL;
	m_title="";
	m_titleStyle=CStyle(CStyle(CColorPair::WHITE_BLACK,CStyle::BOLD));
	m_destroyed=false;
}

CFrame::~CFrame()
{
	Destroy();
}

void CFrame::initFrame()
{
	initscr();
	m_win=newwin(m_height,m_width,m_upperLeft.getY(),m_upperLeft.getX());
	cbreak();
	noecho();
	nodelay(m_win,TRUE);
	keypad(m_win,TRUE);
	start_color();
	defineColorPair(CColorPair::RED_BLACK,CColor::RED,CColor::BLACK);
	defineColorPair(CColorPair::GREEN_BLACK,CColor::GREEN,CColor::BLACK);
	defineColorPair(CColorPair::BLUE_BLACK,CColor::BLUE,CColor::BLACK);
	defineColorPair(CColorPair::YELLOW_BLACK,CColor::YELLOW,CColor::BLACK);
	defineColorPair(CColorPair::MAGENTA_BLACK,CColor::MAGENTA,CColor::BLACK);
	defineColorPair(CColorPair::CYAN_BLACK,CColor::CYAN,CColor::BLACK);
	defineColorPair(CColorPair::WHITE_BLACK,CColor::WHITE,CColor::BLACK);
	defineColorPair(CColorPair::WHITE_GREEN,CColor::WHITE,CColor::GREEN);
	defineColorPair(CColorPair::WHITE_BLUE,CColor::WHITE,CColor::BLUE);
	defineColorPair(CColorPair::WHITE_RED,CColor::WHITE,CColor::RED);
	defineColorPair(CColorPair::WHITE_YELLOW,CColor::WHITE,CColor::YELLOW);
	defineColorPair(CColorPair::WHITE_CYAN,CColor::WHITE,CColor::CYAN);
	defineColorPair(CColorPair::WHITE_MAGENTA,CColor::WHITE,CColor::MAGENTA);
	curs_set(0);
	wclear(m_win);
	endwin();
}

void CFrame::Destroy()
{
	if (!m_destroyed) {
		CFrameComponent *frameComp;
		m_show=false;
		std::list<DWORD>::const_iterator p1;
		for (p1=m_components.begin();p1!=m_components.end();++p1) {
			frameComp=(CFrameComponent*)(*p1);
			frameComp->deActivate();
			frameComp->m_frame=NULL;
			delete frameComp;
		}
		m_components.clear();
		endwin();
	}
	m_destroyed=true;
}

void CFrame::showFrame()
{
	CFrameComponent *frameComp;
	std::list<DWORD>::const_iterator p1;
	for (p1=m_components.begin();p1!=m_components.end();++p1) {
		frameComp=(CFrameComponent*)(*p1);
		frameComp->Activate();
	}
	m_show=true;
	Refresh();	
}

void CFrame::Refresh()
{
	CFrameComponent *frameComp;
	WORD len;
	IRA::CString temp;
	std::list<DWORD>::const_iterator p1;
	wborder(m_win,m_ls,m_rs,m_us,m_bs,0,0,0,0);
	len=m_title.GetLength();
	if (len>0) {
		if (len>getWidth()-1) {
			temp=m_title.Left(getWidth()-1);
		}
		else {
			temp=m_title;
		}
		Write(CPoint(1,0),m_titleStyle,temp);
	}
	wrefresh(m_win);
	for (p1=m_components.begin();p1!=m_components.end();++p1) {
		frameComp=(CFrameComponent*)(*p1);
		frameComp->Refresh();
	}	
}

void CFrame::closeFrame()
{
	CFrameComponent *frameComp;
	m_show=false;
	std::list<DWORD>::const_iterator p1;
	for (p1=m_components.begin();p1!=m_components.end();++p1) {
		frameComp=(CFrameComponent*)(*p1);
		frameComp->deActivate();
	}
	endwin();
}

void CFrame::addComponent(CFrameComponent *comp)
{
	DWORD id;
	if (comp!=NULL) {
		id=(DWORD)comp;
		m_components.push_back(id);
		comp->m_frame=this;
	}
}

void CFrame::removeComponent(CFrameComponent *comp)
{
	DWORD id;
	std::list<DWORD>::iterator pos;
	id=(DWORD) comp;
	pos=find(m_components.begin(),m_components.end(),id); 
	if (pos!=m_components.end()) {
		m_components.erase(pos);		
		comp->deActivate();
		comp->m_frame=NULL;
	}
}

bool CFrame::writeCanvas(const CPoint& xy,const CStyle& style,const IRA::CString& text)
{
	if (!m_show) return false;
	if (!insideBorders(xy)) return false;
	CPoint point;		
	WORD len=text.GetLength();
	WORD width=getUsableWidth();
	IRA::CString temp;
	if ((xy.m_x+len-1)>=width) {
		temp=text.Left(width-xy.m_x);
	}
	else {
		temp=text;
	}
	point=xy+CPoint(1,1);
	Write(point,style,temp);
	return true;
}

bool CFrame::clearCanvas(const CPoint& xy,const WORD& cols,const CStyle& style)
{
	if (!m_show) return false;
	if (!insideBorders(xy)) return false;
	CPoint point;
	WORD width=getUsableWidth();
	WORD elements;
	char *temp;
	if ((xy.m_x+cols-1)>=width) {
		elements=width-xy.m_x;
	}
	else {
		elements=cols;
	}
	temp=new char[(elements+1)];
	memset(temp,' ',elements);
	temp[elements]=0;	
	point=xy+CPoint(1,1);
	Write(point,style,IRA::CString(temp));
	delete [] temp;
	return true;	
}

bool CFrame::readCanvas(char &ch)
{
	int read;
	ACS::ThreadSyncGuard guard(&m_mutex);
	read=wgetch(m_win);
	if (read==ERR) {
		return false;
	}
	else {
		ch=(char)read;
		return true;
	}
}

bool CFrame::insideBorders(const CPoint& p) const
{
	if ((p.m_x>getUsableWidth()-1) || (p.m_x<0)) return false;
	if ((p.m_y>getUsableHeight()-1) || (p.m_y<0)) return false;
	return true;
}

short CFrame::getUserColorNumber() const
{
	return CColor::DEFINABLECOLORS;
}

short CFrame::getUserColorPairNumber() const
{
	return CColorPair::DEFINABLEPAIRS;
}

bool CFrame::colorReady() const
{
	return can_change_color();
}

void CFrame::defineColor(const CColor& color,short red,short green,short blue) const
{
	if (red<0) red=0;
	else if (red>1000) red=1000;		
	if (green<0) green=0;
	else if (green>1000) green=1000;
	if (blue<0) blue=0;
	else if (blue>1000) blue=1000;
	init_color((short)color,red,green,blue);
}

void CFrame::defineColorPair(const CColorPair& pair,const CColor& fore,const CColor& back) const
{
	init_pair((short)pair,(short)fore,(short)back);
}

void CFrame::Write(const CPoint& xy,const CStyle& style,const IRA::CString& text)
{
	CColorPair pair=style.m_colorPair;
	int attributes=style.m_textAttributes;
	ACS::ThreadSyncGuard guard(&m_mutex);
	wattron(m_win,attributes);
	wattron(m_win,COLOR_PAIR((short)pair));
	mvwaddstr(m_win,xy.m_y,xy.m_x,(const char*)text);
	wattroff(m_win,COLOR_PAIR((short)pair));
	wattroff(m_win,attributes);
	wrefresh(m_win);
}
