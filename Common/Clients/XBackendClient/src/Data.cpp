#include "Data.h"

// Constructors/Destructors
//

Data::Data ( ) {
	SetZeroData();
	initAttributes();
}

Data::~Data ( ) { }

void Data::initAttributes ( ) {

}

void Data::SetZeroData ( ) {
	intestazioneCh1.SetZeroHeadResult();
	intestazioneCh2.SetZeroHeadResult();
	buffer=NULL;
	sizeBuff=0;
	Overflow=false;
}

