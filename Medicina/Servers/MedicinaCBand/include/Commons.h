#ifndef MEDICINA_C_BAND_COMMONS_H
#define MEDICINA_C_BAND_COMMONS_H

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>
#include <ReceiversDefinitionsC.h>


	/**
	 * @brief Calibration mark data repr
	 * 
	 */
	typedef struct {
		double skyFrequency;
		double markValue;
		Receivers::TPolarization polarization;
	} TMarkValue;

    /**
     * @brief Synth data repr. 
     */
	typedef struct {
		double frequency;
		double outputPower;
	} TLOValue;

    /**
     * @brief Taper data repr.
     */
	typedef struct {
		double frequency;
		double taper;
	} TTaperValue;

    /**
     * @brief Feed representation 
     */
	typedef struct {
		WORD code;
		double xOffset;
		double yOffset;
		double relativePower;
	} TFeedValue;

    /**
     * @brief Board value repr.
     */
	typedef struct {
		double temperature;
        ACS::Time timestamp;
	} BoardValue;


/** Helpers namespace */

namespace Helpers{

     /**
     * @todo 
     */    
     double linearFit(double *X,double *Y,const WORD& size,double x);
    /**
     *  Convert the voltage value of the vacuum to mbar
     */
    double voltage2mbar(double voltage);

    /**
     *  Convert the voltage value of the temperatures to Kelvin
     */
    double voltage2Kelvin(double voltage);

    /**
     *  Convert the voltage value of the temperatures to Celsius (Sensor B57703-10K)
     */
    double voltage2Celsius(double voltage);

    /**
     *  Convert the ID voltage value to the mA value
     */
    double currentConverter(double voltage);

    /**
     *  Convert the VD and VG voltage values using a right scale factor
     */
    double voltageConverter(double voltage);

}
#endif