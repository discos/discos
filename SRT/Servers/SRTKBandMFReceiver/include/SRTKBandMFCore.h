/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Andrea Orlati <orlati@ira.inaf.it>
 *  Marco Buttu <mbuttu@oa-cagliari.inaf.it>
\*******************************************************************************/

#ifndef _SRTKBANDMFCORE_H_
#define _SRTKBANDMFCORE_H_

#include "MFKBandBaseCore.h"

/**
 * This class inherits from CComponentCore and so contains the code of almost 
 * all the features  of the component.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>.
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * <h1>Interface Summary</h1>
 *
 * <h2>Methods defined in this class</h2>
 * <ul>
 *      <li>virtual void initialize(...): it initializes the object</li>
 *      <li>ACS::doubleSeq getStageValues(...): it returns for each feed the fet quantity value 
 *      <li>of a given channel (if) and stage</li>
 *      <li>void setMode(): it allows to change the operating mode of the receiver</li>
 *      <li>void updateVdLNAControls(): it reads and updates from the LNA control board the 
 *      <li>drain voltage values of the transistors</li>
 *      <li>void updateIdLNAControls(): it reads and updates from the LNA control board the 
 *      <li>drain current values of the transistors</li>
 *      <li>void updateVgLNAControls(): it reads and updates from the LNA control board the 
 *      <li>gate voltage values of the transistors</li>
 *
 * <h2>Methods defined in the base class CComponentCore</h2>
 * <ul>
 *      <li>void initialize(...)</li>
 *      <li>virtual CConfiguration const * const execute()</li>
 *      <li>virtual void cleanup()</li>
 *      <li>void setLO(...)</li>
 *      <li>void activate(...)</li>
 *      <li>void getCalibrationMark(...)</li>
 *      <li>double getTaper(...)</li>
 *      <li>void calOn()</li>
 *      <li>void calOff()</li>
 *      <li>void vacuumSensorOn()</li>
 *      <li>void vacuumSensorOff()</li>
 *      <li>void lnaOn()</li>
 *      <li>void lnaOff()</li>
 *      <li>virtual void setMode(...)</li>
 *      <li>void updateVacuum()</li>
 *      <li>void updateVacuumPump()</li>
 *      <li>void updateVacuumValve()</li>
 *      <li>void updateCryoCoolHead()</li>
 *      <li>void updateCryoCoolHeadWin()</li>
 *      <li>void updateCryoLNA()</li>
 *      <li>void updateCryoLNAWin()</li>
 *      <li>void updateVertexTemperature()</li>
 *      <li>void updateIsRemote()</li>
 *      <li>void updateCoolHead()</li>
 *      <li>void updateNoiseMark()</li>
 *      <li>void updateComponent()</li>
 *      <li>double getVacuum()</li>
 *      <li>double getCryoCoolHead()</li>
 *      <li>double getCryoCoolHeadWin()</li>
 *      <li>double getCryoLNA()</li>
 *      <li>double getCryoLNAWin()</li> 
 *      <li>double getVertexTemperature()</li>
 *      <li>DWORD getStatusWord()</li>
 *      <li>double getFetValue(...)</li>
 *      <li>long getFeeds(...)</li>
 *      <li>void getLO(...)</li>
 *      <li>void getBandwidth(...)</li>
 *      <li>void getStartFrequency(...)</li>
 *      <li>void getPolarization(...)</li>
 *      <li>const IRA::CString& getSetupMode()</li>
 *      <li>const DWORD& getIFs()</li>
 *      <li>const DWORD& getFeeds()</li>
 *      <li>const Management::TSystemStatus& getComponentStatus()</li>
 *      <li>inline void setVacuumDefault(...)</li>
 * </ul>
 */
class SRTKBandMFCore : public CComponentCore {

public:

    SRTKBandMFCore();
    virtual ~SRTKBandMFCore();


    /**
     * This method initializes the object
     * @param service pointer to container services object provided by the container
     */
    virtual void initialize(maci::ContainerServices* services);


    /** Return for each feed the fet quantity value of a given channel (if) and stage. For
     *  instance, if you want to get the VD left channel values of all the feeds related to the amplifier stage N,
     *  you must call the method like so: stageValues(DRAIN_VOLTAGE, 0, N).
     *  @param quantity a FetValue: DRAIN_VOLTAGE, DRAIN_CURRENT or GATE_CURRENT 
     *  @param ifs the channel (0: LEFT, 1: RIGHT)
     *  @param stage the stage number (from 1 to 5)
     *  @return a doubleSeq of values for a given fet ``quantity``, channel and ``stage_number``.
     */
    ACS::doubleSeq getStageValues(const IRA::ReceiverControl::FetValue& control, DWORD ifs, DWORD stage);


    /**
     * It allows to change the operating mode of the receiver. If the mode does not correspond 
     * to a valid mode an error is thrown.
     * @param mode mode code as a string
     */
    void setMode(const char * mode) throw (
            ReceiversErrors::ModeErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl,
            ComponentErrors::ValidationErrorExImpl,
            ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,
            ComponentErrors::CORBAProblemExImpl,
            ReceiversErrors::LocalOscillatorErrorExImpl
    );

    
    /**
     * It reads and updates from the LNA control board the drain voltage values of the transistors
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateVdLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It reads and updates from the LNA control board the drain current values of the transistors
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateIdLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It reads and updates from the LNA control board the gate voltage values of the transistors
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateVgLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

protected:

	    /************************ CONVERSION FUNCTIONS **************************/
    // Convert the voltage value of the vacuum to mbar
    static double voltage2mbarF(double voltage) { return(pow(10, 1.5 * voltage - 12)); }


    // Convert the voltage value of the temperatures to Kelvin
    static double voltage2KelvinF(double voltage) {
        return voltage < 1.12 ? \
                  (660.549422889947 * pow(voltage, 6)) - (2552.334255456860 * \
                  pow(voltage, 5)) + (3742.529989384570 * pow(voltage, 4)) - \
                  (2672.656926956470 * pow(voltage, 3)) + (947.905578508975 * \
                  pow(voltage, 2)) - 558.351002849576 * voltage + 519.607622398508 \
                :
                  (865.747519105672 * pow(voltage, 6)) - (7271.931957100480 * \
                  pow(voltage, 5)) + (24930.666241800500 * pow(voltage, 4)) - \
                  (44623.988512320400 * pow(voltage, 3)) + (43962.922216886600 * \
                  pow(voltage, 2)) - 22642.245121997700 * voltage + 4808.631312836750;
    }


    // Convert the voltage value of the temperatures to Celsius (Sensor B57703-10K)
    static double voltage2CelsiusF(double voltage) 
    { return -5.9931 * pow(voltage, 5) + 40.392 * pow(voltage, 4) - 115.41 * pow(voltage, 3) + 174.67 * pow(voltage, 2) - 174.23 * voltage + 112.79; }

    // Convert the ID voltage value to the mA value
    static double currentConverterF(double voltage) { return(10 * voltage); }


    // Convert the VD and VG voltage values using a right scale factor
    static double voltageConverterF(double voltage) { return(voltage); }
    /********************** END CONVERSION FUNCTIONS ***********************/

private:

    std::vector<IRA::ReceiverControl::StageValues> m_vdStageValues;
    std::vector<IRA::ReceiverControl::StageValues> m_idStageValues;
    std::vector<IRA::ReceiverControl::StageValues> m_vgStageValues;
};


#endif
