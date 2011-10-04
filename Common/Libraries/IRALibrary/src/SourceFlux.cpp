// $id$

#include "SourceFlux.h"
#include <cstring>
#include <cmath>
#include <DateTime.h>
#include <IRA>

using namespace IRA;

CSourceFlux::CSourceFlux()
{

}

CSourceFlux::CSourceFlux(const flux_ds flux)
{
    m_flux = flux;
} 

CSourceFlux::~CSourceFlux()
{	
}

double CSourceFlux::computeSourceFlux(const double frequency, const double FWHM)
{
    double logfreq,logflux,fluxv;
    double fac0,fac1,fac2,fac4,fac5,fac;
    double corr, size;

    fluxv=1.0;
    corr=1.0;
    size=0.0;

    
    if (m_flux.init && frequency >= m_flux.fmin && frequency <= m_flux.fmax) {
        logfreq=log10(frequency);
        logflux=m_flux.fcoeff[0]+logfreq*m_flux.fcoeff[1]+logfreq*logfreq*m_flux.fcoeff[2];
        fluxv=pow(10.0,logflux);
        if(strcmp("casa",m_flux.name)==0) {
            double dflux;
            double epoch;
            TIMEVALUE now;
            TIMEVALUE tS;
            IRA::CIRATools::getTime (now);
            tS.value (now.value().value);
            epoch = tS.year () + tS.dayOfYear () / 366.0;
            if (m_flux.fcoeff[0] > 5.7){  /*entry is for discredited 1977 flux*/
                dflux = 1 - ( (0.97 - 0.30*(logfreq-3)) /100.0);
                fluxv*= pow(10.0,(epoch-1980.0)*log10(dflux));
            } else {                         /*entry for preliminary 2006 flux*/
                dflux = 1 - ( 0.65  /100.0);
                fluxv*= pow(10.0,(epoch-2006.0)*log10(dflux));
            }
        }
        if((strcmp("gauss",m_flux.model)==0)) {
            fac1=m_flux.mcoeff[1]/FWHM;
            fac2=m_flux.mcoeff[2]/FWHM;
            fac4=m_flux.mcoeff[4]/FWHM;
            fac5=m_flux.mcoeff[5]/FWHM;
            corr=m_flux.mcoeff[0]*sqrt((1.0+fac1*fac1)*(1.0+fac2*fac2))+m_flux.mcoeff[3]*sqrt((1.0+fac4*fac4)*(1.0+fac5*fac5));
        } else if((strcmp("disk",m_flux.model)==0)) {
            fac0=m_flux.mcoeff[0]/FWHM;
            fac=log(2.0)*fac0*fac0;
            corr=fac/(1.-exp(-fac));
        } else if((strcmp("2pts",m_flux.model)==0)) {
            fac0=(m_flux.mcoeff[0]*.5)/FWHM;
            corr=exp(4.0*log(2.0)*fac0*fac0);
        } else
	        corr=1.0;
            size=m_flux.size;
    }

    m_SourceFlux = fluxv / corr;

    return m_SourceFlux;
}
