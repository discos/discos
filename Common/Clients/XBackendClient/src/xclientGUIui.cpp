#include "../include/xclientGUIui.h"
#include "Qt/qmessagebox.h"
#include <IRA>
#define MAX_GAIN 256
#define MAXBINS 4096

//static char hostname[]="localhost";

xclientGUIui::xclientGUIui(QWidget *parent)
: QMainWindow(parent)
{	
	DataIntegrazione Risul;

	ui.setupUi(this);
	connect( ui.action_Exit, SIGNAL( activated() ), this, SLOT( FileExit() ) );
	connect( ui.actionXPlot, SIGNAL( activated() ), this, SLOT( OpenXplot() ) );
	connect( ui.pushButtonPrec, SIGNAL( clicked() ), this, SLOT( Precedente_activated() ) );
	connect( ui.pushButtonSucc, SIGNAL( clicked() ), this, SLOT( Successivo_activated() ) );
	connect( ui.pushButtonInt, SIGNAL( clicked() ), this, SLOT( Integra() ) );
	connect( ui.Modo8Bit, SIGNAL( clicked() ), this, SLOT( Check() ) );
	connect( ui.pushButtonStart,SIGNAL( clicked() ), this, SLOT( StartInt() ));
	connect( ui.pushButtonAbort,SIGNAL( clicked() ), this, SLOT( AbortInt() ));
	connect( ui.pushButtonBroad,SIGNAL( clicked() ), this, SLOT( Broadcast() ));
	connect( &XThread, SIGNAL( upState(char) ), this, SLOT( UpdateStatus(char) ));
    connect( &XThread, SIGNAL(setGUICatchAllErrorCode()), this, SLOT(changeGUICatchAllErrorCode()));
    initAttributes ( );
   
    Risul.SetZeroDataIntegratione();
    RisultatiIntegrazione.reserve(spec.GetMaxSezioni());
	for(int i=0;i<spec.GetMaxSezioni();i++){
		RisultatiIntegrazione.push_back(Risul);//Creo e Azzero la struttura vector
	}
	
	//XThread= new XControlSystem();
	p=new Xplot(parent,spec.GetMaxSezioni(),&RisultatiIntegrazione);
	//p->show();
    ledHW=new Led(ui.HW,QString("HW"));
    ledData=new Led(ui.Data,QString("DataReady"));
    ledOverflowData=new Led(ui.Overflow,QString("OverflowData"),Led::Red);
    ledAbort=new Led(ui.Abort,QString("Abort"));
    ledActive=new Led(ui.Active,QString("Active"));
    ledSetting=new Led(ui.Setting,QString("Setting"));   
    ledBusy=new Led(ui.Busy,QString("Busy"),Led::Red);

}

void xclientGUIui::setParameters(maci::SimpleClient* theClient,Backends::XBackends_var theSender, Management::FitsWriter_var theRecv)
{
	sc=theClient; 
	sender=theSender;
	recv=theRecv;
	XThread.setSimpleClient(theClient);
	XThread.setSender(theSender);
	XThread.setRecv(theRecv);
	XThread.start();
	INT=false;
}

xclientGUIui::~xclientGUIui()
{
	if(p!=NULL) delete p;
}

void xclientGUIui::OpenXplot(){
	int lung=0,pos=0;
	
	CString cp(XThread.NameFile);
	lung=cp.GetLength();
	if(lung>0){
		char str[lung];
		for (int x=lung-1;x>=0;x--){
			if((pos==0)&&(cp[x]=='/')) pos=x;
			str[x]=cp[x];
		}
		if(pos<lung){
			str[pos]='\0';//path
			char name[lung-pos];
			for (int x=0;x<lung-pos;x++) name[x]=cp[pos+x+1];
			name[lung-pos-1]='\0';
			OpenFile(str,name);
		}
		else OpenFile(str,"");
	}
	p->show();

	//p->Visual(groupSpec.Xspec.GetNSezioni());
}


bool xclientGUIui::OpenFile(char path[],char name[])
{
	int index=0,channel=0,m=0,d=0,lung=0,count=0;
	char car='\0';
	bool ok=false;
	QString fn(path); //= QFileDialog::getOpenFileName(this, "Choose a file to open", path, QString::null);
	char str[512];
	double ch=0,beams=0,sampleSize=0,integration=0,val=0,bins=0,pola=0;
	double *vett=new double[(MAXBINS*4)+2];
	HeadResult head;
	DataIntegrazione *RisulInt;
	vector<HeadResult> sezioni;
  
	head.SetZeroHeadResult();
	sezioni.reserve(spec.GetMaxSezioni());
	for(int i=0;i<spec.GetMaxSezioni();i++){
		sezioni.push_back(head);//Creo e Azzero la struttura vector
	}
	RisulInt=NULL;
	fn.append('/');
	fn.append(name);
	lung=fn.size();
	char f[lung];
	for(int i=0;i<lung;i++) f[i]=fn.data()[i].toAscii();;
	f[lung]='\0';
	if (!fn.isEmpty()){
		fstream in_file(f, ios::in);
		if (!(in_file.is_open()) ) {
		    cout << "Error opening file: "<< f << endl;
		    return false;
		}
		else{
			cout << "File successfully open: "<< f << endl;
			while(!(in_file.eof ( ))){				
				in_file.read(&car,1);
				while((!(in_file.eof( )))&&((car==':')||((d>((ch*8)+3))&&(car==',')))){
					count=0;
					in_file.read(&car,1);
					while((!(in_file.eof ( )))&&(car!=',')&&(count<512)){
						str[count]=car;	
						in_file.read(&car,1);
						if ((d>((ch*8)+5))&&(car==':')) ok=true;
						count++;
					}
					str[count]='\0';
					switch(d) {
						case 0:{
							ch=strtod(str,NULL);
							if(!(ch<spec.GetMaxSezioni())) ch=spec.GetMaxSezioni();								
							channel=0;
						}
						break;
						case 1: beams=strtod(str,NULL);
						break;
						case 2: {
							sampleSize=strtod(str,NULL);
							if(sampleSize!=8){
								cout << "Data Error in file: "<< f << endl;
								return false;
							}
						}
						break;
						case 3: integration=strtod(str,NULL);
						break;
						case 5: {
							bins=strtod(str,NULL);		
							if(bins<MAXBINS){
								delete[] vett;
								vett=new double[((int)bins*4)+2];
							}
							else bins=MAXBINS;
							for(int j=0;j<(bins*4)+2;j++) vett[j]=0;						
							for(int i=0;i<ch;i++) {
								RisulInt=&RisultatiIntegrazione[i];
								RisulInt->SetZeroDataIntegratione();
								RisulInt->SetLungArrayDati((int)bins);
								RisulInt->SetTempoIntegrazione((int)integration);
								RisulInt->SetXxZero();
								RisulInt->SetYyZero();
								RisulInt->SetXyZero();
								RisulInt->SetTpZero();
								RisulInt->SetValoriValidi(false);
							}						
						}
						break;
						default: {
							if((d==(6+index*8))&&(d<((ch*8)+5))){
								head.SetZeroHeadResult();
								pola=strtod(str,NULL);
								if ((pola==0)||(pola==1)) head.SetModoPol(false);
								else if(pola==2) head.SetModoPol(true);
									else return false;
								head.Setindex(index);
								sezioni[index]=head;
								index++;
							}							
						}							
					}
					if (d>((ch*8)+4)){						
						val=strtod(str,NULL);						
						if((m<(bins*4)+2)&&(!ok)) vett[m]=val;
						m++;
						if(ok){//il +2 sono i tp
							if(channel<ch){
								RisulInt=&RisultatiIntegrazione[channel];
								if(sezioni[channel].GetModoPol()==false){
									RisulInt->SetHead(sezioni[channel]);
									RisulInt->SetXx(&vett[2]);
									RisulInt->SetTp(vett,2,true);
									RisulInt->SetValoriValidi(true);
									channel++;
									for(int j=0;j<(bins*4)+2;j++) vett[j]=0;	
									m=0;
									ok=false;
								}
								else {
							
									RisulInt->SetHead(sezioni[channel]);
									RisulInt->SetXx(&vett[2]);
									RisulInt->SetYy(&vett[2]);
									RisulInt->SetXy(&vett[2]);
									RisulInt->SetTp(vett,2,true);
									RisulInt->SetValoriValidi(true);
									channel++;
									for(int j=0;j<(bins*4)+2;j++) vett[j]=0;	
									m=0;
									ok=false;								
								}									
							}
						}
					}
					d++;
				}
			}
			if(channel<ch){
				RisulInt=&RisultatiIntegrazione[channel];
				if(sezioni[channel].GetModoPol()==false){
					RisulInt->SetHead(sezioni[channel]);
					RisulInt->SetXx(&vett[2]);
					RisulInt->SetTp(vett,2,true);
					RisulInt->SetValoriValidi(true);
					channel++;
				}
				else {
					RisulInt->SetHead(sezioni[channel]);
					RisulInt->SetXx(&vett[2]);
					RisulInt->SetYy(&vett[2]);
					RisulInt->SetXy(&vett[2]);
					RisulInt->SetTp(vett,2,true);
					RisulInt->SetValoriValidi(true);
					channel++;
				}
			}
		}
	}
	delete[] vett;
	p->Visual((int)ch);
	return true;
}

void xclientGUIui::UpdateStatus(char status)
{
	if (status&0x80) ledHW->setColor(Led::Green);
	else ledHW->setColor(Led::Red);
	if(status&0x08)ledData->setMode(Led::On);
	else ledData->setMode(Led::Off);
	if(status&0x40)ledSetting->setMode(Led::On);
	else ledSetting->setMode(Led::Off);
	if(status&0x20)ledActive->setMode(Led::On);
	else ledActive->setMode(Led::Off);	
	if(status&0x10)ledAbort->setMode(Led::On);
	else ledAbort->setMode(Led::Off);
	if(status&0x04)ledOverflowData->setMode(Led::On);
	else ledOverflowData->setMode(Led::Off);
	if(!(XThread.BUSY)) ledBusy->setMode(Led::On);
	else ledBusy->setMode(Led::Off);
	
	if((INT)&&((status&0x10) ||((status&0x08)&&(!(status&0x20))))){
		ui.pushButtonAbort->setEnabled(false);
		ui.pushButtonInt->setEnabled(true);
		ui.pushButtonStart->setEnabled(false);
		ui.Modo8Bit->setEnabled(true);
		ui.ModoPol->setEnabled(true);
		ui.bandBox->setEnabled(true);
		SetReadOnly(false);
		INT=false;
		
		if(status&0x08){
			AbortInt();
			OpenXplot();
		}
	}
}

void xclientGUIui::FileExit(){
	XThread.stop();
    if (XThread.isRunning() == true) {
		while (XThread.isFinished() == false)
			XThread.wait();
    }
    XThread.terminate();
	try {
		sender->terminate();
		sender->disconnect();
		recv->closeReceiver();
	}
	catch (...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::FileExit()");
		impl.log();
	}
	ACE_OS::sleep(1);	
	try {
		sc->manager()->release_component(sc->handle(),"Backends/XBackends");
		sc->manager()->release_component(sc->handle(),"MANAGEMENT/Writer1");
	}
	catch (maciErrType::CannotReleaseComponentExImpl& E) {
		E.log();
	}
	ACS_LOG(LM_FULL_INFO,"::FileExit()",(LM_INFO,"COMPONENTS_RELEASED"));

	exit( 0 );
}

void xclientGUIui::initAttributes ( ) {

	N=0;
	visualizza(N);
}

void xclientGUIui::Rivisualizza(int N){

	visualizza(N);
}

void xclientGUIui::Modifica( ){

	Salva(N);
	visualizza(N);
}

void xclientGUIui::Salva(int n){
	HeadResult temp;

	if (ui.Modo8Bit->isChecked()==true) spec.SetModo8bit(true);
	else spec.SetModo8bit(false);
	spec.SetTempoIntegrazione((int)((ui.intTimeLine->displayText().toDouble())*60));//Da minuti-> in secondi
	if(spec.GetTempoIntegrazione()<spec.tempoEle){
		spec.SetTempoIntegrazione(spec.tempoEle);			
	}
	else{
		spec.SetTempoIntegrazione((spec.GetTempoIntegrazione()/spec.tempoEle)*spec.tempoEle);
	}
	ui.intTimeLine->setText(QString::number((double)spec.GetTempoIntegrazione()/60, 'f', 3 ));
	spec.SetNCicli(ui.repeatBox->value());
	temp.SetZeroHeadResult();
	temp.SetFlo( ui.freqLine->displayText().toDouble() * 1e6);
	temp.SetGain(ui.gainBox->value());
	temp.SetAdc(ui.ChInBox->value());
	temp.SetBanda(ui.bandBox->currentText().toDouble() * 1e6);
	temp.Setindex(n);
	if (ui.ModoPol->isChecked()==true) temp.SetModoPol(true);
	else temp.SetModoPol(false);
	spec.specificaSezione[n]=temp;
}

void xclientGUIui::Broadcast(){
	HeadResult temp;
	int n=0;
	int scelta=0;
	
	scelta=QMessageBox::question(this,"MessageBox","Do you want setting all the Sections? ",QMessageBox::Yes|QMessageBox::No);
	if(scelta==QMessageBox::Yes){
		Salva(N);
		//groupSpec.setting=false;		
		temp=spec.specificaSezione[N];//Sezione al momento visualizzata
		for(n=0;n<spec.GetNSezioni();n++){
			temp.Setindex(n);
			spec.specificaSezione[n]=temp;
		}
	}
}

void xclientGUIui::Integra(){
	int s=0,fd=0;
	int pol;
	HeadResult temp;
	double bw=0,sr=0,att=0,flo=0;
	CString str("");
	
	Modifica();
	ui.pushButtonStart->setEnabled(true);
	visualizza(N);
	try{
		sender->setSectionsNumber(spec.GetNSezioni());
		sender->setInputsNumber(spec.GetNSezioni());
	}
	_CATCH_ALL("xclientGUIui::Integra()  setSection()");
	try{
		sender->setIntegration(spec.GetTempoIntegrazione());
	}
	_CATCH_ALL("xclientGUIui::Integra()  setIntegration()");
	s=0;
	while(s<spec.GetNSezioni()){
		temp=spec.specificaSezione[s];
		bw=temp.GetBanda()*1e-6;
		sr=(1/(temp.GetBanda()))*1e3;
		if(temp.GetGain()==0) att=50;
		else att=-20*log10(temp.GetGain()/MAX_GAIN);
		if(bw==125) flo=0;
		else flo=(temp.GetFlo()*1e-6+62.5-temp.GetBanda()*1e-6);		
		if (temp.GetModoPol()==true) pol=Backends::BKND_FULL_STOKES;
		else if(temp.GetAdc()%2==0) pol=Backends::BKND_LCP;
			else pol=Backends::BKND_RCP;
		fd=temp.GetAdc()/2;
//		if (temp.GetModoPol()==true) fd=temp.GetAdc()/2;
//		else fd=(((int)temp.GetAdc()/2)+pol);
		str.Concat("s=");
		str.Concat((int)s);		
		str.Concat("     flo=");
		str.Concat((double)flo);		
		str.Concat("     bw=");
		str.Concat((double)bw);
		str.Concat("    att=");
		str.Concat((double)att);
		str.Concat(" ChIn = ");
		str.Concat((int)temp.GetAdc());		
		str.Concat(" feed = ");
		str.Concat((int)fd);
		str.Concat("    Gain=");
		str.Concat((double)temp.GetGain());		
		str.Concat("     sr=");
		str.Concat((double)sr);	
		str.Concat("    pol=");
		str.Concat((int)pol);	
		str.Concat(" \n");
		if(bw==125) flo=0;

		try{		
			sender->setSection(s,flo,bw,fd,pol,sr,4096);
			//sender->setAttenuation();
			//Bisonga settare l'attenuazione!!!
			//mount->setChannel(s,flo,bw,sr,att,pol,4096);		
		}
		_CATCH_ALL("xclientGUIui::Integra()  setSection()");
	s++;
	}
	std::cout << str << std::endl;
	try{
		//mount->Init();
		sender->sendHeader();
	}
	_CATCH_ALL("xclientGUIui::Integra()  setSection()");
ACS_SHORT_LOG((LM_INFO, "xclientGUIui::Integra() %s","Backends/XBackends"));
}

void xclientGUIui::Successivo_activated(){
	int scelta=0;
	
	Modifica();//Effettua il Salvataggio Automatico di tutte le impostatazioni
	if (N<(spec.GetNSezioni()-1)) N++;
	else {
		if (N<(spec.GetMaxSezioni()-1)){
			scelta=QMessageBox::question(this,"MessageBox","Do you want create a new Section? ",QMessageBox::Yes|QMessageBox::No);
			if(scelta==QMessageBox::Yes){
				spec.SetNSezioni((spec.GetNSezioni()+1));
				spec.InsertElementArray();
				N++;
			}
		}
	}	
	visualizza(N);
}

void xclientGUIui::Precedente_activated(){

	Modifica();//Effettua il Salvataggio Automatico di tutte le impostatazioni
	if (N>0) N--;
	visualizza(N);
}

void xclientGUIui::visualizza(int n ){
	HeadResult temp;
	int i=0;
	double banda=0, tb=0;
	
	temp=spec.specificaSezione[n];
	if(temp.Getindex()!=-1){
		ui.freqLine->setText(QString::number((temp.GetFlo())* 1e-6, 'f', 6 ));
		ui.gainBox->setValue((int)temp.GetGain());
		if (temp.GetModoPol()==true) ui.ModoPol->setChecked(true);
		else ui.ModoPol->setChecked(false);
		ui.SezioneCorrente->setText(QString::number( n,'i',0 ));
		banda=temp.GetBanda( )* 1e-6;
		tb=ui.bandBox->itemText(i).toDouble();
		while(banda!=tb){
			i++;
			tb=ui.bandBox->itemText(i).toDouble();
			if (i>100) break;
		}
		if (spec.GetModo8bit()==true){
			ui.ChInBox->setMinimum(2);//setMaximum(5);
			ui.Modo8Bit->setChecked(true);
		}
		else{
			ui.ChInBox->setMinimum(0);//setMaximum(7);
			ui.Modo8Bit->setChecked(false);
		}
		ui.bandBox->setCurrentIndex(i);
		ui.intTimeLine->setText(QString::number((double)spec.GetTempoIntegrazione()/60, 'f', 3 ));
	    ui.repeatBox->setValue(spec.GetNCicli());
	    ui.ChInBox->setValue(temp.GetAdc());
	}
}

void xclientGUIui::Check(){
	int scelta=0;

	scelta=QMessageBox::question(this,"MessageBox","Do you want modify the quantization? ",QMessageBox::Yes|QMessageBox::No);
	if(scelta==QMessageBox::Yes){
		spec.ResetArray();
		if(ui.Modo8Bit->isChecked()==true){
			ui.ChInBox->setMinimum(2);//setMaximum(5);
			spec.SetModo8bit(true);
			//groupSpec.AdcSetMode(spec.GetModo8bilt());
			ui.Modo8Bit->setChecked(true);
		}
		else{
			ui.ChInBox->setMinimum(0);//setMaximum(7);
			spec.SetModo8bit(false);
			//groupSpec.AdcSetMode(pec.GetModo8bilt());						
			ui.Modo8Bit->setChecked(false);
		}
	}else{
		if(ui.Modo8Bit->isChecked()!=true){
			ui.ChInBox->setMinimum(2);//setMaximum(5);
			spec.SetModo8bit(true);
			ui.Modo8Bit->setChecked(true);
		}
		else{
			ui.ChInBox->setMinimum(0);//setMaximum(7);
			spec.SetModo8bit(false);
			ui.Modo8Bit->setChecked(false);
		}
	}
	visualizza(N);
}

void xclientGUIui::AbortInt(){
	
	//groupSpec.AbortInt();
	ui.pushButtonAbort->setEnabled(false);
	ui.pushButtonInt->setEnabled(true);
	ui.pushButtonStart->setEnabled(false);
	ui.Modo8Bit->setEnabled(true);
	ui.ModoPol->setEnabled(true);
	ui.bandBox->setEnabled(true);
	SetReadOnly(false);

	try{
		//sender->Abort();
		sender->sendStop();
	}
	_CATCH_ALL("xclientGUIui::AbortInt()  StopSender()");
	INT=false;
}

void xclientGUIui::StartInt(){
	
	IRA::CString fileName("");
	TIMEVALUE now;	
	CString nomeFile("XWriter");
	CString backend("Xarcos");//("");
	CString path("/home/Manager/OutXarcos");//("");
	CString observer("dummy");
	
	ui.pushButtonAbort->setEnabled(true);
	ui.pushButtonInt->setEnabled(false);
	ui.pushButtonStart->setEnabled(false);
	ui.Modo8Bit->setEnabled(false);
	ui.ModoPol->setEnabled(false);
	ui.bandBox->setEnabled(false);
	SetReadOnly(true);

	IRA::CIRATools::getTime(now);			
	fileName.Format("%s/%s_%02d_%02d_%02d.fits",(const char *)path,(const char *)nomeFile,now.hour(),now.minute(),now.second());

	try{
		recv->setFileName((const char*)fileName);
		recv->setProjectName("PrototypeProject");
		recv->setObserverName((const char *)observer);
		recv->setReceiver("KKC");	
		ACS_DEBUG("xclientGUIui::StartInt"," setWriter");

	}
	_CATCH_ALL("xclientGUIui::StartInt()  setWriter()");	
	try{
		//sender->Start();
		sender->sendData(now.value().value);	
		ACS_DEBUG("xclientGUIui::StartInt"," SenderData()");

	}
	_CATCH_ALL("xclientGUIui::StartInt()  SenderData()");
	INT=true;
}

void xclientGUIui::SetReadOnly(bool modo){

	ui.freqLine->setReadOnly(modo);
	ui.gainBox->setReadOnly(modo);
	ui.ChInBox->setReadOnly(modo);
	ui.intTimeLine->setReadOnly(modo);
	ui.repeatBox->setReadOnly(modo);
}

void xclientGUIui::changeGUICatchAllErrorCode()
{	int scelta=0;

scelta=QMessageBox::question(this,"MessageBox",QCatchAllError,QMessageBox::Yes|QMessageBox::No);

}
