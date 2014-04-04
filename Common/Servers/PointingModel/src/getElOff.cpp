#include "PointingModelImpl.h"
#include <cmath>
#include <slamac.h>

void PointingModelImpl::getElOff(double az, double el, double &elOff)
{
	double f,cosx,cosy,sinx,siny,sinl,cosl,cos2x,sin2x,cos8y,sin8y;

	cosx=cos(az);
  	cosy=cos(el);
  	cos2x=cos(2.0*az);
  	cos8y=cos(8.0*el);
  	sinx=sin(az);
  	siny=sin(el);
  	sin2x=sin(2.0*az);
  	sin8y=sin(8.0*el);
  	sinl=sin(PM.phi*DD2R);
  	cosl=cos(PM.phi*DD2R);
  	f=0.0;

	if (PM.coefficientNum[ 4] != 0) f=f+PM.coefficientVal[4]*DD2R*cosx;
	if (PM.coefficientNum[ 5] != 0) f=f+PM.coefficientVal[5]*DD2R*sinx;
	if (PM.coefficientNum[ 6] != 0) f=f+PM.coefficientVal[6]*DD2R;
	if (PM.coefficientNum[ 7] != 0) f=f-PM.coefficientVal[7]*DD2R*(cosl*siny*cosx-sinl*cosy);
	if (PM.coefficientNum[ 8] != 0) f=f+PM.coefficientVal[8]*DD2R*el;
	if (PM.coefficientNum[ 9] != 0) f=f+PM.coefficientVal[9]*DD2R*cosy;
	if (PM.coefficientNum[10] != 0) f=f+PM.coefficientVal[10]*DD2R*siny;
	if (PM.coefficientNum[14] != 0) f=f+PM.coefficientVal[14]*DD2R*cos2x;
	if (PM.coefficientNum[15] != 0) f=f+PM.coefficientVal[15]*DD2R*sin2x;
	if (PM.coefficientNum[18] != 0) f=f+PM.coefficientVal[18]*DD2R*cos8y;
	if (PM.coefficientNum[19] != 0) f=f+PM.coefficientVal[19]*DD2R*sin8y;
	if (PM.coefficientNum[20] != 0) f=f+PM.coefficientVal[20]*DD2R*cosx;
	if (PM.coefficientNum[21] != 0) f=f+PM.coefficientVal[21]*DD2R*sinx;

	elOff = f;
};
