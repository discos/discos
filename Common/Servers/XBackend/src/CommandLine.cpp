// $Id: CommandLine.cpp,v 1.41 2010/07/22 10:04:31 bliliana Exp $

#include <LogFilter.h>
#include "CommandLine.h"
#include "Common.h"
#include "math.h"
#include "Specifiche.h"

#define DECODE_ERROR -100

_IRA_LOGFILTER_IMPORT;

CCommandLine::CCommandLine(GroupSpectrometer *groupS):	
m_GroupSpectrometer(groupS)
{
	AUTO_TRACE("CCommandLine::CCommandLine()");
	m_pLink=new CSecureArea<GroupSpectrometer>(groupS);
	m_bTimedout=false;
	m_Error.Reset();
	m_configuration=NULL;
	m_backendStatus=0;
	setStatus(CNTD);//NOTCNTD
	m_busy=false;
	m_pcontrolLoop=NULL;
}

CCommandLine::~CCommandLine()
{
	AUTO_TRACE("CCommandLine::~CCommandLine()");

//	// if the backend is tranfering data...make a try to inform the backend before closing the connection
	if (getIsBusy()) {
		stopDataAcquisition();
	}

}

void CCommandLine::setControlThread(XarcosThread * controlLoop)
{
	m_pcontrolLoop=controlLoop;
}

void CCommandLine::InitConf(CConfiguration *config) throw (ComponentErrors::ValidationErrorExImpl)
{
	AUTO_TRACE("CCommandLine::InitConf()");
	m_configuration=config;

	if (!initializeConfiguration(m_configuration->getConfiguration())) {
		IRA::CString msg("");
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::InitConf()");
		msg.Format("the requested configuration %s is not known",(const char *)m_configuration->getConfiguration());
		impl.setReason((const char *)msg);
		throw impl;
	}
}

void CCommandLine::getInputSection(ACS::longSeq& inpSection) const
{
	inpSection.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		inpSection[i]=m_inputSection[i];
	}
}

void CCommandLine::getZeroTPI(DWORD *tpi) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
		BackendsErrors::BackendBusyExImpl)
{
	m_pcontrolLoop->getTpZero(tpi);
}

void CCommandLine::stopDataAcquisition() throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
		XBackendsErrors::NoSettingExImpl)
{
	AUTO_TRACE("CCommandLine::stopDataAcquisition()");	
	if (!getIsBusy()) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::stopDataAcquisition()");
		impl.setReason("transfer job cannot be stopped in this configuration");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::stopDataAcquisition()");
	}
	checkBackend();//aggiorno lo stato backend
	if((m_backendStatus & (1 <<DATAREADY))){
		m_pcontrolLoop->setStop(true);
	}else if((m_backendStatus & (1 << ACTIVE))){
		m_pcontrolLoop->AbortInt();	
		m_pcontrolLoop->setStop(true);
	}else {
		_THROW_EXCPT(XBackendsErrors::NoSettingExImpl,"CCommandLine::resumeDataAcquisition()");
	}
	ACS_LOG(LM_FULL_INFO,"CCommandLine::stopDataAcquisition()",(LM_INFO,"TRANSFER_JOB_STOPPED"));
	m_busy=false;
}

void CCommandLine::startDataAcquisition() 
	throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConnectionExImpl,XBackendsErrors::NoSettingExImpl)
{
	AUTO_TRACE("CCommandLine::startDataAcquisition()");	
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::startDataAcquisition()");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::startDataAcquisition()");
	}
	//m_backendStatus=(ACS::pattern)groupS->getBackendStatus();
	m_pcontrolLoop->Init();//Configuro Specifiche nell'HW
	
	checkBackend();//aggiorno lo stato backend
	if((m_backendStatus & (1 << SETTING))){
		m_busy=true;
	}else{
		_THROW_EXCPT(XBackendsErrors::NoSettingExImpl,"CCommandLine::startDataAcquisition()");
	}
	getConfiguration();//Aggiorno la Configurazione dell'HW
}

void CCommandLine::resumeDataAcquisition(const ACS::Time& startT) 
	throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,XBackendsErrors::NoSettingExImpl)
{

	AUTO_TRACE("CCommandLine::resumeDataAcquisition()");
	if ( !getIsBusy()) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::resumeDataAcquisition()");
		impl.setReason("transfer job cannot be resumed in present configuration");
		throw impl;
	}
	// check that the backend latency in preparing the data transfer is respected......
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::resumeDataAcquisition()");
	}
	checkBackend();//aggiorno lo stato backend
	if((m_backendStatus & (1 << SETTING))){
		m_pcontrolLoop->StartInt();	
		m_pcontrolLoop->setStop(false);
	}else{
		_THROW_EXCPT(XBackendsErrors::NoSettingExImpl,"CCommandLine::resumeDataAcquisition()");
	}

}

void CCommandLine::suspendDataAcquisition() throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
		XBackendsErrors::NoSettingExImpl)
{

	AUTO_TRACE("CCommandLine::suspendDataAcquisition()");
	if (!getIsBusy()) { //not suspended....running
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::suspendDataAcquisition()");
		impl.setReason("transfer job cannot be suspended in present configuration");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::suspendDataAcquisition()");
	}
	checkBackend();//aggiorno lo stato backend
	if((m_backendStatus & (1 <<DATAREADY))){
		m_pcontrolLoop->setStop(true);
	}else if((m_backendStatus & (1 << ACTIVE))){
	//	m_pcontrolLoop->AbortInt();	
		m_pcontrolLoop->setStop(true);
	}else {
		_THROW_EXCPT(XBackendsErrors::NoSettingExImpl,"CCommandLine::resumeDataAcquisition()");
	}
	ACS_LOG(LM_FULL_INFO,"CCommandLine::suspendDataAcquisition()",(LM_INFO,"TRANSFER_JOB_SUSPENDED"));

}

void CCommandLine::setAttenuation(const long&secId, const double& attenuation) 
	throw (ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::ValidationErrorExImpl,
		BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl,XBackendsErrors::ErrorConfigurationExImpl)
{
	AUTO_TRACE("CCommandLine::setAttenuation()");
	double newAtt;
	CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	HeadResult temp;
	temp.SetZeroHeadResult();
	long c=0;
	
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setAttenuation()");
		throw impl;
	}
	if (secId>=0) {
		if (secId>=m_sectionsNumber) {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setAttenuation()");
			impl.setReason("the input identifier is out of range");
			throw impl;
		}
	}
	if (attenuation>=0) { // // the user ask for a new value
		if  (attenuation<MIN_ATTENUATION) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setAttenuation()");
			impl.setValueName("attenuation");
			impl.setValueLimit(MIN_ATTENUATION);
			throw impl;						
		}
		else if (attenuation>MAX_ATTENUATION) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setAttenuation()");
			impl.setValueName("attenuation");
			impl.setValueLimit(MAX_ATTENUATION);
			throw impl;									
		}
		newAtt=attenuation;
	}
	else{
		return;//non cambia niente
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setAttenuation()");
	}	
	if(m_sectionsNumber!=groupS->Xspec.GetNSezioni()){
		_THROW_EXCPT(XBackendsErrors::ErrorConfigurationExImpl,"CCommandLine::setAttenuation()");
	}
	if (secId>=0) {	
		temp=groupS->Xspec.specificaSezione[secId];
		temp.SetGain(attenuationToGain(newAtt));
		temp.Setindex(secId);			
		groupS->Xspec.specificaSezione[secId]=temp;
	}
	else{
		for(long i=0;i<groupS->Xspec.GetNSezioni();i++){
			temp=groupS->Xspec.specificaSezione[i];
			temp.SetGain(attenuationToGain(newAtt));
			temp.Setindex(i);			
			groupS->Xspec.specificaSezione[i]=temp;
		}
	}	
	c=0;
	for (long i=0;i<m_sectionsNumber;i++,c++) {
		temp=groupS->Xspec.specificaSezione[i];
		if (m_polarization[i]==Backends::BKND_FULL_STOKES) {
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
			c++;
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
		}
		else {
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
		}
	}
	if(m_inputsNumber!=c){
		_THROW_EXCPT(XBackendsErrors::ErrorConfigurationExImpl,"CCommandLine::setConfiguration()");
	}
}

void CCommandLine::setConfiguration(const long& secId,const double& freq,const double& bw,
		const long& fd,const long& pol, const double& sr,const long& bins) 
	throw (ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::ValidationErrorExImpl,
			BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl,
			XBackendsErrors::ErrorConfigurationExImpl,XBackendsErrors::DisableChInExImpl)
{
	AUTO_TRACE("CCommandLine::setConfiguration()");
	CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	CString str("");
	long newBins=-1,newPol=-1,newFd=-1;
	double	newFreq=-1,newBW=-1,newSR=-1;
	long c=0;
	HeadResult temp;
	temp.SetZeroHeadResult();

	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setConfiguration()");
		throw impl;
	}
	if (secId>=0) {
		if (secId>=m_sectionsNumber){
			_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setReason("the input identifier is out of range");
			throw impl;
		}
	}
	else {
		//if the configuration involves all the inputs all the salient value must be given
		if ((bins*pol*fd*freq*bw*sr)==1)return;		
		if (fd>=0) {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setReason("feed value can't be change in the broadcast configuration");
			throw impl;
		}
	}
	if (bw>=0) { // the user ask for a new value
		if (bw<MIN_BANDWIDTH)  {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("bandWidth");
			impl.setValueLimit(MIN_BANDWIDTH);
			throw impl;			
		}
		else if (bw>MAX_BANDWIDTH) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("bandWidth");
			impl.setValueLimit(MAX_BANDWIDTH);
			throw impl;						
		}
		newBW=bw;
	}
	else { // else keep the present value
		if (secId>=0) newBW=m_bandWidth[secId];
		else newBW=bw;
	}
    if (sr>=0) {// the user ask for a new value
		if (sr>DEFAULT_SAMPLERATE) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("sampleRate");
			impl.setValueLimit(DEFAULT_SAMPLERATE);
			throw impl;			
		}
		newSR=sr;
	}
	else {
		if (secId>=0) {
            newSR=m_sampleRate[secId];
            ACS_DEBUG("CCommandLine::setConfiguration()","SAMPLE_RATE no change");
        }
		if(newBW!=-1){
			newSR=2*newBW;
			ACS_DEBUG("CCommandLine::setConfiguration()","newSR=2*newBW");
		}
		else {
			newSR=-1;
			ACS_DEBUG("CCommandLine::setConfiguration()","SAMPLE_RATE no change");
		}
	}
	if (fd>=0) { // the user ask for a new value
		if (fd>=m_beams) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("feed");
			impl.setValueLimit(m_beams);
			throw impl;						
		}
		newFd=fd;
	}
	else {
		if (secId>=0) newFd=m_feedNumber[secId];
		else newFd=-1;
	}
	if(newFd!=-1) {
		int chIn=searchChIn(newFd);
		if(!(m_adc[chIn])) {
			_THROW_EXCPT(XBackendsErrors::DisableChInExImpl,"CCommandLine::setConfiguration()");
		}
	}
	if (bins>=0) { // the user ask for a new value
		//if (bins>=DEFAULT_BINS) {
		if (bins>DEFAULT_BINS) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("attenuation");
			impl.setValueLimit(DEFAULT_BINS);
			throw impl;						
		}
		//newBins=bins;
		newBins=DEFAULT_BINS;
		ACS_DEBUG("CCommandLine::setConfiguration()","bins=DEFAULT_BINS");
	}
	else {
		if (secId>=0) newBins=m_bins[secId];
		else newBins=DEFAULT_BINS;
	}	
	if (freq>=0) { // the user ask for a new value		
		if (freq<MIN_FREQUENCY)  {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("frequency");
			impl.setValueLimit(MIN_FREQUENCY);
			throw impl;			
		}
		if (freq>=MAX_FREQUENCY) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("frequency");
			impl.setValueLimit(MAX_FREQUENCY);
			throw impl;						
		}
		newFreq=freq;
	}
	else {
		if (secId>=0) newFreq=m_frequency[secId];
		else newFreq=freq;
	}
	if (pol>=0) { // the user ask for a new value
		if (pol>2) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("polarization");
			impl.setValueLimit(2);
			throw impl;						
		}
		newPol=pol;
	}
	else {
		if (secId>=0) newPol=m_polarization[secId];
		else newPol=pol;
	}	
	if ((newBins*newPol*newFd*newFreq*newBW*newSR)==1)return;
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setConfiguration()");
	}
	if(m_sectionsNumber!=groupS->Xspec.GetNSezioni()){
		_THROW_EXCPT(XBackendsErrors::ErrorConfigurationExImpl,"CCommandLine::setConfiguration()");
	}
	if (secId>=0) {	
		int chIn=searchChIn(newFd);
		temp=groupS->Xspec.specificaSezione[secId];
		temp.SetBanda(newBW*1e6);
		if(newBW==MAX_BANDWIDTH) temp.SetFlo(0);
		else temp.SetFlo((newFreq-MIN_FREQUENCY+newBW*0.5)*1e6-62.5e6);
		if (newPol==Backends::BKND_FULL_STOKES){
			temp.SetModoPol(true);
			temp.SetAdc(chIn);
		}
		else {
			temp.SetModoPol(false);
			if (newPol==Backends::BKND_LCP) temp.SetAdc(chIn);						
			else temp.SetAdc(chIn+1);
		}
		temp.Setindex(secId);	
		groupS->Xspec.specificaSezione[secId]=temp;
		m_bandWidth[secId]=newBW;
		m_sampleRate[secId]=newSR;
		m_frequency[secId]=newFreq;
		m_bins[secId]=newBins;
		m_feedNumber[secId]=newFd;
		m_polarization[secId]=(Backends::TPolarization)newPol;
	}
	else {
		double oldFreq=-1;
		for(long i=0;i<groupS->Xspec.GetNSezioni();i++){
			oldFreq=-1;
			temp=groupS->Xspec.specificaSezione[i];		
//			ACS_LOG(LM_FULL_INFO,"ELSE CCommandLine::setConfiguration()",(LM_NOTICE,"DEVICE_CONFIGURED ALL,BW=%lf,SR=%lf,ATT=%lf",newBW,newSR,newAtt));		
			if(newBW!=-1){				
				oldFreq=MIN_FREQUENCY+(62.5e6+temp.GetFlo()-temp.GetBanda()*0.5)*1e-6;
				temp.SetBanda(newBW*1e6);
			}
			if(newBW==MAX_BANDWIDTH) temp.SetFlo(0);
			else{
				if(newFreq!=-1) temp.SetFlo((newFreq-MIN_FREQUENCY)*1e6+temp.GetBanda()*0.5-62.5e6); 
				else if(oldFreq!=-1)temp.SetFlo((oldFreq-MIN_FREQUENCY)*1e6+temp.GetBanda()*0.5-62.5e6);
			}
			if(newPol!=-1){
				newFd=searchFeed(temp.GetAdc());
				int chIn=searchChIn(newFd);
				if(newPol==Backends::BKND_FULL_STOKES){
					temp.SetModoPol(true);
					temp.SetAdc(chIn);
				}
				else {
					temp.SetModoPol(false);
					if (newPol==Backends::BKND_LCP){
						temp.SetAdc(chIn);						
					}else{
						temp.SetAdc(chIn+1);
					}
				}
			}
			temp.Setindex(i);
			groupS->Xspec.specificaSezione[i]=temp;
			if(newBW!=-1)m_bandWidth[i]=newBW;
			if(newSR!=-1)m_sampleRate[i]=newSR;
			if(newFreq!=-1)m_frequency[i]=newFreq;
			if(newBins!=-1)m_bins[i]=newBins;
			if(newFd!=-1)m_feedNumber[i]=newFd;
			if(newPol!=-1)m_polarization[i]=(Backends::TPolarization)newPol;
		}
	}
	c=0;
	for (long i=0;i<m_sectionsNumber;i++,c++) {
		temp=groupS->Xspec.specificaSezione[i];
		if (m_polarization[i]==Backends::BKND_FULL_STOKES) {
			m_inputSection[c]=i;
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
			c++;
			m_inputSection[c]=i;
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
		}
		else {
			m_inputSection[c]=i;
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
		}
	}
	m_inputsNumber=c;
	ACS_LOG(LM_FULL_INFO,"IF CCommandLine::setConfiguration()",(LM_NOTICE," DEVICE_CONFIGURED ALL,secId=%lf,FD=%lf,ChIn=%lf",secId,newFd,temp.GetAdc()));
}

void CCommandLine::setDefaultConfiguration() 
	throw (XBackendsErrors::ErrorConfigurationExImpl)
{
	AUTO_TRACE("CCommandLine::setDefaultConfiguration()");
	CString str("");
	
	HeadResult temp;
	long c=0;
	CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	
	temp.SetZeroHeadResult();
	groupS->Xspec.SetModo8bit(m_mode8bit);
	groupS->Xspec.SetTempoIntegrazione(m_integration);
	if(groupS->Xspec.GetNSezioni()<m_sectionsNumber){
		for(long i=(groupS->Xspec.GetNSezioni());i<m_sectionsNumber;i++){
			groupS->Xspec.SetNSezioni((groupS->Xspec.GetNSezioni()+1));
			groupS->Xspec.InsertElementArray();
		}
	}
	else{
		for(long i=m_sectionsNumber;i<(groupS->Xspec.GetNSezioni());i++){
			groupS->Xspec.specificaSezione[i].SetZeroHeadResult();
		}
		groupS->Xspec.SetNSezioni(m_sectionsNumber);
	}
	groupS->Xspec.SetNCicli(0);
	c=0;
	int chIn;
	for(long i=0;i<m_sectionsNumber;i++){			
		temp.SetBanda(m_bandWidth[i]*1e6);
		if(m_bandWidth[i]==MAX_BANDWIDTH)
            temp.SetFlo(0);
		else 
            temp.SetFlo((m_frequency[i]-MIN_FREQUENCY+m_bandWidth[i]*0.5)*1e6-62.5e6);
		if (m_polarization[i]==Backends::BKND_FULL_STOKES){
			temp.SetModoPol(true);				
			chIn=searchChIn(m_feedNumber[i]);
			temp.SetAdc(chIn);
			temp.SetGain(attenuationToGain(m_attenuation[c]));
			c++;
		}
		else {
			temp.SetModoPol(false);
			chIn=searchChIn(m_feedNumber[i]);
			temp.SetAdc(chIn+m_polarization[i]);
			temp.SetGain(attenuationToGain(m_attenuation[c]));
			c++;
		}
		temp.Setindex(i);
		groupS->Xspec.specificaSezione[i]=temp;
	}
	m_inputsNumber=c;
	if(m_inputsNumber!=c){
		_THROW_EXCPT(XBackendsErrors::ErrorConfigurationExImpl,"CCommandLine::setConfiguration()");
	}	
	c=0;
	for (long i=0;i<m_sectionsNumber;i++,c++) {
		temp=groupS->Xspec.specificaSezione[i];
		if (m_polarization[i]==Backends::BKND_FULL_STOKES) {
			m_inputSection[c]=i;
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
			c++;
			m_inputSection[c]=i;
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
		}
		else {
			m_inputSection[c]=i;
			m_attenuation[c]=gainToAttenuation(temp.GetGain());
		}
	}
	m_inputsNumber=c;
	m_pcontrolLoop->Init();//Configuro Specifiche nell'HW		
	//if(groupS->section!=m_sectionsNumber){
		//m_sectionsNumber=groupS->section;
	//	ACS_DEBUG_PARAM("CCommandLine::setSectionsNumber()"," section %i ",groupS->section);;
	//}

}

void CCommandLine::setup(const char *conf) throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConfigurationErrorExImpl)
{
	AUTO_TRACE("CCommandLine::setup()");
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setup()");
		throw impl;
	}
	if (!initializeConfiguration(conf)) {
		_EXCPT(BackendsErrors::ConfigurationErrorExImpl,impl,"CCommandLine::setup()");
		throw impl;
	}	
	setDefaultConfiguration(); //could throw exceptions........
	ACS_LOG(LM_FULL_INFO,"CCommandLine::initializeConfiguration()",(LM_NOTICE,"BACKEND_INITIALIZED: %s",conf)); 
}


void CCommandLine::setEnabled(const ACS::longSeq& en) throw (XBackendsErrors::NoImplementedExImpl)
{
//	int bound;
//	if (getIsBusy()) {
//		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setEnabled()");
//		throw impl;
//	}
//	if ((long)en.length()>=m_sectionsNumber) {
//		bound=m_sectionsNumber;
//	}
//	else {
//		bound=en.length();
//	}
//	for (int i=0;i<bound;i++) {
//		if (en[i]>0) m_enabled[i]=true;
//		else if (en[i]==0) m_enabled[i]=false;
//	}	
	ACS_LOG(LM_FULL_INFO,"CCommandLine::setEnabled()",(LM_NOTICE,"not implemented"));
	_THROW_EXCPT(XBackendsErrors::NoImplementedExImpl,"CCommandLine::setEnabled()");
}

void CCommandLine::setIntegration(const long& integration)  throw (BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::setIntegration()");
	CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setIntegration()");
		throw impl;
	}
	if (integration>=0) {
		m_integration=integration;
		groupS->Xspec.SetTempoIntegrazione(integration);
		ACS_LOG(LM_FULL_INFO,"CCommandLine::setIntegration()",(LM_NOTICE,"INTEGRATION is now %ld (millisec)",m_integration));
	}
}

void CCommandLine::setSectionsNumber(const long& sectionsNumber)  
	throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl)
{
	AUTO_TRACE("CCommandLine::setSectionsNumber()");	
	int old=0,i=0;
	long j=0;
	HeadResult temp;

	CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setSectionsNumber()");
		throw impl;
	}		
	//if ((sectionsNumber<=0)&&(sectionsNumber>MAX_SECTION_NUMBER)) {
	if ((sectionsNumber<=0)||(sectionsNumber>MAX_SECTION_NUMBER)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setSectionsNumber()");
		impl.setReason("the sectionsNumber identifier is out of range");
		throw impl;
	}
	if (sectionsNumber>0){
		old=groupS->Xspec.GetNSezioni();
		j=searchChIn(m_feedNumber[old-1]/*+1*/); //TBC!!!!!
		j--;
		if (sectionsNumber>m_sectionsNumber){			
			for(i=m_sectionsNumber;i<sectionsNumber;i++){
				if (i>=old) {	
					groupS->Xspec.SetNSezioni((groupS->Xspec.GetNSezioni()+1));
					groupS->Xspec.InsertElementArray();
				}
				temp=groupS->Xspec.specificaSezione[i];
				temp.Setindex(i);
				temp.SetBanda(m_bandWidth[i]*1e6);
				if(m_bandWidth[i]==MAX_BANDWIDTH) temp.SetFlo(0);
				else temp.SetFlo((m_frequency[i]-MIN_FREQUENCY+m_bandWidth[i]*0.5)*1e6-62.5e6);
				temp.SetModoPol(false);			
				temp.SetGain(DEFAULT_GAIN);
				do{									
					j++;
					while(!(m_adc[j])){
						j++;
						if(j>=MAX_ADC_NUMBER) j=0;
					}
					m_feedNumber[i]=searchFeed(j);
				}while(m_feedNumber[i]==-1);
				temp.SetAdc(j);
				groupS->Xspec.specificaSezione[i]=temp;//le nuove sezioni devono essere non pol!!
			}
		}
		else {
			for(i=sectionsNumber;i<old;i++) groupS->Xspec.specificaSezione[i].SetZeroHeadResult();
		}
		m_sectionsNumber=sectionsNumber;
		groupS->Xspec.SetNSezioni(m_sectionsNumber);
		ACS_LOG(LM_FULL_INFO,"CCommandLine::setSectionsNumber()",(LM_NOTICE,"Section is now %ld ",m_sectionsNumber));
	}
	groupS->setting=false;
	m_pcontrolLoop->Init();//Inizializzo L'HW
	if(groupS->section!=m_sectionsNumber){
		m_sectionsNumber=groupS->section;
		ACS_DEBUG_PARAM("CCommandLine::setSectionsNumber()"," section %i ",groupS->section);;
	}
	getConfiguration();//Aggiorno la configurazione di ACS!!
}

void CCommandLine::setInputsNumber(const long& inputsNumber)  
	throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl)
{
	AUTO_TRACE("CCommandLine::setInputsNumber() *********DA IMPLEMENTARE!!!");
	m_inputsNumber=inputsNumber;
}

void CCommandLine::getAttenuation(ACS::doubleSeq& att) const
{
	AUTO_TRACE("CCommandLine::getAttenuation()");

	att.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		att[i]=m_attenuation[i];
	}
}

void CCommandLine::getFrequency(ACS::doubleSeq& freq) const
{	
	AUTO_TRACE("CCommandLine::getFrequency()");
	CSecAreaResourceWrapper<GroupSpectrometer> line=m_pLink->Get();

	freq.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		freq[i]=m_frequency[i];
	}
}

void CCommandLine::getBins(ACS::longSeq& bins) const
{
	AUTO_TRACE("CCommandLine::getBins()");

	bins.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		bins[i]=m_bins[i];
	}
}

void CCommandLine::getFeed(ACS::longSeq& feed) const
{
	AUTO_TRACE("CCommandLine::getFeed()");

	feed.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		feed[i]=m_feedNumber[i];
	}
}

void CCommandLine::getPolarization(ACS::longSeq& pol) const
{	
	AUTO_TRACE("CCommandLine::getPolarization()");
	pol.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		pol[i]=(long)m_polarization[i];
	}
}

void CCommandLine::getBandWidth(ACS::doubleSeq& bw) const
{
	AUTO_TRACE("CCommandLine::getBandWidth()");

	bw.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		bw[i]=m_bandWidth[i];
	}
}

void CCommandLine::getIFs(ACS::longSeq& ifs) const
{
	ifs.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		ifs[i]=m_ifNumber[i];
	}	
}

void CCommandLine::getBackendStatus(DWORD& status) 
{
	AUTO_TRACE("CCommandLine::getBackendStatus()");
	CSecAreaResourceWrapper<GroupSpectrometer> groupSpec=m_pLink->Get();

	status=groupSpec->getBackendStatus();
	m_backendStatus=(ACS::pattern)status;	
}

void CCommandLine::getSampleRate(ACS::doubleSeq& sr) const
{
	AUTO_TRACE("CCommandLine::getSampleRate()");

	sr.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		sr[i]=m_sampleRate[i];
	}
}

void CCommandLine::getTime(ACS::Time& tt) const
{
	AUTO_TRACE("CCommandLine::getTime()");
	
	tt=getTimeStamp(); 
	ACE_CString timeString=getStringifiedTimeStamp();
	//ACS_DEBUG_PARAM("CCommandLine::getTime()","timeString = %s",(const char*)timeString.c_str());
}

long CCommandLine::getInputsConfiguration(ACS::doubleSeq& freq,ACS::doubleSeq& bandWidth,ACS::longSeq& feed,ACS::longSeq& ifs/*Receivers::TPolarizationSeq& polarization*/)
{
	long inputs;
	ACS::longSeq pol;
	
	getFeed(feed);
	getFrequency(freq);
	getInputsNumber(inputs);
	getBandWidth(bandWidth);
    getIFs(ifs);
    /*
	getPolarization(pol);
	polarization.length(pol.length());
	for (unsigned i=0;i<pol.length();i++) {
		if (pol[i]==Backends::BKND_LCP) {
			polarization[i]=Receivers::RCV_LCP;
		}
		else {
			polarization[i]=Receivers::RCV_RCP;
		}
	}
    */
	return inputs;
}

void CCommandLine::fillMainHeader(Backends::TMainHeader& bkd)
{
	CSecAreaResourceWrapper<GroupSpectrometer> groupSpec=m_pLink->Get();
//	long chs=0;
//	int indice=-1, n=0;	
//	Data tmpD;
	
//Per mantenere traccia dei canali settati	
//	for(int i=0;i<m_sectionsNumber;i++) m_enabled[i]=false;
	
	// count the available channels.......
//	for (n=0;n<groupSpec->GetNumSpec();n++){
//		tmpD=groupSpec->dato[n];
//		indice=tmpD.GetIntestazioneCh1().Getindex();
//		if(indice!=-1){
//			chs++;
////			m_enabled[indice]=true;
//			if(tmpD.GetIntestazioneCh1().GetModoPol()==false) {
//				indice=tmpD.GetIntestazioneCh2().Getindex();
//				if(indice!=-1){
//					chs++;
////					m_enabled[indice]=true;
//				}
//			}
//		}	
//	}		
	ACS_DEBUG_PARAM("CCommandLine::fillMainHeader()","groupSpec->GetNumSpec(): %i",groupSpec->GetNumSpec());
	bkd.sections=groupSpec->section;//m_sectionsNumber
	ACS_DEBUG_PARAM("CCommandLine::fillMainHeader()","section: %i",groupSpec->section);
	bkd.beams=m_beams;
	bkd.integration=groupSpec->Xspec.GetTempoIntegrazione();
	bkd.sampleSize=SAMPLESIZE;	
	ACS_LOG(LM_FULL_INFO,"CCommandLine::fillMainHeader",(LM_INFO,"Main_OK"));
}

void CCommandLine::fillChannelHeader(Backends::TSectionHeader *chHr,const long& size)
{
	long index=0;
//	int S=0;
	CSecAreaResourceWrapper<GroupSpectrometer> groupSpec=m_pLink->Get();
	int indice=-1, n=0;	
	Data tmpD;
	HeadResult head;
	
	head.SetZeroHeadResult();
	// count the available channels.......
	//m_sectionsNumber
	for (n=0;n<groupSpec->GetNumSpec();n++){
		tmpD=groupSpec->dato[n];
		indice=tmpD.GetIntestazioneCh1().Getindex();
		if(indice!=-1){
			head=tmpD.GetIntestazioneCh1();
			index=indice;
			if (index<size) {
				chHr[index].id=index;
				chHr[index].bins=DEFAULT_BINS;
				chHr[index].bandWidth=head.GetBanda();
				chHr[index].frequency=MIN_FREQUENCY+(62.5e6+head.GetFlo()-(head.GetBanda())*0.5)*1e-6;		
				if(head.GetModoPol()){
					chHr[index].inputs=2;
					chHr[index].polarization=Backends::BKND_FULL_STOKES;
					chHr[index].attenuation[0]=gainToAttenuation(head.GetGain());
					chHr[index].attenuation[1]=gainToAttenuation(tmpD.GetIntestazioneCh2().GetGain());
				}
				else if((head.GetAdc()%2)==0){
					chHr[index].inputs=1;
					chHr[index].polarization=Backends::BKND_LCP;
					chHr[index].attenuation[0]=gainToAttenuation(head.GetGain());
					chHr[index].attenuation[1]=0;
				}
					else{ 
						chHr[index].inputs=1;
						chHr[index].polarization=Backends::BKND_RCP;
						chHr[index].attenuation[0]=0;
						chHr[index].attenuation[1]=gainToAttenuation(head.GetGain());						
					}
//				chHr[index].sampleRate=(1/(head.GetBanda()))*1e3;
//				if(indice<4) S=0;//DA VEDERE SE FUNZIONA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111
//				else S=4;
//				chHr[index].feed=searchFeed(head.GetAdc());
				chHr[index].feed=m_feedNumber[index];
				chHr[index].sampleRate=m_sampleRate[index];
				index++;
			}
			if(tmpD.GetIntestazioneCh1().GetModoPol()==false){
				indice=tmpD.GetIntestazioneCh2().Getindex();
				if(indice!=-1){
					head=tmpD.GetIntestazioneCh2();
					index=indice;
					if (index<size) {
						chHr[index].id=index;
						chHr[index].bins=DEFAULT_BINS;
						chHr[index].bandWidth=head.GetBanda();
						chHr[index].frequency=MIN_FREQUENCY+(62.5e6+head.GetFlo()-(head.GetBanda())*0.5)*1e-6;				
	
						chHr[index].inputs=1;			
						if((head.GetAdc()%2)==0){	
							chHr[index].attenuation[0]=gainToAttenuation(head.GetGain());
							chHr[index].attenuation[1]=0;
							chHr[index].polarization=Backends::BKND_LCP;
						}
						else{ 						
							chHr[index].attenuation[0]=0;
							chHr[index].attenuation[1]=gainToAttenuation(head.GetGain());
							chHr[index].polarization=Backends::BKND_RCP;
						}
//						chHr[index].sampleRate=(1/(head.GetBanda()))*1e3;
//						if(indice<4) S=0;//DA VEDERE SE FUNZIONA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111
//						else S=4;
//						chHr[index].feed=(head.GetAdc()*0.5+S);
						chHr[index].sampleRate=m_sampleRate[index];
						chHr[index].feed=m_feedNumber[index];
						index++;
					}					
				}
			}
		}	
	}		
	ACS_LOG(LM_FULL_INFO,"CCommandLine::fillChannelHeader",(LM_INFO,"OK"));
	
}

int CCommandLine::getConfiguration()
{//Ritorna ad ACS la configurazione di specifiche settata nell Hardware
	
	CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	HeadResult Htmp;
	long c=0;
	Htmp.SetZeroHeadResult();	
	m_backendStatus=(ACS::pattern)groupS->getBackendStatus();
	m_sectionsNumber=groupS->Xspec.GetNSezioni();
	m_integration=groupS->Xspec.GetTempoIntegrazione();	
	m_mode8bit=groupS->Xspec.GetModo8bit();
	c=0;
	for (int i=0;i<m_sectionsNumber;i++,c++) {
		Htmp=groupS->Xspec.specificaSezione[i];
		if(Htmp.Getindex()!=-1){
			if(Htmp.GetGain()!=0) m_attenuation[i]=gainToAttenuation(Htmp.GetGain());
			else m_attenuation[i]=50;
			if(Htmp.GetBanda()==125e6)
				m_frequency[i]=MIN_FREQUENCY;
			else 
				m_frequency[i]=MIN_FREQUENCY+(62.5e6+Htmp.GetFlo()-(Htmp.GetBanda())*0.5)*1e-6;
			m_bandWidth[i]=(Htmp.GetBanda())*1e-6;
			if(Htmp.GetModoPol()) {
				m_polarization[i]=Backends::BKND_FULL_STOKES;
				m_inputSection[c]=i;
				m_attenuation[c]=gainToAttenuation(Htmp.GetGain());
				c++;				
				m_inputSection[c]=i;
				m_attenuation[c]=gainToAttenuation(Htmp.GetGain());
			}
			else if((Htmp.GetAdc()%2)==0){
				m_polarization[i]=Backends::BKND_LCP;			
				m_inputSection[c]=i;
				m_attenuation[c]=gainToAttenuation(Htmp.GetGain());

			}
			else {
				m_polarization[i]=Backends::BKND_RCP;		
				m_inputSection[c]=i;
				m_attenuation[c]=gainToAttenuation(Htmp.GetGain());
			}
			//m_sampleRate[i]=(1/(Htmp.GetBanda()))*1e3;//m_sampleRate[i]=(1/(2*Htmp.GetBanda()))*1e3;//*2(millisec)
		}
	}
	m_inputsNumber=c;	
	CString str("");
	str.Concat("************getConfiguration ");	
	str.Concat("m_sectionsNumber=");
	str.Concat((int)m_sectionsNumber);		
	str.Concat("     m_inputsNumber=");
	str.Concat((int)m_inputsNumber);		
//	str.Concat("     bw=");
//	str.Concat((double)m_bandWidth[i]);
//	str.Concat("    att=");
//	str.Concat((double)m_attenuation[i]);		
//	str.Concat("   Gain=");
//	str.Concat((double)temp.GetGain());		
//	str.Concat("     sr=");
//	str.Concat((double)m_sampleRate[i]);	
	str.Concat(" \n");
	std::cout << str << std::endl;
	return 1;//OK
}

DWORD CCommandLine::checkBackend()
{
	DWORD ptrn;
	
	getBackendStatus(ptrn);
	return ptrn;
}

bool CCommandLine::checkConnection()
{
//	int Res;
	CError Tmp;
	CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	if(groupS->isConnected()==false) setStatus(NOTCNTD);
	if (getStatus()==CNTD)  {
		return true;
	}
	else if (getStatus()==CNTDING) {
		ACS_DEBUG_PARAM("CCommandLine::checkConnection()","SOCKET_RECONNECTED Status: %s"," CNTDING");
		ACS_LOG(LM_FULL_INFO,"CCommandLine::checkConnection()",(LM_WARNING,"CANNOT_VALIDE_STATUS"));
		return false;
	}
	else {  		
		if(groupS->reConnect()){ 
			setStatus(CNTD);
			ACS_LOG(LM_FULL_INFO,"CCommandLine::checkConnection()",(LM_NOTICE,"SOCKET_RECONNECTED")); //we do not want to filter this info						
			return true;
		}
		return false;
	}
}

long CCommandLine::searchChIn(long feed)
{
	long i=0;
	
	if((feed<0)&&(feed>=m_beams)) return -1;
	else {
		while(!((i<MAX_ADC_NUMBER)&&(m_ChIn[i]==feed))){
			i++;
		}
	}
	if(i>=MAX_ADC_NUMBER) return -1;
	return i;
}

long CCommandLine::searchFeed(long ChIn)
{
	if((ChIn<0)&&(ChIn>=MAX_ADC_NUMBER)) {
        return -1;
    }
	return m_ChIn[ChIn];
}

bool CCommandLine::initializeConfiguration(const IRA::CString & config) 
{
//	bool ok=false;	
	long i=0;
	for( i=0;i<MAX_INPUT_NUMBER;i++) {
		m_attenuation[i]=gainToAttenuation(DEFAULT_GAIN);//DEFAULT_ATTENUATION;
		m_inputSection[i]=(long)(i/2);
		m_tsys[i]=0.0;	
		m_tpiZero[i]=0.0;	
	}
    m_sectionsNumber=DEFAULT_SECTION_NUMBER;
    if (!DEFAULT_POLARIZATION)
        m_sectionsNumber=DEFAULT_SECTION_NUMBER*2;

	for( i=0;i<m_sectionsNumber;i++) {
		m_bandWidth[i]= DEFAULT_BANDWIDTH;
		m_frequency[i]=DEFAULT_FREQUENCY;
		m_sampleRate[i]=DEFAULT_SAMPLERATE;
		if(DEFAULT_POLARIZATION) m_polarization[i]=Backends::BKND_FULL_STOKES;
		else if((i%2)==0) m_polarization[i]=Backends::BKND_LCP;
			else m_polarization[i]=Backends::BKND_RCP;
		m_bins[i]=DEFAULT_BINS;
		m_enabled[i]=true;		
	}

	m_integration=DEFAULT_INTEGRATION;
	m_inputsNumber=DEFAULT_SECTION_NUMBER*2;
	//m_sectionsNumber=DEFAULT_SECTION_NUMBER;
    //if (!DEFAULT_POLARIZATION)
        //m_sectionsNumber=DEFAULT_SECTION_NUMBER*2;
	m_beams=MAX_DEFAULT_BEAM;
	m_sampleSize=SAMPLESIZE;	
	
	if(m_beams*2>MAX_ADC_NUMBER) m_beams=MAX_ADC_NUMBER/2;
	for( i=0;i<m_beams*2;i++) { 
		m_ChIn[i]=(long)(i/2);//{0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,-1,-1};//Mapping Feed to ChIn
	}
	for( i=(m_beams)*2;i<(MAX_ADC_NUMBER);i++) m_ChIn[i]=-1;//-1 non è connesso
	if (DEFAULT_MODE8BIT){
		m_mode8bit=true;
#ifdef DOPPIO 
		for( i=0;i<MAX_ADC_NUMBER;i++) m_adc[i]=true;  //CDB  Decidere li quali ADC abilitare o meno
	//	m_adc[0]=m_adc[1]=m_adc[8]=m_adc[9]=false;     // 
#else 
		for( i=0;i<MAX_ADC_NUMBER;i++) m_adc[i]=true;  //
	//	m_adc[0]=m_adc[1]=false;                       //
#endif 	
	}
	else {
		m_mode8bit=false;
		for( i=0;i<MAX_ADC_NUMBER;i++) m_adc[i]=true;
	}


// AGGIUNGO MOMENTANEAMENTE
        bool ottobit=DEFAULT_MODE8BIT;
        CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	//oldMode=groupS->Xspec.GetModo8bit();
	//if(oldMode!=mode){     // TENTATIVO 4 DIC 2012
        //if (ottobit=true) { //AGGIUNTO 05/12/12
		groupS->Xspec.SetModo8bit(ottobit);
		groupS->AdcSetMode(groupS->Xspec.GetModo8bit());//Cambio la distribuzione 
        //} //AGGIUNTO 05/12/12
	//}	
	//else return;	

// *********************
	int j=0;
	i=0;
	//while(i<DEFAULT_SECTION_NUMBER){
	while(i<m_sectionsNumber){
		do{
			while(!(m_adc[j])) j++;
			m_feedNumber[i]=searchFeed(j);
			if(m_feedNumber[i]==-1) j++;
		}while((m_feedNumber[i]==-1)&&(j<MAX_ADC_NUMBER));
		i++;
		if(j==MAX_ADC_NUMBER) j=0;
		else j++;
	}

//	if (config=="22GHzMultiFeed") { //in order to add a new configuration add an other if

	return true;
}

void CCommandLine::getTsys(ACS::doubleSeq& tsys) const
{
	tsys.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		tsys[i]=m_tsys[i];
	}	
}

void CCommandLine::saveTsys(const ACS::doubleSeq& tsys)
{
	for (int i=0;i<m_inputsNumber;i++) {
		m_tsys[i]=tsys[i];
	}
}

void CCommandLine::Init() 
	throw (BackendsErrors::ConnectionExImpl,BackendsErrors::ConnectionExImpl,XBackendsErrors::NoSettingExImpl)
{
	if (!checkConnection()) {
	_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::Init()");
	}	
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::Init()");
		throw impl;
	}
	m_pcontrolLoop->Init();
	
	checkBackend();//aggiorno lo stato backend
	if((m_backendStatus & (1 << SETTING))){
	}else{
		_THROW_EXCPT(XBackendsErrors::NoSettingExImpl,"CCommandLine::Init()");
	}

}

void CCommandLine::VisualizzaDato() 
	throw (BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl)
{		
	if (!checkConnection()) {
	_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::VisualizzaDato()");
	}	
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::VisualizzaDato()");
		throw impl;
	}
	m_pcontrolLoop->VisualizzaDato();
}

void CCommandLine::VisualizzaSpecific() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl)
{		
	if (!checkConnection()) {
	_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::Specific()");
	}	
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::Specific()");
		throw impl;
	}
	m_pcontrolLoop->VisualizzaSpecific();
}

void CCommandLine::VisualizzaACS() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl)
{		
	if (!checkConnection()) {
	_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::VisualizzaACS");
	}	
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::VisualizzaACS");
		throw impl;
	}
	AUTO_TRACE("CCommandLine::VisualizzaACS");
	CString str("");
	int s=0,c=0;	
	
	str.Concat("\n\n ******* ACS *******");
	str.Concat("\n TempoIntegrazione = ");
	str.Concat((int)m_integration);
	str.Concat(" sec\n");
	str.Concat(" Modo8bit = "); 
	str.Concat(m_mode8bit);
	str.Concat("\n NSezioni = ");
	str.Concat((int)m_sectionsNumber);
	str.Concat("\n NInputs = ");
	str.Concat((int)m_inputsNumber);
	str.Concat("\n");
	while(s<m_sectionsNumber){		
		str.Concat("\n    Index = ");
		str.Concat(s);
		str.Concat("\n    inputSection = ");
		str.Concat((int)m_inputSection[s]);
		str.Concat(" Freq = ");
		str.Concat(m_frequency[s]);	
		str.Concat(" MHz    Attenuation = ");
		str.Concat(m_attenuation[s]);
		str.Concat("db    Banda =");
		str.Concat(m_bandWidth[s]);		
		str.Concat("MHz    Feed = "); 
		str.Concat((int)m_feedNumber[s]);
		str.Concat("    Sample Rate = ");
		str.Concat(m_sampleRate[s]);			
		str.Concat(" MHz    ");
		str.Concat("ModoPolarimetric = "); 
		str.Concat(m_polarization[s]);	
		if(m_polarization[s]==Backends::BKND_FULL_STOKES) c++;
		str.Concat("Bins = "); 
		str.Concat((int)m_bins[s]);	
		str.Concat("Enable = "); 
		str.Concat(m_enabled[s]);
		str.Concat(" \n");
		s++;c++;
	}	
	str.Concat("\n *******  *******");
	std::cout << str << std::endl;
}

void CCommandLine::getTpZero(ACS::doubleSeq& tpiA, bool zero) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getTpZero()");
	DWORD tpiZ[MAX_INPUT_NUMBER];
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getTpZero()");
	}	
	try {
		if(zero) m_pcontrolLoop->getTpZero(tpiZ);
		else m_pcontrolLoop->getTp(tpiZ);
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"CCommandLine::getTpZero()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
	tpiA.length(m_inputsNumber);
	if(zero){
		for (int j=0;j<m_inputsNumber;j++) {
			tpiA[j]=m_tpiZero[j]=tpiZ[j];  //it should already be normalized to 1 millisec of integration
	    }
	}
	else{
		for (int j=0;j<m_inputsNumber;j++) {
			tpiA[j]=tpiZ[j];  //it should already be normalized to 1 millisec of integration
	    }
	}
}

void CCommandLine::setMode8bit(bool mode)
	throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::setMode8bit()");
	bool oldMode;
	long i=0;
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setMode8bit()");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setMode8bit()");
	}	
	CSecAreaResourceWrapper<GroupSpectrometer> groupS=m_pLink->Get();
	oldMode=groupS->Xspec.GetModo8bit();
	if(oldMode!=mode){     
		groupS->Xspec.SetModo8bit(mode);
		groupS->AdcSetMode(groupS->Xspec.GetModo8bit());//Cambio la distribuzione 
	}	
	else return;  
	if (mode){
		m_mode8bit=true;
#ifdef DOPPIO 
		for( i=0;i<MAX_ADC_NUMBER;i++) m_adc[i]=true;
		m_adc[0]=m_adc[1]=m_adc[8]=m_adc[9]=false;
#else 
		for( i=0;i<MAX_ADC_NUMBER;i++) m_adc[i]=true;
		m_adc[0]=m_adc[1]=false;
#endif 	
	}
	else {
		m_mode8bit=false;
		for( i=0;i<MAX_ADC_NUMBER;i++) m_adc[i]=true;
	} 
	if(oldMode==false){//Check ChIn and Feed
		int j=0;
		i=0;
		while(i<MAX_SECTION_NUMBER){
			if(m_adc[searchChIn(m_feedNumber[i])]){
				i++;
			}
			else{
				j=searchChIn(m_feedNumber[i]);
				while(!(m_adc[j])&&(j<MAX_ADC_NUMBER)) j++;
				m_feedNumber[i]=searchFeed(j);
			}				
		}
	}
	setDefaultConfiguration();//Aggiorno la configurazione nell'HW
}

void CCommandLine::getSample(ACS::doubleSeq& tpi,bool zero) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getSample()");
//	int res;
//	//bool busy=getIsBusy();
//	long integration;
//	
//	if (!checkConnection()) {
//		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
//	}
//	tpi.length(m_inputsNumber);
//	for (int j=0;j<m_inputsNumber;j++) {
//		tpi[j]=m_tpiZero[j];  //it should already be normalized to 1 millisec of integration
//	}
//	integration=(long)round(1.0/ (m_commonSampleRate*1000.0));
//
//	res=getConfiguration(); // refresh the m_currentSampleRate..........
////	if (res>0) { // load OK
//		// do nothing
//	}
//	_CHECK_ERRORS("CommandLine::getSample()");
//	// if the Tpizero has been requested...than connect the backend to the 50ohm
//	if (zero) {
//		len=CProtocol::setZeroInput(sBuff,true); // get the buffer
//		if ((res=sendBuffer(sBuff,len))==SUCCESS) {
//			res=receiveBuffer(rBuff,RECBUFFERSIZE);
//		}
//		if (res>0) { // operation was ok.
//			if (!CProtocol::isAck(rBuff)) {
//				_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getSample()");
//			} 
//		}
//		else if (res==FAIL) {
//			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
//			dummy.setCode(m_Error.getErrorCode());
//			dummy.setDescription((const char*)m_Error.getDescription());
//			m_Error.Reset();
//			_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getSample()");
//		}
//		else if (res==WOULDBLOCK) {
//			_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getSample()");
//		}
//		else {
//			_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
//		}
//		waitTime=integration*2000;
//	}
//	//if the requested integration is not equal to the current samplerate (given is milliseconds as a sample period)
//	// or the integration time must be forced...and the backend is not busy. Then set the correct sample rate....
//	if (((integration!=m_currentSampleRate) || (m_setTpiIntegration)) && !busy) {
//		len=CProtocol::setIntegrationTime(sBuff,integration); // get the buffer
//		if ((res=sendBuffer(sBuff,len))==SUCCESS) {
//			res=receiveBuffer(rBuff,RECBUFFERSIZE);
//		}
//		if (res>0) { // operation was ok.
//			if (!CProtocol::isAck(rBuff)) {
//				_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getSample()");
//			} 
//		}
//		else if (res==FAIL) {
//			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
//			dummy.setCode(m_Error.getErrorCode());
//			dummy.setDescription((const char*)m_Error.getDescription());
//			m_Error.Reset();
//			_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getSample()");
//		}
//		else if (res==WOULDBLOCK) {
//			_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getSample()");
//		}
//		else {
//			_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
//		}
//		m_setTpiIntegration=false;
//		waitTime=1000000+integration*2000; // //wait time in microseconds...we should wait 2 seconds plus twice the integration time (which is given in milliseconds).
//	}
//	if (waitTime>0) IRA::CIRATools::Wait(waitTime); 
//	// now read the total power
//	len=CProtocol::getSample(sBuff); // get the buffer
//	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
//		res=receiveBuffer(rBuff,RECBUFFERSIZE);
//	}
//	if (res>0) { // operation was ok.
//		DWORD data[MAX_INPUT_NUMBER];
//		if (!CProtocol::decodeData(rBuff,data)) {
//			_THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"CCommandLine::getSample()");
//		}
//		tpi.length(m_inputsNumber);
//		for (int j=0;j<m_inputsNumber;j++) {
//			tpi[j]=(double)data[j]/(double)integration;
//			if (zero) m_tpiZero[j]=tpi[j]; // in case of tpiZero we store it......
//		}
//	}
//	else if (res==FAIL) {
//		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
//		dummy.setCode(m_Error.getErrorCode());
//		dummy.setDescription((const char*)m_Error.getDescription());
//		m_Error.Reset();
//		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getSample()");
//	}
//	else if (res==WOULDBLOCK) {
//		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getSample()");
//	}
//	else {
//		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
//	}
//	// now return to the default attenuation level
//	if (zero) {
//		len=CProtocol::setZeroInput(sBuff,false); // get the buffer
//		if ((res=sendBuffer(sBuff,len))==SUCCESS) {
//			res=receiveBuffer(rBuff,RECBUFFERSIZE);
//		}
//		if (res>0) { // operation was ok.
//			if (!CProtocol::isAck(rBuff)) {
//				_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getSample()");
//			} 
//		}
//		else if (res==FAIL) {
//			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
//			dummy.setCode(m_Error.getErrorCode());
//			dummy.setDescription((const char*)m_Error.getDescription());
//			m_Error.Reset();
//			_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getSample()");
//		}
//		else if (res==WOULDBLOCK) {
//			_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getSample()");
//		}
//		else {
//			_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
//		}
//	}
}

void CCommandLine::setStatus(TLineStatus sta)
{
	m_Linestatus=sta;
	if (m_Linestatus!=CNTD) {
		setStatusField(HW);
	}
	else {
		clearStatusField(HW);
	}
}
