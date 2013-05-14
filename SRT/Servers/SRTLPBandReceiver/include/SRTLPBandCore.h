/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Marco Buttu <mbuttu@oa-cagliari.inaf.it>
 *  Andrea Orlati <orlati@ira.inaf.it>
\*******************************************************************************/

#ifndef _SRTKBANDMFCORE_H_
#define _SRTKBANDMFCORE_H_

#include "ComponentCore.h"

/**
 * This class inherits from CComponentCore, so it contains the code of almost 
 * all the features  of the component.
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>.
 * <h1>Interface Summary</h1>
 *
 * <h2>Methods</h2>
 * <ul>
 *      <li>virtual void initialize(...): it initializes the object</li>
 *      <li>ACS::doubleSeq getStageValues(...): it returns for each feed the fet quantity value 
 *      <li>of a given channel (if) and stage</li>
 *      <li>void setMode(): it allows to change the operating mode of the receiver</li>
 *      <li>void updateVdLNAControls(): it reads and updates from the LNA control board the 
 *      drain voltage values of the transistors</li>
 *      <li>void updateIdLNAControls(): it reads and updates from the LNA control board the 
 *      drain current values of the transistors</li>
 *      <li>void updateVgLNAControls(): it reads and updates from the LNA control board the 
 *      gate voltage values of the transistors</li>
 *      <li>void setLBandColdLoadPath(): it sets the L band RF path to the cold load</li>
 *      <li>void setPBandColdLoadPath(): it sets the P band RF path to the cold load</li>
 *      <li>void setLBandSkyPath(): it sets the L band RF path to the sky</li>
 *      <li>void setPBandSkyPath(): it sets the P band RF path to the sky</li>
 *
 * <h2>Methods inherited from the class CComponentCore</h2>
 * <ul>
 *      <li>void initialize(...)</li>
 *      <li>virtual CConfiguration const * const execute()</li>
 *      <li>virtual void cleanup()</li>
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
class SRTLPBandCore : public CComponentCore {

public:

    SRTLPBandCore();
    virtual ~SRTLPBandCore();


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

 
    /**
     * This method is called in order to set the L band filter.
     * @param filter_id the ID of the filter:
     *     ID 1 -> all band filter, 1300-1800 no filter
     *     ID 2 -> 1320-1780 MHz
     *     ID 3 -> 1350-1450 MHz (VLBI)
     *     ID 4 -> 1300-1800 MHz (band-pass)
     *     ID 5 -> 1625-1715 MHz (VLBI)
     * @throw ReceiversErrors::NoRemoteControlErrorExImpl,
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void setLBandFilter(long filter_id) throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /**
     * This method is called in order to se the P band filter.
     * @param filter_id the ID of the filter:
     *     ID 1 -> all band filter, 305-410 no filter
     *     ID 2 -> 310-350 MHz
     *     ID 3 -> 305-410 MHz (band-pass filter)
     * @throw ReceiversErrors::NoRemoteControlErrorExImpl,
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void setPBandFilter(long filter_id) throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /**
     * This method is called in order to set the L band polarization.
     * @param polarization "L" for Linear, "C" for Circular
     * @throw ReceiversErrors::NoRemoteControlErrorExImpl,
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */ 
    void setLBandPolarization(const char * polarization) throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /**
     * This method is called in order to set the L band polarization.
     * @param polarization "L" for Linear, "C" for Circular
     * @throw ReceiversErrors::NoRemoteControlErrorExImpl,
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */ 
    void setPBandPolarization(const char * polarization) throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /**
     * It sets the L band RF path to the cold load
     * @throw ReceiversErrors::NoRemoteControlErrorExImpl
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void setLBandColdLoadPath() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /**
     * It sets the P band RF path to the cold load
     * @throw ReceiversErrors::NoRemoteControlErrorExImpl
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void setPBandColdLoadPath() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /**
     * It sets the L band RF path to the sky
     * @throw ReceiversErrors::NoRemoteControlErrorExImpl
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void setLBandSkyPath() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /**
     * It sets the P band RF path to the sky
     * @throw ReceiversErrors::NoRemoteControlErrorExImpl
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void setPBandSkyPath() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
    );

private:

    std::vector<IRA::ReceiverControl::StageValues> m_vdStageValues;
    std::vector<IRA::ReceiverControl::StageValues> m_idStageValues;
    std::vector<IRA::ReceiverControl::StageValues> m_vgStageValues;
};


#endif
