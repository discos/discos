#ifndef _SRTACTIVESURFACEBOSSCORE_H_
#define _SRTACTIVESURFACEBOSSCORE_H_

/* ************************************************************************ */
/* OAC Osservatorio Astronomico di Cagliari                                 */
/* $Id: SRTActiveSurfaceBossCore.h,v 1.6 2011-03-11 12:30:53 c.migoni Exp $ */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                  when        What                    */
/* Carlo Migoni (migoni@ca.astro.it)   26/01/2009  Creation                 */

#include <acsContainerServices.h>
#include <maciContainerServices.h>
#include <acsThread.h>
#include <baci.h>
#include <DateTime.h>
#include <usdC.h>
#include <lanC.h>
#include <AntennaBossC.h>
#include <ComponentErrors.h>
#include <ASErrors.h>
#include <ManagmentDefinitionsS.h>
#include <SRTActiveSurfaceBossS.h>
#include <IRA>
#include <fstream>
#include <iostream>
#include <slamac.h>
#include <vector>

#define SECTORS 8
#define CIRCLES 17
#define ACTUATORS 96
#define firstUSD 1
#define lastUSD 1116
#define LOOPTIME 100000 // 0,10 sec
#define CDBPATH std::string(getenv("ACS_CDB")) + "/CDB/"
#define USDTABLE CDBPATH + "alma/AS/tab_convUSD.txt"
#define USDTABLECORRECTIONS CDBPATH + "alma/AS/act_rev02.txt"
#define MM2HSTEP    350 //(10500 HSTEP / 30 MM)
#define MM2STEP     1400 //(42000 STEP / 30 MM)
#define WARNINGUSDPERCENT 0.95
#define ERRORUSDPERCENT 0.90
#define THRESHOLDPOS 16 // 12 micron in step
#define NPOSITIONS 7
#define DELTAEL 15.0

// mask pattern for status
#define UNAV    0xFF000000
#define MRUN    0x000080
#define CAMM    0x000100
#define ENBL    0x002000
#define PAUT    0x000800
#define CAL     0x008000

#define _SET_CDB_CORE(PROP,LVAL,ROUTINE) {    \
        if (!CIRATools::setDBValue(m_services,#PROP,(const long&) LVAL)) \
        { ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
            exImpl.setFieldName(#PROP); throw exImpl; \
        } \
}

using namespace IRA;
using namespace baci;
using namespace maci;
using namespace ComponentErrors;
using namespace std;

class SRTActiveSurfaceBossImpl;
//class CSRTActiveSurfaceBossWatchingThread;
class CSRTActiveSurfaceBossWorkingThread;

/**
 * This class models the SRTActiveSurfaceBoss datasets and functionalities.
 * @author <a href=mailto:migoni@ca.astro.it>Migoni Carlo</a>
 * Osservatorio Astronomico di Cagliari, Italia
 * <br>
 */
class CSRTActiveSurfaceBossCore {
    friend class SRTActiveSurfaceBossImpl;
    //friend class CSRTActiveSurfaceBossWatchingThread;
    friend class CSRTActiveSurfaceBossWorkingThread;
    friend class CSRTActiveSurfaceBossSectorThread;
    friend class CSRTActiveSurfaceBossInitializationThread;
public:
    /**
     * Constructor. Default Constructor.
     * @param service pointer to the continaer services.
     * @param me pointer to the component itself
    */
    CSRTActiveSurfaceBossCore(ContainerServices *service,acscomponent::ACSComponentImpl *me);

    /**
     * Destructor.
    */
    virtual ~CSRTActiveSurfaceBossCore();

    /**
     * This function initializes the boss core, all preliminary operation are performed here.
    */
    virtual void initialize();

    /**
     * This function starts the boss core  so that it will available to accept operations and requests.
     * @throw ComponentErrors::CouldntGetComponentExImpl
     * @throw ComponentErrors::CORBAProblemExImpl
    */
    virtual void execute() throw (ComponentErrors::CouldntGetComponentExImpl);

    /**
     * This function performs all the clean up operation required to free al the resources allocated by the class
    */
    virtual void cleanUp();

    void calibrate(int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl);

    void calVer(int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl);

    void onewayAction(ActiveSurface::TASOneWayAction action, int circle, int actuator, int radius, double elevation, double correction, long incr, ActiveSurface::TASProfile profile);

    void workingActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);

    //void watchingActiveSurfaceStatus() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void usdStatus4GUIClient(int circle, int actuator, CORBA::Long_out status) throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void asStatus4GUIClient(ACS::longSeq& status) throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void setActuator(int circle, int actuator, long int& actPos, long int& cmdPos, long int& Fmin, long int& Fmax, long int& acc, long int& delay) throw (ComponentErrors::PropertyErrorExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void recoverUSD(int circle, int actuator) throw (ComponentErrors::CouldntGetComponentExImpl);

    /**
     * This function returns the status of the Active Surface subsystem; this indicates if the system is working correctly or there are some
     * possible problems or a failure has been encoutered. This flag takes also into consideration the status of the Boss.
     */
    inline const Management::TSystemStatus& getStatus() const { return m_status; };

    /**
     * This function returns the enable flags, this flag is true if the boss is enable to send command to the antenna. Viceversa if false the component
     * works in a sort of simulation mode.
     * @return a boolean value that is true if the antenna is enabled
    */
    inline bool getEnable() const { return m_enable; }

    inline const ActiveSurface::TASProfile& getProfile() const { return m_profile; };

    inline bool getTracking() const { return m_tracking; }

    inline std::string getLUTfilename() const { return m_lut.substr(m_lut.find_last_of('/') + 1); }

    /**
     * Sets the <i>AutoUpdate</i> flag to false, i.e. the component will not update automatically the surface.
    */
    void disableAutoUpdate();

    /**
     * Sets the <i>AutoUpdate</i> flag to true, i.e. the component will update automatically the surface.
    */
    void enableAutoUpdate();

    void checkASerrors(const char* str, int circle, int actuator, ASErrors::ASErrorsEx Ex);
    void checkASerrors(const char *str, int circle, int actuator, CompletionImpl comp);
    void checkASerrors(const char *str, int circle, int actuator, int code);

    void asSetup() throw (ComponentErrors::ComponentErrorsEx);

    void asOn();

    void asOff() throw (ComponentErrors::ComponentErrorsEx);

    void asPark() throw (ComponentErrors::ComponentErrorsEx);

    void setProfile (const ActiveSurface::TASProfile& profile) throw (ComponentErrors::ComponentErrorsExImpl);

    void asSetLUT(const char* newlut);

private:
    std::map<int, std::string> m_error_strings;
    ContainerServices* m_services;

    ActiveSurface::USD_var usd[CIRCLES+1][ACTUATORS+1];

    ActiveSurface::USD_var lanradius[CIRCLES+1][ACTUATORS+1];

    ActiveSurface::lan_var lan[9][13];

    IRA::CString lanCobName;

    int usdCounter;
    std::vector<int> usdCounters;
    int actuatorcounter, circlecounter, totacts;
    ACS::doubleSeq actuatorsCorrections;

    /**
     * This represents the status of the whole Active Surface subsystem, it also includes and sammerizes the status of the boss component
     */
    Management::TSystemStatus m_status;

    /**
     * if this flag is false the active surface isn't active during
     * observations
    */
    bool m_enable;

    bool AutoUpdate;

    void setradius(int radius, int &actuatorsradius, int &jumpradius);

    void setserial (int circle, int actuator, int &lanIndex, char *serial_usd);

    void singleUSDonewayAction(ActiveSurface::TASOneWayAction action, ActiveSurface::USD_var usd, double elevation, double correction, long incr, ActiveSurface::TASProfile profile);

    Antenna::AntennaBoss_var m_antennaBoss;

    ActiveSurface::TASProfile m_profile;

    bool m_tracking;

    char *s_usdTable;

    char *s_usdCorrections;

    bool m_profileSetted;

    bool m_ASup;
    
    bool m_newlut;

    bool m_initialized;

    std::string m_lut;

    std::vector<int> actuatorsInCircle;
};

#endif /*SRTACTIVESURFACEBOSSCORE_H_*/
