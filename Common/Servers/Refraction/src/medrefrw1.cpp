#include "RefractionImpl.h"
#include <cmath>
#include <slamac.h>

#define REF_MAX(a,b) ((a > b) ? a : b)

void RefractionImpl::medrefrw1(double delin, double tempc, double humi, double pres, double &corZenithDistance)
{
	double el,pwpst,pp,tempk,sn;
  	double aphi,ang,dele,bphi;
	double a = 40;
	double b = 2.7;
	double c = 4.0;
	double d = 42.5;
	double e = 0.4;
	double f = 2.64;
	double g = .57295787e-4;
 
  	el = REF_MAX(1.0,delin*DR2D);

	/* Compute SN (surface refractivity) */

	tempk = tempc + 273.0;

	pwpst=6.105*exp(25.22*((tempk-273)/tempk)-5.31*log(tempk/273));

	/*pressione di vapor saturo dal Crane*/

	pp=pwpst*humi/100;

	sn = 77.6*(pres+(4810.0*pp)/tempk)/tempk;/*Rifrattivita'*/

	/* Compute refraction at elevation*/

  	aphi = a/pow((el+b),c);
  	ang = (90-el)*DD2R;
  	dele = -d/pow((el+e),f);
  	bphi = g*(sin(ang)/cos(ang)+dele);
  	if (el < 0.01) bphi = g*(1.0+dele);
  	corZenithDistance = (bphi*sn-aphi)*DD2R;
};
