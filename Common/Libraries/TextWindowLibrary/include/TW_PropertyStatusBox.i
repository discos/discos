// $Id: TW_PropertyStatusBox.i,v 1.1.1.1 2007-05-04 09:53:29 a.orlati Exp $

template <_TW_PROPERTYSTATUSBOX_C>
CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::CPropertyStatusBox(PR property,const EN& init) : CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>(property)
{
	m_valuesList.clear();
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setHAlign(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::MIDDLE);
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setWAlign(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::CENTER);
	m_statusValue=init;
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValue("");
	try {
		m_Descriptions=property->statesDescription();
	}
	catch (CORBA::SystemException &Ex) {
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::m_LastError.Format("Caught a CORBA System Exception, while trying to read states descriptions: %s:%d",Ex._name(),Ex.minor());
	}
	catch (...) {
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::m_LastError.Format("Caught an unknown exception, while trying to read states descriptions");
	}	
}

template <_TW_PROPERTYSTATUSBOX_C>
CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::~CPropertyStatusBox()
{
	m_valuesList.clear();
}

template <_TW_PROPERTYSTATUSBOX_C>
void CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::setStatusLook(const EN& status,const IRA::CString& caption,const CStyle& style)
{
	m_valuesList[status].caption=caption;
	m_valuesList[status].style=style;
	m_valuesList[status].status=status;
}

template <_TW_PROPERTYSTATUSBOX_C>
void CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::setStatusLook(const EN& status,const IRA::CString& caption)
{
	m_valuesList[status].caption=caption;
	m_valuesList[status].style=CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle();
	m_valuesList[status].status=status;
}

template <_TW_PROPERTYSTATUSBOX_C>
void CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::setStatusLook(const EN& status,const CStyle& style)
{
	m_valuesList[status].caption=IRA::CString(m_Descriptions[static_cast<unsigned long>(status)]);
	m_valuesList[status].style=style;
	m_valuesList[status].status=status;
}

template <_TW_PROPERTYSTATUSBOX_C>
void CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::setStatusLook(const EN& status)
{
	m_valuesList[status].caption=IRA::CString(m_Descriptions[static_cast<unsigned long>(status)]);
	m_valuesList[status].style=CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle();
	m_valuesList[status].status=status;
}

template <_TW_PROPERTYSTATUSBOX_C>
void CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::getStatusLook(const EN& status,IRA::CString& caption,CStyle& style)
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

template <_TW_PROPERTYSTATUSBOX_C>
bool CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::getStatusFromDescription(const IRA::CString& descr,EN& status)
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

template <_TW_PROPERTYSTATUSBOX_C>
void CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::setValue(const IRA::CString& val)
{
	char *endPtr;		
	DDWORD intVal=strtoull((const char*)val,&endPtr,10);
	if ((intVal==0) && (endPtr==(const char*)val)) {
	}
	else {
		m_statusValue=static_cast<EN>(intVal);		
		CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValue(val);
	}
}

template <_TW_PROPERTYSTATUSBOX_C>
void CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::setValue(const EN& val)
{
	int intVal=(int)val;
	CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValue(IRA::CString(intVal));
	m_statusValue=val;
}

template <_TW_PROPERTYSTATUSBOX_C>
WORD CPropertyStatusBox<_TW_PROPERTYSTATUSBOX_TL>::draw()
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
	if (!CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getEnabled()) return 1;
	if (!CFrameComponent::getMainFrame()->insideBorders(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition())) return 2;
	height=CFrameComponent::getEffectiveHeight();
	width=CFrameComponent::getEffectiveWidth();
	getStatusLook(m_statusValue,caption,currentStyle);	
	step=caption.GetLength()/width;
	newY=CFrameComponent::getHeightAlignment(CPropertyComponent <_TW_PROPERTYCOMPONENT_TL>::getHAlign(),step,height);
	iter=0;
	done=false;
	for (int i=0;i<height;i++) {
		if (!CFrameComponent::getMainFrame()->clearCanvas(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition()+CPoint(0,i),width,
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
		newX=CFrameComponent::getWidthAlignment(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getWAlign(),tmp.GetLength(),width);
		newPosition=CPoint(newX,newY);
		if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition(),currentStyle,tmp)) return 3;
		newY++;
	}
	return 0;
}
