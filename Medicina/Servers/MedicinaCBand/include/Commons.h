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

   

#endif