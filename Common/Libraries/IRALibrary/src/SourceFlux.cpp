// $id$

#include "SourceFlux.h"
#include <cstring>
#include <cmath>
#include <DateTime.h>
#include <IRA>

using namespace IRA;

#define BRIGHTNESS_MODEL_DISK "DISK"
#define BRIGHTNESS_MODEL_2PTS "2PTS"
#define BRIGHTNESS_MODEL_GAUSS "GAUSS"
#define BRIGHTNESS_NO_MODEL "NO_MODEL"

CSourceFlux::CSourceFlux()
{

}

CSourceFlux::CSourceFlux(const flux_ds& flux)
{
    m_flux = flux;
} 

CSourceFlux::~CSourceFlux()
{	
}

double CSourceFlux::computeSourceFlux(const double&frequency, const double& FWHM)
{
    double logfreq,logflux,fluxv;
    double fac0,fac1,fac2,fac4,fac5,fac;
    double corr, size;
    double fMin=0.0, fMax=0.0;
    double fMinRange[3], fMaxRange[3];
    double fluxCoeff1[3], fluxCoeff2[3], fluxCoeff3[3];
    double fcoeff0=0.0, fcoeff1=0.0, fcoeff2=0.0;

    fluxv=1.0;
    corr=1.0;
    size=0.0;

    if (m_flux.nFreqRange == 1) {
        sscanf(m_flux.freqRange,"%lf-%lf",&fMinRange[0],&fMaxRange[0]);
        fMin = fMinRange[0]; fMax = fMaxRange[0];
        sscanf(m_flux.fluxCoeff1,"%lf",&fluxCoeff1[0]); fcoeff0 = fluxCoeff1[0];
        sscanf(m_flux.fluxCoeff2,"%lf",&fluxCoeff2[0]); fcoeff1 = fluxCoeff2[0];
        sscanf(m_flux.fluxCoeff3,"%lf",&fluxCoeff3[0]); fcoeff2 = fluxCoeff3[0];
    }
    if (m_flux.nFreqRange == 2) {
        sscanf(m_flux.freqRange,"%lf-%lf,%lf-%lf",&fMinRange[0],&fMaxRange[0],&fMinRange[1],&fMaxRange[1]);
        if (frequency >= fMinRange[0] && frequency <= fMaxRange[0]) {
            fMin = fMinRange[0]; fMax = fMaxRange[0];
            sscanf(m_flux.fluxCoeff1,"%lf",&fluxCoeff1[0]); fcoeff0 = fluxCoeff1[0];
            sscanf(m_flux.fluxCoeff2,"%lf",&fluxCoeff2[0]); fcoeff1 = fluxCoeff2[0];
            sscanf(m_flux.fluxCoeff3,"%lf",&fluxCoeff3[0]); fcoeff2 = fluxCoeff3[0];
        }
        if (frequency >= fMinRange[1] && frequency <= fMaxRange[1]) {
            fMin = fMinRange[1]; fMax = fMaxRange[1];
            sscanf(m_flux.fluxCoeff1,"%lf",&fluxCoeff1[1]); fcoeff0 = fluxCoeff1[1];
            sscanf(m_flux.fluxCoeff2,"%lf",&fluxCoeff2[1]); fcoeff1 = fluxCoeff2[1];
            sscanf(m_flux.fluxCoeff3,"%lf",&fluxCoeff3[1]); fcoeff2 = fluxCoeff3[1];
        }
    }
    if (m_flux.nFreqRange == 3) {
        sscanf(m_flux.freqRange,"%lf-%lf,%lf-%lf,%lf-%lf",&fMinRange[0],&fMaxRange[0],&fMinRange[1],&fMaxRange[1],&fMinRange[2],&fMaxRange[2]);
        if (frequency >= fMinRange[0] && frequency <= fMaxRange[0]) {
            fMin = fMinRange[0]; fMax = fMaxRange[0];
            sscanf(m_flux.fluxCoeff1,"%lf",&fluxCoeff1[0]); fcoeff0 = fluxCoeff1[0];
            sscanf(m_flux.fluxCoeff2,"%lf",&fluxCoeff2[0]); fcoeff1 = fluxCoeff2[0];
            sscanf(m_flux.fluxCoeff3,"%lf",&fluxCoeff3[0]); fcoeff2 = fluxCoeff3[0];
        }
        if (frequency >= fMinRange[1] && frequency <= fMaxRange[1]) {
            fMin = fMinRange[1]; fMax = fMaxRange[1];
            sscanf(m_flux.fluxCoeff1,"%lf",&fluxCoeff1[1]); fcoeff0 = fluxCoeff1[1];
            sscanf(m_flux.fluxCoeff2,"%lf",&fluxCoeff2[1]); fcoeff1 = fluxCoeff2[1];
            sscanf(m_flux.fluxCoeff3,"%lf",&fluxCoeff3[1]); fcoeff2 = fluxCoeff3[1];
        }
        if (frequency >= fMinRange[2] && frequency <= fMaxRange[2]) {
            fMin = fMinRange[2]; fMax = fMaxRange[2];
            sscanf(m_flux.fluxCoeff1,"%lf",&fluxCoeff1[2]); fcoeff0 = fluxCoeff1[2];
            sscanf(m_flux.fluxCoeff2,"%lf",&fluxCoeff2[2]); fcoeff1 = fluxCoeff2[2];
            sscanf(m_flux.fluxCoeff3,"%lf",&fluxCoeff3[2]); fcoeff2 = fluxCoeff3[2];
        }
    }

    if (m_flux.init && frequency >= fMin && frequency <= fMax) {
        logfreq=log10(frequency);
        logflux=fcoeff0+logfreq*fcoeff1+logfreq*logfreq*fcoeff2;
        fluxv=pow(10.0,logflux);
        if(strcmp("casa",m_flux.name)==0) {
            double dflux;
            double epoch;
            TIMEVALUE now;
            TIMEVALUE tS;
            IRA::CIRATools::getTime (now);
            tS.value (now.value().value);
            epoch = tS.year () + tS.dayOfYear () / 366.0;
            if (fcoeff0 > 5.7){  /*entry is for discredited 1977 flux*/
                dflux = 1 - ( (0.97 - 0.30*(logfreq-3)) /100.0);
                fluxv*= pow(10.0,(epoch-1980.0)*log10(dflux));
            } else {                         /*entry for preliminary 2006 flux*/
                dflux = 1 - ( 0.65  /100.0);
                fluxv*= pow(10.0,(epoch-2006.0)*log10(dflux));
            }
        }
        if((strcmp(BRIGHTNESS_MODEL_GAUSS,m_flux.model)==0)) {
            if ((strcmp("virgoa",m_flux.model)==0) && (frequency >= fMinRange[1] && frequency <= fMaxRange[1])) {
                m_flux.mcoeff[0]=0.973;
                m_flux.mcoeff[3]=0.027;
            }
            fac1=m_flux.mcoeff[1]/FWHM;
            fac2=m_flux.mcoeff[2]/FWHM;
            fac4=m_flux.mcoeff[4]/FWHM;
            fac5=m_flux.mcoeff[5]/FWHM;
            corr=m_flux.mcoeff[0]*sqrt((1.0+fac1*fac1)*(1.0+fac2*fac2))+m_flux.mcoeff[3]*sqrt((1.0+fac4*fac4)*(1.0+fac5*fac5));
        } else if((strcmp(BRIGHTNESS_MODEL_DISK,m_flux.model)==0)) {
            fac0=m_flux.mcoeff[0]/FWHM;
            fac=log(2.0)*fac0*fac0;
            corr=fac/(1.-exp(-fac));
        } else if((strcmp(BRIGHTNESS_MODEL_2PTS,m_flux.model)==0)) {
            fac0=(m_flux.mcoeff[0]*.5)/FWHM;
            corr=exp(4.0*log(2.0)*fac0*fac0);
        } else  // NO_MODEL
	        corr=1.0;
        
        m_SourceFlux = fluxv / corr;
    }
    else
        m_SourceFlux = 0.0;

    return m_SourceFlux;
}
