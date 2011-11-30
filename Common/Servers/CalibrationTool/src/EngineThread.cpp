// $Id: EngineThread.cpp,v 1.16 2011-06-13 11:46:21 c.migoni Exp $

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
    antennaBossError = false;
    m_dataSeq.length (DATACOORDINATESSEQLENGTH);
    m_tsysDataSeq.length (DATATSYSSEQLENGTH);
    m_dataSeqCounter = 0;
    m_coordinate = m_lastCoordinate = 0;
    m_off = new float[DATATSYSSEQLENGTH];
    m_secsFromMidnight = new float[DATATSYSSEQLENGTH];
    m_Par = new float[PARAMETERNUMBER];
    m_errPar = new float[PARAMETERNUMBER];
    m_observatory = Antenna::Observatory::_nil ();
    observatoryError = false;
    m_lonDone = m_latDone = 0;
    m_LatOff = m_LonOff = 0.0;
    m_latPositions = new double[DATACOORDINATESSEQLENGTH];
    m_checkScan = 0;
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
    try {
        CCommonTools::unloadAntennaBoss (m_antennaBoss, m_service);
    }
    catch (ACSErr::ACSbaseExImpl & ex) {
        ex.log (LM_WARNING);
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
    if (m_fileOpened) {
        m_file.close ();
        m_fileOpened = false;
    }
}

void CEngineThread::initialize ()
{
    AUTO_TRACE ("CEngineThread::initialize()");
    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();

    try {
        CCommonTools::getAntennaBoss (m_antennaBoss, m_service, m_config->getAntennaBossComponent(), antennaBossError);
    }
    catch (ComponentErrors::CouldntGetComponentExImpl & ex) {
        _IRA_LOGFILTER_LOG_EXCEPTION (ex, LM_ERROR);
        data->setStatus (Management::MNG_FAILURE);
    }
    catch (CORBA::SystemException & ex) {
        _EXCPT (ComponentErrors::CORBAProblemExImpl, impl, "CEngineThread::initialize()");
        impl.setName (ex._name ());
        impl.setMinor (ex.minor ());
        _IRA_LOGFILTER_LOG_EXCEPTION (impl, LM_ERROR);
        data->setStatus (Management::MNG_FAILURE);
        antennaBossError = true;
    }
    catch ( ...) {
        _EXCPT (ComponentErrors::UnexpectedExImpl, impl, "CEngineThread::initialize()");
        _IRA_LOGFILTER_LOG_EXCEPTION (impl, LM_ERROR);
        data->setStatus (Management::MNG_FAILURE);
    }
    try {
        CCommonTools::getObservatory (m_observatory, m_service, m_config->getObservatoryComponent(), observatoryError);
    }
    catch (ComponentErrors::CouldntGetComponentExImpl & ex) {
        _IRA_LOGFILTER_LOG_EXCEPTION (ex, LM_ERROR);
        data->setStatus (Management::MNG_FAILURE);
    }
    catch (CORBA::SystemException & ex) {
        _EXCPT (ComponentErrors::CORBAProblemExImpl, impl, "CEngineThread::initialize()");
        impl.setName (ex._name ());
        impl.setMinor (ex.minor ());
        _IRA_LOGFILTER_LOG_EXCEPTION (impl, LM_ERROR);
        data->setStatus (Management::MNG_FAILURE);
        observatoryError = true;
    }
    catch ( ...) {
        _EXCPT (ComponentErrors::UnexpectedExImpl, impl, "CEngineThread::initialize()");
        _IRA_LOGFILTER_LOG_EXCEPTION (impl, LM_ERROR);
        data->setStatus (Management::MNG_FAILURE);
    }
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
}

bool CEngineThread::checkTime (const ACS::Time & currentTime)
{
    CSecAreaResourceWrapper < CDataCollection > m_data = m_dataWrapper->Get ();
    return (currentTime > (m_data->getFirstDumpTime () + getSleepTime () + m_timeSlice));	// gives the cache time to fill a little bit
}

bool CEngineThread::checkTimeSlot (const ACS::Time & slotStart)
{
    /*TIMEVALUE now;
       IRA::CIRATools::getTime(now);
       ACS::TimeInterval slot=m_timeSlice; // gives the slot of time to finalize the job
       ACS::TimeInterval duration=now.value().value-slotStart;
       if (duration<slot) return true;
       else return false; */
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
    ACSErr::Completion_var completion;
    ACS::ROdouble_var ra_var;
    ACS::ROdouble_var dec_var;
    CORBA::Double targetRA, targetDEC;
    CORBA::Double targetAZ, targetEL;
    CORBA::Double targetLON, targetLAT;


    // gets coordinates
    ra_var = m_antennaBoss->targetRightAscension ();
    targetRA = ra_var->get_sync (completion.out ());
    dec_var = m_antennaBoss->targetDeclination ();
    targetDEC = dec_var->get_sync (completion.out ());

    CSecAreaResourceWrapper < CDataCollection > data = m_dataWrapper->Get ();

    // get tsys from devices
    if (!data->getDump (m_time, calOn, bufferCopy, buffer, tracking, buffSize))
        return false;
    tS.value (m_time);

    CalibrationTool_private::getTsysFromBuffer (buffer, data->getInputsNumber (), m_ptsys);

    // we need only the tsys related to the device setted
    m_device = data->getDevice ();
    data->setDataY (m_ptsys[m_device]);
    m_tsysDataSeq[m_dataSeqCounter] = m_ptsys[m_device];

    CSkySource CTskySource (targetRA, targetDEC, IRA::CSkySource::SS_J2000);
    CDateTime CTdateTime (tS);

    switch (data->getScanAxis ()) {
        case Management::MNG_NO_AXIS:
        break;
        case Management::MNG_HOR_LON:
            CTskySource.process (CTdateTime, m_site);
	        CTskySource.getApparentHorizontal (targetAZ, targetEL);
	        m_antennaBoss->getObservedHorizontal (m_time, data->getIntegrationTime () * 10000, az, el);
	        m_coordinate = az;
	        if (az * DR2D > 0.0 && m_lastCoordinate * DR2D > 359.0) { // CCW near 0 position
                offset = m_coordinate;
	        }
	        else if (az * DR2D < 0.0 && m_lastCoordinate * DR2D < 1.0) { // CW near 0 position
                offset = -m_coordinate;
	        }
	    else
	      offset = targetAZ - m_coordinate;
	    m_lastCoordinate = az;
	    m_cosLat = cos (targetEL);
	    m_CoordIndex = 0;	// LON
        m_latPositions[m_dataSeqCounter] = el;
	    break;
        case Management::MNG_HOR_LAT:
            CTskySource.process (CTdateTime, m_site);
            CTskySource.getApparentHorizontal (targetAZ, targetEL);
            m_antennaBoss->getObservedHorizontal (m_time, data->getIntegrationTime () * 10000, az, el);
	        m_coordinate = el;
	        offset = targetEL - m_coordinate;
	        m_CoordIndex = 1;	// LAT
        break;
        case Management::MNG_EQ_LON:
            m_antennaBoss->getObservedEquatorial (m_time, data->getIntegrationTime () * 10000, ra, dec);
            m_coordinate = ra;
            if (ra * DR2D > 0.0 && m_lastCoordinate * DR2D > 359.0) {			// CCW near 0 position
                offset = m_coordinate;
            }
            else if (ra * DR2D < 0.0 && m_lastCoordinate * DR2D < 1.0) {			// CW near 0 position
		        offset = -m_coordinate;
	        }
	        else
	            offset = targetRA - m_coordinate;
	        m_lastCoordinate = ra;
	        m_cosLat = cos (targetDEC);
	        m_CoordIndex = 0;	// LON
            m_latPositions[m_dataSeqCounter] = dec;
	    break;
        case Management::MNG_EQ_LAT:
	        m_antennaBoss->getObservedEquatorial (m_time, data->getIntegrationTime () * 10000, ra, dec);
	        m_coordinate = dec;
	        offset = targetDEC - m_coordinate;
	        m_CoordIndex = 1;	// LAT
	    break;
        case Management::MNG_GAL_LON:
	        CTskySource.process (CTdateTime, m_site);
	        CTskySource.equatorialToGalactic (targetRA, targetDEC, targetLON, targetLAT);
	        m_antennaBoss->getObservedGalactic (m_time, lon, lat);
	        m_coordinate = lon;
	        if (lon * DR2D > 0.0 && m_lastCoordinate * DR2D > 359.0) {			// CCW near 0 position
                offset = m_coordinate;
	        }
	        else if (lon * DR2D < 0.0 && m_lastCoordinate * DR2D < 1.0) {			// CW near 0 position
                offset = -m_coordinate;
	        }
	        else
	            offset = targetLON - m_coordinate;
	        m_lastCoordinate = lon;
	        m_cosLat = cos (targetLAT);
	        m_CoordIndex = 0;	// LON
            m_latPositions[m_dataSeqCounter] = lat;
	    break;
        case Management::MNG_GAL_LAT:
	        CTskySource.process (CTdateTime, m_site);
	        CTskySource.equatorialToGalactic (targetRA, targetDEC, targetLON, targetLAT);
	        m_antennaBoss->getObservedGalactic (m_time, lon, lat);
	        m_coordinate = lat;
	        offset = targetLAT - m_coordinate;
	        m_CoordIndex = 1;	// LAT
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
    }
    data->setDataX (m_coordinate);
    m_dataSeq[m_dataSeqCounter] = m_coordinate;

    // offsets array for fit2 function
    if (offset != 0.0)
        m_off[m_dataSeqCounter] = offset;
    else
        m_off[m_dataSeqCounter] = 0.0;	// could be the center?

    // secs from midnight array for fit2 function
    m_secsFromMidnight[m_dataSeqCounter] = tS.hour () * 3600.0 + tS.minute () * 60.0 + tS.second () + (tS.microSecond () / 1000000.0);

    if (m_CoordIndex == 1) {
        out.Format ("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#lat ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
        m_file << (const char *) out;
    }
    if (m_CoordIndex == 0) {
        out.Format ("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#lon ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
        m_file << (const char *) out;
    }
    m_file << m_dataSeqCounter << " " << m_secsFromMidnight[m_dataSeqCounter] << " " << m_off[m_dataSeqCounter] << " " << m_tsysDataSeq[m_dataSeqCounter] << std::endl;

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
    IRA::CString sourceName;
    IRA::CString projectName;
    IRA::CString observerName;
    ACS::ROstring_var targetRef;
    CORBA::String_var target;
    ACSErr::Completion_var completion;
    ACS::ROdouble_var BWHM_var;
    CORBA::Double BWHM_val;
    ACS::ROdouble_var sourceFlux_var;
    CORBA::Double sourceFlux_val;
    int i;
    int Len;

    // fit2 function parameters
    static integer ftry = 20;
    static real tol = (float) .001;
    static integer par = 5;
    double tmid, tmax, ti;
    int imax;

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
    if (data->isReady ()) {     //main headers are already saved
        if (data->isStart ()) {     //file has to be opened
            if (!m_fileOpened) {
                data->setStatus (Management::MNG_OK);
                // create the file with a combination of
                // calibration tool string and
                // project name string
		        projectName = data->getProjectName ();
		        Len = projectName.GetLength ();
		        if (Len) {
                    fileName.Format ("CalibrationTool-%s_%02d_%02d_%02d.log", (const char *) projectName, now.hour (), now.minute (), now.second ());
                }
                else {
                    fileName.Format ("CalibrationTool-%02d_%02d_%02d.log", now.hour (), now.minute (), now.second ());
                }
                data->setFileName (fileName);

		        m_file.open ((const char *) fileName, ios_base::out | ios_base::app);
                if (!m_file.is_open ()) {
                    _EXCPT (ComponentErrors::FileIOErrorExImpl, impl, "CEngineThread::runLoop()");
                    impl.setFileName ((const char *) fileName);
                    impl.log (LM_ERROR);	// not filtered, because the user need to know about the problem immediately
                    data->setStatus (Management::MNG_WARNING);
                }
		        m_fileOpened = true;
			    // Start writing file
                tS.value (now.value().value);
                out.Format("%04d.%03d.%02d:%02d:%02d.%02d#Calibration Tool Start\n", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (),
                        (long)(tS.microSecond () / 10000.));
                m_file << (const char *) out;
		    }

            /* if at this level this parameter is 1 it means that a LON
             * scan is missed and then all the LAT-LON sequence must be
             * discarded and restarted with a new one LAT-LON sequence
             */
            if (m_checkScan == 1) {
                    ACS_LOG (LM_FULL_INFO, "CalibrationTool::CEngineThread::runLoop()", (LM_NOTICE, "LON SCAN MISSED, RESTART WITH A NEW LAT-LON SEQUENCE"));
                    m_checkScan = 0;
            }

            if (/*(m_lonDone == 0) && (m_latDone == 0) && */(m_checkScan == 0)) {
                /*// Start writing file
                tS.value (now.value().value);
                out.Format("%04d.%03d.%02d:%02d:%02d.%02d#Calibration Tool Start\n", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (),
                        (long)(tS.microSecond () / 10000.));
                m_file << (const char *) out;*/
                // File Name
                tS.value (now.value().value);
                fileName = data->getFileName();
                out.Format("%04d.%03d.%02d:%02d:%02d.%02d#File Name: ", tS.year(),tS.dayOfYear(),tS.hour(),tS.minute(),tS.second(),(long)(tS.microSecond()/10000.));
                m_file << (const char *) out;
                m_file << (const char *) fileName << std::endl;

                // Project Name
                tS.value (now.value().value);
                projectName = data->getProjectName ();
                out.Format("%04d.%03d.%02d:%02d:%02d.%02d#Project Name: ", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (), (long)(tS.microSecond () / 10000.));
                m_file << (const char *) out;
                m_file << (const char *) projectName << std::endl;
                // Observer Name
		        tS.value (now.value().value);
		        observerName = data->getObserverName ();
		        out.Format("%04d.%03d.%02d:%02d:%02d.%02d#Observer Name: ",
			   tS.year (), tS.dayOfYear (), tS.hour (),
			   tS.minute (), tS.second (),
			   (long)(tS.microSecond () / 10000.));
		        m_file << (const char *) out;
		        m_file << (const char *) observerName << std::endl;
		      // Source Name
		      targetRef = m_antennaBoss->target ();
		      target = targetRef->get_sync (completion.out ());
		      ACSErr::CompletionImpl targetCompl (completion);
		      if (!targetCompl.isErrorFree ()) {
                  _ADD_BACKTRACE (ComponentErrors::CouldntGetAttributeExImpl, impl,
					    targetCompl,
					    "CEngineThread::collectAntennaData()");
                  impl.setAttributeName ("target");
                  impl.setComponentName ((const char *) m_config->getAntennaBossComponent());
                  impl.log (LM_ERROR);
                  data->setStatus (Management::MNG_WARNING);
                  sourceName = "";
              }
		      else
                  sourceName = (const char *) target;
		      data->setSourceName (sourceName);
		      tS.value (now.value().value);
		      out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#source ",
			   tS.year (), tS.dayOfYear (), tS.hour (),
			   tS.minute (), tS.second (),
			   (long)(tS.microSecond () / 10000.));
		      m_file << (const char *) out;
		      m_file << (const char *) sourceName;
		      m_file << " 000000.0 +000000 0000.0 0000.000.00:00:00" << std::endl;

		      sourceFlux_var = m_antennaBoss->targetFlux();
		      sourceFlux_val = sourceFlux_var->get_sync(completion.out());
		      data->setSourceFlux (sourceFlux_val);

		      tS.value (now.value().value);
		      switch (data->getScanAxis ())
			{
			case Management::MNG_NO_AXIS:
			    break;
			case Management::MNG_HOR_LON:
			    out.Format
				("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept azel 0 0 0  0 nn  0  0 ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    break;
			case Management::MNG_HOR_LAT:
			    out.Format
				("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept azel 0 0 0  0 nn  0  0 ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    break;
			case Management::MNG_EQ_LON:
			    out.Format
				("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept hadc 0 0 0  0 nn  0  0 ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    break;
			case Management::MNG_EQ_LAT:
			    out.Format
				("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept hadc 0 0 0  0 nn  0  0 ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    break;
			case Management::MNG_GAL_LON:
			    out.Format
				("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept gall 0 0 0  0 nn  0  0 ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    break;
			case Management::MNG_GAL_LAT:
			    out.Format
				("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#fivept gall 0 0 0  0 nn  0  0 ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
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
			}
		      m_file << (const char *) out;
		      m_file << sourceFlux_val << std::endl;
		      ACS_LOG (LM_FULL_INFO, "CalibrationTool::CEngineThread::runLoop()", (LM_NOTICE, "FILE_OPENED %s", (const char *) data->getFileName ()));
		  }
		data->startRunnigStage ();
	    }			// if data->isStart()
	  else if (data->isStop ())
	    {
		//save all the data in the buffer and then finalize the file
		if (m_fileOpened)
		  {
		      while (processData ());
		      // tsys array for fit2 function
		      for (i = 0; i < m_dataSeqCounter; i++)
			  m_ptsys2[i] = (float) m_tsysDataSeq[i];

		      tmid =
			  m_secsFromMidnight[((m_dataSeqCounter + 1) / 2) -
					     1];
		      m_Par[4] =
			  (m_ptsys2[m_dataSeqCounter - 1] -
			   m_ptsys2[0]) /
			  (m_secsFromMidnight[m_dataSeqCounter - 1] -
			   m_secsFromMidnight[0]);
		      m_Par[3] =
			  m_ptsys2[0] + m_Par[4] * (tmid -
						    m_secsFromMidnight[0]);
		      if (m_dataSeqCounter < 5)
			{
			    m_Par[3] = m_Par[4] = (float) 0.;
			}
		      m_errPar[3] = m_errPar[4] = (float) 0.;

		      m_secsFromMidnight[0] -= tmid;
		      tmax =
			  m_ptsys2[0] - (m_Par[3] +
					 m_Par[4] * m_secsFromMidnight[0]);
		      imax = 1;
		      for (i = 2; i <= m_dataSeqCounter; ++i)
			{
			    m_secsFromMidnight[i - 1] -= tmid;
			    ti = m_ptsys2[i - 1] - (m_Par[3] +
						    m_Par[4] *
						    m_secsFromMidnight[i -
								       1]);
			    if (tmax >= ti)
			      {
				  goto gaussianFit;
			      }
			    tmax = ti;
			    imax = i;
			  gaussianFit:
			    ;
			}
		      m_Par[0] = tmax;
		      m_Par[1] = m_off[imax - 1];

		      BWHM_var = m_antennaBoss->FWHM ();
		      BWHM_val = BWHM_var->get_sync (completion.out ());

		      if (m_CoordIndex == 1 /*&& m_latResult == 0*/) {	// LAT scans
			    m_Par[2] = BWHM_val;

			    fit2_ (m_off, m_ptsys2, m_secsFromMidnight, m_Par,
				   m_errPar, &m_dataSeqCounter, &par, &tol,
				   &ftry, (E_fp) fgaus_, &m_reducedCHI,
				   &m_ierr);

			    m_LatPos = (m_dataSeq[0] + m_dataSeq[m_dataSeqCounter - 1]) / 2.;
			    m_LatOff = m_Par[1];
			    m_LatErr = m_errPar[1];

			    // latfit, laterr
			    tS.value (now.value().value);
			    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#latfit ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    m_file << (const char *) out;
			    m_file << m_Par[1] * DR2D << " " << m_Par[2] * DR2D << " " << m_Par[0] << " " << m_Par[3] << " " << m_Par[4] << " " << m_ierr << std::endl;

			    tS.value (now.value().value);
			    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#laterr ",
				tS.year (), tS.dayOfYear (), tS.hour (),
				tS.minute (), tS.second (),
				(long)(tS.microSecond () / 10000.));
			    m_file << (const char *) out;
			    m_file << m_errPar[1] * DR2D << " " << m_errPar[2] * DR2D<< " " << m_errPar[0] << " " << m_errPar[3] << " " << m_errPar[4] << " " << m_reducedCHI << std::endl;

			    data->setAmplitude (m_Par[0]);
			    data->setPeakOffset (m_Par[1]);
			    data->setHPBW (m_Par[2]);
			    data->setOffset (m_Par[3]);
			    data->setSlope (m_Par[4]);

			    if ((fabs(m_Par[1]) < m_off[0]) && (fabs(m_Par[1]) < fabs(m_off[m_dataSeqCounter - 1])) && (m_ierr > 0) ||
				    (fabs(m_Par[1]) > m_off[0]) && (fabs(m_Par[1]) < fabs(m_off[m_dataSeqCounter - 1])) && (m_ierr > 0))
			      {
				  /* if data fitting results are ok, sets new offsets in antenna */
				  
                      m_latResult = 1;

                    m_antennaBoss->getAllOffsets (m_azUserOff, m_elUserOff, m_raUserOff, m_decUserOff, m_lonUserOff, m_latUserOff);

				  switch (data->getScanAxis ())
				    {
				    case Management::MNG_NO_AXIS:
					break;
				    case Management::MNG_HOR_LON:
				    case Management::MNG_HOR_LAT:
					m_antennaBoss->setOffsets (m_azUserOff,
                                                /*m_elUserOff + */m_LatOff,
                                                Antenna::ANT_HORIZONTAL);
					break;
				    case Management::MNG_EQ_LON:
				    case Management::MNG_EQ_LAT:
					m_antennaBoss->setOffsets (m_raUserOff,
                                                /*m_decUserOff + */m_LatOff ,
								                Antenna::ANT_EQUATORIAL);
					break;
				    case Management::MNG_GAL_LON:
				    case Management::MNG_GAL_LAT:
					m_antennaBoss->setOffsets (m_lonUserOff,
                                                /*m_latUserOff + */m_LatOff,
								                Antenna::ANT_GALACTIC);
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
				    }
			      }
			    data->setArrayDataX (m_dataSeq);
			    data->setArrayDataY (m_tsysDataSeq);
			    for (i = 0; i < m_dataSeqCounter; i++)
			      {
				  m_dataSeq[i] = 0.0;
				  m_tsysDataSeq[i] = 0.0;
			      }
			    m_dataSeqCounter = 0;
			    m_latDone = 1;
                m_checkScan = 1;
			}
		      else if ((m_CoordIndex == 0)/* && (m_lonResult == 0)*/)
			{	// LON scans
			    m_Par[2] = BWHM_val / m_cosLat;

			    fit2_ (m_off, m_ptsys2, m_secsFromMidnight, m_Par,
				   m_errPar, &m_dataSeqCounter, &par, &tol,
				   &ftry, (E_fp) fgaus_, &m_reducedCHI,
				   &m_ierr);

			    m_Par[2] *= m_cosLat;
			    m_errPar[2] *= m_cosLat;
			    m_LonPos = (m_dataSeq[0] + m_dataSeq[m_dataSeqCounter -1]) / 2.;
			    m_LonOff = m_Par[1];
			    m_LonErr = m_errPar[1];

                // need to calculate here if the calibration starts with LON scans
                m_LatPos = (m_latPositions[0] + m_latPositions[m_dataSeqCounter -1]) / 2.;

			    // lonfit, lonerr
			    tS.value (now.value().value);
			    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#lonfit ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    m_file << (const char *) out;
			    m_file << m_Par[1] * DR2D << " " << m_Par[2] * DR2D << " " << m_Par[0] << " " <<  m_Par[3] << " " << m_Par[4] << " " << m_ierr << std::endl;
			    tS.value (now.value().value);
			    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#lonerr ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    m_file << (const char *) out;
			    m_file << m_errPar[1] * DR2D << " " << m_errPar[2] * DR2D << " " << m_errPar[0] << " " << m_errPar[3] << " " << m_errPar[4] << " " << m_reducedCHI << std::endl;

			    data->setAmplitude (m_Par[0]);
			    data->setPeakOffset (m_Par[1]);
			    data->setHPBW (m_Par[2]);
			    data->setOffset (m_Par[3]);
			    data->setSlope (m_Par[4]);

			    if ((fabs(m_Par[1]) < m_off[0]) && (fabs(m_Par[1]) < fabs(m_off[m_dataSeqCounter - 1])) && (m_ierr > 0) ||
			    	(fabs(m_Par[1]) > m_off[0]) && (fabs(m_Par[1]) < fabs(m_off[m_dataSeqCounter - 1])) && (m_ierr > 0)	)
			      {
				  /* if data fitting results are ok, sets new offsets in antenna */
				  
                      m_lonResult = 1;

                    m_antennaBoss->getAllOffsets (m_azUserOff, m_elUserOff, m_raUserOff, m_decUserOff, m_lonUserOff, m_latUserOff);

				  switch (data->getScanAxis ())
				    {
				    case Management::MNG_NO_AXIS:
					break;
				    case Management::MNG_HOR_LON:
					m_antennaBoss->setOffsets (/*m_azUserOff + */cos(m_LatPos) * m_LonOff ,
                                                m_elUserOff,
                                                Antenna::ANT_HORIZONTAL);
					break;
				    case Management::MNG_HOR_LAT:
					break;
				    case Management::MNG_EQ_LON:
					m_antennaBoss->setOffsets (/*m_raUserOff + */cos(m_LatPos) * m_LonOff,
                                                m_decUserOff,
                                                Antenna::ANT_EQUATORIAL);
					break;
				    case Management::MNG_EQ_LAT:
					break;
				    case Management::MNG_GAL_LON:
					m_antennaBoss->setOffsets (/*m_lonUserOff + */cos(m_LatPos) * m_LonOff,
                                                m_latUserOff,
                                                Antenna::ANT_GALACTIC);
					break;
				    case Management::MNG_GAL_LAT:
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
				    }
			      }
			    data->setArrayDataX (m_dataSeq);
			    data->setArrayDataY (m_tsysDataSeq);
			    for (i = 0; i < m_dataSeqCounter; i++)
			      {
				  m_dataSeq[i] = 0.0;
				  m_tsysDataSeq[i] = 0.0;
			      }
			    m_dataSeqCounter = 0;
			    m_lonDone = 1;
                m_checkScan = 0;
			}
		      if ((m_latDone == 1) && (m_lonDone == 1) && (m_checkScan == 0))
			{
			    // offset m_LonPos, m_LatPos, m_lonOff, m_latOff, m_lonResult, m_latResult 
			    tS.value (now.value().value);
			    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#offset ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    m_file << (const char *) out;
			    m_file << m_LonPos * DR2D << " " <<  m_LatPos * DR2D << " " << m_LonOff * DR2D << " " << m_LatOff * DR2D << " " << m_lonResult << " " << m_latResult << std::endl;

			    // xoffset m_LonPos, m_LatPos, m_lonOff, m_latOff, m_LonErr, m_LatErr, m_lonResult, m_latResult
			    tS.value (now.value().value);
			    out.Format("%04d.%03d.%02d:%02d:%02d.%02d#fivpt#xoffset ",
				 tS.year (), tS.dayOfYear (), tS.hour (),
				 tS.minute (), tS.second (),
				 (long)(tS.microSecond () / 10000.));
			    m_file << (const char *) out;
			    m_file << m_LonPos * DR2D << " " <<  m_LatPos * DR2D << " " << cos (m_LatPos) * m_LonOff * DR2D << " " << m_LatOff * DR2D << " " << cos (m_LatPos) * m_LonErr * DR2D << " " << m_LatErr * DR2D << " " << m_lonResult << " " << m_latResult << std::endl;
			    
		      	ACS_LOG (LM_FULL_INFO, "CalibrationTool::CEngineThread::runLoop()", (LM_NOTICE, "OFFSETS = %lf %lf %lf %lf %d %d",
					m_LonPos * DR2D, m_LatPos * DR2D, m_LonOff * DR2D, m_LatOff * DR2D, m_lonResult, m_latResult));
		      	ACS_LOG (LM_FULL_INFO, "CalibrationTool::CEngineThread::runLoop()", (LM_NOTICE, "XOFFSETS = %lf %lf %lf %lf %lf %lf %d %d",
					m_LonPos * DR2D, m_LatPos * DR2D, cos (m_LatPos) * m_LonOff * DR2D, m_LatOff * DR2D, cos (m_LatPos) * m_LonErr * DR2D, m_LatErr * DR2D, m_lonResult, m_latResult));
			    ACS_LOG (LM_FULL_INFO, "CalibrationTool::CEngineThread::runLoop()", (LM_NOTICE, "FILE_FINALIZED"));
			    m_latDone = m_lonDone = 0;
			    m_lonResult = m_latResult = 0;
			}
		  }
		data->haltStopStage ();
	    }
	  else if (data->isRunning ())
	    {
		// file was already created, then saves the data into it
		// until there is somthing to process and there is still time available
		if (m_fileOpened)
		  {
		      while (checkTime (now.value ().value)
			     && checkTimeSlot (now.value ().value)
			     && processData ());
		  }
	    }
      }
}
