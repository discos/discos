#include "../include/SingleSpectrometer.h"

SingleSpectrometer::SingleSpectrometer(//Costruttore della classe Generico
		Cpld2Intf *intf)//Puntatore all'oggetto che gestisce il protocollo TCP con l'HW
{
	initAttributes(intf);
}

SingleSpectrometer::SingleSpectrometer(//Costruttore della classe 
		Cpld2Intf *intf,//Puntatore all'oggetto che gestisce il protocollo TCP con l'HW
		int addrFFT, //Indirizzo Fisico dell'elemeto FFT
		int addrBBC1, //Indirizzo Fisico dell'elemeto BBC1 relatito all FFT
		int addrBBC2, //Indirizzo Fisico dell'elemeto BBC2 relatito all FFT
		int PuntiSpettro)
{
	fft = new Spectrometer(intf,addrFFT);
	bbc1= new BbcFilter(intf, addrBBC1);         // Filter1 (on chip 0x3-2)
	bbc2= new BbcFilter(intf, addrBBC2);         // Filter2 (on chip 0x3-2)
	setOFF(PuntiSpettro);
	fft->Bbc(bbc1);

}
SingleSpectrometer::~SingleSpectrometer ( )
{
	if(fft!=NULL) delete fft;
	if(bbc1!=NULL) delete bbc1;
	if(bbc2!=NULL) delete bbc2;
	
}//Distruttore della classe

void SingleSpectrometer::GetDataTp(//Gestisce il recupero delle misure di Tp
		SingleSpectrometer *single, //Puntatore al Spettrometro
		Data *tmpD, //Puntatore alla locazione di memoria
		bool modo)//modo=true fa partire la misura
			  //modo=false recupera i risultati della misura
{   
//	mutex.lock();
	Spectrometer *spect;
    BbcFilter *filt1;
    BbcFilter *filt2;
    
	int i,nb,na;		// Get results
    long *ResultPol;

    spect=single->GetFFT();
    filt1=single->GetBbc1();
    filt2=single->GetBbc2();
    nb=na=single->getNumPuntiSpettro();
    if(tmpD->GetIntestazioneCh1().Getindex()!=-1){
	    if(single->getModoPolarimetrico()==true){
		    if(tmpD->GetBuffer()==NULL){
		    	if(tmpD->GetSizeBuffer()!=0) return;
		    	ResultPol = new long[4*na+2];
		    	tmpD->SetBuffer(ResultPol);
		    	tmpD->SetSizeBuffer(4*na+2);
		    	for (i=0; i<4*na+2; ++i) ResultPol[i] = 0;
		    }else ResultPol=tmpD->GetBuffer();
		    if (modo){
		    	ResultPol[(tmpD->GetSizeBuffer())-2]=filt1->Start();
		    	ResultPol[(tmpD->GetSizeBuffer())-1]=filt2->Start();
		    }else {
		    	ResultPol[(tmpD->GetSizeBuffer())-2]=filt1->TPRead(modo);//false
		    	ResultPol[(tmpD->GetSizeBuffer())-1]=filt2->TPRead(modo);//false
		    }
	    }else{
	    	if(tmpD->GetBuffer()==NULL){
	    		if(tmpD->GetSizeBuffer()!=0) return;
		    	ResultPol = new long[na+2];
		    	tmpD->SetBuffer(ResultPol);
		    	tmpD->SetSizeBuffer(na+2);
		    	for (i=0; i<na+2; ++i) ResultPol[i] = 0;
		    }else ResultPol=tmpD->GetBuffer();
	    	if (modo){
				ResultPol[(tmpD->GetSizeBuffer())-2]=filt1->Start();
				ResultPol[(tmpD->GetSizeBuffer())-1]=filt2->Start();
			}else {
				ResultPol[tmpD->GetSizeBuffer()-2]=filt1->TPRead(modo);//false
				ResultPol[tmpD->GetSizeBuffer()-1]=filt2->TPRead(modo);//false
			}

	    }
	}
 //   mutex.unlock();
}
//void SingleSpectrometer::GetData(//Gestisce il recupero delle Integrazioni
//		SingleSpectrometer *single,//Puntatore al Spettrometro
//		Data *tmpD)//Puntatore alla locazione di memoria
//	//di sospendere il recupero delle Integrazioni a favore delle misure di Tp
//{	
////	mutex.lock();
//	Spectrometer *spect;	
//    BbcFilter *filt1;
//    BbcFilter *filt2;
//
//    bool ready=false;
//
//	int i,j,nb=4096,err,na=4096;//4096;		// Get results
//    long *thisResultA = new long[na];
//    long *thisResultB = new long[nb];
//    long *thisResultC = new long[na];
//    long *thisResultD = new long[nb];
//    long *ResultPol;
//        
//    spect=single->GetFFT();
//    filt1=single->GetBbc1();
//    filt2=single->GetBbc2();
//    nb=na=single->getNumPuntiSpettro();
//    if(tmpD->GetIntestazioneCh1().Getindex()!=-1){
//	    if(single->getModoPolarimetrico()==true){
//		    if(tmpD->GetBuffer()==NULL){
//		    	if(tmpD->GetSizeBuffer()!=0) return;
//		    	ResultPol = new long[4*na+2];
//		    	tmpD->SetBuffer(ResultPol);
//		    	tmpD->SetSizeBuffer(4*na+2);
//		    }else{
//		    	ResultPol=tmpD->GetBuffer();
//		    	delete[] ResultPol;
//		    	ResultPol = new long[4*na+2];
//		    	tmpD->SetBuffer(ResultPol);
//		    	tmpD->SetSizeBuffer(4*na+2);
//		    }
//		    while(!ready){
//		    	ready=spect->DataReady();
//		    }
//		    if(spect->Overflow()) tmpD->SetOverflow(true);
//			for (i=0; i<4*na+2; ++i) ResultPol[i] = 0; 
//			err=spect->GetResults(thisResultA+na/2, na/2, 0); 
//			err=spect->GetResults(thisResultA,      na/2, na/2);//************
//			err=spect->GetResults(thisResultB+nb/2, nb/2, na);
//			err=spect->GetResults(thisResultB,      nb/2, na+nb/2);//************
//			err=spect->GetResults(thisResultC+na/2, na/2, na+nb);//*************************
//			err=spect->GetResults(thisResultC,      na/2, na+nb+na/2);
//			err=spect->GetResults(thisResultD+nb/2, nb/2, 2*na+nb);
//			err=spect->GetResults(thisResultD,      nb/2, 2*na+nb+nb/2);
//			for (i=0; i<na; ++i) ResultPol[i] = thisResultA[i];
//			for (j=0; i<2*na;j++, ++i) ResultPol[i] = thisResultB[j];
//			for (j=0; i<3*na;j++, ++i) ResultPol[i] = thisResultC[j];
//			for (j=0; i<4*na;j++, ++i) ResultPol[i] = thisResultD[j];
//	    }else{
//	    	if(tmpD->GetBuffer()==NULL){
//	    		if(tmpD->GetSizeBuffer()!=0) return;
//		    	ResultPol = new long[na+2];
//		    	tmpD->SetBuffer(ResultPol);
//		    	tmpD->SetSizeBuffer(na+2);
//		    }else{
//		    	ResultPol=tmpD->GetBuffer();
//		    	delete[] ResultPol;
//		    	ResultPol = new long[na+2];
//		    	tmpD->SetBuffer(ResultPol);
//		    	tmpD->SetSizeBuffer(na+2);
//		    }
//	    	for (i=0; i<na+2; ++i) ResultPol[i] = 0;
//	    	while(!ready){
//		    	ready=spect->DataReady();
//		    }
//	    	if(spect->Overflow()) tmpD->SetOverflow(true);
//	    	err=spect->GetResults(thisResultA+na/2, na/2, 0);
//	    	err=spect->GetResults(thisResultA,      na/2, na/2);
//			if(tmpD->GetIntestazioneCh2().Getindex()!=-1){
//				ResultPol = new long[2*na+2];
//				for (i=0; i<2*na+2; ++i) ResultPol[i] = 0;
//				err=spect->GetResults(thisResultB+nb/2, nb/2, na);
//				err=spect->GetResults(thisResultB,      nb/2, na+nb/2);
//				for (j=0, i=na; i<2*na;j++, ++i) ResultPol[i] = thisResultB[j];
//				tmpD->SetBuffer(ResultPol);
//				tmpD->SetSizeBuffer(2*na+2);
//			}
//			for (i=0; i<na; ++i) ResultPol[i] = thisResultA[i];
//	    }
//	} 
//    delete[] thisResultA;
//    delete[] thisResultB;
//	delete[] thisResultC;
//    delete[] thisResultD;
// //   mutex.unlock();
//}

void SingleSpectrometer::GetData(//Gestisce il recupero delle Integrazioni
		SingleSpectrometer *single,//Puntatore al Spettrometro
		Data *tmpD)//Puntatore alla locazione di memoria
	//di sospendere il recupero delle Integrazioni a favore delle misure di Tp
{	
//	mutex.lock();
	Spectrometer *spect;	
    BbcFilter *filt1;
    BbcFilter *filt2;

    bool ready=false;
    const int noff = 4096;//Necessario per essere indipendente dal numero di bins funzionamente
    int i,j,nb,err,na;		// Get results
    nb=na=single->getNumPuntiSpettro();
    long *thisResultA = new long[na];
    long *thisResultB = new long[nb];
    long *thisResultC = new long[na];
    long *thisResultD = new long[nb];
    long *ResultPol;
        
    spect=single->GetFFT();
    filt1=single->GetBbc1();
    filt2=single->GetBbc2();
//    nb=na=single->getNumPuntiSpettro();
    if(tmpD->GetIntestazioneCh1().Getindex()!=-1){
	    if(single->getModoPolarimetrico()==true){
		    if(tmpD->GetBuffer()==NULL){
		    	if(tmpD->GetSizeBuffer()!=0) return;
		    	ResultPol = new long[4*na+2];
		    	tmpD->SetBuffer(ResultPol);
		    	tmpD->SetSizeBuffer(4*na+2);
		    }else{
		    	ResultPol=tmpD->GetBuffer();
		    	delete[] ResultPol;
		    	ResultPol = new long[4*na+2];
		    	tmpD->SetBuffer(ResultPol);
		    	tmpD->SetSizeBuffer(4*na+2);
		    }
		    while(!ready){
		    	ready=spect->DataReady();
		    }
		    if(spect->Overflow()) tmpD->SetOverflow(true);
			for (i=0; i<4*na+2; ++i) ResultPol[i] = 0; 
			err=spect->GetResults(thisResultA+na/2, na/2, 0); 
			err=spect->GetResults(thisResultA,      na/2, na/2);//************
//			err=spect->GetResults(thisResultB+nb/2, nb/2, na);
//			err=spect->GetResults(thisResultB,      nb/2, na+nb/2);//************
//			err=spect->GetResults(thisResultC+na/2, na/2, na+nb);//*************************
//			err=spect->GetResults(thisResultC,      na/2, na+nb+na/2);
//			err=spect->GetResults(thisResultD+nb/2, nb/2, 2*na+nb);
//			err=spect->GetResults(thisResultD,      nb/2, 2*na+nb+nb/2);
			err=spect->GetResults(thisResultB+nb/2, nb/2, noff);
			err=spect->GetResults(thisResultB,      nb/2, noff+nb/2);//************
			err=spect->GetResults(thisResultC+na/2, na/2, noff*2);//*************************
			err=spect->GetResults(thisResultC,      na/2, noff*2+na/2);
			err=spect->GetResults(thisResultD+nb/2, nb/2, noff*3);
			err=spect->GetResults(thisResultD,      nb/2, noff*3+nb/2);
			for (i=0; i<na; ++i) ResultPol[i] = thisResultA[i];
			for (j=0; i<2*na;j++, ++i) ResultPol[i] = thisResultB[j];
			for (j=0; i<3*na;j++, ++i) ResultPol[i] = thisResultC[j];
			for (j=0; i<4*na;j++, ++i) ResultPol[i] = thisResultD[j];
	    }else{
	    	if(tmpD->GetBuffer()==NULL){
	    		if(tmpD->GetSizeBuffer()!=0) return;
		    	ResultPol = new long[na+2];
		    	tmpD->SetBuffer(ResultPol);
		    	tmpD->SetSizeBuffer(na+2);
		    }else{
		    	ResultPol=tmpD->GetBuffer();
		    	delete[] ResultPol;
		    	ResultPol = new long[na+2];
		    	tmpD->SetBuffer(ResultPol);
		    	tmpD->SetSizeBuffer(na+2);
		    }
	    	for (i=0; i<na+2; ++i) ResultPol[i] = 0;
	    	while(!ready){
		    	ready=spect->DataReady();
		    }
	    	if(spect->Overflow()) tmpD->SetOverflow(true);
	    	err=spect->GetResults(thisResultA+na/2, na/2, 0);
	    	err=spect->GetResults(thisResultA,      na/2, na/2);
			if(tmpD->GetIntestazioneCh2().Getindex()!=-1){
				ResultPol = new long[2*na+2];
				for (i=0; i<2*na+2; ++i) ResultPol[i] = 0;
				err=spect->GetResults(thisResultB+nb/2, nb/2, na);
				err=spect->GetResults(thisResultB,      nb/2, na+nb/2);
				for (j=0, i=na; i<2*na;j++, ++i) ResultPol[i] = thisResultB[j];
				tmpD->SetBuffer(ResultPol);
				tmpD->SetSizeBuffer(2*na+2);
			}
			for (i=0; i<na; ++i) ResultPol[i] = thisResultA[i];
	    }
	} 
    delete[] thisResultA;
    delete[] thisResultB;
	delete[] thisResultC;
    delete[] thisResultD;
 //   mutex.unlock();
}


void SingleSpectrometer::Init(//Inizializza l'HW
		SingleSpectrometer *single,//Puntatore al Spettrometro
		Data *tmpD)//Puntatore dei valori da impostare
{
	Spectrometer *fft;
	BbcFilter *bbc1;
	BbcFilter *bbc2;
	HeadResult tp;

	
	fft=single->GetFFT();
	fft->IntTime(tempoIntElementare);//Setto il tempo di integrazione Elemetare al FFT
	tp=tmpD->GetIntestazioneCh1();
	modoPolarimetrico=tp.GetModoPol();
	bbc1=single->GetBbc1();
	freqCentroBandaA=bbc1->Freq(tp.GetFlo());
	tp.SetFlo(freqCentroBandaA);
	banda=tp.GetBanda();
	banda=bbc1->Band(banda);
	tp.SetBanda(banda);
	bbc1->Gain((int)tp.GetGain());
	bbc1->InputChannel(tp.GetAdc(),modo8Bit);
	tmpD->SetIntestazioneCh1(tp);//Ritorno alla struttura i valori realmente accettati dal hardware
	tp=tmpD->GetIntestazioneCh2();
	bbc2=single->GetBbc2();
	freqCentroBandaB=bbc2->Freq(tp.GetFlo());
	tp.SetFlo(freqCentroBandaB);
	banda=bbc2->Band(banda);
	tp.SetBanda(banda);
	bbc2->Gain((int)tp.GetGain());
	bbc2->InputChannel(tp.GetAdc(),modo8Bit);
	tmpD->SetIntestazioneCh2(tp);//Ritorno alla struttura i valori realmente accettati dal hardware
	bbc1->Set();
	bbc2->Set();
}

void SingleSpectrometer::setON(int PuntiSpettro )//Setto ai valori di ON un elemento della classe
{

	  modo8Bit=true;
	  modoPolarimetrico=true;
	  tempoIntElementare=10;//10 secondi
	  numPuntiSpettro=PuntiSpettro;
	  banda=125* 1e6;;
	  freqCentroBandaA=25.0* 1e6;
	  freqCentroBandaB=25.0* 1e6;

}

void SingleSpectrometer::setOFF(int PuntiSpettro) //Setto ai valori di OFF un elemento della classe
{

	  modo8Bit=true;
	  modoPolarimetrico=false;
	  tempoIntElementare=10;//10 secondi
	  numPuntiSpettro=PuntiSpettro;
	  banda=125* 1e6;;
	  freqCentroBandaA=0.0* 1e6;
	  freqCentroBandaB=0.0* 1e6;

}

void SingleSpectrometer::initAttributes ( //inizializzo un elemento generico della classe
		Cpld2Intf *intf) //Puntatore all'oggetto che gestisce il protocollo TCP con l'HW
{
	setOFF(4096);
	fft = new Spectrometer(intf,0x00520);
	bbc1= new BbcFilter(intf, 0x00740);         // Filter1 (on chip 0x3-2)
	bbc2= new BbcFilter(intf, 0x00780);         // Filter2 (on chip 0x3-2)
	fft->Bbc(bbc1);
}
