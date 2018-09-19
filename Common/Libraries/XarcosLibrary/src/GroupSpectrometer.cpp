#include "../include/GroupSpectrometer.h"
#include "../include/Specifiche.h"
#include "../include/SingleSpectrometer.h"
#include "../include/DataIntegrazione.h"
#include "../include/Cpld2IntfClient.h"
#include "../include/AdcConditioner.h"

//#define DOPPIO

GroupSpectrometer::GroupSpectrometer(const char * a_server, int a_port, bool a_doppio)
{	
	doppio=a_doppio;	
	setting=false;//OK
	active=false;//OK
	abort=false;//OK
	dataReady=false;//NO!!**********************
	overflowData=false;//OK
	hw=initAttributes (a_server, a_port);//OK
	section=0;
	input=0;
}

GroupSpectrometer::~GroupSpectrometer() 
{
	SingleSpectrometer *singlespec;
		AdcConditioner *adc;
		int i=0;
		
		for (i=0;i<numSpec;i++){
			singlespec=Spettrometro[i];
			delete singlespec;
		}
		for (i=0;i<numSpec/2;i++){
			adc=Adc[i];
			delete adc;
		}
		if (intf!=NULL) delete intf;
	
} //Distruttore della classe 

char GroupSpectrometer::getBackendStatus() 
{
	status=0x0;
    if (hw) status|=0x01;
    if (setting) status|=0x02;
    if (active) status|=0x04;
    if (abort) status|=0x08;
    if (dataReady) status|=0x10;
    if (overflowData) status|=0x20;
	
	return status;
}

bool GroupSpectrometer::initAttributes (const char * a_server, int a_port) 
{
	DataIntegrazione RisulInt;
	SingleSpectrometer *singlespec;
	AdcConditioner *adc;
	Data tmpD;
	int i=0;
	
	countTp=countInt=0;
	numSpec=8;
	numPuntiSpettro=4096;

if(doppio){//#ifdef DOPPIO
	numSpec=16;
	numPuntiSpettro=2048;
}//#endif
	tempoEle=10;//secondi
	tmpD.SetZeroData();
	dato.reserve(numSpec);
	for(i=0;i<numSpec;i++){
		dato.push_back(tmpD);//Creo e Azzero la struttura vector
	}
	RisulInt.SetZeroDataIntegratione();
	RisultatiIntegrazione.reserve(Xspec.GetMaxSezioni());
	for(i=0;i<Xspec.GetMaxSezioni();i++){
		RisultatiIntegrazione.push_back(RisulInt);//Creo e Azzero la struttura vector
	}
	intf =NULL;
	intf = new Cpld2IntfClient(a_server, a_port );//Instauro la connessione TCP con HW
	if(intf ==NULL) return false;
	
	Spettrometro.reserve(numSpec);//Creo e Azzero la struttura vector
	
	singlespec= new SingleSpectrometer(intf,0x00010,0x00210,0x00220,numPuntiSpettro);//Spettrometri del Gurppo ZERO
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x00020,0x00240,0x00280,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x00110,0x00310,0x00320,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x00120,0x00340,0x00380,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x00410,0x00610,0x00620,numPuntiSpettro);//Spettrometri del Gurppo UNO
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x00420,0x00640,0x00680,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x00510,0x00710,0x00720,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x00520,0x00740,0x00780,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
if(doppio){//#ifdef DOPPIO
	singlespec= new SingleSpectrometer(intf,0x02010,0x02210,0x02220,numPuntiSpettro);//Spettrometri del Gurppo ZERO
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x02020,0x02240,0x02280,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x02110,0x02310,0x02320,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x02120,0x02340,0x02380,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x02410,0x02610,0x02620,numPuntiSpettro);//Spettrometri del Gurppo UNO
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x02420,0x02640,0x02680,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x02510,0x02710,0x02720,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
	singlespec= new SingleSpectrometer(intf,0x02520,0x02740,0x02780,numPuntiSpettro);
	Spettrometro.push_back(singlespec);
}//#endif
	Adc.reserve(numSpec/2);//Creo e Azzero la struttura vector
	adc= new  AdcConditioner(intf,0x1000);//ADC0,ADC1
	Adc.push_back(adc);
	adc= new  AdcConditioner(intf,0x1100);//ADC2,ADC3
	Adc.push_back(adc);
	adc= new  AdcConditioner(intf,0x1200);//ADC4,ADC5
	Adc.push_back(adc);
	adc= new  AdcConditioner(intf,0x1300);//ADC6,ADC7
	Adc.push_back(adc);
if(doppio){//#ifdef DOPPIO
	adc= new  AdcConditioner(intf,0x3000);//ADC0,ADC1
	Adc.push_back(adc);
	adc= new  AdcConditioner(intf,0x3100);//ADC2,ADC3
	Adc.push_back(adc);
	adc= new  AdcConditioner(intf,0x3200);//ADC4,ADC5
	Adc.push_back(adc);
	adc= new  AdcConditioner(intf,0x3300);//ADC6,ADC7
	Adc.push_back(adc);
}//#endif
	AdcSetMode(Xspec.GetModo8bit());//Setto il modo a 8 Bit

	enabled.resize(numSpec*4);
	for (int e=0;e<numSpec*4;e++) enabled[e]=false;
	//AdcReset(false);//Resetto il circuito di totalPower è disabilitato
	return true;
}

bool GroupSpectrometer::Setting()//Prepara le specifiche per l'HW
{
	HeadResult tmpD, temp, tp;
	
	int M=4;
if(doppio){//#ifdef DOPPIO	
	M=8;
}//#endif
	int i=0, s=0,j=0,k=0;
	bool scambio=false;
	vector<HeadResult> spec;

	spec.reserve(Xspec.GetNSezioni());
	for(i=0;i<Xspec.GetNSezioni();i++){
		spec.push_back(Xspec.specificaSezione[i]);//Duplico il vettore delle specifiche
		
	}

	do{//******************************Ordiniamo secondo il ChIn di ingresso
		scambio=false;
		for(i=0;i<(Xspec.GetNSezioni())-1;i++){
			temp=spec[i];
			tp=spec[i+1];
			if(temp.GetAdc()>tp.GetAdc()){
				scambio=true;
				tmpD=spec[i];
				spec[i]=spec[i+1];
				spec[i+1]=tmpD;
			}
		}
	}while(scambio);

	do{//******************************Ordiniamo secondo il ChIn e il Modo Polarimetrico
		scambio=false;
		i=0;
		temp=spec[i];
		while(i<(Xspec.GetNSezioni()-1)){
            //printf("1 modoPol = %d\n", temp.GetModoPol());
			if(temp.GetModoPol()==true);
			else{
				tp=spec[i+1];
              //  printf("2 modoPol = %d\n", tp.GetModoPol());
				if(tp.GetModoPol()==true){
					if(temp.GetAdc()==tp.GetAdc()){
						scambio=true;
						tmpD=spec[i];
						spec[i]=spec[i+1];
						spec[i+1]=tmpD;
					}
				}
			}
			i++;
			temp=spec[i];
		}
	}while(scambio);
		//M=8 Genero la struttura POL 0-7 NonPOL 0-15 POL8-15
	do{//M=4 Genero la struttura POL 0-3 NonPOL 0-7 POL4-7
		scambio=false;
		i=0;
		temp=spec[i];
		while(i<(Xspec.GetNSezioni()-1)){
            //printf("3 modoPol = %d\n", temp.GetModoPol());
			if((temp.GetAdc()<M)&&(temp.GetModoPol()));
			else{
				tp=spec[i+1];
              //  printf("4 modoPol = %d\n", tp.GetModoPol());
				if((temp.GetAdc()<M)&&(tp.GetAdc()<M)&&(tp.GetModoPol())){
					scambio=true;
					tmpD=spec[i];
					spec[i]=spec[i+1];
					spec[i+1]=tmpD;
				}
			}
            //printf("5 modoPol = %d\n", temp.GetModoPol());
			if((temp.GetAdc()>=M)&&(!(temp.GetModoPol())));
			else{
				tp=spec[i+1];
              //  printf("6 modoPol = %d\n", tp.GetModoPol());
				if((temp.GetAdc()>=M)&&(tp.GetAdc()>=M)&&(!(tp.GetModoPol()))){
					scambio=true;
					tmpD=spec[i];
					spec[i]=spec[i+1];
					spec[i+1]=tmpD;
				}
			}
			i++;
			temp=spec[i];
		}
	}while(scambio);

	i=0;//Ordino i non polarimetric per banda
	scambio=false;
    // inserito un ulteriore controllo (i < Xspec.GetNSezioni())
    // per evitare un segmentation fault della GetModoPol
	//while((spec[i].GetModoPol()) && (i < 7))	{
	/*while((spec[i].GetModoPol()) && (i < Xspec.GetNSezioni()))	{
        //printf("7 modoPol = %d\n", spec[i].GetModoPol());
        i++;
        // inserito un ulteriore controllo (i==Xspec.GetNSezioni())
        // per evitare un segmentation fault della GetModoPol
        printf("nsez = %d\n", Xspec.GetNSezioni());
        if (i==Xspec.GetNSezioni()) {
            printf("break\n");
            break;
        }
    }*/
        i=Xspec.GetNSezioni()-1;
	while((!(spec[i].GetModoPol()))&&(i<Xspec.GetNSezioni())){
		j=i;
		temp=spec[i];
		do{
			j++;
			tp=spec[j];
		}while((!(spec[j].GetModoPol()))&&(j<(Xspec.GetNSezioni()-1))&&(temp.GetBanda()!=tp.GetBanda()));
		if(spec[j].GetModoPol()) break;
		i++;
		if((temp.GetBanda()==tp.GetBanda())&&(((temp.GetAdc()<4)&&(tp.GetAdc()<4))||((temp.GetAdc()>=4)&&(tp.GetAdc()>=4)))){
			k=i;
			temp=spec[j];
			while(k<j){
				spec[k+1]=spec[k];
				k++;
			}
			spec[i]=temp;
			i++;
		}
		temp=spec[j];
	}
//	for(i=0;i<Xspec.GetNSezioni();i++){//vediamo se la struttura è riordinata
//		dato[i].SetIntestazioneCh1(spec[i]);
//	}
	for(i=0;i<numSpec;i++){//Azzero la struttura
		dato[i].SetZeroData();
	}
	s=j=0;
	if (Xspec.GetModo8bit()==true){
		Imposta(s,j,2,spec);//Settiamo il gruppo Zero ADC2-ADC5
	}else{
		Imposta(s,j,0,spec);//Settiamo il gruppo Zero ADC0-ADC3
	}
	Imposta(s,j,4,spec);//Settiamo il gruppo uno ADC4-ADC7
if(doppio){//#ifdef DOPPIO	
	if (Xspec.GetModo8bit()==true){
		Imposta(s,j,10,spec);//Settiamo il gruppo Zero ADC10-ADC13
	}else{
		Imposta(s,j,8,spec);//Settiamo il gruppo Zero ADC8-ADC11
	}
	Imposta(s,j,12,spec);//Settiamo il gruppo uno ADC12-ADC15
}//#endif	
return true;
}

//*************************************************Setta il gruppo ADC
bool GroupSpectrometer::Imposta(// FUNZIONE CORRELATA ALLA PROCEDURA DI Setting
		int &s, 	// Indice primo spettrometro libero
		int &j, 	// Indice prima intestazione da considerare
		int M, 		// Primo ADC del gruppo
		vector<HeadResult> spec)	// Elenco specifiche ordinate
{
	HeadResult temp;
	Data tmpD;
	int t=4;

	if (M==4){
		s=4;
		t=8;
	}	
	if((M==10)||(M==8)){
		s=8;
		t=12;
	}
	if (M==12){
		s=12;
		t=16;
	}
	while((j<(Xspec.GetNSezioni()))&&(s<t)){
		while((j<(Xspec.GetNSezioni()))&&((spec[j].GetAdc())>=M)&&((spec[j].GetAdc())<(M+4))&&(s<t)){
			temp.SetZeroHeadResult();
			temp=spec[j];
			if(temp.GetModoPol()==true){//MODO POLARIMTERICO
				tmpD.SetIntestazioneCh1(temp);
				if((temp.GetAdc())==(0+M)) temp.SetAdc((M+1));
				else if((temp.GetAdc())==(1+M)) temp.SetAdc((M+0));
					else if((temp.GetAdc())==(2+M)) temp.SetAdc((M+3));
						else if((temp.GetAdc())==(3+M)) temp.SetAdc((M+2));
				tmpD.SetIntestazioneCh2(temp);
				j++;
			}else{//MODO NON POLARIMETRICO
				tmpD.SetIntestazioneCh1(temp);
				j++; // Confronta specifica j e j+1
				if( ( ((temp.GetAdc()<4)&&(spec[j].GetAdc()<4)) // Nello stesso gruppo
						|| ((temp.GetAdc()>=4)&&(spec[j].GetAdc()>=4)) )
// nota: da correggere, meglio controllare se specj.adc e' compreso tra M e M + 3
//
						&& ((spec[j].GetModoPol()==false) // nessuno polarimetrico
							&&(temp.GetBanda()==spec[j].GetBanda()) // stessa banda
							&&(j<Xspec.GetNSezioni())) )
					{ // accorpabili, mette nel Ch2 la seconda intestazione
						temp=spec[j];
						tmpD.SetIntestazioneCh2(temp);
						j++;
				}else{ // azzera seconda intestazione
					double banda=temp.GetBanda();
					temp.SetZeroHeadResult();
					temp.SetBanda(banda);//Setto le banda uguali********************
					tmpD.SetIntestazioneCh2(temp);
				}
				// Aggiusta gli ADC considerando il primo ADC del gruppo
				//
			}
			temp=tmpD.GetIntestazioneCh1();
			temp.SetAdc((temp.GetAdc())-M);
			tmpD.SetIntestazioneCh1(temp);
			temp=tmpD.GetIntestazioneCh2();
			temp.SetAdc((temp.GetAdc())-M);
			tmpD.SetIntestazioneCh2(temp);
			dato[s]=tmpD;
			s++;
		}
		if(!(j<(Xspec.GetNSezioni()))) break; //esco la While
		if((spec[j].GetAdc()>=0)&&(spec[j].GetAdc())<(M)) j++;
		if(!(spec[j].GetAdc()<(M+4))) break; //esco la While
	}
return true;
}

void GroupSpectrometer::Init()//Inizializza le specifiche correttamente settare nell'HW
{
	SingleSpectrometer *single;
	Data *tmpD;
	float nCicli;
	int indice;
	
	for (int e=0;e<numSpec*4;e++) enabled[e]=false;
	section=0;
	input=0;
	for (int n=0;n<numSpec;n++){
		tmpD=&dato[n];
		single=Spettrometro[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();
		if(indice!=-1){	
			section++;
			input++;
			enabled[indice*2]=true;
			if(tmpD->GetIntestazioneCh1().GetModoPol()==false) {
				indice=tmpD->GetIntestazioneCh2().Getindex();
				if(indice!=-1){
					section++;
					input++;
					enabled[indice*2+1]=true;
				}
			}else{
				input++;
				enabled[indice*2+1]=true;
			}
			single->setTempoIntElementare(tempoEle);
			single->setModo8Bit(Xspec.GetModo8bit());
			single->Init(single,tmpD);
		}
	}
	countInt=countTp=0;
	nCicli=Xspec.GetTempoIntegrazione()/tempoEle;
	if(nCicli<1) nCicli=1;
	Xspec.SetNCicli((int)nCicli);
	//AdcIntTime(tempoEle);
	//AdcReset(true);//abilito il circuito di totalPower
}



bool GroupSpectrometer::GetData(//Recupera i risultati parziali di un integrazione elementare
		int nCicli )//Contatore decrescente del flusso dei dati 
{
	DataIntegrazione *RisulInt;
	SingleSpectrometer *single;
	Data *tmpD;
    int n=0,indice=-1;


   	for (n=0;n<numSpec;n++){
		tmpD=&dato[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();//Qui si dovrebbe sostituire un controllo di stato
			if(indice!=-1){
				single=Spettrometro[n];
				single->GetData(single,tmpD);
			}
	}
	for (n=0;n<numSpec;n++){
		tmpD=&dato[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();
		if(indice!=-1){
			if(tmpD->GetIntestazioneCh1().GetModoPol()==true){
				RisulInt=&RisultatiIntegrazione[indice];
				RisulInt->SetXx(tmpD->GetBuffer());
				RisulInt->SetYy(tmpD->GetBuffer());
				RisulInt->SetXy(tmpD->GetBuffer());

				//RisulInt->SetTp(tmpD->GetBuffer(),tmpD->GetSizeBuffer(),true);
				if(nCicli==0){
					if (tmpD->GetOverflow()==true) overflowData=true;
					RisulInt->SetOverflow(tmpD->GetOverflow());
					RisulInt->SetUtFine();
					RisulInt->SetValoriValidi(true);
				}
			}else{
				RisulInt=&RisultatiIntegrazione[indice];
				RisulInt->SetXx(tmpD->GetBuffer());

				//RisulInt->SetTp(tmpD->GetBuffer(),tmpD->GetSizeBuffer(),false);
				if(nCicli==0){
					if (tmpD->GetOverflow()==true) overflowData=true;
					RisulInt->SetOverflow(tmpD->GetOverflow());
					RisulInt->SetUtFine();
					RisulInt->SetValoriValidi(true);
				}
				indice=tmpD->GetIntestazioneCh2().Getindex();
				if(indice!=-1){
					RisulInt=&RisultatiIntegrazione[indice];
					RisulInt->SetXx2(tmpD->GetBuffer());
					//RisulInt->SetTp(tmpD->GetBuffer(),(tmpD->GetSizeBuffer())+1,false);
					if(nCicli==0){
						if (tmpD->GetOverflow()==true) overflowData=true;
						RisulInt->SetOverflow(tmpD->GetOverflow());
						RisulInt->SetUtFine();
						RisulInt->SetValoriValidi(true);
					}
				}
			}
		}
	}
	return true;
}

//bool GroupSpectrometer::GetDataTpZero(bool modo)
//{	
//	HeadResult temp;
//	vector<HeadResult> spec;
//	int i=0;
////	int NCicli=Xspec.GetNCicli();
//	if(!active){//se non sto integrando
//		spec.reserve(Xspec.GetNSezioni());
//		if(modo){//Gain=0;
//			for(i=0;i<Xspec.GetNSezioni();i++){
//				spec.push_back(Xspec.specificaSezione[i]);//Duplico il vettore delle specifiche
//				temp=Xspec.specificaSezione[i];
//				temp.SetGain(0);//Metto a zero
//				Xspec.specificaSezione[i]=temp;
//			}
////			Xspec.SetNCicli(1);
//			StartInt();//
//			//GetDataTp(true);//Faccio partire l'integrazione tp
//			usleep(200000);//Aspetto i 200 milli secondi
//			GetDataTp(false);//Recupero l'integrazione tp
//			
//			for(i=0;i<Xspec.GetNSezioni();i++){
//				Xspec.specificaSezione[i]=spec[i];//Rimposto il vettore delle specifiche
//			}
//		}
//		else{
//			Xspec.SetNCicli(1);
//			StartInt();//
//			//GetDataTp(true);//Faccio partire l'integrazione tp
//			usleep(200000);//Aspetto i 200 milli secondi
//			GetDataTp(false);//Recupero l'integrazione tp
//		}
////		Xspec.SetNCicli(NCicli);
//		return true;
//	}
//	else {//se integro!!
//		if(modo) return false;
//		else{
//			//Da fare!!
//		}
//	}
//}

bool GroupSpectrometer::GetDataTpZero(bool modo, vector<double> &tp)
{
	HeadResult temp;
	vector<HeadResult> spec;
	int i=0;
	
	if(!active){//se non sto integrando
		spec.reserve(Xspec.GetNSezioni());
		if(modo){//Gain=0;
			for(i=0;i<Xspec.GetNSezioni();i++){
				spec.push_back(Xspec.specificaSezione[i]);//Duplico il vettore delle specifiche
				temp=Xspec.specificaSezione[i];
				temp.SetGain(0);//Metto a zero
				Xspec.specificaSezione[i]=temp;
			}
			Setting();Init();
			GetDataTp(true,tp);//Faccio partire l'integrazione tp
			usleep(200000);//Aspetto i 200 milli secondi
			GetDataTp(false,tp);//Recupero l'integrazione tp
			for(i=0;i<Xspec.GetNSezioni();i++){
				Xspec.specificaSezione[i]=spec[i];//Rimposto il vettore delle specifiche
			}
			Setting();Init();
		}
		else{
			Setting();Init();
			GetDataTp(true,tp);//Faccio partire l'integrazione tp
			usleep(200000);//Aspetto i 200 milli secondi
			GetDataTp(false,tp);//Recupero l'integrazione tp
		}
	}
	else {//se integro!!
		if(modo) return false;
		else{
			GetDataTp(false,tp);//Recupero l'integrazione tp senza aspettare
		}
	}
	return true;
}

bool GroupSpectrometer::GetDataTp(//Recupera i risultati parziali di un misura elementare (200 millisecond)
		bool modo,  vector<double> &tp)//modo=false nel recupero delle misure Tp
{
	int c=0,n=0,max=0,indice=-1;
	Data *tmpD;
	SingleSpectrometer *single;
	
  	for (n=0;n<numSpec;n++){
		tmpD=&dato[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();//Qui si dovrebbe sostituire un controllo di stato
		if(indice!=-1){
			if(indice>max) max=indice;
			single=Spettrometro[n];
			single->GetDataTp(single,tmpD,modo);//modo=false
		}
	}	
	for (n=0;n<numSpec;n++){
		tmpD=&dato[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();
		if(indice!=-1){
			if(tmpD->GetIntestazioneCh1().GetModoPol()==true){
				for(c=0;c<2;c++) tp[indice*2+c]+=(double)(tmpD->GetBuffer())[c+tmpD->GetSizeBuffer()-2];
			}
			else{
				tp[indice*2]+=(tmpD->GetBuffer())[tmpD->GetSizeBuffer()-2];
				tp[indice*2+1]+=(tmpD->GetBuffer())[tmpD->GetSizeBuffer()-1];
				indice=tmpD->GetIntestazioneCh2().Getindex();
				if(indice!=-1){
					tp[indice*2]+=(tmpD->GetBuffer())[tmpD->GetSizeBuffer()-2];
					tp[indice*2+1]+=(tmpD->GetBuffer())[tmpD->GetSizeBuffer()-1];
				}
			}
		}
	}
	return true;
}


bool GroupSpectrometer::GetDataTp(//Recupera i risultati parziali di un misura elementare (200 millisecond)
		bool modo)//modo=false nel recupero delle misure Tp
{	
	DataIntegrazione *RisulInt;
	SingleSpectrometer *single;
	Data *tmpD;
    int n=0,indice=-1;

   	for (n=0;n<numSpec;n++){
		tmpD=&dato[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();//Qui si dovrebbe sostituire un controllo di stato
			if(indice!=-1){
				single=Spettrometro[n];
				single->GetDataTp(single,tmpD,modo);//modo=false
			}
	}
	for (n=0;n<numSpec;n++){
		tmpD=&dato[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();
		if(indice!=-1){
			if(tmpD->GetIntestazioneCh1().GetModoPol()==true){
				RisulInt=&RisultatiIntegrazione[indice];
				RisulInt->SetTp(tmpD->GetBuffer(),tmpD->GetSizeBuffer(),true);
			}else{
				RisulInt=&RisultatiIntegrazione[indice];
				RisulInt->SetTp(tmpD->GetBuffer(),tmpD->GetSizeBuffer(),false);
				indice=tmpD->GetIntestazioneCh2().Getindex();
				if(indice!=-1){
					RisulInt=&RisultatiIntegrazione[indice];
					RisulInt->SetTp(tmpD->GetBuffer(),(tmpD->GetSizeBuffer())+1,false);
				}
			}
		}
	}
	return true;
}

bool GroupSpectrometer::AbortDataTp()//Recupera i risultati parziali di un misura elementare (200 millisecond)
{	
	DataIntegrazione *RisulInt;
	Data *tmpD;
    int n=0,indice=-1;
    float norm=(float)(countInt*50)/countTp;
    
	for (n=0;n<numSpec;n++){
		tmpD=&dato[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();
		if(indice!=-1){
			if(tmpD->GetIntestazioneCh1().GetModoPol()==true){
				RisulInt=&RisultatiIntegrazione[indice];
				RisulInt->tp[0]=RisulInt->tp[0]*norm;
				RisulInt->tp[1]=RisulInt->tp[1]*norm;
			}else{
				RisulInt=&RisultatiIntegrazione[indice];
				RisulInt->tp[0]=RisulInt->tp[0]*norm;
				indice=tmpD->GetIntestazioneCh2().Getindex();
				if(indice!=-1){
					RisulInt=&RisultatiIntegrazione[indice];
					RisulInt->tp[1]=RisulInt->tp[1]*norm;
				}
			}
		}
	}
	return true;
	
}

void GroupSpectrometer::StartInt()//Avvia l'integrazione secondo le specifiche
{
	DataIntegrazione *RisulInt;
	Data *tmpD;
	SingleSpectrometer *single;
	int n=0,indice=-1;

	if (Xspec.GetNCicli()==0) return;
	for (n=0;n<numSpec;n++){
		tmpD=&dato[n];
		single=Spettrometro[n];
		indice=tmpD->GetIntestazioneCh1().Getindex();
		if(indice!=-1){
			single->GetFFT()->Start();
			if(tmpD->GetIntestazioneCh1().GetModoPol()==true){
				RisulInt=&RisultatiIntegrazione[indice];
				RisulInt->SetHead(tmpD->GetIntestazioneCh1());
				RisulInt->SetModo8Bit(true);
				RisulInt->SetNArrayDati(3);
				RisulInt->SetLungArrayDati(numPuntiSpettro);
				RisulInt->SetTempoIntegrazione(Xspec.GetTempoIntegrazione());
				RisulInt->SetUtInizio();
				RisulInt->SetXxZero();
				RisulInt->SetYyZero();
				RisulInt->SetXyZero();
				RisulInt->SetTpZero();
				RisulInt->SetValoriValidi(false);
			}else{
				RisulInt=&RisultatiIntegrazione[indice];
				RisulInt->SetHead(tmpD->GetIntestazioneCh1());
				RisulInt->SetModo8Bit(false);
				RisulInt->SetNArrayDati(1);
				RisulInt->SetLungArrayDati(numPuntiSpettro);
				RisulInt->SetTempoIntegrazione(Xspec.GetTempoIntegrazione());
				RisulInt->SetUtInizio();
				RisulInt->SetXxZero();
				RisulInt->SetTpZero();
				RisulInt->SetValoriValidi(false);
				indice=tmpD->GetIntestazioneCh2().Getindex();
				if(indice!=-1){
					RisulInt=&RisultatiIntegrazione[indice];
					RisulInt->SetHead(tmpD->GetIntestazioneCh2());
					RisulInt->SetModo8Bit(false);
					RisulInt->SetNArrayDati(1);
					RisulInt->SetLungArrayDati(numPuntiSpettro);
					RisulInt->SetTempoIntegrazione(Xspec.GetTempoIntegrazione());
					RisulInt->SetUtInizio();
					RisulInt->SetXxZero();
					RisulInt->SetTpZero();
					RisulInt->SetValoriValidi(false);
				}
			}

		}
	}
	GetDataTp(true);//Parte la misurazione Tp
	return;
}

void GroupSpectrometer::AbortInt()//Abortice l'integrazione in corso
{
	int n=0;
	SingleSpectrometer *single;

	for (n=0;n<numSpec;n++){
		single=Spettrometro[n];
		single->GetFFT()->Stop();
	}
}

void GroupSpectrometer::AdcIntTime(
		int ms)//specifica il tempo di integrazione nell' ACD
{//NON VIENE USATO NEL PROGRAMMA
	AdcConditioner *AdcTmp;

	for(int a=0;a<numSpec/2;a++){
		AdcTmp=Adc[a];
		AdcTmp->IntTime(ms);
		AdcTmp->Set();
	}
}

void GroupSpectrometer::AdcReset(
		bool reset)//reset=true abilito il circuito di totalPower
{//NON VIENE USATO NEL PROGRAMMA
	AdcConditioner *AdcTmp;

	for(int a=0;a<numSpec/2;a++){
		AdcTmp=Adc[a];
		AdcTmp->ResetTotPow(reset);
		//AdcTmp->Set();
	}

}

void GroupSpectrometer::AdcSetMode(bool modo)//modo=true Setto l'ADC al funzionamento a 8 Bit
				  //modo=false Setto l'ADC al funzionamento a 6 Bit
{
	AdcConditioner *AdcTmp;
	int distr[]={0x20,0x60,0xa0,0xe0,
                     0x20,0x60,0xa0,0xe0 };//Configurazione per la Distribuzione dei Bit su backplane

	int func=Xspec.GetFunctionAdc();
	if(func==5) func=0x0a;

	for(int a=0;a<numSpec/2;a++){
		AdcTmp=Adc[a];
		AdcTmp->SetMode(func,false,!modo);//non Tolto !
		if (modo) AdcTmp->Distribution(0);
		else AdcTmp->Distribution(distr[a]);
		AdcTmp->Set();
	}
}

void GroupSpectrometer::AdcSetFunction(
		int func)//Setto le funzionalità di test dell'ADC 
{//QUESTA FUNZIONE é PREVISTA SOLO PER L'IMPLEMENTAZINE Xarcos
	AdcConditioner *AdcTmp;

	if(func==5) func=0x0a;
	for(int a=0;a<numSpec/2;a++){
		AdcTmp=Adc[a];
		if(func==1) AdcTmp->Freq(60.e6);//Frequenza della sinusoide generata dall'ADC 
		AdcTmp->SetMode(func,false,!Xspec.GetModo8bit());//non Tolto !
		AdcTmp->Set();
	}
}

bool GroupSpectrometer::AdcGetData()
{//NON VIENE USATO NEL PROGRAMMA
	AdcConditioner *AdcTmp;
	//long xl, yl, xylr, xyli;
	double xx, yy;
	complex<double> xy;

	for(int a=0;a<numSpec/2;a++){
		AdcTmp=Adc[a];
		//AdcTmp->ReadTotPow(xl, yl, xylr, xyli);
		AdcTmp->ReadTotPow(xx, yy, xy);
	}
return true;
}

bool GroupSpectrometer::isConnected()//Controlla se la connessione è attiva
{//QUESTA FUNZIONE é PREVISTA PER L'IMPLEMENTAZINE XBackends
	
	if(intf==NULL) return false;
	return intf-> IsOpen();
}

bool GroupSpectrometer::reConnect(){//DA PROVARE ************************
	if(intf==NULL) return false;
	if(intf->Open()==0) return true;	
	return false;
}


