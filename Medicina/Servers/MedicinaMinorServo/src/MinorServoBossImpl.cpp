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
    m_motionInfo(this),
    m_tracking(this),
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
    ComponentErrors::CouldntGetComponentExImpl)
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
        THROW_EX(ComponentErrors, CouldntGetComponentEx, 
                 "cannot initialize simple supplier",
                 false);
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
        //THROW_EX(ComponentErrors,CDBAccessEx, 
        THROW_EX(ComponentErrors, CouldntGetComponentEx, 
                 "cannot read server_ip from CDB",
                 false);
    CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::initialize",
              (LM_DEBUG, "server ip: %s", (const char*)m_server_ip));
    if(!IRA::CIRATools::getDBValue(m_services, "AntennaBossInterface", m_antennaBossInterface))
        THROW_EX(ComponentErrors, CouldntGetComponentEx, 
                 "cannot read AntennaBossInterface from CDB",
                 false);
    CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::initialize",
              (LM_DEBUG, "antenna boss interface: %s", 
              (const char*)m_antennaBossInterface));
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
    try{
        //m_control = get_servo_control(m_server_ip);
        m_control.reset(new MedMinorServoControl(m_server_ip));
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::initialize",
              (LM_DEBUG, "Instantiated new minor servo control"));
    }catch(const ServoConnectionError& sce){
        /**
        THROW_EX(ComponentErrors, CouldntGetComponentEx, 
                 sce.what(),
                 false);
        */
        m_control.reset();
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::initialize",
              (LM_ERROR, sce.what()));
    }catch(const ServoTimeoutError& ste){
        THROW_EX(ComponentErrors, CouldntGetComponentEx, 
                 ste.what(),
                 false);
    }

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
    m_parser->add(
            "servoReset", 
            new function0<MinorServoBossImpl, non_constant, void_type>(this, &MinorServoBossImpl::reset), 
            0
    );
    m_parser->add(
            "servoConnect", 
            new function0<MinorServoBossImpl, non_constant, void_type>(this, &MinorServoBossImpl::connect), 
            0
    );
    m_parser->add(
            "servoDisconnect", 
            new function0<MinorServoBossImpl, non_constant, void_type>(this, &MinorServoBossImpl::disconnect), 
            0
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
		m_motionInfo = new baci::ROstring(getContainerServices()->getName() + ":motionInfo",
                getComponent(), new DevIOMotionInfo(&m_servo_status, m_control), true);
        m_tracking = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":tracking",getComponent(), \
                   new DevIOTracking(&m_servo_status, m_control), true);
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
throw (MinorServoErrors::SetupErrorExImpl)
{
    if(!(m_control))
        THROW_EX(MinorServoErrors, SetupErrorEx, 
                 "Minor Servo Server is not connected", false);
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
    }catch(...) {
        THROW_EX(MinorServoErrors, SetupErrorEx, "cannot turn the tracking off",
        false);
    }
    try{
        m_actual_config = &(m_config[std::string(config)]);
    }catch(...) {
        THROW_EX(MinorServoErrors, SetupErrorEx, "invalid configuration", false);
    }
    try{
        m_servo_status.reset();
        m_servo_status.starting = true;
    }catch(const ServoTimeoutError& ste){
        THROW_EX(MinorServoErrors, SetupErrorEx, ste.what(), false);
    }catch(const ServoConnectionError& sce){
        THROW_EX(MinorServoErrors, SetupErrorEx, sce.what(), false);
    }catch(...){
        m_servo_status.starting = false;
        THROW_EX(MinorServoErrors, SetupErrorEx, "Cannot conclude setup", false);
    }
    /**
     * Get the setup position at 45 deg
     */
    MedMinorServoPosition setup_position = m_actual_config->get_position();
    try{
        m_offset.initialize(m_actual_config->is_primary_focus());
    }catch(MinorServoOffsetError& msoe){
        THROW_EX(MinorServoErrors, SetupErrorEx, msoe.what(), false);
    }
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
        
        if(isElevationTrackingEn())
            turnTrackingOn();

    }catch(const ServoTimeoutError& ste){
        THROW_EX(MinorServoErrors, SetupErrorEx, ste.what(), false);
    }catch(const ServoConnectionError& sce){
        THROW_EX(MinorServoErrors, SetupErrorEx, sce.what(), false);
    }catch(...){
        m_servo_status.starting = false;
        THROW_EX(MinorServoErrors, SetupErrorEx, "Cannot conclude setup >> "+boost::current_exception_diagnostic_information(), false);
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
                 ComponentErrors::ComponentErrorsEx)
{
    if(!isReady())
        THROW_MINORSERVO_EX(StatusErrorEx, 
                 "getAxesInfo(): the system is not ready", 
                 true);

    ACS::stringSeq_var axes_res = new ACS::stringSeq;
    ACS::stringSeq_var units_res = new ACS::stringSeq;

    std::vector<std::string> a = m_actual_config->getAxes();
    std::vector<std::string> u = m_actual_config->getUnits();
    axes_res->length(a.size());
    units_res->length(u.size());
    if(a.size() != u.size())
        THROW_MINORSERVO_EX(ConfigurationErrorEx, 
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
    if(!(m_control))
        THROW_MINORSERVO_EX(CommunicationErrorEx, 
                 "Minor Servo Server is not connected", false);
    if(!isReady())
        THROW_MINORSERVO_EX(StatusErrorEx, 
                 "getAxesInfo(): the system is not ready", 
                 true);
    ACS::doubleSeq_var positions_res = new ACS::doubleSeq;
    vector<double> vpositions;
    MedMinorServoPosition position;
    try{
        if(time == 0)
            position = m_control->get_position();
        else
            position = m_control->get_position_at(time);
    }catch(const ServoTimeoutError& ste){
        THROW_MINORSERVO_EX(CommunicationErrorEx, ste.what(), false);
    }catch(const ServoConnectionError& sce){
        THROW_MINORSERVO_EX(CommunicationErrorEx, sce.what(), false);
    }catch(...){
        THROW_MINORSERVO_EX(CommunicationErrorEx, "Cannot get position", false);
    }
    //We hide the system offsets from the user view
    position -= m_offset.getSystemOffsetPosition();
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
     ) throw (MinorServoErrors::StatusErrorEx,
              MinorServoErrors::ScanErrorEx)
{    
    if(!isReady())
        THROW_EX(MinorServoErrors, StatusErrorEx, "checkFocusScan: the system is not ready", true);
    if(isScanning())
        THROW_EX(MinorServoErrors, ScanErrorEx, "checkFocusScan: the system is executing another scan", true);
    string axis_code = m_actual_config->is_primary_focus() ? "ZP" : "Z";
    //return checkScanImpl(starting_time, range, total_time, axis_code);

    //TODO: fake implementation we shold remove this method or make it conform
    //to the same interface of checkScan
    return true; 
}

bool 
MinorServoBossImpl::checkScan(
        ACS::Time starting_time, 
        const MinorServo::MinorServoScan& scan_parameters,
        const Antenna::TRunTimeParameters& antenna_parameters,
        TRunTimeParameters_out minor_servo_parameters
) throw (MinorServoErrors::MinorServoErrorsEx,
         ComponentErrors::ComponentErrorsEx)
{
    if(!isReady())
        THROW_MINORSERVO_EX(ScanErrorEx, "checkScan: the system is not ready", true);
    if(isScanning())
        THROW_MINORSERVO_EX(ScanErrorEx, "checkScan: the system is executing another scan", true);
    return checkScanImpl(starting_time, 
                         scan_parameters,
                         antenna_parameters,
                         minor_servo_parameters);
}


ACS::Time 
MinorServoBossImpl::getMinScanStartingTime(
        double range, 
        const string axis_code, 
        double & acceleration, 
        double & max_speed
    )
    throw (MinorServoErrors::ConfigurationErrorExImpl, MinorServoErrors::ScanErrorEx)
{
    return ACS::Time(0.0);
}


bool 
MinorServoBossImpl::checkScanImpl(
        ACS::Time starting_time, 
        const MinorServo::MinorServoScan& scan_parameters,
        const Antenna::TRunTimeParameters& antenna_parameters,
        TRunTimeParameters_out minor_servo_parameters
     ) throw (MinorServoErrors::MinorServoErrorsEx,
              ComponentErrors::ComponentErrorsEx)
{
    minor_servo_parameters = new TRunTimeParameters;
    double center = 0;
    MedMinorServoPosition central_position = 
        m_actual_config->get_position(antenna_parameters.elevation,&m_offset);

    MedMinorServoScan scan(central_position, 
                           starting_time, 
                           scan_parameters.range,
                           scan_parameters.total_time,
                           std::string(scan_parameters.axis_code), 
                           isElevationTracking());
    minor_servo_parameters->startEpoch = scan.getStartingTime();
    try{
        center = central_position.get_axis_position(
                                        scan_parameters.axis_code);
    }catch(const MinorServoAxisNameError& msane){
        CUSTOM_LOG(LM_FULL_INFO, 
                   "MinorServo::MinorServoBossImpl::checkScanImpl",
                   (LM_WARNING, "Wrong axis name, defaulting to Z"));
        center = central_position.z;
    }
    minor_servo_parameters->centerScan = center;
    minor_servo_parameters->scanAxis = CORBA::string_dup(
                                         scan_parameters.axis_code);
    minor_servo_parameters->timeToStop = scan.getStopTime();
    if(scan_parameters.is_empty_scan)
    {
        minor_servo_parameters->onTheFly = false;
        //minor_servo_parameters->timeToStop = 0;
        return true;
    }
    if(scan.check())
    {
        minor_servo_parameters->onTheFly = true;
        //minor_servo_parameters->timeToStop = scan.getStopTime();
        return true;
    }else{
        return false;
    }
}


void 
MinorServoBossImpl::startScan(
        ACS::Time & starting_time, 
        const MinorServo::MinorServoScan& scan_parameters,
        const Antenna::TRunTimeParameters& antenna_parameters
) throw (MinorServoErrors::MinorServoErrorsEx,
         ComponentErrors::ComponentErrorsEx)
{
    if(!isReady())
        THROW_MINORSERVO_EX(ScanErrorEx, "startScan: the system is not ready", true);
    if(isScanActive())
        THROW_MINORSERVO_EX(ScanErrorEx, "startScan: the system is executing another scan", true);
    startScanImpl(starting_time, 
              scan_parameters,
              antenna_parameters);
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
        THROW_MINORSERVO_EX(ScanErrorEx, "startFocusScan: the system is not ready", true);
    if(isScanActive())
        THROW_MINORSERVO_EX(ScanErrorEx, "startFocusScan: the system is executing another scan", true);
    string axis_code = m_actual_config->is_primary_focus() ? "ZP" : "Z";
    //TODO: fake implementation we shold remove this method or make it conform
    //to the same interface of checkScan
    //startScanImpl(starting_time, range, total_time, axis_code);
}

void 
MinorServoBossImpl::startScanImpl(
        ACS::Time & starting_time, 
        const MinorServo::MinorServoScan& scan_parameters,
        const Antenna::TRunTimeParameters& antenna_parameters
     ) throw (MinorServoErrors::MinorServoErrorsEx)
{
    if(!(m_control))
        THROW_MINORSERVO_EX(CommunicationErrorEx, 
                 "Minor Servo Server is not connected", false);
    if(scan_parameters.is_empty_scan)
        return;
    boost::recursive_mutex::scoped_lock lock(_scan_guard);
    m_servo_status.scan_active = true;
    m_servo_status.scanning = false;
    bool was_elevation_tracking = isElevationTracking();
    turnTrackingOff();
    MedMinorServoPosition central_position = 
        m_actual_config->get_position(antenna_parameters.elevation,&m_offset);
    MedMinorServoScan scan(central_position, 
                           starting_time, 
                           scan_parameters.range,
                           scan_parameters.total_time,
                           std::string(scan_parameters.axis_code), 
                           was_elevation_tracking);
    if(!(scan.check()))
    {
        m_servo_status.scan_active = false;
        THROW_MINORSERVO_EX(ScanErrorEx, "startScanImpl: cannot start scan", true);
    }

    starting_time = scan.getStartingTime();
    CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::startScanImpl",
               (LM_INFO, "scan will start at: %llu", starting_time));
    m_scan = scan;
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
               (LM_INFO, "Started minor servo scan"));
}

void 
MinorServoBossImpl::closeScan(ACS::Time &timeToStop) 
throw (MinorServoErrors::MinorServoErrorsEx,
       ComponentErrors::ComponentErrorsEx)
{
    if(!(m_control))
        THROW_MINORSERVO_EX(CommunicationErrorEx, 
                 "Minor Servo Server is not connected", false);
    boost::recursive_mutex::scoped_lock lock(_scan_guard);
    if(isScanActive())
    {
        if(isScanning())
        {
            CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::closeScan",
                   (LM_NOTICE, "Scan interrupted during execution"));
            m_servo_status.scanning = false;
        }
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::closeScan",
               (LM_DEBUG, "returning to central scan position"));
        double min_time = MedMinorServoGeometry::min_time(
                            m_control->get_position(),
                            m_scan.getCentralPosition());
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::closeScan",
               (LM_DEBUG, "time to stop position: %f", min_time));
        ACS::TimeInterval min_time_interval = MedMinorServoTime::deltaToACSTimeInterval(min_time);
        ACS::Time now = IRA::CIRATools::getACSTime();
        m_control->set_position(m_scan.getCentralPosition());
        timeToStop = now + min_time_interval + SCAN_TOLERANCE;
        if(m_scan.was_elevation_tracking())
        {
            CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::closeScan",
               (LM_DEBUG, "reactivating elevation tracking"));
            turnTrackingOn();
        }
        m_servo_status.scan_active = false;
    }else{
        timeToStop = 0;
	CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBossImpl::closeScan",
               (LM_DEBUG, "No scan presently active"));
        //THROW_MINORSERVO_EX(StatusErrorEx, "no scan active", true);
    }
}

CORBA::Double 
MinorServoBossImpl::getCentralScanPosition() throw (
    MinorServoErrors::MinorServoErrorsEx,
    ComponentErrors::ComponentErrorsEx)
{
    if(isScanActive()) {
        double center;
        MedMinorServoPosition central_position = m_scan.getCentralPosition();
        std::string axis_code = m_scan.getAxisCode();
        try{
            center = central_position.get_axis_position(axis_code.c_str());
            return center;
        }catch(...){
            THROW_MINORSERVO_EX(ScanErrorEx, "Cannot get centarl position", true);
        }
    } else {
            THROW_MINORSERVO_EX(StatusErrorEx, "no scan active", true);
    }
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
throw (MinorServoErrors::MinorServoErrorsEx)
{
    if(!(m_control))
        THROW_MINORSERVO_EX(CommunicationErrorEx, 
                 "Minor Servo Server is not connected", false);
    //if(isStarting())
    //    THROW_MINORSERVO_EX(TrackingErrorEx, "turnTrackingOn: the system is starting.", true);
    if(isParking())
        THROW_MINORSERVO_EX(TrackingErrorEx, "turnTrackingOn: the system is parking.", true);
    //if(!isReady())
    //    THROW_MINORSERVO_EX(TrackingErrorEx, "turnTrackingOn: the system is not ready.", true);

    if(m_tracking_thread_ptr != NULL) {
        m_tracking_thread_ptr->suspend();
        m_tracking_thread_ptr->terminate();
        m_tracking_thread_ptr = NULL;
    }

    //m_servo_status.elevation_tracking = true;
    try {
        TrackerThreadParameters params(&m_servo_status,
                                        m_control,
                                        &m_actual_config,
                                        &m_offset,
                                        m_services,
                                        std::string((const char*)m_antennaBossInterface));

        m_tracking_thread_ptr = getContainerServices()->
                                 getThreadManager()->
                                 create<MSBossTracker, TrackerThreadParameters>
                                 (TRACKING_THREAD_NAME, params);
    }
    catch(...) {
        THROW_MINORSERVO_EX(TrackingErrorEx, "Error in TrackingThread", true);
    }
    m_tracking_thread_ptr->resume();
}

void MinorServoBossImpl::turnTrackingOff() throw (
    MinorServoErrors::MinorServoErrorsEx)
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
    try{
        m_offset.clearUserOffset();
        setCorrectPosition();
    }catch(MinorServoOffsetError& msoe){
        THROW_MINORSERVO_EX(OffsetErrorEx, 
                 msoe.what(), false);
    }
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
    try{
        m_offset.clearSystemOffset();
        setCorrectPosition();
    }catch(MinorServoOffsetError& msoe){
        THROW_MINORSERVO_EX(OffsetErrorEx, 
                 msoe.what(), true);
    }
}

void 
MinorServoBossImpl::setUserOffset(const char *axis_code, 
                                  const double offset) 
throw ( MinorServoErrors::MinorServoErrorsEx,
        ComponentErrors::ComponentErrorsEx)
{
    setOffsetImpl(string(axis_code), offset, string("user"));
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
MinorServoBossImpl::setCorrectPosition()
{
    if(isElevationTracking()){
        return;
    }
    MedMinorServoPosition offset_position = m_offset.getOffsetPosition();
    MedMinorServoPosition correct_position = m_actual_config->get_position();
    try{
        m_control->set_position(correct_position + offset_position);
        CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MinorServoBoss::setCorrectPosition",
              (LM_DEBUG, "Correcting position"));
    }catch(const ServoTimeoutError& ste){
        THROW_MINORSERVO_EX(CommunicationErrorEx, ste.what(), true);
    }catch(const ServoConnectionError& sce){
        THROW_MINORSERVO_EX(CommunicationErrorEx, sce.what(), true);
    }catch(const ServoPositionError& spe){
        THROW_MINORSERVO_EX(PositioningErrorEx, spe.what(), true);
    }
}

void
MinorServoBossImpl::setOffsetImpl(string axis_code, 
                                   const double offset_value, 
                                   string offset_type) throw (
                                   MinorServoErrors::MinorServoErrorsEx)
{
    if(!isReady())
        THROW_MINORSERVO_EX(StatusErrorEx, 
                 "setOffsetImpl(): the system is not ready", false);
    int axis_mapping = m_actual_config->getAxisMapping(axis_code);
    if(axis_mapping < 0)
        THROW_MINORSERVO_EX( OffsetErrorEx, 
                 string("Wrong offset axis"),
                 true);
    try{
        if(offset_type == "user")
            m_offset.setUserOffset(axis_mapping, offset_value);
        else
            m_offset.setSystemOffset(axis_mapping, offset_value);
        setCorrectPosition();
    }catch(MinorServoOffsetError& msoe){
        THROW_MINORSERVO_EX(OffsetErrorEx, 
                 msoe.what(), true);
    }
}


ACS::doubleSeq * MinorServoBossImpl::getUserOffset(ACS::Time acs_time)
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx)
{
    //TODO No matter the given acs_time, this implementation only returns the current offsets
    //     It would be ideal to have a queue of offsets and retrieve them starting from the given acs_time
    vector<double> offset;
    offset = getOffsetImpl("user");
    ACS::doubleSeq_var offset_res = new ACS::doubleSeq;
    offset_res->length(offset.size());
    for(size_t i=0; i<offset_res->length(); i++)
        offset_res[i] = offset[i];
    return offset_res._retn();
}


ACS::doubleSeq * 
MinorServoBossImpl::getSystemOffset(ACS::Time acs_time)
     throw (MinorServoErrors::MinorServoErrorsEx,
            ComponentErrors::ComponentErrorsEx)
{
    //TODO No matter the given acs_time, this implementation only returns the current offsets
    //     It would be ideal to have a queue of offsets and retrieve them starting from the given acs_time
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
     throw (MinorServoErrors::MinorServoErrorsEx)
{
    if(!isReady())
        THROW_MINORSERVO_EX(StatusErrorEx, 
                 "getOffsetImpl(): the system is not ready", true);
    vector<double> axes_values;
    try{
        if(offset_type == "user")
            return m_offset.getUserOffset();
        else
            return m_offset.getSystemOffset();
    }catch(MinorServoOffsetError& msoe){
        THROW_MINORSERVO_EX(OffsetErrorEx, 
                 msoe.what(), true);
    }
}

void 
MinorServoBossImpl::setElevationTracking(const char * value) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx)
{
    try{
        setElevationTrackingImpl(value);
    }catch(const MinorServoErrors::MinorServoErrorsExImpl& mse){
        THROW_MINORSERVO_EX(OperationNotPermittedEx, 
                 "setElevationTracking(): error setting tracking info", 
                 true);
    }
}

void 
MinorServoBossImpl::setElevationTrackingImpl(const char * value) 
          throw (MinorServoErrors::MinorServoErrorsExImpl)
{
    string flag(value);
    try{
        if(flag == "ON")
            turnTrackingOn();
        else
            turnTrackingOff();
    }catch(const MinorServoErrors::MinorServoErrorsEx & mse)
    {
        THROW_EX(
                MinorServoErrors, 
                OperationNotPermittedEx, 
                "cannot set elevation tracking",
                false );
    }
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
        if(m_control)
            m_control->connect();
        else
            m_control.reset(new MedMinorServoControl(m_server_ip));
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
        if(m_control)
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
        if(m_control)
            m_control->reset();
    }catch(const ServoTimeoutError& ste){
        THROW_EX(MinorServoErrors, CommunicationErrorEx, ste.what(), false);
    }catch(const ServoConnectionError& sce){
        THROW_EX(MinorServoErrors, CommunicationErrorEx, sce.what(), false);
    }
}

void 
MinorServoBossImpl::getServoTime(ACS::Time &servoTime)
throw (MinorServoErrors::CommunicationErrorExImpl)
{
    try{
        m_control->update_position();
        MedMinorServoPosition position = m_control->get_position();
        servoTime = position.time;
    }catch(...){
        THROW_EX(MinorServoErrors, CommunicationErrorEx, "cannot get time", false);
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
GET_PROPERTY_REFERENCE(MinorServoBossImpl, ACS::ROstring, m_motionInfo, motionInfo);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_tracking, tracking);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_starting, starting);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_asConfiguration, asConfiguration);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_elevationTrack, elevationTrack);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_scanActive, scanActive);
GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTBoolean, m_scanning, scanning);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(MinorServoBossImpl)

