/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __BOSSCONFIGURATION_H__
#define __BOSSCONFIGURATION_H__


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <IRA>
#include <ComponentErrors.h>
#include <vector>
#include <string>
#include <map>
#include <baciROdoubleSeq.h>
#include <MinorServoS.h>
#include <WPServoImpl.h>
#include <ManagementErrors.h>
#include <ManagmentDefinitionsS.h>
#include <macros.def>
#include <SP_parser.h>

const std::string actions_separator = "@";
const std::string items_separator = ":";
const std::string pos_separator = ",";
const std::string park_token = "park";
const std::string boundary_tokens = "()";
const std::string slaves_separator = ",";
const std::string coeffs_separator = ";";
const std::string coeffs_id = "=";

/**
 * This class stores the actual minor servo system configuration. The data inside this class are 
 * initialized at the startup from the configuration database and then are used (read) inside the component.
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * INAF Osservatorio Astronomico di Cagliari, Italia
 * <br> 
  */
class MSBossConfiguration {

public:

    /** Default constructor */
    MSBossConfiguration(maci::ContainerServices *Services);

    /** Destructor */
    ~MSBossConfiguration();
    
    void init(std::string commandedSetup) throw (ManagementErrors::ConfigurationErrorEx);

    inline bool isConfigured() { return m_isConfigured; }

    inline bool isStarting() { return m_isStarting; }

    inline bool isParking() { return m_isParking; }

    inline bool isElevationTracking() { return m_isElevationTracking; }

    inline bool isTrackingEn() { return m_isTrackingEn; }

    inline bool isValidCDBConfiguration() { return m_isValidCDBConfiguration; }

    inline std::string getActualSetup() { return m_actualSetup; }

    inline std::string getCommandedSetup() { return m_commandedSetup; }

    inline std::vector<std::string> getServosToPark() { return m_servosToPark; }

    inline std::vector<std::string> getServosToMove() { return m_servosToMove; }

    inline std::vector<std::string> getDynamicComponents() { return m_dynamic_comps; }

    /** 
     * Return a doubleSeq of positions to set
     * @param comp_name string component name
     * @param elevation the antenna elevation
     * @return doubleSeq of positions to set
     */
    ACS::doubleSeq getPosition(string comp_name, double elevation=0)
        throw (ManagementErrors::ConfigurationErrorExImpl);

    std::string m_actualSetup;
    std::string m_commandedSetup;
    bool m_isConfigured;
    bool m_isStarting;
    bool m_isParking;
    bool m_isValidCDBConfiguration;
    bool m_isElevationTracking;
    bool m_isTrackingEn; // Is the tracking enabled?

    std::vector<std::string> m_servosToPark;

    std::vector<std::string> m_servosToMove;

    /** Components with number of coefficients greather than 1 */
    std::vector<std::string> m_dynamic_comps;

    std::map<std::string, std::vector<std::vector<double> > > m_servosCoefficients;

    Management::TSystemStatus m_status;

    /** Map of component references */
    map<string, MinorServo::WPServo_var> m_component_refs;

    maci::ContainerServices * m_services;
};


#endif
