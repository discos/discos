#ifndef _NOTOACTIVESURFACEBOSSCORE_H_
#define _NOTOACTIVESURFACEBOSSCORE_H_

/* ************************************************************************ */
/* OAC Osservatorio Astronomico di Cagliari                                 */
/* $Id: NotoActiveSurfaceBossCore.h,v 1.6 2011-03-11 12:30:53 c.migoni Exp $ */
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
//#include <usdC.h>
//#include <lanC.h>
#include <AntennaBossC.h>
#include <ComponentErrors.h>
#include <ASErrors.h>
#include <ManagmentDefinitionsS.h>
#include <NotoActiveSurfaceBossS.h>
#include <IRA>
#include <fstream>
#include <iostream>
#include <slamac.h>
#include "Configuration.h"

#define CIRCLES 17
#define ACTUATORS 96
#define firstUSD 1
#define lastUSD 1116
#define LOOPTIME 100000 // 0,10 sec
#define USDTABLE "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD.txt\0"
#define USDTABLECORRECTIONS "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/act_rev02.txt\0"
#define MM2HSTEP	350 //(10500 HSTEP / 30 MM)
#define MM2STEP	1400 //(42000 STEP / 30 MM)
#define WARNINGUSDPERCENT 0.95
#define ERRORUSDPERCENT 0.90
#define THRESHOLDPOS 16 // 12 micron in step
#define NPOSITIONS 7
#define DELTAEL 15.0

// mask pattern for status 
#define MRUN	0x000080
#define CAMM	0x000100
#define ENBL	0x002000
#define PAUT	0x000800
#define CAL 	0x008000

#define SENDBUFFERSIZE 100
#define RECBUFFERSIZE 512
#define PROT_TERMINATOR_CH '\n'

/*
#define _SET_CDB_CORE(PROP,LVAL,ROUTINE) {	\
		if (!CIRATools::setDBValue(m_services,#PROP,(const long&) LVAL)) \
		{ ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
			exImpl.setFieldName(#PROP); throw exImpl; \
		} \
}
*/

using namespace IRA;
using namespace baci;
using namespace maci;
using namespace ComponentErrors;
using namespace std;

class NotoActiveSurfaceBossImpl;
class CNotoActiveSurfaceBossWatchingThread;
class CNotoActiveSurfaceBossWorkingThread;

/**
 * This class models the NotoActiveSurfaceBoss datasets and functionalities. 
 * @author <a href=mailto:migoni@ca.astro.it>Migoni Carlo</a>
 * Osservatorio Astronomico di Cagliari, Italia
 * <br> 
 */
class CNotoActiveSurfaceBossCore : public CSocket {
	friend class NotoActiveSurfaceBossImpl;
	friend class CNotoActiveSurfaceBossWatchingThread;
	friend class CNotoActiveSurfaceBossWorkingThread;
public:
	enum TLineStatus {
		NOTCNTD,   	        /*!< Socket is not connected, no operation available  */
		CNTDING,              /*!< Connection is in progress, no operation available */
		CNTD  		           /*!< Socket is connected and ready, line is ready */
	};

	/**
	 * Constructor. Default Constructor.
	 * @param service pointer to the continaer services.
	 * @param me pointer to the component itself
	*/
	CNotoActiveSurfaceBossCore(ContainerServices *service);
		
	/** It contains error information */	
	CError m_Error;

	/** 
	 * Destructor.
	*/ 
	virtual ~CNotoActiveSurfaceBossCore();
	
	/**
	 * This function initializes the boss core, all preliminary operation are performed here.
	*/
	virtual void initialize();

	/**
	 * This function starts the boss core  so that it will available to accept operations and requests.
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	*/
	virtual void execute(CConfiguration *config) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::SocketErrorExImpl,
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::CDBAccessExImpl);
	
	/**
	 * This function performs all the clean up operation required to free al the resources allocated by the class
	*/
	virtual void cleanUp();

    void reset(int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void calibrate(int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl);

    void calVer(int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl);

    void onewayAction(ActiveSurface::TASOneWayAction onewayAction, int circle, int actuator, int radius, double elevation, double correction, long incr, ActiveSurface::TASProfile profile) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentNotActiveExImpl);

    void workingActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);

    void sector1ActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);
    void sector2ActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);
    void sector3ActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);
    void sector4ActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);
    void sector5ActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);
    void sector6ActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);
    void sector7ActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);
    void sector8ActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx);

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

	inline const ActiveSurface::TASProfile& getProfile() const { return m_profile; };

	inline bool getTracking() const { return m_tracking; }

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

    	void asSetup() throw (ComponentErrors::ComponentErrorsEx);

    	void asOn();

    	void asOff() throw (ComponentErrors::ComponentErrorsEx);

	void asPark() throw (ComponentErrors::ComponentErrorsEx);

	void setProfile (const ActiveSurface::TASProfile& profile) throw (ComponentErrors::ComponentErrorsExImpl);

private:
	ContainerServices* m_services;
	/** Component configuration data */
	CConfiguration *m_configuration;

	/**This flag indicates if the socket has timedout */
	bool m_bTimedout;
	bool m_reiniting;

	/**
	 * List the fields of the backend status 
	 */
	enum TstatusFields {
		TIME_SYNC=0,             /*!< backend time not sync  */
		BUSY=1,                        /*!< backend is busy(transfer job initiated) and cannot accept other commands */
		SUSPEND=2,				  /*!< backend data flow is suspended */
		SAMPLING=3,              /*!< backend is recording */
		CMDLINERROR=4,      /*!< error in the command line */ 
		DATALINERROR=5     /*!< error in the data line */
	};

	/** Connection status */
	TLineStatus m_Linestatus;

//	ActiveSurface::USD_var usd[CIRCLES+1][ACTUATORS+1];

//	ActiveSurface::USD_var lanradius[CIRCLES+1][ACTUATORS+1];

//	ActiveSurface::lan_var lan[9][13];

//	IRA::CString lanCobName;

	int usdCounter, usdCounterS1, usdCounterS2, usdCounterS3, usdCounterS4, usdCounterS5, usdCounterS6, usdCounterS7, usdCounterS8;
	int lanIndex, circleIndex, usdCircleIndex;
	int lanIndexS1, circleIndexS1, usdCircleIndexS1;
	int lanIndexS2, circleIndexS2, usdCircleIndexS2;
	int lanIndexS3, circleIndexS3, usdCircleIndexS3;
	int lanIndexS4, circleIndexS4, usdCircleIndexS4;
	int lanIndexS5, circleIndexS5, usdCircleIndexS5;
	int lanIndexS6, circleIndexS6, usdCircleIndexS6;
	int lanIndexS7, circleIndexS7, usdCircleIndexS7;
	int lanIndexS8, circleIndexS8, usdCircleIndexS8;
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

    Antenna::AntennaBoss_var m_antennaBoss;

    ActiveSurface::TASProfile m_profile;

	bool m_tracking;

	char *s_usdTable;

	char *s_usdCorrections;

	bool m_sector1, m_sector2, m_sector3, m_sector4, m_sector5, m_sector6, m_sector7, m_sector8;

	bool m_profileSetted;
	
	bool m_ASup;

	/**
	 * Check if the connection is still up and ready. If a previuos call timed out it will check the status of the connection.
	 * Insted if the connection is off it will try a reconnect. In both case the return value is false.
	 * @return true if the connection is ok
	 */ 
	bool checkConnection();

	/**
	* This member function is called to send a command to the backend
	 * @param Msg ponter to the byte buffer that contains the message
	 * @param Len length of the buffer
	 * @return SUCCESS if the buffer was sent correctly, WOULDBLOCK if the send would block, FAIL and the <i>m_Error</i> member is set accordingly.
	*/
	OperationResult sendBuffer(char *Msg,WORD Len);

	/**
    * This member function is called to receive a buffer. if it realizes that the remote side disconnected it sets the member <i>m_Status</i> 
	 * to NOTCNTD.
	 * @param Msg ponter to the byte buffer that will contain the answer
	 * @param Len length of the buffer, that means the exact number of bytes that can be read
	 * @return the number of bytes read, 0 if the connection fell down, FAIL in case of error and the <i>m_Error</i> member is set accordingly, 
	 *         WOULDBLOCK if the timeout expires
	*/
	int receiveBuffer(char *Msg,WORD Len);

	/** 
	 * Sets the antenna status.
	 * @param sta the new antenna status.
	*/
	void setStatus(TLineStatus sta);

	/**
	 * @return the current line status
	*/
	inline const TLineStatus& getLineStatus() const { return m_Linestatus; }

	/**
	 * This function will set the status bit corresponding to the given field 
	 */
	inline void setStatusField(TstatusFields field) { /*m_backendStatus |= 1 << field;*/ }
	/**
	 * This function will unset (cleared) the status bit corresponding to the given field 
	 */
	inline void clearStatusField(TstatusFields field) { /*m_backendStatus &= ~(1 << field);*/ }
};

#endif /*NOTOACTIVESURFACEBOSSCORE_H_*/
