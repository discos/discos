
#include "MixerOperator.h"
#include <LogFilter.h>
#include <ReceiverControl.h>
#include <LocalOscillatorInterfaceC.h>
#include <ManagmentDefinitionsC.h>
#include "Commons.h"
#include "Defs.h"

/** 
 * @todo Aggioranre i log gli include del modulo, macro comprese
 * Inserire nel modulo ComponetCore e verificare che sia tutto ok
 */

MixerOperator::MixerOperator()
{
	MED_TRACE();
	m_services= NULL;
    m_configuration= NULL;
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
	MED_TRACE();
	m_services= p_services;
}	

void MixerOperator::setConfigurations(CConfiguration * p_confs){
    m_configuration= p_confs;
}

bool MixerOperator::isLoaded() const
{
  return m_init_ok; 	
}

void MixerOperator::loadComponents() 
        throw (ComponentErrors::CouldntGetComponentExImpl)
{
	MED_TRACE_MSG(" IN ");
#ifndef EXCLUDE_MIXER	 
    if( m_init_ok && CORBA::is_nil(m_loDev_1st) && CORBA::is_nil(m_loDev_2nd) ){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs already actives"));
        return;
    }        
    if (! m_configuration ){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs configuration not provided!"));
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }                      
    MED_TRACE_FMT("LO 1 : %s\n", (const char*)m_configuration->getLocalOscillatorInstance1st());
    MED_TRACE_FMT("LO 2 : %s\n", (const char*)m_configuration->getLocalOscillatorInstance2nd());
    try{           
        loadDevices();
    }catch(...){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs loading failed!"));
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }
#else
    	#warning "Excluding mixer.."
	 	MED_TRACE_MSG(" TESTING ");    
#endif
    m_init_ok= true;
    MED_TRACE_MSG(" OUT ");
}

void MixerOperator::releaseComponents() throw (ReceiversErrors::LocalOscillatorErrorExImpl)
{
#ifndef EXCLUDE_MIXER
    if (! m_configuration ){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs configuration not provided!"));
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }    
    try{        
        releaseDevices();                
    }catch(...){
       ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs release failed!"));
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }
#else
#endif
    m_init_ok= false;
}


bool MixerOperator::setValue(double p_value)
                    throw (ComponentErrors::ValidationErrorExImpl,
                        ComponentErrors::ValueOutofRangeExImpl,
                        ComponentErrors::CouldntGetComponentExImpl,
                        ComponentErrors::CORBAProblemExImpl,
                        ReceiversErrors::LocalOscillatorErrorExImpl)
{
    MED_TRACE_MSG(" IN ");        
    if(!m_init_ok || CORBA::is_nil(m_loDev_1st) || CORBA::is_nil(m_loDev_2nd) ){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::setLO()",
                    (LM_NOTICE,"LOs not ready to be set"));
		MED_TRACE_MSG(" LOs not ready to be set! ");                    
        return false;
    }             
    if (! m_configuration ){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs configuration not provided!"));
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }
    /**/
    double trueValue;
    double amp_lo, amp_lo2; 
    DWORD size_lo, size_lo2;
    double *freq_lo=NULL;
    double *power_lo=NULL;
    double *freq_lo2=NULL;
    double *power_lo2=NULL;

    ReceiverConfHandler::ConfigurationSetup l_setup= m_configuration->getCurrentSetup();	  
    
    // in case -1 is given we keep the current value...so nothing to do
    if (p_value == -1) {
        ACS_LOG(LM_FULL_INFO,"MixerOperator::setLO()",
                    (LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",m_current_value));
        return false;
    }    
    // now check if the requested value match the limits
    if (p_value < l_setup.m_LOMin[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"MixerOperator::setLO");
        impl.setValueName("local oscillator lower limit");
        impl.setValueLimit(l_setup.m_LOMin[0]);
        throw impl;
    }
    else if (p_value > l_setup.m_LOMax[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"MixerOperator::setLO");
        impl.setValueName("local oscillator upper limit");
        impl.setValueLimit(l_setup.m_LOMax[0]);
        throw impl;
    }
	MED_TRACE_MSG(" calculate value ");
    // LO2 specs fiex freq, get the freq, get the power
    size_lo2= m_configuration->getSynthesizerTable("LO2", freq_lo2, power_lo2);
    if (size_lo2 != 1){
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"MixerOperator::setLO");
        impl.setValueName("local oscillator 2 expecting only one configuration for freq/power value");        
        throw impl;
    } 
    if (l_setup.m_fixedLO2[0] != power_lo2[0]){
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"MixerOperator::setLO");
        impl.setValueName("local oscillator 2 conf table freq not matching expencting fixed frequency (2300MHz)");        
        throw impl;
    }
    amp_lo2= power_lo2[0];    
    //computes the synthesizer settings
    trueValue= p_value + l_setup.m_fixedLO2[0];
    // LO specs 
    size_lo= m_configuration->getSynthesizerTable("LO", freq_lo, power_lo);
    amp_lo= round(Helpers::linearFit(freq_lo, power_lo, size_lo, trueValue));
    if (power_lo) delete [] power_lo;
    if (freq_lo) delete [] freq_lo;    
    if (power_lo2) delete [] power_lo2;
    if (freq_lo2) delete [] freq_lo2; 
    ACS_LOG(LM_FULL_INFO,"MixerOperator::setLO()",(LM_DEBUG, "SYNTHESIZER_VALUES %lf %lf", trueValue, amp_lo));
    try {
		#ifndef EXCLUDE_MIXER
            m_loDev_2nd->set(amp_lo2, l_setup.m_fixedLO2[0]);
        	m_loDev_1st->set(amp_lo, trueValue);
        #endif
    }
    catch (CORBA::SystemException& ex) {
        m_mixer_fault= true;
        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"MixerOperator::setLO()");
        impl.setName(ex._name());
        impl.setMinor(ex.minor());
        throw impl;
    }
    catch (ReceiversErrors::ReceiversErrorsEx& ex) { 
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }
    m_current_value= p_values[0];
	MED_TRACE_MSG(" OUT ");
    return true;  
}

bool MixerOperator::setValue(const ACS::doubleSeq& p_values)
                    throw (ComponentErrors::ValidationErrorExImpl,
                        ComponentErrors::ValueOutofRangeExImpl,
                        ComponentErrors::CouldntGetComponentExImpl,
                        ComponentErrors::CORBAProblemExImpl,
                        ReceiversErrors::LocalOscillatorErrorExImpl)
{
	MED_TRACE_MSG(" IN ");        
    if(!m_init_ok || CORBA::is_nil(m_loDev_1st) || CORBA::is_nil(m_loDev_2nd) ){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::setLO()",
                    (LM_NOTICE,"LOs not ready to be set"));
		MED_TRACE_MSG(" LOs not ready to be set! ");                    
        return false;
    }             
    if (! m_configuration ){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs configuration not provided!"));
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }
    /**/
    double trueValue;
    double amp_lo, amp_lo2; 
    DWORD size_lo, size_lo2;
    double *freq_lo=NULL;
    double *power_lo=NULL;
    double *freq_lo2=NULL;
    double *power_lo2=NULL;

    ReceiverConfHandler::ConfigurationSetup l_setup= m_configuration->getCurrentSetup();	  
    
    if (p_values.length()==0) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"MixerOperator::setLO");
        impl.setReason("at least one value must be provided");
        throw impl;
    }
    // in case -1 is given we keep the current value...so nothing to do
    if (p_values[0]==-1) {
        ACS_LOG(LM_FULL_INFO,"MixerOperator::setLO()",
                    (LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",m_current_value));
        return false;
    }    
    // now check if the requested value match the limits
    if (p_values[0]<l_setup.m_LOMin[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"MixerOperator::setLO");
        impl.setValueName("local oscillator lower limit");
        impl.setValueLimit(l_setup.m_LOMin[0]);
        throw impl;
    }
    else if (p_values[0]> l_setup.m_LOMax[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"MixerOperator::setLO");
        impl.setValueName("local oscillator upper limit");
        impl.setValueLimit(l_setup.m_LOMax[0]);
        throw impl;
    }
	MED_TRACE_MSG(" calculate value ");
    // LO2 specs fiex freq, get the freq, get the power
    size_lo2= m_configuration->getSynthesizerTable("LO2", freq_lo2, power_lo2);
    if (size_lo2 != 1){
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"MixerOperator::setLO");
        impl.setValueName("local oscillator 2 expecting only one configuration for freq/power value");        
        throw impl;
    } 
    if (l_setup.m_fixedLO2[0] != power_lo2[0]){
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"MixerOperator::setLO");
        impl.setValueName("local oscillator 2 conf table freq not matching expencting fixed frequency (2300MHz)");        
        throw impl;
    }
    amp_lo2= power_lo2[0];    
    //computes the synthesizer settings
    trueValue= p_values[0]+ l_setup.m_fixedLO2[0];
    // LO specs 
    size_lo= m_configuration->getSynthesizerTable("LO", freq_lo, power_lo);
    amp_lo= round(Helpers::linearFit(freq_lo, power_lo, size_lo, trueValue));
    if (power_lo) delete [] power_lo;
    if (freq_lo) delete [] freq_lo;    
    if (power_lo2) delete [] power_lo2;
    if (freq_lo2) delete [] freq_lo2; 
    ACS_LOG(LM_FULL_INFO,"MixerOperator::setLO()",(LM_DEBUG, "SYNTHESIZER_VALUES %lf %lf", trueValue, amp_lo));
    try {
		#ifndef EXCLUDE_MIXER
            m_loDev_2nd->set(amp_lo2, l_setup.m_fixedLO2[0]);
        	m_loDev_1st->set(amp_lo, trueValue);
        #endif
    }
    catch (CORBA::SystemException& ex) {
        m_mixer_fault= true;
        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"MixerOperator::setLO()");
        impl.setName(ex._name());
        impl.setMinor(ex.minor());
        throw impl;
    }
    catch (ReceiversErrors::ReceiversErrorsEx& ex) { 
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }
    m_current_value= p_values[0];
	MED_TRACE_MSG(" OUT ");
    return true;
}

double MixerOperator::getValue() throw (ReceiversErrors::LocalOscillatorErrorExImpl)
{
    double l_power;
    double l_freq;
    if( CORBA::is_nil(m_loDev_1st)){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::getValue()",(LM_INFO,"LO is null!"));
        return 0.0;
    }
    try{
    	#ifndef EXCLUDE_MIXER                    
            m_loDev_1st->get(l_power, l_freq);            
        #else
            return 0.0;
       #endif
    } catch (ReceiversErrors::ReceiversErrorsEx& ex) { 
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;        
    }
    return l_freq;
}

double MixerOperator::getEffectiveValue() throw (ReceiversErrors::LocalOscillatorErrorExImpl)
{
    double l_power;
    double l_freq;
    if( CORBA::is_nil(m_loDev_1st)){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::getValue()",(LM_INFO,"LO is null!"));
        return 0.0;
    }
    try{
    	#ifndef EXCLUDE_MIXER  
            ReceiverConfHandler::ConfigurationSetup l_setup= m_configuration->getCurrentSetup();                  
            m_loDev_1st->get(l_power, l_freq);            
            l_freq -= l_setup.m_fixedLO2[0];
        #else
            return 0.0;
       #endif
    } catch (ReceiversErrors::ReceiversErrorsEx& ex) { 
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;        
    }
    return l_freq;
}

bool MixerOperator::isLocked() throw (ReceiversErrors::LocalOscillatorErrorExImpl, ReceiversErrors::LocalOscillatorErrorExImpl)
{
#ifndef EXCLUDE_MIXER
    if (! m_configuration ){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs configuration not provided!"));
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;
    }	
    try{
        return  isDeviceLocked();                
    }catch(...){
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadComponents()",
                    (LM_NOTICE,"LOs loading failed!"));
        _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"MixerOperator::setLO()");
        throw impl;        
    }
#else
	return false;
#endif
}

/* *** PRIVATE *** */

void MixerOperator::loadDevices()
                 throw (ComponentErrors::CouldntGetComponentExImpl)
{	    
    if ( ( !CORBA::is_nil(m_loDev_1st) || !CORBA::is_nil(m_loDev_2nd) ) && !m_mixer_fault ) { 
        #ifndef EXCLUDE_MIXER
            try {
                releaseDevices();
            }catch (...) { 
                //dispose silently...if an error...no matter
            }
        #endif
        m_loDev_1st= Receivers::LocalOscillator::_nil();
        m_loDev_2nd= Receivers::LocalOscillator::_nil();
    }    
    try {
    #ifndef EXCLUDE_MIXER
        m_loDev_1st= m_services->getComponent<Receivers::LocalOscillator>((const char*)m_configuration->getLocalOscillatorInstance1st());
        m_loDev_2nd= m_services->getComponent<Receivers::LocalOscillator>((const char*)m_configuration->getLocalOscillatorInstance2nd());
        ACS_LOG(LM_FULL_INFO,"MixerOperator::loadLocalOscillator()",(LM_INFO,"LOCAL_OSCILLATOR_OBTAINED"));
    #endif
        m_mixer_fault= false;
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl,Impl,"MixerOperator::loadLocalOscillator()");
        Impl.setComponentName("LO1 or LO2");
        m_loDev_1st= Receivers::LocalOscillator::_nil();
        m_loDev_2nd= Receivers::LocalOscillator::_nil();
        throw Impl;
    }
    catch (maciErrType::NoPermissionExImpl& ex) {
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl,Impl,"MixerOperator::loadLocalOscillator()");
        Impl.setComponentName("LO1 or LO2");
        m_loDev_1st= Receivers::LocalOscillator::_nil();
        m_loDev_2nd= Receivers::LocalOscillator::_nil();
        throw Impl;
    }
    catch (maciErrType::NoDefaultComponentExImpl& ex) {
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl,Impl,"MixerOperator::loadLocalOscillator()");
        Impl.setComponentName("LO1 or LO2");
        m_loDev_1st= Receivers::LocalOscillator::_nil();
        m_loDev_2nd= Receivers::LocalOscillator::_nil();
        throw Impl;
    }
    catch(...){
        MED_TRACE_MSG(" Failed to load LOs ");
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl,Impl,"MixerOperator::loadLocalOscillator()");
        Impl.setComponentName("LO1 or LO2");
        m_loDev_1st= Receivers::LocalOscillator::_nil();
        m_loDev_2nd= Receivers::LocalOscillator::_nil();
        throw Impl;
    }
    MED_TRACE_FMT("Device %s loaded \n", "LOs");
}

void MixerOperator::releaseDevices()
                        throw (ComponentErrors::CouldntReleaseComponentExImpl,
                            ComponentErrors::UnexpectedExImpl)
{
	#ifndef EXCLUDE_MIXER
    try {
        if(!CORBA::is_nil(m_loDev_1st))         		
            m_services->releaseComponent((const char*)m_configuration->getLocalOscillatorInstance1st());
        if(!CORBA::is_nil(m_loDev_2nd))         		
            m_services->releaseComponent((const char*)m_configuration->getLocalOscillatorInstance2nd());
    }
    catch (maciErrType::CannotReleaseComponentExImpl& ex) {
        _EXCPT(ComponentErrors::CouldntReleaseComponentExImpl,Impl,"MixerOperator::unloadLocalOscillator()");
        Impl.setComponentName("LO1 or LO2");
        Impl.log(LM_WARNING);
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"MixerOperator::unloadLocalOscillator()");
        impl.log(LM_WARNING);
    }
    m_loDev_1st= Receivers::LocalOscillator::_nil();
    m_loDev_2nd= Receivers::LocalOscillator::_nil();
    m_init_ok= false;    
    #else
    m_loDev_1st= Receivers::LocalOscillator::_nil();
    m_loDev_2nd= Receivers::LocalOscillator::_nil();
    m_init_ok= false;
    #endif
}

bool MixerOperator::isDeviceLocked()
                            throw (ComponentErrors::CORBAProblemExImpl,
                                    ComponentErrors::CouldntGetAttributeExImpl)
{
	ACSErr::Completion_var comp;
    ACS::ROlong_var isLockedRef1;
    ACS::ROlong_var isLockedRef2;
    CORBA::Long isLocked1;
    CORBA::Long isLocked2;
    if ( CORBA::is_nil(m_loDev_1st) && CORBA::is_nil(m_loDev_2nd) )
        return false;
    try {      
        if(!CORBA::is_nil(m_loDev_1st))
            isLockedRef1=m_loDev_1st->isLocked();
        else isLocked1= false;
        if(!CORBA::is_nil(m_loDev_2nd))
            isLockedRef2=m_loDev_2nd->isLocked();
        else isLocked2= false;
    }
    catch (CORBA::SystemException& ex) {
        m_mixer_fault=true;
        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"MixerOperator::checkLocalOscillator()");
        impl.setName(ex._name());
        impl.setMinor(ex.minor());
        throw impl;
    }
    if(!CORBA::is_nil(m_loDev_1st))
        isLocked1=isLockedRef1->get_sync(comp.out());
    if(!CORBA::is_nil(m_loDev_2nd))
        isLocked2=isLockedRef2->get_sync(comp.out());
    ACSErr::CompletionImpl complImpl(comp);
    if (!complImpl.isErrorFree()) {
        _EXCPT(ComponentErrors::CouldntGetAttributeExImpl,impl,"MixerOperator::isDeviceLocked()");
        impl.setAttributeName("isLocked");
        /**@todo to be improved reporting 1 oscillator */
        impl.setComponentName((const char*)m_configuration->getLocalOscillatorInstance1st());
        throw impl;
    }
    return isLocked1 || isLocked2;	
}
