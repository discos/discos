#include "PointingModelImpl.h"
#include <cmath>
#include <slamac.h>

void PointingModelImpl::getAzOff(double az, double el, double &azOff)
{
	double cosx,cosy,cosl,sinx,siny,sinl,f,cos2x,sin2x;

	cosx = cos(az);
  	cosy = cos(el);
  	cosl = cos(PM.phi*DD2R);
  	sinx = sin(az);
  	siny = sin(el);
  	cos2x = cos(2.*az);
  	sin2x = sin(2.*az);
  	sinl = sin(PM.phi*DD2R);
  	f = 0.0;

	if (PM.coefficientNum[ 0] != 0) f=f+PM.coefficientVal[0]*DD2R;
	if (PM.coefficientNum[ 1] != 0) f=f-PM.coefficientVal[1]*DD2R*cosl*sinx/cosy;
	if (PM.coefficientNum[ 2] != 0) f=f+PM.coefficientVal[2]*DD2R*siny/cosy;
	if (PM.coefficientNum[ 3] != 0) f=f-PM.coefficientVal[3]*DD2R/cosy;
	if (PM.coefficientNum[ 4] != 0) f=f+PM.coefficientVal[4]*DD2R*sinx*siny/cosy;
	if (PM.coefficientNum[ 5] != 0) f=f-PM.coefficientVal[5]*DD2R*siny*cosx/cosy;
	if (PM.coefficientNum[11] != 0) f=f+PM.coefficientVal[11]*DD2R*az;
	if (PM.coefficientNum[12] != 0) f=f+PM.coefficientVal[12]*DD2R*cosx;
	if (PM.coefficientNum[13] != 0) f=f+PM.coefficientVal[13]*DD2R*sinx;
	if (PM.coefficientNum[16] != 0) f=f+PM.coefficientVal[16]*DD2R*cos2x;
	if (PM.coefficientNum[17] != 0) f=f+PM.coefficientVal[17]*DD2R*sin2x;

	azOff = f;
};
