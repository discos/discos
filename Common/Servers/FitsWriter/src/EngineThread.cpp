
#include "EngineThread.h"
#include <LogFilter.h>
#include <Definitions.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <DateTime.h>
#include <SkySource.h>
#include <ReceiversModule.h>
#include <ManagementModule.h>
#include <AntennaModule.h>
#include "CommonTools.h"

using namespace IRA;
using namespace FitsWriter_private;

_IRA_LOGFILTER_IMPORT;

CEngineThread::CEngineThread(const ACE_CString& name,FitsWriter_private::CDataCollection *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : 
				ACS::Thread(name,responseTime,sleepTime),m_data(param)
{
	AUTO_TRACE("CEngineThread::CEngineThread()");
	m_fileOpened=false;
	m_summaryOpened=false;
	m_file=NULL;
	m_summary=NULL;
	m_ptsys=new double[256];
	m_receiversBoss=Receivers::ReceiversBoss::_nil();
	receiverBossError=false;
	m_antennaBoss=Antenna::AntennaBoss::_nil();
	antennaBossError=false;
	m_lastMinorServoEnquireTime=0;
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
	if (m_summary) {
		delete m_summary;
	}
	if (m_ptsys) {
		delete [] m_ptsys;
	}
	try {
		CCommonTools::unloadAntennaBoss(m_antennaBoss,m_service);
		CCommonTools::unloadReceiversBoss(m_receiversBoss,m_service);
		CCommonTools::unloadMSBoss(m_minorServoBoss,m_service);
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
 
bool CEngineThread::checkTime(const ACS::Time& jobStartTime)
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	return (now.value().value<(jobStartTime+m_timeSlice));
	//return (currentTime>(m_data->getFirstDumpTime()+getSleepTime()+m_timeSlice)); // gives the cache time to fill a little bit
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
	bool applyServoPositions;
	long pol,bins;
	long buffSize;
	double ra,dec;
	double az,el;
	bool tracking;
	double hum,temp,press;
	double derot;
	ACS::doubleSeq_var servoPositions;
	long long integrationTime;
	//CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();

	if (m_data->getIsNoData()) {
		//IRA::CString outString;
		if (!m_data->getFakeDump(time,calOn,bufferCopy,buffer,tracking,buffSize)) return false;
		//IRA::CIRATools::timeToStr(time,outString);
	}
	else {
		if (!m_data->getDump(time,calOn,bufferCopy,buffer,tracking,buffSize)) return false;
	}

	TIMEVALUE tS;
	tS.value(time);
#ifdef FW_DEBUG
	IRA::CString out;
	out.Format("%02d:%02d:%02d.%03d  %d ",tS.hour(),tS.minute(),tS.second(),tS.microSecond()/1000,calOn);
	m_file << out;
#else
	CFitsWriter::TDataHeader tdh;
	CDateTime tConverter(tS,m_data->getDut1()); 
	tdh.time=tConverter.getMJD();
	//integration is multiplied by 10000 because internally we have the value in millesec while the method requires 100ns.
	integrationTime=m_data->getIntegrationTime()*10000;
#endif
	try {
		CCommonTools::getAntennaBoss(m_antennaBoss,m_service,m_config->getAntennaBossComponent(),antennaBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		_IRA_LOGFILTER_LOG_EXCEPTION(ex,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
		antennaBossError=true;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CEngineThread::processData()");
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
	}
	ra=dec=az=el=0.0;
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->getObservedEquatorial(time,integrationTime,ra,dec);
			m_antennaBoss->getObservedHorizontal(time,integrationTime,az,el);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
		antennaBossError=true;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CEngineThread::processData()");
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
	}
	try {
		CCommonTools::getReceiversBoss(m_receiversBoss,m_service,m_config->getReceiversBossComponent(),receiverBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		_IRA_LOGFILTER_LOG_EXCEPTION(ex,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
	}
	derot=0.0;
	try {
		derot=m_receiversBoss->getDerotatorPositionFromHistory(time);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
		receiverBossError=true;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
		impl.setOperationName("getDerotatorPositionFromHistory()");
		impl.setComponentName((const char *)m_config->getReceiversBossComponent());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
	}
	catch (ReceiversErrors::ReceiversErrorsEx & ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
		impl.setOperationName("getDerotatorPositionFromHistory()");
		impl.setComponentName((const char *)m_config->getReceiversBossComponent());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
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
	FitsWriter_private::getTsysFromBuffer(buffer,m_data->getInputsNumber(),m_ptsys);
	tdh.raj2000=ra;
	tdh.decj2000=dec;
	tdh.az=az;
	tdh.el=el;
	tdh.par_angle=IRA::CSkySource::paralacticAngle(tConverter,m_data->getSite(),az,el);
	tdh.derot_angle=derot;
	tdh.flag_cal=calOn;
	tdh.flag_track=tracking;
	m_data->getMeteo(hum,temp,press);
	tdh.weather[0]=hum;
	tdh.weather[1]=temp;
	tdh.weather[2]=press;
	if (!m_file->storeAuxData(tdh,m_ptsys)) {
		_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::processData()");
		impl.setFileName((const char *)m_data->getFileName());
		impl.setError(m_file->getLastError());
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		m_data->setStatus(Management::MNG_FAILURE);
	}
#endif
	if (m_config->getMinorServoBossComponent()!="") {
		try {
			CCommonTools::getMSBoss(m_minorServoBoss,m_service,m_config->getMinorServoBossComponent(),minorServoBossError);
		}
		catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
			_IRA_LOGFILTER_LOG_EXCEPTION(ex,LM_ERROR);
			m_data->setStatus(Management::MNG_FAILURE);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
			m_data->setStatus(Management::MNG_FAILURE);
			minorServoBossError=true;
		}
		//servoPositions->length(0);
		applyServoPositions=false;
		try {
			if (!CORBA::is_nil(m_minorServoBoss)) {
				ACS::TimeInterval interval=(ACS::TimeInterval)m_config->getMinorServoEnquireMinGap()*10;
				if (time>=m_lastMinorServoEnquireTime+interval) {
					servoPositions=m_minorServoBoss->getAxesPosition(time);
					m_lastMinorServoEnquireTime=time;
					applyServoPositions=true;
				}
			}
		}
		catch (ManagementErrors::ConfigurationErrorEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::processData()");
				impl.setOperationName("getAxesPosition()");
				impl.setComponentName((const char *)m_config->getMinorServoBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
		}
		catch ( ComponentErrors::UnexpectedEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::processData()");
			impl.setOperationName("getAxesPosition()");
			impl.setComponentName((const char *)m_config->getMinorServoBossComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
			m_data->setStatus(Management::MNG_FAILURE);
			minorServoBossError=true;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CEngineThread::processData()");
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
			m_data->setStatus(Management::MNG_FAILURE);
		}
#ifdef FW_DEBUG
		if (applyServoPositions) {
			IRA::CString tempOut;
			out="Minor Servo: "
					for (long inc=0;inc<servoPositions->length();inc++) {
						tempOut.Format("%f ",servoPositions[inc]);
						out+=tempOut;
					}
			m_file << (const char *) out;
			m_file << endl;
		}
#else
		if (applyServoPositions) {
			if (!m_file->storeServoData(tdh.time,servoPositions.in())) {
				_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::processData()");
				impl.setFileName((const char *)m_data->getFileName());
				impl.setError(m_file->getLastError());
				_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
				m_data->setStatus(Management::MNG_FAILURE);
			}
			m_file->add_servo_table_row();
		}
#endif
	}  // m_config->getMinorServoBossComponent()!=""
	if (!m_data->getIsNoData()) {
		for (int i=0;i<m_data->getSectionsNumber();i++) {
			bins=m_data->getSectionBins(i);
			pol=m_data->getSectionStreamsNumber(i);
			switch (m_data->getSampleSize()) {
				case sizeof(BYTE2_TYPE): {
	#ifdef FW_DEBUG
					BYTE2_TYPE channel[bins*pol];
					FitsWriter_private::getChannelFromBuffer<BYTE2_TYPE>(i,pol,bins,buffer,channel);
					out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ",i,pol,bins,m_data->getSampleSize());
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
						impl.setFileName((const char *)m_data->getFileName());
						impl.setError(m_file->getLastError());
						_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
						m_data->setStatus(Management::MNG_FAILURE);
					}
	#endif
					break;
				}
				case sizeof(BYTE4_TYPE): {
	#ifdef FW_DEBUG
					BYTE4_TYPE channel[bins*pol];
					FitsWriter_private::getChannelFromBuffer<BYTE4_TYPE>(i,pol,bins,buffer,channel);
					out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ",i,pol,bins,m_data->getSampleSize());
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
						impl.setFileName((const char *)m_data->getFileName());
						impl.setError(m_file->getLastError());
						_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
						m_data->setStatus(Management::MNG_FAILURE);
					}
	#endif
					break;
				}
				default : { // it should be BYTE8_TYPE
	#ifdef FW_DEBUG
					BYTE8_TYPE channel[bins*pol];
					FitsWriter_private::getChannelFromBuffer<BYTE8_TYPE>(i,pol,bins,buffer,channel);
					out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ",i,pol,bins,m_data->getSampleSize());
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
						impl.setFileName((const char *)m_data->getFileName());
						impl.setError(m_file->getLastError());
						_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
						m_data->setStatus(Management::MNG_FAILURE);
					}
	#endif
				}
			}
		} //end for
	}


#ifndef FW_DEBUG
	m_file->add_row();
#endif
	delete [] bufferCopy;
	return true;
}

void CEngineThread::runLoop()
{
	TIMEVALUE nowEpoch;
	IRA::CString filePath,fileName;
	//CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	IRA::CIRATools::getTime(nowEpoch); // it marks the start of the activity job
	//cout << "inizio : " << nowEpoch.value().value << endl;
	if (m_summaryOpened && m_data->isWriteSummary()) {
		std::list<double> va;
		ACS::doubleSeq rf;
		m_info.getRestFreq(rf);
		va.clear();
		CCommonTools::map(rf,va);
		m_summary->getFilePointer()->setKeyword("RESTFREQ",va);
		if ((!m_summary->write()) || (!m_summary->close())) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
			impl.setFileName((const char *)m_data->getSummaryFileName());
			impl.setError((const char *)m_summary->getLastError());
			impl.log(LM_ERROR); 
			m_data->setStatus(Management::MNG_FAILURE);
		}
		delete m_summary;
		m_summary=NULL;
		m_summaryOpened=false;
		m_data->closeSummary();
		ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"SUMMARY_FILE_FINALIZED"));
	}
	if (m_data->isReset()) {
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
		if (m_summaryOpened) {
			if (m_summary) {
				delete m_summary;
			}
			m_summary=NULL;
			ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"SUMMARY_FILE_CLOSED"));
			m_summaryOpened=false;
		}
		m_data->haltResetStage();
		return;
	}
	if (m_data->isStart() && m_data->isReady() &&  m_data->isScanHeaderReady() && m_data->isSubScanHeaderReady()) { // //main headers are already saved and file has to be opened
		//*****************************************************************************************
		ACS_LOG(LM_FULL_INFO,"CEngineThread::runLoop()",(LM_NOTICE,"LETS BEGIN SUBSCAN %ld!",m_data->getSubScanID()));
		//************************* ADDDED FOR DEBUGGING NoData/Roach Could be deleted ****************
		if (!m_fileOpened) {
			m_data->setStatus(Management::MNG_OK);
			// create the file and save main headers
			m_data->getFileName(fileName,filePath);
			if (!IRA::CIRATools::directoryExists(filePath)) {
				if (!IRA::CIRATools::makeDirectory(filePath)) {
					_EXCPT(ComponentErrors::FileIOErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)filePath);
					impl.log(LM_ERROR);
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else {
					ACS_LOG(LM_FULL_INFO,"CEngineThread::runLoop()",(LM_NOTICE,"NEW_SCAN_FOLDER_CREATED: %s",(const char *)filePath));
				}
			}
#ifdef FW_DEBUG
			m_file.open((const char *)m_data->getFileName(),ios_base::out|ios_base::trunc);
			if (!m_file.is_open()) {
				_EXCPT(ComponentErrors::FileIOErrorExImpl,impl,"CEngineThread::runLoop()");
				impl.setFileName((const char *)m_data->getFileName());
				impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
				m_data->setStatus(Management::MNG_FAILURE);
			}
#else
			//let's create the summary file, it should be created before the first subscan of the scan.......
			// the the summary will be valid for the duration of all the subscans.....
			if (!m_summaryOpened) {
			 	TIMEVALUE currentUT;
				IRA::CDateTime now;
				TIMEDIFFERENCE currentLST;
				IRA::CString lstStr;

				// now let's create the summary file.
				m_summary=new CSummaryWriter();
				m_summary->setBasePath("");
				m_summary->setFileName((const char *)m_data->getSummaryFileName());
				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_DEBUG,"SUMMARY_FILE %s",(const char*)m_data->getSummaryFileName()));
				if (!m_summary->create()) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getSummaryFileName());
					impl.setError((const char *)m_summary->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				// compute the LST time for the scan
			 	IRA::CIRATools::getTime(currentUT); // get the current time
			 	now.setDateTime(currentUT,m_data->getDut1());  // transform the current time in a CDateTime object
			 	now.LST(m_data->getSite()).getDateTime(currentLST);  // get the current LST time
			 	currentLST.day(0);
			 	IRA::CIRATools::intervalToStr(currentLST.value().value,lstStr);
			 	m_summary->getFilePointer()->setKeyword("LST",lstStr);
			 	IRA::CIRATools::timeToStrExtended(currentUT.value().value,lstStr);
			 	m_summary->getFilePointer()->setKeyword("DATE-OBS",lstStr);

				m_summaryOpened=true;
				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_DEBUG,"SUMMARY_OPENED"));
			}
			///**********************************************************************************
			ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"CREO IL FILE"));
			///******************* DEBUG ********************************
			m_file = new CFitsWriter();
			m_file->setBasePath("");
			m_file->setFileName((const char *)m_data->getFileName());
			if (!m_file->create()) {
				_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
				impl.setFileName((const char *)m_data->getFileName());
				impl.setError(m_file->getLastError());
				impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
				m_data->setStatus(Management::MNG_FAILURE);
			}
			///**********************************************************************************
			ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"CREATO"));
			///******************* DEBUG ********************************

#endif
			else {
#ifdef FW_DEBUG
				IRA::CString out;
				Backends::TMainHeader mH=m_data->getMainHeader();
				out.Format("Main - ch: %d , beams: %d, sampleSize: %d, integration: %d \n ",
					mH.sections,mH.beams,mH.sampleSize,mH.integration);
				m_file<< (const char *) out;
				Backends::TSectionHeader const *cH=m_data->getSectionHeader();
				for (int j=0;j<m_data->getSections();j++) {
					out.Format("channel id: %d, bins: %d , pol: %d, bandWidth: %lf, frequency: %lf, attenuationL: %lf, attenuationR: %lf"
						 "sampleRate: %lf, feed: %d \n",cH[j].id,cH[j].bins,cH[j].polarization,cH[j].bandWidth,
						 cH[j].frequency,cH[j].attenuation[0],cH[j].attenuation[1],cH[j].sampleRate,cH[j].feed);
					m_file<< (const char *) out;
				}
#else
				//*****************************************************************************************
				ACS_LOG(LM_FULL_INFO,"CEngineThread::runLoop()",(LM_NOTICE,"OUTPUT_FILE_CREATED_NOW"));
				//************************* ADDDED FOR DEBUGGING NoData/Roach Could be deleted ****************
				m_fileOpened=true;
				m_data->startRunnigStage();
				//get data from receivers boss
				if (m_summaryOpened) {
					collectReceiversData(m_summary->getFilePointer());
				}
				else {
					collectReceiversData(NULL);
				}
				if (m_summaryOpened) { //this must be called before the collectAntennaData
					collectSchedulerData(m_summary->getFilePointer());
				}
				else {
					collectSchedulerData(NULL);
				}
				//get the data from the antenna boss
				if (m_summaryOpened) {
					collectAntennaData(m_summary->getFilePointer());
				}
				else {
					collectAntennaData(NULL);
				}

				//get the data from the minor servo boss...if subsystem is enabled
				collectMinorServoData();

				//*****************************************************************************************
				ACS_LOG(LM_FULL_INFO,"CEngineThread::runLoop()",(LM_NOTICE,"DATA_COLLECTION_COMPLETED"));
				//************************* ADDDED FOR DEBUGGING NoData/Roach Could be deleted ****************

				// now creates the file, the tables and the headers
				Backends::TMainHeader mH=m_data->getMainHeader();
				Backends::TSectionHeader const *cH=m_data->getSectionHeader();
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
				ACS::longSeq feedsID;
				ACS::longSeq ifsID;
				ACS::doubleSeq atts;
				ACS::longSeq sectionsID;
				ACS::stringSeq axisName,axisUnit;

				m_data->getSite(site,dut1,siteName);
				m_info.getLocalOscillator(LocalOscillator);
				m_info.getSectionsID(sectionsID);
				m_info.getBackendAttenuations(atts);
				m_info.getFeedsID(feedsID);
				m_info.getIFsID(ifsID);
				m_info.getSkyBandwidth(skyBw);
				m_info.getSkyFrequency(skyFreq);
				m_info.getCalibrationMarks(calib);
				m_info.getSourceFlux(fluxes);
				m_info.getReceiverPolarization(polarizations);
				m_info.getSource(sourceName,sourceRa,sourceDec,sourceVlsr);
				m_info.getAntennaOffsets(azOff,elOff,raOff,decOff,lonOff,latOff);
				scanTag=m_data->getScanTag();
				scanID=m_data->getScanID();
				subScanID=m_data->getSubScanID();
				scheduleName=m_data->getScheduleName();

				if (!m_file->saveMainHeader(mH)) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Project_Name",(const char *)m_data->getProjectName(),"Name of the project")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Observer",(const char *)m_data->getObserverName(),"Name of the observer")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Antenna",(const char *)siteName,"Name of the station")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("SiteLongitude",site.getLongitude(),"Longitude of the site (radians)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("SiteLatitude",site.getLatitude(),"Latitude of the site (radians)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("SiteHeight",site.getHeight(),"Height of the site (meters)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Beams",/*mH.beams*/m_info.getFeedNumber(),"Number of beams")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Sections",mH.sections,"Total number of sections")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if (!m_file->setPrimaryHeaderKey("Sample Size",mH.sampleSize,"Number of bytes of a data")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Receiver Code",(const char *)m_info.getReceiverCode(),"Keyword that identifies the receiver")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Source",(const char *)sourceName,"Source identifier")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("RightAscension",sourceRa,"Source right ascension at J2000 (radians)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Declination",sourceDec,"Source declination at J2000 (radians)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Vlsr",sourceVlsr,"Source radial velocity")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Azimuth Offset",azOff,"Longitude offset in horizontal frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Elevation Offset",elOff,"Latitude offset in horizontal frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("RightAscension Offset",raOff,"Longitude offset in equatorial frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Declination Offset",decOff,"Latitude offset in equatorial frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("GalacticLon Offset",lonOff,"Longitude offset in galactic frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("GalacticLat Offset",latOff,"Latitude offset in galactic frame")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("ScanID",scanID,"Scan Identifier")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("SubScanID",subScanID,"Subscan Identifier")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("ScheduleName",(const char *)scheduleName,"name of the running schedule")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("SubScanType",(const char *)m_data->getSubScanType(),"describes the scan type based on telescope movement")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else if(!m_file->setPrimaryHeaderKey("Signal",(const char *)Management::Definitions::map(m_info.getSubScanConf().signal),"Flag for position switching phase")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				if (scanTag>=0) {
					if(!m_file->setPrimaryHeaderKey("Scan Tag",scanTag,"Scan tag identifier")) {
						_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
						impl.setFileName((const char *)m_data->getFileName());
						impl.setError(m_file->getLastError());
						impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
						m_data->setStatus(Management::MNG_FAILURE);
					}
				}
				if (!m_file->saveSectionHeader(cH)) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				if (!m_file->addSectionTable(sectionsID,feedsID,ifsID,polarizations,LocalOscillator,skyFreq,skyBw,calib,fluxes,atts,m_data->getIsNoData())) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				if (!m_file->setSectionHeaderKey("Integration",mH.integration,"Integration time (milliseconds)")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				CFitsWriter::TFeedHeader *feedH=m_info.getFeedHeader();
				if (!m_file->addFeedTable("FEED TABLE")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				for (WORD j=0;j<m_info.getFeedNumber();j++) {
					if (!m_file->saveFeedHeader(feedH[j])) {
						_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
						impl.setFileName((const char *)m_data->getFileName());
						impl.setError(m_file->getLastError());
						impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
						m_data->setStatus(Management::MNG_FAILURE);
						j=m_info.getFeedNumber(); //exit the cycle
					}
				}
				double dewarPos;
				Receivers::TDerotatorConfigurations dewarMode;
				m_info.getDewarConfiguration(dewarMode,dewarPos);
				if (!m_file->setFeedHeaderKey("DEWRTMOD",(const char *)Receivers::Definitions::map(dewarMode),"Dewar positioner configuration mode")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				if (!m_file->setFeedHeaderKey("DEWUSER",(double)dewarPos,"Dewar initial angle")) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				if (!m_file->addDataTable()) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getFileName());
					impl.setError(m_file->getLastError());
					impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					m_data->setStatus(Management::MNG_FAILURE);
				}
				if (m_config->getMinorServoBossComponent()!="") {
					m_info.getServoAxisNames(axisName);
					m_info.getServoAxisUnits(axisUnit);
					if (!m_file->addServoTable(axisName,axisUnit)) {
						_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
						impl.setFileName((const char *)m_data->getFileName());
						impl.setError(m_file->getLastError());
						impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
						m_data->setStatus(Management::MNG_FAILURE);
					}
				}				
#endif
				//m_fileOpened=true;
				//m_data->startRunnigStage();
				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_DEBUG,"RUNNING_FROM_NOW" ));
				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"FILE_OPENED %s",(const char *)m_data->getFileName()));
			}
		} // end !m_fileOpened
	}
	else if (m_data->isStop()) {
		ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_DEBUG,"STOPPING" ));
		//save all the data in the buffer an then finalize the file
		if (m_fileOpened) {
			//cout << "Stopping, cached  dumps: " << m_data->getDumpCollectionSize() << endl;
			while (processData());

#ifdef FW_DEBUG
		m_file.close();
#else
		//m_file->close();
		delete m_file; // file close called directly by the class destructor
		m_file=NULL;
#endif
		}
		m_fileOpened=false;
		ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"FILE_FINALIZED"));
		/*if (m_summaryOpened && m_data->isWriteSummary()) {
			if ((!m_summary->write()) || (!m_summary->close())) {
					_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"CEngineThread::runLoop()");
					impl.setFileName((const char *)m_data->getSummaryFileName());
					impl.setError((const char *)m_summary->getLastError());
					impl.log(LM_ERROR); 
					m_data->setStatus(Management::MNG_FAILURE);
			}
			delete m_summary;
			m_summary=NULL;
			m_summaryOpened=false;
			ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"SUMMARY_FILE_FINALIZED"));
		}*/
		m_data->haltStopStage();
	}
	else if (m_data->isRunning()) { // file was already created.... then saves the data into it
		 // until there is something to process and
		// there is still time available.......
		if (m_fileOpened) {
			//cout << "cached before dumps: " << m_data->getDumpCollectionSize() << endl;
			while (checkTime(nowEpoch.value().value) && checkTimeSlot(nowEpoch.value().value) && processData());
			//cout << "cached after  dumps: " << m_data->getDumpCollectionSize() << endl;
			//IRA::CIRATools::getTime(nowEpoch);
			//cout << "fine :" << nowEpoch.value().value << endl;
		}
	}
}
//}

void CEngineThread::collectMinorServoData()
{
	//CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	if (m_config->getMinorServoBossComponent()=="") {
		return; // in case the minor servo subsystem is not enabled
	}
	try {
		CCommonTools::getMSBoss(m_minorServoBoss,m_service,m_config->getMinorServoBossComponent(),minorServoBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		m_data->setStatus(Management::MNG_WARNING);
		m_minorServoBoss=MinorServo::MinorServoBoss::_nil();
	}
	if (!CORBA::is_nil(m_minorServoBoss)) {
		ACS::stringSeq_var names;
		ACS::stringSeq_var units;
		try {
			m_minorServoBoss->getAxesInfo(names.out(),units.out());
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectMinorServoData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			minorServoBossError=true;
		}
		catch (ManagementErrors::ConfigurationErrorEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectMinorServoData()");
			impl.setOperationName("getAxesInfo()");
			impl.setComponentName((const char *)m_config->getMinorServoBossComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setServoAxis();
		}
		m_info.setServoAxis(names.in(),units.in());
	}
}

void CEngineThread::collectAntennaData(FitsWriter_private::CFile* summaryFile)
{
	//CSecAreaResourceWrapper<CDataCollection> m_data=m_dataWrapper->Get();
	try {
		CCommonTools::getAntennaBoss(m_antennaBoss,m_service,m_config->getAntennaBossComponent(),antennaBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		m_data->setStatus(Management::MNG_WARNING);		
		m_antennaBoss=Antenna::AntennaBoss::_nil();
	}
	if (!CORBA::is_nil(m_antennaBoss)) {
		ACSErr::Completion_var comp;
		CORBA::Double ra=0.0,dec=0.0,vrad=0.0;
		CORBA::Double raOff=0.0,decOff=0.0,azOff=0.0,elOff=0.0,lonOff=0.0,latOff=0.0;
		IRA::CString sourceName="";
		Antenna::TReferenceFrame VFrame=Antenna::ANT_UNDEF_FRAME;
		Antenna::TVradDefinition VDefinition=Antenna::ANT_UNDEF_DEF;

		try { //get the target name and parameters
			ACS::ROstring_var targetRef;
			CORBA::String_var target;
			ACS::ROdouble_var raRef,decRef,vradRef;
			ACS::ROdouble_var raOffRef,decOffRef,azOffRef,elOffRef,lonOffRef,latOffRef;
			Antenna::ROTReferenceFrame_var VFrameRef;
			Antenna::ROTVradDefinition_var VDefinitionRef;

			//let's take the references to the attributes
			targetRef=m_antennaBoss->target();
			raRef=m_antennaBoss->targetRightAscension();
			decRef=m_antennaBoss->targetDeclination();
			vradRef=m_antennaBoss->targetVrad();
			azOffRef=m_antennaBoss->azimuthOffset();
			elOffRef=m_antennaBoss->elevationOffset();
			raOffRef=m_antennaBoss->rightAscensionOffset();
			decOffRef=m_antennaBoss->declinationOffset();
			lonOffRef=m_antennaBoss->longitudeOffset();
			latOffRef=m_antennaBoss->latitudeOffset();
			VFrameRef=m_antennaBoss->vradReferenceFrame();
			VDefinitionRef=m_antennaBoss->vradDefinition();

			VDefinition=VDefinitionRef->get_sync(comp.out());
			ACSErr::CompletionImpl VDefinitionRefCompl(comp);
			if (!VDefinitionRefCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,VDefinitionRefCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("vradDefinition");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
			}
			else {
				if (summaryFile) summaryFile->setKeyword("VDEF",Antenna::Definitions::map(VDefinition));
			}
			VFrame=VFrameRef->get_sync(comp.out());
			ACSErr::CompletionImpl VFrameRefCompl(comp);
			if (!VFrameRefCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,VFrameRefCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("vradReferenceFrame");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
			}
			else {
				if (summaryFile) summaryFile->setKeyword("VFRAME",Antenna::Definitions::map(VFrame));
			}
			target=targetRef->get_sync(comp.out());
			ACSErr::CompletionImpl targetCompl(comp);
			if (!targetCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,targetCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("target");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
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
				m_data->setStatus(Management::MNG_WARNING);
				ra=0.0;
			}
			dec=decRef->get_sync(comp.out());
			ACSErr::CompletionImpl decCompl(comp);
			if (!decCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,decCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("targetDeclination");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				dec=0.0;
			}
			vrad=vradRef->get_sync(comp.out());
			ACSErr::CompletionImpl vradCompl(comp);
			if (!vradCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,vradCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("targetVrad");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				vrad=0.0;
			}
			azOff=azOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl azOffCompl(comp);
			if (!azOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,azOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("azimuthOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				azOff=0.0;
			}
			elOff=elOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl elOffCompl(comp);
			if (!elOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,elOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("elevationOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				elOff=0.0;
			}
			raOff=raOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl raOffCompl(comp);
			if (!raOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,raOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("rightAscensionOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				raOff=0.0;
			}
			decOff=decOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl decOffCompl(comp);
			if (!decOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,decOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("declinationOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				decOff=0.0;
			}
			lonOff=lonOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl lonOffCompl(comp);
			if (!lonOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,lonOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("longitudeOffset");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				lonOff=0.0;
			}
			latOff=latOffRef->get_sync(comp.out());
			ACSErr::CompletionImpl latOffCompl(comp);
			if (!latOffCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,latOffCompl,"CEngineThread::collectAntennaData()");
				impl.setAttributeName("targetVlsr");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				latOff=0.0;
			}
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectAntennaData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			antennaBossError=true;
			sourceName="";
			ra=dec=vrad=0.0;
		}
		m_info.setSource(sourceName,ra,dec,vrad);
		m_info.setAntennaOffsets(azOff,elOff,raOff,decOff,lonOff,latOff);
		if (summaryFile) {
			summaryFile->setKeyword("OBJECT",sourceName);
			summaryFile->setKeyword("RightAscension",ra);
			summaryFile->setKeyword("Declination",dec);
			summaryFile->setKeyword("VRAD",vrad);
		}
		try { //get the estimated source fluxes
			ACS::doubleSeq_var fluxes;
			ACS::doubleSeq freqs;
			ACS::doubleSeq bw;
			m_info.getSkyFrequency(freqs);
			m_info.getSkyBandwidth(bw);
			for (unsigned i=0;i<freqs.length();i++) {
				freqs[i]+=bw[i]/2.0; //computes the central frequency;
			}
			m_antennaBoss->getFluxes(freqs,fluxes.out());
			m_info.setSourceFlux(fluxes);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectAntennaData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			antennaBossError=true;
			m_info.setSourceFlux();
		}
	}
}

void CEngineThread::collectSchedulerData(FitsWriter_private::CFile* summaryFile)
{
	try {
		CCommonTools::getScheduler(m_scheduler,m_service,m_config->getSchedulerComponent(),m_schedulerError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		m_data->setStatus(Management::MNG_WARNING);
		m_scheduler=Management::Scheduler::_nil();
	}
	if (!CORBA::is_nil(m_scheduler)) {
		ACSErr::Completion_var comp;
		ACS::ROdoubleSeq_var restFreqRef;
		ACS::doubleSeq_var restFreq;
		try {
			restFreqRef=m_scheduler->restFrequency();
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectSchedulerdata()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setRestFreq();
			m_schedulerError=true;
		}
		restFreq=restFreqRef->get_sync(comp.out());
		//std::list<double> va;
		//va.clear();
		//CCommonTools::map(restFreq,va);
		//if (summaryFile) summaryFile->setKeyword("RESTFREQ",va);
		m_info.setRestFreq(restFreq);
		Management::TSubScanConfiguration_var conf;
		try {
			m_scheduler->getSubScanConfigruation(conf.out());
			m_info.setSubScanConf(conf.in());
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectSchedulerdata()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			minorServoBossError=true;
			m_info.setSubScanConf();
		}
		catch (ManagementErrors::ManagementErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectSchedulerdata()");
			impl.setOperationName("getSubScanConfigruation()");
			impl.setComponentName((const char *)m_config->getSchedulerComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setSubScanConf();
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectSchedulerdata()");
			impl.setOperationName("getSubScanConfigruation()");
			impl.setComponentName((const char *)m_config->getSchedulerComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setSubScanConf();
		}
	}
	else {
		m_info.setSubScanConf();
		m_info.setRestFreq();
	}
}

void CEngineThread::collectReceiversData(FitsWriter_private::CFile* summaryFile)
{
	//CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	try {
		CCommonTools::getReceiversBoss(m_receiversBoss,m_service,m_config->getReceiversBossComponent(),receiverBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		m_data->setStatus(Management::MNG_WARNING);		
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
				m_info.setReceiverCode((const char *)recvCode);
				if (summaryFile) summaryFile->setKeyword("ReceiverCode",IRA::CString(recvCode.in()));
			}
			else {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,compImpl,"CEngineThread::collectReceiversData()");
				impl.setAttributeName("receiverCode");
				impl.setComponentName((const char *)m_config->getReceiversBossComponent());
				impl.log(LM_ERROR);
				m_data->setStatus(Management::MNG_WARNING);
				m_info.setReceiverCode(IRA::CString(""));
			}
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			receiverBossError=true;
			m_info.setReceiverCode(IRA::CString(""));
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
			m_info.saveFeedHeader(fH,inputs); //fH deleted inside this object
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			receiverBossError=true;
			m_info.saveFeedHeader(NULL,0);
		} 
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
			impl.setOperationName("getFeeds()");
			impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.saveFeedHeader(NULL,0);
		}
		catch (ReceiversErrors::ReceiversErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
			impl.setOperationName("getFeeds()");
			impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.saveFeedHeader(NULL,0);
		}
		try { // get the calibration marks values and inputs configuration
			ACS::doubleSeq freqs,bws,atts;
			ACS::longSeq feeds,ifs,sectionsID;
			ACS::doubleSeq_var skyFreq;
			ACS::doubleSeq_var skyBw;
			ACS::doubleSeq_var calMarks;
			ACS::longSeq_var rcvPol;
			ACS::doubleSeq_var LO;
			ACS::doubleSeq_var IFFreq;
			ACS::doubleSeq_var IFBw;
			double scale;
			m_data->getInputsConfiguration(sectionsID,feeds,ifs,freqs,bws,atts);
			calMarks=m_receiversBoss->getCalibrationMark(freqs,bws,feeds,ifs,skyFreq.out(),skyBw.out(),scale);
			m_receiversBoss->getIFOutput(feeds,ifs,IFFreq.out(),IFBw.out(),rcvPol.out(),LO.out());
			m_info.setInputsTable(sectionsID,feeds,ifs,rcvPol.in(),skyFreq.in(),skyBw.in(),LO.in(),atts,calMarks.in());
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setInputsTable();
			/*m_data->setCalibrationMarks();
			m_data->setSkyFrequency();
			m_data->setSkyBandwidth();*/
			receiverBossError=true;
		} 		
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
			impl.setOperationName("getCalibrationMark(),getIFOutputMark()");
			impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setInputsTable();
			//m_data->setCalibrationMarks();
		}
		catch (ReceiversErrors::ReceiversErrorsEx & ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
			impl.setOperationName("getCalibrationMark(),getIFOutputMark()");
			impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setInputsTable();
			//m_data->setCalibrationMarks();
		}
		try {
			Receivers::TDerotatorConfigurations mod;
			double setupPos;
			m_receiversBoss->getDewarParameter(mod,setupPos);
			m_info.setDewarConfiguration(mod,setupPos);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::collectReceiversData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setDewarConfiguration();
			receiverBossError=true;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
			impl.setOperationName("getDewarParameter()");
			impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setDewarConfiguration();
			//m_data->setCalibrationMarks();
		}
		catch (ReceiversErrors::ReceiversErrorsEx & ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::collectReceiversData()");
			impl.setOperationName("getDewarParameter()");
			impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			impl.log(LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);
			m_info.setDewarConfiguration();
		}
	}
	else {
		m_info.setReceiverCode(IRA::CString(""));
		m_info.saveFeedHeader(NULL,0);
		m_info.setInputsTable();
	}
}
