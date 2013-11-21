#include "../include/Specifiche.h"

// Constructors/Destructors
//

Specifiche::Specifiche ( ) //Costruttore della classe
{
initAttributes();
}

Specifiche::~Specifiche ( ) { } //Distruttore della classe

void Specifiche::initAttributes ( ) //Inizializza un elemento della classe
{
	int i=0;
	HeadResult temp;

	FunctionAdc=0;//1;//deve essere 0
	nCicli=0;
	tempoIntegrazione=5*60;
	modo8bit=false;//true;Questo in Xarcos al momento non cambia mai!!
	maxSezioni=16;
	nSezioni=1;//8;
	//temp.SetZeroHeadResult();//Sono tutti non usati
	//specificaSezione.reserve(nSezioni);
	specificaSezione.reserve(maxSezioni);
	for(i=0;i<nSezioni;i++){
		temp.Setindex(i);
		specificaSezione.push_back(temp);//Creo e Azzero la struttura vector
	}
}

void Specifiche::InsertElementArray ( )//Inserisco un nuovo elemento nella struttura vector specificaSezione
{
	HeadResult temp;
	
	//temp.SetZeroHeadResult();//Sono tutti non usati
	temp.Setindex(nSezioni-1);
	specificaSezione.push_back(temp);
		
}

void Specifiche::ResetArray ( )//Azzero la struttura vector specificaSezione
{
	int i=0;
	HeadResult temp;
	
	//temp.SetZeroHeadResult();//Sono tutti non usati
	for(i=0;i<nSezioni;i++){
			temp.Setindex(i);
			specificaSezione[i]=temp;//Creo e Azzero la struttura vector
		}
}
