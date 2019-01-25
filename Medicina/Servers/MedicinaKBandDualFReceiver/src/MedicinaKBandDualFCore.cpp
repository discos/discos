#include "MedicinaKBandDualFCore.h"

#define NUMBER_OF_STAGES 5 // Amplification stages

MedicinaKBandDualFCore::MedicinaKBandDualFCore() {
    voltage2mbar=voltage2mbarF;
    voltage2Kelvin=voltage2KelvinF;
    voltage2Celsius=voltage2CelsiusF;
    currentConverter=currentConverterF;
    voltageConverter=voltageConverterF;
}

MedicinaKBandDualFCore::~MedicinaKBandDualFCore() {}


void MedicinaKBandDualFCore::initialize(maci::ContainerServices* services)
{
    m_vdStageValues = std::vector<IRA::ReceiverControl::StageValues>(NUMBER_OF_STAGES);
    m_idStageValues = std::vector<IRA::ReceiverControl::StageValues>(NUMBER_OF_STAGES);
    m_vgStageValues = std::vector<IRA::ReceiverControl::StageValues>(NUMBER_OF_STAGES); 
    CComponentCore::initialize(services);
}


ACS::doubleSeq MedicinaKBandDualFCore::getStageValues(const IRA::ReceiverControl::FetValue& control, DWORD ifs, DWORD stage)
{
    baci::ThreadSyncGuard guard(&m_mutex);

    ACS::doubleSeq values;
    values.length(getFeeds());
    for(size_t i=0; i<getFeeds(); i++)
        values[i] = 0.0;
    if (ifs >= m_configuration.getIFs() || stage > NUMBER_OF_STAGES || stage < 1)
        return values;   	
    // Left Channel
    if(m_polarization[ifs] == (long)Receivers::RCV_LCP) {
        if (control == IRA::ReceiverControl::DRAIN_VOLTAGE) {
        		if (getFeeds()>m_vdStageValues[stage-1].left_channel.size())
    				return values;
            for(size_t i=0; i<getFeeds(); i++) {
            	values[i] = (m_vdStageValues[stage-1]).left_channel[i];
            }
        }
        else {
            if (control == IRA::ReceiverControl::DRAIN_CURRENT) {
           		if (getFeeds()>m_idStageValues[stage-1].left_channel.size())
    					return values;
               for(size_t i=0; i<getFeeds(); i++)
               	values[i] = (m_idStageValues[stage-1]).left_channel[i];
            }
            else {
            	if (getFeeds()>m_vgStageValues[stage-1].left_channel.size())
    					return values;
               for(size_t i=0; i<getFeeds(); i++)
               	values[i] = (m_vgStageValues[stage-1]).left_channel[i];
            }
        }
    }
    // Right Channel
    if (m_polarization[ifs] == (long)Receivers::RCV_RCP) {
        if (control==IRA::ReceiverControl::DRAIN_VOLTAGE) {
        		if (getFeeds()>m_vdStageValues[stage-1].right_channel.size())
    				return values;
            for(size_t i=0;i<getFeeds();i++)
                values[i] = (m_vdStageValues[stage-1]).right_channel[i];
        }
        else {
            if (control == IRA::ReceiverControl::DRAIN_CURRENT) {
            	if (getFeeds()>m_idStageValues[stage-1].right_channel.size())
    					return values; 
               for(size_t i=0; i<getFeeds(); i++)
               	values[i] = (m_idStageValues[stage-1]).right_channel[i];
            }
            else { 
            	if (getFeeds()>m_vgStageValues[stage-1].right_channel.size())
    					return values;
               for(size_t i=0; i<getFeeds(); i++)
               	values[i] = (m_vgStageValues[stage-1]).right_channel[i];
            }
       }
    }

    return values;
}


void MedicinaKBandDualFCore::setMode(const char * mode) throw (
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
    
	if(cmdMode == "SINGLEDISH") {
		//Correct mode
	}
   else {
   	_EXCPT(ReceiversErrors::ModeErrorExImpl,impl,"MedicinaKBandDualFCore::setMode()"); 
   	throw impl; // If the mode is not supported, raise an exception
   }

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
	
	m_setupMode = cmdMode;
   //Here an error is raised when mode variable is inserted into the log string
   //the string is somehow malformed resulting in impossible XML log message parsing
   //ACS_LOG(LM_FULL_INFO,"CComponentCore::setMode()",(LM_NOTICE,"RECEIVER_MODE %s",mode));
   //Maybe the following line fixes the bug by passing the c_string contained in the IRA::CString
   ACS_LOG(LM_FULL_INFO,"CComponentCore::setMode()",(LM_NOTICE,"RECEIVER_MODE %s",(const char*)cmdMode));
}


void MedicinaKBandDualFCore::updateVertexTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	 // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_envTemperature=m_control->vertexTemperature(voltage2Celsius,
				MCB_CMD_DATA_TYPE_F32,     
            MCB_PORT_TYPE_AD24,       
            MCB_PORT_NUMBER_00_07,
            5        
        );
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"MedicinaKBandDualFCore::updateVertexTemperature()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

void MedicinaKBandDualFCore::updateCryoCoolHead() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	m_cryoCoolHead=0.0;
}


void MedicinaKBandDualFCore::updateCryoCoolHeadWin() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	m_cryoCoolHeadWin=0.0;
}


void MedicinaKBandDualFCore::updateCryoLNA() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoLNA=m_control->cryoTemperature(1,voltage2Kelvin);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"MedicinaKBandDualFCore::updateCryoLNA()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void MedicinaKBandDualFCore::updateCryoLNAWin() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoLNAWin=m_control->cryoTemperature(0,voltage2Kelvin);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"MedicinaKBandDualFCore::updateCryoLNAWin()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void MedicinaKBandDualFCore::updateVdLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        for(size_t i=0; i<NUMBER_OF_STAGES; i++)
            m_vdStageValues[i] = m_control->stageValues(IRA::ReceiverControl::DRAIN_VOLTAGE,i+1,voltageConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl, "MedicinaKBandDualFCore::updateVdLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}


void MedicinaKBandDualFCore::updateIdLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        for(size_t i=0; i<NUMBER_OF_STAGES; i++)
            m_idStageValues[i] = m_control->stageValues(IRA::ReceiverControl::DRAIN_CURRENT, i+1,MedicinaKBandDualFCore::currentConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl, impl, "MedicinaKBandDualFCore::updateIdLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}


void MedicinaKBandDualFCore::updateVgLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        for(size_t i=0; i<NUMBER_OF_STAGES; i++)
            m_vgStageValues[i] = m_control->stageValues(IRA::ReceiverControl::GATE_VOLTAGE, i+1,MedicinaKBandDualFCore::voltageConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl, impl, "MedicinaKBandDualFCore::updateVgLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

