
#include "MixerOperator.h"
#include <LogFilter.h>
#include <ReceiverControl.h>
#include <LocalOscillatorInterfaceC.h>
#include <ManagmentDefinitionsC.h>
#include "Commons.h"

/** 
 * @todo Aggioranre i log gli include del modulo, macro comprese
 * Inserire nel modulo ComponetCore e verificare che sia tutto ok
 */

MixerOperator::MixerOperator(CConfiguration & p_config):
    m_configuration(p_config)
{
	 m_services= NULL;
    m_loDev_1st= Receivers::LocalOscillator::_nil();
    m_loDev_2nd= Receivers::LocalOscillator::_nil();
    m_current_value= 0.0;
    m_init_ok= false;
}

MixerOperator::~MixerOperator()
{
    m_loDev_1st= Receivers::LocalOscillator::_nil();
    m_loDev_2nd= Receivers::LocalOscillator::_nil();
}

void MixerOperator::setServices(maci::ContainerServices * p_services)
{
	m_services= p_services;
}	

bool MixerOperator::isLoaded() const
{
  return m_init_ok; 	
}

void MixerOperator::loadComponents() 
        throw (ComponentErrors::CouldntGetComponentExImpl)
{
    const char * m_1st_name= m_configuration.getLocalOscillatorInstance1st();
    const char * m_2nd_name= m_configuration.getLocalOscillatorInstance2nd();    
    loadDevice(m_loDev_1st, m_1st_name);
    loadDevice(m_loDev_2nd, m_2nd_name);
    m_init_ok= true;
}

void MixerOperator::releaseComponents()
{
	 const char * m_1st_name= m_configuration.getLocalOscillatorInstance1st();
    const char * m_2nd_name= m_configuration.getLocalOscillatorInstance2nd();
    releaseDevice(m_loDev_1st, m_1st_name);
    releaseDevice(m_loDev_2nd, m_2nd_name);
    m_init_ok= false;
}

bool MixerOperator::setValue(const ACS::doubleSeq& p_values)
                    throw (ComponentErrors::ValidationErrorExImpl,
                        ComponentErrors::ValueOutofRangeExImpl,
                        ComponentErrors::CouldntGetComponentExImpl,
                        ComponentErrors::CORBAProblemExImpl,
                        ReceiversErrors::LocalOscillatorErrorExImpl)
{
    /** @todo controllare */
    if(!m_init_ok || CORBA::is_nil(m_loDev_1st) || CORBA::is_nil(m_loDev_1st) ){
        ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",
                    (LM_NOTICE,"LOs not ready to be set"));
        return false;
    }        
    /**/
    double trueValue,amp;
    double *freq=NULL;
    double *power=NULL;
    DWORD size;
    if (p_values.length()==0) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::setLO");
        impl.setReason("at least one value must be provided");
        throw impl;
    }
    // in case -1 is given we keep the current value...so nothing to do
    if (p_values[0]==-1) {
        ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",
                    (LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",m_current_value));
        return false;
    }
    // now check if the requested value match the limits
    if (p_values[0]<m_configuration.getLOMin()[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
        impl.setValueName("local oscillator lower limit");
        impl.setValueLimit(m_configuration.getLOMin()[0]);
        throw impl;
    }
    else if (p_values[0]>m_configuration.getLOMax()[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
        impl.setValueName("local oscillator upper limit");
        impl.setValueLimit(m_configuration.getLOMax()[0]);
        throw impl;
    }
    //computes the synthesizer settings
    trueValue= p_values[0]+m_configuration.getFixedLO2()[0];
    size= m_configuration.getSynthesizerTable(freq,power);
    amp= round(Helpers::linearFit(freq,power,size,trueValue));
    if (power) delete [] power;
    if (freq) delete [] freq;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_DEBUG,"SYNTHESIZER_VALUES %lf %lf",trueValue,amp));
    try {
        m_loDev_1st->set(amp, trueValue);
    }
    catch (CORBA::SystemException& ex) {
        m_mixer_fault= true;
        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CComponentCore::setLO()");
        impl.setName(ex._name());
        impl.setMinor(ex.minor());
        throw impl;
    }
    catch (ReceiversErrors::ReceiversErrorsEx& ex) { 
        _ADD_BACKTRACE(ReceiversErrors::LocalOscillatorErrorExImpl,impl,ex,"CComponentCore::setLO()");
        throw impl;
    }
    m_current_value= p_values[0];
    return true;
}

double MixerOperator::getValue()
{
    double l_power;
    double l_freq;
    try{
        m_loDev_1st->get(l_power, l_freq);
    } catch (ReceiversErrors::ReceiversErrorsEx& ex) { 
        _ADD_BACKTRACE(ReceiversErrors::LocalOscillatorErrorExImpl,impl,ex,"CComponentCore::setLO()");
        throw impl;        
    }
    return l_freq;
}

bool MixerOperator::isLocked()
{
   bool l_1st_lock= isDeviceLocked(m_loDev_1st, (const char *)m_configuration.getLocalOscillatorInstance1st());
	bool l_2nd_lock= isDeviceLocked(m_loDev_2nd, (const char *)m_configuration.getLocalOscillatorInstance2nd());
	return l_1st_lock || l_2nd_lock;
}

/* *** PRIVATE *** */

void MixerOperator::loadDevice(Receivers::LocalOscillator_var p_loDev, const char * p_lo_name){

    if ((!CORBA::is_nil(p_loDev)) && (m_mixer_fault)) { // if reference was already taken, but an error was found....dispose the reference
        try {
            releaseComponents();
        }catch (...) { //dispose silently...if an error...no matter
        }
        p_loDev= Receivers::LocalOscillator::_nil();
    }
    if (CORBA::is_nil(p_loDev)) {  //only if it has not been retrieved yet
        try {
            p_loDev= m_services->getComponent<Receivers::LocalOscillator>((const char*)p_lo_name);
            ACS_LOG(LM_FULL_INFO,"CCore::loadLocalOscillator()",(LM_INFO,"LOCAL_OSCILLATOR_OBTAINED"));
            m_mixer_fault= false;
        }
        catch (maciErrType::CannotGetComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
            Impl.setComponentName((const char*)p_lo_name);
            p_loDev= Receivers::LocalOscillator::_nil();
            throw Impl;
        }
        catch (maciErrType::NoPermissionExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
            Impl.setComponentName((const char*)p_lo_name);
            p_loDev= Receivers::LocalOscillator::_nil();
            throw Impl;
        }
        catch (maciErrType::NoDefaultComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
            Impl.setComponentName((const char*)p_lo_name);
            p_loDev= Receivers::LocalOscillator::_nil();
            throw Impl;
        }
    }
}

void MixerOperator::releaseDevice(Receivers::LocalOscillator_var p_loDev,
                    					const char * p_lo_name)
{
    if (!CORBA::is_nil(p_loDev)) {
        try {
            m_services->releaseComponent((const char*)p_lo_name);
        }
        catch (maciErrType::CannotReleaseComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CComponentCore::unloadLocalOscillator()");
            Impl.setComponentName(p_lo_name);
            Impl.log(LM_WARNING);
        }
        catch (...) {
            _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CComponentCore::unloadLocalOscillator()");
            impl.log(LM_WARNING);
        }
        p_loDev= Receivers::LocalOscillator::_nil();
        m_init_ok= false;
    }
}


bool MixerOperator::isDeviceLocked(Receivers::LocalOscillator_var p_loDev,
											 const char* p_lo_name){
	 ACSErr::Completion_var comp;
    ACS::ROlong_var isLockedRef;
    CORBA::Long isLocked;
    try {
        isLockedRef=p_loDev->isLocked();
    }
    catch (CORBA::SystemException& ex) {
        m_mixer_fault=true;
        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CComponentCore::checkLocalOscillator()");
        impl.setName(ex._name());
        impl.setMinor(ex.minor());
        throw impl;
    }
    isLocked=isLockedRef->get_sync(comp.out());
    ACSErr::CompletionImpl complImpl(comp);
    if (!complImpl.isErrorFree()) {
        _ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,complImpl,"CComponentCore::isDeviceLocked()");
        impl.setAttributeName("isLocked");
        impl.setComponentName(p_lo_name);
        throw impl;
    }
    return isLocked;	
}
