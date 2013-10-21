#include "EngineThread.h"
#include <LogFilter.h>
#include <Definitions.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <DateTime.h>
#include "CommonTools.h"
#include "fit2.h"
#include "fgaus.h"
#include <SkySource.h>

using namespace IRA;
using namespace CalibrationTool_private;

_IRA_LOGFILTER_IMPORT;

CEngineThread::CEngineThread (const ACE_CString & name,
        CSecureArea < CDataCollection > *param,
        const ACS::TimeInterval & responseTime,
        const ACS::TimeInterval & sleepTime):
    ACS::Thread (name, responseTime, sleepTime),
    m_dataWrapper (param)
{
    AUTO_TRACE ("CEngineThread::CEngineThread()");
    m_fileOpened = false;
    m_ptsys = new double[DATATSYSSEQLENGTH];
    m_ptsys2 = new float[DATATSYSSEQLENGTH];
    m_antennaBoss = Antenna::AntennaBoss::_nil ();
    m_minorServoBoss=MinorServo::MinorServoBoss::_nil();
    m_antennaBossError = false;
    m_minorServoBossError=false;
    m_dataSeq.length (DATACOORDINATESSEQLENGTH);
    m_tsysDataSeq.length (DATATSYSSEQLENGTH);
    m_dataSeqCounter = 0;
    //m_lastCoordinate = 0;
    m_off = new float[DATATSYSSEQLENGTH];
    m_secsFromMidnight = new float[DATATSYSSEQLENGTH];
    m_Par = new float[PARAMETERNUMBER];
    m_errPar = new float[PARAMETERNUMBER];
    m_observatory = Antenna::Observatory::_nil ();
    observatoryError = false;
    m_LatOff = m_LonOff = 0.0;
    m_latAmp=m_lonAmp=m_latAmpErr=m_lonAmpErr=m_latFwhm=m_lonFwhm=m_latFwhmErr=m_lonFwhmErr=0.0;
    m_latPositions = new double[DATACOORDINATESSEQLENGTH];
    m_lonPositions = new double[DATACOORDINATESSEQLENGTH];
	m_fwhm=0.0;
	m_targetRa=m_targetDec=0.0;
	m_focusScanCenter=0.0;
	m_minorServoCurrentAxisPosition=-1;
}

CEngineThread::~CEngineThread ()
{
    AUTO_TRACE ("CEngineThread::~CEngineThread()");
    if (m_fileOpened) {
        m_file.close ();
    }
    if (m_ptsys) {
        delete[]m_ptsys;
    }
    if (m_ptsys2) {
        delete[]m_ptsys2;
    }
    if (m_off) {
        delete[]m_off;
    }
    if (m_secsFromMidnight) {
        delete[]m_secsFromMidnight;
    }
    if (m_Par) {
        delete[]m_Par;
    }
    if (m_errPar) {
        delete[]m_errPar;
    }
    if (m_latPositions) {
        delete [] m_latPositions;
    }
    if (m_lonPositions) {
        delete [] m_lonPositions;
    }
    try {
        CCommonTools::unloadAntennaBoss (m_antennaBoss, m_service);
    }
    catch (ACSErr::ACSbaseExImpl & ex) {
        ex.log (LM_WARNING);
    }
    if (m_config->getMinorServoBossComponent()!="") {
    	try {
    		CCommonTools::unloadMinorServoBoss (m_minorServoBoss, m_service);
    	}
        catch (ACSErr::ACSbaseExImpl & ex) {
            ex.log (LM_WARNING);
        }
    }
    try {
        CCommonTools::unloadObservatory (m_observatory, m_service);
    }
    catch (ACSErr::ACSbaseExImpl & ex) {
        ex.log (LM_WARNING);
    }
}

void
CEngineThread::onStart ()
{
    AUTO_TRACE ("CEngineThread::onStart()");
}

void CEngineThread::onStop ()
{
    AUTO_TRACE ("CEngineThread::onStop()");
}

void CEngineThread::initialize () throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl)
{
    AUTO_TRACE ("CEngineThread::initialize()");
    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();
    CCommonTools::getObservatory (m_observatory, m_service, m_config->getObservatoryComponent(), observatoryError); // throw ComponentErrors::CouldntGetComponentExImpl
    try {
        site = m_observatory->getSiteSummary ();
    }
    catch (CORBA::SystemException & ex) {
        _EXCPT (ComponentErrors::CORBAProblemExImpl, __dummy, "CEngineThread::initialize()");
        __dummy.setName (ex._name ());
        __dummy.setMinor (ex.minor ());
        throw __dummy;
    }
    m_site = CSite (site.out ());
    try {
        CCommonTools::unloadObservatory (m_observatory, m_service);
    }
    catch (ACSErr::ACSbaseExImpl & ex) {
        ex.log (LM_WARNING);
    }
}

bool CEngineThread::checkTime (const ACS::Time & currentTime)
{
    CSecAreaResourceWrapper < CDataCollection > m_data = m_dataWrapper->Get ();
    return (currentTime > (m_data->getFirstDumpTime () + getSleepTime () + m_timeSlice));	// gives the cache time to fill a little bit
}

bool CEngineThread::checkTimeSlot (const ACS::Time & slotStart)
{
	return true;
}

bool CEngineThread::processData ()
{
    char * buffer;			//pointer to the buffer that contains the real data
    char * bufferCopy;		// pointer to the memory that has to be freed
    bool calOn;
    long buffSize;
    double ra, dec;
    double az, el;
    double lon, lat;
    bool tracking;
    double offset = 0.0;
    IRA::CString out;
    TIMEVALUE tS;
    //double targetRA, targetDEC;
    double  targetAZ, targetEL;
    double  targetLON, targetLAT;
    double coordinate=0.0;
    ACS::Time time;

    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();

    // get tsys from devices
    if (!data->getDump (time, calOn, bufferCopy, buffer, tracking, buffSize))   return false;
    tS.value (time);
    CalibrationTool_private::getTsysFromBuffer (buffer, data->getInputsNumber (), m_ptsys);

    // we need only the tsys related to the device
    m_device = data->getDevice ();
    data->setDataY (m_ptsys[m_device]);
    m_tsysDataSeq[m_dataSeqCounter] = m_ptsys[m_device];

    CSkySource CTskySource (m_targetRa, m_targetDec, IRA::CSkySource::SS_J2000);
    CDateTime CTdateTime (tS);
    IRA::CString temp;

	try {
		CCommonTools::getAntennaBoss(m_antennaBoss,m_service,m_config->getAntennaBossComponent(),m_antennaBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		data->setStatus(Management::MNG_WARNING);
		m_antennaBoss=Antenna::AntennaBoss::_nil();
		data->detectError();
	}

	if (m_config->getMinorServoBossComponent()!="") {
		try {
			CCommonTools::getMinorServoBoss(m_minorServoBoss,m_service,m_config->getMinorServoBossComponent(),m_minorServoBossError);
		}
		catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
			ex.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			m_minorServoBoss=MinorServo::MinorServoBoss::_nil();
			data->detectError();
		}
	}

    switch (data->getScanAxis ()) {
        case Management::MNG_NO_AXIS:
        	break;
        case Management::MNG_HOR_LON:
            CTskySource.process (CTdateTime, m_site);
	        CTskySource.getApparentHorizontal (targetAZ, targetEL);
	        try {
	        	if (!CORBA::is_nil(m_antennaBoss)) {
	        		m_antennaBoss->getObservedHorizontal (time, data->getIntegrationTime () * 10000, az, el);
	        	}
	        	else data->detectError();
	        }
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_FAILURE);
				m_antennaBossError=true;
				data->detectError();
				az=el=0;
			}
	        coordinate = az;
	        offset=IRA::CIRATools::differenceBetweenAnglesRad(az,targetAZ);
	        //m_cosLat = cos (targetEL);
	        m_latPositions[m_dataSeqCounter] = targetEL;
	        m_lonPositions[m_dataSeqCounter] = targetAZ;
	        break;
        case Management::MNG_HOR_LAT:
            CTskySource.process (CTdateTime, m_site);
            CTskySource.getApparentHorizontal (targetAZ, targetEL);
            try {
            	if (!CORBA::is_nil(m_antennaBoss)) m_antennaBoss->getObservedHorizontal (time, data->getIntegrationTime () * 10000, az, el);
            	else data->detectError();
            }
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_FAILURE);
				m_antennaBossError=true;
				data->detectError();
				az=el=0;
			}
	        coordinate = el;
	        m_latPositions[m_dataSeqCounter] = targetEL;
	        m_lonPositions[m_dataSeqCounter] = targetAZ;
	        offset =  el - targetEL ;
	        break;
        case Management::MNG_EQ_LON:
        	try {
        		if (!CORBA::is_nil(m_antennaBoss)) m_antennaBoss->getObservedEquatorial (time, data->getIntegrationTime () * 10000, ra, dec);
        		else data->detectError();
        	}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_FAILURE);
				m_antennaBossError=true;
				data->detectError();
				ra=dec=0;
			}
            coordinate = ra;
            offset=IRA::CIRATools::differenceBetweenAnglesRad(ra,m_targetRa);
	        // m_cosLat = cos (m_targetDec);
            m_latPositions[m_dataSeqCounter] = m_targetDec;
	        m_lonPositions[m_dataSeqCounter] = m_targetRa;
            break;
        case Management::MNG_EQ_LAT:
        	try {
        		if (!CORBA::is_nil(m_antennaBoss)) m_antennaBoss->getObservedEquatorial (time, data->getIntegrationTime () * 10000, ra, dec);
        		else data->detectError();
        	}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_FAILURE);
				m_antennaBossError=true;
				data->detectError();
				ra=dec=0;
			}
	        coordinate = dec;
	        m_latPositions[m_dataSeqCounter] = m_targetDec;
	        m_lonPositions[m_dataSeqCounter] = m_targetRa;
	        offset=dec - m_targetDec;
	        break;
        case Management::MNG_GAL_LON:
	        CTskySource.process (CTdateTime, m_site);
	        IRA::CSkySource::equatorialToGalactic (m_targetRa,m_targetDec, targetLON, targetLAT);
	        try {
	        	if (!CORBA::is_nil(m_antennaBoss)) m_antennaBoss->getObservedGalactic (time,data->getIntegrationTime () * 10000,lon, lat);
	        	else data->detectError();
	        }
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_FAILURE);
				m_antennaBossError=true;
				data->detectError();
				lon=lat=0;
			}
	        coordinate = lon;
	        offset=IRA::CIRATools::differenceBetweenAnglesRad(lon,targetLON);
	        //m_cosLat = cos (targetLAT);
            m_latPositions[m_dataSeqCounter] =targetLAT;
	        m_lonPositions[m_dataSeqCounter] = targetLON;
            break;
        case Management::MNG_GAL_LAT:
	        CTskySource.process (CTdateTime, m_site);
	        IRA::CSkySource::equatorialToGalactic (m_targetRa, m_targetDec, targetLON, targetLAT);
	        try {
	        	if (!CORBA::is_nil(m_antennaBoss)) m_antennaBoss->getObservedGalactic (time, data->getIntegrationTime () * 10000,lon, lat);
	        	else data->detectError();
	        }
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_FAILURE);
				m_antennaBossError=true;
				data->detectError();
				lon=lat=0;
			}
	        coordinate = lat;
	        m_latPositions[m_dataSeqCounter] = targetLAT;
	        m_lonPositions[m_dataSeqCounter] = targetLON;
	        offset = lat - targetLAT;
        break;
        case Management::MNG_SUBR_Z: {
        	ACS::doubleSeq_var positions;
	        try {
	        	if (!CORBA::is_nil(m_antennaBoss)) {
	        		m_antennaBoss->getObservedHorizontal (time, data->getIntegrationTime () * 10000, az, el);
	        	}
	        	else data->detectError();
	        }
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_FAILURE);
				m_antennaBossError=true;
				data->detectError();
				az=el=0;
			}
			if (!CORBA::is_nil(m_minorServoBoss)) {
				try {
					positions=m_minorServoBoss->getAxesPosition(time+(data->getIntegrationTime () * 10000)/2);
					if (m_minorServoCurrentAxisPosition!=-1) {
						coordinate=positions[m_minorServoCurrentAxisPosition];
					}
					else {
						data->detectError();
						coordinate=m_focusScanCenter;
					}
				}
				catch (ManagementErrors::ConfigurationErrorEx &ex) {
					_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::processData()");
					impl.setOperationName("getAxesPosition()");
					impl.setComponentName((const char *)m_config->getMinorServoBossComponent());
					impl.log(LM_ERROR);
					data->setStatus(Management::MNG_WARNING);
					coordinate=m_focusScanCenter;
					data->detectError();
				}
				catch (CORBA::SystemException& ex) {
					_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::getMinorServoData()");
					impl.setName(ex._name());
					impl.setMinor(ex.minor());
					impl.log(LM_ERROR);
					data->setStatus(Management::MNG_WARNING);
					coordinate=m_focusScanCenter;
					data->detectError();
					m_minorServoBossError=true;
				}
			}
			else {
				coordinate=0;
			}
			offset=coordinate-m_focusScanCenter;
	        m_latPositions[m_dataSeqCounter] = az;
	        m_lonPositions[m_dataSeqCounter] = el;
	        break;
        }
        case Management::MNG_SUBR_X:
	    break;
        case Management::MNG_SUBR_Y:
	    break;
        case Management::MNG_PFP_Z:

	    break;
        case Management::MNG_PFP_Y: {
        	ACS::doubleSeq_var positions;
	        try {
	        	if (!CORBA::is_nil(m_antennaBoss)) {
	        		m_antennaBoss->getObservedHorizontal (time, data->getIntegrationTime () * 10000, az, el);
	        	}
	        	else data->detectError();
	        }
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::processData()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_FAILURE);
				m_antennaBossError=true;
				data->detectError();
				az=el=0;
			}
			if (!CORBA::is_nil(m_minorServoBoss)) {
				try {
					positions=m_minorServoBoss->getAxesPosition(time+(data->getIntegrationTime () * 10000)/2);
					if (m_minorServoCurrentAxisPosition!=-1) {
						coordinate=positions[m_minorServoCurrentAxisPosition];
					}
					else {
						data->detectError();
						coordinate=m_focusScanCenter;
					}
				}
				catch (ManagementErrors::ConfigurationErrorEx &ex) {
					_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::processData()");
					impl.setOperationName("getAxesPosition()");
					impl.setComponentName((const char *)m_config->getMinorServoBossComponent());
					impl.log(LM_ERROR);
					data->setStatus(Management::MNG_WARNING);
					coordinate=m_focusScanCenter;
					data->detectError();
				}
				catch (CORBA::SystemException& ex) {
					_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::getMinorServoData()");
					impl.setName(ex._name());
					impl.setMinor(ex.minor());
					impl.log(LM_ERROR);
					data->setStatus(Management::MNG_WARNING);
					coordinate=m_focusScanCenter;
					data->detectError();
					m_minorServoBossError=true;
				}
			}
			else {
				coordinate=0;
			}
			offset=coordinate-m_focusScanCenter;
	        m_latPositions[m_dataSeqCounter] = az;
	        m_lonPositions[m_dataSeqCounter] = el;
	        break;
        }
        default:
        break;
    }
    data->setDataX (coordinate);
    m_dataSeq[m_dataSeqCounter] = coordinate;

    // offsets array for fit2 function
    if (offset != 0.0)
        m_off[m_dataSeqCounter] = offset;
    else
        m_off[m_dataSeqCounter] = 0.0;	// could be the center?

    // seconds from midnight array for fit2 function
    m_secsFromMidnight[m_dataSeqCounter] = tS.hour () * 3600.0 + tS.minute () * 60.0 + tS.second () + (tS.microSecond () / 1000000.0);

    //source, scu, axis, offset

    if (m_fileOpened) {
    	if (data->isPointingScan()) {
    		if (data->getCoordIndex() == 1) {
    			out.Format ("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#lat ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    			m_file << (const char *) out;
    		}
    		if (data->getCoordIndex() == 0) {
    			out.Format ("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#lon ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    			m_file << (const char *) out;
    		}
    		m_file << m_dataSeqCounter << " " << m_secsFromMidnight[m_dataSeqCounter] << " " << m_off[m_dataSeqCounter]*DR2D << " " << m_tsysDataSeq[m_dataSeqCounter] << std::endl;
    	}
    	else if (data->isFocusScan()) {
    		out.Format ("%04d.%03d.%02d:%02d:%02d.%02d#peakf#axis ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    		 m_file << (const char *) out;
    		 if (data->getCoordIndex() == 2) {
    			 m_file << "Zp" << " " << m_dataSeqCounter << " " << m_secsFromMidnight[m_dataSeqCounter] << " " << m_off[m_dataSeqCounter] << " " << m_tsysDataSeq[m_dataSeqCounter] << std::endl;
    		 }
    		 if (data->getCoordIndex() == 3) {
    			 m_file << "Zs" << " " << m_dataSeqCounter << " " << m_secsFromMidnight[m_dataSeqCounter] << " " << m_off[m_dataSeqCounter] << " " << m_tsysDataSeq[m_dataSeqCounter] << std::endl;
    		 }
    	}
    }
    m_dataSeqCounter++;

    delete[]bufferCopy;

    return true;
}

void CEngineThread::runLoop ()
{
	TIMEVALUE now;
    TIMEVALUE tS;
    IRA::CString out;
    IRA::CString fileName;
    IRA::CString filePath;
    IRA::CString projectName;

    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();
    IRA::CIRATools::getTime (now);	// it marks the start of the activity job

    if (data->isReset()) {
		if (m_fileOpened) {
			m_file.close();
			ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()",(LM_NOTICE,"FILE_CLOSED"));
			m_fileOpened=false;
		}
		data->haltResetStage();
		return;
	}
    if (data->isStart() && data->isReady() &&  data->isScanHeaderReady() && data->isSubScanHeaderReady()) {
    	data->setStatus (Management::MNG_OK);
    	if (!m_fileOpened && m_config->outputFile()) {
    		data->getFileName(fileName,filePath);
    		if (!IRA::CIRATools::directoryExists(filePath)) {
    			if (!IRA::CIRATools::makeDirectory(filePath)) {
    				_EXCPT(ComponentErrors::FileIOErrorExImpl,impl,"CEngineThread::runLoop()");
    				impl.setFileName((const char *)filePath);
    				impl.log(LM_ERROR);
    				data->setStatus(Management::MNG_FAILURE);
    			}
    			else {
    				ACS_LOG(LM_FULL_INFO,"CEngineThread::runLoop()",(LM_NOTICE,"DATA_FOLDER_CREATED: %s",(const char *)filePath));
    			}
    		}
    		m_file.open ((const char *) data->getFileName(), ios_base::out | ios_base::app);
    		if (!m_file.is_open ()) {
    			_EXCPT (ComponentErrors::FileIOErrorExImpl, impl, "CEngineThread::runLoop()");
    			impl.setFileName ((const char *) fileName);
    			impl.log (LM_ERROR);
    			data->setStatus (Management::MNG_WARNING);
    		}
    		else {
    			m_fileOpened = true;
    			prepareFile(now.value().value);
    		}
    	}
    	if (data->isPointingScan()) {
    		if (data->isPointingScanClosed()) { // done only on the  first subscan of the cross scan!
    			getAntennaData();
    			if (m_fileOpened) {
    				writePointingFileHeaders(now.value().value);
    			}
    		}
    	}
    	else if (data->isFocusScan()) {
    		if (data->isFocusScanClosed()) {
    			getAntennaData();
    			getMinorServoData();
    			if (m_fileOpened) {
    				writeFocusFileHeaders(now.value().value);
    			}
    		}
    	}
        data->startRunnigStage ();
	  }
	  else if (data->isStop ()) {
		//save all the data in the buffer and then finalize the file
		  while (processData());
		  gaussFit(now.value().value);
		  setAxisOffsets();
		  if (data->isPointingScan()) {
			  if (data->isPointingScanDone()) {
				  data->closePointingScan();
				  m_lonResult = m_latResult = 0;
			  }
		  }
		  else if (data->isFocusScan()) {
			  if (data->isFocusScanDone()) {
				  data->closeFocusScan();
				  m_focusResult =0;
			  }
		  }
		  data->haltStopStage ();
	  }
	  else if (data->isRunning ()) {
		  // file was already created, then saves the data into it
		  // until there is something to process and there is still time available
		  while (checkTime (now.value ().value)  && checkTimeSlot (now.value ().value) && processData ());
	  }
}

void  CEngineThread::getMinorServoData()
{
	m_focusScanCenter=0.0;
	m_minorServoCurrentAxisPosition=-1;
	if (m_config->getMinorServoBossComponent()=="") {
		return;
	}
	ACS::stringSeq_var axesNames;
	ACS::stringSeq_var axesUnits;
	CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	try {
		CCommonTools::getMinorServoBoss(m_minorServoBoss,m_service,m_config->getMinorServoBossComponent(),m_minorServoBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		data->setStatus(Management::MNG_WARNING);
		m_minorServoBoss=MinorServo::MinorServoBoss::_nil();
		data->detectError();
	}
	if (!CORBA::is_nil(m_minorServoBoss)) {
		try {
			m_focusScanCenter=m_minorServoBoss->getCentralScanPosition();
			ACS_LOG (LM_FULL_INFO, "CEngineThread::getMinorServoData()", (LM_DEBUG, "FOCUS_SCAN_CENTER  %lf",m_focusScanCenter));
		}
		catch (ManagementErrors::SubscanErrorEx &ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::getMinorServoData()");
			impl.setOperationName("getCentralScanPosition()");
			impl.setComponentName((const char *)m_config->getMinorServoBossComponent());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			m_focusScanCenter=0.0;
			data->detectError();
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::getMinorServoData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			m_focusScanCenter=0.0;
			data->detectError();
			m_minorServoBossError=true;
		}
		try {
			unsigned pos;
			m_minorServoBoss->getAxesInfo(axesNames.out(),axesUnits.out());
			if  (!data->getMinorServoAxisPosition(axesNames.in(),pos)) {
				m_minorServoCurrentAxisPosition=-1;
				data->detectError();
			}
			else {
				m_minorServoCurrentAxisPosition=pos;
			}
		}
		catch (ManagementErrors::ConfigurationErrorEx &ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::getMinorServoData()");
			impl.setOperationName("getAxesInfo()");
			impl.setComponentName((const char *)m_config->getMinorServoBossComponent());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			data->detectError();
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::getMinorServoData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			data->detectError();
			m_minorServoBossError=true;
		}
	}
}


void  CEngineThread::getAntennaData()
{
	CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	try {
		CCommonTools::getAntennaBoss(m_antennaBoss,m_service,m_config->getAntennaBossComponent(),m_antennaBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		data->setStatus(Management::MNG_WARNING);
		m_antennaBoss=Antenna::AntennaBoss::_nil();
		data->detectError();
	}
	if (!CORBA::is_nil(m_antennaBoss)) {
		ACSErr::Completion_var comp;
		CORBA::Double fwhm,targetRa,targetDec,sourceFlux;
		CORBA::String_var target;
		try { //get the target name and parameters
			ACS::ROdouble_var fwhmRef,targetRaRef,targetDecRef,sourceFluxRef;
			ACS::ROstring_var targetRef;
			fwhmRef=m_antennaBoss->FWHM();
			fwhm=fwhmRef->get_sync(comp.out());
			ACSErr::CompletionImpl fwhmCompl(comp);
			if (!fwhmCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,fwhmCompl,"CEngineThread::getAntennaData()");
				impl.setAttributeName("FWHM");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_fwhm=0.0;
				data->detectError();
			}
			else {
				m_fwhm=(double)fwhm;
			}
			targetRaRef=m_antennaBoss->targetRightAscension();
			targetRa=targetRaRef->get_sync(comp.out());
			ACSErr::CompletionImpl targetRaCompl(comp);
			if (!targetRaCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,targetRaCompl,"CEngineThread::getAntennaData()");
				impl.setAttributeName("targetRightAscension");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_targetRa=0.0;
				data->detectError();
			}
			else {
				m_targetRa=(double)targetRa;
			}
			targetDecRef=m_antennaBoss->targetDeclination();
			targetDec=targetDecRef->get_sync(comp.out());
			ACSErr::CompletionImpl targetDecCompl(comp);
			if (!targetDecCompl.isErrorFree()) {
				_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,targetDecCompl,"CEngineThread::getAntennaData()");
				impl.setAttributeName("declination");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_targetDec=0.0;
				data->detectError();
			}
			else {
				m_targetDec=(double)targetDec;
			}
		    targetRef = m_antennaBoss->target ();
		    target = targetRef->get_sync (comp.out ());
		    ACSErr::CompletionImpl targetCompl (comp);
		    if (!targetCompl.isErrorFree ()) {
		    	_ADD_BACKTRACE (ComponentErrors::CouldntGetAttributeExImpl, impl, targetCompl, "CEngineThread::getAntennaData()");
		    	impl.setAttributeName ("target");
		    	impl.setComponentName ((const char *) m_config->getAntennaBossComponent());
		    	impl.log (LM_ERROR);
		    	data->setStatus (Management::MNG_WARNING);
		    	data->setSourceName ("");
		    	data->detectError();
		    }
		    else  {
		    	data->setSourceName ((const char *)target);
		    }
		    sourceFluxRef = m_antennaBoss->targetFlux();
		    sourceFlux = sourceFluxRef->get_sync(comp.out());
		    ACSErr::CompletionImpl sourceFluxCompl (comp);
		    if (!sourceFluxCompl.isErrorFree ()) {
		    	_ADD_BACKTRACE (ComponentErrors::CouldntGetAttributeExImpl, impl, targetCompl, "CEngineThread::getAntennaData()");
		    	impl.setAttributeName ("targetFlux");
		    	impl.setComponentName ((const char *) m_config->getAntennaBossComponent());
		    	impl.log (LM_ERROR);
		    	data->setStatus (Management::MNG_WARNING);
		    	data->setSourceFlux (0.0);
		    	data->detectError();
		    }
		    else  {
		    	data->setSourceFlux (sourceFlux);
		    }
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::getAntennaData()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log(LM_ERROR);
			data->setStatus(Management::MNG_WARNING);
			m_antennaBossError=true;
			m_fwhm=0.0;
			m_targetRa=m_targetDec=0.0;
			data->setSourceName ("");
			data->detectError();
		}
	}
}

void CEngineThread::gaussFit(const ACS::Time& now)
{
    int i;
    // fit2 function parameters
    static integer ftry = 20;
    static real tol = (float) .001;
    static integer par = 5;
    double tmid, tmax, ti;
    int imax;
    TIMEVALUE tS;
    IRA::CString out;

    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();

	for (i = 0; i < m_dataSeqCounter; i++)
		m_ptsys2[i] = (float) m_tsysDataSeq[i];

    tmid =  m_secsFromMidnight[((m_dataSeqCounter + 1) / 2) - 1];
    m_Par[4] =  (m_ptsys2[m_dataSeqCounter - 1] -   m_ptsys2[0]) /  (m_secsFromMidnight[m_dataSeqCounter - 1] -   m_secsFromMidnight[0]);
    m_Par[3] = m_ptsys2[0] + m_Par[4] * (tmid - m_secsFromMidnight[0]);
    if (m_dataSeqCounter < 5) {
	    m_Par[3] = m_Par[4] = (float) 0.;
	}
    m_errPar[3] = m_errPar[4] = (float) 0.;
    m_secsFromMidnight[0] -= tmid;
    tmax =  m_ptsys2[0] - (m_Par[3] + m_Par[4] * m_secsFromMidnight[0]);
    imax = 1;
    for (i = 2; i <= m_dataSeqCounter; ++i) {
    	m_secsFromMidnight[i - 1] -= tmid;
	    ti = m_ptsys2[i - 1] - (m_Par[3] +  m_Par[4] *  m_secsFromMidnight[i - 1]);
	    if (tmax >= ti)  {
	    	goto gaussianFit;  // break ????
	    }
	    tmax = ti;
	    imax = i;
	    gaussianFit:
	    ;
    }
    m_Par[0] = tmax;
    m_Par[1] = m_off[imax - 1];

    if (data->getCoordIndex() == 1) {	// LAT scans
    	double offMin,offMax;
    	double toll;
	    m_Par[2] = m_fwhm;
	    int pos=computeScanCenter(m_off,m_dataSeqCounter);
	    fit2_ (m_off, m_ptsys2, m_secsFromMidnight, m_Par, m_errPar, (integer *)&m_dataSeqCounter, &par, &tol, &ftry, (E_fp) fgaus_, &m_reducedCHI, &m_ierr);


	    m_LatPos =m_latPositions[pos];
	    m_LatOff = m_Par[1];
	    m_LatErr = m_errPar[1];

	    m_latAmp=m_Par[0];
	    m_latAmpErr=m_errPar[0];
        m_latFwhm=m_Par[2];
        m_latFwhmErr=m_errPar[2];

	    if (m_fileOpened) {
	    	// latfit, laterr
	    	tS.value (now/*.value().value*/);
	    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#latfit ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
	    	m_file << (const char *) out;
	    	m_file << m_Par[1] * DR2D << " " << m_Par[2] * DR2D << " " << m_Par[0] << " " << m_Par[3] << " " << m_Par[4] << " " << m_ierr << std::endl;

	    	/*tS.value (now.value().value);*/
	    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#laterr ",tS.year (), tS.dayOfYear (), tS.hour (),tS.minute (), tS.second (),(long)(tS.microSecond () / 10000.));
	    	m_file << (const char *) out;
	    	m_file << m_errPar[1] * DR2D << " " << m_errPar[2] * DR2D<< " " << m_errPar[0] << " " << m_errPar[3] << " " << m_errPar[4] << " " << m_reducedCHI << std::endl;
	    }
	    ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "LATFIT  %lf %lf %lf %lf %lf %d",m_Par[1] * DR2D, m_Par[2] * DR2D, m_Par[0] , m_Par[3], m_Par[4], m_ierr));
	    m_latResult=0;
	    offMin=GETMIN(m_off[0],m_off[m_dataSeqCounter-1]);
	    offMax=GETMAX(m_off[0],m_off[m_dataSeqCounter-1]);
	    toll=1+m_config->getFWHMTolerance();
	    if (!data->isErrorDetected()) {
			if (m_ierr>0) {
				if ((m_Par[1]>offMin) && (m_Par[1]<offMax)) {
					if ((m_Par[2]<m_fwhm*toll) && (m_Par[2]>m_fwhm/toll)) {
						m_latResult = 1;
					}
					else {
						ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "INVALID_FWHM_IN_LATFIT"));
					}
				}
				else {
					ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "INVALID_OFFSET_IN_LATFIT"));
				}
			}
			else {
				ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "LATFIT_DID_NOT_CONVERGE"));
			}
	    }
	    else {
			ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "ERROR_DETECTED_DURING_LAT_SCAN"));
	    }
	    data->setAmplitude (m_Par[0]);
	    data->setPeakOffset (m_Par[1]);
	    data->setHPBW (m_Par[2]);
	    data->setOffset (m_Par[3]);
	    data->setSlope (m_Par[4]);
	    data->setArrayDataX (m_dataSeq,m_dataSeqCounter);
	    data->setArrayDataY (m_tsysDataSeq,m_dataSeqCounter);
	    for (i = 0; i < m_dataSeqCounter; i++) {
	    	m_dataSeq[i] = 0.0;
	    	m_tsysDataSeq[i] = 0.0;
	    }
	    m_dataSeqCounter = 0;
	    data->setLatDone();
	} // m_coordIndex==1
    else if ((data->getCoordIndex() == 0)) {	// LON scans
    	int pos=computeScanCenter(m_off,m_dataSeqCounter);
    	double tempLat;
    	double offMin,offMax;
    	double toll;
	    m_LonPos=m_lonPositions[pos];
	    tempLat=m_latPositions[pos];
	    m_cosLat=cos(tempLat);
	    m_Par[2] = m_fwhm / m_cosLat;
	    fit2_ (m_off, m_ptsys2, m_secsFromMidnight, m_Par, m_errPar, (integer *)&m_dataSeqCounter, &par, &tol, &ftry, (E_fp) fgaus_, &m_reducedCHI, &m_ierr);

	    m_Par[2] *=m_cosLat;
	    m_errPar[2] *=m_cosLat;
	    m_LonOff = m_Par[1];
	    m_LonErr = m_errPar[1];

        m_lonAmp=m_Par[0];
	    m_lonAmpErr=m_errPar[0];
        m_lonFwhm=m_Par[2];
        m_lonFwhmErr=m_errPar[2];

	    if (m_fileOpened) {
	    	// lonfit, lonerr
	    	tS.value (now/*.value().value*/);
	    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#lonfit ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
	    	m_file << (const char *) out;
	    	m_file << m_Par[1] * DR2D << " " << m_Par[2] * DR2D << " " << m_Par[0] << " " <<  m_Par[3] << " " << m_Par[4] << " " << m_ierr << std::endl;
	    	/*tS.value (now.value().value);*/
	    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#lonerr ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
	    	m_file << (const char *) out;
	    	m_file << m_errPar[1] * DR2D << " " << m_errPar[2] * DR2D << " " << m_errPar[0] << " " << m_errPar[3] << " " << m_errPar[4] << " " << m_reducedCHI << std::endl;
	    }
		ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "LONFIT  %lf %lf %lf %lf %lf %d",m_Par[1] * DR2D, m_Par[2] * DR2D, m_Par[0] , m_Par[3], m_Par[4], m_ierr));
	    m_lonResult=0;
	    offMin=GETMIN(m_off[0],m_off[m_dataSeqCounter-1]);
	    offMax=GETMAX(m_off[0],m_off[m_dataSeqCounter-1]);
	    toll=1+m_config->getFWHMTolerance();
	    if (!data->isErrorDetected()) {
			if (m_ierr>0) {
				if ((m_Par[1]>offMin) && (m_Par[1]<offMax)) {
					if ((m_Par[2]<m_fwhm*toll) && (m_Par[2]>m_fwhm/toll)) {
						m_lonResult = 1;
					}
					else {
						ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "INVALID_FWHM_IN_LONFIT"));
					}
				}
				else {
					ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "INVALID_OFFSET_IN_LONFIT"));
				}
			}
			else {
				ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "LONFIT_DID_NOT_CONVERGE"));
			}
	    }
	    else {
			ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "ERROR_DETECTED_DURING_LON_SCAN"));
	    }
	    data->setAmplitude (m_Par[0]);
		data->setPeakOffset (m_Par[1]);
		data->setHPBW (m_Par[2]);
		data->setOffset (m_Par[3]);
		data->setSlope (m_Par[4]);
	    data->setArrayDataX (m_dataSeq,m_dataSeqCounter);
	    data->setArrayDataY (m_tsysDataSeq,m_dataSeqCounter);
	    for (i = 0; i < m_dataSeqCounter; i++) {
		  m_dataSeq[i] = 0.0;
		  m_tsysDataSeq[i] = 0.0;
	    }
	    m_dataSeqCounter = 0;
	   data->setLonDone();
	} // m_CoordIndex == 0
    if (data->isPointingScan()) {
		if (data->isPointingScanDone()) {
			if (m_fileOpened) {
				// offset m_LonPos, m_LatPos, m_lonOff, m_latOff, m_lonResult, m_latResult
				tS.value (now/*.value().value*/);
				out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#offset ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
				m_file << (const char *) out;
				m_file << m_LonPos * DR2D << " " <<  m_LatPos * DR2D << " " << m_LonOff * DR2D << " " << m_LatOff * DR2D << " " << m_lonResult << " " << m_latResult << std::endl;

				// xoffset m_LonPos, m_LatPos, m_lonOff, m_latOff, m_LonErr, m_LatErr, m_lonResult, m_latResult
				tS.value (now/*.value().value*/);
				out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#xoffset ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
				m_file << (const char *) out;
				m_file << m_LonPos * DR2D << " " <<  m_LatPos * DR2D << " " << m_cosLat* m_LonOff * DR2D << " " << m_LatOff * DR2D << " " << m_cosLat * m_LonErr * DR2D << " " << m_LatErr * DR2D << " " << m_lonResult << " " << m_latResult << std::endl;
				out.Format("%04d.%03d.%02d:%02d:%02d.%02d#xgain", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
				m_file << (const char *) out << " " << (const char *)data->getSourceName() << " ";
				m_file << m_LonPos * DR2D << " " <<  m_LatPos * DR2D << " " << m_lonAmp << " " << m_lonAmpErr << " " << m_latAmp << " " << m_latAmpErr << " " << m_lonFwhm * DR2D << " " << m_lonFwhmErr * DR2D << " " \
						<< m_latFwhm * DR2D << " " << m_latFwhmErr * DR2D 	<< " " << data->getSourceFlux() << " " << m_lonResult << " " << m_latResult << std::endl;
			}
			ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "OFFSETS = %lf %lf %lf %lf %d %d",
				m_LonPos * DR2D, m_LatPos * DR2D, m_LonOff * DR2D, m_LatOff * DR2D, m_lonResult, m_latResult));
			ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "XOFFSETS = %lf %lf %lf %lf %lf %lf %d %d",
				m_LonPos * DR2D, m_LatPos * DR2D, m_cosLat * m_LonOff * DR2D, m_LatOff * DR2D, m_cosLat * m_LonErr * DR2D, m_LatErr * DR2D, m_lonResult, m_latResult));

			ACS_LOG (LM_FULL_INFO, "CEngineThread::gaussFit()", (LM_NOTICE, "XGAIN =%s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d",(const char *)data->getSourceName(),
				m_LonPos * DR2D, m_LatPos * DR2D, m_lonAmp, m_lonAmpErr, m_latAmp, m_latAmpErr, m_lonFwhm * DR2D, m_lonFwhmErr * DR2D, m_latFwhm * DR2D, m_latFwhmErr * DR2D, data->getSourceFlux(), m_lonResult, m_latResult));
		}
    }
    else if (data->isFocusScan()) {
    	/**********************/
    	//OUTPUT OF FOCUS SCAN
    	/************************/
    }
}

void CEngineThread::setAxisOffsets()
{
	CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();
	try {
		CCommonTools::getAntennaBoss(m_antennaBoss,m_service,m_config->getAntennaBossComponent(),m_antennaBossError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		ex.log(LM_ERROR);
		data->setStatus(Management::MNG_WARNING);
		m_antennaBoss=Antenna::AntennaBoss::_nil();
		return;
	}
    switch (data->getScanAxis ()) {
	   	case Management::MNG_NO_AXIS:
	   		break;
	   	case Management::MNG_HOR_LON:
	   		try {
	   			if (!CORBA::is_nil(m_antennaBoss)) {
	   				if ((data->isLonDone()) && (m_lonResult)) {
	   					m_antennaBoss->getAllOffsets (m_azUserOff, m_elUserOff, m_raUserOff, m_decUserOff, m_lonUserOff, m_latUserOff);
	   					m_antennaBoss->setOffsets (m_cosLat * m_LonOff , m_elUserOff, Antenna::ANT_HORIZONTAL);
	   				}
	   			}
	   		}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::setAxisOffsets()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_antennaBossError=true;
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
			catch (AntennaErrors::AntennaErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
	   		break;
	   	case Management::MNG_HOR_LAT:
	   		try {
	   			if (!CORBA::is_nil(m_antennaBoss)) {
	   				if ((data->isLatDone()) && (m_latResult)) {
	   					m_antennaBoss->getAllOffsets (m_azUserOff, m_elUserOff, m_raUserOff, m_decUserOff, m_lonUserOff, m_latUserOff);
	   					m_antennaBoss->setOffsets (m_azUserOff, /*m_elUserOff + */m_LatOff, Antenna::ANT_HORIZONTAL);
	   				}
	   			}
	   		}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::setAxisOffsets()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_antennaBossError=true;
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
			catch (AntennaErrors::AntennaErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
	   		break;
	   	case Management::MNG_EQ_LON:
	   		try {
	   			if (!CORBA::is_nil(m_antennaBoss)) {
	   				if ((data->isLonDone()) && (m_lonResult)) {
	   					m_antennaBoss->getAllOffsets (m_azUserOff, m_elUserOff, m_raUserOff, m_decUserOff, m_lonUserOff, m_latUserOff);
	   					m_antennaBoss->setOffsets (m_cosLat * m_LonOff, m_decUserOff, Antenna::ANT_EQUATORIAL);
	   				}
	   			}
	   		}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::setAxisOffsets()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_antennaBossError=true;
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
			catch (AntennaErrors::AntennaErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
	   		break;
	   	case Management::MNG_EQ_LAT:
	   		try {
	   			if (!CORBA::is_nil(m_antennaBoss)) {
	   				if ((data->isLatDone()) && (m_latResult)) {
	   					m_antennaBoss->getAllOffsets (m_azUserOff, m_elUserOff, m_raUserOff, m_decUserOff, m_lonUserOff, m_latUserOff);
	   					m_antennaBoss->setOffsets (m_raUserOff, /*m_decUserOff + */m_LatOff , Antenna::ANT_EQUATORIAL);
	   				}
	   			}
	   		}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::setAxisOffsets()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_antennaBossError=true;
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
			catch (AntennaErrors::AntennaErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
	   		break;
	   	case Management::MNG_GAL_LON:
	   		try {
	   			if (!CORBA::is_nil(m_antennaBoss)) {
	   				if ((data->isLonDone()) && (m_lonResult)) {
	   					m_antennaBoss->getAllOffsets (m_azUserOff, m_elUserOff, m_raUserOff, m_decUserOff, m_lonUserOff, m_latUserOff);
	   					m_antennaBoss->setOffsets (m_cosLat* m_LonOff,m_latUserOff, Antenna::ANT_GALACTIC);
	   				}
   				}
	   		}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::setAxisOffsets()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_antennaBossError=true;
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
			catch (AntennaErrors::AntennaErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
	   		break;
	   	case Management::MNG_GAL_LAT:
	   		try {
	   			if (!CORBA::is_nil(m_antennaBoss)) {
	   				if ((data->isLatDone()) && (m_latResult)) {
	   					m_antennaBoss->getAllOffsets (m_azUserOff, m_elUserOff, m_raUserOff, m_decUserOff, m_lonUserOff, m_latUserOff);
	   					m_antennaBoss->setOffsets (m_lonUserOff, /*m_latUserOff + */m_LatOff, Antenna::ANT_GALACTIC);
	   				}
	   			}
	   		}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CEngineThread::setAxisOffsets()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
				m_antennaBossError=true;
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
			catch (AntennaErrors::AntennaErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CEngineThread::setAxisOffsets()");
				impl.setOperationName("setOffsets()");
				impl.setComponentName((const char *)m_config->getAntennaBossComponent());
				impl.log(LM_ERROR);
				data->setStatus(Management::MNG_WARNING);
			}
	   		break;
	   	case Management::MNG_SUBR_Z:
	   		break;
	   	case Management::MNG_SUBR_X:
	   		break;
	   	case Management::MNG_SUBR_Y:
	   		break;
	   	case Management::MNG_PFP_Z:
	   		break;
	   	case Management::MNG_PFP_Y:
	   		break;
	   	default:
	   		break;
    }
}

void CEngineThread::prepareFile(const ACS::Time& now)
{
	TIMEVALUE tS;
	IRA::CString out;
	IRA::CString fileName;
	IRA::CString projectName;
    IRA::CString observerName;

    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();
	tS.value (now/*.value().value*/);
	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#Calibration Tool Start\n", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (),(long)(tS.microSecond () / 10000.));
	m_file << (const char *) out;
    // File Name
    fileName = data->getFileName();
    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#File Name: ", tS.year(),tS.dayOfYear(),tS.hour(),tS.minute(),tS.second(),(long)(tS.microSecond()/10000.));
    m_file << (const char *) out;
    m_file << (const char *) fileName << std::endl;
    // Project Name
    projectName = data->getProjectName ();
    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#Project Name: ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    m_file << (const char *) out;
    m_file << (const char *) projectName << std::endl;
    // Observer Name
    observerName = data->getObserverName ();
    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#Observer Name: ",tS.year (), tS.dayOfYear (), tS.hour (),  tS.minute (), tS.second (),  (long)(tS.microSecond () / 10000.));
    m_file << (const char *) out;
    m_file << (const char *) observerName << std::endl;
    // Source Name
    ACS_LOG (LM_FULL_INFO, "CEngineThread::prepareFile()", (LM_NOTICE, "FILE_OPENED %s", (const char *) data->getFileName ()));
}

void CEngineThread::writeFocusFileHeaders(const ACS::Time& now)
{
	TIMEVALUE tS;
	IRA::CString sourceName;
    IRA::CString out;

    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();
    sourceName=data->getSourceName();

    tS.value (now/*.value().value*/);
    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#peakf#source ",tS.year (), tS.dayOfYear (), tS.hour (),  tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    m_file << (const char *) out;
    m_file << (const char *) sourceName;
    IRA::CIRATools::radToHourAngle(m_targetRa,out);
    m_file << (const char *) out;
    IRA::CIRATools::radToSexagesimalAngle(m_targetDec,out);
    m_file << (const char *) out;
    m_file << " 2000.0" << std::endl;
    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#peakf#parameters z 0 0 0 0 0 ",tS.year (), tS.dayOfYear (), tS.hour (),  tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    m_file << (const char *) out;
    m_file << std::endl;
}

void CEngineThread::writePointingFileHeaders(const ACS::Time& now)
{
	TIMEVALUE tS;
	IRA::CString sourceName;
    double sourceFlux;
    IRA::CString out;

    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();
    sourceName=data->getSourceName();
    sourceFlux=data->getSourceFlux();
    tS.value (now/*.value().value*/);


    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#source ",tS.year (), tS.dayOfYear (), tS.hour (),  tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    m_file << (const char *) out;
    m_file << (const char *) sourceName;
    m_file << " 000000.0 +000000 0000.0 0000.000.00:00:00" << std::endl;

    switch (data->getScanAxis ()) {
    case Management::MNG_NO_AXIS:
    	break;
    case Management::MNG_HOR_LON:
    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept azel 0 0 0  0 nn  0  0 ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    	break;
    case Management::MNG_HOR_LAT:
    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept azel 0 0 0  0 nn  0  0 ",tS.year (), tS.dayOfYear (), tS.hour (),	 tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    	break;
    case Management::MNG_EQ_LON:
    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept hadc 0 0 0  0 nn  0  0 ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    	break;
    case Management::MNG_EQ_LAT:
    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept hadc 0 0 0  0 nn  0  0 ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    	break;
    case Management::MNG_GAL_LON:
    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept gall 0 0 0  0 nn  0  0 ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    	break;
    case Management::MNG_GAL_LAT:
    	out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept gall 0 0 0  0 nn  0  0 ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
    	break;
    case Management::MNG_SUBR_Z:
    	break;
    case Management::MNG_SUBR_X:
    	break;
    case Management::MNG_SUBR_Y:
    	break;
    case Management::MNG_PFP_Z:
    	break;
    case Management::MNG_PFP_Y:
    	break;
    case Management::MNG_BEAMPARK:
    	break;
    case Management::MNG_TRACK:
    	break;
    case Management::MNG_GCIRCLE:
    	break;
    }
    m_file << (const char *) out;
    m_file << sourceFlux << std::endl;
}

int CEngineThread::computeScanCenter(float *vect,const int& size)
{
	double  min=fabs(vect[0]);
	double abs;
	int pos=0;
	for (int j=1;j<size;j++) {
		abs=fabs(vect[j]);
		if (abs<min){
			min=abs;
			pos=j;
		}
	}
	return pos;
}

