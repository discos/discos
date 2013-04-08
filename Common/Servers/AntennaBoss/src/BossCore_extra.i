#ifndef _BOSS_CORE_EXTRA_I_
#define _BOSS_CORE_EXTRA_I_


void CBossCore::setProgramTrack() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (m_enable) {
			m_mount->changeMode(Antenna::ACU_PROGRAMTRACK,Antenna::ACU_PROGRAMTRACK);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::setProgramTrack()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE); // Failing to change the mode is a high priority error
		m_mountError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::setProgramTrack()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("changeMode()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::setProgramTrack()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("changeMode()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		changeBossStatus(Management::MNG_FAILURE);
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::setProgramTrack()");
	}
}

void CBossCore::setPreset() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (m_enable) {
			m_mount->changeMode(Antenna::ACU_PRESET,Antenna::ACU_PRESET);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::setPreset()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE); // Failing to change the mode is a high priority error
		m_mountError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::setPreset()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("changeMode()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::setPreset()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("changeMode()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		changeBossStatus(Management::MNG_FAILURE);
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::setPreset()");
	}	
}

void CBossCore::unstow() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	ACS::CBDescIn desc;
	loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
	desc.normal_timeout=4000000;
	//cbvoid=callbackUnstow._this(); 
	try {
		if (m_enable) {
			m_mount->unstow(m_cbUnstow,desc);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::unstow()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw impl;
	}
}

void CBossCore::preset(const double& az,const double& el) throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	double azDeg,elDeg;
	azDeg=az*DR2D;
	elDeg=el*DR2D;
	loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (m_enable) {
			m_mount->preset(azDeg,elDeg);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::preset()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::preset()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("preset()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
}

#endif


