/* ***************************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                                            */
/* $Id: SourceFlux.h,v 1.1 2011-09-26 09:53:59 c.migoni Exp $                            */
/*                                                                                       */
/* This code is under GNU General Public Licence (GPL).                                  */
/*                                                                                       */
/* Who                                          when           What                      */
/* Carlo Migoni (migoni@oa-cagliari.inaf.it)    26/09/2011     Creation                  */

#ifndef SOURCEFLUX_H_
#define SOURCEFLUX_H_

namespace IRA {

    /**
 * This class performs the flux computation of a source.
 * The conputation is based on the flux_val() routine of the Field System.
 * @author <a href=mailto:migoni@oa-cagliari.inaf.it>Carlo Migoni</a>,
 * OAC - Osservatorio Astronomico di Cagliari, Italia
 * <br> 
*/
    struct flux_ds {
        bool init;
        char name[11];
        char type;
        double fmin;
        double fmax;
        double fcoeff[3];
        double size;
        char model[5];
        double mcoeff[6];
    };

    /**
     * This class performs the flux computation of a source.
     * The conputation is based on the flux_val() routine of the Field System.
     * @author <a href=mailto:migoni@oa-cagliari.inaf.it>Carlo Migoni</a>,
     * OAC - Osservatorio Astronomico di Cagliari, Italia
     * <br> 
     */

    class CSourceFlux {
    
        public:
            /**
             * Constructor.
            */
            CSourceFlux();

            /** 
             * Constructor.
             * @param geometry flux parameter
            */
            CSourceFlux(const flux_ds flux);

            /**
             * Destructor.
            */
            ~CSourceFlux();

            /**
             * This function calculates the flux of a source
             * @param frequency frequency of the observer
             * @param FWHM Full Width Half Maximum
             * return flux
            */
            double computeSourceFlux (const double frequency, const double FWHM);

        private:

            double m_SourceFlux;

            flux_ds m_flux;
    };

}

#endif /*SOURCEFLUX_H_*/
