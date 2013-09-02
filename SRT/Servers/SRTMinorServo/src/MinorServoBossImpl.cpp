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
    m_scanning = false;
    m_publisher_thread_ptr = NULL;
    m_thread_params.is_setup_locked = false;
    m_thread_params.is_parking_locked = false;
    m_thread_params.is_initialized = false;
    m_thread_params.is_scanning_ptr = NULL;
    m_status_value = Management::MNG_OK;
    (m_thread_params.scan_data).positioning_time = 0;
    m_servo_scanned = "none";
    m_configuration = new MSBossConfiguration(m_services);
    m_parser= new SimpleParser::CParser<MinorServoBossImpl>(this, 1);
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
    }

    if (m_park_thread_ptr != NULL) {
        m_park_thread_ptr->suspend();
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

    turnTrackingOff(); // It raises ConfigurationErrorEx

    try {
        setupImpl(config);
    }
    catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getConfigurationErrorEx();     
    }
}

void MinorServoBossImpl::setupImpl(const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorExImpl)
{
    if(m_configuration->isStarting())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "The system is executing another setup", false);

    if(m_configuration->isParking())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "The system is executing a park", false);

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

void MinorServoBossImpl::parkImpl() throw (CORBA::SystemException, ManagementErrors::ParkingErrorExImpl)
{
    AUTO_TRACE("MinorServoBossImpl::parkImpl()");

    if(m_configuration->isStarting()) {
        THROW_EX(ManagementErrors, ParkingErrorEx, "The system is executing a setup.", false);
    }

    if(m_configuration->isParking())
        THROW_EX(ManagementErrors, ParkingErrorEx, "The system is executing another park.", false);

    try {
        turnTrackingOff(); // Raises ConfigurationError
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::park(): some problems turning the elevation tracking off."));
    }

    m_configuration->m_isParking = true;

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


void MinorServoBossImpl::getAxesInfo(ACS::stringSeq_out axes, ACS::stringSeq_out units)
    throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx) 
{

    if(!isReady())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "getAxesInfo(): the system is not ready", true);

    ACS::stringSeq_var axes_res = new ACS::stringSeq;
    ACS::stringSeq_var units_res = new ACS::stringSeq;

    vector<string> a = m_configuration->getAxes();
    vector<string> u = m_configuration->getUnits();
    axes_res->length(a.size());
    units_res->length(u.size());

    if(a.size() != u.size())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "getAxesInfo(): mismatch between axes and units length", true);

    for(size_t i=0; i<a.size(); i++) {
        axes_res[i] = (a[i]).c_str();
        units_res[i] = (u[i]).c_str();
    }

    axes = axes_res._retn();
    units = units_res._retn();
}


ACS::doubleSeq * MinorServoBossImpl::getAxesPosition(ACS::Time time) 
    throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx, ComponentErrors::UnexpectedEx)
{

    if(!isReady())
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "getAxesPosition(): the system is not ready", true);

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
                THROW_EX(ManagementErrors, ConfigurationErrorEx, msg.c_str(), true);
            }
        }
        else {
            THROW_EX(ManagementErrors, ConfigurationErrorEx, msg.c_str(), true);
                string msg("getAxesPosition(): " + comp_name);
                THROW_EX(ManagementErrors, ConfigurationErrorEx, (msg + " not found").c_str(), true);
        }

        position = component_ref->getPositionFromHistory(time); // Raises ComponentErrors::UnexpectedEx

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

void MinorServoBossImpl::stopScan() throw (ManagementErrors::SubscanErrorEx)
{

    // TODO!
    if(!m_thread_params.is_initialized)
        THROW_EX(ManagementErrors, SubscanErrorEx, "stopScan: the system is not initialized", true);

    if(m_scanning && m_scan_thread_ptr != NULL) {
        // TODO: If it is not suspended?
        m_scan_thread_ptr->suspend();
        m_scan_thread_ptr->terminate();
        m_scan_thread_ptr = NULL;
    }
    else
        if(m_scan_active == false)
            THROW_EX(ManagementErrors, SubscanErrorEx, "stopScan: scanning not active", true);

    usleep(SCAN_STOP_TIME_GUARD); // Wait a bit

    if(m_configuration->isTrackingEn())
        turnTrackingOn(); // NO: it raises ConfigurationError
    else {
        int mutex_res = pthread_mutex_trylock(&tracking_mutex); // TODO: no!
        try {
            if(mutex_res != 0) {
                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::stopScan: some problems going the minor servo back to its position"));
                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::stopScan: perform a turnTrackingOn or a setup"));
                THROW_EX(ManagementErrors, SubscanErrorEx, "checkScan: lock in setup", true);
            }
            
            for(vector<string>::iterator iter = (m_thread_params.actions).begin(); \
                    iter != (m_thread_params.actions).end(); iter++) {
                // Split the action in items.
                vector<string> items = split(*iter, items_separator);
                // Set the name of component to move from a string
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
                string comp_name = get_component_name(items.front());
                if(comp_name != m_servo_scanned)
                    continue;

                // Set a doubleSeq from a string of positions
                ACS::doubleSeq positions = get_positions(comp_name, items.back(), &m_thread_params);

                if(positions.length() && (*m_thread_params.component_refs).count(comp_name)) {
                    component_ref = (*m_thread_params.component_refs)[comp_name];
                    // Set a minor servo position if the doubleSeq is not empty
                    if(!CORBA::is_nil(component_ref))
                        component_ref->setPosition(positions, NOW);
                }
            }
            if(mutex_res == 0)
                pthread_mutex_unlock(&tracking_mutex); 
            m_scan_active = false;
        }
        catch(...) {
            if(mutex_res == 0)
                pthread_mutex_unlock(&tracking_mutex); 
            m_scan_active = false;
            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::stopScan: test 5: catturata una eccezione"));
            throw;
        }
    }
}


bool MinorServoBossImpl::checkScan(
        const ACS::Time starting_time, 
        double range, 
        const ACS::Time total_time, 
        const unsigned short axis, 
        const char *servo
    ) throw (ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx)
{
    int mutex_res = pthread_mutex_trylock(&tracking_mutex);  // TODO: no!
    try {
        if(mutex_res != 0)
            THROW_EX(ManagementErrors, ConfigurationErrorEx, "checkScan: system in lock", true);

        TIMEVALUE now(0.0L);
        TIMEVALUE stime(starting_time);
        TIMEVALUE ttime(total_time);
        TIMEVALUE dtime(SCAN_DELTA_TIME);

        if(!isReady())
            THROW_EX(ManagementErrors, ConfigurationErrorEx, "StartScan: the system is not ready", true);
            
        IRA::CIRATools::getTime(now);
        if(CIRATools::timeSubtract(ttime, dtime) < 0) {
            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan: total time is too short"));
            return false;
        }
        if(CIRATools::timeSubtract(stime, now) <= 0) {
            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan: starting time is not valid"));
            return false;
        }

        string comp_name(servo);
        MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();

        if((m_configuration->m_component_refs).count(comp_name)) {
            component_ref = (m_configuration->m_component_refs)[comp_name];
            if(!CORBA::is_nil(component_ref))
                if(!component_ref->isReady()) {
                    if(mutex_res == 0)
                        pthread_mutex_unlock(&tracking_mutex); 
                    return false;
                }
                else {
                    CDB::DAL_ptr dal_p = getContainerServices()->getCDB();
                    CDB::DAO_ptr dao_p = dal_p->get_DAO_Servant(("alma/MINORSERVO/" + comp_name).c_str());
                    long number_of_axis = dao_p->get_long("number_of_axis");
                    bool virtual_rs = dao_p->get_long("virtual_rs");
                    long servo_address = dao_p->get_long("servo_address");
                    double zero = dao_p->get_double("zero");
                    if(axis > number_of_axis - 1) {
                        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - axis %d does not exist", axis));
                        if(mutex_res == 0)
                            pthread_mutex_unlock(&tracking_mutex); 
                        return false;
                    }

                    ACS::doubleSeq *acc = component_ref->getData("ACCELERATION");
                    ACS::doubleSeq *mspeed = component_ref->getData("MAX_SPEED");
                    size_t idx = (*acc).length() - number_of_axis + axis;
                    // ACS::doubleSeq pos_limit = component_ref->getData("POS_LIMIT");
                    // ACS::doubleSeq neg_limit = component_ref->getData("NEG_LIMIT");
                    if((*acc).length() - 1 < idx || (*mspeed).length() - 1 < idx) {
                        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - error, index out of range"));
                        if(mutex_res == 0)
                            pthread_mutex_unlock(&tracking_mutex); 
                        return false;
                    }
                    double acceleration = (*acc)[idx]; 
                    double max_speed= (*mspeed)[idx]; 
                    if(acceleration != 0 && max_speed != 0) {
                        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - acceleration == %f", acceleration));
                        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - max_speed == %f", max_speed));

                        ACSErr::Completion_var completion;          
                        ACS::doubleSeq act_pos = *((component_ref->actPos())->get_sync(completion.out()));

                        if(act_pos.length() <= axis) {
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan: wrong position indexing"));
                            if(mutex_res == 0)
                                pthread_mutex_unlock(&tracking_mutex); 
                            return false;
                        }

                        ACS::doubleSeq positions_left, positions_right;
                        positions_left = positions_right = act_pos;
                        positions_left[axis] = positions_left[axis] - range / 2;
                        positions_right[axis] = positions_right[axis] + range / 2;
                        
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

                            for(size_t i = 0; i < act_pos.length(); i++) {
                                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - positions[%d] = %f", i, act_pos[i]));
                                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - left[%d] = %f", i, positions_left[i]));
                                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - right[%d] = %f", i, positions_right[i]));
                                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - diff[%d] = %f", i, diff[i]));
                            }
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - max =  %f", max_diff));

                            for(size_t i = 0; i < act_pos.length(); i++) {
                                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - diff_left[%d] = %f", i, diff_left[i]));
                                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - diff_right[%d] = %f", i, diff_right[i]));
                            }
                            ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - positioning_distance = %f", positioning_distance));


                        }

                        (m_thread_params.scan_data).positioning_time = get_min_time(positioning_distance, acceleration, max_speed);
                        
                        ACS::Time guard_time = static_cast<ACS::Time>(SCAN_SHIFT_TIME + \
                                (m_thread_params.scan_data).positioning_time * (1 + GUARD_COEFF));
                        IRA::CIRATools::getTime(now);
                        TIMEVALUE gtime(now.value().value + guard_time);
                        if(CIRATools::timeSubtract(stime, gtime) <= 0) {
                            THROW_EX(ManagementErrors, SubscanErrorEx, "startScan: starting time too much close to actual time", true);
                            if(mutex_res == 0)
                                pthread_mutex_unlock(&tracking_mutex); 
                            return false;
                        }

                        if(mutex_res == 0)
                            pthread_mutex_unlock(&tracking_mutex); 
                        return get_min_time(range, acceleration, max_speed) * (1 + GUARD_COEFF) <= total_time;
                    }
                    else {
                        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - Acceleration or maximum speed wrong")); 
                        if(mutex_res == 0)
                            pthread_mutex_unlock(&tracking_mutex); 
                        return false;
                    }
                }
        }
        else {
            ACS_SHORT_LOG((LM_WARNING, ("MinorServoBoss::checkScan - some problems getting the component " + comp_name).c_str()));
            if(mutex_res == 0)
                pthread_mutex_unlock(&tracking_mutex); 
            return false;
        }
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - unexpected exception"));
        if(mutex_res == 0)
            pthread_mutex_unlock(&tracking_mutex); 
        throw;
    }

    if(mutex_res == 0)
        pthread_mutex_unlock(&tracking_mutex); 
    ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan - last line!"));
    return false;
}


void MinorServoBossImpl::startScan(
        const ACS::Time starting_time, 
        const double range, 
        const ACS::Time total_time, 
        const unsigned short axis, 
        const char *servo
    ) throw (ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx)
{

    try {
        if(!isReady())
            THROW_EX(ManagementErrors, ConfigurationErrorEx, "StartScan: the system is not ready", true);

        if(isScanning())
            THROW_EX(ManagementErrors, ConfigurationErrorEx, "StartScan: the system is executing another scan", true);
        
        m_scanning = true;
        m_scan_active = true;
        m_configuration->m_isScanning = true;

        const ACS::Time SCAN_DELTA_TIME = 10000000; // 1 second, 
        
        TIMEVALUE now(0.0L);
        IRA::CIRATools::getTime(now);

        // If the total time is less than the delta time (the time between two consecutive positioning)
        TIMEVALUE ttime(total_time);
        TIMEVALUE dtime(SCAN_DELTA_TIME);
        if(CIRATools::timeSubtract(ttime, dtime) < 0)
            THROW_EX(ManagementErrors, SubscanErrorEx, "startScan: total time is too short", true);

        TIMEVALUE stime(starting_time);
        if(CIRATools::timeSubtract(stime, now) <= 0)
            THROW_EX(ManagementErrors, SubscanErrorEx, "startScan: starting time is not valid", true);

        string comp_name(servo);
        m_servo_scanned = comp_name;
        MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
        if((m_configuration->m_component_refs).count(comp_name)) {
            component_ref = (m_configuration->m_component_refs)[comp_name];

            if(CORBA::is_nil(component_ref)) 
                THROW_EX(ManagementErrors, SubscanErrorEx, "startScan: cannot get the reference of the component.", true);

            if(!component_ref->isReady()) 
                THROW_EX(ManagementErrors, SubscanErrorEx, "startScan: the component is not ready.", true);

            CDB::DAL_ptr dal_p = getContainerServices()->getCDB();
            CDB::DAO_ptr dao_p = dal_p->get_DAO_Servant(("alma/MINORSERVO/" + comp_name).c_str());
            long number_of_axis = dao_p->get_long("number_of_axis");
                                                        
            if(axis > number_of_axis - 1)               
                THROW_EX(ManagementErrors, SubscanErrorEx, "startScan: axis index error", true);
                                                        
            // Get the actual position                  
            ACSErr::Completion_var completion;          
            ACS::doubleSeq act_pos = *((component_ref->actPos())->get_sync(completion.out()));
                                                        
            turnTrackingOff();
                                                    
            if(act_pos.length() <= axis) {          
                ACS_SHORT_LOG((LM_WARNING, "MinorServoBoss::checkScan: wrong actual position length"));
                THROW_EX(ManagementErrors, SubscanErrorEx, "startScan: wrong actual position length.", true);
            }                                       
                                                    
            double N = static_cast<double>((ttime.value()).value) / SCAN_DELTA_TIME;
            double delta = range / N;               
            ACS::doubleSeq pos; // The position to set
            pos.length(act_pos.length());           
            // The first position is the actual one for all axes except for the one to scan, whose is (act_pos - range/2)
            for(size_t i=0; i<pos.length(); i++) {
                if(i==axis)                             
                    pos[i] = act_pos[i] - range/2;      
                else                                    
                    pos[i] = act_pos[i];                
            }

            try {
                component_ref->setPosition(pos, 0); // Go to the starting position
                double starting_pos = pos[axis]; 
                for(size_t i=0; i<=N; i++) {
                    pos[axis] = starting_pos + delta * i;
                    component_ref->setPosition(pos, stime.value().value + dtime.value().value * i);
                    usleep(20000); // Wait a bit (20 ms)
                }
            }
            catch(...) { // Position not allowed
                THROW_EX(ManagementErrors, SubscanErrorEx, "startScan: position not allowed.", true);
            }

            m_configuration->m_isConfigured = false;
        }
        
        if(m_scan_thread_ptr != NULL) { 
            m_setup_thread_ptr->restart();
        }
        else {
        }

        (m_configuration->m_scanning).starting_time = starting_time;
        (m_configuration->m_scanning).total_time = total_time;

        if(m_setup_thread_ptr != NULL)
            m_setup_thread_ptr->restart();
        else {
            m_scan_thread_ptr = getContainerServices()->getThreadManager()->create<ScanThread, MSBossConfiguration *> 
                ("ScanThread", m_configuration);
            m_scan_thread_ptr->resume();
        }
    }
    catch(...) {
        m_scanning = false;
        m_configuration->m_isScanning = false;
        throw;
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
    if(mutex_res != 0) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "turnTrackingOff: the system is busy.", true);
    }

    if(m_tracking_thread_ptr != NULL) {
        m_tracking_thread_ptr->suspend();
        m_tracking_thread_ptr->terminate();
        m_tracking_thread_ptr = NULL;
    }
    m_configuration->m_isConfigured = false;

    if(mutex_res == 0 && pthread_mutex_unlock(&tracking_mutex)); 
}


void MinorServoBossImpl::clearUserOffset(const char *servo) throw (MinorServoErrors::OperationNotPermittedEx){

    clearOffset(servo, "user");
}


void MinorServoBossImpl::clearSystemOffset(const char *servo) throw (MinorServoErrors::OperationNotPermittedEx){

    clearOffset(servo, "system");
}

void MinorServoBossImpl::clearOffset(const char *servo, string offset_type) throw (MinorServoErrors::OperationNotPermittedEx){

    string comp_name = get_component_name(string(servo));
    MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
    if(comp_name == "ALL") {
        vector<string> slaves = get_slaves();
        for(vector<string>::iterator iter = slaves.begin(); iter != slaves.end(); iter++) {
            if(m_component_refs.count(*iter)) {
                component_ref = MinorServo::WPServo::_nil();
                component_ref = m_component_refs[*iter];
                if(!CORBA::is_nil(component_ref))
                    if(offset_type == "user")
                        component_ref->clearUserOffset();
                    else
                        if(offset_type == "system")
                            component_ref->clearSystemOffset();
                        else {
                            THROW_EX(
                                    MinorServoErrors, 
                                    OperationNotPermittedEx, 
                                    string("The offset ") + offset_type + string(" doesn't exist"), 
                                    true
                            );
                        }
            }
        }
    }
    else {
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
                        component_ref->clearUserOffset();
                    else
                        if(offset_type == "system")
                            component_ref->clearSystemOffset();
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
    }
}


void MinorServoBossImpl::setUserOffset(const char *servo, const ACS::doubleSeq &offset) 
    throw (MinorServoErrors::OperationNotPermittedEx){

    setOffset(servo, offset, "user");

}


void MinorServoBossImpl::setSystemOffset(const char *servo, const ACS::doubleSeq &offset)
    throw (MinorServoErrors::OperationNotPermittedEx){

    setOffset(servo, offset, "system");

}


void MinorServoBossImpl::setOffset(const char *servo, const ACS::doubleSeq &offset, string offset_type) 
    throw (MinorServoErrors::OperationNotPermittedEx)
{
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
                component_ref->setUserOffset(offset);
            else
                if(offset_type == "system")
                    component_ref->setSystemOffset(offset);
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
}


ACS::doubleSeq * MinorServoBossImpl::getUserOffset(const char *servo) throw (MinorServoErrors::OperationNotPermittedEx) {
    ACS::doubleSeq_var offset = new ACS::doubleSeq;
    offset = getOffset(servo, "user");
    return offset._retn();
}


ACS::doubleSeq * MinorServoBossImpl::getSystemOffset(const char *servo) throw (MinorServoErrors::OperationNotPermittedEx) {
    ACS::doubleSeq_var offset = new ACS::doubleSeq;
    offset = getOffset(servo, "system");
    return offset._retn();
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
            else
                if(offset_type == "system")
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


bool MinorServoBossImpl::isTrackingEn() {
    return m_configuration->isTrackingEn();
}


bool MinorServoBossImpl::isTracking() {
    return m_configuration->isElevationTracking();
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
    double threshold = pow(max_speed, 2) / acceleration;
    double min_time = range <= threshold ? 2 * sqrt(range / acceleration) : \
           2 * max_speed / acceleration + sqrt((range - threshold) / max_speed);
    return static_cast<ACS::Time>(min_time * pow(10, 7));
}


GET_PROPERTY_REFERENCE(MinorServoBossImpl, Management::ROTSystemStatus, m_status, status);
// GET_PROPERTY_REFERENCE(MinorServoBossImpl, ACS::ROpattern, m_verbose_status, verbose_status);


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(MinorServoBossImpl)

