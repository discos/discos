// $Id: TestDBTable.cpp,v 1.4 2011-03-14 13:18:54 a.orlati Exp $

#include <IRA>
#include <SourceFlux.h>
#include <cstdio>

using namespace IRA;

int main(int argc, char *argv[])
{
    flux_ds flux;
    double frequency = 10000;
    double fwhm = 5.0;

    flux.init = true;
    flux.name = "3c48      ";
    flux.type = 'c';
    flux.fmin = 500;
    flux.fmax = 23780;
    flux.fcoeff[0] = 2.465;
    flux.fcoeff[1] = -0.004;
    flux.fcoeff[2] = -0.1251;
    flux.size = 1.5;
    flux.model[0] = 'g';
    flux.model[1] = 'a';
    flux.model[2] = 'u';
    flux.model[3] = 's';
    flux.model[4] = 's';
    flux.mcoeff[0] = 100;
    flux.mcoeff[1] = 1;
    flux.mcoeff[2] = 0;
    flux.mcoeff[3] = 0;
    flux.mcoeff[4] = 0;
    flux.mcoeff[5] = 0;

    CSourceFlux sFlux(flux);

    printf("flux = %f\n", sFlux.computeSourceFlux(frequency,fwhm));

    frequency = 5000;
    fwhm = 2.5;

    printf("flux = %f\n", sFlux.computeSourceFlux(frequency,fwhm));

	return 0;	
}
