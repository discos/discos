#include "MSBossConfiguration.h"
#include "utils.h"

using namespace IRA;
using namespace std;

MSBossConfiguration::MSBossConfiguration(maci::ContainerServices *Services)
{
    m_actualSetup = "unknown";
    m_commandedSetup = "";
    m_isConfigured = false;
    m_isStarting = false;
    m_isParking = false;
    m_isElevationTracking = false;
    m_isScanning = false;
    m_dynamic_comps.clear();
    m_services = Services;
    m_isTrackingEn = false;
    m_isASConfiguration = false;
    m_nchannel = NULL;
}

MSBossConfiguration::~MSBossConfiguration() {}


void MSBossConfiguration::init(string setupMode) throw (ManagementErrors::ConfigurationErrorExImpl)
{
    // Starting
    m_isStarting = true;
    m_commandedSetup = string(setupMode);
    m_isASConfiguration = false;
    ACS::doubleSeq dummy;

    IRA::CError error;
    try {
        m_as_ref =  ActiveSurface::SRTActiveSurfaceBoss::_nil();
        m_as_ref = m_services->getComponent<ActiveSurface::SRTActiveSurfaceBoss>("AS/Boss");
        if(!CORBA::is_nil(m_as_ref)) {
            if(false) { // TODO: remove this line and decomment the one below
            // if(m_as_ref->isReadyToUpdate()) {
                setupMode = setupMode + string("_ASACTIVE");
                m_isASConfiguration = true;
            }
            else {
                ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::init(): 'NO ACTIVE SUFRACE' configuration."));
            }
        }
        else {
            ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::init(): nil ASBoss reference: 'NO ACTIVE SUFRACE' configuration."));
        }
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::init(): cannot get the ASBoss component: 'NO ACTIVE SUFRACE' configuration."));
    }

    // Read the component configuration
    IRA::CString config;
    if(!CIRATools::getDBValue(m_services, setupMode.c_str(), config)) {
        m_isStarting = false;
        THROW_EX(ManagementErrors, ConfigurationErrorEx, setupMode + ": unavailable configuration code.", false);
    }

    // initializing
    m_isConfigured = false;
    m_servosToPark.clear();
    m_servosToMove.clear();
    m_servosCoefficients.clear();
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

        m_servosToMove.push_back(comp_name);
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
}

void MSBossConfiguration::setScan(
        ACS::Time starting_time, 
        ACS::Time total_time, 
        ACS::Time delta_time, 
        double range, 
        std::string comp_name, 
        unsigned short axis_index,
        ACS::doubleSeq actPos
        )
{
    m_scan.starting_time = starting_time;
    m_scan.total_time = total_time;
    m_scan.delta_time = delta_time;
    m_scan.range = range;
    m_scan.comp_name = comp_name;
    m_scan.axis_index = axis_index;
    m_scan.actPos = actPos;
}



ACS::doubleSeq MSBossConfiguration::getPosition(string comp_name, double elevation)
    throw (ManagementErrors::ConfigurationErrorExImpl)
{
    ACS::doubleSeq positions;
    if(m_servosCoefficients.count(comp_name)) {
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
    }
    else {
        THROW_EX(ManagementErrors, ConfigurationErrorEx, comp_name + "has no coefficients", false);
    }

    return positions;
}

