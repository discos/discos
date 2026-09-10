#include <Cplusplus11Helper.h>
#define _CPLUSPLUS11_PORTING_

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
#include "EngineThread.h"
C11_IGNORE_WARNING_POP
#include <LogFilter.h>
#include <Definitions.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <DateTime.h>
#include <SkySource.h>
// #include "CommonTools.h"
#include <DiscosVersion.h>

using namespace IRA;
using namespace DataWriter_private;

_IRA_LOGFILTER_IMPORT;

CEngineThread::CEngineThread(const ACE_CString &name, DataWriter_private::CDataCollection *param,
							 const ACS::TimeInterval &responseTime, const ACS::TimeInterval &sleepTime) : ACS::Thread(name, responseTime, sleepTime), m_data(param)
{
	AUTO_TRACE("CEngineThread::CEngineThread()");
	m_fileOpened = false;
	// m_summaryOpened=false;
	//m_file = NULL;
	// m_summary=NULL;
	m_ptsys = new double[256];
	// m_receiversBoss=Receivers::ReceiversBoss::_nil();
	// receiverBossError=false;
	// m_antennaBoss=Antenna::AntennaBoss::_nil();
	// antennaBossError=false;
	// m_lastMinorServoEnquireTime=0;
}

CEngineThread::~CEngineThread()
{
	AUTO_TRACE("CEngineThread::~CEngineThread()");
	if (m_fileOpened)
	{
		m_file.close();
	}
	if (m_ptsys)
	{
		delete[] m_ptsys;
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

bool CEngineThread::checkTime(const ACS::Time &jobStartTime)
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	return (now.value().value < (jobStartTime + m_timeSlice));
	// return (currentTime>(m_data->getFirstDumpTime()+getSleepTime()+m_timeSlice)); // gives the cache time to fill a little bit
}

bool CEngineThread::checkTimeSlot(const ACS::Time &slotStart)
{
	return true;
}

bool CEngineThread::processData()
{
	char *buffer;	  // pointer to the buffer that contains the real data
	char *bufferCopy; // pointer to the memory that has to be freed
	ACS::Time time;
	bool calOn;
	// bool applyServoPositions;
	long pol,bins;
	long buffSize;
	// double ra,dec;
	// double az,el;
	bool tracking;
	// double hum,temp,press;
	// double derot;
	// ACS::doubleSeq_var servoPositions, servoUserOffsets, servoSystemOffsets;
	long long integrationTime;
	// CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();

	if (!m_data->getDump(time, calOn, bufferCopy, buffer, tracking, buffSize))
		return false;

	// integration is multiplied by 10000 because internally we have the value in millesec while the method requires 100ns.
	integrationTime = m_data->getIntegrationTime() * 10000;
	// The timestamp must be referred to the mid time of the sample. Since time is the beginning
	//(see idl documentation of generic backed), I need to add half integration time.
	time += (ACS::Time)(integrationTime * 0.5);
	TIMEVALUE tS;
	tS.value(time);
	IRA::CString out;
	out.Format("%02d:%02d:%02d.%03d  %d ", tS.hour(), tS.minute(), tS.second(), tS.microSecond() / 1000, calOn);
	m_file << out;

	/*out.Format("%f %f %f %f ",ra,dec,az,el);
	m_file << (const char *) out;
	if (tracking) {
		out.Format("yes ");
	}
	else {
		out.Format("no ");
	}
	m_file << (const char *) out;
	m_file << endl;*/

	for (int i = 0; i < m_data->getSectionsNumber(); i++)
	{
		bins = m_data->getSectionBins(i);
		pol = m_data->getSectionStreamsNumber(i);
		switch (m_data->getSampleSize())
		{
		case sizeof(BYTE2_TYPE):
		{
			BYTE2_TYPE channel[bins * pol];
			DataWriter_private::getChannelFromBuffer<BYTE2_TYPE>(i, pol, bins, buffer, channel);
			out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ", i, pol, bins, m_data->getSampleSize());
			m_file << (const char *)out;
			for (long j = 0; j < pol * bins; j++)
			{
				out.Format("%d ", channel[j]);
				m_file << (const char *)out;
			}
			m_file << '\n';
			break;
		}
		case sizeof(BYTE4_TYPE):
		{
			BYTE4_TYPE channel[bins * pol];
			DataWriter_private::getChannelFromBuffer<BYTE4_TYPE>(i, pol, bins, buffer, channel);
			out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ", i, pol, bins, m_data->getSampleSize());
			m_file << (const char *)out;
			for (long j = 0; j < bins * pol; j++)
			{
				out.Format("%10.5f, %d ", channel[j], channel[j]);
				m_file << (const char *)out;
			}
			m_file << '\n';
			break;
		}
		default:
		{ // it should be BYTE8_TYPE
			BYTE8_TYPE channel[bins * pol];
			DataWriter_private::getChannelFromBuffer<BYTE8_TYPE>(i, pol, bins, buffer, channel);
			out.Format("sect %d, pols: %d , bins: %d, sampleSize: %d - ", i, pol, bins, m_data->getSampleSize());
			m_file << (const char *)out;
			for (long j = 0; j < pol * bins; j++)
			{
				out.Format("%10.5lf ", channel[j]);
				m_file << (const char *)out;
			}
			m_file << '\n';
		}
		}
	} // end for
	//m_file->add_row();
	delete[] bufferCopy;
	return true;
}

void CEngineThread::runLoop()
{
	TIMEVALUE nowEpoch;
	IRA::CString filePath, fileName;
	// CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	IRA::CIRATools::getTime(nowEpoch); // it marks the start of the activity job
	// cout << "inizio : " << nowEpoch.value().value << endl;
	if (m_data->isReset())
	{
		if (m_fileOpened)
		{
			m_file.close();
			ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_NOTICE, "FILE_CLOSED"));
			m_fileOpened = false;
		}
		m_data->haltResetStage();
		return;
	}
	if (m_data->isStart() && m_data->isReady() && m_data->isScanHeaderReady() && m_data->isSubScanHeaderReady())
	{ // //main headers are already saved and file has to be opened
		if (!m_fileOpened)
		{
			// m_data->setStatus(Management::MNG_OK);
			//  create the file and save main headers
			m_data->getFileName(fileName, filePath);
			if (!IRA::CIRATools::directoryExists(filePath))
			{
				IRA::CIRATools::makeDirectory(filePath);
				if (!IRA::CIRATools::directoryExists(filePath))
				{
					_EXCPT(ComponentErrors::FileIOErrorExImpl, impl, "CEngineThread::runLoop()");
					impl.setFileName((const char *)filePath);
					impl.log(LM_ERROR);
					m_data->setStatus(Management::MNG_FAILURE);
				}
				else
				{
					ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_NOTICE, "NEW_SCAN_FOLDER_CREATED: %s", (const char *)filePath));
				}
			}

			m_file.open((const char *)m_data->getFileName(),std::ofstream::out | std::ofstream::app);
			if (!m_file) {
				_EXCPT(ManagementErrors::FitsCreationErrorExImpl, impl, "CEngineThread::runLoop()");
				impl.setFileName((const char *)m_data->getFileName());
				impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
				m_data->setStatus(Management::MNG_FAILURE);
			}
			else
			{
				m_fileOpened = true;
				m_data->startRunnigStage();

				// now creates the file, the tables and the headers
				//Backends::TMainHeader mH = m_data->getMainHeader();
				//Backends::TSectionHeader const *cH = m_data->getSectionHeader();
				/*IRA::CString siteName;
				IRA::CString sourceName;
				double sourceRa, sourceDec, sourceVlsr;
				double azOff, elOff, raOff, decOff, lonOff, latOff;
				double lonUserOff, latUserOff;
				Antenna::TCoordinateFrame frameUserOff;
				double dut1;
				long scanTag;
				long scanID, subScanID;
				IRA::CString scheduleName;
				IRA::CSite site;
				ACS::doubleSeq LocalOscillator;
				ACS::doubleSeq calib;
				ACS::longSeq polarizations;
				ACS::doubleSeq skyFreq, skyBw;
				ACS::doubleSeq fluxes;
				ACS::longSeq feedsID;
				ACS::longSeq ifsID;
				ACS::doubleSeq atts, restFreqs;
				ACS::longSeq sectionsID;
				ACS::stringSeq axisName, axisUnit;*/

				// m_data->getSite(site,dut1,siteName);

				/** The multiplicity of these arrays is the total number of inputs */
				/*m_info.getLocalOscillator(LocalOscillator);
				m_info.getSectionsID(sectionsID);
				m_info.getBackendAttenuations(atts);
				m_info.getFeedsID(feedsID);
				m_info.getIFsID(ifsID);
				// useful bandwidth, resulting from the matching of backend and frontend settings
				m_info.getSkyBandwidth(skyBw);
				m_info.getSkyFrequency(skyFreq);
				m_info.getCalibrationMarks(calib);
				m_info.getSourceFlux(fluxes);
				m_info.getReceiverPolarization(polarizations);
				m_info.getSource(sourceName, sourceRa, sourceDec, sourceVlsr);
				m_info.getAntennaOffsets(azOff, elOff, raOff, decOff, lonOff, latOff);
				m_info.getAntennaUserOffsets(lonUserOff, latUserOff, frameUserOff);*/
				//scanTag = m_data->getScanTag();
				//scanID = m_data->getScanID();
				//subScanID = m_data->getSubScanID();
				/*scheduleName = m_data->getScheduleName();*/

				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_DEBUG, "RUNNING_FROM_NOW"));
				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_NOTICE, "FILE_OPENED %s", (const char *)m_data->getFileName()));
			}
		} // end !m_fileOpened
	}
	else if (m_data->isStop())
	{
		ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_DEBUG, "STOPPING"));
		// save all the data in the buffer an then finalize the file
		if (m_fileOpened)
		{
			// cout << "Stopping, cached  dumps: " << m_data->getDumpCollectionSize() << endl;
			while (processData())
			{
				m_info.incTotalDumps();
			};
			m_file.close();
		}
		m_fileOpened = false;
		ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_NOTICE, "FILE_FINALIZED"));

		m_data->haltStopStage();
	}
	else if (m_data->isRunning())
	{	// file was already created.... then saves the data into it
		// until there is something to process and
		// there is still time available.......
		if (m_fileOpened)
		{
			// cout << "cached before dumps: " << m_data->getDumpCollectionSize() << endl;
			while (checkTime(nowEpoch.value().value) && checkTimeSlot(nowEpoch.value().value) && processData())
			{
				m_info.incTotalDumps();
			}
			// cout << "cached after  dumps: " << m_data->getDumpCollectionSize() << endl;
			// IRA::CIRATools::getTime(nowEpoch);
			// cout << "fine :" << nowEpoch.value().value << endl;
		}
	}
}
//}
