// $Id: TW_StatusBox.i,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

template <_TW_STATUSBOX_C>
CStatusBox<_TW_STATUSBOX_TL>::CStatusBox(const EN& init) : CFrameComponent()
{
	m_valuesList.clear();
	CFrameComponent::setHAlign(CFrameComponent::MIDDLE);
	CFrameComponent::setWAlign(CFrameComponent::CENTER);
	m_statusValue=init;
	CFrameComponent::setValue("");
}

template <_TW_STATUSBOX_C>
CStatusBox<_TW_STATUSBOX_TL>::~CStatusBox()
{
	m_valuesList.clear();
}

template <_TW_STATUSBOX_C>
void CStatusBox<_TW_STATUSBOX_TL>::setStatusLook(const EN& status,const IRA::CString& caption,const CStyle& style)
{
	m_valuesList[status].caption=caption;
	m_valuesList[status].style=style;
	m_valuesList[status].status=status;
}

template <_TW_STATUSBOX_C>
void CStatusBox<_TW_STATUSBOX_TL>::setStatusLook(const EN& status,const IRA::CString& caption)
{
	m_valuesList[status].caption=caption;
	m_valuesList[status].style=CFrameComponent::getStyle();
	m_valuesList[status].status=status;
}

template <_TW_STATUSBOX_C>
void CStatusBox<_TW_STATUSBOX_TL>::getStatusLook(const EN& status,IRA::CString& caption,CStyle& style)
{
	if (m_valuesList.find(status)==m_valuesList.end()) {
		caption=IRA::CString("");
		style=CStyle();
	}
	else {
		caption=m_valuesList[status].caption;
		style=m_valuesList[status].style;
	}
}

template <_TW_STATUSBOX_C>
bool CStatusBox<_TW_STATUSBOX_TL>::getStatusFromDescription(const IRA::CString& descr,EN& status)
{
	typename std::map<EN,TElement>::iterator iter;
	for(iter=m_valuesList.begin();iter!=m_valuesList.end();iter++) {
		if (iter->second.caption==descr) {
			status=iter->second.status;
			return true;
		}
	}
	return false;
}

template <_TW_STATUSBOX_C>
void CStatusBox<_TW_STATUSBOX_TL>::setValue(const IRA::CString& val)
{
	char *endPtr;		
	DDWORD intVal=strtoull((const char*)val,&endPtr,10);
	if ((intVal==0) && (endPtr==(const char*)val)) {
	}
	else {
		m_statusValue=static_cast<EN>(intVal);		
		CFrameComponent::setValue(val);
	}
}

template <_TW_STATUSBOX_C>
void CStatusBox<_TW_STATUSBOX_TL>::setValue(const EN& val)
{
	int intVal=(int)val;
	setValue(IRA::CString(intVal));
	m_statusValue=val;
}

template <_TW_STATUSBOX_C>
WORD CStatusBox<_TW_STATUSBOX_TL>::draw()
{
	WORD width,height;
	bool done;
	CPoint newPosition;
	IRA::CString tmp;
	WORD iter,step;
	WORD newX,newY;
	IRA::CString caption;	
	CStyle currentStyle;
	if (!CFrameComponent::getMainFrame()) return 4;
	if (!CFrameComponent::getEnabled()) return 1;
	if (!CFrameComponent::getMainFrame()->insideBorders(CFrameComponent::getPosition())) return 2;
	height=CFrameComponent::getEffectiveHeight();
	width=CFrameComponent::getEffectiveWidth();
	getStatusLook(m_statusValue,caption,currentStyle);	
	step=caption.GetLength()/width;
	newY=CFrameComponent::getHeightAlignment(CFrameComponent::getHAlign(),step,height);
	iter=0;
	done=false;
	for (int i=0;i<height;i++) {
		if (!CFrameComponent::getMainFrame()->clearCanvas(CFrameComponent::getPosition()+CPoint(0,i),width,
		  CStyle(currentStyle.getColorPair(),0))) return 3;
	}	
	while ((newY<height) && (!done)) {
		if (step>0) {
			tmp=caption.Mid(iter*width,width);
		}
		else {
			tmp=caption;
			done=true;
		}
		iter++;
		newX=CFrameComponent::getWidthAlignment(CFrameComponent::getWAlign(),tmp.GetLength(),width);
		newPosition=CPoint(newX,newY);
		if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+CFrameComponent::getPosition(),currentStyle,tmp)) return 3;
		newY++;
	}
	return 0;
}
