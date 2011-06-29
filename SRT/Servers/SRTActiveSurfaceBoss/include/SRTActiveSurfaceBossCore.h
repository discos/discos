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

#define CIRCLES 17
#define ACTUATORS 96
#define firstUSD 1
#define lastUSD 139 // TEST ONLY
//#define lastUSD 1116 // TEST ONLY, all actuators are 1116
#define LOOPTIME 1000 // 1 msec
#define USDTABLE "/home/almamgr/SRT/CDB/alma/AS/tab_convUSD.txt\0"
#define USDTABLECORRECTIONS "/home/almamgr/SRT/CDB/alma/AS/actuatorsCorrections.txt\0"
#define MM2HSTEP	350 //(10500 HSTEP / 30 MM)
#define MM2STEP	1400 //(42000 HSTEP / 30 MM)
#define WARNINGUSDPERCENT 0.95
#define ERRORUSDPERCENT 0.90

// mask pattern for status 
#define MRUN	0x000080
#define CAMM	0x000100
#define ENBL	0x002000
#define PAUT	0x000800
#define CAL 	0x008000

using namespace IRA;
using namespace baci;
using namespace maci;
using namespace ComponentErrors;
using namespace std;

class SRTActiveSurfaceBossImpl;
class CSRTActiveSurfaceBossWatchingThread;

/**
 * This class models the SRTActiveSurfaceBoss datasets and functionalities. 
 * @author <a href=mailto:migoni@ca.astro.it>Migoni Carlo</a>
 * Osservatorio Astronomico di Cagliari, Italia
 * <br> 
 */
class CSRTActiveSurfaceBossCore {
	friend class SRTActiveSurfaceBossImpl;
	friend class CSRTActiveSurfaceBossWatchingThread;
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

    void reset(int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void calibrate(int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl);

    void calVer(int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl);

    void onewayAction(SRTActiveSurface::TASOneWayAction onewayAction, int circle, int actuator, int radius, CORBA::Long elevation, double correction, long incr, SRTActiveSurface::TASProfile profile) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void workingActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);

    void watchingActiveSurfaceStatus() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void usdStatus4GUIClient(int circle, int actuator, CORBA::Long_out status) throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl);

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
    
	/**
	 * Sets the <i>AutoUpdate</i> flag to false, i.e. the component will not update automatically the surface. 
	*/
	void disableAutoUpdate();
	
	/**
	 * Sets the <i>AutoUpdate</i> flag to true, i.e. the component will update automatically the surface. 
	*/	
	void enableAutoUpdate();

    void checkASerrors(char* str, int circle, int actuator, ASErrors::ASErrorsEx Ex);

	void checkAScompletionerrors (char *str, int circle, int actuator, CompletionImpl comp);

    void setupAS() throw (ComponentErrors::ComponentErrorsEx);

    void startAS();

    void stopAS() throw (ComponentErrors::ComponentErrorsEx);

    void stowAS() throw (ComponentErrors::ComponentErrorsEx);

private:
	ContainerServices* m_services;

	SRTActiveSurface::USD_var usd[CIRCLES+1][ACTUATORS+1];

	SRTActiveSurface::USD_var lanradius[CIRCLES+1][ACTUATORS+1];

	int usdCounter;
	int lanIndex, circleIndex, usdCircleIndex;
    int actuatorcounter, circlecounter, totacts;
    double actuatorsCorrections[100440];

    /** pointer to the component itself */
	acscomponent::ACSComponentImpl *m_thisIsMe;

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

    Antenna::AntennaBoss_var m_antennaBoss;

    SRTActiveSurface::TASProfile m_profile;
};

#endif /*SRTACTIVESURFACEBOSSCORE_H_*/
