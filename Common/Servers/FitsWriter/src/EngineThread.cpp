// $Id: EngineThread.cpp,v 1.15 2011-04-22 18:51:49 a.orlati Exp $

#include "EngineThread.h"
#include <LogFilter.h>
#include <Definitions.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <DateTime.h>
#include <SkySource.h>
#include "CommonTools.h"

using namespace IRA;
using namespace FitsWriter_private;

_IRA_LOGFILTER_IMPORT;

CEngineThread::CEngineThread(const ACE_CString& name,CSecureArea<CDataCollection> *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : 
				ACS::Thread(name,responseTime,sleepTime),m_dataWrapper(param)
{
	AUTO_TRACE("CEngineThread::CEngineThread()");
	m_fileOpened=false;
	m_ptsys=new double[256];
	m_receiversBoss=Receivers::ReceiversBoss::_nil();
	receiverBossError=false;
	m_antennaBoss=Antenna::AntennaBoss::_nil();
	antennaBossError=false;
}

CEngineThread::~CEngineThread()
{
	AUTO_TRACE("CEngineThread::~CEngineThread()");
	if (m_fileOpened) { 
#ifdef FW_DEBUG	
		m_file.close();
#else
		delete m_file; 
#endif
	}
	if (m_ptsys) {
		delete [] m_ptsys;
	}
	try {
		CCommonTools::unloadAntennaBoss(m_antennaBoss,m_service);
		CCommonTools::unloadReceiversBoss(m_receiversBoss,m_service);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_WARNING);
	}
}

void CEngineThread::onStart()
{
	AUTO_TRACE("CEngineThread::onStart()");
}

void CEngineThread::onStop()
{
	 AUTO_TRACE("CEngineThread::onStop()");
}
 
bool CEngineThread::checkTime(const ACS::Time& currentTime)
{
	CSecAreaResourceWrapper<CDataCollection> m_data=m_dataWrapper->Get();
	return (currentTime>(m_data->getFirstDumpTime()+getSleepTime()+m_timeSlice)); // gives the cache time to fill a little bit
}

bool CEngineThread::checkTimeSlot(const ACS::Time& slotStart)
{
	/*TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::TimeInterval slot=m_timeSlice; // gives the slot of time to finalize the job
	ACS::TimeInterval duration=now.value().value-slotStart;
	if (duration<slot) return true;
	else return false;*/
	return true;
}

bool CEngineThread::processData() 
{
	char *buffer; //pointer to the buffer that contains the real data
	char *bufferCopy; // pointer to the memory that has to be freed
	ACS::Time time;
	bool calOn;
	long pol,bins;
	long buffSize;
	double ra,dec;
	double az,el;
	bool tracking;
	double hum,temp,press;
	CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	if (!data->getDump(time,calOn,bufferCopy,buffer,tracking,buffSize)) return false;
	TIMEVALUE tS;
	tS.value(time);
#ifdef FW_DEBUG
	IRA::CString out;
	out.Format("%02d:%02d:%02d.%03d  %d ",tS.hour(),tS.minute(),tS.second(),tS.microSecond()/1000,calOn);
	m_file << out;
#else
	CFitsWriter::TDataHeader tdh;
	CDateTime tConverter(tS,data->getDut1()); 
	tdh.time=tConverter.getMJD();
#endif
	try {
		CCommonTools::getAntennaBoss(m_antennaBoss,m_service,m_config->getAntennaBossComponent(),antennaBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		_IRA_LOGFILTER_LOG_EXCEPTION(ex,LM_ERROR);
		data->setStatus(Management::MNG_FAILURE);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		data->setStatus(Management::MNG_FAILURE);
		antennaBossError=true;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CEngineThread::processData()");
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		data->setStatus(Management::MNG_FAILURE);
	}
	try {
		//integration is multiplied by 10000 because internally we have the value in millesec while the method requires 100ns.
		m_antennaBoss->getObservedEquatorial(time,data->getIntegrationTime()*10000,ra,dec);
		m_antennaBoss->getObservedHorizontal(time,data->getIntegrationTime()*10000,az,el);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		data->setStatus(Management::MNG_FAILURE);
		antennaBossError=true;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CEngineThread::processData()");
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		data->setStatus(Management::MNG_FAILURE);
	}
#ifdef FW_DEBUG
	out.Format("%f %f %f %f ",ra,dec,az,el);
	m_file << (const char *) out;
	if (tracking) {
		out.Format("yes ");
	}
	else {
		out.Format("no ");
	}
	m_file << (const char *) out;
	m_file << endl;
#else
	FitsWriter_private::getTsysFromBuffer(buffer,data->getInputsNumber(),m_ptsys);
	tdh.raj2000=ra;
	tdh.decj2000=dec;
	tdh.az=az;
	tdh.el=el;
	tdh.par_angle=IRA::CSkySource::paralacticAngle(tConverter,data->getSite(),az,el);
	tdh.derot_angle=0.333357887; /*********************** get it Now is fixed to 19.1 degreees*******************/
	tdh.flag_cal=calOn;
	tdh.flag_track=tracking;
	data->getMeteo(hum,temp,press);
	tdh.weather[0]=hum;
	tdh.weather[1]=temp;
	tdh.weather[2]=press;
	if (!m_file->storeAuxData(tdh,m_ptsys)) {
		_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::processData()");
		impl.setFileName((const char *)data->getFileName());
		impl.setError(m_file->getLastError());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		data->setStatus(Management::MNG_FAILURE);
	}
#endif
	for (int i=0;i<data->getSectionsNumber();i++) {
		bins=data->getSectionBins(i);
		pol=data->SectionPolNumber(i);
		switch (data->getSampleSize()) {
			case sizeof(BYTE2_TYPE): {
#ifdef FW_DEBUG
				BYTE2_TYPE channel[bins*pol];
				FitsWriter_private::getChannelFromBuffer<BYTE2_TYPE>(i,pol,bins,buffer,channel);
				out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ",i,pol,bins,data->getSampleSize());
				m_file << (const char *)out;				
				for (long j=0;j<pol*bins;j++) {
					out.Format("%d ",channel[j]);
					m_file << (const char *) out;
				}
				m_file << '\n';
#else
				BYTE2_TYPE channel[bins*pol];
				FitsWriter_private::getChannelFromBuffer<BYTE2_TYPE>(i,pol,bins,buffer,channel);
				if (!m_file->storeData(channel,bins*pol,i)) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::processData()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
					data->setStatus(Management::MNG_FAILURE);				
				}
#endif
				break;
			}
			case sizeof(BYTE4_TYPE): {
#ifdef FW_DEBUG
				BYTE4_TYPE channel[bins*pol];
				FitsWriter_private::getChannelFromBuffer<BYTE4_TYPE>(i,pol,bins,buffer,channel);
				out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ",i,pol,bins,data->getSampleSize());
				m_file << (const char *)out;				
				for (long j=0;j<bins*pol;j++) {
					out.Format("%10.5f, %d ",channel[j],channel[j]);
					m_file << (const char *) out;
				}
				m_file << '\n';
#else
				BYTE4_TYPE channel[bins*pol];
				FitsWriter_private::getChannelFromBuffer<BYTE4_TYPE>(i,pol,bins,buffer,channel);
				if (!m_file->storeData(channel,bins*pol,i)) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::processData()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
					data->setStatus(Management::MNG_FAILURE);
				}			
#endif
				break;	
			}
			default : { // it should be BYTE8_TYPE
#ifdef FW_DEBUG
				BYTE8_TYPE channel[bins*pol];
				FitsWriter_private::getChannelFromBuffer<BYTE8_TYPE>(i,pol,bins,buffer,channel);
				out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ",i,pol,bins,data->getSampleSize());
				m_file << (const char *)out;				
				for (long j=0;j<pol*bins;j++) {
					out.Format("%10.5lf ",channel[j]);
					m_file << (const char *) out;
				}
				m_file << '\n';
#else
				BYTE8_TYPE channel[bins*pol];
				FitsWriter_private::getChannelFromBuffer<BYTE8_TYPE>(i,pol,bins,buffer,channel);
				if (!m_file->storeData(channel,bins*pol,i)) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::processData()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
					data->setStatus(Management::MNG_FAILURE);						
				}		
#endif
			}
		}
	}
#ifndef FW_DEBUG
	m_file->add_row();
#endif
	delete [] bufferCopy;
	return true;
}

void CEngineThread::runLoop()
{
	TIMEVALUE now;
	IRA::CString filePath,fileName;
	CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	IRA::CIRATools::getTime(now); // it marks the start of the activity job
	if (data->isReset()) {
		if (m_fileOpened) {
			#ifdef FW_DEBUG
			m_file.close();
			#else
			delete m_file;
			m_file=NULL;
			#endif
			ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"FILE_CLOSED"));
			m_fileOpened=false;
		}
		data->haltResetStage();
		return;
	}
	if (data->isStart() && data->isReady() &&  data->isScanHeaderReady() && data->isSubScanHeaderReady()) { // //main headers are already saved and file has to be opened
		if (!m_fileOpened) {
			data->setStatus(Management::MNG_OK);
			// create the file and save main headers
			data->getFileName(fileName,filePath);
			if (!IRA::CIRATools::directoryExists(filePath)) {
				if (!IRA::CIRATools::makeDirectory(filePath)) {
					_EXCPT(ComponentErrors::FileIOErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)filePath);
					impl.log(LM_ERROR);
					data->setStatus(Management::MNG_FAILURE);
				}
				else {
					ACS_LOG(LM_FULL_INFO,"CEngineThread::runLoop()",(LM_NOTICE,"NEW_SCAN_FOLDER_CREATED: %s",(const char *)filePath));
				}
			}
#ifdef FW_DEBUG
			m_file.open((const char *)data->getFileName(),ios_base::out|ios_base::trunc);
			if (!m_file.is_open()) {
				_EXCPT(ComponentErrors::FileIOErrorExImpl,impl,"CEngineThread::runLoop()");
				impl.setFileName((const char *)data->getFileName());
				impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
				data->setStatus(Management::MNG_FAILURE);
			}
#else
			m_file = new CFitsWriter();
			m_file->setBasePath("");
			m_file->setFileName((const char *)data->getFileName());
			if (!m_file->create()) {
				_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
				impl.setFileName((const char *)data->getFileName());
				impl.setError(m_file->getLastError());
				impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
				data->setStatus(Management::MNG_FAILURE);
			}
#endif
			else {
#ifdef FW_DEBUG
				IRA::CString out;
				Backends::TMainHeader mH=data->getMainHeader();
				out.Format("Main - ch: %d , beams: %d, sampleSize: %d, integration: %d \n ",
					mH.sections,mH.beams,mH.sampleSize,mH.integration);
				m_file<< (const char *) out;
				Backends::TSectionHeader const *cH=data->getSectionHeader();
				for (int j=0;j<data->getSections();j++) {
					out.Format("channel id: %d, bins: %d , pol: %d, bandWidth: %lf, frequency: %lf, attenuationL: %lf, attenuationR: %lf"
						 "sampleRate: %lf, feed: %d \n",cH[j].id,cH[j].bins,cH[j].polarization,cH[j].bandWidth,
						 cH[j].frequency,cH[j].attenuation[0],cH[j].attenuation[1],cH[j].sampleRate,cH[j].feed);
					m_file<< (const char *) out;
				}
#else
				//get data from receivers boss
				collectReceiversData();
				//get the data from the antenna boss
				collectAntennaData();
				// now creates the file, the tables and the headers
				Backends::TMainHeader mH=data->getMainHeader();
				Backends::TSectionHeader const *cH=data->getSectionHeader();
				IRA::CString siteName;
				IRA::CString sourceName;
				double sourceRa,sourceDec,sourceVlsr;
				double azOff,elOff,raOff,decOff,lonOff,latOff;
				double dut1;
				long scanTag;
				long scanID,subScanID;
				IRA::CString scheduleName;
				IRA::CSite site;
				ACS::doubleSeq LocalOscillator;
				ACS::doubleSeq calib;
				ACS::longSeq polarizations;
				ACS::doubleSeq skyFreq,skyBw;
				ACS::doubleSeq fluxes;

				data->getSite(site,dut1,siteName);
				data->getLocalOscillator(LocalOscillator);
				data->getSkyBandwidth(skyBw);
				data->getSkyFrequency(skyFreq);
				data->getCalibrationMarks(calib);
				data->getSourceFlux(fluxes);
				data->getReceiverPolarization(polarizations);
				data->getSource(sourceName,sourceRa,sourceDec,sourceVlsr);
				data->getAntennaOffsets(azOff,elOff,raOff,decOff,lonOff,latOff);
				scanTag=data->getScanTag();
				scanID=data->getScanID();
				subScanID=data->getSubScanID();
				scheduleName=data->getScheduleName();

				if (!m_file->saveMainHeader(mH)) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Project_Name",(const char *)data->getProjectName(),"Name of the project")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Observer",(const char *)data->getObserverName(),"Name of the observer")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Antenna",(const char *)siteName,"Name of the station")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("SiteLongitude",site.getLongitude(),"Longitude of the site (radians)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("SiteLatitude",site.getLatitude(),"Latitude of the site (radians)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("SiteHeight",site.getHeight(),"Height of the site (meters)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Beams",mH.beams,"Number of beams")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Sections",mH.sections,"Total number of sections")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Sample Size",mH.sampleSize,"Number of bytes of a data")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Receiver Code",(const char *)data->getReceiverCode(),"Keyword that identifies the receiver")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Source",(const char *)sourceName,"Source identifier")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("RightAscension",sourceRa,"Source right ascension at J2000 (radians)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Declination",sourceDec,"Source declination at J2000 (radians)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Vlsr",sourceVlsr,"Source radial velocity")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Azimuth Offset",azOff,"Longitude offset in horizontal frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Elevation Offset",elOff,"Latitude offset in horizontal frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("RightAscension Offset",raOff,"Longitude offset in equatorial frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Declination Offset",decOff,"Latitude offset in equatorial frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("GalacticLon Offset",lonOff,"Longitude offset in galactic frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("GalacticLat Offset",latOff,"Latitude offset in galactic frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("ScanID",scanID,"Scan Identifier")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("SubScanID",subScanID,"Subscan Identifier")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("ScheduleName",(const char *)scheduleName,"name of the running schedule")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				if (scanTag>=0) {
					if(!m_file->setPrimaryHeaderKey("Scan Tag",scanTag,"Scan tag identifier")) {
						_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
						impl.setFileName((const char *)data->getFileName());
						impl.setError(m_file->getLastError());
						impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
						data->setStatus(Management::MNG_FAILURE);
					}
				}
				if (!m_file->saveSectionHeader(cH)) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				if (!m_file->addSectionTable(polarizations,LocalOscillator,skyFreq,skyBw,calib,fluxes)) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				if (!m_file->setSectionHeaderKey("Integration",mH.integration,"Integration time (milliseconds)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				CFitsWriter::TFeedHeader *feedH=data->getFeedHeader();
				if (!m_file->addFeedTable("FEED TABLE")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
				for (WORD j=0;j<data->getFeedNumber();j++) {
					if (!m_file->saveFeedHeader(feedH[j])) {
						_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
						impl.setFileName((const char *)data->getFileName());
						impl.setError(m_file->getLastError());
						impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
						data->setStatus(Management::MNG_FAILURE);
						j=data->getFeedNumber(); //exit the cycle
					}
				}
				if (!m_file->addDataTable()) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					data->setStatus(Management::MNG_FAILURE);
				}
#endif
				m_fileOpened=true;
				data->startRunnigStage();
				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_DEBUG,"RUNNING_FROM_NOW" ));
				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"FILE_OPENED %s",(const char *)data->getFileName()));
			}
		} // end !m_fileOpened
	}
	else if (data->isStop()) {
		ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_DEBUG,"STOPPING" ));
		//save all the data in the buffer an then finalize the file
		if (m_fileOpened) {
			while (processData());
#ifdef FW_DEBUG
		m_file.close();
#else
		//m_file->close();
		delete m_file; // file close called directly by the class destructor
		m_file=NULL;
#endif
		}
		data->haltStopStage();
		m_fileOpened=false;
		ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"FILE_FINALIZED"));
	}
	else if (data->isRunning()) { // file was already created.... then saves the data into it
		 // until there is something to process and
		// there is still time available.......
		if (m_fileOpened) {
			while (checkTime(now.value().value) && checkTimeSlot(now.value().value) && processData());
		}
	}
}
//}

void CEngineThread::collectAntennaData()
{
	CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	try {
		CCommonTools::getAntennaBoss(m_antennaBoss,m_service,m_config->getAntennaBossComponent(),antennaBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		data->setStatus(Management::MNG_WARNING);		
		m_antennaBoss=Antenna::AntennaBoss::_nil();
	}
	if (!CORBA::is_nil(m_antennaBoss)) {
		ACSErr::Completion_var comp;
		CORBA::Double ra=0.0,dec=0.0,vlsr=0.0;	
		CORBA::Double raOff=0.0,decOff=0.0,azOff=0.0,elOff=0.0,lonOff=0.0,latOff=0.0;
		IRA::CString sourceName="";
		try { //get the target name and parameters
			ACS::ROstring_var targetRef;
			CORBA::String_var target;
			ACS::ROdouble_var raRef,decRef,vlsrRef;
			ACS::ROdouble_var raOffRef,decOffRef,azOffRef,elOffRef,lonOffRef,latOffRef;
			//let's take the references to the attributes
			targetRef=m_antennaBoss->target();
			raRef=m_antennaBoss->targetRightAscension();
			decRef=m_antennaBoss->targetDeclination();
			vlsrRef=m_antennaBoss->targetVlsr();
			azOffRef=m_antennaBoss->azimuthOffset();
			elOffRef=m_antennaBoss->elevationOffset();
			raOffRef=m_antennaBoss->rightAscensionOffset();
			decOffRef=m_antennaBoss->declinationOffset();
			lonOffRef=m_antennaBoss->longitudeOffset();
			latOffRef=m_antennaBoss->latitudeOffset();
			target=targetRef->get_sync(comp.out());
			ACSErr::CompletionImpl targetCompl(comp);
			if (!targetCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,targetCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("target");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				sourceName="";
			}
			else {
				sourceName=(const char *)target;
			}
			ra=raRef->get_sync(comp.out());
			ACSErr::CompletionImpl raCompl(comp);
			if (!raCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,raCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("targetRightAscension");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				ra=0.0;
			}
			dec=decRef->get_sync(comp.out());
			ACSErr::CompletionImpl decCompl(comp);
			if (!decCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,decCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("targetDeclination");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				dec=0.0;
			}
			vlsr=vlsrRef->get_sync(comp.out());
			ACSErr::CompletionImpl vlsrCompl(comp);
			if (!vlsrCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,vlsrCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("targetVlsr");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				vlsr=0.0;
			}
			azOff=azOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl azOffCompl(comp);
			if (!azOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,azOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("azimuthOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				azOff=0.0;
			}
			elOff=elOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl elOffCompl(comp);
			if (!elOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,elOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("elevationOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				elOff=0.0;
			}
			raOff=raOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl raOffCompl(comp);
			if (!raOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,raOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("rightAscensionOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				raOff=0.0;
			}
			decOff=decOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl decOffCompl(comp);
			if (!decOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,decOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("declinationOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				decOff=0.0;
			}
			lonOff=lonOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl lonOffCompl(comp);
			if (!lonOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,lonOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("longitudeOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				lonOff=0.0;
			}
			latOff=latOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl latOffCompl(comp);
			if (!latOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,latOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("targetVlsr");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				latOff=0.0;
			}
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectAntennaData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			antennaBossError=true;
			sourceName="";
			ra=dec=vlsr=0.0;
		}
		data->setSource(sourceName,ra,dec,vlsr);
		data->setAntennaOffsets(azOff,elOff,raOff,decOff,lonOff,latOff);
		try { //get the estimated source fluxes
			ACS::doubleSeq_var fluxes;
			ACS::doubleSeq freqs;
			ACS::doubleSeq bw;
			data->getSkyFrequency(freqs);
			data->getSkyBandwidth(bw);
			for (unsigned i=0;i<freqs.length();i++) {
				freqs[i]+=bw[i]/2.0; //computes the central frequency;
			}
			m_antennaBoss->getFluxes(freqs,fluxes.out());
			data->setSourceFlux(fluxes);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectAntennaData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			antennaBossError=true;
			data->setSourceFlux();
		}
	}
}

void CEngineThread::collectReceiversData()
{
	CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	try {
		CCommonTools::getReceiversBoss(m_receiversBoss,m_service,m_config->getReceiversBossComponent(),receiverBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		data->setStatus(Management::MNG_WARNING);		
		m_receiversBoss=Receivers::ReceiversBoss::_nil();
	}
	if (!CORBA::is_nil(m_receiversBoss)) {
		ACSErr::Completion_var comp;
		try { //get the receiver code
			ACS::ROstring_var recvCodeRef;
			CORBA::String_var recvCode;
			recvCodeRef=m_receiversBoss->actualSetup();
			recvCode=recvCodeRef->get_sync(comp.out());
			ACSErr::CompletionImpl compImpl(comp);
			if (compImpl.isErrorFree()) {
				data->setReceiverCode((const char *)recvCode);
			}
			else {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,compImpl,"CEngineThread::collectReceiversData()");
				impl.setAttributeName("receiverCode");
				impl.setComponentName((const char *)m_config->getReceiversBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				data->setReceiverCode(IRA::CString(""));
			}
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			receiverBossError=true;
			data->setReceiverCode(IRA::CString(""));
		}
		try { //get the local oscillator
			ACS::ROdoubleSeq_var loRef;
			ACS::doubleSeq_var lo;
			loRef=m_receiversBoss->LO();
			lo=loRef->get_sync(comp.out());
			ACSErr::CompletionImpl compImpl(comp);
			if (compImpl.isErrorFree()) {
				data->setLocalOscillator(lo.in());
			}
			else {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,compImpl,"CEngineThread::collectReceiversData()");
				impl.setAttributeName("localOscillator");
				impl.setComponentName((const char *)m_config->getReceiversBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				data->setLocalOscillator();
			}							
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			receiverBossError=true;
			data->setLocalOscillator();							
		}
		try { //get the band width for each if
			ACS::ROdoubleSeq_var bwRef;
			ACS::doubleSeq_var bw;
			bwRef=m_receiversBoss->bandWidth();
			bw=bwRef->get_sync(comp.out());
			ACSErr::CompletionImpl compImpl(comp);
			if (compImpl.isErrorFree()) {
				data->setReceiverBandWidth(bw.in());
			}
			else {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,compImpl,"CEngineThread::collectReceiversData()");
				impl.setAttributeName("bandWidth");
				impl.setComponentName((const char *)m_config->getReceiversBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				data->setReceiverBandWidth();
			}							
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			receiverBossError=true;
			data->setReceiverBandWidth();							
		}			
		try { //get the band initial frequency for each IF of the receiver
			ACS::ROdoubleSeq_var ifreqRef;
			ACS::doubleSeq_var ifreq;
			ifreqRef=m_receiversBoss->initialFrequency();
			ifreq=ifreqRef->get_sync(comp.out());
			ACSErr::CompletionImpl compImpl(comp);
			if (compImpl.isErrorFree()) {
				data->setReceiverInitialFrequency(ifreq.in());
			}
			else {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,compImpl,"CEngineThread::collectReceiversData()");
				impl.setAttributeName("initialFrequency");	/**
				 * check if a directory exists
				 */
				bool DirectoryExists(const IRA::CString& path);
				impl.setComponentName((const char *)m_config->getReceiversBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				data->setReceiverInitialFrequency();
			}							
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			receiverBossError=true;
			data->setReceiverInitialFrequency();							
		}	
		try { //get the local polarization of each ifs
			ACS::ROlongSeq_var polRef;
			ACS::longSeq_var pol;
			polRef=m_receiversBoss->polarization();
			pol=polRef->get_sync(comp.out());
			ACSErr::CompletionImpl compImpl(comp);
			if (compImpl.isErrorFree()) {
				data->setReceiverPolarization(pol.in());
			}
			else {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,compImpl,"CEngineThread::collectReceiversData()");
				impl.setAttributeName("polarization");
				impl.setComponentName((const char *)m_config->getReceiversBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				data->setReceiverPolarization();
			}							
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			receiverBossError=true;
			data->setReceiverPolarization();							
		}
		try { //get the feeds geometry
			long inputs;
			ACS::doubleSeq_var xOff;
			ACS::doubleSeq_var yOff;
			ACS::doubleSeq_var power;
			inputs=(long)m_receiversBoss->getFeeds(xOff.out(),yOff.out(),power.out());
			CFitsWriter::TFeedHeader *fH=new CFitsWriter::TFeedHeader[inputs];
			for (long i=0;i<inputs;i++) {
				fH[i].id=i;
				fH[i].xOffset=xOff[i];
				fH[i].yOffset=yOff[i];
				fH[i].relativePower=power[i];
			}
			data->saveFeedHeader(fH,inputs);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			receiverBossError=true;
			data->saveFeedHeader(NULL,0);
		} 
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
			impl.setOperationName("getFeeds()");
			impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			data->saveFeedHeader(NULL,0);
		}
		try { // get the calibration marks values
			ACS::doubleSeq_var calMarks;
			ACS::doubleSeq freqs,bws,atts;
			ACS::longSeq feeds,ifs;
			ACS::doubleSeq_var skyFreq;
			ACS::doubleSeq_var skyBw;
			data->getInputsConfiguration(feeds,ifs,freqs,bws,atts);
			calMarks=m_receiversBoss->getCalibrationMark(freqs,bws,feeds,ifs,skyFreq.out(),skyBw.out());
			data->setCalibrationMarks(calMarks.in());
			data->setSkyFrequency(skyFreq.in());
			data->setSkyBandwidth(skyBw.in());
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			data->setCalibrationMarks();
			data->setSkyFrequency();
			data->setSkyBandwidth();
			receiverBossError=true;
		} 		
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
			impl.setOperationName("getCalibrationMark()");
			impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			data->setCalibrationMarks();
		}
	}
	else {
		data->setReceiverCode(IRA::CString(""));
		data->setLocalOscillator();
		data->saveFeedHeader(NULL,0);
		data->setCalibrationMarks();
	}
}
