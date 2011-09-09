#ifndef _RECVBOSSCORE_H_
#define _RECVBOSSCORE_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  21/07/2010      Creation                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  28/02/2011      For computation is now considered the real band, given by the receiver band and the backend filter */

#ifdef COMPILE_TARGET_MED

#include <ManagmentDefinitionsC.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <ReceiversErrors.h>
#include <ReceiversBossS.h>
#include <ReceiversDefinitionsS.h>
#include <IRA>
#include "Configuration.h"

#define _RECVBOSSCORE_MAX_IFS 4

#else

#include <ManagmentDefinitionsC.h>
#include <GenericReceiverS.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <ReceiversErrors.h>
#include <IRA>
#include "Configuration.h"

#endif

class CRecvBossCore {
public:
	/**
	 * Constructor. Default Constructor.
	 * @param service pointer to the container services.
	 * @param conf pointer to the class CConfiguration tat conatains all the configuration regarding the component
	 * @param me pointer to the component itself
	*/
	CRecvBossCore();

	/** 
	 * Destructor.
	*/ 
	virtual ~CRecvBossCore();
	
	/**
	 * This function starts the boss core  so that it will available to accept operations and requests.
	 * @throw ComponentErrors::IRALibraryResourceExImpl
	 * @throw ComponentErrors::CDBAccessExImpl
	*/
	virtual void execute() throw (ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl);
	
	/**
	 * This function initializes the boss core, all preliminary operation are performed here.
	*/
	virtual void initialize(maci::ContainerServices* services,CConfiguration *config);
	
	/** 
	 * This function performs all the clean up operation required to free al the resources allocated by the class
	*/
	virtual void cleanUp();
	
	void calOn() throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl);
	
	void calOff() throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl);
	
	void setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl);
	
	/**
	 * This method will configure the component that controls the receiver actually identified by the given code
	 * @param code identifier code of the receiver
	 */
	void setupReceiver(const char * code) throw (ManagementErrors::ConfigurationErrorExImpl);

	void setMode(const char * mode) throw (ComponentErrors::ValidationErrorExImpl,ReceiversErrors::ModeErrorExImpl);
	
	void park();
	
	long getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power) throw (ComponentErrors::ValidationErrorExImpl);
	
	double getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl);
	
	void getCalibrationMark(ACS::doubleSeq& result,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,const ACS::longSeq& ifs) throw (
			ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl);
	
	void getLO(ACS::doubleSeq& lo);
	
	void getInitialFrequency(ACS::doubleSeq& iFreq);
	
	void getBandWidth(ACS::doubleSeq& bw);
	
	void getPolarization(ACS::longSeq& pol);
	
	void getFeeds(long& feeds);
	
	void getIFs(long& ifs);
	
	const IRA::CString& getRecvCode() const;
	
	const IRA::CString& getOperativeMode() const;

	const Management::TSystemStatus& getStatus() const;
private:


	Management::TSystemStatus m_status;
	maci::ContainerServices* m_services;
	CConfiguration *m_config;
	BACIMutex m_mutex;

#ifdef COMPILE_TARGET_MED
	/**
	 * Socket to the KBand Recevier
	 */
	IRA::CSocket m_kBandSocket;
	
	/**
	 * Socket for the Receviers configuration
	 */
	IRA::CSocket m_recvSocket;
	IRA::CSocket m_fsSocket;
	bool m_recvOpened;
	bool m_fsOpened;
	IRA::CDBTable *m_KKCFeedTable;
	double m_LO[_RECVBOSSCORE_MAX_IFS];
	long m_feeds;
	long m_IFs;
	Receivers::TPolarization m_pols[_RECVBOSSCORE_MAX_IFS];
	double m_startFreq[_RECVBOSSCORE_MAX_IFS];
	double m_bandWidth[_RECVBOSSCORE_MAX_IFS];
	IRA::CString m_currentReceiver;
	IRA::CString m_currentOperativeMode;

#else
	Receivers::Receiver_var m_currentRecv;
	bool m_currentRecvError;
	IRA::CString m_currentRecvInstance;
	Management::TSystemStatus m_bossStatus;
	ACS::Time  m_lastStatusChange;


	void loadReceiver(const IRA::CString instance) throw (ComponentErrors::CouldntGetComponentExImpl);
	void unloadReceiver();
	void changeBossStatus(const Management::TSystemStatus& status);


#endif

#ifdef COMPILE_TARGET_MED
	void setup(const char * code) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl);
#else
	void setup(const char * code) throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
			ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl);
#endif

};

#endif
