// $Id: SenderThread.cpp,v 1.1 2011-03-14 15:16:22 a.orlati Exp $

#include "NoiseGeneratorImpl.h"
#include "CommandLine.h"
#include "SenderThread.h"
#include "Protocol.h"


CSenderThread::CSenderThread(const ACE_CString& name,TSenderParameter  *par, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSenderThread::CSenderThread()");
	m_sender=par->sender;
	m_commandLine=par->command;
	m_configuration=par->configuration;
	for (int i=0;i<MAX_INPUT_NUMBER;i++) {
		m_KCountsRatio[i]=0.0;
	}
	m_stop=m_go=m_sending=false;
}

CSenderThread::~CSenderThread()
{
	AUTO_TRACE("CSenderThread::~CSenderThread()");
}

void CSenderThread::onStart()
{
	AUTO_TRACE("CSenderThread::onStart()");
}

void CSenderThread::onStop()
{
	 AUTO_TRACE("CSenderThread::onStop()");
}

void CSenderThread::runLoop()
{
	TIMEVALUE now;
	if (m_stop) { // we were asked to stop the data transfer.....
		m_go=false;
		m_stop=false;
		try {
			if (m_sending) {
				m_sender->getSender()->stopSend(FLOW_NUMBER);
			}
			m_sending=false;
		}
		catch (AVStopSendErrorExImpl& ex) {
			_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"CSenderThread::runLoop()");
			impl.setDetails("stop message could not be sent");
			impl.log(LM_ERROR);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSenderThread::runLoop()");
			impl.log(LM_ERROR);
		}
	}
	if (m_go) {
		IRA::CIRATools::getTime(now);
		ACS::TimeInterval integration=m_dataHeader.integration*10000; // integration in 100 ns units
		if (m_startTime.value().value<=(now.value().value+integration)) {
			double *tsys;
			SAMPLETYPE *data;
			long tsysSize;
			long dataSize;
			Backends::TDumpHeader dh;
			m_startTime.value(now.value().value); // copy the time of the acquisition
			generateBuffer(tsys,data,tsysSize,dataSize);
			
			dh.calOn=false;
			dh.time=now.value().value;
			dh.dumpSize=tsysSize+dataSize;
			ACE_Message_Block buffer(dh.dumpSize+sizeof(Backends::TDumpHeader));
			
			buffer.copy((const char *)&dh,sizeof(Backends::TDumpHeader));
			buffer.copy((const char *)tsys,tsysSize);
			buffer.copy((const char *)data,dataSize);
			try {
				m_sender->getSender()->sendData(FLOW_NUMBER,&buffer);
				m_sending=true;
			}
			catch (AVSendFrameErrorExImpl& ex) {
				_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"CSenderThread::computeSample()");
				impl.setDetails("raw data could not be sent");
				impl.log();
			}
			catch (...) {
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSenderThread::computeSample()");
				impl.log();
			}
			delete []tsys;
			delete []data;
			buffer.reset();		
		}
	}
}

void CSenderThread::saveDataHeader(Backends::TMainHeader* mH,Backends::TSectionHeader* cH)
{
	m_dataHeader.sections=mH->sections;
	m_dataHeader.integration=mH->integration;
	m_dataHeader.beams=mH->beams;
	m_dataHeader.sampleSize=mH->sampleSize;	
	for (int j=0;j<m_dataHeader.sections;j++) {
		m_sectionsHeader[j]=cH[j];
	}       
}

void CSenderThread::generateBuffer(double *& tsys,SAMPLETYPE *& data,long& tsysSize,long& dataSize)
{
	long inputs=0;
	long pols,bins;
	long counter=0;
	long tsysCounter=0;
	dataSize=0;
	for (long i=0;i<m_dataHeader.sections;i++) {
		inputs+=m_sectionsHeader[i].inputs;
		if (m_sectionsHeader[i].polarization==Backends::BKND_FULL_STOKES) pols=4;
		else pols=1;
		dataSize+=m_sectionsHeader[i].bins*pols;
	}
	tsys=new double[inputs];
	data=new SAMPLETYPE[dataSize];
	dataSize*=m_dataHeader.sampleSize; //buffer dimension in bytes
	tsysSize=inputs*sizeof(double);
	for (long i=0;i<m_dataHeader.sections;i++) {
		for (long j=0;j<m_sectionsHeader[i].inputs;j++) {
			double att;
			if (m_sectionsHeader[i].polarization==Backends::BKND_LCP) att=m_sectionsHeader[i].attenuation[0];
			else if (m_sectionsHeader[i].polarization==Backends::BKND_RCP) att=m_sectionsHeader[i].attenuation[1];
			else att=(m_sectionsHeader[i].attenuation[0]+m_sectionsHeader[i].attenuation[1])/2.0;
			tsys[tsysCounter]=CProtocol::getRandomValue(att);
			tsys[tsysCounter]*=m_KCountsRatio[m_sectionsHeader[i].id];
			tsysCounter++;
		}
		if (m_sectionsHeader[i].polarization==Backends::BKND_FULL_STOKES) pols=4;
		else pols=1;
		bins=m_sectionsHeader[i].bins;
		for (long p=0;p<pols;p++) {
			for (long b=0;b<bins;b++) {
				data[counter]=CProtocol::getRandomValue();
				if (bins==1) printf("%lf \n",data[counter]);
				counter++;
			}
		}
	}
}

void CSenderThread::saveStartTime(const ACS::Time& time)
{
	if (time!=0) {
		m_startTime.value(time);
	} 
	else {
		IRA::CIRATools::getTime(m_startTime);
	}
}

void CSenderThread::setKelvinCountsRatio(const ACS::doubleSeq& ratio)
{
	for (unsigned i=0;i<ratio.length();i++) {
		m_KCountsRatio[i]=ratio[i];
	}
}
