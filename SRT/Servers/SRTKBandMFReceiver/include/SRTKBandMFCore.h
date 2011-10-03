/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Andrea Orlati <orlati@ira.inaf.it>
 *  Marco Buttu <mbuttu@oa-cagliari.inaf.it>
\*******************************************************************************/

#ifndef _SRTKBANDMFCORE_H_
#define _SRTKBANDMFCORE_H_

#include "ComponentCore.h"

/**
 * TODO: a brief description...
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>.
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * Interface Summary:
 * <ul>
 *   <li> ... </li>
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


private:

    std::vector<IRA::ReceiverControl::StageValues> m_vdStageValues;
    std::vector<IRA::ReceiverControl::StageValues> m_idStageValues;
    std::vector<IRA::ReceiverControl::StageValues> m_vgStageValues;
};


#endif
