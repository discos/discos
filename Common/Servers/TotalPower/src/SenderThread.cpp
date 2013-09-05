// $Id: SenderThread.cpp,v 1.2 2011-05-12 14:14:31 a.orlati Exp $

#include "TotalPowerImpl.h"
#include "CommandLine.h"
#include "SenderThread.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;

CSenderThread::CSenderThread(const ACE_CString& name,TSenderParameter  *par, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSenderThread::CSenderThread()");
	m_sender=par->sender;
	m_commandLine=par->command;
	m_configuration=par->configuration;
	m_dataLine=par->dataLine;
	m_isConnected=false;
	m_inputBuffer=new BYTE[DATABUFFERSIZE];
	m_tempBuffer=new BYTE[DATABUFFERSIZE*2];
	m_tempBufferPointer=0;
	m_zeroBuffer=new DWORD[MAX_SECTION_NUMBER];
	m_KCountsRatio.length(MAX_SECTION_NUMBER);
	for (int i=0;i<MAX_SECTION_NUMBER;i++) {
		m_zeroBuffer[i]=0;
		m_KCountsRatio[i]=1.0;
	}
	clearIntegration(m_tpi);
	clearIntegration(m_cal);
	m_lastReceiveEpoch.reset();
	m_stop=m_go=m_sending=false;
	//m_immediateStart=false;
}

CSenderThread::~CSenderThread()
{
	AUTO_TRACE("CSenderThread::~CSenderThread()");
	if (m_inputBuffer) delete[] m_inputBuffer;
	if (m_tempBuffer) delete[] m_tempBuffer;
	if (m_zeroBuffer) delete[] m_zeroBuffer;
}

void CSenderThread::onStart()
{
	AUTO_TRACE("CSenderThread::onStart()");
}

void CSenderThread::onStop()
{
	 AUTO_TRACE("CSenderThread::onStop()");
}

void CSenderThread::closeSocket()
{
	IRA::CError err;
	m_isConnected=false;
	m_stop=true;
	m_backendSock.Close(err);
}
 
void CSenderThread::runLoop()
{
	IRA::CError err;
	int res;
	IRA::CSecAreaResourceWrapper<CCommandLine> cmd=m_commandLine->Get();
	if (cmd->m_reiniting) {
		cmd->m_reiniting=false;
		closeSocket();
		cmd->clearStatusField(CCommandLine::DATALINERROR);
		cmd->clearStatusField(CCommandLine::SAMPLING);
		return;
	}
	cmd.Release();
	//the thread starts to listen to the socket for incoming connections to
	if (!m_isConnected) {
		res=m_dataLine->Accept(err,m_backendSock);
		if (res==IRA::CSocket::WOULDBLOCK) {
			// do nothing...exit the run loop until the next iteration
		}
		else if (res==IRA::CSocket::FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			IRA::CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
			line->setStatusField(CCommandLine::DATALINERROR);
			_IRA_LOGFILTER_LOG_EXCEPTION(dummy,LM_WARNING); //log as warn because this is the thread and no client is aware of that
		}
		else {
			m_isConnected=true;  // the conneciton has been enstablished
			IRA::CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
			line->clearStatusField(CCommandLine::DATALINERROR);
			ACS_LOG(LM_FULL_INFO,"CCSenderThread::runLoop()",(LM_INFO,"BACKEND_CONNECTED_TO_DATA_LINE"));
			initTransfer();
		}
	}
	else { //already connected...then try a new data reception
		res=m_backendSock.Receive(err,m_inputBuffer,DATABUFFERSIZE);
		if (res==0) { // backend disconnected...propably as a consequence of a terminate data transmission command.
			// gets the access to the 
			IRA::CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
			line->clearStatusField(CCommandLine::DATALINERROR);
			line->clearStatusField(CCommandLine::SAMPLING);
			closeSocket();
			ACS_LOG(LM_FULL_INFO,"CCSenderThread::runLoop()",(LM_INFO,"DISCONNECTED"));
		}
		else if (res==IRA::CSocket::FAIL) { //error during receive !!!!
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			IRA::CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
			line->setStatusField(CCommandLine::DATALINERROR);
			_IRA_LOGFILTER_LOG_EXCEPTION(dummy,LM_WARNING);
		}
		else if (res==IRA::CSocket::WOULDBLOCK) { //nothing to read
			TIMEVALUE now;
			IRA::CIRATools::getTime(now);
			if (IRA::CIRATools::timeDifference(now,m_lastReceiveEpoch)>2000000) { // if the last data reception is more than two seconds old
				IRA::CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
				line->clearStatusField(CCommandLine::SAMPLING);
			} 
		}
		else { // receive was ok and res is the number of bytes that are read
			long *boards;
			long sectionNumber;
			IRA::CIRATools::getTime(m_lastReceiveEpoch);
			IRA::CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
			boards=line->boardsMapping();
			sectionNumber=line->sectionNumber();
			line->setStatusField(CCommandLine::SAMPLING);
			line->clearStatusField(CCommandLine::DATALINERROR);
			line->getKCRatio(m_KCountsRatio); // get the K/C ratio
			processData((DWORD)res,sectionNumber,boards);
		}
	}
	if (m_stop) { // we were asked to stop the data transfer.....
		m_go=false;
		m_stop=false;
		try {
			#ifndef BKD_DEBUG
			if (m_sending) {
				m_sender->getSender()->stopSend(FLOW_NUMBER);
			}
			#endif
			m_sending=false;
		}
		catch (AVStopSendErrorExImpl& ex) {
			_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"CSenderThread::runLoop()");
			impl.setDetails("stop message could not be sent");
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSenderThread::runLoop()");
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
	}
	
}

void CSenderThread::saveDataHeader(Backends::TMainHeader* mH,Backends::TSectionHeader* cH)
{
	m_dataHeader.channels=mH->sections;
	m_dataHeader.integration=mH->integration;
	for (int j=0;j<m_dataHeader.channels;j++) {
		m_dataHeader.id[j]=cH[j].id;
	}
	m_dataHeader.sampleRate=cH[0].sampleRate; // should be the same for all the channels.....       
	m_dataHeader.sampleSize=mH->sampleSize;
	//m_dataHeader.normalization=round(m_dataHeader.sampleRate*double(m_dataHeader.integration*1000)); // integration from milli to microseconds
	m_dataHeader.normalization=(1.0/m_dataHeader.sampleRate)/1000.0;
}

void CSenderThread::saveStartTime(const ACS::Time& time)
{
	/*if (time!=0) {*/
		m_startTime.value(time);
		/*m_immediateStart=false;
	} 
	else {
		m_immediateStart=true;
	}*/
}

/*void CSenderThread::setKelvinCountsRatio(const ACS::doubleSeq& ratio)
{
	for (unsigned i=0;i<ratio.length();i++) {
		m_KCountsRatio[i]=ratio[i];
	}
}*/

void CSenderThread::saveZero(DWORD *zeros)
{
	for (int i=0;i<MAX_SECTION_NUMBER;i++) {
		m_zeroBuffer[i]=zeros[i];
	}
}

void CSenderThread::processData(DWORD dataSize,const long& sectionNumber,long *boards)
{
	BYTE *currentPointer;
	//DWORD *data;
	DWORD data[MAX_SECTION_NUMBER];
	WORD counter;
	WORD status;
	TIMEVALUE sampleTime;
	DWORD bufferCounter=0;
	memcpy(m_tempBuffer+m_tempBufferPointer,m_inputBuffer,dataSize);
	m_tempBufferPointer+=dataSize;
	currentPointer=m_tempBuffer;
	while (bufferCounter+m_configuration->getDataBufferSize()<=m_tempBufferPointer) {
		currentPointer=m_tempBuffer+bufferCounter;
		CProtocol::decodeData(currentPointer,m_dataHeader.sampleRate,m_previousStatus,m_previousCounter,sampleTime,counter,status,data,sectionNumber,boards);
		bufferCounter+=m_configuration->getDataBufferSize();
		if (!CProtocol::isNewStream(m_previousStatus,m_previousCounter,counter)) { //the first sample is discarded
			if  (m_startTime<=sampleTime.value())/* || (m_immediateStart)) */{ //if the start time is elapsed than start processing
				for (int i=0;i<MAX_SECTION_NUMBER;i++) { // subtract the zero level
					if (data[i]>m_zeroBuffer[i]) {
						data[i]-=m_zeroBuffer[i];
					}
					else {
						data[i]=0;
					}
				}
				if (CProtocol::isCal(m_previousStatus)) { // the sample is a calibration diode on
					computeSample(m_cal,sampleTime,data,true);
				}
				if (CProtocol::isTpi(m_previousStatus)) { // the sample is a normal tpi measure
					computeSample(m_tpi,sampleTime,data,false);
				}
			}
		}
		else {
			clearIntegration(m_tpi);
			clearIntegration(m_cal);
			ACS_LOG(LM_FULL_INFO,"CCSenderThread::processData()",(LM_INFO,"STREAM_STARTED"));
		}
		m_previousStatus=status; // the current status refers to the next sample
		m_previousCounter=counter;
	}
	if (bufferCounter>0) {
		m_tempBufferPointer-=bufferCounter;
		memcpy(m_tempBuffer,m_tempBuffer+bufferCounter,m_tempBufferPointer);
	}
}

void CSenderThread::initTransfer()
{
	clearIntegration(m_tpi);
	clearIntegration(m_cal);
	m_previousStatus=0;
	m_previousCounter=0;
}

void CSenderThread::clearIntegration(TSampleRecord& samp)
{
	samp.started=false;
	for (int i=0;i<MAX_SECTION_NUMBER;i++) samp.tpi[i]=0;
	samp.start.reset();
	samp.counts=0;
	samp.accumulations=0;
}

void CSenderThread::computeSample(TSampleRecord& samp,TIMEVALUE& sampleTime,DWORD *data,bool isCal)
{
	if (!samp.started) { // if the integration is not started yet
		double accTemp;
		IRA::CIRATools::timeCopy(samp.start,sampleTime); //save the start time
		samp.started=true; // mark the integration started;
		accTemp=(double)m_dataHeader.integration*1000.0*m_dataHeader.sampleRate;
		samp.accumulations=(DWORD)round(accTemp); //computes the number of integrations
		if (samp.accumulations<1) samp.accumulations=1; //at least one accumulation is needed
	}
	for (int i=0;i<MAX_SECTION_NUMBER;i++) { // computes the new data....
		samp.tpi[i]+=(SAMPLETYPE)(double(data[i])/(m_dataHeader.normalization*(double)samp.accumulations));
	}
	samp.counts++; //increase the number of accumulations
	if (samp.counts==samp.accumulations) { // if the required number of accumulations is reached.....
		// computes the mean time
		ACS::Time diff=samp.start.value().value+(sampleTime.value().value-samp.start.value().value)/2;
		samp.start.value(diff);
		// now send the data		
		SAMPLETYPE sample[MAX_SECTION_NUMBER];
		double  tsys[MAX_SECTION_NUMBER],allTsys[MAX_SECTION_NUMBER];
		Backends::TDumpHeader hd;
		//TDumpRecord *buffer=(TDumpRecord *)new char[sizeof(TDumprecord)];
		//char buffer[sizeof(SAMPLETYPE)*MAX_INPUT_NUMBER+sizeof(Backends::TDumpHeader)];
		hd.calOn=isCal;
		hd.time=samp.start.value().value;
		hd.dumpSize=m_dataHeader.sampleSize*m_dataHeader.channels; // add data size.
		hd.dumpSize+=sizeof(double)*m_dataHeader.channels; // add data size required for tsys trasmission
		ACE_Message_Block buffer(hd.dumpSize+sizeof(Backends::TDumpHeader));
		//memcpy(buffer,&hd,sizeof(Backends::TDumpHeader));
		for (long jj=0;jj<MAX_SECTION_NUMBER;jj++) {  //computes the system temperatures for all inputs
			allTsys[jj]=(double)samp.tpi[jj]*m_KCountsRatio[jj];
		}
		for (long i=0;i<m_dataHeader.channels;i++) {
			sample[i]=samp.tpi[m_dataHeader.id[i]];  // stores the tpi for the enabled sections
			tsys[i]=allTsys[m_dataHeader.id[i]]; // stores the tsys for th enable inputs
			//printf("section %ld, %lf, %lf, %ld\n",i,sample[i],tsys[i],m_dataHeader.id[i]);
		}
		//if (buffer.copy((const char *)&hd,sizeof(Backends::TDumpHeader))==0) printf("COPIA FATTA CON SUCCESSO!!\n");
		//if (buffer.copy((const char *)sample,hd.dumpSize)==0) printf("COPIA FATTA CON SUCCESSO!!\n");
		buffer.copy((const char *)&hd,sizeof(Backends::TDumpHeader));
		buffer.copy((const char *)tsys,sizeof(double)*m_dataHeader.channels);
		buffer.copy((const char *)sample,m_dataHeader.sampleSize*m_dataHeader.channels);
		//memcpy(buffer+sizeof(Backends::TDumpHeader),sample,hd.dumpSize);
		#ifndef BKD_DEBUG
		try {
		     // m_sender->getSender()->sendData(FLOW_NUMBER,(const char *)&buffer,
		     //		  sizeof(Backends::TDumpHeader)+buffer.hd.dumpSize);
			//m_sender->getSender()->sendData(FLOW_NUMBER,(const char *)&hd,sizeof(Backends::TDumpHeader));
			//m_sender->getSender()->sendData(FLOW_NUMBER,(const char *)sample,hd.dumpSize);
			//if (!m_stop) {
			if (m_go) {
				m_sender->getSender()->sendData(FLOW_NUMBER,&buffer);
				m_sending=true;
			}
			else {
			}
		      //m_sender->getSender()->sendData(FLOW_NUMBER,(const char *)sample,hd.dumpSize);
		}
		catch (AVSendFrameErrorExImpl& ex) {
			_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"CSenderThread::computeSample()");
			impl.setDetails("raw data could not be sent");
			//impl.log();
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSenderThread::computeSample()");
			//throw impl.getComponentErrorsEx();
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		#else
		if (m_go) {
			printf("Time: %d-%d, %02d:%02d:%02d.%d Size: %d Cal: %d\n ",samp.start.year(),samp.start.dayOfYear(),samp.start.hour(),
										samp.start.minute(),samp.start.second(),samp.start.microSecond(),hd.dumpSize,hd.calOn);
			for (int y=0;y<m_dataHeader.channels;y++) {
				printf("%ld %f - ",m_dataHeader.id[y],sample[y]);
			}
			printf("\n");
			m_sending=true;
		}
		#endif
		buffer.reset();
		//finally clear the integration...in order to stat a new one the next step
		clearIntegration(samp);
	}
}
