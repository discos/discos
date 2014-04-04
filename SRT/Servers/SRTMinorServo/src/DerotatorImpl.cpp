/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <new>
#include <baciDB.h>
#include <ManagmentDefinitionsS.h>
#include "DerotatorImpl.h"
#include "sensorDevIO.h"
#include "icdDevIO.h"
#include "macros.def"

DerotatorImpl::DerotatorImpl(
        const ACE_CString &CompName,maci::ContainerServices *containerServices
        ) : 
    CharacteristicComponentImpl(CompName,containerServices),
    m_sensor_position(this),
    m_icd_position(this),
    m_icd_verbose_status(this),
    m_icd_summary_status(this)
{   
    AUTO_TRACE("DerotatorImpl::DerotatorImpl()");
}


DerotatorImpl::~DerotatorImpl() {
    AUTO_TRACE("DerotatorImpl::~DerotatorImpl()");
}


void DerotatorImpl::getIcdTargetPosition(CORBA::Double_out icd_target_position) {
    AUTO_TRACE("Derotatormpl::getIcdTargetPosition()");
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    icd_target_position = socket->getTargetPosition();
}


void DerotatorImpl::getIcdURSPosition(CORBA::Double_out icd_urs_position) {
    AUTO_TRACE("Derotatormpl::getIcdURSPosition()");
    CSecAreaResourceWrapper<icdSocket> socket = m_icdLink->Get();
    icd_urs_position = socket->getURSPosition();
}


void DerotatorImpl::getSensorURSPosition(CORBA::Double_out sensor_urs_position) {
    AUTO_TRACE("Derotatormpl::getSensorURSPosition()");
    CSecAreaResourceWrapper<sensorSocket> socket = m_sensorLink->Get();
    sensor_urs_position = socket->getURSPosition();
}


void DerotatorImpl::initialize() throw (ComponentErrors::CDBAccessExImpl, ACSErr::ACSbaseExImpl)
{
    CString PS_IP;
    DWORD PS_PORT;
    DWORD PS_TIMEO;
    long PS_REFERENCE;
    double PS_CF;
    sensorSocket *sensor_socket = NULL;

    CString ICD_IP;
    DWORD ICD_PORT;
    DWORD ICD_TIMEO;
    long ICD_REFERENCE;
    double ICD_CF;
    double ICD_MAX_VALUE;
    double ICD_MIN_VALUE;
    double ICD_POSITION_EXPIRE_TIME;
    icdSocket *icd_socket = NULL;

    AUTO_TRACE("DerotatorImpl::initialize()");

    //----  Sensor Network Parameters ----//
    
    try {
        if(!CIRATools::getDBValue(getContainerServices(), "PS_IP", PS_IP)) {
            ACS_SHORT_LOG((
                        LM_ERROR, 
                        "Error getting PS_IP from CDB. Actual PS_IP: %s", 
                        (const char*)PS_IP
                        ));
            ACS_LOG(
                    LM_FULL_INFO, 
                    "DerotatorImpl::initialize", 
                    (LM_INFO, "Error reading the PS_IP form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "DerotatorImpl::initialize(), I can't read PS_IP from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(getContainerServices(), "PS_PORT", PS_PORT)) {
            ACS_LOG(
                    LM_FULL_INFO, 
                    "DerotatorImpl::initialize", 
                    (LM_INFO, "Error reading the PS_PORT attribute form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "DerotatorImpl::initialize(), I can't read PS_PORT from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(getContainerServices(), "PS_TIMEO", PS_TIMEO)) {
            ACS_LOG(
                    LM_FULL_INFO, 
                    "DerotatorImpl::initialize()", 
                    (LM_INFO, "Error reading the PS_TIMEO attribute form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "DerotatorImpl::initialize(), I can't read PS_TIMEO from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(
                    getContainerServices(), 
                    "PS_REFERENCE", 
                    PS_REFERENCE
                    )
                ) {
            ACS_LOG(LM_FULL_INFO, "DerotatorImpl::initialize()", 
                    (LM_INFO, "Error reading the PS_REFERENCE form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "DerotatorImpl:initialize(), I can't read PS_REFERENCE from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(getContainerServices(), "PS_CF", PS_CF)) {
            ACS_LOG(LM_FULL_INFO, "DerotatorImpl::initialize()", 
                    (LM_INFO, "Error reading the PS_CF form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "DerotatorImpl:initialize(), I can't read PS_CF from CDB");
            throw exImpl;
        }

        if(!CIRATools::getDBValue(getContainerServices(), "ICD_REFERENCE", ICD_REFERENCE)) {
            ACS_LOG(
                    LM_FULL_INFO, 
                    "DerotatorImpl::initialize", 
                    (LM_INFO, "Error reading the ICD_REFERENCE form CDB")
                    );
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                    "DerotatorImpl::initialize(), I can't read ICD_REFERENCE from CDB");
            throw exImpl;
        }

        try {
            sensor_socket = new sensorSocket(
                    PS_IP, 
                    PS_PORT, 
                    PS_TIMEO, 
                    PS_CF, 
                    PS_REFERENCE,
                    ICD_REFERENCE
                    );
            m_sensorLink = new CSecureArea<sensorSocket>(sensor_socket);
        }
        catch (std::bad_alloc &ex) {
            _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "DerotatorImpl::initialize()");
            throw dummy;
        }

        sensor_socket->Init();

    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO, "DerotatorImpl::initialize()", 
                (LM_INFO, "Error creating sensor socket")
                );
    }


    //---- ICD Network Parameters ----//

    if(!CIRATools::getDBValue(getContainerServices(), "ICD_IP", ICD_IP)) {
        ACS_SHORT_LOG((
                    LM_ERROR, 
                    "Error getting ICD_IP from CDB. Actual ICD_IP: %s", 
                    (const char*)ICD_IP
                    ));
        ACS_LOG(
                LM_FULL_INFO, 
                "DerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ICD_IP form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "DerotatorImpl::initialize(), I can't read ICD_IP from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ICD_PORT", ICD_PORT)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "DerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ICD_PORT form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "DerotatorImpl::initialize(), I can't read ICD_PORT from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ICD_TIMEO", ICD_TIMEO)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "DerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ICD_TIMEO form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "DerotatorImpl::initialize(), I can't read ICD_TIMEO from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ICD_CF", ICD_CF)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "DerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ICD_CF form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "DerotatorImpl::initialize(), I can't read ICD_CF from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ICD_REFERENCE", ICD_REFERENCE)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "DerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ICD_REFERENCE form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "DerotatorImpl::initialize(), I can't read ICD_REFERENCE from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ICD_MAX_VALUE", ICD_MAX_VALUE)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "DerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ICD_MAX_VALUE form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "DerotatorImpl::initialize(), I can't read ICD_MAX_VALUE from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ICD_MIN_VALUE", ICD_MIN_VALUE)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "DerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ICD_MIN_VALUE form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "DerotatorImpl::initialize(), I can't read ICD_MIN_VALUE from CDB");
        throw exImpl;
    }

    if(!CIRATools::getDBValue(getContainerServices(), "ICD_POSITION_EXPIRE_TIME", ICD_POSITION_EXPIRE_TIME)) {
        ACS_LOG(
                LM_FULL_INFO, 
                "DerotatorImpl::initialize", 
                (LM_INFO, "Error reading the ICD_POSITION_EXPIRE_TIME form CDB")
                );
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, 
                "DerotatorImpl::initialize(), I can't read ICD_POSITION_EXPIRE_TIME from CDB");
        throw exImpl;
    }


    try {
        ACS_SHORT_LOG((LM_INFO, "In try"));
        icd_socket = new icdSocket(
                ICD_IP, 
                ICD_PORT, 
                ICD_TIMEO,
                ICD_CF,
                ICD_REFERENCE,
                ICD_MAX_VALUE,
                ICD_MIN_VALUE,
                ICD_POSITION_EXPIRE_TIME
                );
        m_icdLink = new CSecureArea<icdSocket>(icd_socket);
    }
    catch (std::bad_alloc &ex) {
        printf("MemoryAllocationExImpl");
        ACS_SHORT_LOG((LM_INFO, "MemoryAllocationExImpl"));
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, 
                "DerotatorImpl::initialize(), MemoryAllocationExImpl");
        throw dummy;
    }

    icd_socket->Init();
    icd_socket->driveEnable();
}

void DerotatorImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("DerotatorImpl::execute()");

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
                "DerotatorImpl::initialize(); Exception creating ROdouble (sensor position)"
                );
        throw dummy;
    }

    try {       
        m_icd_position = new RWdouble(getContainerServices()->getName() + ":icd_position", 
                getComponent(), new icdDevIO<CORBA::Double>(m_icdLink, icdDevIO<CORBA::Double>::POSITION), true);
        m_icd_verbose_status = new ROpattern(getContainerServices()->getName() + ":icd_verbose_status", 
                getComponent(), new icdDevIO<ACS::pattern>(m_icdLink, icdDevIO<ACS::pattern>::VERBOSE_STATUS), true);
        m_icd_summary_status = new ROpattern(getContainerServices()->getName() + ":icd_summary_status", 
                getComponent(), new icdDevIO<ACS::pattern>(m_icdLink, icdDevIO<ACS::pattern>::SUMMARY_STATUS), true);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "DerotatorImpl::execute() |ICD|");
        throw dummy;
    }
}


void DerotatorImpl::cleanUp() {
    AUTO_TRACE("DerotatorImpl::cleanUp()");
    stopPropertiesMonitoring();

    // the protected object is destoyed by the secure area destructor
    if (m_sensorLink) {
        delete m_sensorLink;
        m_sensorLink=NULL;
    }
    ACS_LOG(LM_FULL_INFO,"DerotatorImpl::cleanUp()",(LM_INFO,"sensor_socket::SOCKET_CLOSED"));
    CharacteristicComponentImpl::cleanUp(); 

    if (m_icdLink) {
        delete m_icdLink;
        m_icdLink=NULL;
    }
    ACS_LOG(LM_FULL_INFO,"DerotatorImpl::cleanUp()",(LM_INFO,"icd_socket::SOCKET_CLOSED"));
    CharacteristicComponentImpl::cleanUp(); 
}


void DerotatorImpl::aboutToAbort()
{
    AUTO_TRACE("DerotatorImpl::aboutToAbort()");

    if (m_sensorLink) {
        delete m_sensorLink;
        m_sensorLink=NULL;
    }

    if (m_icdLink) {
        delete m_icdLink;
        m_icdLink=NULL;
    }
}



GET_PROPERTY_REFERENCE(DerotatorImpl, ACS::ROdouble, m_sensor_position, sensor_position);
GET_PROPERTY_REFERENCE(DerotatorImpl, ACS::RWdouble, m_icd_position, icd_position);
GET_PROPERTY_REFERENCE(DerotatorImpl, ACS::ROpattern, m_icd_verbose_status, icd_verbose_status);
GET_PROPERTY_REFERENCE(DerotatorImpl, ACS::ROpattern, m_icd_summary_status, icd_summary_status);


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(DerotatorImpl)
