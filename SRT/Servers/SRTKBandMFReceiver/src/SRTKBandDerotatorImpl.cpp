/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
\*******************************************************************************/

#include <new>
#include <baciDB.h>
#include <ManagmentDefinitionsS.h>
#include "SRTKBandDerotatorImpl.h"
#include "sensorDevIO.h"
#include "icdDevIO.h"
#include "macros.def"

SRTKBandDerotatorImpl::SRTKBandDerotatorImpl(
        const ACE_CString &CompName,maci::ContainerServices *containerServices
        ) : 
    CharacteristicComponentImpl(CompName,containerServices),
    m_sensor_position(this),
    m_actPosition(this),
    m_cmdPosition(this),
    m_positionDiff(this),
    m_tracking(this),
    m_icd_verbose_status(this),
    m_status(this)
{   
    AUTO_TRACE("SRTKBandDerotatorImpl::SRTKBandDerotatorImpl()");
}


SRTKBandDerotatorImpl::~SRTKBandDerotatorImpl() {
    AUTO_TRACE("SRTKBandDerotatorImpl::~SRTKBandDerotatorImpl()");
}


void SRTKBandDerotatorImpl::initialize() throw (ComponentErrors::CDBAccessExImpl, ACSErr::ACSbaseExImpl)
{

    SensorIP = "";
    SensorPort = 0;
    SensorTimeout = 0;
    SensorZeroReference = 0;
    SensorConversionFactor = 1;
    sensorSocket *sensor_socket = NULL;

    IP = "";
    Port = 0;
    Timeout = 0;
    ZeroReference = 0;
    ConversionFactor = 1;
    MaxValue = 0;
    MinValue = 0;
    PositionExpireTime = 0;

    icdSocket *icd_socket = NULL;

    AUTO_TRACE("SRTKBandDerotatorImpl::initialize()");

    //----  Sensor Network Parameters ----//
    
    try {
        if(!CIRATools::getDBValue(getContainerServices(), "SensorIP", SensorIP)) {
            ACS_SHORT_LOG((
                        LM_ERROR, 
                        "Error getting SensorIP from CDB. Actual SensorIP: %s", 
                        (const char*)SensorIP
                        ));
            ACS_LOG(
                    LM_FULL_INFO, 
                    "SRTKBandDerotatorImpl::initialize", 
                    (LM_INFO, "Error reading the SensorIP form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "SRTKBandDerotatorImpl::initialize(), I can't read SensorIP from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(getContainerServices(), "SensorPort", SensorPort)) {
            ACS_LOG(
                    LM_FULL_INFO, 
                    "SRTKBandDerotatorImpl::initialize", 
                    (LM_INFO, "Error reading the SensorPort attribute form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "SRTKBandDerotatorImpl::initialize(), I can't read SensorPort from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(getContainerServices(), "SensorTimeout", SensorTimeout)) {
            ACS_LOG(
                    LM_FULL_INFO, 
                    "SRTKBandDerotatorImpl::initialize()", 
                    (LM_INFO, "Error reading the SensorTimeout attribute form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "SRTKBandDerotatorImpl::initialize(), I can't read SensorTimeout from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(
                    getContainerServices(), 
                    "SensorZeroReference", 
                    SensorZeroReference
                    )
                ) {
            ACS_LOG(LM_FULL_INFO, "SRTKBandDerotatorImpl::initialize()", 
                    (LM_INFO, "Error reading the SensorZeroReference form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "SRTKBandDerotatorImpl:initialize(), I can't read SensorZeroReference from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(getContainerServices(), "SensorConversionFactor", SensorConversionFactor)) {
            ACS_LOG(LM_FULL_INFO, "SRTKBandDerotatorImpl::initialize()", 
                    (LM_INFO, "Error reading the SensorConversionFactor form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "SRTKBandDerotatorImpl:initialize(), I can't read SensorConversionFactor from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(getContainerServices(), "ZeroReference", ZeroReference)) {
            ACS_LOG(
                    LM_FULL_INFO, 
                    "SRTKBandDerotatorImpl::initialize", 
                    (LM_INFO, "Error reading the Reference form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "SRTKBandDerotatorImpl::initialize(), I can't read Reference from CDB");
            throw exImpl;
        }

        try {
            sensor_socket = new sensorSocket(
                    SensorIP, 
                    SensorPort, 
                    SensorTimeout, 
                    SensorConversionFactor, 
                    SensorZeroReference,
                    ZeroReference
                    );

            m_sensorLink = new CSecureArea<sensorSocket>(sensor_socket);
        }
        catch (std::bad_alloc &ex) {
            _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "SRTKBandDerotatorImpl::initialize()");
            throw dummy;
        }

        sensor_socket->Init();

    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO, "SRTKBandDerotatorImpl::initialize()", 
                (LM_INFO, "Error creating sensor socket")
                );
    }


    //---- ICD Network Parameters ----//

    if(!CIRATools::getDBValue(getContainerServices(), "IP", IP)) {
        ACS_SHORT_LOG((
                    LM_ERROR, 
                    "Error getting IP from CDB. Actual IP: %s", 
                    (const char*)IP
                    ));
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the IP form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read IP from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "Port", Port)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the Port form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read Port from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "Timeout", Timeout)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the Timeout form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read Timeout from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ConversionFactor", ConversionFactor)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ConversionFactor form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read ConversionFactor from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ZeroReference", ZeroReference)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the Reference form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read Reference from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "MaxValue", MaxValue)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the MaxValue form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read MaxValue from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "MinValue", MinValue)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the MinValue form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read MinValue from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "TrackingDelta", TrackingDelta)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the TrackingDelta form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read TrackingDelta from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "PositionExpireTime", PositionExpireTime)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "SRTKBandDerotatorImpl::initialize", 
                (LM_INFO, "Error reading the PositionExpireTime form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "SRTKBandDerotatorImpl::initialize(), I can't read PositionExpireTime from CDB");
        throw exImpl;
    }


    try {
        ACS_SHORT_LOG((LM_INFO, "In try"));
        icd_socket = new icdSocket(
                IP, 
                Port, 
                Timeout,
                ConversionFactor,
                ZeroReference,
                MaxValue,
                MinValue,
                TrackingDelta,
                PositionExpireTime
                );
        m_icdLink = new CSecureArea<icdSocket>(icd_socket);
    }
    catch (std::bad_alloc &ex) {
        printf("MemoryAllocationExImpl");
        ACS_SHORT_LOG((LM_INFO, "MemoryAllocationExImpl"));
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, 
                "SRTKBandDerotatorImpl::initialize(), MemoryAllocationExImpl");
        throw dummy;
    }

    icd_socket->Init();
    icd_socket->driveEnable();
}

void SRTKBandDerotatorImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("SRTKBandDerotatorImpl::execute()");

    try {       
        m_sensor_position = new ROdouble(
                getContainerServices()->getName() + ":sensor_position", 
                getComponent(),
                new sensorDevIO<CORBA::Double>(
                    m_sensorLink, 
                    sensorDevIO<CORBA::Double>::POSITION
                    ),
                true
                );
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(
                ComponentErrors::MemoryAllocationExImpl,
                dummy, 
                "SRTKBandDerotatorImpl::initialize(); Exception creating ROdouble (sensor position)"
                );
        throw dummy;
    }

    try {       
        m_actPosition = new ROdouble(
                getContainerServices()->getName() + ":actPosition", 
                getComponent(), 
                new icdDevIO<CORBA::Double>(m_icdLink, icdDevIO<CORBA::Double>::ACT_POSITION), 
                true
        );
        m_cmdPosition = new RWdouble(
                getContainerServices()->getName() + ":cmdPosition", 
                getComponent(), 
                new icdDevIO<CORBA::Double>(m_icdLink, icdDevIO<CORBA::Double>::CMD_POSITION), 
                true
        );
        m_positionDiff = new ROdouble(
                getContainerServices()->getName() + ":positionDiff", 
                getComponent(), 
                new icdDevIO<CORBA::Double>(m_icdLink, icdDevIO<CORBA::Double>::POSITION_DIFF), 
                true
        );
        
        m_tracking = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>
                (getContainerServices()->getName() + ":tracking", getComponent(),
                new icdDevIO<Management::TBoolean>(m_icdLink, icdDevIO<Management::TBoolean>::TRACKING), 
                true
        );

        m_icd_verbose_status = new ROpattern(getContainerServices()->getName() + ":icd_verbose_status", 
                getComponent(), new icdDevIO<ACS::pattern>(m_icdLink, icdDevIO<ACS::pattern>::VERBOSE_STATUS), true);

        m_status = new ROpattern(getContainerServices()->getName() + ":status", 
                getComponent(), new icdDevIO<ACS::pattern>(m_icdLink, icdDevIO<ACS::pattern>::STATUS), true);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "SRTKBandDerotatorImpl::execute() |ICD|");
        throw dummy;
    }
}


void SRTKBandDerotatorImpl::cleanUp() {
    AUTO_TRACE("SRTKBandDerotatorImpl::cleanUp()");
    stopPropertiesMonitoring();

    // the protected object is destoyed by the secure area destructor
    if (m_sensorLink) {
        delete m_sensorLink;
        m_sensorLink=NULL;
    }
    ACS_LOG(LM_FULL_INFO,"SRTKBandDerotatorImpl::cleanUp()",(LM_INFO,"sensor_socket::SOCKET_CLOSED"));
    CharacteristicComponentImpl::cleanUp(); 

    if (m_icdLink) {
        delete m_icdLink;
        m_icdLink=NULL;
    }
    ACS_LOG(LM_FULL_INFO,"SRTKBandDerotatorImpl::cleanUp()",(LM_INFO,"icd_socket::SOCKET_CLOSED"));
    CharacteristicComponentImpl::cleanUp(); 
}


void SRTKBandDerotatorImpl::aboutToAbort()
{
    AUTO_TRACE("SRTKBandDerotatorImpl::aboutToAbort()");

    if (m_sensorLink) {
        delete m_sensorLink;
        m_sensorLink=NULL;
    }

    if (m_icdLink) {
        delete m_icdLink;
        m_icdLink=NULL;
    }
}


void SRTKBandDerotatorImpl::setup() throw (
        CORBA::SystemException,
        DerotatorErrors::ConfigurationErrorEx, 
        ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("Derotatormpl::setup()");
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    try {
        socket->driveEnable();
    }
    catch (ComponentErrors::SocketErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        _EXCPT(
                DerotatorErrors::ConfigurationErrorExImpl,
                impl, 
                "SRTKBandDerotatorImpl::setup(): cannot enable the derotator."
        );
        throw impl.getDerotatorErrorsEx();
    }
    catch (...) {
        _EXCPT(
                ComponentErrors::UnexpectedExImpl, 
                impl, 
                "SRTKBandDerotatorImpl::setup()"
        );
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

void SRTKBandDerotatorImpl::setPosition(double position) throw (
        CORBA::SystemException, 
        ComponentErrors::ComponentErrorsEx, 
        DerotatorErrors::PositioningErrorEx,
        DerotatorErrors::CommunicationErrorEx
    )
{
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    try {
        socket->setPosition(position);
    }
    catch (DerotatorErrors::CommunicationErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }
    catch (DerotatorErrors::PositioningErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }
    catch (...) {
        _EXCPT(
                ComponentErrors::UnexpectedExImpl, 
                impl, 
                "SRTKBandDerotatorImpl::setPosition()"
        );
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


double SRTKBandDerotatorImpl::getActPosition() 
    throw (
            CORBA::SystemException, 
            ComponentErrors::ComponentErrorsEx, 
            DerotatorErrors::CommunicationErrorEx
    )
{
    AUTO_TRACE("Derotatormpl::getActPosition()");
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    try {
        return socket->getActPosition();
    }
    catch (DerotatorErrors::CommunicationErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }
    catch (DerotatorErrors::ValidationErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        _EXCPT(
                DerotatorErrors::ConfigurationErrorExImpl, 
                impl, 
                "SRTKBandDerotatorImpl::getActPosition(): wrong derotator answer."
        );
        throw impl.getDerotatorErrorsEx();
    }
    catch (...) {
        _EXCPT(
                ComponentErrors::UnexpectedExImpl, 
                impl, 
                "SRTKBandDerotatorImpl::setup()"
        );
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


double SRTKBandDerotatorImpl::getCmdPosition() {
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    return socket->getCmdPosition();
}


double SRTKBandDerotatorImpl::getPositionFromHistory(ACS::Time t) {
    AUTO_TRACE("Derotatormpl::getIcdTargetPosition()");
    return 0; // TODO
}


double SRTKBandDerotatorImpl::getMaxLimit() {
    return MaxValue - ZeroReference;
}


double SRTKBandDerotatorImpl::getMinLimit() {
    return MinValue - ZeroReference;
}


bool SRTKBandDerotatorImpl::isReady() {
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    return socket->isReady();
}


bool SRTKBandDerotatorImpl::isSlewing() {
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    return socket->isSlewing();
}


bool SRTKBandDerotatorImpl::isTracking() {
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    return socket->isTracking();
}


double SRTKBandDerotatorImpl::getSensorPosition() {
    AUTO_TRACE("Derotatormpl::getSensorPosition()");
    CSecAreaResourceWrapper<sensorSocket> socket = m_sensorLink->Get();
    return socket->getURSPosition();
}


GET_PROPERTY_REFERENCE(SRTKBandDerotatorImpl, ACS::ROdouble, m_sensor_position, sensor_position);
GET_PROPERTY_REFERENCE(SRTKBandDerotatorImpl, ACS::ROdouble, m_actPosition, actPosition);
GET_PROPERTY_REFERENCE(SRTKBandDerotatorImpl, ACS::RWdouble, m_cmdPosition, cmdPosition);
GET_PROPERTY_REFERENCE(SRTKBandDerotatorImpl, ACS::ROdouble, m_positionDiff, positionDiff);
GET_PROPERTY_REFERENCE(SRTKBandDerotatorImpl, Management::ROTBoolean, m_tracking, tracking);
GET_PROPERTY_REFERENCE(SRTKBandDerotatorImpl, ACS::ROpattern, m_icd_verbose_status, icd_verbose_status);
GET_PROPERTY_REFERENCE(SRTKBandDerotatorImpl, ACS::ROpattern, m_status, status);


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTKBandDerotatorImpl)
