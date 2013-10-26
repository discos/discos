// $Id: DataCollection.cpp,v 1.9 2011-04-22 18:51:48 a.orlati Exp $

#include "DataCollection.h"
#include <SecureArea.h>
#include <libgen.h>

void FitsWriter_private::getTsysFromBuffer(char *& buffer,const DWORD& channels ,double *tsys) {
	for (DWORD i=0;i<channels;i++) {
		tsys[i]=*((double *)buffer);
		buffer+=sizeof(double);
	}
}

using namespace FitsWriter_private;

CDataCollection::CDataCollection()
{
	m_running=m_ready=m_start=m_stop=false;
	m_scanHeader=m_subScanHeader=false;
	m_reset=false;
	m_sectionH=NULL;
	m_fileName=m_fullPath="";
	m_project="";
	m_observer="";
	m_scheduleName="";
	m_status=Management::MNG_OK;
	m_siteName="";
	m_pressure=m_temperature=m_humidity=0.0;
	m_scanTag=-1;
	m_deviceID=0;
	m_scanAxis=Management::MNG_NO_AXIS;
	m_scanID=m_subScanID=0;
	m_telecopeTacking=m_prevTelescopeTracking=false;
	m_telescopeTrackingTime=0;
}
	
CDataCollection::~CDataCollection()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_sectionH!=NULL) delete[] m_sectionH;
}

void CDataCollection::forceReset()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_running=m_ready=m_start=m_stop=false;
	m_scanHeader=m_subScanHeader=false;
	m_reset=true;
	m_status=Management::MNG_OK;
}

void CDataCollection::saveMainHeaders(Backends::TMainHeader const * h,
		Backends::TSectionHeader const * ch)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	memcpy(&m_mainH,h,sizeof(Backends::TMainHeader));
	if (m_sectionH!=NULL) delete[] m_sectionH;
	m_sectionH=new Backends::TSectionHeader[m_mainH.sections];
	memcpy(m_sectionH,ch,sizeof(Backends::TSectionHeader)*m_mainH.sections);
	m_ready=true; // main headers are saved....before that no activity can take place	
}

bool CDataCollection::saveDump(char * memory)
{
	bool track;
	char *buffer;
	baci::ThreadSyncGuard guard(&m_mutex);
	Backends::TDumpHeader *dh=(Backends::TDumpHeader *) memory;
	buffer=memory+sizeof(Backends::TDumpHeader); 
	if (dh->time>=m_telescopeTrackingTime) track=m_telecopeTacking;
	else track=m_prevTelescopeTracking;
	if ((!m_running) && (!m_start) && (!m_stop)) {
		m_start=true;
	}
	return m_dumpCollection.pushDump(dh->time,dh->calOn,memory,buffer,track,dh->dumpSize);
}

ACS::Time CDataCollection::getFirstDumpTime()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	return m_dumpCollection.getFirstTime();
}

long CDataCollection::getInputsNumber()
{
	long sum=0;
	baci::ThreadSyncGuard guard(&m_mutex);
	for (long i=0;i<m_mainH.sections;i++) {
		sum+=m_sectionH[i].inputs;
	}
	return sum;
}

IRA::CString CDataCollection::getSubScanType() const
{
	if (m_scanAxis==Management::MNG_HOR_LON) {
		return "AZ";
	}
	else if (m_scanAxis==Management::MNG_HOR_LAT) {
		return "EL";
	}
	else if (m_scanAxis==Management::MNG_EQ_LON) {
		return "RA";
	}
	else if (m_scanAxis==Management::MNG_EQ_LAT) {
		return "DEC";
	}
	else if (m_scanAxis==Management::MNG_GAL_LON) {
		return "GLON";
	}
	else if (m_scanAxis==Management::MNG_GAL_LAT) {
		return "GLAT";
	}
	else if (m_scanAxis==Management::MNG_BEAMPARK) {
		return "BEAMPARK";
	}
	else if (m_scanAxis==Management::MNG_TRACK) {
		return "TRACKING";
	}
	else if (m_scanAxis==Management::MNG_GCIRCLE) {
		return "GCIRCLE";
	}
	else if (m_scanAxis==Management::MNG_PFP_Y) {
		return "PFP_Y";
	}
	else if (m_scanAxis==Management::MNG_PFP_Z) {
		return "FOCUSING";
	}
	else if (m_scanAxis==Management::MNG_SUBR_Z) {
		return "FOCUSING";
	}
	else if (m_scanAxis==Management::MNG_SUBR_X) {
		return "SUBR_X";
	}
	else if (m_scanAxis==Management::MNG_SUBR_Y) {
		return "SUB_Y";
	}
	else if (m_scanAxis==Management::MNG_NO_AXIS) {
		return "UNKNOWN";
	}
	else {
		return "UNKNOWN";
	}
}

void CDataCollection::getInputsConfiguration(ACS::longSeq& sectionID,ACS::longSeq& feeds,ACS::longSeq& ifs,ACS::doubleSeq& freqs,ACS::doubleSeq& bws,ACS::doubleSeq& atts)
{
	long inputs=0;
	baci::ThreadSyncGuard guard(&m_mutex);
	long inputsNumber=getInputsNumber();
	freqs.length(inputsNumber);
	bws.length(inputsNumber);
	feeds.length(inputsNumber);
	ifs.length(inputsNumber);
	atts.length(inputsNumber);
	sectionID.length(inputsNumber);
	for(int i=0;i<m_mainH.sections;i++) {
		if (m_sectionH[i].inputs==1) {
			ifs[inputs]=m_sectionH[i].IF[0];
			atts[inputs]=m_sectionH[i].attenuation[0];
			feeds[inputs]=m_sectionH[i].feed;
			freqs[inputs]=m_sectionH[i].frequency;
			bws[inputs]=m_sectionH[i].bandWidth;
			sectionID[inputs]=m_sectionH[i].id;
			inputs+=1;
		}
		else { //inputs==2
			ifs[inputs]=m_sectionH[i].IF[0];
			atts[inputs]=m_sectionH[i].attenuation[0];
			feeds[inputs]=m_sectionH[i].feed;
			freqs[inputs]=m_sectionH[i].frequency;
			bws[inputs]=m_sectionH[i].bandWidth;
			sectionID[inputs]=m_sectionH[i].id;
			inputs+=1;
			ifs[inputs]=m_sectionH[i].IF[1];
			atts[inputs]=m_sectionH[i].attenuation[1];
			feeds[inputs]=m_sectionH[i].feed;
			freqs[inputs]=m_sectionH[i].frequency;
			bws[inputs]=m_sectionH[i].bandWidth;
			sectionID[inputs]=m_sectionH[i].id;
			inputs+=1;
		}
	}
}

bool CDataCollection::getDump(ACS::Time& time,bool& calOn,char *& memory,char *& buffer,bool& tracking,long& buffSize)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	return m_dumpCollection.popDump(time,calOn,memory,buffer,tracking,buffSize);
}

void CDataCollection::startStopStage()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_running) m_stop=true;
}

void CDataCollection::startRunnigStage()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_subScanHeader=false;
	m_running=true;
	m_start=false;
}

void CDataCollection::haltStopStage()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_running=false;
	m_stop=false;
}

void CDataCollection::haltResetStage()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_reset=false;
}

void CDataCollection::getFileName(IRA::CString& fileName,IRA::CString& fullPath) const
{
	fileName=m_fileName;
	fullPath=m_fullPath;
}

IRA::CString CDataCollection::getFileName() const 
{
	return m_fullPath+m_fileName;
}

bool CDataCollection::setScanSetup(const Management::TScanSetup& setup,bool& recording,bool& inconsistent)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_start && m_running) {
		recording=true;
		inconsistent =false;
		return false;
	}
	else {
		if (!m_scanHeader) {
			IRA::CString basePath;
			IRA::CString extraPath;
			IRA::CString baseName;
			m_project=setup.projectName;
			m_scanTag=setup.scanTag;
			m_deviceID=setup.device;
			m_scanID=setup.scanId;
			m_observer=setup.observerName;
			m_scheduleName=setup.schedule;
			basePath=setup.path;
			extraPath=setup.extraPath;
			baseName=setup.baseName;
			m_scanHeader=true;
			if (extraPath!="") {
				m_fullPath=basePath+extraPath+baseName+"/";
			}
			else {
				m_fullPath=basePath+baseName+"/";
			}
			//setup.scanLayout is not used
			return true;
		}
		else {
			recording=false;
			inconsistent =true;
			return false;
		}
	}
}

bool CDataCollection::setSubScanSetup(const Management::TSubScanSetup& setup,bool& recording,bool& inconsistent)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_start && m_running) {
		recording=true;
		inconsistent =false;
		return false;
	}
	else {
		if (m_scanHeader && !m_subScanHeader) {
			IRA::CString temp;
			IRA::CString baseName;
			m_subScanID=setup.subScanId;
			baseName=setup.baseName;
			temp.Format("_%03d_%03d",m_scanID,m_subScanID);
			m_fileName=baseName+temp+".fits";
			m_subScanHeader=true;
			m_scanAxis=setup.axis;
			//setup.startUt and setup.targetID are not used
			return true;
		}
		else {
			recording=false;
			inconsistent =true;
			return false;
		}
	}
}

bool CDataCollection::stopScan()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_ready=false;
	m_scanHeader=false;
	return true;
}

/*************** Private ***********************************************************/





