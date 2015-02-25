/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Bartolini
 *  E-mail:       bartolini@ira.inaf.it
\*******************************************************************************/

#include "MinorServoBossImpl.h"
#include <acsThread.h>
#include <acsThreadManager.h>

using namespace SimpleParser;

MinorServoBossImpl::MinorServoBossImpl(
        const ACE_CString &CompName, maci::ContainerServices *containerServices
        ) : 
    CharacteristicComponentImpl(CompName, containerServices),
    m_services(containerServices),
    m_actual_config(NULL),
    m_actual_conf("unconfigured"),
    m_status(this),
    m_ready(this),
    m_actualSetup(this),
    m_starting(this),
    m_asConfiguration(this),
    m_elevationTrack(this),
    m_scanActive(this),
    m_scanning(this),
    // m_verbose_status(this),
    m_publisher_thread_ptr(NULL),
    m_tracking_thread_ptr(NULL),
    m_position_monitoring_thread_ptr(NULL),
    m_setup_thread_ptr(NULL),
    m_scan_thread_ptr(NULL),
    m_nchannel(NULL)
{   
    AUTO_TRACE("MinorServoBossImpl::MinorServoBossImpl()");
    m_commanded_conf = "none";
    m_servo_scanned = "none";
    m_parser = new SimpleParser::CParser<MinorServoBossImpl>(this, 10);
    //m_antennaBoss = Antenna::AntennaBoss::_nil();
}


MinorServoBossImpl::~MinorServoBossImpl() { 
    AUTO_TRACE("MinorServoBossImpl::~MinorServoBossImpl()"); 
    if(m_parser != NULL)
        delete m_parser;
}


void MinorServoBossImpl::initialize() throw (
    ComponentErrors::CouldntGetComponentExImpl, 
    MinorServoErrors::SetupErrorExImpl)
{
    AUTO_TRACE("MinorServoBossImpl::initialize()");

    /** 
     * INITIALIZE NOTIFICATION CHANNEL
     */
    try {
        m_nchannel = new nc::SimpleSupplier(MinorServo::MINORSERVO_DATA_CHANNEL, this);
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::initialize",
                   (LM_DEBUG, "Initialized NC supplier"));
    }
    catch (...) {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"MinorServoBoss::initialize()");
    }

    /**
     * READ POSSIBLE SETUPS FROM CDB
     */
    m_config = get_configuration_from_CDB(m_services);
    CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::initialize",
              (LM_DEBUG, "Read setups from CDB"));
    /**
     * READ ATTRIBUTES FROM CDB
     */
    if(!IRA::CIRATools::getDBValue(m_services, "server_ip", m_server_ip))
        THROW_EX(ComponentErrors,CDBAccessEx, 
                 "cannot read server_ip from CDB",
                 false);
    CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::initialize",
              (LM_DEBUG, "server ip: %s", (const char*)m_server_ip));
    if(!IRA::CIRATools::getDBValue(m_services, "AntennaBossInterface", 
                                  (const char*)m_antennaBossInterface))
        THROW_EX(ComponentErrors,CDBAccessEx, 
                 "cannot read AntennaBossInterface from CDB",
                 false);
    /**
     * INITIALIZE STATUS
     */
    m_servo_status.starting = false;
    m_servo_status.ready = false;
    m_servo_status.scan_active = false;
    m_servo_status.scanning = false;
    m_servo_status.parking = false;
    m_servo_status.parked = false;

    /**
     * INITIALIZE SERVO CONTROL
     */
    //TODO: add exception management here
    m_control = get_servo_control(m_server_ip);
    CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::initialize",
              (LM_DEBUG, "Instantiated new minor servo control"));

    /**
     * INITIALIZE PARSER WITH COMMANDS
     */
    m_parser->add(
            "servoPark", 
            new function0<MinorServoBossImpl, non_constant, void_type>(this, &MinorServoBossImpl::parkImpl), 
            0
    );
    m_parser->add(
            "servoSetup", 
            new function1<MinorServoBossImpl, non_constant, void_type, I<string_type> >(this, &MinorServoBossImpl::setupImpl), 
            1
    );
    m_parser->add(
            "setServoElevationTracking", 
            new function1<MinorServoBossImpl, non_constant, void_type, I<string_type> >(this, &MinorServoBossImpl::setElevationTrackingImpl), 
            1
    );
    m_parser->add(
            "setServoASConfiguration", 
            new function1<MinorServoBossImpl, non_constant, void_type, I<string_type> >(this, &MinorServoBossImpl::setASConfigurationImpl), 
            1
    );
    m_parser->add(
            "clearServoOffsets", 
            new function0<MinorServoBossImpl, non_constant, void_type>(this, &MinorServoBossImpl::clearOffsetsFromOI), 
            0
    );
 
    m_parser->add(
            "setServoOffset", 
             new function2<MinorServoBossImpl, non_constant, void_type, I<string_type>, I<double_type> >(this, &MinorServoBossImpl::setUserOffsetFromOI), 
             2
    );
}

void 
MinorServoBossImpl::execute() 
throw (ComponentErrors::MemoryAllocationExImpl)
{
    AUTO_TRACE("MinorServoBossImpl::execute()");
    try {       
        m_status = new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), 
                                  POA_Management::ROTSystemStatus>(
                                    getContainerServices()->getName() + ":status", 
                                    getComponent(), 
                                    new DevIOStatus(&m_servo_status), 
                                    true
                                  );  
        m_ready = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":ready", getComponent(), \
                   new DevIOReady(&m_servo_status), true);
		m_actualSetup = new baci::ROstring(getContainerServices()->getName() + ":actualSetup",
                getComponent(), new DevIOActualSetup(&m_actual_config), true);
        m_starting = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":starting",getComponent(), \
                   new DevIOStarting(&m_servo_status), true);
        m_asConfiguration = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":asConfiguration",getComponent(), \
                   new DevIOASConfiguration(&m_servo_status), true);
        m_elevationTrack = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":elevationTrack",getComponent(), \
                   new DevIOElevationTrack(&m_servo_status), true);
        m_scanActive = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":scanActive",getComponent(), \
                   new DevIOScanActive(&m_servo_status), true);
        m_scanning = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":scanning",getComponent(), \
                   new DevIOScanning(&m_servo_status), true);
    }
    catch (std::bad_alloc& ex) {
        THROW_EX(ComponentErrors, MemoryAllocationEx, "MinorServoBoss::execute(): 'new' failure", false);
    }
    catch (...) {
        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::execute: unexpected exception allocating status properties"));
    }

    if(m_publisher_thread_ptr != NULL) {
        m_publisher_thread_ptr->suspend();
        m_publisher_thread_ptr->terminate();
        m_publisher_thread_ptr = NULL;
    }
    try {
        PublisherThreadParameters thread_params(&m_servo_status, 
                                                m_nchannel,
                                                m_control);
        m_publisher_thread_ptr = getContainerServices()->
                                 getThreadManager()->
                                 create<MSBossPublisher, PublisherThreadParameters>
                                 (PUBLISHER_THREAD_NAME, thread_params);
        m_publisher_thread_ptr->resume();
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::execute",
                   (LM_DEBUG, "Started publisher thread"));
    }
    catch(...) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "Error: MSBossPublisher already exists", true);
    }
    m_position_monitoring_thread_ptr = getContainerServices()->
                                       getThreadManager()->
                                       create<PositionMonitoringThread, MedMinorServoControl_sp>
                                       (POSITION_MONITORING_THREAD_NAME, m_control);
    m_position_monitoring_thread_ptr->resume();
    CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::execute",
              (LM_DEBUG, "Position Monitoring Thread started"));
    startPropertiesMonitoring();
}

void 
MinorServoBossImpl::cleanUp() 
{
    AUTO_TRACE("MinorServoBossImpl::cleanUp()");
    stopPropertiesMonitoring();

    try {
        turnTrackingOff(); // Raises ConfigurationError
    }
    catch(...){
        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::cleanUp(): some problems turning the elevation tracking off."));
    }
    if (m_publisher_thread_ptr != NULL) {
        m_publisher_thread_ptr->suspend();
        m_publisher_thread_ptr->terminate();
    }
    if (m_position_monitoring_thread_ptr != NULL) {
        m_position_monitoring_thread_ptr->suspend();
        m_position_monitoring_thread_ptr->terminate();
    }
    if(m_nchannel !=NULL ) {
        m_nchannel->disconnect();
        m_nchannel = NULL;
    }
    disconnect();
    CharacteristicComponentImpl::cleanUp(); 
}

void 
MinorServoBossImpl::aboutToAbort()
{
    AUTO_TRACE("MinorServoBossImpl::aboutToAbort()");
    stopPropertiesMonitoring();
    try {
        turnTrackingOff();
    }
    catch(...){
        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::aboutToAbort(): some problems turning the elevation tracking off."));
    }
    if (m_publisher_thread_ptr != NULL) {
        m_publisher_thread_ptr->suspend();
        m_publisher_thread_ptr->terminate();
    }
    if (m_position_monitoring_thread_ptr != NULL) {
        m_position_monitoring_thread_ptr->suspend();
        m_position_monitoring_thread_ptr->terminate();
    }
    if(m_setup_thread_ptr != NULL)
    {
        getContainerServices()->getThreadManager()->destroy(m_setup_thread_ptr);
        m_setup_thread_ptr = NULL;
    }
    if(m_scan_thread_ptr != NULL)
    {
        getContainerServices()->getThreadManager()->destroy(m_scan_thread_ptr);
        m_scan_thread_ptr = NULL;
    }
    if(m_nchannel != NULL ) {
        m_nchannel->disconnect();
        m_nchannel = NULL;
    }
    disconnect();
    CharacteristicComponentImpl::aboutToAbort(); 
}

void 
MinorServoBossImpl::setup(const char *config) throw (
    CORBA::SystemException, 
    MinorServoErrors::SetupErrorEx)
{
    AUTO_TRACE("MinorServoBossImpl::setup()");
    try {
        setupImpl(config);
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::setup",
                   (LM_NOTICE, "Minor Servo Setup: %s", config));
    }
    catch (MinorServoErrors::SetupErrorExImpl& ex) {
        ex.log(LM_WARNING);
        throw ex.getSetupErrorEx();     
    }
}

void 
MinorServoBossImpl::setupImpl(const char *config) 
throw (MinorServoErrors::SetupErrorEx)
{
    if(m_servo_status.starting)
        THROW_EX(MinorServoErrors, SetupErrorEx, 
                 "The system is executing another setup", false);
    if(m_servo_status.parking)
        THROW_EX(MinorServoErrors, SetupErrorEx, 
                 "The system is executing a park", false);
    if(m_servo_status.scan_active)
        THROW_EX(MinorServoErrors, SetupErrorEx, 
                 "The system is performing a scan", false);
    if(m_config.count(std::string(config)) == 0)
        THROW_EX(MinorServoErrors, SetupErrorEx, 
                 "Cannot find requested configuration", false);
    try {
        setElevationTrackingImpl(IRA::CString("OFF"));
    }
    catch(...) {
        THROW_EX(MinorServoErrors, SetupErrorEx, "cannot turn the tracking off", false);
    }
    m_servo_status.reset();
    m_servo_status.starting = true;
    m_actual_config = &(m_config[std::string(config)]);
    /**
     * Get the setup position at 45 deg
     */
    MedMinorServoPosition setup_position = m_actual_config->get_position();
    m_offset.initialize(m_actual_config->is_primary_focus());
    try{
        m_actual_conf = string(config);
        m_control->set_position(setup_position);
        SetupThreadParameters thread_params(m_control,
                                            &m_servo_status);
        if(m_setup_thread_ptr != NULL)
        {
            getContainerServices()->getThreadManager()->destroy(m_setup_thread_ptr);
            m_setup_thread_ptr = NULL;
        }
        m_setup_thread_ptr = getContainerServices()->getThreadManager()->
                                create<SetupThread, SetupThreadParameters>
                                 (SETUP_THREAD_NAME, thread_params);
        if(m_setup_thread_ptr->isSuspended())
            m_setup_thread_ptr->resume();
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::setupImpl",
                   (LM_DEBUG, "Started setup positioning thread"));
    }catch(const ServoTimeoutError& ste){
        THROW_EX(MinorServoErrors, SetupErrorEx, ste.what(), false);
    }catch(...){
        THROW_EX(MinorServoErrors, SetupErrorEx, "Cannot conclude setup", false);
    }
}

void 
MinorServoBossImpl::park() 
throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx)
{
    try {
        parkImpl();
    }
    catch (ManagementErrors::ParkingErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getParkingErrorEx();       
    }
}

void 
MinorServoBossImpl::parkImpl() 
throw (ManagementErrors::ParkingErrorExImpl)
{
    AUTO_TRACE("MinorServoBossImpl::parkImpl()");

    if(m_servo_status.starting) {
        THROW_EX(ManagementErrors, ParkingErrorEx, "The system is executing a setup.", false);
    }

    if(m_servo_status.parking)
        THROW_EX(ManagementErrors, ParkingErrorEx, "The system is executing another park.", false);

    try { 
        setElevationTrackingImpl(IRA::CString("OFF")); // Raises ConfigurationError
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::park(): some problems turning the elevation tracking off."));
    }

    //m_servo_status.parking = true;
    //m_servo_status.parking = false;
}


void 
MinorServoBossImpl::getAxesInfo(ACS::stringSeq_out axes, 
                                ACS::stringSeq_out units)
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);
{
    if(!isReady())
        THROW_EX(MinorServoErrors, 
                 StatusErrorEx, 
                 "getAxesInfo(): the system is not ready", 
                 true);

    ACS::stringSeq_var axes_res = new ACS::stringSeq;
    ACS::stringSeq_var units_res = new ACS::stringSeq;

    std::vector<std::string> a = m_actual_config->getAxes();
    std::vector<std::string> u = m_actual_config->getUnits();
    axes_res->length(a.size());
    units_res->length(u.size());
    if(a.size() != u.size())
        THROW_EX(MinorServoErrors, ConfigurationErrorEx, 
                 "getAxesInfo(): mismatch between axes and units length", true);

    for(size_t i=0; i<a.size(); i++) {
        axes_res[i] = (a[i]).c_str();
        units_res[i] = (u[i]).c_str();
    }

    axes = axes_res._retn();
    units = units_res._retn();
}

ACS::doubleSeq * 
MinorServoBossImpl::getAxesPosition(ACS::Time time) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx)
{
    if(!isReady())
        THROW_EX(MinorServoErrors, 
                 StatusErrorEx, 
                 "getAxesInfo(): the system is not ready", 
                 true);
    ACS::doubleSeq_var positions_res = new ACS::doubleSeq;
    vector<double> vpositions;
    MedMinorServoPosition position;
    if(time == 0)
        position = m_control->get_position();
    else
        position = m_control->get_position_at(time);
    vpositions = position.get_axes_positions();
    positions_res->length(vpositions.size());
    for(size_t i=0; i<vpositions.size(); i++)
        positions_res[i] = vpositions[i];
    return positions_res._retn();
}

bool MinorServoBossImpl::isStarting() { return m_servo_status.starting; }
bool MinorServoBossImpl::isASConfiguration() { return m_servo_status.as_configuration; }
bool MinorServoBossImpl::isParking() { return m_servo_status.parking; }
bool MinorServoBossImpl::isReady() { return m_servo_status.ready; }
bool MinorServoBossImpl::isScanning() { return m_servo_status.scanning; }
bool MinorServoBossImpl::isScanActive() { return m_servo_status.scan_active; }

CORBA::Boolean 
MinorServoBossImpl::command(const char *cmd, CORBA::String_out answer) 
throw (CORBA::SystemException)
{
    AUTO_TRACE("MinorServoBossImpl::command()");

    IRA::CString out;
    bool res;
    try {
        m_parser->run(cmd, out);
        res = true;
    }
    catch(ParserErrors::ParserErrorsExImpl& ex) {
        res = false;
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        ex.log(LM_ERROR); 
        res = false;
    }
    catch(ACSErr::ACSbaseExImpl& ex) {
        ex.log(LM_ERROR); 
        res = false;
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::command(): unknown exception."));
        res = false;
    }
    answer = CORBA::string_dup((const char *)out);
    return res;
}

/*
void
MinorServoBossImpl::loadAntennaBoss()
{
    if ((!CORBA::is_nil(m_antenna_boss)) && (m_antenna_boss_error)) {
        try {
            m_services->releaseComponent((const char*)m_antenna_boss->name());
        }catch (...) {}
        m_antenna_boss = Antenna::AntennaBoss::_nil();
    }
    if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
        try {
            m_antenna_boss = m_services->getDefaultComponent<Antenna::AntennaBoss>((const char*)m_config->getAntennaBossComponent());
            m_antenna_boss_error = false;
         }catch (maciErrType::CannotGetComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadAntennaBoss()");
            Impl.setComponentName((const char*)m_config->getAntennaBossComponent());
            ref=Antenna::AntennaBoss::_nil();
            throw Impl;     
        }catch (maciErrType::NoPermissionExImpl& ex){
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadAntennaBoss()");
            Impl.setComponentName((const char*)m_config->getAntennaBossComponent());
            ref=Antenna::AntennaBoss::_nil();
            throw Impl;     
        }catch (maciErrType::NoDefaultComponentExImpl& ex){
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadAntennaBoss()");
            Impl.setComponentName((const char*)m_config->getAntennaBossComponent());
            ref=Antenna::AntennaBoss::_nil();
            throw Impl;             
        }
    }
}

void
MinorServoBossImpl::unloadAntennaBoss()
{
    if (!CORBA::is_nil(ref)) {
        try {
            m_services->releaseComponent((const char*)m_antenna_boss->name());
        }catch(maciErrType::CannotReleaseComponentExImpl& ex) {
_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadAntennaBoss()");
Impl.setComponentName((const
char
*)m_config->getAntennaBossComponent());
Impl.log(LM_WARNING);
}
catch
(...)
{
_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadAntennaBoss())");
            impl.log(LM_WARNING);
                    }
                            ref=Antenna::AntennaBoss::_nil();
                                }
}*/

bool 
MinorServoBossImpl::isParked() 
throw (MinorServoErrors::ConfigurationErrorEx) 
{
    if(isStarting() || isParking() || isReady())
        return false;
    return true;
}

bool 
MinorServoBossImpl::checkFocusScan(
             const ACS::Time starting_time, 
             const double range, 
             const ACS::Time total_time
     ) throw (ManagementErrors::ConfigurationErrorEx, 
              MinorServoErrors::ScanErrorEx)
{    
    if(!isReady())
        THROW_EX(MinorServoErrors, StatusErrorEx, "checkFocusScan: the system is not ready", true);
    if(isScanning())
        THROW_EX(MinorServoErrors, ScanErrorEx, "checkFocusScan: the system is executing another scan", true);
    string axis_code = m_actual_config->is_primary_focus() ? "ZP" : "Z";
    return checkScanImpl(starting_time, range, total_time, axis_code);
}

MinorServo::TRunTimeParameters 
MinorervoBossImpl::checkScan(
        ACS::Time & starting_time, 
        MinorServo::MinorServoScan scan_parameters,
        Antenna::TRunTimeParameters antenna_parameters,
) throw (MinorServoErrors::MinorServoErrorsEx,
         ComponentErrors::ComponentErrorsEx)
{
    if(!isReady())
        THROW_EX(MinorServoErrors, ScanErrorEx, "checkScan: the system is not ready", true);
    if(isScanning())
        THROW_EX(MinorServoErrors, ScanErrorEx, "checkScan: the system is executing another scan", true);
    return checkScanImpl(starting_time, 
                         scan_parameters.range, 
                         scan_parameters.total_time, 
                         string(scan_parameters.axis_code));
}


ACS::Time 
MinorServoBossImpl::getMinScanStartingTime(
        double range, 
        const string axis_code, 
        double & acceleration, 
        double & max_speed
    )
    throw (MinorServoErrors::ConfigurationErrorExImpl, MinorServoErrors::ScanErrorEx);
{
    return ACS::Time(0.0);
}


bool MinorServoBossImpl::checkScanImpl(
        const ACS::Time starting_time, 
        double range, 
        const ACS::Time total_time, 
        const string axis_code
     ) throw (MinorServoErrors::ScanErrorEx)
{
    //TODO: this is so approximate and should change in newer versions
    MedMinorServoPosition central_position = m_control->get_position();
    MedMinorServoScan scan(central_position, starting_time, range, total_time,
                           axis_code, isElevationTracking());
    return scan.check();
}


void 
MinorServoBossImpl::startScan(
        ACS::Time & starting_time, 
        MinorServo::MinorServoScan scan_parameters,
        Antenna::TRunTimeParameters antenna_parameters,
) throw (MinorServoErrors::MinorServoErrorsEx,
         ComponentErrors::ComponentErrorsEx);
{
    if(!isReady())
        THROW_EX(MinorServoErrors, ScanErrorEx, "checkScan: the system is not ready", true);
    if(isScanActive())
        THROW_EX(MinorServoErrors, ScanErrorEx, "checkScan: the system is executing another scan", true);
    startScanImpl(starting_time, 
                  scan_parameters.range, 
                  scan_parameters.total_time, 
                  string(scan_parameters.axis_code));
}


void 
MinorServoBossImpl::startFocusScan(
             ACS::Time & starting_time, 
             const double range, 
             const ACS::Time total_time
     ) throw (MinorServoErrors::MinorServoErrorsEx,
              ComponentErrors::ComponentErrorsEx)
{    
    if(!isReady())
        THROW_EX(MinorServoErrors, ScanErrorEx, "checkScan: the system is not ready", true);
    if(isScanActive())
        THROW_EX(MinorServoErrors, ScanErrorEx, "checkScan: the system is executing another scan", true);
    /**
     * we call both primary and secondary focus axes "Z"
     */
    startScanImpl(starting_time, range, total_time, "Z");
}

void 
MinorServoBossImpl::startScanImpl(
        ACS::Time & starting_time, 
        const double range, 
        const ACS::Time total_time, 
        string axis_code
     ) throw (MinorServoErrors::ScanErrorEx)
{
    boost::recursive_mutex::scoped_lock lock(_scan_guard);
    m_servo_status.scanning = false;
    m_servo_status.scan_active = true;
    bool was_tracking = m_servo_status.elevation_tracking;
    turnTrackingOff();
    //TODO: get position at starting time
    MedMinorServoPosition central_position = m_control->get_position();
    //MedMinorServoPosition central_position = actual_conf->get_position(az,el);
    m_scan.init(central_position, starting_time, range, total_time, axis_code,
                was_tracking);
    ScanThreadParameters thread_params(m_control,
                                       &m_servo_status,
                                       m_scan);
    if(m_scan_thread_ptr != NULL)
    {
        getContainerServices()->getThreadManager()->destroy(m_scan_thread_ptr);
        m_scan_thread_ptr = NULL;
    }
    m_scan_thread_ptr = getContainerServices()->getThreadManager()->
                            create<ScanThread, ScanThreadParameters>
                             (SCAN_THREAD_NAME, thread_params);
    if(m_scan_thread_ptr->isSuspended())
        m_scan_thread_ptr->resume();
    CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::startScanImpl",
               (LM_DEBUG, "Started scan thread"));
}

void 
MinorServoBossImpl::stopScan() 
throw (MinorServoErrors::ScanErrorEx)
{
    boost::recursive_mutex::scoped_lock lock(_scan_guard);
    if(isScanActive())
    {
        if(isScanning())
        {
            CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::stopScan",
                   (LM_WARNING, "Scan interrupted during execution"));
        }
        m_servo_status.scanning = false;
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::stopScan",
               (LM_DEBUG, "returning to central scan position"));
        m_control->set_position(m_scan.getCentralPosition());
        if(m_scan.was_elevation_tracking())
        {
            CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::stopScan",
               (LM_DEBUG, "reactivating elevation tracking"));
            turnTrackingOn();
        }
        m_servo_status.scan_active = false;
    }
}

CORBA::Double 
MinorServoBossImpl::getCentralScanPosition() throw (
    MinorServoErrors::MinorServoErrorsEx,
    ComponentErrors::ComponentErrorsEx)
{
    return 0.0;
}


char *
MinorServoBossImpl::getScanAxis() {
    if(isScanActive()) {
        return CORBA::string_dup(m_scan.getAxisCode().c_str());
    }
    else {
        return CORBA::string_dup("");
    }
}

void 
MinorServoBossImpl::turnTrackingOn() 
throw (MinorServoErrors::TrackingErrorEx)
{
    if(isStarting())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOn: the system is starting.", true);
    if(isParking())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOn: the system is parking.", true);
    if(!isReady())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOn: the system is not ready.", true);

    if(m_tracking_thread_ptr != NULL) {
        m_tracking_thread_ptr->suspend();
        m_tracking_thread_ptr->terminate();
        m_tracking_thread_ptr = NULL;
    }

    m_servo_status.elevation_tracking = true;
    try {
        TrackerThreadParameters params(&m_servo_status,
                                        m_control,
                                        &m_actual_config,
                                        &m_offset,
                                        m_services);

        m_tracking_thread_ptr = getContainerServices()->
                                 getThreadManager()->
                                 create<MSBossTracker, TrackerThreadParameters>
                                 (TRACKING_THREAD_NAME, params);
    }
    catch(...) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "Error in TrackingThread", true);
    }
    m_tracking_thread_ptr->resume();
}

void MinorServoBossImpl::turnTrackingOff() throw (MinorServoErrors::TrackingErrorEx);
{
    if(m_tracking_thread_ptr != NULL) {
        m_tracking_thread_ptr->suspend();
        m_tracking_thread_ptr->terminate();
        m_tracking_thread_ptr = NULL;
    }
    m_servo_status.elevation_tracking = false;
}


void 
MinorServoBossImpl::clearUserOffset(const char *servo) throw (
    MinorServoErrors::MinorServoErrorsEx,
    ComponentErrors::ComponentErrorsEx)
{
    //TODO: add control and exception
    m_offset.clearUserOffset();
}


// Clear offset from Operator Input
void 
MinorServoBossImpl::clearOffsetsFromOI() throw (
    MinorServoErrors::OperationNotPermittedExImpl)
{
    try {
        clearUserOffset();
    }
    catch(...) {
        THROW_EX(
                MinorServoErrors, 
                OperationNotPermittedEx, 
                string("Cannot clear the offsets"), 
                false 
        )
    }
}

void 
MinorServoBossImpl::clearSystemOffset(const char *servo)
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx)
{
    //TODO: add control and exception
    m_offset.clearSystemOffset();
}

void 
MinorServoBossImpl::setUserOffset(const char *axis_code, 
                                  const double offset) 
throw ( MinorServoErrors::MinorServoErrorsEx,
        ComponentErrors::ComponentErrorsEx)
{
    try {
        setOffsetImpl(string(axis_code), offset, string("user"));
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getConfigurationErrorEx();     
    }
    catch(MinorServoErrors::OperationNotPermittedExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getOperationNotPermittedEx();     
    }
}

void 
MinorServoBossImpl::setUserOffsetFromOI(const char * axis_code, 
                                        const double & offset)
         throw (MinorServoErrors::OperationNotPermittedExImpl)
{
    try {
        setUserOffset(axis_code, offset);
    }
    catch(...) {
        THROW_EX(
                MinorServoErrors, 
                OperationNotPermittedEx, 
                string("Cannot set the offset"), 
                false 
        )
    }
}

void 
MinorServoBossImpl::setSystemOffset(const char *axis_code, 
                                    const double offset) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx)
{
    setOffsetImpl(string(axis_code), offset, string("system"));
}

void
MinorServoBossImpl::setOffsetImpl(string axis_code, 
                                   const double offset_value, 
                                   string offset_type) throw (
                                   MinorServoErrors::MinorServoErrorsEx)
{
    if(!isReady())
        THROW_EX(MinorServoErrors, StatusErrorEx, 
                 "setOffsetImpl(): the system is not ready", false);
    int axis_mapping = m_actual_config->getAxisMapping(axis_code);
    if(axis_mapping < 0)
        THROW_EX(MinorServoErrors, 
                 OperationNotPermittedEx, 
                 string("Wrong offset axis"),
                 false
        );
    if(offset_type == "user")
        m_offset.setUserOffset(axis_mapping, offset_value);
    else
        m_offset.setSystemOffset(axis_mapping, offset_value);
}


ACS::doubleSeq * MinorServoBossImpl::getUserOffset() 
     throw (MinorServoErrors::OperationNotPermittedEx, ManagementErrors::ConfigurationErrorEx)
{
    vector<double> offset;
    try {
        offset = getOffsetImpl("user");
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getConfigurationErrorEx();     
    }
    catch(MinorServoErrors::OperationNotPermittedExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getOperationNotPermittedEx();     
    }

    ACS::doubleSeq_var offset_res = new ACS::doubleSeq;
    offset_res->length(offset.size());
    for(size_t i=0; i<offset_res->length(); i++)
        offset_res[i] = offset[i];
    return offset_res._retn();
}


ACS::doubleSeq * 
MinorServoBossImpl::getSystemOffset() 
     throw (MinorServoErrors::MinorServoErrorsEx,
            ComponentErrors::ComponentErrorsEx)
{
    vector<double> offset;
    offset = getOffsetImpl("system");
    ACS::doubleSeq_var offset_res = new ACS::doubleSeq;
    offset_res->length(offset.size());
    for(size_t i=0; i<offset_res->length(); i++)
        offset_res[i] = offset[i];
    return offset_res._retn();
}

vector<double> 
MinorServoBossImpl::getOffsetImpl(string offset_type)
     throw (MinorServoErrors::MinorServoErrorsEx);
{
    if(!isReady())
        THROW_EX(MinorServoErrors, StatusError, 
                 "getOffsetImpl(): the system is not ready", false);
    vector<double> axes_values;
    if(offset_type == "user")
        return m_offset.getUserOffset();
    else
        return m_offset.getSystemOffset();
}

void 
MinorServoBossImpl::setElevationTracking(const char * value) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx)
{
    setElevationTrackingImpl(value);
}

void 
MinorServoBossImpl::setElevationTrackingImpl(const char * value) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx)
{
    string flag(value);
    if(flag == "ON")
        turnTrackingOn();
    else
        turnTrackingOff();
}

void 
MinorServoBossImpl::setASConfiguration(const char * value)
  throw (MinorServoErrors::MinorServoErrorsEx,
         ComponentErrors::ComponentErrorsEx)
{
        setASConfigurationImpl(value);
}

void MinorServoBossImpl::setASConfigurationImpl(const char * value)
{}

bool MinorServoBossImpl::isElevationTrackingEn() {
    return m_actual_config->can_track_elevation();
}

bool MinorServoBossImpl::isElevationTracking() {
    return m_servo_status.elevation_tracking;
}

bool MinorServoBossImpl::isTracking() {
    if(!isReady())
        return false;
    if(isScanActive())
        if(isScanning())
            return true;
        else
            return false;
    return m_control->is_tracking();
}

char * MinorServoBossImpl::getActualSetup() {
    //return CORBA::string_dup((m_actual_config->get_name()).c_str());
    return CORBA::string_dup(m_actual_conf.c_str());
}

char * MinorServoBossImpl::getCommandedSetup() {
    //return CORBA::string_dup((m_configuration->m_commandedSetup).c_str());
    return CORBA::string_dup((m_actual_config->get_name()).c_str());
}

void 
MinorServoBossImpl::connect()
throw (MinorServoErrors::CommunicationErrorExImpl)
{
    try{
        m_control->connect();
    }catch(ServoTimeoutError& ste){
        THROW_EX(MinorServoErrors,CommunicationErrorEx, ste.what(), false);
    }catch(const ServoConnectionError& sce){
        THROW_EX(MinorServoErrors, CommunicationErrorEx, sce.what(), false);
    }
}

void 
MinorServoBossImpl::disconnect()
throw (MinorServoErrors::CommunicationErrorExImpl)
{
    try{
        m_control->disconnect();
    }catch(ServoTimeoutError& ste){
        THROW_EX(MinorServoErrors,CommunicationErrorEx, ste.what(), false);
    }catch(const ServoConnectionError& sce){
        THROW_EX(MinorServoErrors, CommunicationErrorEx, sce.what(), false);
    }
}

void 
MinorServoBossImpl::reset()
throw (MinorServoErrors::CommunicationErrorExImpl)
{
    try{
        m_control->reset();
    }catch(const ServoTimeoutError& ste){
        THROW_EX(MinorServoErrors, CommunicationErrorEx, ste.what(), false);
    }catch(const ServoConnectionError& sce){
        THROW_EX(MinorServoErrors, CommunicationErrorEx, sce.what(), false);
    }
}

ACS::Time get_min_time(double range, double acceleration, double max_speed) {
    if(max_speed == 0 || acceleration == 0)
        return static_cast<ACS::Time>(pow((double)10, (double)15));
    /*
     * s = 1/2 * a * T**2
     * T = Vmax / a
     * s = 1/2 Vmax**2 / a
     * Acceleration and deceleration -> 2 * s = Vmax**2 / a
     * Time in acce and decc: 2*Vmax/acce
    */
    double threshold = pow(max_speed, 2) / acceleration; // 
    double min_time = range <= threshold ? sqrt(range / acceleration) : \
           2 * max_speed / acceleration + (range - threshold) / max_speed;
    return static_cast<ACS::Time>(min_time * pow((double)10, (double)7));
}

GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTSystemStatus, m_status, status);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_ready, ready);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, ACS::ROstring, m_actualSetup, actualSetup);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_starting, starting);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_asConfiguration, asConfiguration);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_elevationTrack, elevationTrack);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_scanActive, scanActive);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_scanning, scanning);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(MinorServoBossImpl)

