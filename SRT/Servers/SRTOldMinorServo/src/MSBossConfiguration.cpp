#include "MSBossConfiguration.h"
#include "utils.h"

using namespace IRA;
using namespace std;

static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t position_mutex = PTHREAD_MUTEX_INITIALIZER;

MSBossConfiguration::MSBossConfiguration(maci::ContainerServices *Services, MinorServoBossImpl * bossImpl_ptr)
{
    m_actualSetup = "unknown";
    m_commandedSetup = "";
    m_baseSetup = "";
    m_isConfigured = false;
    m_isStarting = false;
    m_isParking = false;
    m_isElevationTracking = false;
    m_isElevationTrackingEn = false;
    m_isTracking = false;
    m_trackingError = false;
    m_isScanning = false;
    m_isScanLocked = false;
    m_isScanActive = false;
    m_dynamic_comps.clear();
    m_services = Services;
    m_isASConfiguration = false;
    m_nchannel = NULL;
    m_antennaBoss = Antenna::AntennaBoss::_nil();
    m_bossImpl_ptr = bossImpl_ptr;
    m_antennaBossError = false;
    m_status = Management::MNG_WARNING;
}

MSBossConfiguration::~MSBossConfiguration() {}


void MSBossConfiguration::init(string setupMode, bool keepSetup) throw (ManagementErrors::ConfigurationErrorExImpl)
{
    int mutex_res = pthread_mutex_trylock(&init_mutex); 
    if(mutex_res != 0) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "MSBossConfiguration::init(): system busy.", true);
    }
    try {
        // Starting
        m_isStarting = true;
        m_baseSetup = setupMode;
        // m_commandedSetup = m_isASConfiguration ? setupMode + "_ASACTIVE" : setupMode;
        if(!endswith(setupMode, "P"))
            m_commandedSetup = m_isASConfiguration ? setupMode + "_ASACTIVE" : setupMode;
        else
            m_commandedSetup = setupMode;
        ACS::doubleSeq dummy;

        // Read the component configuration
        IRA::CString config;
        if(!CIRATools::getDBValue(m_services, m_commandedSetup.c_str(), config)) {
            m_isStarting = false;
            THROW_EX(ManagementErrors, ConfigurationErrorEx, m_commandedSetup + ": unavailable configuration code.", false);
        }

        // initializing
        m_isConfigured = false;
        m_actualSetup = "unknown";
        m_servosToPark.clear();
        m_servosToMove.clear();
        m_servosCoefficients.clear();
        m_servosToPowerOffEncoder.clear();
        m_dynamic_comps.clear();
        m_axes.clear();
        m_units.clear();
        m_isValidCDBConfiguration = false;

        vector<string> actions = split(string(config), actions_separator);

        // Get the actions
        for(vector<string>::iterator iter = actions.begin(); iter != actions.end(); iter++) {
            // Split the action in items.
            vector<string> items = split(*iter, items_separator);

            // Get the name of component 
            string comp_name = items.front();
            strip(comp_name);

            // Get the component
            try {
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
                component_ref = m_services->getComponent<MinorServo::WPServo>(("MINORSERVO/" + comp_name).c_str());
                if(!CORBA::is_nil(component_ref))
                    m_component_refs[comp_name] = component_ref;
                else {
                    m_isStarting = false;
                    THROW_EX(ManagementErrors, ConfigurationErrorEx, ("Cannot get component " + comp_name).c_str(), false);
                }
            }
            catch (maciErrType::CannotGetComponentExImpl& ex) {
                m_isStarting = false;
                THROW_EX(ManagementErrors, ConfigurationErrorEx, ("Cannot get the reference of " + comp_name).c_str(), false);
            }

            // Get the action
            string action(items.back());
            strip(action);

            if(startswith(action, "none") || startswith(action, "NONE")) {
                continue;
            }

            if(startswith(action, "park") || startswith(action, "PARK")) {
                m_servosToPark.push_back(comp_name);
                continue;
            }

            if(startswith(action, "power_off_encoder") || startswith(action, "POWER_OFF_ENCODER")) {
                m_servosToPowerOffEncoder.push_back(comp_name);
                continue;
            }

            m_servosToMove.push_back(comp_name);
            if(comp_name == "SRP" || comp_name == "PFP") {
                m_active_pfocus_servo = comp_name;
                if(comp_name == "PFP")
                    setElevationTracking("OFF"); 
            }

            vector<double> position_values;
            vector<string> aitems = split(action, coeffs_separator);
            vector<string> values;
            for(vector<string>::iterator viter = aitems.begin(); viter != aitems.end(); viter++) {
                vector<string> labels = split(*viter, coeffs_id);
                string coeffs = labels.back();
                string axis_and_unit(labels.front());
                strip(coeffs);

                // Get the axis and unit
                vector<string> tokens = split(axis_and_unit, string("("));
                string axis = tokens.front();
                strip(axis);
                m_axes.push_back(comp_name + string("_") + axis);
                string unit = tokens.back();
                strip(unit, string(")"));
                strip(unit);
                m_units.push_back(unit);
                // End axis and unit

                for(string::size_type idx = 0; idx != boundary_tokens.size(); idx++)
                    strip(coeffs, char2string(boundary_tokens[idx]));

                values = split(coeffs, pos_separator);
                vector<double> coefficients;
                for(vector<string>::size_type idx = 0; idx != values.size(); idx++) {
                    coefficients.push_back(str2double(values[idx]));
                }

                if(coefficients.empty()) {
                    m_isStarting = false;
                    m_axes.clear();
                    m_units.clear();
                    THROW_EX(ManagementErrors, ConfigurationErrorEx, ("No coefficients for " + comp_name).c_str(), false);
                }

                (m_servosCoefficients[comp_name]).push_back(coefficients);
                if(coefficients.size() > 1) {
                    m_dynamic_comps.push_back(comp_name);
                }
            }

            if(m_servosCoefficients.count(comp_name) && m_component_refs.count(comp_name)) {
                if((m_servosCoefficients[comp_name]).size() != (m_component_refs[comp_name])->numberOfAxes()) {
                    m_isStarting = false;
                    m_axes.clear();
                    m_units.clear();
                    THROW_EX(ManagementErrors, ConfigurationErrorEx, "Mismatch between number of coefficients and number of axes", false);
                }
            }
        }
        m_isValidCDBConfiguration = true;
 
        try {
            m_antennaBoss = Antenna::AntennaBoss::_nil();
            m_antennaBoss = m_services->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
            if(CORBA::is_nil(m_antennaBoss)) {
                ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::init(): _nil reference of AntennaBoss component"));
                m_isElevationTracking = false;
            }
        }
        catch (maciErrType::CannotGetComponentExImpl& ex) {
            m_isElevationTracking = false;
            ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::init(): cannot get the AntennaBoss component"));
        }
        if(keepSetup) {
            m_isStarting = false;
            m_isConfigured = true;
            m_actualSetup = m_commandedSetup;
        }
        if(mutex_res == 0 && pthread_mutex_unlock(&init_mutex)); 
    }
    catch(...) {
        if(mutex_res == 0 && pthread_mutex_unlock(&init_mutex)); 
        m_isStarting = false;
        m_isConfigured = false;
        m_actualSetup = "unknown";
        throw;
    }

}

std::string MSBossConfiguration::motionInfo(void) {
    if(isStarting()) {
        return std::string("Starting...");
    }
    else if(isParking()) {
        return std::string("Parking...");
    }
    else if(!isConfigured()) {
        return std::string("Not Ready to Move");
    }
    else if(m_isElevationTrackingEn) {
        return std::string("Elevation Track Mode");
    }
    else {
        return std::string("No Elevation Track Mode");
    }
}

InfoAxisCode MSBossConfiguration::getInfoFromAxisCode(string axis_code) throw (ManagementErrors::ConfigurationErrorExImpl) {
    InfoAxisCode result;
    short id = 0;
    string comp_name;
    strip(axis_code);
    if(axis_code.size() == 0) {
        result.comp_name = "";
        result.numberOfAxes = 0;
        result.axis_id = -1;
    }

    try {
        vector<string> items = split(axis_code, string("_"));
        comp_name = items[0];
        result.comp_name = comp_name;
    }
    catch(...) {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "Wrong axis code", false);
    }

    // Get the component
    MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
    if(m_component_refs.count(comp_name)) {
        component_ref = m_component_refs[comp_name];
        if(CORBA::is_nil(component_ref)) {
            string msg("getAxesPosition(): cannot get the reference of " + comp_name);
            THROW_EX(ManagementErrors, ConfigurationErrorEx, msg.c_str(), false);
        }
        result.numberOfAxes = component_ref->numberOfAxes();
    }
    else {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, ("Cannot get component " + comp_name).c_str(), false);
    }

    bool found = false;
    for(size_t i=0; i<m_axes.size(); i++) {
        vector<string> items = split(m_axes[i], string("_"));
        if(items[0] == comp_name) {
            if(m_axes[i] == axis_code) {
                found = true;
                break;
            }
            id++;
        }
    }
    if(found) {
        result.axis_id = id;
        return result;
    }
    else{
        THROW_EX(ManagementErrors, ConfigurationErrorEx, "Axis not active", false);
    }
}

void MSBossConfiguration::setASConfiguration(IRA::CString flag) throw (ManagementErrors::ConfigurationErrorExImpl) {
    const IRA::CString ON("ON");
    const IRA::CString OFF("OFF");
    if(flag == ON || flag == OFF) {
        m_isASConfiguration = (flag == ON) ? true : false;
    }
    else {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, string("setASConfiguration(): value ") + string(flag) + " not allowed.", false);
    }
}



void MSBossConfiguration::setElevationTracking(IRA::CString flag) throw (ManagementErrors::ConfigurationErrorExImpl) {
    const IRA::CString ON("ON");
    const IRA::CString OFF("OFF");
    if(flag == ON || flag == OFF) {
        m_isElevationTrackingEn = (flag == ON) ? true : false;
    }
    else {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, string("setElevationTracking(): value ") + string(flag) + " not allowed.", false);
    }
}

void MSBossConfiguration::setScan(
        ACS::Time starting_time, 
        ACS::Time total_time, 
        ACS::Time delta_time, 
        double range, 
        std::string comp_name, 
        unsigned short axis_index,
        std::string axis_code,
        ACS::doubleSeq actPos,
        ACS::doubleSeq centralPos,
        ACS::doubleSeq plainCentralPos,
        bool wasElevationTrackingEn
        )
{
    m_scan.starting_time = starting_time;
    m_scan.total_time = total_time;
    m_scan.delta_time = delta_time;
    m_scan.range = range;
    m_scan.comp_name = comp_name;
    m_scan.axis_index = axis_index;
    m_scan.actPos = actPos;
    m_scan.centralPos = centralPos;
    m_scan.plainCentralPos = plainCentralPos;
    m_scan.wasElevationTrackingEn = wasElevationTrackingEn;
    m_scan.axis_code = axis_code;
}


bool MSBossConfiguration::isTracking() { 
    return m_isTracking && !m_trackingError; 
}

ACS::doubleSeq MSBossConfiguration::getPosition(string comp_name, ACS::Time time)
    throw (ManagementErrors::ConfigurationErrorExImpl)
{
    double elevation = 45.0 / DR2D; // From decimal to radians
    double azimuth;
    if(isElevationTrackingEn()) {
        try {
            if(!CORBA::is_nil(m_antennaBoss)) {
                m_antennaBoss->getRawCoordinates(time, azimuth, elevation);
                m_isElevationTracking = true;
                m_antennaBossError = false;
            }    
            else {
                try {
                    m_antennaBoss = m_services->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
                    m_antennaBossError = false;
                    if(CORBA::is_nil(m_antennaBoss)) {
                        if(!m_antennaBossError) {
                            ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::init(): _nil reference of AntennaBoss component"));
                            m_antennaBossError = true;
                        }
                        m_isElevationTracking = false;
                    }
                }
                catch (maciErrType::CannotGetComponentExImpl& ex) {
                    m_isElevationTracking = false;
                    if(!m_antennaBossError) {
                        ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::getPosition(): cannot get the AntennaBoss component"));
                        m_antennaBossError = true;
                    }
                }
            }
        }    
        catch (ComponentErrors::ComponentErrorsEx& ex) {
            if(!m_antennaBossError) {
                ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::getPosition(): cannot get the AntennaBoss component"));
                m_antennaBossError = true;
            }
            m_antennaBoss = Antenna::AntennaBoss::_nil();
            m_isElevationTracking = false;
        }    
        catch(AntennaErrors::AntennaErrorsEx& ex) {
            if(!m_antennaBossError) {
                ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::getPosition(): cannot get the antenna elevation"));
                m_antennaBossError = true;
            }
            m_antennaBoss = Antenna::AntennaBoss::_nil();
            m_isElevationTracking = false;
        }    
        catch(CORBA::SystemException& ex) {
            if(!m_antennaBossError) {
                ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::getPosition(): the AntennaBoss component is not active"));
                m_antennaBossError = true;
            }
            m_antennaBoss = Antenna::AntennaBoss::_nil();
            m_isElevationTracking = false;
            try {
                m_services->releaseComponent("ANTENNA/Boss");
                // The component will be reloaded the next time getPosition() is called
            }
            catch(maciErrType::CannotReleaseComponentExImpl& ex) {
                if(!m_antennaBossError) {
                    ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::getPosition(): cannot release the AntennaBoss component"));
                    m_antennaBossError = true;
                }
            }
        }    
        catch(...) {
            if(!m_antennaBossError) {
                ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::getPosition(): unknown error getting the elevation"));
                m_antennaBossError = true;
            }
            m_antennaBoss = Antenna::AntennaBoss::_nil();
            m_isElevationTracking = false;
        }    
    }
    return getPositionFromElevation(comp_name, elevation);
}



ACS::doubleSeq MSBossConfiguration::getPositionFromElevation(string comp_name, double elevation) 
    throw (ManagementErrors::ConfigurationErrorExImpl)
{
    ACS::doubleSeq positions;
    if(m_servosCoefficients.count(comp_name)) {
        try {
            pthread_mutex_lock(&position_mutex);

            elevation = elevation * DR2D; // From radians to decimal (be sure we want to use decimal...)
            vector<vector<double> > vcoeff = m_servosCoefficients[comp_name]; 
            positions.length(vcoeff.size());
            size_t position_idx(0);
            for(size_t j=0; j != vcoeff.size(); j++) {
                double axis_value(0);
                // For instance if an axis has the following coefficients: [C0, C1, C2], the axis value will be:
                // C0 + C1*E + C2*E^2
                for(size_t idx = 0; idx != (vcoeff[j]).size(); idx++)
                    axis_value += (vcoeff[j])[idx] * pow(elevation, idx);

                positions[position_idx] = axis_value;
                position_idx++;
            }
            pthread_mutex_unlock(&position_mutex);
        }
        catch (...) {
            pthread_mutex_unlock(&position_mutex);
            THROW_EX(ManagementErrors, 
                     ConfigurationErrorEx, 
                     "cannot compute the" + comp_name + "position", 
                     false);
        }
    }
    else {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, comp_name + " has no coefficients", false);
    }
    return positions;
}

