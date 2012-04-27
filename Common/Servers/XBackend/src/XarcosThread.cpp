// $Id: XarcosThread.cpp,v 1.47 2010/08/26 13:29:34 bliliana Exp $

#include "XBackendsImpl.h"
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include "XarcosThread.h"
#include <LogFilter.h>

using namespace IRA;

_IRA_LOGFILTER_IMPORT;

XarcosThread::XarcosThread(const ACE_CString& name,TSenderParameter  *par,
  const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime): ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("XarcosThread::XarcosThread()");
	ACS_DEBUG("XarcosThread::XarcosThread()"," control thread starts");
	Timer::TimerParameter timerPar;
	ACS::Time timeTp=(ACS::ThreadBase::defaultSleepTime)*2-300000;//<200 millisecond
	ACS::Time timeInt=(ACS::ThreadBase::defaultSleepTime)*100;//=10 second
	long idTp=0;
	long idInt=0;	
	tpTimer=NULL;
	intTimer=NULL;
	m_pControl=par->sender;
    m_commandLine=par->commandLine;
    groupS=par->group;
    commandL=par->command;
    m_GroupSpectrometer=par->groupSpectrometer;
    timerPar.sender=this;
    timerPar.interval=2000000;//200 millisecond
    timerPar.tp=true;
    Timer::TimerParameter *tmp=&timerPar;
    tpTimer=getThreadManager()->create<Timer,Timer::TimerParameter *>("TpTimer",tmp,timeTp,timeTp);//=200 millisecond
    idTp=tmp->TimerId;
    tpTimer->resume();
    timerPar.interval=100000000;//10 second
    timerPar.tp=false;
    intTimer=getThreadManager()->create<Timer,Timer::TimerParameter *>("intTimer",tmp,timeInt,timeInt);//=10 second
    idInt=tmp->TimerId;
    intTimer->resume();
    ACS_DEBUG_PARAM("XarcosThread::XarcosThread()"," idTp= %i ",idTp);
    ACS_DEBUG_PARAM("XarcosThread::XarcosThread()"," idInt= %i ",idInt);
    groupS->dataReady=false;
    INT=false;	
    m_stop=true;
    m_go=m_sending=false;
    XDataThread::DataParameter dataPar;
    dataPar.sender=this;
    dataPar.group=par->group;
    XDataThread::DataParameter *temp=&dataPar;
    data=new XDataThread("data",temp,ACS::ThreadBase::defaultResponseTime,ACS::ThreadBase::defaultSleepTime);//=10 second
// 	getThreadManager()->create<XDataThread,XDataThread::DataParameter *>("data",temp,ACS::ThreadBase::defaultResponseTime,ACS::ThreadBase::defaultSleepTime);//=10 second
//  data->resume();
	for (int i=0;i<MAX_INPUT_NUMBER;i++) {
		m_zeroBuffer[i]=0;
		m_KCountsRatio[i]=1.0;
		m_tp[i]=0.0;
	}
	countFile=0;
	m_fileOpened=false;
	sample=NULL;
}

XarcosThread::~XarcosThread()
{ 
	AUTO_TRACE("XarcosThread::~XarcosThread()");
	ACS_DEBUG("XarcosThread::~XarcosThread()"," control thread stop");
	if(tpTimer!=NULL){ 
		getThreadManager()->destroy(tpTimer);
		tpTimer=NULL;
	}
	if (intTimer!=NULL) { 
		getThreadManager()->destroy(intTimer);
		intTimer=NULL;
	}
}
void XarcosThread::VisualizzaSpecific()
{
	AUTO_TRACE("XarcosThread::VisualizzaSpecific()");
	HeadResult temp;
	CString str("");
	int s=0;	
	
	str.Concat("\n\n ******* Specific *******");
	str.Concat("\n TempoIntegrazione = ");
	str.Concat(groupS->Xspec.GetTempoIntegrazione());
	str.Concat(" sec\n");
	if(groupS->Xspec.GetModo8bit())	str.Concat(" Modo8bit = TRUE"); 
	else str.Concat(" Modo8bit = FALSE");
	str.Concat("\n NSezioni = ");
	str.Concat(groupS->Xspec.GetNSezioni());
	str.Concat("\n");
	while(s<groupS->Xspec.GetNSezioni()){
		temp=groupS->Xspec.specificaSezione[s];
		str.Concat("\n Flo = ");
		str.Concat((int)temp.GetFlo());	
		str.Concat(" Hz    Gain = ");
		str.Concat((int)temp.GetGain());
		str.Concat("    ChIn =");
		str.Concat(temp.GetAdc());		
		str.Concat("    Banda = ");
		str.Concat((int)temp.GetBanda());			
		str.Concat(" Hz    ");
		if(temp.GetModoPol()) str.Concat("ModoPolarimetric = TRUE"); 
		else str.Concat("ModoPolarimetric = FALSE");		
		str.Concat("    Index = ");
		str.Concat((int)temp.Getindex());
		str.Concat(" \n");
		s++;
	}
	str.Concat("\n *******  *******");
	std::cout << str << std::endl;


}

void XarcosThread::VisualizzaDato()
{
	AUTO_TRACE("XarcosThread::VisualizzaDato()");
	HeadResult temp;
	CString str("");
	Data Dato;
	int s=0;
	
	str.Concat("\n\n ******* DATO SPETTROMETRI *******");
	str.Concat("\n Numero Spettrometri = ");
	str.Concat(groupS->GetNumSpec());
	str.Concat(" \n");
	while(s<groupS->GetNumSpec()){	
		str.Concat("IntestazioneCh1: ");
		Dato=groupS->dato[s];
		temp=Dato.GetIntestazioneCh1();
		str.Concat("  Flo = ");
		str.Concat((int)temp.GetFlo());	
		str.Concat(" Hz    Gain = ");
		str.Concat((int)temp.GetGain());
		str.Concat("    Adc =");
		str.Concat(temp.GetAdc());		
		str.Concat("    Banda = ");
		str.Concat((int)temp.GetBanda());			
		str.Concat(" Hz    ");
		if(temp.GetModoPol()) str.Concat("ModoPolarimetric = TRUE"); 
		else str.Concat("ModoPolarimetric = FALSE");
		str.Concat("    Index = ");
		str.Concat((int)temp.Getindex());	
		str.Concat("\nIntestazioneCh2: ");
		temp=Dato.GetIntestazioneCh2();
		str.Concat("  Flo = ");
		str.Concat((int)temp.GetFlo());	
		str.Concat(" Hz    Gain = ");
		str.Concat((int)temp.GetGain());
		str.Concat("    Adc =");
		str.Concat(temp.GetAdc());		
		str.Concat("    Banda = ");
		str.Concat((int)temp.GetBanda());			
		str.Concat(" Hz    ");
		if(temp.GetModoPol()) str.Concat("ModoPolarimetric = TRUE"); 
		else str.Concat("ModoPolarimetric = FALSE");
		str.Concat("    Index = ");
		str.Concat((int)temp.Getindex());	
		str.Concat(" \n");		
		s++;
	}
	str.Concat("\n *******  *******");
	std::cout << str << std::endl;
	
	ACS_DEBUG("XarcosThread::VisualizzaDato()"," control thread starts");	
}

void XarcosThread::Init()
{
	AUTO_TRACE("XarcosThread::Init()");
	ACS_DEBUG("XarcosThread::Init()"," control thread starts");
	ACS_DEBUG_PARAM("XarcosThread::Init()"," section %i ",groupS->section);
//	VisualizzaDato();
	groupS->abort=false;
	groupS->Setting();		
//	VisualizzaDato();
	groupS->Init();//Configuro Specifiche nell'HW
	groupS->setting=true;			
	ACS_DEBUG_PARAM("XarcosThread::Init()"," section %i ",groupS->section);
	ACS_DEBUG("XarcosThread::Init()"," control thread starts");
}

void XarcosThread::setKelvinCountsRatio(const ACS::doubleSeq& ratio)
{
	for (unsigned i=0;i<ratio.length();i++) {
		m_KCountsRatio[i]=ratio[i];
	}
}

void XarcosThread::getTpZero(DWORD *tpi)
{	

	int chs=0;
	float norm=1;//(float)(1000/200);//Normalizzare al secondo 
	vector<double> tp;
	
	AUTO_TRACE("XarcosThread::getTpZero()");
	ACS_DEBUG("XarcosThread::getTpZero()"," control thread starts");
	
	
	chs=groupS->section*2;
		
	int c=tp.capacity();
	if(c<chs) tp.resize(chs);
	for (c=0;c<chs;c++) tp[c]=0;
	
	groupS->GetDataTpZero(true,tp);//GAIN=0
	int i=0;
	for(int e=0;e<chs;e++){		
		if(groupS->enabled[e]){	
			ACS_DEBUG_PARAM("XarcosThread::getTpZero()"," e=%i ",e);
			ACS_DEBUG_PARAM("XarcosThread::getTpZero()"," tpiZero %lf ",tp[i]);
			tpi[i]=m_zeroBuffer[i]=(unsigned int)(tp[e]*norm);//Copio i tp !!
			i++;
		}
	}
}

void XarcosThread::getTp(DWORD *tpi)
{	
	int chs=0;
	float norm=1;//(float)(1000/200);//Normalizzare al secondo 
	vector<double> tp;
	
	AUTO_TRACE("XarcosThread::getTp()");
	ACS_DEBUG("XarcosThread::getTp()"," control thread starts");

	
	chs=groupS->section*2;

	int c=tp.capacity();
	if(c<chs) tp.resize(chs);
	for (c=0;c<chs;c++) tp[c]=0;
	
	groupS->GetDataTpZero(false,tp);
	int i=0;
	for(int e=0;e<chs;e++){		
		if(groupS->enabled[e]){
			ACS_DEBUG_PARAM("XarcosThread::getTpZero()"," e=%lf ",e);
			ACS_DEBUG_PARAM("XarcosThread::getTpZero()"," tpiZero=%lf ",tp[i]);
			tpi[i]=(unsigned int)(tp[e]*norm);//Copio i tp !!
			i++;
		}
	}
}

void XarcosThread::AbortInt()
{
	AUTO_TRACE("XarcosThread::Abort");
	ACS_DEBUG("XarcosThread::Abort"," control thread starts");
	INT=false;
//	data->suspend();
	tpTimer->Stop();
    intTimer->Stop();
    groupS->AbortInt();//Integrazione abortita dall'esterno!!
	groupS->active=false;
	groupS->abort=true;
	ACS_DEBUG("XarcosThread::Abort"," control thread starts");
}

void XarcosThread::StartInt()
{
	DataIntegrazione *RisulInt;
	AUTO_TRACE("XarcosThread::Start");
	ACS_DEBUG("XarcosThread::Start"," control thread starts");
	groupS->active=true;
	groupS->overflowData=false;
	groupS->dataReady=false;
	groupS->Xspec.SetNCicli(groupS->Xspec.GetTempoIntegrazione()/groupS->tempoEle);
	INT=true;
    for(int i=0;i<groupS->Xspec.GetMaxSezioni();i++) {
		RisulInt=&groupS->RisultatiIntegrazione[i];
		RisulInt->SetZeroDataIntegratione();
	}
    groupS->AdcSetFunction(groupS->Xspec.GetFunctionAdc());
    if (groupS->Xspec.GetNCicli()==0) return;	
    IRA::CIRATools::getTime(m_startTime);
	groupS->StartInt();		
	tpTimer->Start();
    intTimer->Start();    
//   data->resume();
}

void XarcosThread::onStart()
{
	AUTO_TRACE("XarcosThread::onStart()");
	ACS_DEBUG("XarcosThread::onStart()"," control thread starts");
}
	
void XarcosThread::onStop()
{
	AUTO_TRACE("XarcosThread::onStop()");
	ACS_DEBUG("XarcosThread::onStop()"," control thread stops");
}

void XarcosThread::runLoop()
{
	int sizedata=0;
	int sizetp=0;
	
	if(INT){
		if(groupS->Xspec.GetNCicli()!=0){
			if(tpTimer->event){
				tpTimer->event=false;				
			    groupS->GetDataTp(false);
			    groupS->countTp++;
			}
			if(intTimer->event){
				intTimer->event=false;
				groupS->Xspec.SetNCicli( groupS->Xspec.GetNCicli()-1);
				data->StartSem.release(1);//Rilascio il thread per il recupero dell'integrazione
			}
		}
		if(groupS->Xspec.GetNCicli()==0){
			tpTimer->Stop();
		    intTimer->Stop();
		    data->StopSem.acquire(1);//Aspetto che sia terminato il recupero dell'ultima integrazione
			groupS->AbortDataTp();
			groupS->AbortInt();//Abort();
			groupS->abort=false;
			groupS->setting=false;	
			groupS->active=false;
			INT=false;//Integrazione Finita!!
			groupS->dataReady=true;
			sample=preprocessData(&sizedata,&sizetp);	
//				ACS_DEBUG("XarcosThread::runLoop()"," Provo a salvare ");
#ifdef BKD_DEBUG 
			SaveFileTxt(sample,sizedata,sizetp);
			SaveFileFits();
#else 					
			SaveFileTxt(sample,sizedata,sizetp);
			SaveFileFits();		
			SAMPLETYPE buf[sizedata];
			double tsys[sizetp];//m_dataHeader.channels
			Backends::TDumpHeader hd;
			hd.dumpSize=sizeof(double)*sizedata;
			m_dataHeader.sampleSize=SAMPLESIZE;
			hd.calOn=false;
			hd.time=m_startTime.value().value;
//			hd.dumpSize+=m_dataHeader.sampleSize*sizetp; // add size of Tp.
			hd.dumpSize+=sizeof(double)*sizetp;//m_dataHeader.channels; // add data size required for tsys trasmission
			ACS_DEBUG_PARAM("XarcosThread::runLoop()","hd.dumpSize %i ",hd.dumpSize);
			ACE_Message_Block buffer(hd.dumpSize+sizeof(Backends::TDumpHeader));
			for (int x=0;x<sizedata;x++) buf[x]=sample[x+sizetp];//salto i Tp
			for (long i=0;i<sizetp;i++) {
				tsys[i]=(double)sample[i]*m_KCountsRatio[i];
			}
			buffer.copy((const char *)&hd,sizeof(Backends::TDumpHeader));
			buffer.copy((const char *)tsys,sizeof(double)*sizetp);//m_dataHeader.channels);
			buffer.copy((const char *)buf,sizeof(double)*sizedata);//m_dataHeader.sampleSize*m_dataHeader.channels);
			ACS_DEBUG("XarcosThread::runLoop()"," sto per spedire!!!!! ");
			try{
				m_pControl->getSender()->sendData(FLOW_NUMBER,&buffer);
				m_sending=true;
			}		
			catch (AVSendFrameErrorExImpl& ex) {
				_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XarcosThread::runLoop()");
				impl.setDetails("raw data could not be sent");
				impl.log(LM_DEBUG);
//				throw impl.getBackendsErrorsEx();
				_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
			}
			catch (...) {
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"XarcosThread::runLoop()");
				impl.log(LM_DEBUG);
//				throw impl.getComponentErrorsEx();
				_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
			}
#endif		
		}
	}
	else{
		if(!m_stop){
			Init();
			StartInt();
		}
	}
	if(sample!=NULL) delete[] sample;
	sample=NULL;
}

void XarcosThread::saveDataHeader(Backends::TMainHeader* mH,Backends::TSectionHeader* cH)
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

void XarcosThread::saveStartTime(const ACS::Time& time)
{
	m_startTime.value(time);
}

void XarcosThread::saveZero(DWORD *zeros)
{
	for (int i=0;i<MAX_INPUT_NUMBER;i++) {
		m_zeroBuffer[i]=zeros[i];
	}
}


double * XarcosThread::preprocessData(int *rest, int *Ntp)
{
	int k=0,c=0,a=0,lung=0, chs=0, res=0;
	float norm=1;
	DataIntegrazione *RisulInt;
	
	chs=groupS->input;
	for(int i=0;i<groupS->Xspec.GetMaxSezioni();i++) {
		RisulInt=&groupS->RisultatiIntegrazione[i];
		if (RisulInt->GetValoriValidi()) {
			lung=RisulInt->GetLungArrayDati();
			if(RisulInt->GetNArrayDati()==1){
				res+=lung;
			}
			else{
				res+=lung*4;
			}
		}
	}
	double *t=new double[chs];
	double *d=new double[res+chs];
	for(int i=0;i<res+chs;i++) d[i]=0;
	for(int i=0;i<chs;i++) t[i]=0;
	k=0;
	a=0;
	if(groupS->countTp!=0) norm=(float)1.0/groupS->countTp;
	for(int i=0;i<groupS->Xspec.GetMaxSezioni();i++) {
		RisulInt=&groupS->RisultatiIntegrazione[i];
		if (RisulInt->GetValoriValidi()) {
			lung=RisulInt->GetLungArrayDati();
			if(RisulInt->GetNArrayDati()==1){
				t[a]=RisulInt->tp[0]*norm;
				a++;				
				for(c=0;c<lung;c++,k++)	d[k]=RisulInt->xx[c];
			}
			else{
				t[a]=RisulInt->tp[0]*norm;
				a++;
				t[a]=RisulInt->tp[1]*norm;
				a++;
				for(c=0;c<lung;c++,k++){
					d[k]=RisulInt->xx[c];
					d[k+lung]=RisulInt->yy[c];
					d[k+lung*2]=RisulInt->xy[c].real();
					d[k+lung*3]=RisulInt->xy[c].imag();
				}
				k=k+lung*3;
			}
		}
	}	
	*Ntp=a;	
	for(int i=0;i<a;i++) m_tp[i]=t[i];
	for(c=(res+chs-1);(c>=0&&k>0);c--,k--) d[c]=d[k-1];//Metto tutto in fondo!!
	for(;(c>=0&&a>0);c--,a--) d[c]=t[a-1];//Metto i tp in testa !!
	*rest=res;
	delete[] t;	
	return d;
}

void XarcosThread::SaveFileTxt(double * buff, int size, int sizetp)
{	
	std::string project="XBackends";
	std::string observer="Liliana";
	IRA::CString FileName("");
	Backends::TMainHeader mH;
	Backends::TSectionHeader *cH;
	TIMEVALUE now;
	IRA::CIRATools::getTime(now); // it marks the start of the activity job
	m_fileOpened=false;
	FileName.Format("/alma/XOut_%02d_%02d_%02d.txt",now.hour(),now.minute(),now.second());
	if (!m_fileOpened) {
		// create the file and save main headers
		t_file.open((const char *)FileName,ios_base::out|ios_base::trunc);
		if (!t_file.is_open()) {
			_EXCPT(ComponentErrors::FileIOErrorExImpl,impl,"CEngineThread::SaveFileTxt()");
			impl.setFileName((const char *)FileName);
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		else {
			// save the headers
			IRA::CString out("");
			commandL->fillMainHeader(buffer.header);
			mH=buffer.header;
			if(mH.integration==0) mH.integration=10;
			out.Format("Main - ch:%d, beams:%d, sampleSize:%d, integration:%d, ",mH.sections,mH.beams,mH.sampleSize,mH.integration);
			t_file<< (const char *) out;
			t_file << endl;
			commandL->fillChannelHeader(buffer.chHeader,buffer.header.sections);
			cH=buffer.chHeader;
			for (int j=0;j<mH.sections;j++) {
				out.Format("channel id:%d, bins:%d, pol:%d, bandWidth:%lf, frequency:%lf, attenuation:%lf,sampleRate:%lf, feed:%d,",
						cH[j].id,cH[j].bins,cH[j].polarization,cH[j].bandWidth, cH[j].frequency,cH[j].attenuation,cH[j].sampleRate,cH[j].feed);
				t_file<< (const char *) out;	
				t_file << endl;				
			}
			t_file << endl;
			int k=0;
			int j=sizetp;//Salto i tp
			for (int i=0;i<mH.sections;i++) {
				out.Format(" channel:%i,",i);
				t_file << (const char *) out;
				if(cH[i].polarization<=1){
					out.Format("%10.5lf,%10.5lf,",buff[k],buff[k+1]);
					t_file << (const char *) out;
					t_file << endl;			
					k++;k++;
					for (;j<(i+1)*cH[i].bins;j++) {
						out.Format("%10.5lf, ",buff[j]);
						t_file << (const char *) out;
					}
				}
				else{
					out.Format("%10.5lf,%10.5lf,",buff[k],buff[k+1]);
					t_file << (const char *) out;
					t_file << endl;
					k++;k++;
					for(int s=0;s<4;s++ ){
						for (;j<(i+1)*(cH[i].bins*(s+1));j++) {
							out.Format("%10.5lf, ",buff[j]);
							t_file << (const char *) out;
						}
					}
				}
				t_file << endl;
			}
			m_fileOpened=true;
			ACS_LOG(LM_FULL_INFO, "CEngineThread::SaveFileTxt()",(LM_NOTICE,"FILE_OPENED %s",(const char *)FileName));
		}	
	}
		//save all the data in the buffer an then finalize the file
	t_file.close();
	m_fileOpened=false;
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try{
		line->setFileName(FileName);//Modifico l'attributo di ACS
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"XarcosThread::SaveFileTxt()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}	
	ACS_LOG(LM_FULL_INFO, "CEngineThread::SaveFileTxt()",(LM_NOTICE,"FILE_FINALIZED"));

}

void XarcosThread::SaveFileFits()
{	
//	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	std::string project="XBackends";
	std::string observer="Liliana";
	fileName=new CString("XOut.fits");

	m_fileOpened=false;
	if (!m_fileOpened) {
		m_file = new CFitsWriter();
		TIMEVALUE now;
		IRA::CIRATools::getTime(now); // it marks the start of the activity job
		m_file->setBasePath("/alma/");	
		fileName->Format("XOut_%02d_%02d_%02d.fits",now.hour(),now.minute(),now.second());
		m_file->setFileName((const char*)*fileName);			
		ACS_DEBUG("XarcosThread::SaveFileFits()"," create() MBFits ");
		if (!m_file->create()) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		ACS_DEBUG("XarcosThread::SaveFileFits()"," fillMainHeader() MBFits ");
		commandL->fillMainHeader(buffer.header);
		ACS_DEBUG("XarcosThread::SaveFileFits()"," saveMainHeader() MBFits ");
		if (!m_file->saveMainHeader(buffer.header)) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		else if (!m_file->setPrimaryHeaderKey("Project_Name",project,"Name of the project")) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		else if (!m_file->setPrimaryHeaderKey("Observer",observer,"Name of the observer")) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		ACS_DEBUG("XarcosThread::SaveFileFits()"," fillChannelHeader() MBFits ");
		commandL->fillChannelHeader(buffer.chHeader,buffer.header.sections);
		ACS_DEBUG("XarcosThread::SaveFileFits()"," saveChannelHeader() MBFits ");
		if (!m_file->saveChannelHeader(buffer.chHeader)) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		ACS_DEBUG("XarcosThread::SaveFileFits()"," addChannelTable() MBFits ");
		if (!m_file->addChannelTable("CHANNEL TABLE")) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		ACS_DEBUG("XarcosThread::SaveFileFits()"," setChannelHeaderKey() MBFits ");
		if (!m_file->setChannelHeaderKey("Local_Oscillator",17900.0,"Local oscillator in MHz")) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		ACS_DEBUG("XarcosThread::SaveFileFits()"," addFeedTable() MBFits ");
		CFitsWriter::TFeedHeader feedH[MAX_DEFAULT_BEAM];
		if (!m_file->addFeedTable("FEED TABLE")) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		ACS_DEBUG("XarcosThread::SaveFileFits()"," for addFeedTable() MBFits ");
		for (int j=0;j<MAX_DEFAULT_BEAM;j++) {
			if (!m_file->saveFeedHeader(feedH[j])) {
				_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
				impl.setFileName((const char *)*fileName);
				impl.setError(m_file->getLastError());
				impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
			}
		}
		ACS_DEBUG("XarcosThread::SaveFileFits()"," addDataTable() MBFits ");
		if (!m_file->addDataTable("DATA TABLE")) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
			impl.setFileName((const char *)*fileName);
			impl.setError(m_file->getLastError());
			impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
		}
		m_fileOpened=true;
	}
	processData(buffer.header.sections);
	m_file->add_row();
	delete m_file; // file close called direclty by the class destructor
	m_fileOpened=false;
}

void XarcosThread::storeDataPol(DataIntegrazione *RisulInt,int i)
{
	int c=0;
	int lung=RisulInt->GetLungArrayDati();
	vector<double> d;
	
	d.resize(RisulInt->GetLungArrayDati()*4);	
	for(c=0;c<lung;c++){
		d[c]=RisulInt->xx[c];
		d[c+lung]=RisulInt->yy[c];
		d[c+lung*2]=RisulInt->xy[c].real();
		d[c+lung*3]=RisulInt->xy[c].imag();
	}
	if (!m_file->storeData(&d[0],lung*4,i)) {
		_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
		impl.setFileName((const char *)*fileName);
		impl.setError(m_file->getLastError());
		impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
	}
}

bool XarcosThread::processData(int max)
{		
	CSecAreaResourceWrapper<GroupSpectrometer> groupSpec=m_GroupSpectrometer->Get();
	int index=0;
	bool calOn=false;
	long bins=0;
	double ra=0,dec=0;
	double az=0,el=0;
	bool tracking=false;
	double hum=0,temp=0,press=0;
	CFitsWriter::TDataHeader tdh;
	DataIntegrazione *RisulInt;
//	TIMEVALUE tS;
//	tS.value(time);
	//CDateTime tConverter(tS,m_data->getDut1()); 
	tdh.time=0;//tConverter.getMJD();
	
	tdh.raj2000=ra;
	tdh.decj2000=dec;
	tdh.az=az;
	tdh.el=el;
	tdh.par_angle=0;//IRA::CSkySource::paralacticAngle(tConverter,m_data->getSite(),az,el);
	tdh.derot_angle=0.333357887; /*********************** get it Now is fixed to 19.1 degreees*******************/
	tdh.tsys=1.0;              /// ********************** obtain some how
	tdh.flag_cal=calOn;
	tdh.flag_track=tracking;
	//m_data->getMeteo(hum,temp,press);
	tdh.weather[0]=hum;
	tdh.weather[1]=temp;
	tdh.weather[2]=press;
	if (!m_file->storeAuxData(tdh)) {
		_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
		impl.setFileName((const char *)*fileName);
		impl.setError(m_file->getLastError());
		impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
	}
	for(int i=0;i<groupSpec->GetNumSpec();i++) {
		RisulInt=&groupSpec->RisultatiIntegrazione[i];
		if (RisulInt->GetValoriValidi()) {
			if (index<max) {				
				bins=DEFAULT_BINS;
				if(RisulInt->GetNArrayDati()==1){
					if (!m_file->storeData(&RisulInt->xx[0],bins,i)) {
						_EXCPT(ManagementErrors::FitsCreationErrorExImpl,impl,"XarcosThread::SaveFileFits()");
						impl.setFileName((const char *)*fileName);
						impl.setError(m_file->getLastError());
						impl.log(LM_ERROR); // not filtered, because the user need to know about the problem immediately
					}
				}
				else{
					storeDataPol(RisulInt,i);
				}
				index++;				
			}
		}
	}
	m_file->add_row();
	return true;
}
