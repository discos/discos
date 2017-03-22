#include "Specifiche.h"

// Constructors/Destructors
//

Specifiche::Specifiche ( ) {
initAttributes();
}

Specifiche::~Specifiche ( ) { }

void Specifiche::initAttributes ( ) {
	int i=0;
	HeadResult temp;
	
	tempoEle=10;
	FunctionAdc=1;//deve essere 0
	nCicli=0;
	tempoIntegrazione=5*60;
	modo8bit=true;
	maxSezioni=16;
	nSezioni=1;
	//temp.SetZeroHeadResult();//Sono tutti non usati
	//specificaSezione.reserve(nSezioni);
	specificaSezione.reserve(maxSezioni);
	for(i=0;i<nSezioni;i++){
		temp.Setindex(i);
		specificaSezione.push_back(temp);//Creo e Azzero la struttura vector
	}
}

void Specifiche::InsertElementArray ( ) {
	HeadResult temp;
	
	temp.SetZeroHeadResult();//Sono tutti non usati
	temp.Setindex(nSezioni-1);
	specificaSezione.push_back(temp);
		
}

void Specifiche::ResetArray ( ) {
	int i=0;
	HeadResult temp;
	
	//temp.SetZeroHeadResult();//Sono tutti non usati
	for(i=0;i<nSezioni;i++){
			temp.Setindex(i);
			specificaSezione[i]=temp;//Creo e Azzero la struttura vector
		}
}
