// $Id: CommandLine.cpp,v 1.1 2011-03-14 15:16:22 a.orlati Exp $

#include "CommandLine.h"
#include "Protocol.h"

CCommandLine::CCommandLine()
{
	AUTO_TRACE("CCommandLine::CCommandLine()");
	m_configuration=NULL;
	m_backendStatus=0;
}

CCommandLine::~CCommandLine()
{
	AUTO_TRACE("CCommandLine::~CCommandLine()");
}

void CCommandLine::Init(CConfiguration *config) throw (ComponentErrors::ValidationErrorExImpl)
{
	AUTO_TRACE("CCommandLine::Init()");
	m_configuration=config;
	if (!initializeConfiguration()) {
		IRA::CString msg;
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::Init()");
		throw impl;
	}
}

void CCommandLine::stopDataAcquisition() throw (ComponentErrors::NotAllowedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (!getIsBusy()) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::stopDataAcquisition()");
		impl.setReason("transfer job cannot be stopped in this configuration");
		throw impl;
	}
	clearStatusField(CCommandLine::BUSY);
}

void CCommandLine::startDataAcquisition() throw (BackendsErrors::BackendBusyExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::startDataAcquisition()");
		throw impl;
	}
	setStatusField(CCommandLine::BUSY);
}

void CCommandLine::resumeDataAcquisition(const ACS::Time& startT) throw (ComponentErrors::NotAllowedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (!getIsBusy()) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::resumeDataAcquisition()");
		impl.setReason("transfer job cannot be resumed in present configuration");
		throw impl;
	}
}

void CCommandLine::suspendDataAcquisition() throw (ComponentErrors::NotAllowedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (!getIsBusy()) { 
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::resumeDataAcquisition()");
		impl.setReason("transfer job cannot be suspended in present configuration");
		throw impl;
	}
}

void CCommandLine::getSample(ACS::doubleSeq& tpi,bool zero) const
{
	AUTO_TRACE("CCommandLine::getSample()");
	tpi.length(m_inputsNumber);
	if (zero) {
		for (long i=0;i<m_inputsNumber;i++) {
			tpi[i]=0.0;
		}
	}
	else {
		for (long i=0;i<m_inputsNumber;i++) {
			tpi[i]=CProtocol::getRandomValue(m_attenuation[i]);
		}		
	}
}

void CCommandLine::setTime()
{
	AUTO_TRACE("CCommandLine::setTime()");
}

void CCommandLine::setEnabled(const ACS::longSeq& en) throw (BackendsErrors::BackendBusyExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	int bound;
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setEnabled()");
		throw impl;
	}
	if ((long)en.length()>=m_sectionsNumber) {
		bound=m_sectionsNumber;
	}
	else {
		bound=en.length();
	}
	for (int i=0;i<bound;i++) {
		if (en[i]>0) m_enabled[i]=true;
		else if (en[i]==0) m_enabled[i]=false;
	}	
	ACS_LOG(LM_FULL_INFO,"CCommandLine::setEnabled()",(LM_NOTICE,"CHANGED_ENABLED_CHANNEL"));
}

void CCommandLine::setConfiguration(const long& sectId,const double& freq,const double& bw,const long& feed,const long& pol,const double& sr,const long& bins) throw (
		ComponentErrors::ValidationErrorExImpl,BackendsErrors::BackendBusyExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setConfiguration()");
		throw impl;
	}	
	if ((sectId<0) && (sectId>=m_sectionsNumber)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setConfiguration()");
		impl.setReason("the section identifier is out of range");
		throw impl;
	}
	if (bw>=0) { // the user ask for a new value
		m_bandWidth[sectId]=bw;
	}
	if (sr>=0) {// the user ask for a new value
		m_sampleRate[sectId]=sr;
	}
	if (freq>=0) {
		m_frequency[sectId]=freq;
	}
	if (feed>=0) {
		if (feed>=m_feeds) {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setReason("the selected feed is not valid");
			throw impl;
		}
		else {
			m_feedNumber[sectId]=feed;
		}
	}		
	if (bins>=0) {
		m_bins[sectId]=bins;			
	}
	if (pol>=0) {
		m_polarization[sectId]=(Backends::TPolarization)pol;
		long c=0;
		for (long i=0;i<m_sectionsNumber;i++) {
			if (m_polarization[i]==Backends::BKND_FULL_STOKES) {
				m_inputSection[c]=i;
				c++;
				m_inputSection[c]=i;
				c++;
			}
			else {
				m_inputSection[c]=i;
				c++;
			}
		}
		m_inputsNumber=c;
	}
	IRA::CString temp;
	if (m_polarization[sectId]==Backends::BKND_LCP) {  
		temp="LCP";
	}
	else if (m_polarization[sectId]==Backends::BKND_RCP) {
		temp="RCP";
	}
	else {
		temp="FULL_STOKES";
	}
	ACS_LOG(LM_FULL_INFO,"CCommandLine::setConfiguration()",(LM_NOTICE,"SECTION_CONFIGURED %ld,FREQ=%lf,BW=%lf,FEED=%d,POL=%s,SR=%lf,BINS=%d",sectId,m_frequency[sectId],m_bandWidth[sectId],
			m_feedNumber[sectId],(const char *)temp,m_sampleRate[sectId],m_bins[sectId]));		
}


void CCommandLine::setIntegration(const long& integration) throw (BackendsErrors::BackendBusyExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setIntegration()");
		throw impl;
	}
	if (integration>=0) {
		m_integration=integration;
		ACS_LOG(LM_FULL_INFO,"CCommandLine::setIntegration()",(LM_NOTICE,"INTEGRATION is now %ld (millisec)",m_integration));
	}
}

void CCommandLine::setAttenuation(const long& inputId,const double& attenuation) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setAttenuation()");
		throw impl;
	}
	if ((inputId<0) && (inputId>=m_inputsNumber)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setAttenuation()");
		impl.setReason("the input identifier is out of range");
		throw impl;
	}
	if (attenuation>=0) { // // the user ask for a new value, if negative the previous valus is kept
		m_attenuation[inputId]=attenuation;
	}
}

long CCommandLine::getInputsConfiguration(ACS::doubleSeq& freq,ACS::doubleSeq& bandWidth,ACS::longSeq& feed,ACS::longSeq& ifs)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	freq.length(m_inputsNumber); //dealing with inputs not sections
	bandWidth.length(m_inputsNumber);
	feed.length(m_inputsNumber);
	ifs.length(m_inputsNumber);
	bool full=false;
	for (long i=0;i<m_inputsNumber;i++) {
		feed[i]=m_feedNumber[m_inputSection[i]];
		bandWidth[i]=m_bandWidth[m_inputSection[i]];
		freq[i]=m_frequency[m_inputSection[i]];
		if (m_polarization[m_inputSection[i]]==Backends::BKND_LCP) {
			ifs[i]=0;
		}
		else if (m_polarization[m_inputSection[i]]==Backends::BKND_RCP) {
			ifs[i]=1;
		}
		else { //the corresponding section is full_stokes
			if (!full) { 
				ifs[i]=0;
				full=true;
			}
			else {
				ifs[i]=1;
				full=false;
			}
		}
	}
	return m_inputsNumber;
}

void CCommandLine::getAttenuation(ACS::doubleSeq& att)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	att.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
			att[i]=m_attenuation[i];
	}
}

void CCommandLine::getFrequency(ACS::doubleSeq& freq)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	freq.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		freq[i]=m_frequency[i];
	}
}

void CCommandLine::getBackendStatus(DWORD& status)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	status=m_backendStatus;
}

void CCommandLine::getSampleRate(ACS::doubleSeq& sr)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	sr.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		sr[i]=m_sampleRate[i];
	}
}

void CCommandLine::getTsys(ACS::doubleSeq& tsys)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	tsys.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		tsys[i]=m_tsys[i];
	}	
}

void CCommandLine::getBins(ACS::longSeq& bins)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	bins.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		bins[i]=m_bins[i];
	}
}

void CCommandLine::getPolarization(ACS::longSeq& pol)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	pol.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		pol[i]=(long)m_polarization[i];
	}
}

void CCommandLine::getFeed(ACS::longSeq& feed) 
{
	baci::ThreadSyncGuard guard(&m_mutex);
	feed.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		feed[i]=m_feedNumber[i];
	}
}

void CCommandLine::getInputSection(ACS::longSeq& inpSection)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	inpSection.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		inpSection[i]=m_inputSection[i];
	}
}

void CCommandLine::getBandWidth(ACS::doubleSeq& bw)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	bw.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		bw[i]=m_bandWidth[i];
	}
}

void CCommandLine::getTime(ACS::Time& tt) const
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	tt=now.value().value;
}

void CCommandLine::fillMainHeader(Backends::TMainHeader& bkd)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	long chs=0;
	// count the available channels.......
	for(int i=0;i<m_sectionsNumber;i++) {
		if (m_enabled[i]) chs++;
	}
	bkd.sections=chs;
	bkd.beams=m_feeds;
	bkd.integration=m_integration;
	bkd.sampleSize=m_sampleSize;
}

void CCommandLine::fillChannelHeader(Backends::TSectionHeader *chHr,const long& size)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	long index=0;
	long c=0;
	for (int i=0;i<m_sectionsNumber;i++) {
		if (m_enabled[i]) {
			if (index<size) {
				chHr[index].id=i;
				chHr[index].bins=m_bins[i];
				chHr[index].polarization=m_polarization[i];
				chHr[index].bandWidth=m_bandWidth[i];
				chHr[index].frequency=m_frequency[i];
				if (m_polarization[i]==Backends::BKND_FULL_STOKES) { 
					chHr[index].attenuation[0]=m_attenuation[c];
					chHr[index].attenuation[1]=m_attenuation[c+1];
					chHr[index].IF[0]=0;
					chHr[index].IF[1]=1;
					c+=2;
					chHr[index].inputs=2;
				}
				else {
					chHr[index].attenuation[0]=m_attenuation[c];
					chHr[index].attenuation[1]=0.0;
					if (m_polarization[i]==Backends::BKND_LCP) {
						chHr[index].IF[0]=0;
					}
					else if (m_polarization[i]==Backends::BKND_RCP) {
						chHr[index].IF[0]=1;
					}
					c++;
					chHr[index].inputs=1;
				}				
				chHr[index].sampleRate=m_sampleRate[i];
				chHr[index].feed=m_feedNumber[i];
				index++;
			}
		}
		else {
			if (m_polarization[i]==Backends::BKND_FULL_STOKES) {
				c+=2;
			}
			else {
				c+=1;
			}
		}
	}
}

void CCommandLine::saveTsys(const ACS::doubleSeq& tsys)
{
	for (int i=0;i<m_inputsNumber;i++) {
		m_tsys[i]=tsys[i];
	}
}

// private methods

bool CCommandLine::initializeConfiguration() 
{
	// This will configure N spectrometric sections in full polarization mode attached to one single beam.
	for(long i=0;i<MAX_INPUT_NUMBER;i++) {
		m_attenuation[i]=DEFAULT_ATTENUATION;
		m_inputSection[i]=(long)(i/2);
		m_tsys[i]=0.0;
	}
	for(long i=0;i<MAX_SECTION_NUMBER;i++) {
		m_bandWidth[i]=DEFAULT_BANDWIDTH;
		m_frequency[i]=DEFAULT_FREQUENCY;
		m_sampleRate[i]=DEFAULT_SAMPLE_RATE;
		m_polarization[i]=DEFAULT_POLARIZATION;
		m_bins[i]=DEFAULT_BINS_NUMBER;
		m_enabled[i]=true;
		m_feedNumber[i]=0;		
	}
	m_integration=DEFAULT_INTEGRATION;
	m_inputsNumber=MAX_INPUT_NUMBER;
	m_sectionsNumber=MAX_SECTION_NUMBER;
	m_feeds=MAX_SECTION_NUMBER;
	m_sampleSize=SAMPLESIZE;	
	return true;
}
