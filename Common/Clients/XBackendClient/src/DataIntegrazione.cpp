#include "DataIntegrazione.h"
const int Max=4096;

DataIntegrazione::DataIntegrazione ( ) {
	SetZeroDataIntegratione ( );
	initAttributes();

}

DataIntegrazione::~DataIntegrazione ( ) { }

void DataIntegrazione::SetTpZero ( ) {

	tp.resize(2);
	for(int i=0;i<2;i++) tp[i]=0;
 }

void DataIntegrazione::SetXxZero ( ) {

	xx.resize(lungArrayDati);
	for(int i=0;i<lungArrayDati;i++) xx[i]=0;
 }

void DataIntegrazione::SetYyZero () {

	yy.resize(lungArrayDati);
	for(int i=0;i<lungArrayDati;i++) yy[i]=0;
 }

void DataIntegrazione::SetXyZero () {
	 complex< float > zero;//è gia inizializzato a zero

	 xy.resize(lungArrayDati);
	 for(int i=0;i<lungArrayDati;i++) xy[i]=zero;
}

void DataIntegrazione::SetTp ( double *buff, int size, bool modo){
	int c=0;

	c=tp.capacity();
	if(c<2) SetTpZero();
	if (modo){ for(c=0;c<2;c++) tp[c]+=(double)buff[c+size-2];}
	else tp[0]+=(double)buff[size-2];
}

void DataIntegrazione::SetXx ( double *buff){
	int c=0;

	c=xx.capacity();
	if(c<lungArrayDati) SetXxZero();
	for(c=0;c<lungArrayDati;c++) xx[c]+=(double)buff[c];
}

void DataIntegrazione::SetXx2( long *buff){
	int c=0;

	c=xx.capacity();
	if(c<lungArrayDati) SetXxZero();
	for(c=0;c<lungArrayDati;c++) xx[c]+=(double)buff[c+lungArrayDati];
}

void DataIntegrazione::SetYy ( double *buff){
	int c=0;

	c=yy.capacity();
	if(c<lungArrayDati) SetYyZero();
	for(c=0;c<lungArrayDati;c++) yy[c]+=(double)buff[c+lungArrayDati];
}

void DataIntegrazione::SetXy ( double *buff){
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

void DataIntegrazione::initAttributes ( ) {//DA MODIFICARE******************
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

void DataIntegrazione::SetZeroDataIntegratione ( ) {

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

