// $Id: DataCollection.cpp,v 1.2 2011-01-18 10:50:21 c.migoni Exp $

#include "DataCollection.h"
#include <SecureArea.h>

void CalibrationTool_private::getTsysFromBuffer(char *& buffer,const DWORD& channels ,double *tsys) {
	for (DWORD i=0;i<channels;i++) {
		tsys[i]=*((double *)buffer);
		buffer+=sizeof(double);
	}
}

using namespace CalibrationTool_private;

CDataCollection::CDataCollection()
{
	m_running=m_ready=m_start=m_stop=false;
    m_scanHeader=m_subScanHeader=false;
	m_sectionH=NULL;
	m_fileName="";
	m_projectName="";
	m_observerName="";
	m_status=Management::MNG_OK;
	m_sourceName="";
	m_telecopeTacking=m_prevTelescopeTracking=false;
	m_telescopeTrackingTime=0;
}
	
CDataCollection::~CDataCollection()
{
	if (m_sectionH!=NULL)
        delete[] m_sectionH;
}

void CDataCollection::saveMainHeaders(Backends::TMainHeader const * h,
		Backends::TSectionHeader const * ch)
{
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
	return m_dumpCollection.getFirstTime();
}

long CDataCollection::getInputsNumber() const
{
	long sum=0;
	for (long i=0;i<m_mainH.sections;i++) {
		sum+=m_sectionH[i].inputs;
	}
	return sum;
}

bool CDataCollection::getDump(ACS::Time& time,bool& calOn,char *& memory,char *& buffer,bool& tracking,long& buffSize)
{
	return m_dumpCollection.popDump(time,calOn,memory,buffer,tracking,buffSize);
}

void CDataCollection::startStopStage()
{
	if (m_running) m_stop=true;
}

void CDataCollection::startRunnigStage()
{
    m_subScanHeader=false;
	m_running=true;
	m_start=false;
}

void CDataCollection::haltStopStage()
{
	m_running=false;
	m_stop=false;
}

void CDataCollection::haltResetStage()
{
	m_reset=false;
}

bool CDataCollection::setFileName(const IRA::CString& name)
{
	if (m_start) {
		return false;
	}
	else {
		m_fileName=name;
		return true;
	}
}

bool CDataCollection::setScanSetup(const Management::TScanSetup& setup,bool& recording,bool& inconsistent)
{
	if (m_start && m_running) {
		recording=true;
		inconsistent =false;
		return false;
	}
	else {
        if (!m_scanHeader) {
            m_fileName=setup.baseName;
            m_projectName=setup.projectName;
            m_observerName=setup.observerName;
            m_deviceID=setup.device;
            m_scanHeader=true;
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
	if (m_start && m_running) {
		recording=true;
		inconsistent =false;
		return false;
	}
	else {
        if (m_scanHeader && !m_subScanHeader) {
            m_scanAxis=setup.axis;
            m_subScanHeader=true;
            return true;
        }
        else {
			recording=false;
			inconsistent =true;
			return false;
		}
	}
}

bool CDataCollection::scanStop()
{
	m_ready=false;
    m_scanHeader=false;
	return true;
}

void CDataCollection::forceReset()
{
	m_running=m_ready=m_start=m_stop=false;
    m_scanHeader=m_subScanHeader=false;
	m_reset=true;
	m_status=Management::MNG_OK;
}

/*************** Private ***********************************************************/





