#include "headResult.h"


HeadResult::HeadResult ( ) {// Constructors

//SetZeroHeadResult();
initAttributes();
}

HeadResult::~HeadResult ( ) { }//Destructors


void HeadResult::initAttributes ( ) {

	flo=25.0* 1e6;
	gain=2;//20
	banda=125* 1e6;
	adc=5;
	index=0;
	modoPol=false;//true;
}

void HeadResult::SetZeroHeadResult ( ) {//Spettrometro non usato

	flo=0.0;
	banda=125.e6;
	gain=0.0;
	adc=0;
	index=-1;
	modoPol=false;//DA chiedere****************************
}

bool HeadResult::operator==(const HeadResult & second) const
{
	return (adc == second.adc) && (flo == second.flo)
	&& (gain == second.gain)&& (index == second.index)
	&& (modoPol == second.modoPol)&& (banda == second.banda);
}

bool HeadResult::operator!=(const HeadResult & second) const
{
	return (adc != second.adc) || (flo != second.flo)
	||(gain != second.gain)|| (index != second.index)
	||(modoPol != second.modoPol)||(banda != second.banda);
}
