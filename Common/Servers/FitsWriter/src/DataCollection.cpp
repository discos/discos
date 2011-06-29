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
	m_sectionH=NULL;
	m_fileName="";
	m_project="";
	m_observer="";
	m_status=Management::MNG_OK;
	m_feeds=NULL;
	m_feedNumber=0;
	m_receiverCode="";
	m_siteName="";
	m_sourceName="";
	m_sourceRa=m_sourceDec=m_sourceVlsr=0.0;
	m_pressure=m_temperature=m_humidity=0.0;
	m_scanId=-1;
	//m_antennaBoss=Antenna::AntennaBoss::_nil();
	//m_meteoData=Metrology::MeteoData::_nil();
	//m_receiversBoss=Receivers::ReceiversBoss::_nil();
	//m_scheduler=Management::Scheduler::_nil();
	//m_config=config;
	//m_services=services;
	m_telecopeTacking=m_prevTelescopeTracking=false;
	m_telescopeTrackingTime=0;
	m_localOscillator.length(0);
	m_calibrationMarks.length(0);
	m_recBandWidth.length(0);
	m_recInitialFrequency.length(0);
}
	
CDataCollection::~CDataCollection()
{
	if (m_sectionH!=NULL) delete[] m_sectionH;
	if (m_feeds!=NULL) {
		delete [] m_feeds;
		m_feeds=NULL;
	}
	//unloadAntennaBoss();
	//unloadReceiversBoss();
	//unloadScheduler();
	//unloadMeteo();
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

void CDataCollection::saveFeedHeader(CFitsWriter::TFeedHeader * fH,const WORD& number)
{
	if (m_feeds!=NULL) {
		delete [] m_feeds;
		m_feedNumber=0;
	}
	m_feeds=fH;
	m_feedNumber=number;
}

ACS::Time CDataCollection::getFirstDumpTime()
{
	return m_dumpCollection.getFirstTime();
}

long CDataCollection::SectionPolNumber(const long& ch) const 
{ 
	if (m_sectionH[ch].polarization==Backends::BKND_FULL_STOKES) return 4; 
	else return 1;
}

long CDataCollection::getInputsNumber() const
{
	long sum=0;
	for (long i=0;i<m_mainH.sections;i++) {
		sum+=m_sectionH[i].inputs;
	}
	return sum;
}

void CDataCollection::getInputsConfiguration(ACS::longSeq& feeds,ACS::longSeq& ifs,ACS::doubleSeq& freqs,ACS::doubleSeq& bws,ACS::doubleSeq& atts) const
{
	long inputs=0;
	long inputsNumber=getInputsNumber();
	freqs.length(inputsNumber);
	bws.length(inputsNumber);
	feeds.length(inputsNumber);
	ifs.length(inputsNumber);
	atts.length(inputsNumber);
	for(int i=0;i<m_mainH.sections;i++) {
		if (m_sectionH[i].inputs==1) {
			ifs[inputs]=m_sectionH[i].IF[0];
			atts[inputs]=m_sectionH[i].attenuation[0];
			feeds[inputs]=m_sectionH[i].feed;
			freqs[inputs]=m_sectionH[i].frequency;
			bws[inputs]=m_sectionH[i].bandWidth;
			inputs+=1;
		}
		else { //inputs==2
			ifs[inputs]=m_sectionH[i].IF[0];
			atts[inputs]=m_sectionH[i].attenuation[0];
			feeds[inputs]=m_sectionH[i].feed;
			freqs[inputs]=m_sectionH[i].frequency;
			bws[inputs]=m_sectionH[i].bandWidth;
			inputs+=1;
			ifs[inputs]=m_sectionH[i].IF[1];
			atts[inputs]=m_sectionH[i].attenuation[1];
			feeds[inputs]=m_sectionH[i].feed;
			freqs[inputs]=m_sectionH[i].frequency;
			bws[inputs]=m_sectionH[i].bandWidth;
			inputs+=1;
		}
	}
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
	m_running=true;
	m_start=false;
}

void CDataCollection::haltStopStage()
{
	m_running=false;
	m_stop=false;
}

void CDataCollection::getFileName(IRA::CString& fileName,IRA::CString& fullPath)
{
	TIMEVALUE now;
	IRA::CString dateDir;
	//here we'd like also to add a distributions of files based on date...
	//...so get the present date and append a new directory to the path
	IRA::CIRATools::getTime(now);
	dateDir.Format("%04d%02d%02d",now.year(),now.month(),now.day());
	fullPath=m_fullPath+"/"+dateDir+"/";
	fileName=m_fileName;
}

IRA::CString CDataCollection::getFileName()
{
	IRA::CString fn,fp;
	getFileName(fn,fp);
	return fp+fn;
}

bool CDataCollection::setFileName(const IRA::CString& name)
{
	if (m_start) {
		return false;
	}
	else {
		// computes the real path and name.
	 	char *passedArg=new char [name.GetLength()+1]; // make a copy beacuse the man pages states that the input string may be changed in functions basename() and dirname()
	 	strcpy(passedArg,(const char *)name);
	 	char *tmp=dirname(passedArg);
	 	m_fullPath=IRA::CString(tmp);
	 	strcpy(passedArg,(const char *)name);
	 	tmp=basename(passedArg);
	 	m_fileName=IRA::CString(tmp);
	 	delete [] passedArg;
	 	return true;
	}
}

/*************** Private ***********************************************************/





