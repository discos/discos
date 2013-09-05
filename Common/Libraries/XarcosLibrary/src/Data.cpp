#include "../include/Data.h"

// Constructors/Destructors
//

Data::Data ( ) //Costruttore della classe
{
	SetZeroData();
	initAttributes();
}

Data::~Data ( ) { }//Distruttore della classe

void Data::initAttributes ( )//Inizializza un elemento della classe
{
//	intestazioneCh1.SetZeroHeadResult();
//	intestazioneCh2.SetZeroHeadResult();
//	buffer=NULL;
//	sizeBuff=0;
//	Overflow=false;
}

void Data::SetZeroData ( ) //Azzera un elemento della classe
{
	intestazioneCh1.SetZeroHeadResult();
	intestazioneCh2.SetZeroHeadResult();
	buffer=NULL;
	sizeBuff=0;
	Overflow=false;
}

