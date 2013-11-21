#include "../include/DataIntegrazione.h"

DataIntegrazione::DataIntegrazione ( )//Costruttore della classe 
{
	SetZeroDataIntegratione ( );
	initAttributes();

}

DataIntegrazione::~DataIntegrazione ( ) { }//Distruttore della classe 

void DataIntegrazione::SetTpZero ( ) //Azzera la struttura vector tp
{

	tp.resize(2);
	for(int i=0;i<2;i++) tp[i]=0;
 }

void DataIntegrazione::SetXxZero ( ) //Azzera la struttura vector xx
{

	xx.resize(lungArrayDati);
	for(int i=0;i<lungArrayDati;i++) xx[i]=0;
 }

void DataIntegrazione::SetYyZero () //Azzera la struttura vector yy
{

	yy.resize(lungArrayDati);
	for(int i=0;i<lungArrayDati;i++) yy[i]=0;
 }

void DataIntegrazione::SetXyZero () //Azzera la struttura vector xy
{
	 complex< float > zero;//è gia inizializzato a zero

	 xy.resize(lungArrayDati);
	 for(int i=0;i<lungArrayDati;i++) xy[i]=zero;
}

void DataIntegrazione::SetTp ( //Aggiorno la struttura vector tp
		long *buff,//Puntatore alla locazione di memoria
					//che contiene risultati di un misura elemetare (200 millisecond)
		int size,//Dimensione del Buffer
		bool modo)//modo=true siamo in FULL STOCKS OUT=2
{				  //modo=false non siamo in FULL STOCKS OUT=1
	int c=0;

	c=tp.capacity();
	if(c<2) SetTpZero();
	if (modo){ for(c=0;c<2;c++) tp[c]+=(double)buff[c+size-2];}
	else tp[0]+=(double)buff[size-2];
}

void DataIntegrazione::SetXx ( //Aggiorno la struttura vector xx
		long *buff)//Puntatore alla locazione di memoria
{					//che contiene risultati di un integrazione elemetare
	int c=0;

	c=xx.capacity();
	if(c<lungArrayDati) SetXxZero();
	for(c=0;c<lungArrayDati;c++) xx[c]+=(double)buff[c];
}

void DataIntegrazione::SetXx2( //Aggiorno la struttura vector xx
		long *buff)//Puntatore alla locazione di memoria
{					//che contiene risultati di un integrazione elemetare
	int c=0;

	c=xx.capacity();
	if(c<lungArrayDati) SetXxZero();
	for(c=0;c<lungArrayDati;c++) xx[c]+=(double)buff[c+lungArrayDati];
}

void DataIntegrazione::SetYy ( //Aggiorno la struttura vector yy
		long *buff)//Puntatore alla locazione di memoria
{					//che contiene risultati di un integrazione elemetare
	int c=0;

	c=yy.capacity();
	if(c<lungArrayDati) SetYyZero();
	for(c=0;c<lungArrayDati;c++) yy[c]+=(double)buff[c+lungArrayDati];
}

void DataIntegrazione::SetXy ( //Aggiorno la struttura vector xy
		long *buff)//Puntatore alla locazione di memoria 
{					//che contiene risultati di un integrazione elemetare
	complex< double > comp;
	double a=0,b=0;
	int c=0;

	c=xy.capacity();
	if(c<lungArrayDati) SetXyZero()	;
	for(c=0;c<lungArrayDati;c++){
		a=(double)buff[c+lungArrayDati*2];
		b=(double)buff[c+lungArrayDati*3];
		comp=complex< double >(a,b);
		xy[c]+=comp;
		comp=xy[c];
	}

}

void DataIntegrazione::initAttributes ( ) //Inizializza un elemento della classe
{
	  head.SetZeroHeadResult();
	  tempoIntegrazione=0;
	  modo8Bit=true;
	  utInizio.dstflag=0;
	  utInizio.millitm=0;
	  utInizio.timezone=0;
	  utInizio.time=0;
//	  utInizio.timeb=0;
	  utFine=utInizio;
	  ValoriValidi=false;
	  nArrayDati=0;
	  lungArrayDati=0;
	  Overflow=false;
	  //Gestione Array da Implementare***************************************
}

void DataIntegrazione::SetZeroDataIntegratione ( ) //Azzera un elemento della classe
{

	  head.SetZeroHeadResult();
	  tempoIntegrazione=0;
	  modo8Bit=true;
	  utInizio.dstflag=0;
	  utInizio.millitm=0;
	  utInizio.timezone=0;
	  utInizio.time=0;
//	  utInizio.timeb=0;
	  utFine=utInizio;
	  ValoriValidi=false;
	  nArrayDati=0;
	  lungArrayDati=0;
	  Overflow=false;
	  //Gestione Array da Implementare***************************************
}

