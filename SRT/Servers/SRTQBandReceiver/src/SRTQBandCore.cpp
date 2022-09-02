#include "SRTQBandCore.h"

#define NUMBER_OF_FEEDS 5 // Number of feeds per AD24 port

SRTQBandCore::SRTQBandCore() {
    voltage2mbar=voltage2mbarF;
    voltage2Kelvin=voltage2KelvinF;
    voltage2Celsius=voltage2CelsiusF;
    currentConverter=currentConverterF;
    voltageConverter=voltageConverterF;	
}

SRTQBandCore::~SRTQBandCore() {}


void SRTQBandCore::initialize(maci::ContainerServices* services)
{
    m_vdFeedValues = std::vector<IRA::ReceiverControl::FeedValues>(NUMBER_OF_FEEDS);
    m_idFeedValues = std::vector<IRA::ReceiverControl::FeedValues>(NUMBER_OF_FEEDS);
    m_vgFeedValues = std::vector<IRA::ReceiverControl::FeedValues>(NUMBER_OF_FEEDS);

    CComponentCore::initialize(services);
}

ACS::doubleSeq SRTQBandCore::getFeedValues(const IRA::ReceiverControl::FetValue& control, DWORD ifs, DWORD feed)
{
    baci::ThreadSyncGuard guard(&m_mutex);

    ACS::doubleSeq values;
    values.length(getFeeds());
    cout << "getFeeds() " << getFeeds() << endl;
	 cout << "ifs " << ifs  << endl;
    cout << "feed " << feed << endl;
	 cout << "m_configuration.getIFs() " << m_configuration.getIFs()  << endl;
	 cout << "m_configuration.getFeeds()" << m_configuration.getFeeds() << endl;
    for(size_t i=0; i<getFeeds(); i++)
        values[i] = 0.0;
    if (ifs >= m_configuration.getIFs() || feed > NUMBER_OF_FEEDS || feed < 1)
        return values;
    cout << "m_polarization[ifs] " << m_polarization[ifs] << endl;
	 cout << "Receivers::RCV_LCP " << Receivers::RCV_LCP << endl;
    cout << "control " << control << endl;
    cout << "IRA::ReceiverControl::DRAIN_VOLTAGE " << IRA::ReceiverControl::DRAIN_VOLTAGE << endl;
    cout << "IRA::ReceiverControl::DRAIN_CURRENT " << IRA::ReceiverControl::DRAIN_CURRENT << endl;
        cout << "IRA::ReceiverControl::GATE_VOLTAGE " << IRA::ReceiverControl::GATE_VOLTAGE << endl;
	 cout << "m_vdFeedValues[feed-1].left_channel.size() " << m_vdFeedValues[feed-1].left_channel.size() << endl;
	 cout << "m_idFeedValues[feed-1].left_channel.size() " << m_idFeedValues[feed-1].left_channel.size() << endl;
	 cout << "m_vgFeedeValues[feed-1].left_channel.size() " << m_vgFeedValues[feed-1].left_channel.size() << endl;

	 cout << "m_vdFeedValues[feed-1].right_channel.size() " << m_vdFeedValues[feed-1].right_channel.size() << endl;
	 cout << "m_idFeedValues[feed-1].right_channel.size() " << m_idFeedValues[feed-1].right_channel.size() << endl;
	 cout << "m_vgFeedeValues[feed-1].right_channel.size() " << m_vgFeedValues[feed-1].right_channel.size() << endl;
    
    // Left Channel
    if(m_polarization[ifs] == (long)Receivers::RCV_LCP) {
        if (control == IRA::ReceiverControl::DRAIN_VOLTAGE) {
        		if (getFeeds()>m_vdFeedValues[feed-1].left_channel.size())
    				return values;
            for(size_t i=0; i<getFeeds(); i++) {
            	values[i] = (m_vdFeedValues[feed-1]).left_channel[i];
            }
        }
        else {
            if (control == IRA::ReceiverControl::DRAIN_CURRENT) {
           		if (getFeeds()>m_idFeedValues[feed-1].left_channel.size())
    					return values;
               for(size_t i=0; i<getFeeds(); i++)
               	values[i] = (m_idFeedValues[feed-1]).left_channel[i];
            }
            else {
            	if (getFeeds()>m_vgFeedValues[feed-1].left_channel.size())
    					return values;
               for(size_t i=0; i<getFeeds(); i++)
               	values[i] = (m_vgFeedValues[feed-1]).left_channel[i];
            }
        }
    }
    // Right Channel
    if (m_polarization[ifs] == (long)Receivers::RCV_RCP) {
        if (control==IRA::ReceiverControl::DRAIN_VOLTAGE) {
        		if (getFeeds()>m_vdFeedValues[feed-1].right_channel.size())
    				return values;
            for(size_t i=0;i<getFeeds();i++)
                values[i] = (m_vdFeedValues[feed-1]).right_channel[i];
        }
        else {
            if (control == IRA::ReceiverControl::DRAIN_CURRENT) {
            	if (getFeeds()>m_idFeedValues[feed-1].right_channel.size())
    					return values; 
               for(size_t i=0; i<getFeeds(); i++)
               	values[i] = (m_idFeedValues[feed-1]).right_channel[i];
            }
            else { 
            	if (getFeeds()>m_vgFeedValues[feed-1].right_channel.size())
    					return values;
               for(size_t i=0; i<getFeeds(); i++)
               	values[i] = (m_vgFeedValues[feed-1]).right_channel[i];
            }
       }
    }
    
    for(size_t j = 0; j < getFeeds(); j++){
      cout << "values " << values[j] << endl;
    }    
    return values;
}



void SRTQBandCore::setMode(const char * mode) throw (
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
    cout << "cmdMode " << cmdMode << endl;
    // Set the operating mode to the board
    try {
        if(cmdMode == "SINGLEDISH")
                m_control->setSingleDishMode();
        else
            if(cmdMode == "VLBI")
                m_control->setVLBIMode();
            else 
                throw impl; // If the mode is not supported, raise an exception
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl, "SRTQBandCore::setMode()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
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

    // Verify the m_setupMode is the same mode active on the board
    bool isSingleDishModeOn, isVLBIModeOn;
    try {
        isSingleDishModeOn = m_control->isSingleDishModeOn();
        isVLBIModeOn = m_control->isVLBIModeOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl, "SRTQBandCore::setMode()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    if((cmdMode == "SINGLEDISH" && !isSingleDishModeOn) || (cmdMode == "VLBI" && !isVLBIModeOn)) {
        m_setupMode = ""; // If m_setupMode doesn't match the mode active on the board, then set un unknown mode
        throw impl;
    }

    m_setupMode = cmdMode;
    //Here an error is raised when mode variable is inserted into the log string
    //the string is somehow malformed resulting in impossible XML log message parsing
    //ACS_LOG(LM_FULL_INFO,"CComponentCore::setMode()",(LM_NOTICE,"RECEIVER_MODE %s",mode));
    //Maybe the following line fixes the bug by passing the c_string contained in the IRA::CString
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setMode()",(LM_NOTICE,"RECEIVER_MODE %s",(const char*)cmdMode));
}


void SRTQBandCore::updateVdLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        cout << "Number of feeds per pcb " << NUMBER_OF_FEEDS << endl;
        for(size_t i=0; i<NUMBER_OF_FEEDS; i++)
            m_vdFeedValues[i] = m_control->feedValues(IRA::ReceiverControl::DRAIN_VOLTAGE, i+1, SRTQBandCore::voltageConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl, "SRTQBandCore::updateVdLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}


void SRTQBandCore::updateIdLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        for(size_t i=0; i<NUMBER_OF_FEEDS; i++)
            m_idFeedValues[i] = m_control->feedValues(IRA::ReceiverControl::DRAIN_CURRENT, i+1, SRTQBandCore::currentConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl, impl, "SRTQBandCore::updateIdLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}


void SRTQBandCore::updateVgLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        for(size_t i=0; i<NUMBER_OF_FEEDS; i++)
            m_vgFeedValues[i] = m_control->feedValues(IRA::ReceiverControl::GATE_VOLTAGE, i+1, SRTQBandCore::voltageConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl, impl, "SRTQBandCore::updateVgLNAControls()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

