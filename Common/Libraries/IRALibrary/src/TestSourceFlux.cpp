// $Id: TestDBTable.cpp,v 1.4 2011-03-14 13:18:54 a.orlati Exp $

#include <IRA>
#include <SourceFlux.h>
#include <cstdio>
#include <slamac.h>

using namespace IRA;

int main(int argc, char *argv[])
{
    flux_ds flux;
    double frequency;
    double fwhm;


    frequency = 22365.0;
    fwhm = 1.578*DPI /(180.0* 60.0);   // arcmin.....in radians

	//DR21

    flux.init = true;
    strcpy(flux.name,"dr21");
    flux.type = 'c';
    flux.nFreqRange=1;
    strcpy(flux.freqRange,"18000.0-26500.0");
    strcpy(flux.fluxCoeff1,"1.231");
    strcpy(flux.fluxCoeff2,"0.0");
    strcpy(flux.fluxCoeff3,"0.0");
    flux.size=20*DPI/(180.0*3600.0);   // arcsec ...in radians;
    strcpy(flux.model,"gauss");
    flux.mcoeff[0] = 100/100;  //percentage
    flux.mcoeff[1] = 20*DPI/(180.0*3600.0);   // arcsec ...in radians
    flux.mcoeff[2] = 20*DPI/(180.0*3600.0);   // arcsec ...in radians
    flux.mcoeff[3] = 0;
    flux.mcoeff[4] = 0;
    flux.mcoeff[5] = 0;

    CSourceFlux dr21Flux(flux);

    printf("dr21 flux = %f\n", dr21Flux.computeSourceFlux(frequency,fwhm));

    //3c123

    flux.init = true;
    strcpy(flux.name,"3c123");
    flux.type = 'c';
    flux.nFreqRange=1;
    strcpy(flux.freqRange,"500.0-23780.0");

    strcpy(flux.fluxCoeff1,"2.525");
    strcpy(flux.fluxCoeff2,"0.246");
    strcpy(flux.fluxCoeff3,"-0.1638");
    flux.size=23*DPI/(180.0*3600.0);   // arcsec ...in radians;
    strcpy(flux.model,"gauss");
    flux.mcoeff[0] = 100/100;  //percentage
    flux.mcoeff[1] = 23*DPI/(180.0*3600.0);   // arcsec ...in radians
    flux.mcoeff[2] = 5*DPI/(180.0*3600.0);   // arcsec ...in radians
    flux.mcoeff[3] = 0;
    flux.mcoeff[4] = 0;
    flux.mcoeff[5] = 0;

    CSourceFlux c123Flux(flux);

    printf("3c123 flux = %f\n", c123Flux.computeSourceFlux(frequency,fwhm));


    //dummy


    frequency = 21000.0;

    flux.init = true;
    strcpy(flux.name,"dummy");
    flux.type = 'c';
    flux.nFreqRange=1;
    strcpy(flux.freqRange,"20000.0-22000.0");

    strcpy(flux.fluxCoeff1,"1.3926969");
    strcpy(flux.fluxCoeff2,"0.0");
    strcpy(flux.fluxCoeff3,"0.0");
    flux.size=1*DPI/(180.0*3600.0);   // arcsec ...in radians;
    strcpy(flux.model,"noModel");  //  no convolution with beam and source model done! So parmaters below have no meaning
    flux.mcoeff[0] = 100/100;  //percentage
    flux.mcoeff[1] = 1*DPI/(180.0*3600.0);   // arcsec ...in radians
    flux.mcoeff[2] = 0;
    flux.mcoeff[3] = 0;
    flux.mcoeff[4] = 0;
    flux.mcoeff[5] = 0;

    CSourceFlux dummyFlux(flux);

    printf("dummy flux = %f\n", dummyFlux.computeSourceFlux(frequency,fwhm));




	return 0;	
}
