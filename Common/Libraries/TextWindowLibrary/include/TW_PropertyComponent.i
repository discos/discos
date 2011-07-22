// $Id: TW_PropertyComponent.i,v 1.2 2007-09-11 15:02:07 a.orlati Exp $

template <_TW_PROPERTYCOMPONENT_C>
CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::CPropertyComponent(PR property) : CFrameComponent()
{
	m_LastError="";
	m_installed=false;
	m_property_p=property;
	m_callback_p=NULL;
}

template <_TW_PROPERTYCOMPONENT_C>
CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::~CPropertyComponent()
{
	try {
		if (m_callback_p.ptr()!=NULL) {
			PortableServer::ObjectId *oid=m_callback._default_POA()->reference_to_id(m_callback_p.in());
			m_callback._default_POA()->deactivate_object(*oid);
		}
	}
	catch (...) {
	}
	if (m_installed) {
		m_monitor_p->suspend();
		m_monitor_p->destroy();
		m_callback.setFrameComponent(NULL);
		m_installed=false;
	}
}

template <_TW_PROPERTYCOMPONENT_C>
bool CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::installAutomaticMonitor(const DWORD& logInterval)
{
	ACS::CBDescIn desc;
	try {
		m_callback.setFrameComponent(this);
		m_callback.setGuardInterval(logInterval*1000);
		m_callback_p=m_callback._this();
		m_monitor_p=m_property_p->create_monitor(m_callback_p.in(),desc);
		if (m_monitor_p.ptr()!=M::_nil()) {
			m_installed=true;
			return true;
		}
		else return false;
	}
	catch (CORBA::SystemException &Ex) {
		m_LastError.Format("Caught a CORBA System Exception: %s:%d",Ex._name(),Ex.minor());
		return false;
	}
	catch (...) {
		m_LastError.Format("Caught an unknown exception");
		return false;
	}
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setFormatFunction(CCommonCallback::TFormatFunction func,const void*arg)
{
	m_callback.setFormatFunction(func,arg);
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setTriggerTime(DWORD millisec)
{
	ACS::TimeInterval timer;
	timer=millisec*10000;
	if (m_installed) m_monitor_p->set_timer_trigger(timer);
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValueTrigger(double delta,bool enable)
{
	if (m_installed) {
		m_monitor_p->set_value_trigger(delta,enable);
	}
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValueTrigger(long delta,bool enable)
{
	if (m_installed) {
		m_monitor_p->set_value_trigger(delta,enable);
	}
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValueTrigger(long long delta,bool enable)
{
	if (m_installed) {
		m_monitor_p->set_value_trigger(delta,enable);
	}
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::setValueTrigger(unsigned long long delta,bool enable)
{
	if (m_installed) {
		m_monitor_p->set_value_trigger(delta,enable);
	}
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::deActivate()
{
	if (m_installed) { 
		m_callback.Stop(); 
	}	
	CFrameComponent::deActivate();		
}

template <_TW_PROPERTYCOMPONENT_C>
void CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::Activate()
{
	if (m_installed) {
		m_callback.Start();
	}
	CFrameComponent::Activate();
}
