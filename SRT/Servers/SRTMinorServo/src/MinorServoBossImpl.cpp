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
#include "MinorServoBossImpl.h"
#include "SubsystemStatusDevIO.h"
#include "SubsystemVStatusDevIO.h"
#include "DevIOActualSetup.h"
#include "DevIOReady.h"
#include "DevIOParking.h"
#include "DevIOScanActive.h"
#include "DevIOStarting.h"
#include "DevIOASConfiguration.h"
#include "DevIOScanning.h"
#include "DevIOElevationTrack.h"



#include "utils.h"
#include "macros.def"
#include "SetupThread.h"
#include <cstring>
#include "acstime.h"
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "libCom.h"
#include "MSBossConfiguration.h"

using namespace SimpleParser;

static pthread_mutex_t tracking_mutex = PTHREAD_MUTEX_INITIALIZER;

MSThreadParameters MinorServoBossImpl::m_thread_params;


MinorServoBossImpl::MinorServoBossImpl(
        const ACE_CString &CompName, maci::ContainerServices *containerServices
        ) : 
    CharacteristicComponentImpl(CompName, containerServices),
    m_services(containerServices),
    m_status(this),
    m_ready(this),
    m_actualSetup(this),
    m_starting(this),
    m_asConfiguration(this),
    m_elevationTrack(this),
    m_scanActive(this),
    m_scanning(this),
    // m_verbose_status(this),
    m_nchannel(NULL)
{   
    AUTO_TRACE("MinorServoBossImpl::MinorServoBossImpl()");
    m_commanded_conf = "none";
    m_actual_conf = "none";
    m_tracking_thread_ptr = NULL;
    m_setup_thread_ptr = NULL;
    m_park_thread_ptr = NULL;
    m_scan_thread_ptr = NULL;
    m_publisher_thread_ptr = NULL;
    m_thread_params.is_setup_locked = false;
    m_thread_params.is_parking_locked = false;
    m_thread_params.is_initialized = false;
    m_thread_params.is_scanning_ptr = NULL;
    m_status_value = Management::MNG_OK;
    (m_thread_params.scan_data).positioning_time = 0;
    m_servo_scanned = "none";
    m_configuration = new MSBossConfiguration(m_services, this);
    m_parser= new SimpleParser::CParser<MinorServoBossImpl>(this, 10);
}


MinorServoBossImpl::~MinorServoBossImpl() { 
    AUTO_TRACE("MinorServoBossImpl::~MinorServoBossImpl()"); 

    if(m_configuration != NULL)
        delete m_configuration;

    if(m_parser != NULL)
        delete m_parser;
}


void MinorServoBossImpl::initialize() throw (ComponentErrors::CouldntGetComponentExImpl, ManagementErrors::ConfigurationErrorExImpl)
{
    AUTO_TRACE("MinorServoBossImpl::initialize()");
    // Retrive the slaves parameter from CDB (a string of slaves to manage)
    if(!CIRATools::getDBValue(getContainerServices(), "slaves", m_cdb_slaves))
        THROW_EX(ComponentErrors, CDBAccessEx, "I cannot read 'slaves' from CDB", false);

    try {
        m_nchannel = new nc::SimpleSupplier(MinorServo::MINORSERVO_DATA_CHANNEL, this);
        m_configuration->m_nchannel = m_nchannel;
    }
    catch (...) {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"MinorServoBoss::initialize()");
    }

    // The m_vstatus_flags is an alias of m_thread_params.is_initialized
    m_vstatus_flags.is_initialized = &(m_thread_params.is_initialized = false);
    m_vstatus_flags.is_parking = false;
    m_tracking_list.clear();
    // Get the component references.
    vector<string> slaves = get_slaves();
    for(vector<string>::iterator iter = slaves.begin(); iter != slaves.end(); iter++) {
        if(!m_component_refs.count(*iter)) {
            try {
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
                component_ref = m_services->getComponent<MinorServo::WPServo>(("MINORSERVO/" + *iter).c_str());
                if(!CORBA::is_nil(component_ref))
                    m_component_refs[*iter] = component_ref;
            }
            catch(...) {
                ACS_SHORT_LOG((LM_WARNING, ("MinorServoBossImpl::initialize(): error getting " + *iter + "component").c_str()));
            }
        }
    }
    m_thread_params.nchannel = m_nchannel;
    m_thread_params.status = &m_status_value;
    m_thread_params.component_refs = &m_component_refs;
    m_thread_params.services = m_services;
    m_thread_params.commanded_conf = m_commanded_conf;
    m_thread_params.actual_conf = &m_actual_conf;
    (m_thread_params.actions).clear();

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


void MinorServoBossImpl::execute() throw (ComponentErrors::MemoryAllocationExImpl)
{
    AUTO_TRACE("MinorServoBossImpl::execute()");

    try {       
        m_status = new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus>(
                getContainerServices()->getName() + ":status", 
                getComponent(), 
                new SubsystemStatusDevIO(&(m_configuration->m_status)), 
                true
        );  

        m_ready = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":ready",getComponent(), \
                   new DevIOReady(m_configuration),true);
		m_actualSetup = new baci::ROstring(getContainerServices()->getName() + ":actualSetup",
                getComponent(), new DevIOActualSetup(m_configuration), true);

        m_starting = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":starting",getComponent(), \
                   new DevIOStarting(m_configuration),true);
        m_asConfiguration = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":asConfiguration",getComponent(), \
                   new DevIOASConfiguration(m_configuration),true);
        m_elevationTrack = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":elevationTrack",getComponent(), \
                   new DevIOElevationTrack(m_configuration),true);
        m_scanActive = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":scanActive",getComponent(), \
                   new DevIOScanActive(m_configuration),true);
        m_scanning = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>\
                  (getContainerServices()->getName()+":scanning",getComponent(), \
                   new DevIOScanning(m_configuration),true);

        // m_verbose_status = new ROpattern(
        //         getContainerServices()->getName() + ":verbose_status", 
        //         getComponent(), 
        //         new SubsystemVStatusDevIO<ACS::pattern>(SubsystemVStatusDevIO<ACS::pattern>::VERBOSE_STATUS, &m_vstatus_flags), 
        //         true
        // );
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
        m_publisher_thread_ptr = getContainerServices()->getThreadManager()->create<MSBossPublisher,
            MSBossConfiguration *>("MSBossPublisher", m_configuration);
        m_publisher_thread_ptr->resume();
    }
    catch(...) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "Error: MSBossPublisher already exists", true);
    }

}


void MinorServoBossImpl::cleanUp() 
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

    if (m_setup_thread_ptr != NULL) {
        m_setup_thread_ptr->suspend();
        m_setup_thread_ptr->terminate();
    }

    if (m_park_thread_ptr != NULL) {
        m_park_thread_ptr->suspend();
        m_setup_thread_ptr->terminate();
    }

    map<string, MinorServo::WPServo_var>::reverse_iterator rbegin_iter = m_component_refs.rbegin(); 
    map<string, MinorServo::WPServo_var>::reverse_iterator rend_iter = m_component_refs.rend(); 
    for(map<string, MinorServo::WPServo_var>::reverse_iterator rviter = rbegin_iter; rviter != rend_iter; rviter++)
        m_services->releaseComponent(("MINORSERVO/" + rviter->first).c_str());

    m_component_refs.clear();
    m_tracking_list.clear();

    if(m_nchannel !=NULL ) {
        m_nchannel->disconnect();
        m_nchannel = NULL;
    }

    CharacteristicComponentImpl::cleanUp(); 
}


void MinorServoBossImpl::aboutToAbort()
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

    if (m_setup_thread_ptr != NULL) {
        m_setup_thread_ptr->suspend();
        m_setup_thread_ptr->terminate();
    }

    if (m_park_thread_ptr != NULL) {
        m_park_thread_ptr->suspend();
        m_park_thread_ptr->terminate();
    }

    map<string, MinorServo::WPServo_var>::reverse_iterator rbegin_iter = m_component_refs.rbegin(); 
    map<string, MinorServo::WPServo_var>::reverse_iterator rend_iter = m_component_refs.rend(); 
    for(map<string, MinorServo::WPServo_var>::reverse_iterator rviter = rbegin_iter; rviter != rend_iter; rviter++)
        m_services->releaseComponent(("MINORSERVO/" + rviter->first).c_str());

    m_component_refs.clear();
    m_tracking_list.clear();

    if(m_nchannel != NULL ) {
        m_nchannel->disconnect();
        m_nchannel = NULL;
    }

    CharacteristicComponentImpl::aboutToAbort(); 
}


void MinorServoBossImpl::setup(const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx)
{
    AUTO_TRACE("MinorServoBossImpl::setup()");

    try {
        setupImpl(config);
    }
    catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getConfigurationErrorEx();     
    }
}

void MinorServoBossImpl::setupImpl(const char *config) throw (ManagementErrors::ConfigurationErrorExImpl)
{
    try {
        if(!endswith(string(config), "P")) {
            if(!m_configuration->isElevationTrackingEn()) {
                setElevationTrackingImpl(IRA::CString("ON"));
            }
        }
        else {
            if(m_configuration->isElevationTrackingEn()) {
                turnTrackingOff();
            }
        }
    }
    catch(...) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "cannot turn the tracking on", false);
    }

    if(m_configuration->isStarting())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "The system is executing another setup", false);

    if(m_configuration->isParking())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "The system is executing a park", false);

    if(!endswith(string(config), "P")) {
        m_configuration->setASConfiguration(IRA::CString("ON"));
    }
    m_configuration->init(string(config));  // Throw (ComponentErrors::CDBAccessExImpl);

    try {
        if(m_setup_thread_ptr != NULL)
            m_setup_thread_ptr->restart();
        else {
            m_setup_thread_ptr = getContainerServices()->getThreadManager()->create<SetupThread,
               MSBossConfiguration *>("SetupThread", m_configuration);
            m_setup_thread_ptr->resume();
        }
    }
    catch(...) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "The MinorServoBoss is attempting to execute a previous setup", false);
    }
}


void MinorServoBossImpl::park() throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx)
{
    try {
        parkImpl();
    }
    catch (ManagementErrors::ParkingErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getParkingErrorEx();       
    }
}

void MinorServoBossImpl::parkImpl() throw (ManagementErrors::ParkingErrorExImpl)
{
    AUTO_TRACE("MinorServoBossImpl::parkImpl()");

    if(m_configuration->isStarting()) {
        THROW_EX(ManagementErrors, ParkingErrorEx, "The system is executing a setup.", false);
    }

    if(m_configuration->isParking())
        THROW_EX(ManagementErrors, ParkingErrorEx, "The system is executing another park.", false);

    try { 
        if(m_configuration->isElevationTrackingEn())
            setElevationTrackingImpl(IRA::CString("OFF")); // Raises ConfigurationError
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::park(): some problems turning the elevation tracking off."));
    }

    m_configuration->m_isParking = true;
    m_configuration->setASConfiguration("OFF");

    try {
        if(m_park_thread_ptr != NULL)
            m_park_thread_ptr->restart();
        else {
            m_park_thread_ptr = getContainerServices()->getThreadManager()->create<ParkThread,
               MSBossConfiguration *>("ParkThread", m_configuration);
            m_park_thread_ptr->resume();
        }
    }
    catch(...) {
        THROW_EX(ManagementErrors, ParkingErrorEx, "The MinorServoBoss is attempting to execute a previous park", false);
    }
}


void MinorServoBossImpl::getAxesInfo(ACS::stringSeq_out axes, ACS::stringSeq_out units) throw (
            CORBA::SystemException, 
            MinorServoErrors::MinorServoErrorsEx, 
            ComponentErrors::ComponentErrorsEx)
{

    if(!isReady()) {
        string msg("getAxesInfo(): the system is not ready");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    ACS::stringSeq_var axes_res = new ACS::stringSeq;
    ACS::stringSeq_var units_res = new ACS::stringSeq;

    vector<string> a = m_configuration->getAxes();
    vector<string> u = m_configuration->getUnits();
    axes_res->length(a.size());
    units_res->length(u.size());

    if(a.size() != u.size()) {
        string msg("getAxesInfo(): mismatch between axes and units length");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    for(size_t i=0; i<a.size(); i++) {
        axes_res[i] = (a[i]).c_str();
        units_res[i] = (u[i]).c_str();
    }

    axes = axes_res._retn();
    units = units_res._retn();
}


ACS::doubleSeq * MinorServoBossImpl::getAxesPosition(ACS::Time time) throw (
            CORBA::SystemException, 
            MinorServoErrors::MinorServoErrorsEx, 
            ComponentErrors::ComponentErrorsEx)
{

    if(!isReady()) {
        string msg("getAxesPosition(): the system is not ready");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    vector<string> servosToMove = m_configuration->getServosToMove();

    ACS::doubleSeq_var positions_res = new ACS::doubleSeq;
    ACS::doubleSeq_var position = new ACS::doubleSeq;
    vector<double> vpositions;

    for(size_t i=0; i<servosToMove.size(); i++) {
        string comp_name = servosToMove[i];
        MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
        if((m_configuration->m_component_refs).count(comp_name)) {
            component_ref = (m_configuration->m_component_refs)[comp_name];
            if(CORBA::is_nil(component_ref)) {
                string msg("getAxesPosition(): cannot get the reference of " + comp_name);
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_DEBUG);
                throw impl.getMinorServoErrorsEx();
            }
        }
        else {
            string msg("getAxesPosition(): " + comp_name);
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }

        try {
            position = component_ref->getPositionFromHistory(time); // Raises ComponentErrors::UnexpectedEx
        }
        catch (...) {
            string msg("getAxesPosition(): cannot get the history");
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }

        for(size_t j=0; j<position->length(); j++)
            vpositions.push_back(position[j]);
    }

    positions_res->length(vpositions.size());
    for(size_t i=0; i<vpositions.size(); i++)
        positions_res[i] = vpositions[i];

    return positions_res._retn();
}


bool MinorServoBossImpl::isStarting() { return m_configuration->isStarting(); }

bool MinorServoBossImpl::isASConfiguration() { return m_configuration->isASConfiguration(); }

bool MinorServoBossImpl::isParking() { return m_configuration->isParking(); }

bool MinorServoBossImpl::isReady() { return m_configuration->isConfigured(); }

bool MinorServoBossImpl::isScanning() { return m_configuration->m_isScanning; }

bool MinorServoBossImpl::isScanActive() { return m_configuration->m_isScanActive; }


CORBA::Boolean MinorServoBossImpl::command(const char *cmd, CORBA::String_out answer) throw (CORBA::SystemException)
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


bool MinorServoBossImpl::slave_exists(string sname) {

    vector<string> slaves = get_slaves();
    for(vector<string>::iterator iter = slaves.begin(); iter != slaves.end(); iter++)
        if(*iter == sname)
            return true;
    return false;
}

vector<string> MinorServoBossImpl::get_slaves()
{
    string cdb_slaves(m_cdb_slaves);
    vector<string> slaves = split(cdb_slaves, slaves_separator);
    for(vector<string>::iterator iter = slaves.begin(); iter != slaves.end(); iter++)
        strip(*iter);
    return slaves;
}


bool MinorServoBossImpl::isParked() throw (ManagementErrors::ConfigurationErrorEx) {

    if(isStarting() || isParking())
        return false;

    map<string, MinorServo::WPServo_var>::iterator begin_iter = m_component_refs.begin(); 
    map<string, MinorServo::WPServo_var>::iterator end_iter = m_component_refs.end(); 
    for(map<string, MinorServo::WPServo_var>::iterator viter = begin_iter; viter != end_iter; viter++)
        if(!CORBA::is_nil(viter->second))
            if((viter->second)->isReady())
                return false;

    return true;
}

void MinorServoBossImpl::closeScan(ACS::Time& timeToStop) throw (
             MinorServoErrors::MinorServoErrorsEx, 
             ComponentErrors::ComponentErrorsEx)
{
	timeToStop=0;
    if(m_configuration->m_isScanActive) {
        string comp_name((m_configuration->m_scan).comp_name);
        if(m_component_refs.count(comp_name)) {
            MinorServo::WPServo_var component_ref = m_component_refs[comp_name];
            try {
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
                component_ref = m_services->getComponent<MinorServo::WPServo>(("MINORSERVO/" + comp_name).c_str());
                if(!CORBA::is_nil(component_ref)) {
                    if(m_scan_thread_ptr != NULL) {
                        m_scan_thread_ptr->terminate();
                        m_scan_thread_ptr->suspend();
                    }
                    component_ref->cleanPositionsQueue(NOW);
                    m_configuration->m_isScanLocked = false;
                    m_configuration->m_isScanning = false;
                    if((m_configuration->m_scan).wasElevationTrackingEn)
                        turnTrackingOn();
                    else {
                        if(component_ref->isReady()) {
                            component_ref->setPosition((m_configuration->m_scan).plainCentralPos, NOW);
                        }
                        else {
                            string msg("closeScan(): component not ready");
                            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                            impl.log(LM_DEBUG);
                            throw impl.getMinorServoErrorsEx();
                        }
                    }

                    m_configuration->m_isScanActive = false;

                    ACSErr::Completion_var completion;          
                    ACS::doubleSeq actPos = *((component_ref->actPos())->get_sync(completion.out()));
                    ACS::doubleSeq centralPos = (m_configuration->m_scan).centralPos;
                    size_t axis = (m_configuration->m_scan).axis_index;
                    ACS::doubleSeq *acc = component_ref->getData("ACCELERATION");
                    ACS::doubleSeq *mspeed = component_ref->getData("MAX_SPEED");

                    CDB::DAL_ptr dal_p = getContainerServices()->getCDB();
                    CDB::DAO_ptr dao_p = dal_p->get_DAO_Servant(
                            ("alma/MINORSERVO/" + (m_configuration->m_scan).comp_name).c_str());
                    size_t number_of_axis = dao_p->get_long("number_of_axis");
                    size_t idx = (*acc).length() - number_of_axis + axis;
                    if((*acc).length() - 1 < idx || (*mspeed).length() - 1 < idx) {
                        string msg("closeScan(): acceleration index out of range");
                        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                        impl.log(LM_DEBUG);
                        throw impl.getMinorServoErrorsEx();
                    }
                    double acceleration = (*acc)[idx]; 
                    double max_speed= (*mspeed)[idx]; 
                    double distance = fabs(actPos[axis] - centralPos[axis]);

                    ACS::Time min_scan_time = get_min_time(distance, acceleration, max_speed);
                    ACS::Time guard_min_scan_time = static_cast<ACS::Time>(min_scan_time * (1 + SCAN_GUARD_COEFF));
                    timeToStop = getTimeStamp() + guard_min_scan_time;
                }
                else {
                    string msg("closeScan(): nil component reference");
                    _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                    impl.log(LM_DEBUG);
                    throw impl.getMinorServoErrorsEx();
                }
            }
            catch(...) {
                string msg("closeScan(): unexpected exception");
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_DEBUG);
                throw impl.getMinorServoErrorsEx();
            }
        }
        else {
            string msg("closeScan(): cannot get the component reference");
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }
    }
    else {
        m_configuration->m_isScanLocked = false;
        m_configuration->m_isScanning = false;
        m_configuration->m_isScanActive = false;
    }
}


CORBA::Boolean MinorServoBossImpl::checkScan(
        const ACS::Time startingTime, 
        const MinorServo::MinorServoScan & msScanInfo,
        const Antenna::TRunTimeParameters & antennaInfo,
        MinorServo::TRunTimeParameters_out msParameters
    ) throw (MinorServoErrors::MinorServoErrorsEx, ComponentErrors::ComponentErrorsEx)
{
    if(!isReady()) {
        string msg("checkScan(): the system is not ready");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    if(m_configuration->m_isScanLocked) {
        string msg("checkScan(): the system is executing another scan");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }
    MinorServo::TRunTimeParameters_var msParamVar = new MinorServo::TRunTimeParameters;

    msParamVar->onTheFly = false;
    msParamVar->startEpoch = 0;
    msParamVar->centerScan = 0;
    msParamVar->scanAxis = CORBA::string_dup("");
    msParamVar->timeToStop = 0;
    msParameters = msParamVar._retn();
    try {
        return checkScanImpl(startingTime, msScanInfo, antennaInfo, msParameters);
    }
    catch(...) {
        string msg("checkScan(): unexpected exception calling checkScanImpl()");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }
}


bool MinorServoBossImpl::checkScanImpl(
        const ACS::Time startingTime, 
        const MinorServo::MinorServoScan & msScanInfo,
        const Antenna::TRunTimeParameters & antennaInfo,
        MinorServo::TRunTimeParameters_out msParameters
    ) throw (MinorServoErrors::MinorServoErrorsEx, ComponentErrors::ComponentErrorsEx)
{
    MinorServo::TRunTimeParameters_var msParamVar = new MinorServo::TRunTimeParameters;
    msParamVar->onTheFly = (msScanInfo.is_empty_scan == false) ? true : false;
    msParamVar->startEpoch = 0;
    msParamVar->centerScan = 0;
    msParamVar->scanAxis = CORBA::string_dup("");
    msParamVar->timeToStop = 0;

    size_t axis = 0;
    string comp_name;
    try {
        InfoAxisCode info;
        info = m_configuration->getInfoFromAxisCode(string(msScanInfo.axis_code));
        axis = info.axis_id;
        comp_name = info.comp_name;
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ex.log(LM_ERROR);
            return false;
        }
    }
    catch (...) {
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true) {
            return true;
        }
        else {
            ACS_SHORT_LOG((LM_ERROR, "checkScanImpl(): cannot get the axis info"));
            return false;
        }
    }

    ACS::doubleSeq plainCentralPos; // Position without offsets
    ACS::doubleSeq centralPos; // Position with offsets
    try {
        plainCentralPos = m_configuration->getPositionFromElevation(
                comp_name, 
                antennaInfo.elevation);
        centralPos.length(plainCentralPos.length());
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ex.log(LM_ERROR);
            return false;
        }
    }
    catch (...) {
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ACS_SHORT_LOG((LM_ERROR, "checkScanImpl(): cannot get the central scan position"));
            return false;
        }
    }

    MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
    if((m_configuration->m_component_refs).count(comp_name)) {
        component_ref = (m_configuration->m_component_refs)[comp_name];

        if(CORBA::is_nil(component_ref)) {
            string msg("startScanImpl: cannot get the reference of the component.", true);
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }

        if(!component_ref->isReady()) {
            string msg("startScanImpl: the component is not ready", true);
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }
        ACS::doubleSeq_var user_offset = component_ref->getUserOffset();
        for(size_t i=0; i<centralPos.length(); i++) 
            centralPos[i] = plainCentralPos[i] + user_offset[i];
    }
    else {
        string msg("startScanImpl(): cannot get the " + comp_name + " component");
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
            return false;
        }
    }

    ACS::Time min_starting_time = 0;
    msParamVar->centerScan = centralPos[axis];
    msParamVar->scanAxis = CORBA::string_dup(msScanInfo.axis_code);

    double acceleration = 0.0;
    double max_speed = 0.0;
        double scanDistance = msScanInfo.range;
    TIMEVALUE stime(startingTime);
    ACS::Time ttime(msScanInfo.total_time);
    try {
        min_starting_time = getMinScanStartingTime(
                scanDistance, 
                string(msScanInfo.axis_code), 
                antennaInfo.elevation,
                acceleration, 
                max_speed);
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ex.log(LM_ERROR);
            return false;
        }
    }
    catch(ManagementErrors::SubscanErrorExImpl& ex) {
        ex.log(LM_ERROR);
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ex.log(LM_ERROR);
            return false;
        }
    }
    catch(...) {
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan(): unexpected exception getting the min starting time"));
            return false;
        }
    }

    // Add 1.5s in order to consider the time between the check and the start
    min_starting_time += 15000000;
    msParamVar->startEpoch = (startingTime==0) ? min_starting_time : startingTime;
    msParamVar->timeToStop = msParamVar->startEpoch + msScanInfo.total_time;

    if(startingTime != 0 && min_starting_time > startingTime) {
        msParameters = msParamVar._retn(); 
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan(): not enought time to start the scan"));
            return false;
        }
    }

    msParameters = msParamVar._retn(); 
    ACS::Time min_scan_time = get_min_time(scanDistance, acceleration, max_speed);
    ACS::Time guard_min_scan_time = static_cast<ACS::Time>(min_scan_time * (1 + SCAN_GUARD_COEFF));

    TIMEVALUE gmst(guard_min_scan_time);
    if(CIRATools::timeSubtract(ttime, gmst) <= 0) {
        if(msScanInfo.is_empty_scan == true)
            return true;
        else {
            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan(): total time too short for performing the scan."));
            return false;
        }
    }
    else {
        return true;
    }
}


ACS::Time MinorServoBossImpl::getMinScanStartingTime(
        double & range, 
        const string axis_code, 
        const double elevation,
        double & acceleration, 
        double & max_speed
    )
    throw (ManagementErrors::ConfigurationErrorExImpl, ManagementErrors::SubscanErrorExImpl)
{
    ACS::Time min_starting_time = 0;
    InfoAxisCode info;
    try {
        info = m_configuration->getInfoFromAxisCode(axis_code); // Throws ConfigurationErrorExImpl
        size_t axis = info.axis_id;
        string comp_name = info.comp_name;

        TIMEVALUE dtime(SCAN_DELTA_TIME);

        MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();

        if((m_configuration->m_component_refs).count(comp_name)) {
            component_ref = (m_configuration->m_component_refs)[comp_name];
            if(!CORBA::is_nil(component_ref)) 
                if(!component_ref->isReady()) {
                    THROW_EX(ManagementErrors, 
                            ConfigurationErrorEx, 
                            "getMinScanStartingTime(): the component is not ready", 
                            false);
                }
                else {
                    CDB::DAL_ptr dal_p = getContainerServices()->getCDB();
                    CDB::DAO_ptr dao_p = dal_p->get_DAO_Servant(
                            ("alma/MINORSERVO/" + comp_name).c_str());
                    size_t number_of_axis = dao_p->get_long("number_of_axis");
                    bool virtual_rs = dao_p->get_long("virtual_rs");
                    long servo_address = dao_p->get_long("servo_address");
                    double zero = dao_p->get_double("zero");
                    if(axis > number_of_axis - 1) {
                        THROW_EX(ManagementErrors, 
                                ConfigurationErrorEx, 
                                "getMinScanStartingTime(): axis out of range", false);
                    }

                    ACS::doubleSeq *acc = component_ref->getData("ACCELERATION");
                    ACS::doubleSeq *mspeed = component_ref->getData("MAX_SPEED");
                    size_t idx = (*acc).length() - number_of_axis + axis;
                    if((*acc).length() - 1 < idx || (*mspeed).length() - 1 < idx) {
                        THROW_EX(ManagementErrors, 
                                ConfigurationErrorEx, 
                                "getMinScanStartingTime(): index out of range", false);
                    }
                    acceleration = (*acc)[idx]; 
                    max_speed= (*mspeed)[idx]; 
                    if(acceleration != 0 && max_speed != 0) {
                        ACSErr::Completion_var completion;          
                        ACS::doubleSeq act_pos = *((component_ref->actPos())->get_sync(completion.out()));

                        if(act_pos.length() <= axis) {
                            THROW_EX(ManagementErrors, 
                                    ConfigurationErrorEx, 
                                    "getMinScanStartingTime(): wrong position indexing", false);
                        }
                        // Get the servo position related to the antenna elevation 
                        // at the beginning of the scan (sspos means start scan pos)
                        ACS::doubleSeq sspos = m_configuration->getPositionFromElevation(
                                comp_name, 
                                elevation);
                        ACS::doubleSeq centralPos = m_configuration->isScanActive() ? \
                                                    (m_configuration->m_scan).centralPos : sspos;
                        ACS::doubleSeq_var system_offset = component_ref->getSystemOffset();
                        ACS::doubleSeq_var user_offset = component_ref->getUserOffset();

                        for(size_t i=0; i<centralPos.length(); i++)
                            centralPos[i] = centralPos[i] + user_offset[i] + system_offset[i];

                        ACS::doubleSeq positions_left, positions_right;
                        positions_left = positions_right = centralPos;
                        positions_left[axis] = positions_left[axis] - range / 2;
                        positions_right[axis] = positions_right[axis] + range / 2;
                        // Check if the positions are allowed
                        ACS::doubleSeq_var max_values = component_ref->getMaxPositions();
                        ACS::doubleSeq_var min_values = component_ref->getMinPositions();
                        if(system_offset->length() < axis) {
                            THROW_EX(ManagementErrors, 
                                    ConfigurationErrorEx, 
                                    "getMinScanStartingTime(): wrong system offset indexing", false);
                        }
                        if(positions_left[axis] + system_offset[axis] <= min_values[axis]) {
                            THROW_EX(ManagementErrors, 
                                    ConfigurationErrorEx, 
                                    "getMinScanStartingTime(): min position out of range", false);
                        }
                        if(positions_right[axis] + system_offset[axis] >= max_values[axis]) {
                            THROW_EX(ManagementErrors, 
                                    ConfigurationErrorEx, 
                                    "getMinScanStartingTime(): max position out of range", false);
                        }
                        
                        double left_distance = abs_(act_pos[axis] - positions_left[axis]);
                        double right_distance = abs_(act_pos[axis] - positions_right[axis]);
                        double positioning_distance = left_distance <= right_distance ? left_distance : right_distance;

                        // If the component has a virtual reference system
                        if(virtual_rs) {
                            // Conversion from Virtual reference system to the Real one
                            virtual2real(positions_left, servo_address, zero);
                            virtual2real(positions_right, servo_address, zero);
                            ACS::doubleSeq diff;
                            diff.length(act_pos.length());

                            for(size_t i = 0; i < act_pos.length(); i++)
                                diff[i] = abs_(positions_left[i] - positions_right[i]);

                            double max_diff = 0;
                            for(size_t i = 0; i < act_pos.length(); i++)
                                if(diff[i] > max_diff)
                                    max_diff = diff[i];

                            range = max_diff;

                            // Conversion from Virtual reference system to the Real one
                            virtual2real(act_pos, servo_address, zero);
                            ACS::doubleSeq diff_left, diff_right;
                            diff_left.length(act_pos.length());
                            diff_right.length(act_pos.length());

                            for(size_t i = 0; i < act_pos.length(); i++) {
                                diff_left[i] = abs_(act_pos[i] - positions_left[i]);
                                diff_right[i] = abs_(act_pos[i] - positions_right[i]);
                            }

                            double max_diff_left = 0;
                            double max_diff_right = 0;
                            for(size_t i = 0; i < act_pos.length(); i++) {
                                if(diff_left[i] > max_diff_left)
                                    max_diff_left = diff_left[i];
                                if(diff_right[i] > max_diff_right)
                                    max_diff_right = diff_right[i];
                            }
                            positioning_distance = max_diff_left <= max_diff_right ? max_diff_left : max_diff_right;
                        }

                        ACS::Time positioning_time = get_min_time(positioning_distance, acceleration, max_speed);
                        
                        
                        min_starting_time = static_cast<ACS::Time>(
                                getTimeStamp() + SCAN_SHIFT_TIME + positioning_time * (1 + SCAN_GUARD_COEFF)
                        );
                    }
                    else {
                        THROW_EX(
                                ManagementErrors, 
                                ConfigurationErrorEx, 
                                "getMinScanStartingTime(): Acceleration or maximum speed wrong", 
                                false
                        );
                    }
                }
        }
        else {
            THROW_EX(
                    ManagementErrors, 
                    ConfigurationErrorEx, 
                    ("some problems getting the component " + comp_name).c_str(),
                    false
            );
        }
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        throw ex;
    }
    catch(ManagementErrors::SubscanErrorExImpl& ex) {
        throw ex;
    }
    catch(MinorServoErrors::CommunicationErrorEx& ex){
		_ADD_BACKTRACE(
                ManagementErrors::MinorServoScanErrorExImpl,
                impl,
                ex,
                "MinorServoBossImpl::getMinScanStartingTime()");

		throw impl;
    }
    catch(...) {
			_EXCPT(ManagementErrors::MinorServoScanErrorExImpl, 
                   impl,
                   "MinorServoBossImpl::getMinScanStartingTime()");
		throw impl;
    }
    return min_starting_time;
}




void MinorServoBossImpl::startScan(
        ACS::Time & startingTime, 
        const MinorServo::MinorServoScan & msScanInfo,
        const Antenna::TRunTimeParameters & antennaInfo
    ) throw (MinorServoErrors::MinorServoErrorsEx, ComponentErrors::ComponentErrorsEx)
{
    if(!isReady()) {
        string msg("startScan(): the system is not ready");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    if(m_configuration->m_isScanLocked) {
        string msg("startScan(): the system is executing another scan");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    if(msScanInfo.is_empty_scan) {
        startingTime = getTimeStamp();
        return ;
    }
    else {
        startScanImpl(
                startingTime, 
                msScanInfo,
                antennaInfo
        );
    }
}


void MinorServoBossImpl::startScanImpl(
        ACS::Time & startingTime, 
        const MinorServo::MinorServoScan & msScanInfo,
        const Antenna::TRunTimeParameters & antennaInfo
        ) throw (
             MinorServoErrors::MinorServoErrorsEx,
             ComponentErrors::ComponentErrorsEx)
{
    m_configuration->m_isScanLocked = true;
    m_configuration->m_isScanning = false;
    size_t axis;
    string comp_name;
    try {
        InfoAxisCode info;
        info = m_configuration->getInfoFromAxisCode(string(msScanInfo.axis_code));
        axis = info.axis_id;
        comp_name = info.comp_name;
    }
    catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
        m_configuration->m_isScanLocked = false;
         _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "startScanImpl()");
        throw impl.getMinorServoErrorsEx();

    }
    catch(...) {
        m_configuration->m_isScanLocked = false;
        string msg("startScanImpl(): unexpected exception getting the axis information");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_ERROR);
        throw impl.getMinorServoErrorsEx();
    }

    ACS::doubleSeq plainCentralPos;
    try {
        plainCentralPos = m_configuration->getPositionFromElevation(
                comp_name, 
                antennaInfo.elevation);
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        m_configuration->m_isScanLocked = false;
        _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "startScanImpl()");
        throw impl.getMinorServoErrorsEx();
    }
    catch (...) {
        m_configuration->m_isScanLocked = false;
        string msg("startScanImpl(): cannot get the central scan position");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_ERROR);
        throw impl.getMinorServoErrorsEx();
    }

    double acceleration = 0.0;
    double max_speed = 0.0;
    double scanDistance = msScanInfo.range;
    TIMEVALUE stime(startingTime);
    ACS::Time min_starting_time = 0;
    try {
        // getMinScanStartingTime() checks several constraints
        min_starting_time = getMinScanStartingTime(
                scanDistance, 
                string(msScanInfo.axis_code), 
                antennaInfo.elevation,
                acceleration, 
                max_speed);
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
        m_configuration->m_isScanLocked = false;
        _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "startScanImpl()");
        throw impl.getMinorServoErrorsEx();
    }
    catch(ManagementErrors::SubscanErrorExImpl& ex) {
        m_configuration->m_isScanLocked = false;
        _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "startScanImpl()");
        throw impl.getMinorServoErrorsEx();
    }
    catch(...) {
        m_configuration->m_isScanLocked = false;
        string msg("startScanImpl(): cannot get the minimum scan starting time");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_ERROR);
        throw impl.getMinorServoErrorsEx();
    }
 

    if(startingTime != 0) {
        if(min_starting_time > startingTime) {
            m_configuration->m_isScanLocked = false;
            string msg("startScanImpl(): not enough time to start the scan");
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_ERROR);
            throw impl.getMinorServoErrorsEx();
        }
    }
    else {
        startingTime = min_starting_time;
    }

    ACS::Time min_scan_time = get_min_time(scanDistance, acceleration, max_speed);
    ACS::Time guard_min_scan_time = static_cast<ACS::Time>(min_scan_time * (1 + SCAN_GUARD_COEFF));
    TIMEVALUE gmst(guard_min_scan_time);

    ACS::Time ttime(msScanInfo.total_time);
    if(CIRATools::timeSubtract(ttime, gmst) <= 0) {
        m_configuration->m_isScanLocked = false;
        string msg("startScanImpl(): total time too short for performing the scan.");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_ERROR);
        throw impl.getMinorServoErrorsEx();
    }

    try {
        m_servo_scanned = comp_name;
        MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
        if((m_configuration->m_component_refs).count(comp_name)) {
            component_ref = (m_configuration->m_component_refs)[comp_name];

            if(CORBA::is_nil(component_ref)) {
                string msg("startScanImpl: cannot get the reference of the component.", true);
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_ERROR);
                throw impl.getMinorServoErrorsEx();
            }

            if(!component_ref->isReady()) {
                string msg("startScanImpl: the component is not ready", true);
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_ERROR);
                throw impl.getMinorServoErrorsEx();
            }

            CDB::DAL_ptr dal_p = getContainerServices()->getCDB();
            CDB::DAO_ptr dao_p = dal_p->get_DAO_Servant(("alma/MINORSERVO/" + comp_name).c_str());
            size_t number_of_axis = dao_p->get_long("number_of_axis");
                                                        
            if(axis > number_of_axis - 1) {
                string msg("startScanImpl: axis index error", true);
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_ERROR);
                throw impl.getMinorServoErrorsEx();
            }
                                                        
            bool wasTrackingEn = isElevationTrackingEn();
            turnTrackingOff();

            ACSErr::Completion_var completion;          
            ACS::doubleSeq actPos = *((component_ref->actPos())->get_sync(completion.out()));
            ACS::doubleSeq_var user_offset = component_ref->getUserOffset();
            ACS::doubleSeq centralPos;
            centralPos.length(user_offset->length());
            if(user_offset->length() != plainCentralPos.length()) {
                string msg("startScanImpl(): mismatch between offset and central position length");
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_ERROR);
                throw impl.getMinorServoErrorsEx();
            }
            else {
                for(size_t i=0; i<plainCentralPos.length(); i++) 
                    centralPos[i] = plainCentralPos[i] + user_offset[i];
            }

            ACS::Time total_time(msScanInfo.total_time);
            m_configuration->setScan(
                    startingTime, 
                    total_time,
                    SCAN_DELTA_TIME, 
                    msScanInfo.range,
                    comp_name, 
                    axis, 
                    string(msScanInfo.axis_code),
                    actPos, 
                    centralPos,
                    plainCentralPos,
                    wasTrackingEn
            );
        }
        else {
            string msg("startScanImpl(): cannot get the component reference.");
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_ERROR);
            throw impl.getMinorServoErrorsEx();
        }
        
        try {
            if(m_scan_thread_ptr != NULL)
                m_scan_thread_ptr->restart();
            else {
                m_scan_thread_ptr = getContainerServices()->getThreadManager()->create<ScanThread, MSBossConfiguration *> 
                    ("ScanThread", m_configuration);
                m_scan_thread_ptr->resume();
            }
        }
        catch(...) {
            string msg("startScanImpl(): the MinorServoBoss is attempting to execute a previous scan");
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_ERROR);
            throw impl.getMinorServoErrorsEx();
        }
        m_configuration->m_isScanActive = true;
    }
    catch(...) {
        m_configuration->m_isScanLocked = false;
        m_configuration->m_isScanActive = false;
        m_configuration->m_isScanning = false;
        throw;
    }
}


CORBA::Double MinorServoBossImpl::getCentralScanPosition() throw (
             MinorServoErrors::MinorServoErrorsEx,
             ComponentErrors::ComponentErrorsEx)
{
    ACS::doubleSeq_var position_res = new ACS::doubleSeq;
    string comp_name = (m_configuration->m_scan).comp_name;

    if(isScanActive()) {
        MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
        if((m_configuration->m_component_refs).count(comp_name)) {
            component_ref = (m_configuration->m_component_refs)[comp_name];
            if(CORBA::is_nil(component_ref)) {
                string msg("getCentralScanPosition(): cannot get the reference of the component");
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_DEBUG);
                throw impl.getMinorServoErrorsEx();
            }

            ACS::doubleSeq plainCentralPos = (m_configuration->m_scan).plainCentralPos;

            ACS::doubleSeq_var user_offset = component_ref->getUserOffset();
            position_res->length(user_offset->length());
            if(user_offset->length() != plainCentralPos.length()) {
                string msg("getCentralScanPosition(): mismatch between offset and central position length");
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_DEBUG);
                throw impl.getMinorServoErrorsEx();
            }
            for(size_t i=0; i<plainCentralPos.length(); i++) 
                position_res[i] = plainCentralPos[i] + user_offset[i];
        }
        else {
            string msg("getCentralScanPosition(): cannot get the name of " + comp_name);
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }
    }
    else {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "getCentralScanPosition(): scan not active");
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    return position_res[(m_configuration->m_scan).axis_index];
}


char * MinorServoBossImpl::getScanAxis() {
    if(isScanActive()) {
        return CORBA::string_dup(((m_configuration->m_scan).axis_code).c_str());
    }
    else {
        return CORBA::string_dup("");
    }

}


void MinorServoBossImpl::turnTrackingOn() throw (ManagementErrors::ConfigurationErrorEx) 
{ 
    if(isStarting())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOn: the system is starting.", true);

    if(isParking())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOn: the system is parking.", true);

    if(!isReady())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOn: the system is not ready.", true);

    int mutex_res = pthread_mutex_trylock(&tracking_mutex); 
    if(mutex_res != 0) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOn: the system is busy.", true);
    }

    if(m_tracking_thread_ptr != NULL) {
        m_tracking_thread_ptr->suspend();
        m_tracking_thread_ptr->terminate();
        m_tracking_thread_ptr = NULL;
    }

    try {
        m_tracking_thread_ptr = getContainerServices()->getThreadManager()->create<TrackingThread,
            MSBossConfiguration *>("TrackingThread", m_configuration);
    }
    catch(...) {
        if(mutex_res == 0 && pthread_mutex_unlock(&tracking_mutex)); 
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "Error in TrackingThread", true);
    }

    m_tracking_thread_ptr->resume();
    if(mutex_res == 0 && pthread_mutex_unlock(&tracking_mutex)); 
}


void MinorServoBossImpl::turnTrackingOff() throw (ManagementErrors::ConfigurationErrorEx) 
{

    int mutex_res = pthread_mutex_trylock(&tracking_mutex); 
    try {
        if(mutex_res != 0) {
            THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOff: the system is busy.", true);
        }

        if(m_tracking_thread_ptr != NULL) {
            m_tracking_thread_ptr->suspend();
            m_tracking_thread_ptr->terminate();
            m_tracking_thread_ptr = NULL;
        }
        if(isReady()) {
            string comp_name("SRP");
            MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
            if(!m_component_refs.count(comp_name)) {
                try {
                    component_ref = m_services->getComponent<MinorServo::WPServo>(("MINORSERVO/" + comp_name).c_str());
                }
                catch(...) {
                    THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOff: cannot get the SRP component.", true);
                }
            }
            else {
                component_ref = m_component_refs[comp_name];
            }
            try {
                ACS::doubleSeq positions = m_configuration->getPosition(comp_name, getTimeStamp());
                // set the position
                if(positions.length()) {
                    if(!CORBA::is_nil(component_ref)) {
                        if(component_ref->isReady())
                            component_ref->setPosition(positions, NOW);
                        else {
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::turnTrackingOff(): cannot set the SRP position"));
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::turnTrackingOff(): SRP not ready"));
                        }
                    }
                    else {
                        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOff: cannot set the SRP position.", true);
                    }
                }
                else {
                    THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOff: cannot get the SRP position.", true);
                }
            }
            catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
                ex.log(LM_DEBUG);
                throw ex.getConfigurationErrorEx();     
            }
            catch(...) {
                THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOff: cannot set the SRP position.", true);
            }
        }

        if(mutex_res == 0 && pthread_mutex_unlock(&tracking_mutex)); 
    }
    catch(...) {
        if(mutex_res == 0 && pthread_mutex_unlock(&tracking_mutex)); 
        throw;
    }
}


void MinorServoBossImpl::clearUserOffset(const char *servo) throw (
             MinorServoErrors::MinorServoErrorsEx,
             ComponentErrors::ComponentErrorsEx)
{
    clearOffset(servo, "user");
}


// Clear offset from Operator Input
void MinorServoBossImpl::clearOffsetsFromOI() throw (MinorServoErrors::OperationNotPermittedExImpl){
    try {
        clearUserOffset("ALL");
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


void MinorServoBossImpl::clearSystemOffset(const char *servo)
     throw (MinorServoErrors::MinorServoErrorsEx, ComponentErrors::ComponentErrorsEx)
{
    clearOffset(servo, "system");
}


void MinorServoBossImpl::clearOffset(const char *servo, string offset_type) throw (
             MinorServoErrors::MinorServoErrorsEx,
             ComponentErrors::ComponentErrorsEx)
{
    string comp_name = get_component_name(string(servo));
    MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
    if(comp_name == "ALL") {
        vector<string> slaves = m_configuration->getServosToMove();
        for(vector<string>::iterator iter = slaves.begin(); iter != slaves.end(); iter++) {
            if(m_component_refs.count(*iter)) {
                component_ref = MinorServo::WPServo::_nil();
                component_ref = m_component_refs[*iter];
                if(!CORBA::is_nil(component_ref))
                    if(offset_type == "user") {
                        if(component_ref->isReady()) {
                            try {
                                component_ref->clearUserOffset(true);
                            }
                            catch(...) {
                                string msg("Cannot clear the WPServo user offset.");
                                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                                impl.log(LM_DEBUG);
                                throw impl.getMinorServoErrorsEx();
                            }
                        }
                        else if(!component_ref->isParked()) {
                            string msg("MinorServoBossImpl::clearOffset(): cannot clear the user offset, " + *iter + " not ready.");
                            ACS_SHORT_LOG((LM_WARNING, msg.c_str()));
                        }
                    }
                    else
                        if(offset_type == "system") {
                            if(component_ref->isReady()) {
                                try {
                                    component_ref->clearSystemOffset(true);
                                }
                                catch(...) {
                                    string msg("Cannot clear the WPServo system offset.");
                                    _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                                    impl.log(LM_DEBUG);
                                    throw impl.getMinorServoErrorsEx();
                                }
                            }
                            else if(!component_ref->isParked()) {
                                string msg("MinorServoBossImpl::clearOffset(): cannot clear the system offset, " + *iter + " not ready.");
                                ACS_SHORT_LOG((LM_WARNING, msg.c_str()));
                            }
                        }
                        else {
                            string msg(string("The offset ") + offset_type + string(" doesn't exist"));
                            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                            impl.log(LM_DEBUG);
                            throw impl.getMinorServoErrorsEx();
                        }
            }
        }
    }
    else {
        if(!slave_exists(comp_name)) {
            string msg(string("The component ") + comp_name + string(" doesn't exist"));
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }
        if(m_component_refs.count(comp_name)) {
            component_ref = m_component_refs[comp_name];
            if(!CORBA::is_nil(component_ref)) {
                    if(offset_type == "user") {
                        if(component_ref->isReady()) {
                            try {
                                component_ref->clearUserOffset(true);
                            }
                            catch(...) {
                                string msg("Cannot clear the WPServo user offset.");
                                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                                impl.log(LM_DEBUG);
                                throw impl.getMinorServoErrorsEx();
                            }
                        }
                        else if(!component_ref->isParked()) {
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::clearOffset(): cannot clear the user offset."));
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::clearOffset(): SRP not ready"));
                        }
                    }
                    else
                        if(offset_type == "system") {
                            if(component_ref->isReady()) {
                                try {
                                    component_ref->clearSystemOffset(true);
                                }
                                catch(...) {
                                    string msg("Cannot clear the WPServo system offset.");
                                    _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                                    impl.log(LM_DEBUG);
                                    throw impl.getMinorServoErrorsEx();
                                }
                            }
                            else if(!component_ref->isParked()) {
                                ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::clearOffset(): cannot clear the system offset."));
                                ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::clearOffset(): SRP not ready"));
                            }
                        }
                        else {
                            string msg(string("The offset ") + offset_type + string(" doesn't exist"));
                            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                            impl.log(LM_DEBUG);
                            throw impl.getMinorServoErrorsEx();
                        }
            }
            else {
                string msg(string("The reference to component ") + comp_name + string(" is NULL"));
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_DEBUG);
                throw impl.getMinorServoErrorsEx();
            }
        }
        else {
            string msg(string("The component ") + comp_name + string(" is not active"));
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }
    }
}


void MinorServoBossImpl::setUserOffset(const char *axis_code, const double offset) throw (
             MinorServoErrors::MinorServoErrorsEx,
             ComponentErrors::ComponentErrorsEx)
{
    setOffsetImpl(string(axis_code), offset, string("user"));
}

void MinorServoBossImpl::setUserOffsetFromOI(const char * axis_code, const double & offset)
         throw (MinorServoErrors::OperationNotPermittedExImpl)
{
    try {
        setUserOffset(axis_code, offset);
    }
    catch (...) {
        THROW_EX(
                MinorServoErrors, 
                OperationNotPermittedEx, 
                string("Cannot set the offsets"), 
                false 
        )
    }
}

void MinorServoBossImpl::setSystemOffset(const char *axis_code, const double offset)  throw (
             MinorServoErrors::MinorServoErrorsEx,
             ComponentErrors::ComponentErrorsEx)
{
        setOffsetImpl(string(axis_code), offset, string("system"));
}


void MinorServoBossImpl::setOffsetImpl(string axis_code, const double offset_value, string offset_type) 
    throw (MinorServoErrors::MinorServoErrorsEx, ComponentErrors::ComponentErrorsEx)
{
    if(!isReady()) {
        string msg("setOffsetImpl(): the system is not ready");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    InfoAxisCode info;
    try {
        info = m_configuration->getInfoFromAxisCode(axis_code); // raises ManagementErrors::ConfigurationErrorExImpl
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
         _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "setOffsetImpl()");
        throw impl.getMinorServoErrorsEx();
    }
    catch (...) {
        string msg("setOffsetImpl(): cannot get the axes info");
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }

    ACS::doubleSeq offset;
    offset.length(info.numberOfAxes);
    ACS::doubleSeq_var actual_offset;

    MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
    if(m_component_refs.count(info.comp_name)) {
        component_ref = m_component_refs[info.comp_name];
        if(!CORBA::is_nil(component_ref)) {
            if(offset_type == "user") {
                actual_offset = component_ref->getUserOffset();
                for(size_t i=0; i<offset.length(); i++)
                    offset[i] = actual_offset[i];
                offset[info.axis_id] = offset_value;
                try {
                    component_ref->setUserOffset(offset);
                }
                catch(...) {
                    string msg("setOffsetImpl(): Cannot set the WPServo user offset.");
                    _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                    impl.log(LM_DEBUG);
                    throw impl.getMinorServoErrorsEx();
                }
            }
            else if(offset_type == "system") {
                actual_offset = component_ref->getSystemOffset();
                for(size_t i=0; i<offset.length(); i++)
                    offset[i] = actual_offset[i];
                offset[info.axis_id] = offset_value;
                try {
                    component_ref->setSystemOffset(offset);
                }
                catch(...) {
                    string msg("setOffsetImpl(): Cannot set the WPServo system offset.");
                    _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                    impl.log(LM_DEBUG);
                    throw impl.getMinorServoErrorsEx();
                }
            }
            else {
                string msg(string("The offset ") + offset_type + string(" doesn't exist"));
                _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
                impl.log(LM_DEBUG);
                throw impl.getMinorServoErrorsEx();
            }
        }
        else {
            string msg(string("The reference to component ") + info.comp_name + string(" is NULL"));
            _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
            impl.log(LM_DEBUG);
            throw impl.getMinorServoErrorsEx();
        }
    }
    else {
        string msg(string("The component ") + info.comp_name + string(" is not active"));
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, msg.c_str());
        impl.log(LM_DEBUG);
        throw impl.getMinorServoErrorsEx();
    }
}


ACS::doubleSeq * MinorServoBossImpl::getUserOffset() 
     throw (MinorServoErrors::MinorServoErrorsEx, ComponentErrors::ComponentErrorsEx)
{
    vector<double> offset;
    try {
        offset = getOffsetImpl("user");
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
         _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "getUserOffset()");
        throw impl.getMinorServoErrorsEx();
    }
    catch(MinorServoErrors::OperationNotPermittedExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    ACS::doubleSeq_var offset_res = new ACS::doubleSeq;
    offset_res->length(offset.size());
    for(size_t i=0; i<offset_res->length(); i++)
        offset_res[i] = offset[i];
    return offset_res._retn();
}


ACS::doubleSeq * MinorServoBossImpl::getSystemOffset() 
     throw (MinorServoErrors::MinorServoErrorsEx, ComponentErrors::ComponentErrorsEx)
{
    vector<double> offset;
    try {
        offset = getOffsetImpl("system");
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex) {
         _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "getSystemOffset()");
        throw impl.getMinorServoErrorsEx();
    }
    catch(MinorServoErrors::OperationNotPermittedExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    ACS::doubleSeq_var offset_res = new ACS::doubleSeq;
    offset_res->length(offset.size());
    for(size_t i=0; i<offset_res->length(); i++)
        offset_res[i] = offset[i];
    return offset_res._retn();
}


vector<double> MinorServoBossImpl::getOffsetImpl(string offset_type)
     throw (MinorServoErrors::OperationNotPermittedExImpl, ManagementErrors::ConfigurationErrorExImpl)
{
    if(!isReady())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "getOffsetImpl(): the system is not ready", false);

    vector<string> servosToMove = m_configuration->getServosToMove();
    vector<double> axes_values;

    MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
    for(size_t i=0; i<servosToMove.size(); i++) {
        string comp_name = servosToMove[i];
        if((m_configuration->m_component_refs).count(comp_name)) {
            component_ref = (m_configuration->m_component_refs)[comp_name];
            if(CORBA::is_nil(component_ref)) {
                string msg("getOffsetImpl(): cannot get the reference of " + comp_name);
                THROW_EX(MinorServoErrors, OperationNotPermittedEx, msg.c_str(), false);
            }
        }
        else {
            string msg("getOffsetImpl(): " + comp_name);
            THROW_EX(MinorServoErrors, OperationNotPermittedEx, (msg + " not found").c_str(), false);
        }

        ACS::doubleSeq_var offset;
        if(offset_type == "user") {
            offset = component_ref->getUserOffset();
        }
        else if(offset_type == "system") {
            offset = component_ref->getSystemOffset();
        }
        else {
            THROW_EX(MinorServoErrors, OperationNotPermittedEx, "getOffsetImpl(): Wrong offset type", false);
        }

        for(size_t i=0; i<offset->length(); i++)
            axes_values.push_back(offset[i]);
    }

    return axes_values;
}


ACS::doubleSeq * MinorServoBossImpl::getOffset(const char *servo, string offset_type) 
    throw (MinorServoErrors::OperationNotPermittedEx)
{   
    ACS::doubleSeq_var offset = new ACS::doubleSeq;
    string comp_name = get_component_name(string(servo));
    MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();

    if(!slave_exists(comp_name)) {
        THROW_EX(
                MinorServoErrors, 
                OperationNotPermittedEx, 
                string("The component ") + comp_name + string(" doesn't exist"), 
                true
        );
    }

    if(m_component_refs.count(comp_name)) {
        component_ref = m_component_refs[comp_name];
        if(!CORBA::is_nil(component_ref)) {
            if(offset_type == "user")
                offset = component_ref->getUserOffset();
            else if(offset_type == "system")
                offset = component_ref->getSystemOffset();
            else {
                THROW_EX(
                        MinorServoErrors, 
                        OperationNotPermittedEx, 
                        string("The offset ") + offset_type + string(" doesn't exist"), 
                        true
                );
            }
        }
        else {
            THROW_EX(
                    MinorServoErrors, 
                    OperationNotPermittedEx, 
                    string("The reference to component ") + comp_name + string(" is NULL"), 
                    true
            );
        }
    }
    else {
        THROW_EX(
                MinorServoErrors, 
                OperationNotPermittedEx, 
                string("The component ") + comp_name + string(" is not active"), 
                true
        );
    }
    return offset._retn();
}


void MinorServoBossImpl::setElevationTracking(const char * value) throw (
             MinorServoErrors::MinorServoErrorsEx,
             ComponentErrors::ComponentErrorsEx)
{
    try {
        setElevationTrackingImpl(value);
    }
    catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
         _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "setElevationTracking()");
        throw impl.getMinorServoErrorsEx();
    }
}


void MinorServoBossImpl::setElevationTrackingImpl(const char * value) throw (ManagementErrors::ConfigurationErrorExImpl) {
    IRA::CString flag(value);
    flag.MakeUpper();
    m_configuration->setElevationTracking(flag); 
    try {
        if(isReady()) {
            if(m_configuration->isElevationTrackingEn()) {
                turnTrackingOn();
            }
            else {
                turnTrackingOff();
            }
        }
    }
    catch(...) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, string("setElevationTracking(): cannot turn the tracking") + string(flag), false);
    }
}


void MinorServoBossImpl::setASConfiguration(const char * value) throw (
             MinorServoErrors::MinorServoErrorsEx,
             ComponentErrors::ComponentErrorsEx)
{
    try {
        setASConfigurationImpl(value);
    }
    catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
         _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ex, "setASConfiguration()");
        throw impl.getMinorServoErrorsEx();
    }
}


void MinorServoBossImpl::setASConfigurationImpl(const char * value) throw (ManagementErrors::ConfigurationErrorExImpl) {

    if(m_configuration->isStarting())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "Cannot set the ASConfiguration because the system is starting", false);

    if(m_configuration->isParking())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "Cannot set the ASConfiguration because the system is parking", false);

    IRA::CString flag(value);
    flag.MakeUpper();
    m_configuration->setASConfiguration(flag); 

    bool wasTrackingEn = isElevationTrackingEn();
    try {
        if(isReady()) {
            bool turnedOff = false;
    
            try {
                clearUserOffset("ALL");
                clearSystemOffset("ALL");
            }
            catch(...) {
                THROW_EX(
                        ManagementErrors, 
                        ConfigurationErrorEx, 
                        string("Cannot clear the offsets"), 
                        false 
                );
            }

            try {
                turnTrackingOff(); // It raises ConfigurationErrorEx
                turnedOff = true;
            }
            catch(...) {
                THROW_EX(ManagementErrors, ConfigurationErrorEx, "cannot turn the tracking off", false);
            }
            m_configuration->init(m_configuration->m_baseSetup, true); // Keep the actual setup
            if(wasTrackingEn) {
                try {
                    turnTrackingOn(); // It raises ConfigurationErrorEx
                    turnedOff = false;
                }
                catch(...) {
                    THROW_EX(ManagementErrors, ConfigurationErrorEx, "cannot turn the tracking on", false);
                }
            }
            if(turnedOff) {
                string comp_name("SRP");
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
                if(!m_component_refs.count(comp_name)) {
                    try {
                        component_ref = m_services->getComponent<MinorServo::WPServo>(("MINORSERVO/" + comp_name).c_str());
                    }
                    catch(...) {
                        ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::setASConfiguration(): cannot get the SRP component"));
                    }
                }
                else {
                    component_ref = m_component_refs[comp_name];
                }

                ACS::doubleSeq positions = m_configuration->getPosition("SRP", getTimeStamp());
                // set the position
                if(positions.length()) {
                    if(!CORBA::is_nil(component_ref)) {
                        if(component_ref->isReady()) {
                            component_ref->setPosition(positions, NOW);
                        }
                        else {
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::setASConfiguration(): cannot set the position"));
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::setASConfiguration(): SRP not ready"));
                        }
                    }
                    else {
                        ACS_SHORT_LOG((LM_WARNING, "MinorServoBossImpl::setASConfiguration(): cannot set the position"));
                    }
                }
                else {
                    ACS_SHORT_LOG((LM_WARNING, "setASConfiguration(): cannot get the position to set."));
                }
            }
        }
    }
    catch(...) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "setASConfiguration(): cannot change the actual configuration.", false);
    }
}


bool MinorServoBossImpl::isElevationTrackingEn() {
    return m_configuration->isElevationTrackingEn();
}


bool MinorServoBossImpl::isElevationTracking() {
    return m_configuration->isElevationTracking();
}


bool MinorServoBossImpl::isTracking() {
    return m_configuration->isTracking();
}


char * MinorServoBossImpl::getActualSetup() {
    return CORBA::string_dup((m_configuration->m_actualSetup).c_str());
}


char * MinorServoBossImpl::getCommandedSetup() {
    return CORBA::string_dup((m_configuration->m_commandedSetup).c_str());
}


string get_component_name(string token) 
{
    strip(token);
    return token;
}


ACS::doubleSeq get_positions(string comp_name, string token, const MSThreadParameters *const params)
    throw (ManagementErrors::ConfigurationErrorExImpl)
{
    ACS::doubleSeq positions;
    try {
        // Dummy elevation
        double el(2); // Get a reference to AntennaBoss and retrieve the actual elevation
        vector<string> vaules;
        vector<double> position_values;
        strip(token);
        bool is_dynamic(false);
        if(!startswith(token, park_token)) {
            vector<string> items = split(token, coeffs_separator);
            positions.length(items.size());
            vector<string> values;
            size_t position_idx(0);
            for(vector<string>::iterator viter = items.begin(); viter != items.end(); viter++) {
                vector<string> labels = split(*viter, coeffs_id);
                string coeffs = labels.back();
                strip(coeffs);

                for(string::size_type idx = 0; idx != boundary_tokens.size(); idx++)
                    strip(coeffs, char2string(boundary_tokens[idx]));

                values = split(coeffs, pos_separator);

                double axis_value(0);
                // For instance if we have the following values: [C0, C1, C2], the axis value will be:
                // C0 + C1*E + C2*E^2
                for(vector<string>::size_type idx = 0; idx != values.size(); idx++)
                    axis_value += str2double(values[idx]) * pow(el, idx);

                positions[position_idx] = axis_value;
                position_idx++;

                // If there is more than one coefficient then the position is elevation dependent
                is_dynamic = (values.size() > 1) ? true : is_dynamic;
            }
        }
        // If the position is elevation dependent then add the component to tracking_list
        if(!params->is_initialized)
            (*params->tracking_list)[comp_name] = is_dynamic ? true : false;
    }
    catch(...) {
        ACS_SHORT_LOG((LM_ERROR, "Exception in get_positions"));
    }

    return positions;
}


ACS::Time get_min_time(double range, double acceleration, double max_speed) {
    if(max_speed == 0 || acceleration == 0)
        return static_cast<ACS::Time>(pow(10, 15));
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
    return static_cast<ACS::Time>(min_time * pow(10, 7));
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

