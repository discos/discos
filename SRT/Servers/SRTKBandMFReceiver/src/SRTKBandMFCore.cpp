#include "SRTKBandMFCore.h"

#define NUMBER_OF_STAGES 5 // Amplification stages

SRTKBandMFCore::SRTKBandMFCore() {}

SRTKBandMFCore::~SRTKBandMFCore() {}


void SRTKBandMFCore::initialize(maci::ContainerServices* services)
{
    m_vdStageValues = std::vector<IRA::ReceiverControl::StageValues>(NUMBER_OF_STAGES);
    m_idStageValues = std::vector<IRA::ReceiverControl::StageValues>(NUMBER_OF_STAGES);
    m_vgStageValues = std::vector<IRA::ReceiverControl::StageValues>(NUMBER_OF_STAGES);

    CComponentCore::initialize(services);
}


ACS::doubleSeq SRTKBandMFCore::getStageValues(const IRA::ReceiverControl::FetValue& control, DWORD ifs, DWORD stage)
{
    baci::ThreadSyncGuard guard(&m_mutex);

    ACS::doubleSeq values;
    values.length(getFeeds());
    for(size_t i=0; i<getFeeds(); i++)
        values[i] = 0.0;
    if (ifs >= m_configuration.getIFs() || stage > NUMBER_OF_STAGES || stage < 1)
        return values;

    // Left Channel
    if(m_polarization[ifs] == (long)Receivers::RCV_LEFT) {
        if (control == IRA::ReceiverControl::DRAIN_VOLTAGE) {
            for(size_t i=0; i<getFeeds(); i++)
                values[i] = (m_vdStageValues[stage-1]).left_channel[i];
        }
        else {
            if (control == IRA::ReceiverControl::DRAIN_CURRENT) {
                for(size_t i=0; i<getFeeds(); i++)
                    values[i] = (m_idStageValues[stage-1]).left_channel[i];
            }
            else {
                for(size_t i=0; i<getFeeds(); i++)
                    values[i] = (m_vgStageValues[stage-1]).left_channel[i];
            }
        }
    }

    // Right Channel
    if (m_polarization[ifs] == (long)Receivers::RCV_RIGHT) {
        if (control==IRA::ReceiverControl::DRAIN_VOLTAGE)
            for(size_t i=0; i<getFeeds(); i++)
                values[i] = (m_vdStageValues[stage-1]).right_channel[i];
        else 
            if (control == IRA::ReceiverControl::DRAIN_CURRENT) 
                for(size_t i=0; i<getFeeds(); i++)
                    values[i] = (m_idStageValues[stage-1]).right_channel[i];
            else 
                for(size_t i=0; i<getFeeds(); i++)
                    values[i] = (m_vgStageValues[stage-1]).right_channel[i];
    }

    return values;
}


void SRTKBandMFCore::setMode(const char * mode) throw (
        ReceiversErrors::ModeErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,
        ComponentErrors::CORBAProblemExImpl,
        ReceiversErrors::LocalOscillatorErrorExImpl
        )
{
    baci::ThreadSyncGuard guard(&m_mutex);
    m_setupMode = ""; // If we don't reach the end of the method then the mode will be unknown
    IRA::CString cmdMode(mode);
	cmdMode.MakeUpper();

    _EXCPT(ReceiversErrors::ModeErrorExImpl,impl,"CConfiguration::setMode()");

    // Set the operating mode to the board
    if(cmdMode == "SINGLEDISH")
            m_control->setSingleDishMode();
    else
        if(cmdMode == "VLBI")
            m_control->setVLBIMode();
        else 
            throw impl; // If the mode is not supported, raise an exception

    m_configuration.setMode(cmdMode);

    for (WORD i=0;i<m_configuration.getIFs();i++) {
        m_startFreq[i]=m_configuration.getIFMin()[i];
        m_bandwidth[i]=m_configuration.getIFBandwidth()[i];
        m_polarization[i]=(long)m_configuration.getPolarizations()[i];
    }
    // The set the default LO for the default LO for the selected mode.....
    ACS::doubleSeq lo;
    lo.length(m_configuration.getIFs());
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        lo[i]=m_configuration.getDefaultLO()[i];
    }
    // setLO throws:
    //     ComponentErrors::ValidationErrorExImpl,
    //     ComponentErrors::ValueOutofRangeExImpl,
    //     ComponentErrors::CouldntGetComponentExImpl,
    //     ComponentErrors::CORBAProblemExImpl,
    //     ReceiversErrors::LocalOscillatorErrorExImpl
    setLO(lo); 

    // Verify the m_setupMode is the same mode active on the board
    if((cmdMode == "SINGLEDISH" && !m_control->isSingleDishModeOn()) || (cmdMode == "VLBI" && !m_control->isVLBIModeOn())) {
        m_setupMode = ""; // If m_setupMode doesn't match the mode active on the board, then set un unknown mode
        throw impl;
    }

    m_setupMode = cmdMode;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setMode()",(LM_NOTICE,"RECEIVER_MODE %s",mode));
}


void SRTKBandMFCore::updateVdLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        for(size_t i=0; i<NUMBER_OF_STAGES; i++)
            m_vdStageValues[i] = m_control->stageValues(IRA::ReceiverControl::DRAIN_VOLTAGE, i+1, SRTKBandMFCore::voltageConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl, "SRTKBandMFCore::updateVdLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}


void SRTKBandMFCore::updateIdLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        for(size_t i=0; i<NUMBER_OF_STAGES; i++)
            m_idStageValues[i] = m_control->stageValues(IRA::ReceiverControl::DRAIN_CURRENT, i+1, SRTKBandMFCore::currentConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl, impl, "SRTKBandMFCore::updateIdLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}


void SRTKBandMFCore::updateVgLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        for(size_t i=0; i<NUMBER_OF_STAGES; i++)
            m_vgStageValues[i] = m_control->stageValues(IRA::ReceiverControl::GATE_VOLTAGE, i+1, SRTKBandMFCore::voltageConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl, impl, "SRTKBandMFCore::updateVgLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

