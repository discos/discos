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
#include "WPServoImpl.h"
#include "PdoubleSeqDevIO.h"
#include "WPStatusDevIO.h"
#include "macros.def"
#include <pthread.h>
#include <bitset>

// Initialize the thread pointers. We want to instance them only once
ThreadParameters WPServoImpl::m_thread_params;
RequestDispatcher* WPServoImpl::m_dispatcher_ptr = NULL;
SocketListener* WPServoImpl::m_listener_ptr = NULL;
WPStatusUpdater* WPServoImpl::m_status_ptr = NULL;
ExpireTime WPServoImpl::m_expire;
map<int, WPServoTalker *> WPServoImpl::m_talkers;
map<int, unsigned long *> WPServoImpl::m_status_map;
map<int, bool> WPServoImpl::m_status_thread_en;
map<int, bool> WPServoImpl::m_stow_state;
map<int, vector<double> > WPServoImpl::m_park_positions;
CSecureArea<unsigned short> *WPServoImpl::m_instance_counter = NULL;
CSecureArea< map<int, vector<PositionItem> > >* WPServoImpl::m_cmdPos_list = \
    new CSecureArea< map<int, vector<PositionItem> > >(new map<int, vector<PositionItem> >);
CSecureArea< map<int, vector<PositionItem> > >* WPServoImpl::m_actPos_list = \
    new CSecureArea< map<int, vector<PositionItem> > >(new map<int, vector<PositionItem> >);

static pthread_mutex_t const_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t destr_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t listener_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t scheduler_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t offset_mutex = PTHREAD_MUTEX_INITIALIZER;

WPServoImpl::WPServoImpl(
        const ACE_CString &CompName, maci::ContainerServices *containerServices
        ) : 
    CharacteristicComponentImpl(CompName, containerServices),
    m_actPos(this),
    m_plainActPos(this),
    m_cmdPos(this),
    m_posDiff(this),
    m_actElongation(this),
    m_virtualActElongation(this),
    m_engTemperature(this),
    m_counturingErr(this),
    m_torquePerc(this),
    m_engCurrent(this),
    m_engVoltage(this),
    m_driTemperature(this),
    m_utilizationPerc(this),
    m_dcTemperature(this),
    m_driverStatus(this),
    m_errorCode(this),
    m_status(this)
{   
    AUTO_TRACE("WPServoImpl::WPServoImpl()");
    try {
        pthread_mutex_lock(&const_mutex); 
        if(m_instance_counter == NULL) {
            m_instance_counter = new CSecureArea<unsigned short>(new unsigned short);
            CSecAreaResourceWrapper<unsigned short> secure_ptr = m_instance_counter->Get();
            *secure_ptr = 1; // First instance
            secure_ptr.Release();
        }
        else {
            CSecAreaResourceWrapper<unsigned short> secure_ptr = m_instance_counter->Get();
            (*secure_ptr)++; 
            secure_ptr.Release();
        }
        pthread_mutex_unlock(&const_mutex); 
    }
    catch(...) {
        pthread_mutex_unlock(&const_mutex); 
        ACS_SHORT_LOG((LM_ERROR, "Some problem in WPServoImpl"));
    }
}


WPServoImpl::~WPServoImpl() {
    AUTO_TRACE("WPServoImpl::~WPServoImpl()"); 

    try {
        pthread_mutex_lock(&destr_mutex); 
        if(m_instance_counter != NULL) {
            CSecAreaResourceWrapper<unsigned short> secure_ptr = m_instance_counter->Get();
            --(*secure_ptr);
            if(!(*secure_ptr)) {
                if(m_dispatcher_ptr != NULL) {
                    m_dispatcher_ptr->suspend();
                    m_dispatcher_ptr->terminate();
                    delete m_dispatcher_ptr;
                }
                if(m_listener_ptr != NULL) {
                    m_listener_ptr->suspend();
                    m_listener_ptr->terminate();
                    delete m_listener_ptr;
                }
                if(m_status_ptr != NULL) {
                    m_status_ptr->suspend();
                    m_status_ptr->terminate();
                    delete m_status_ptr;
                }
                if(m_actPos_list != NULL) {
                    delete m_actPos_list;
                }
                if(m_cmdPos_list != NULL) {
                    delete m_cmdPos_list;
                }
                if(m_instance_counter != NULL) {
                    delete m_instance_counter;
                }
            }
            secure_ptr.Release();
        }

        if (m_cdb_ptr) {
            m_cdb_ptr = NULL;
            delete m_cdb_ptr;
        }

        if (m_wpServoTalker_ptr) {
            m_wpServoTalker_ptr = NULL;
            delete m_wpServoTalker_ptr;
        }

        pthread_mutex_unlock(&destr_mutex); 

    }
    catch(...) {
        pthread_mutex_unlock(&destr_mutex); 
        ACS_SHORT_LOG((LM_WARNING, "~WPServoImpl: Some problems releasing WPServoImpl"));
    }
}


void WPServoImpl::initialize() throw (
        ComponentErrors::CDBAccessExImpl, 
        ComponentErrors::MemoryAllocationExImpl,
        ComponentErrors::ThreadErrorExImpl)
{
    m_wpServoLink_ptr = NULL;
    m_wpServoTalker_ptr = NULL;
    try {
        m_cdb_ptr = new CDBParameters;
    }
    catch (std::bad_alloc& ex)
        THROW_EX(ComponentErrors, MemoryAllocationEx, "WPServoImpl::initialize(): 'new' failure", false);

    AUTO_TRACE("WPServoImpl::initialize()");

    //----  Retrieve MinorServo Attributes ----//

    // Retrive the number_of_axis parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "number_of_axis", m_cdb_ptr->NUMBER_OF_AXIS))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'number_of_axis' from CDB", false);

    // Retrive the number_of_slaves parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "number_of_slaves", m_cdb_ptr->NUMBER_OF_SLAVES))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'number_of_slaves' from CDB", false);
    
    // Retrive the scale_factor parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "scale_factor", m_cdb_ptr->SCALE_FACTOR))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'scale_factor' from CDB", false);
    
    // Retrive the scale_offset parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "scale_offset", m_cdb_ptr->SCALE_OFFSET))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'scale_offset' from CDB", false);
    
    // Retrive the server_ip parameter from CDB 
    if(!CIRATools::getDBValue(getContainerServices(), "server_ip", m_cdb_ptr->SERVER_IP))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'server_ip' from CDB", false);
    
    // Retrive the server_port parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "server_port", m_cdb_ptr->SERVER_PORT))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'server_port' from CDB", false);
    
    // Retrive the timeout parameter from CDB (timeout of socket connection. optional)
    if(!CIRATools::getDBValue(getContainerServices(), "timeout", m_cdb_ptr->SERVER_TIMEOUT))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'timeout' from CDB", false);
    
    // Retrive the servo_address parameter from CDB (address of single wpServo)
    if(!CIRATools::getDBValue(getContainerServices(), "servo_address", m_cdb_ptr->SERVO_ADDRESS))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'servo_address' from CDB", false);
    
    // Retrive the  zero MinorServo parameter from CDB (zero reference)
    if(!CIRATools::getDBValue(getContainerServices(), "zero", m_cdb_ptr->ZERO))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'zero' from CDB", false);
    
    // Retrive the park_position MinorServo parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "park_position", m_cdb_ptr->PARK_POSITION))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'park_position' from CDB", false);
    
    // Retrive the min_speed MinorServo parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "min_speed", m_cdb_ptr->MIN_SPEED))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'min_speed' from CDB", false);
    
    // Retrive the max_speed MinorServo parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "max_speed", m_cdb_ptr->MAX_SPEED))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'max_speed' from CDB", false);
    
    // Retrive the virtual_rs MinorServo parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "virtual_rs", m_cdb_ptr->VIRTUAL_RS))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'virtual_rs' from CDB", false);
    
    // Retrive the require_calibration MinorServo parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "require_calibration", m_cdb_ptr->REQUIRE_CALIBRATION))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'require_calibration' from CDB", false);
    
    // Retrive the expire_time MinorServo parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "expire_time", m_cdb_ptr->EXPIRE_TIME))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'expire_time' from CDB", false);
    
    // Retrive the tracking_delta MinorServo parameter from CDB
    if(!CIRATools::getDBValue(getContainerServices(), "tracking_delta", m_cdb_ptr->TRACKING_DELTA))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'tracking_delta' from CDB", false);
    
    // Retrive the position limits from the CDB
    if(!CIRATools::getDBValue(getContainerServices(), "limits", m_cdb_ptr->LIMITS))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'limits' from CDB", false);
    setLimits(m_cdb_ptr->LIMITS);

    m_expire.timeLastActPos[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastCmdPos[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastPosDiff[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastActElongation[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastVirtualActElongation[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastEngTemperature[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastCounturingErr[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastTorquePerc[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastEngCurrent[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastEngVoltage[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastDriTemperature[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastUtilizationPerc[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastDcTemperature[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastDriverStatus[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastErrorCode[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastStatus[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastAppState[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastAppStatus[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.timeLastCabState[m_cdb_ptr->SERVO_ADDRESS] = -100;
    m_expire.actPos[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.plainActPos[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.cmdPos[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.posDiff[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.actElongation[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.virtualActElongation[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.engTemperature[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.counturingErr[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.torquePerc[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.engCurrent[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.engVoltage[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.driTemperature[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.utilizationPerc[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.dcTemperature[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.driverStatus[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.errorCode[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.status[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.appState[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.appStatus[m_cdb_ptr->SERVO_ADDRESS] = 0;
    m_expire.cabState[m_cdb_ptr->SERVO_ADDRESS] = 0;
    // Set the length of sequences
    m_expire.actPos[m_cdb_ptr->SERVO_ADDRESS].length(m_cdb_ptr->NUMBER_OF_AXIS);
    m_expire.plainActPos[m_cdb_ptr->SERVO_ADDRESS].length(m_cdb_ptr->NUMBER_OF_AXIS);
    m_expire.cmdPos[m_cdb_ptr->SERVO_ADDRESS].length(m_cdb_ptr->NUMBER_OF_AXIS);
    m_expire.posDiff[m_cdb_ptr->SERVO_ADDRESS].length(m_cdb_ptr->NUMBER_OF_AXIS);
    m_expire.actElongation[m_cdb_ptr->SERVO_ADDRESS].length(m_cdb_ptr->NUMBER_OF_AXIS);
    m_expire.virtualActElongation[m_cdb_ptr->SERVO_ADDRESS].length(m_cdb_ptr->NUMBER_OF_AXIS);

    // Offsets initialization
    (m_offsets.user).length(m_cdb_ptr->NUMBER_OF_AXIS);
    (m_offsets.system).length(m_cdb_ptr->NUMBER_OF_AXIS);
    for(size_t i=0; i<m_cdb_ptr->NUMBER_OF_AXIS; i++) {
        (m_offsets.user)[i] = 0;
        (m_offsets.system)[i] = 0;
    }

    try {
        pthread_mutex_lock(&init_mutex); 

        if(m_status_map.count(m_cdb_ptr->SERVO_ADDRESS))
            THROW_EX(ComponentErrors, ThreadErrorEx, "Attempting to set an existing key in m_status_map", false)
        else
            m_status_map[m_cdb_ptr->SERVO_ADDRESS] = &(m_expire.status[m_cdb_ptr->SERVO_ADDRESS]);

        // The socket connection is shared among minor servos (singleton design pattern)
        m_wpServoLink_ptr = WPServoSocket::getSingletonInstance(m_cdb_ptr, &m_expire);
        m_wpServoTalker_ptr = new WPServoTalker(m_cdb_ptr, &m_expire, m_cmdPos_list, &m_offsets, m_limits, m_status_map);

        if(m_talkers.count(m_cdb_ptr->SERVO_ADDRESS))
            THROW_EX(ComponentErrors, ThreadErrorEx, "Attempting to set an existing key in m_talkers", false)
        else
            m_talkers[m_cdb_ptr->SERVO_ADDRESS] = m_wpServoTalker_ptr;

        m_thread_params.socket_ptr = m_wpServoLink_ptr;
        m_thread_params.map_of_talkers_ptr = &m_talkers;
        m_thread_params.listener_mutex = &listener_mutex;
        m_thread_params.scheduler_mutex = &scheduler_mutex;
        m_thread_params.status_mutex = &status_mutex;
        m_thread_params.expire_time = &m_expire;
        m_thread_params.cmd_pos_list = m_cmdPos_list;
        m_thread_params.act_pos_list = m_actPos_list;
        m_thread_params.status_thread_en = &m_status_thread_en;
        (m_thread_params.tracking_delta)[m_cdb_ptr->SERVO_ADDRESS] = m_cdb_ptr->TRACKING_DELTA;
        m_thread_params.stow_state = &m_stow_state;

        setParkPosition(m_cdb_ptr->PARK_POSITION);
        // set m_park_positions
        for(size_t i=0; i<m_cdb_ptr->NUMBER_OF_AXIS; i++)
            (m_park_positions[m_cdb_ptr->SERVO_ADDRESS]).push_back(m_park_position[i]);

        m_thread_params.park_positions = &m_park_positions;

        try {
            // I'll create the threads whitout the aid of the manager because I don't want
            // it kills them when I'll release the component whose has created them. 
            // I want kill the threads only when I'll release the last component active.

            if(m_dispatcher_ptr == NULL)
                m_dispatcher_ptr = new RequestDispatcher("RequestDispatcher", m_thread_params);

            if(m_listener_ptr == NULL)
                m_listener_ptr = new SocketListener("SocketListener", m_thread_params);

            if(m_status_ptr == NULL)
                m_status_ptr = new WPStatusUpdater("WPStatusUpdater", m_thread_params);
        }
        catch (std::bad_alloc& ex)
            THROW_EX(ComponentErrors, MemoryAllocationEx, "WPServoImpl::initialize(): error allocating the threads", false);

        pthread_mutex_unlock(&init_mutex); 

        // Start the listener thread
        m_dispatcher_ptr->resume();
        m_listener_ptr->resume();
        m_status_ptr->resume();

        ACS::doubleSeq dummy_cmd_pos;
        dummy_cmd_pos.length(m_cdb_ptr->NUMBER_OF_AXIS);
        ACS::Time timestamp = getTimeStamp();
        try {
            // Initialize the commanded position with the actual one
            m_wpServoTalker_ptr->getActPos(dummy_cmd_pos, timestamp);
            for(size_t i=0; i<m_cdb_ptr->NUMBER_OF_AXIS; i++) {
                dummy_cmd_pos[i] -= (m_offsets.user)[i];
            }
        }
        catch(MinorServoErrors::CommunicationErrorEx& ex) {
            _ADD_BACKTRACE(MinorServoErrors::CommunicationErrorExImpl, _dummy, ex, "WPServo::initialize()");
        }
        catch(ComponentErrors::SocketErrorExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl, _dummy, ex, "WPServo::initialize()");
        }
        
        try {
            // We put the position in the commanded position list, without
            // to command it to the MSCU (parameter is_dummy=true)
            // In fact the WPStatusUpdater thread needs a position in the 
            // commanded position list to complete its updating
            m_wpServoTalker_ptr->setCmdPos(dummy_cmd_pos, timestamp, 0, true);
        }
        catch(MinorServoErrors::PositioningErrorEx& ex) {
            _ADD_BACKTRACE(MinorServoErrors::PositioningErrorExImpl, _dummy, ex, "WPServo::initialize()");
        }

        // Now the status thread is ready to update the positions
        setStatusUpdating(true);
    }
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
        pthread_mutex_unlock(&init_mutex); 
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"WPServo::initialize()");
        throw _dummy;
    }
    catch (...) {
        pthread_mutex_unlock(&init_mutex); 
        THROW_EX(ComponentErrors, UnexpectedEx, "WPServoImpl::initialize(): unexpected exception", false);
    }
}


void WPServoImpl::execute() 
    throw (
            ComponentErrors::MemoryAllocationExImpl, 
            ComponentErrors::SocketErrorExImpl
    )
{
    AUTO_TRACE("WPServoImpl::execute()");
    
    try {       
        // Actual Position
        m_actPos = new ROdoubleSeq(
                getContainerServices()->getName() + ":actPos", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::ACT_POS, m_cdb_ptr, &m_expire), true);
        
        // Plain Actual Position
        m_plainActPos = new ROdoubleSeq(
                getContainerServices()->getName() + ":plainActPos", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::PLAIN_ACT_POS, m_cdb_ptr, &m_expire), true);
        

        // Commanded Position
        m_cmdPos = new RWdoubleSeq(
                getContainerServices()->getName() + ":cmdPos", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::CMD_POS, m_cdb_ptr, &m_expire), true);
        
        // Difference between commanded and actual position
        m_posDiff = new ROdoubleSeq(
                getContainerServices()->getName() + ":posDiff", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::POS_DIFF, m_cdb_ptr, &m_expire), true);
        
        // Actual Elongation
        m_actElongation = new ROdoubleSeq(
                getContainerServices()->getName() + ":actElongation", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::ACT_ELONGATION, m_cdb_ptr, &m_expire), true);
 
        // Virtual Actual Elongation
        m_virtualActElongation = new ROdoubleSeq(
                getContainerServices()->getName() + ":virtualActElongation", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::VIRTUAL_ACT_ELONGATION, m_cdb_ptr, &m_expire), true);


        // Engine temperature
        m_engTemperature = new ROdoubleSeq(
                getContainerServices()->getName() + ":engTemperature", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::ENG_TEMPERATURE, m_cdb_ptr, &m_expire), true);

        // Counturing Error
        m_counturingErr= new ROdoubleSeq(
                getContainerServices()->getName() + ":counturingErr", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::COUNTURING_ERR, m_cdb_ptr, &m_expire), true);

        // Torque Percentage
        m_torquePerc= new ROdoubleSeq(
                getContainerServices()->getName() + ":torquePerc", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::TORQUE_PERC, m_cdb_ptr, &m_expire), true);

        // Engine Current
        m_engCurrent = new ROdoubleSeq(
                getContainerServices()->getName() + ":engCurrent", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::ENG_CURRENT, m_cdb_ptr, &m_expire), true);

        // Engine Voltage 
        m_engVoltage = new ROdoubleSeq(
                getContainerServices()->getName() + ":engVoltage", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::ENG_VOLTAGE, m_cdb_ptr, &m_expire), true);

        // Driver Temperature
        m_driTemperature = new ROdoubleSeq(
                getContainerServices()->getName() + ":driTemperature", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::DRI_TEMPERATURE, m_cdb_ptr, &m_expire), true);

        // Utilization Percentage
        m_utilizationPerc= new ROdoubleSeq(
                getContainerServices()->getName() + ":utilizationPerc", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::UTILIZATION_PERC, m_cdb_ptr, &m_expire), true);

        // Drive Cabinet Temperature
        m_dcTemperature= new ROdoubleSeq(
                getContainerServices()->getName() + ":dcTemperature", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::DC_TEMPERATURE, m_cdb_ptr, &m_expire), true);

        // Driver Status
        m_driverStatus= new ROdoubleSeq(
                getContainerServices()->getName() + ":driverStatus", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::DRIVER_STATUS, m_cdb_ptr, &m_expire), true);

        // Error Code
        m_errorCode= new ROdoubleSeq(
                getContainerServices()->getName() + ":errorCode", 
                getComponent(), 
                new PdoubleSeqDevIO(m_wpServoTalker_ptr, PdoubleSeqDevIO::ERROR_CODE, m_cdb_ptr, &m_expire), true);

        // Status
        m_status= new ROpattern(
                getContainerServices()->getName() + ":status", 
                getComponent(), 
                new WPStatusDevIO<ACS::pattern>(m_wpServoTalker_ptr, WPStatusDevIO<ACS::pattern>::STATUS, m_cdb_ptr, &m_expire), true);
     
    }
    catch (std::bad_alloc& ex)
        THROW_EX(ComponentErrors, MemoryAllocationEx, "WPServoImpl::execute(): 'new' failure", false);
}


void WPServoImpl::cleanUp() 
{
    AUTO_TRACE("WPServoImpl::cleanUp()");
    stopPropertiesMonitoring();

    try {
        pthread_mutex_lock(&init_mutex); 
        pthread_mutex_lock(&status_mutex); 
        pthread_mutex_lock(&listener_mutex); 
        pthread_mutex_lock(&scheduler_mutex); 
        if (m_cdb_ptr && m_talkers.count(m_cdb_ptr->SERVO_ADDRESS))
            m_talkers.erase(m_cdb_ptr->SERVO_ADDRESS);
        setStatusUpdating(false);
        pthread_mutex_unlock(&listener_mutex); 
        pthread_mutex_unlock(&scheduler_mutex); 
        pthread_mutex_unlock(&status_mutex); 
        pthread_mutex_unlock(&init_mutex); 
    }
    catch(...) {
        pthread_mutex_unlock(&listener_mutex); 
        pthread_mutex_unlock(&scheduler_mutex); 
        pthread_mutex_unlock(&status_mutex); 
        pthread_mutex_unlock(&init_mutex); 
        setStatusUpdating(false);
    }

    CharacteristicComponentImpl::cleanUp(); 
}


void WPServoImpl::aboutToAbort()
{
    AUTO_TRACE("WPServoImpl::aboutToAbort()");
    stopPropertiesMonitoring();

    try {
        pthread_mutex_lock(&init_mutex); 
        pthread_mutex_lock(&status_mutex); 
        pthread_mutex_lock(&listener_mutex); 
        pthread_mutex_lock(&scheduler_mutex); 
        if (m_cdb_ptr && m_talkers.count(m_cdb_ptr->SERVO_ADDRESS))
            m_talkers.erase(m_cdb_ptr->SERVO_ADDRESS);
        setStatusUpdating(false);
        pthread_mutex_unlock(&listener_mutex); 
        pthread_mutex_unlock(&scheduler_mutex); 
        pthread_mutex_unlock(&status_mutex); 
        pthread_mutex_unlock(&init_mutex); 
    }
    catch(...) {
        pthread_mutex_unlock(&listener_mutex); 
        pthread_mutex_unlock(&scheduler_mutex); 
        pthread_mutex_unlock(&status_mutex); 
        pthread_mutex_unlock(&init_mutex); 
        setStatusUpdating(false);
    }

    CharacteristicComponentImpl::aboutToAbort(); 
}


void WPServoImpl::setPosition(const ACS::doubleSeq &position, const ACS::Time exe_time) 
    throw (MinorServoErrors::PositioningErrorEx, MinorServoErrors::CommunicationErrorEx) 
{

    AUTO_TRACE("WPServoImpl::setPosition()");
    try {
        if(position.length() != m_cdb_ptr->NUMBER_OF_AXIS)
            THROW_EX(MinorServoErrors, PositioningErrorEx, "Cannot set position: wrong number of axis", true);
        
        // Set the position
        ACS::Time timestamp = getTimeStamp();
        m_wpServoTalker_ptr->setCmdPos(position, timestamp, exe_time);
    }
    catch(MinorServoErrors::PositioningErrorEx &ex) {
        throw;
    }
    catch(...)
        THROW_EX(MinorServoErrors, PositioningErrorEx, "Cannot set position", true);
}


void WPServoImpl::disable(const ACS::Time exe_time) throw (MinorServoErrors::CommunicationErrorEx) 
{
    AUTO_TRACE("WPServoImpl::disable()");
    try {
        // The index code of disable command in Talk.cpp is 9
        m_wpServoTalker_ptr->action(9, exe_time);
    }
    catch(...) {
        THROW_EX(MinorServoErrors, CommunicationErrorEx, "Cannot brake the servo", true);
    }

}


void WPServoImpl::clearEmergency() throw (MinorServoErrors::CommunicationErrorEx) 
{
    AUTO_TRACE("WPServoImpl::clearEmergency()");
    try {
        // The index code of clremergency command in Talk.cpp is 9
        m_wpServoTalker_ptr->action(10, 0);
    }
    catch(...) {
        THROW_EX(MinorServoErrors, CommunicationErrorEx, "Cannot clear the emergency stop", true);
    }

}


void WPServoImpl::clearUserOffset() throw (MinorServoErrors::OperationNotPermittedEx, MinorServoErrors::CommunicationErrorEx)
{
    AUTO_TRACE("WPServoImpl::clearUserOffset()");
    ACS::doubleSeq offset;
    offset.length(m_cdb_ptr->NUMBER_OF_AXIS);
    for(size_t i=0; i<m_cdb_ptr->NUMBER_OF_AXIS; i++)
        offset[i] = 0;
    setUserOffset(offset); // Raise a MinorServoErrors::CommunicationErrorEx
}





void WPServoImpl::setUserOffset(const ACS::doubleSeq &offset) 
    throw (MinorServoErrors::OperationNotPermittedEx, MinorServoErrors::CommunicationErrorEx)
{
    AUTO_TRACE("WPServoImpl::setUserOffset()");
    if(offset.length() != m_cdb_ptr->NUMBER_OF_AXIS)  {
        THROW_EX(MinorServoErrors, OperationNotPermittedEx, "Cannot set the user offset: wrong number of axis", true);
    }
    setOffset(offset, m_offsets.user); // Raise a MinorServoErrors::CommunicationErrorEx
}



ACS::doubleSeq * WPServoImpl::getUserOffset(void) 
{
    ACS::doubleSeq_var offset = new ACS::doubleSeq;
    offset->length((m_offsets.user).length());
    pthread_mutex_lock(&offset_mutex); 
    for(size_t i=0; i<(m_offsets.user).length(); i++)
        offset[i] = (m_offsets.user)[i];
    pthread_mutex_unlock(&offset_mutex); 
    return offset._retn();
}


void WPServoImpl::clearSystemOffset() 
    throw (MinorServoErrors::OperationNotPermittedEx, MinorServoErrors::CommunicationErrorEx)
{
    AUTO_TRACE("WPServoImpl::clearSystemOffset()");
    ACS::doubleSeq offset;
    offset.length(m_cdb_ptr->NUMBER_OF_AXIS);
    for(size_t i=0; i<m_cdb_ptr->NUMBER_OF_AXIS; i++)
        offset[i] = 0;
    setSystemOffset(offset);
}


void WPServoImpl::setSystemOffset(const ACS::doubleSeq &offset) 
    throw (MinorServoErrors::OperationNotPermittedEx, MinorServoErrors::CommunicationErrorEx)
{
    AUTO_TRACE("WPServoImpl::setSystemOffset()");
    if(offset.length() != m_cdb_ptr->NUMBER_OF_AXIS)  {
        THROW_EX(MinorServoErrors, OperationNotPermittedEx, "Cannot set the system offset: wrong number of axis", true);
    }
    setOffset(offset, m_offsets.system);
}


ACS::doubleSeq * WPServoImpl::getSystemOffset(void) 
{
    ACS::doubleSeq_var offset = new ACS::doubleSeq;
    offset->length((m_offsets.system).length());
    pthread_mutex_lock(&offset_mutex); 
    for(size_t i=0; i<(m_offsets.system).length(); i++)
        offset[i] = (m_offsets.system)[i];
    pthread_mutex_unlock(&offset_mutex); 
    return offset._retn();
}


void WPServoImpl::setOffset(const ACS::doubleSeq &offset, ACS::doubleSeq &target) 
    throw (MinorServoErrors::OperationNotPermittedEx, MinorServoErrors::CommunicationErrorEx)
{
    pthread_mutex_lock(&offset_mutex); 
    try {
        CSecAreaResourceWrapper<map<int, vector<PositionItem> > > lst_secure_requests = m_cmdPos_list->Get(); 
        
            bool is_changed = false;
            for(size_t i=0; i<m_cdb_ptr->NUMBER_OF_AXIS; i++)
                if(target[i] != offset[i]) {
                    target[i] = offset[i];
                    is_changed = true;
                }
            if(!is_changed) {
                lst_secure_requests.Release();
                pthread_mutex_unlock(&offset_mutex); 
                return;
            }

            // This vector will be a copy of PositionItems vector
            vector<PositionItem> vitem;
            try {
                TIMEVALUE now(0.0L);
                IRA::CIRATools::getTime(now);
                vector<PositionItem>::size_type idx = findPositionIndex(
                        lst_secure_requests,
                        now.value().value, 
                        m_cdb_ptr->SERVO_ADDRESS,
                        false
                );
                // For every item in PositionItem
                for( ; idx < ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).size(); idx++) {
                    PositionItem item;
                    item.exe_time = (((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS])[idx]).exe_time;
                    (item.position).length(m_cdb_ptr->NUMBER_OF_AXIS);
                    for(size_t i = 0; i < m_cdb_ptr->NUMBER_OF_AXIS; i++)
                        (item.position)[i] = ((((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS])[idx]).position)[i];
                    vitem.push_back(item);
                }
            }
            catch(PosNotFoundEx) {
                // Put the actual position in the queue, so it will be commanded with the offset
                PositionItem item;
                item.exe_time = getTimeStamp();
                (item.position).length(m_cdb_ptr->NUMBER_OF_AXIS);
                item.position = m_expire.actPos[m_cdb_ptr->SERVO_ADDRESS];
                vitem.push_back(item);
            }
            // ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).clear(); // Is it necessary?
            lst_secure_requests.Release();
            cleanPositionsQueue();
            ACS::Time timestamp;
            // Set the positions: the new offsets will be added in the setCmdPos method
            for(vector<PositionItem>::iterator iter = vitem.begin(); iter != vitem.end(); iter++) {
                timestamp = getTimeStamp();
                m_wpServoTalker_ptr->setCmdPos((*iter).position, timestamp, (*iter).exe_time);
            }
    }
    catch(...) {
        pthread_mutex_unlock(&offset_mutex); 
        m_status_ptr->restart();
        throw;
    }
        
    pthread_mutex_unlock(&offset_mutex); 
}



void WPServoImpl::setStatusUpdating(bool flag) { m_status_thread_en[m_cdb_ptr->SERVO_ADDRESS] = flag; }


bool WPServoImpl::isStatusThreadEn() { 
    if(m_status_thread_en.count(m_cdb_ptr->SERVO_ADDRESS))
        return m_status_thread_en[m_cdb_ptr->SERVO_ADDRESS]; 
    else
        return false;
}


void WPServoImpl::setup(const ACS::Time exe_time) 
    throw (MinorServoErrors::SetupErrorEx, MinorServoErrors::CommunicationErrorEx) 
{

    AUTO_TRACE("WPServoImpl::setup()");
    if(m_expire.cabState[m_cdb_ptr->SERVO_ADDRESS] == CAB_DISABLED_FROM_OTHER_CAB)
        THROW_EX(MinorServoErrors, SetupErrorEx, "Cannot make a minor servo setup: drive cabinet disabled from other DC", true);

    if(isStarting())
        THROW_EX(MinorServoErrors, SetupErrorEx, "Cannot make a new minor servo setup: an old setup is in progress.", true);

    try {
        // The index code of setup command in Talk.cpp is 3
        m_wpServoTalker_ptr->action(3, exe_time);
    }
    catch(...) {
        THROW_EX(MinorServoErrors, SetupErrorEx, "Cannot make a minor servo setup", true);
    }

}


void WPServoImpl::cleanPositionsQueue(const ACS::Time exe_time) throw (MinorServoErrors::CommunicationErrorEx) 
{

    AUTO_TRACE("WPServoImpl::cleanPositionsQueue()");
    try {
        m_wpServoTalker_ptr->action(8, exe_time);
        // The MSCU sometimes returns a NAK if there is not a delay between a clear and a setpos commands
        usleep(500000); // 500 ms
        CSecAreaResourceWrapper<map<int, vector< PositionItem> > > lst_secure_requests = (m_cmdPos_list)->Get();
        if((*lst_secure_requests).count(m_cdb_ptr->SERVO_ADDRESS)) {
            if(((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).size() > 1) {
                try {
                    vector<PositionItem>::size_type idx = findPositionIndex(
                            lst_secure_requests, 
                            getTimeStamp(), 
                            m_cdb_ptr->SERVO_ADDRESS, 
                            false
                    );
                    vector<PositionItem>::iterator ibegin = ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).begin();
                    vector<PositionItem>::iterator iend = ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).end();
                    if(ibegin + idx + 1 < iend) {
                        ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).erase(ibegin + idx + 1, iend);
                    }
                }
                catch(PosNotFoundEx) {
                    lst_secure_requests.Release();
                    ACS_SHORT_LOG((LM_WARNING, "In WPServoImpl::cleanPositionsQueue(): cannot clean the commanded positions."));
                }
            }
        }
        lst_secure_requests.Release();
    }
    catch(...) {
        THROW_EX(MinorServoErrors, CommunicationErrorEx, "Cannot clean the MSCU positions queue", true);
    }

}


bool WPServoImpl::isParked() 
{
    AUTO_TRACE("WPServoImpl::isParked()");
    bitset<STATUS_WIDTH> status(m_expire.status[m_cdb_ptr->SERVO_ADDRESS]);
    return status.test(STATUS_PARKED); 
}


bool WPServoImpl::isTracking() 
{
    AUTO_TRACE("WPServoImpl::isTracking()");
    bitset<STATUS_WIDTH> status(m_expire.status[m_cdb_ptr->SERVO_ADDRESS]);
    return status.test(STATUS_TRACKING); 
}


bool WPServoImpl::isReady() 
{
    AUTO_TRACE("WPServoImpl::isReady()");
    bitset<STATUS_WIDTH> status(m_expire.status[m_cdb_ptr->SERVO_ADDRESS]);
    return status.test(STATUS_READY); 
}


bool WPServoImpl::isStarting() 
{
    AUTO_TRACE("WPServoImpl::isStarting()");
    bitset<STATUS_WIDTH> status(m_expire.status[m_cdb_ptr->SERVO_ADDRESS]);
    return status.test(STATUS_SETUP); 
}


bool WPServoImpl::isParking() 
{
    AUTO_TRACE("WPServoImpl::isParking()");
    return m_stow_state[m_cdb_ptr->SERVO_ADDRESS];
}


void WPServoImpl::stow(const ACS::Time exe_time) 
    throw (MinorServoErrors::StowErrorEx, MinorServoErrors::CommunicationErrorEx) 
{

    AUTO_TRACE("WPServoImpl::stow()");
    if(!isReady())
        THROW_EX(MinorServoErrors, StowErrorEx, "Cannot make a minor servo stow: servo not ready", true);

    if(isParking())
        THROW_EX(MinorServoErrors, StowErrorEx, "Cannot make a new minor servo stow: an old stow is in progress.", true);

    try {
        // The index code of stow command in Talk.cpp is 4
        m_wpServoTalker_ptr->action(4, exe_time);
    }
    catch(...) {
        THROW_EX(MinorServoErrors, StowErrorEx, "Cannot make a minor servo stow", true);
    }

    ACS::doubleSeq dummy_cmd_pos;
    dummy_cmd_pos.length(m_cdb_ptr->NUMBER_OF_AXIS);
    ACS::Time timestamp = getTimeStamp();
    // Initialize the commanded position with the stow position
    for(size_t i=0; i<m_cdb_ptr->NUMBER_OF_AXIS; i++)
        dummy_cmd_pos[i] = m_park_position[i] - (m_offsets.user)[i];

    // Iposta la dummy_cmd_pos al valore di park_pos
    try {
        // We put the position in the commanded position list, without
        // to command it to the MSCU (parameter is_dummy=true)
        // In fact the WPStatusUpdater thread needs a position in the 
        // commanded position list to complete its updating
        m_wpServoTalker_ptr->setCmdPos(dummy_cmd_pos, timestamp, 0, true);
    }
    catch(MinorServoErrors::PositioningErrorEx& ex) {
        _ADD_BACKTRACE(MinorServoErrors::PositioningErrorExImpl, _dummy, ex, "WPServo::stow()");
    }

}


void WPServoImpl::calibrate(const ACS::Time exe_time) 
    throw (MinorServoErrors::CalibrationErrorEx, MinorServoErrors::CommunicationErrorEx) 
{

    AUTO_TRACE("WPServoImpl::calibrate()");
    if(m_cdb_ptr->REQUIRE_CALIBRATION) {
        try {
            // The index code of calibrate command in Talk.cpp is 5
            m_wpServoTalker_ptr->action(5, exe_time);
        }
        catch(...)
            THROW_EX(MinorServoErrors, CalibrationErrorEx, "Cannot make a minor servo calibration", true);
    }

}


/*
 * POS_LIMIT
 * NEG_LIMIT
 * ACCELERATION
 * MAX_SPEED
 */
ACS::doubleSeq * WPServoImpl::getData(const char *data_name) throw (MinorServoErrors::CommunicationErrorEx) {
    string dname(data_name);
    ACS::Time timestamp;
    ACS::doubleSeq data;

    if(dname == "POS_LIMIT") {
        m_wpServoTalker_ptr->getParameter(
                data,
                timestamp, // Passo il timestamp
                POS_LIMIT_IDX,
                POS_LIMIT_SUB
        );
    }
    else if(dname == "NEG_LIMIT") {
        m_wpServoTalker_ptr->getParameter(
                data,
                timestamp, // Passo il timestamp
                NEG_LIMIT_IDX,
                NEG_LIMIT_SUB
        );
    }
    else if(dname == "ACCELERATION") {
        m_wpServoTalker_ptr->getParameter(
                data,
                timestamp, // Passo il timestamp
                ACCELERATION_IDX,
                ACCELERATION_SUB
        );
    }
    else if(dname == "MAX_SPEED") {
        m_wpServoTalker_ptr->getParameter(
                data,
                timestamp, // Passo il timestamp
                MAX_SPEED_IDX,
                MAX_SPEED_SUB
        );
    }
    else
        THROW_EX(MinorServoErrors, CommunicationErrorEx, ("Data " + dname + " does not exist").c_str(), true);

    ACS::doubleSeq_var data_ptr = new ACS::doubleSeq;
    data_ptr->length(data.length());
    for(size_t i = 0; i < data.length(); i++)
        data_ptr[i] = data[i];

    return data_ptr._retn();
}

void WPServoImpl::setLimits(IRA::CString limits) {
    // Split the action in items.
    vector<string> items = split(string(limits), ";");
    for(vector<string>::iterator iter = items.begin(); iter != items.end(); iter++) {
        vector<string> tuple = split(*iter, ",");
        string min = remove(tuple[0], '(');
        string max = remove(tuple[1], ')');
        Limits lim;
        lim.min = str2double(min);
        lim.max = str2double(max);
        m_limits.push_back(lim);
    }
}


ACS::doubleSeq * WPServoImpl::getMaxPositions() {
    ACS::doubleSeq_var limits = new ACS::doubleSeq;
    limits->length(m_limits.size());
    for(size_t i=0; i<limits->length(); i++)
        limits[i] = (m_limits[i]).max;
    return limits._retn();
}


ACS::doubleSeq * WPServoImpl::getMinPositions() {
    ACS::doubleSeq_var limits = new ACS::doubleSeq;
    limits->length(m_limits.size());
    for(size_t i=0; i<limits->length(); i++)
        limits[i] = (m_limits[i]).min;
    return limits._retn();
}


void WPServoImpl::setParkPosition(IRA::CString position) {
    vector<string> items = split(string(position), ",");
    for(vector<string>::iterator iter = items.begin(); iter != items.end(); iter++)
        m_park_position.push_back(str2double(*iter));

    if(m_park_position.size() != m_cdb_ptr->NUMBER_OF_AXIS)
        THROW_EX(ComponentErrors, UnexpectedEx, "WPServoImpl::setParkPosition(): wrong park position in the CDB", false);
}


CORBA::Long WPServoImpl::getStatus() {
    return m_expire.status[m_cdb_ptr->SERVO_ADDRESS];
}


ACS::doubleSeq * WPServoImpl::getPositionFromHistory(ACS::Time time) throw (ComponentErrors::UnexpectedEx) {

    ACS::doubleSeq_var position = new ACS::doubleSeq;
    position->length(m_cdb_ptr->NUMBER_OF_AXIS);
    PositionItem pos_item;

    try {
        // Get the CSecArea
        CSecAreaResourceWrapper<map<int, vector<PositionItem> > > secure_request = m_actPos_list->Get(); 
        pos_item = getPosItemFromHistory(secure_request, time, m_cdb_ptr->SERVO_ADDRESS);
        secure_request.Release();
        position = pos_item.position;
    }
    catch(PosNotFoundEx) {
        THROW_EX(ComponentErrors, UnexpectedEx, "WPServoImpl::getPositionFromHistory(): position not found.", true);
    }
    catch(IndexErrorEx) {
        THROW_EX(ComponentErrors, UnexpectedEx, "WPServoImpl::getPositionFromHistory(): index out of range.", true);
    }
    catch (...) {
        THROW_EX(ComponentErrors, UnexpectedEx, "WPServoImpl::getPositionFromHistory()", true);
    }

    return position._retn();
}

CORBA::Double WPServoImpl::getTrackingDelta() {
    return m_cdb_ptr->TRACKING_DELTA;
}


bool WPServoImpl::isReadyToSetup() {
    bitset<32> app_status(m_expire.appStatus[m_cdb_ptr->SERVO_ADDRESS]);
    return 
        ( m_expire.cabState[m_cdb_ptr->SERVO_ADDRESS] == CAB_DISABLED_FROM_AIF_IN ||
          m_expire.cabState[m_cdb_ptr->SERVO_ADDRESS] == CAB_BLOCK_REMOVED ||
          (app_status.test(ASTATUS_READY) && !app_status.test(ASTATUS_AUTOMATIC)) 
        ) ? true : false;
}


bool WPServoImpl::isDisabledFromOtherDC() {
    return (m_expire.cabState[m_cdb_ptr->SERVO_ADDRESS] == CAB_DISABLED_FROM_OTHER_CAB) ? true : false;
}


bool WPServoImpl::isInEmergencyStop() {
    return (m_expire.cabState[m_cdb_ptr->SERVO_ADDRESS] == CAB_EMERGENCY_REMOVED) ? true : false;
}


GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_actPos, actPos);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_plainActPos, plainActPos);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::RWdoubleSeq, m_cmdPos, cmdPos);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_posDiff, posDiff);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_actElongation, actElongation);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_virtualActElongation, virtualActElongation);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_engTemperature, engTemperature);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_counturingErr, counturingErr);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_torquePerc, torquePerc);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_engCurrent, engCurrent);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_engVoltage, engVoltage);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_driTemperature, driTemperature);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_utilizationPerc, utilizationPerc);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_dcTemperature, dcTemperature);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_driverStatus, driverStatus);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROdoubleSeq, m_errorCode, errorCode);
GET_PROPERTY_REFERENCE(WPServoImpl, ACS::ROpattern, m_status, status);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(WPServoImpl)
