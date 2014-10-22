// $Id: Protocol.cpp,v 1.1 2011-03-14 15:16:22 a.orlati Exp $

#include "Protocol.h"
#include "Common.h"
#include <stdlib.h>
#include <math.h>

float CProtocol::getRandomValue(const double& att)
{
	unsigned seed;
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	seed=now.microSecond();
	srand(seed);
	float val=(float)rand()/RAND_MAX; // 0..1 random number;
	float amp=NOISE_APPLITUDE/(powf(10,att/10.0));
	val*=amp;
	return val;
}

float CProtocol::getRandomValue()
{
	unsigned seed;
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	seed=now.microSecond();
	srand(seed);
	float val=(float)rand()/RAND_MAX; // 0..1 random number;
	float amp=NOISE_APPLITUDE/2;
	val*=amp;
	return (val+NOISE_APPLITUDE/10);
}
