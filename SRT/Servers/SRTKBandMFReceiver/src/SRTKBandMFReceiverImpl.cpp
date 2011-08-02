/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <new>
#include <IRA>
#include <baciDB.h>
#include <ManagmentDefinitionsS.h>
#include "SRTKBandMFReceiverImpl.h"
#include "ReceiverSocket.h"
#include "macros.def"
#include "RecSingleValueDevIO.h"
#include "RecPdoubleSeqDevIO.h"
#include "RecPlongSeqDevIO.h"


void cdbErrorMessage(CString attribute_name) {
    ACS_SHORT_LOG((LM_ERROR, "CDB Error getting " + attribute_name + " from CDB."));
    ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
            "SRTKBandMFReceiverImpl::initialize(), I can't read " + attribute_name + " from CDB");
    throw exImpl;
}


SRTKBandMFReceiverImpl::SRTKBandMFReceiverImpl(
        const ACE_CString &CompName, maci::ContainerServices *containerServices
        ) : 
    CharacteristicComponentImpl(CompName, containerServices),
    m_vacuum(this),
    m_lowTemperature(this),
    m_highTemperature(this),
    m_status(this),
    m_LO(this),
    m_feeds(this),
    m_IFs(this),
    m_polarization(this),
    m_receiverName(this),
    m_mode(this)
{   
    AUTO_TRACE("SRTKBandMFReceiverImpl::SRTKBandMFReceiverImpl()");
    m_cdb_ptr = new RCDBParameters;
}


SRTKBandMFReceiverImpl::~SRTKBandMFReceiverImpl() {
    AUTO_TRACE("SRTKBandMFReceiverImpl::~SRTKBandMFReceiverImpl()");
}


void SRTKBandMFReceiverImpl::initialize() throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl)
{

    AUTO_TRACE("SRTKBandMFReceiverImpl::initialize()");

    //----  Retrieve Receiver Attributes ----//

    // Retrive the number_of_feeds parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "number_of_feeds", m_cdb_ptr->NUMBER_OF_FEEDS))
        cdbErrorMessage("number_of_feeds");
    // Retrive the ip parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "ip", m_cdb_ptr->IP))
        cdbErrorMessage("ip");
    // Retrive the port parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "port", m_cdb_ptr->PORT))
        cdbErrorMessage("port");
    // Retrive the timeout parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "timeout", m_cdb_ptr->TIMEOUT))
        cdbErrorMessage("timeout");
    // Retrive the receiver_name parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "receiver_name", m_cdb_ptr->RECEIVER_NAME))
        cdbErrorMessage("receiver_name");

    // for(int i = 0; i < NUMBER_OF_SERVOS; i++) {
    //     m_expire_ptr->timeLastActPos[i] = -100;
    //     m_expire_ptr->actPos[i] = 0;
    // }

    // m_expire_ptr->actPos[m_cdb_ptr->SERVO_ADDRESS].length(m_cdb_ptr->NUMBER_OF_AXIS);

    ReceiverSocket *receiver_socket = NULL;
    try {
        receiver_socket = new ReceiverSocket(m_cdb_ptr);
        m_receiver_link = new CSecureArea<ReceiverSocket>(receiver_socket);
    }
    catch (std::bad_alloc &ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "SRTKBandMFReceiverImpl::initialize()");
        throw dummy;
    }

    receiver_socket->Init();
}


void SRTKBandMFReceiverImpl::execute() 
    throw (ComponentErrors::MemoryAllocationExImpl, ComponentErrors::SocketErrorExImpl)
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::execute()");
    
    try {       
        // Vacuum
        m_vacuum = new ROdouble(
                getContainerServices()->getName() + ":vacuum", 
                getComponent(), 
                new RecSingleValueDevIO<CORBA::Double>(m_receiver_link, RecSingleValueDevIO<CORBA::Double>::VACUUM, m_cdb_ptr), 
                true);
        // Low Temperature
        m_lowTemperature = new ROdouble(
                getContainerServices()->getName() + ":lowTemperature", 
                getComponent(), 
                new RecSingleValueDevIO<CORBA::Double>(m_receiver_link, RecSingleValueDevIO<CORBA::Double>::LOW_TEMPERATURE, m_cdb_ptr), 
                true);
        // High Temperature
        m_highTemperature = new ROdouble(
                getContainerServices()->getName() + ":highTemperature", 
                getComponent(), 
                new RecSingleValueDevIO<CORBA::Double>(m_receiver_link, RecSingleValueDevIO<CORBA::Double>::HIGH_TEMPERATURE, m_cdb_ptr), 
                true);
        // Receiver Status
        m_status = new ROpattern(
                getContainerServices()->getName() + ":status", 
                getComponent(), 
                new RecSingleValueDevIO<ACS::pattern>(m_receiver_link, RecSingleValueDevIO<ACS::pattern>::STATUS, m_cdb_ptr), 
                true);

        // Local Oscillator
        m_LO = new ROdoubleSeq(
                getContainerServices()->getName() + ":LO", 
                getComponent(), 
                new RecPdoubleSeqDevIO(m_receiver_link, RecPdoubleSeqDevIO::LO), 
                true);

        // Polarization
        m_polarization = new ROlongSeq(
                getContainerServices()->getName() + ":polarization", 
                getComponent(), 
                new RecPlongSeqDevIO(m_receiver_link, RecPlongSeqDevIO::POLARIZATION), 
                true);

        // Number of Feeds
        m_feeds = new ROlong(
                getContainerServices()->getName() + ":feeds", 
                getComponent(), 
                new RecSingleValueDevIO<CORBA::Long>(m_receiver_link, RecSingleValueDevIO<CORBA::Long>::FEEDS, m_cdb_ptr), 
                true);
        // Intermediate Frequencies
        m_IFs = new ROlong(
                getContainerServices()->getName() + ":IFs", 
                getComponent(), 
                new RecSingleValueDevIO<CORBA::Long>(m_receiver_link, RecSingleValueDevIO<CORBA::Long>::IFS, m_cdb_ptr), 
                true);
        // Receiver Name
		m_receiverName = new ROstring(getContainerServices()->getName() + ":receiverName", getComponent());
	    ACS::Time time;
		m_receiverName->getDevIO()->write(getComponent()->getName(), time);
    }
    catch (std::bad_alloc& ex) {
        ACS_SHORT_LOG((LM_ERROR, "MemoryAllocationExImpl"));
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "SRTKBandMFReceiverImpl::execute()");
        throw dummy;
    }
}


void SRTKBandMFReceiverImpl::cleanUp() {
    AUTO_TRACE("SRTKBandMFReceiverImpl::cleanUp()");
    stopPropertiesMonitoring();

    // The protected object is destoyed by the secure area destructor
    if (m_receiver_link)
        delete m_receiver_link;

    if (m_cdb_ptr)
        delete m_cdb_ptr;

    CharacteristicComponentImpl::cleanUp(); 
}


void SRTKBandMFReceiverImpl::aboutToAbort()
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::aboutToAbort()");

    if (m_receiver_link)
        delete m_receiver_link;

    if (m_cdb_ptr) 
        delete m_cdb_ptr;

}


void SRTKBandMFReceiverImpl::calOn() throw (ComponentErrors::ComponentErrorsEx) 
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::calOn()");
    // Thread Safe socket connection
    CSecAreaResourceWrapper<ReceiverSocket> thsafe_link = m_receiver_link->Get();
    try {
        thsafe_link->cal_on();
    }
    catch(...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl, "SRTKBandMFReceiverImpl::calOn(): Cannot accomplish receiver calibration");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
    }
}


void SRTKBandMFReceiverImpl::calOff() throw (ComponentErrors::ComponentErrorsEx) 
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::calOff()");
    // Thread Safe socket connection
    CSecAreaResourceWrapper<ReceiverSocket> thsafe_link = m_receiver_link->Get();
    try {
        thsafe_link->cal_off();
    }
    catch(...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl, "SRTKBandMFReceiverImpl::calOn(): Cannot stop the receiver calibration");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
    }
}


void SRTKBandMFReceiverImpl::setLO(const ACS::doubleSeq &lo) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx) 
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::setLO()");
}


ACS::doubleSeq * SRTKBandMFReceiverImpl::getCalibrationMark(
        const ACS::doubleSeq &freqs, 
        const ACS::doubleSeq &bandwidths, 
        const ACS::longSeq &feeds,
        const ACS::longSeq &ifs
        ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::getCalibrationMark()");
	ACS::doubleSeq_var result = new ACS::doubleSeq;
	return result._retn();
}


CORBA::Long SRTKBandMFReceiverImpl::getFeeds(
        ACS::doubleSeq_out X,
        ACS::doubleSeq_out Y,
        ACS::doubleSeq_out power
        ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::getFeeds()");
	long dummy(0);
    return dummy;
}


CORBA::Double SRTKBandMFReceiverImpl::getTaper(
        CORBA::Double freq,
        CORBA::Double bandWidth,
        CORBA::Long feed,
        CORBA::Long ifNumber,
        CORBA::Double_out waveLen
        ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::getTaper()");
	CORBA::Double res(0);
    return res;
}


// unsigned short SRTKBandMFReceiverImpl::number_of_feeds(void)
// {
//     AUTO_TRACE("SRTKBandMFReceiverImpl::number_of_feeds()");
//     return m_cdb_ptr->NUMBER_OF_FEEDS;
// }


void SRTKBandMFReceiverImpl::turnLNAsOn()
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::turnLNAsOn()");
}


void SRTKBandMFReceiverImpl::turnLNAsOff()
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::turnLNAsOff()");
}


void SRTKBandMFReceiverImpl::setMode(const char *mode)
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::setMode()");
    // Thread Safe socket connection
    CSecAreaResourceWrapper<ReceiverSocket> thsafe_link = m_receiver_link->Get();
    try {
        thsafe_link->set_mode(mode);
    }
    catch(...) {
        THROW_EX(ReceiversErrors, ConfigurationEx, "Cannot set the observation modality", true);
    }
}


ACS::ROdoubleSeq_ptr SRTKBandMFReceiverImpl::initialFrequency()
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::initialFrequency()");
    return NULL;
}

ACS::ROdoubleSeq_ptr SRTKBandMFReceiverImpl::bandWidth()
{
    AUTO_TRACE("SRTKBandMFReceiverImpl::bandWidth()");
    return NULL;
}



GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROdouble, m_vacuum, vacuum);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROdouble, m_lowTemperature, lowTemperature);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROdouble, m_highTemperature, highTemperature);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROpattern, m_status, status);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROdoubleSeq, m_LO, LO);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROlong, m_feeds, feeds);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROlong, m_IFs, IFs);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROlongSeq, m_polarization, polarization);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROstring, m_receiverName, receiverName);
GET_PROPERTY_REFERENCE(SRTKBandMFReceiverImpl, ACS::ROstring, m_mode, mode);
 

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTKBandMFReceiverImpl)
