#ifndef RECEIVERSBOSSIMPL_H_
#define RECEIVERSBOSSIMPL_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                when                     What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 19/11/2009       Creation                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it) 22/07/2010       Given implementation of command function                   */
/* Andrea Orlati(aorlati@ira.inaf.it) 13/09/2010       Given implementation of new signature of getTaper() method  */
/* Andrea Orlati(aorlati@ira.inaf.it) 12/02/2013       Changed the method command as new commandInterpreter interface requires  */



#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <ReceiversBossS.h>
#include <baciROdoubleSeq.h>
#include <baciROlongSeq.h>
#include <baciROstring.h>
#include <baciROlong.h>
#include <enumpropROImpl.h>
#include <SP_parser.h>
#include <ReceiversErrors.h>
#include <IRA>
#include "Configuration.h"
#include "RecvBossCore.h"
#include "WatchingThread.h"

/** 
 * @mainpage ReceiversBoss component Implementation 
 * @date 18/06/2013
 * @version 0.42.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 8.2.0
 * @remarks compiler version is 4.1.2
*/

/**
 * @author <a href=mailto:a.orlati@ira.inaf.it>Orlati Andrea</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class ReceiversBossImpl: public baci::CharacteristicComponentImpl,
				       public virtual POA_Receivers::ReceiversBoss
{
public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	ReceiversBossImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~ReceiversBossImpl(); 

	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters or 
	 * builds up connection to devices or other components. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);

	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming 
 	 * functional calls any time. 
	 * Must be implemented as a synchronous (blocking) call. In this class the default implementation only 
	 * logs the COMPSTATE_OPERATIONAL
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	
	/** 
	 * Called by the container before destroying the server in a normal situation. This function takes charge of 
	 * releasing all resources.
	*/
	virtual void cleanUp();
	
	/** 
	 * Called by the container in case of error or emergency situation. This function tries to free all resources 
	 * even though there is no warranty that the function is completely executed before the component is destroyed.
	*/	
	virtual void aboutToAbort();
		
	/**
	 * This method is used to turn the calibration diode on.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx  
	 * @thorw ReceiversErrors::ReceiversErrorsEx
	 */
	void calOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * This method is used to turn the calibration diode off.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx  
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	void calOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
	
	/**
	 * This method allows to set local oscillator. Depending on the curretly configured receiver one or more values are considered.
	 * @param lo the list contains the values in MHz for the local oscillator
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @thorw ReceiversErrors::ReceiversErrorsEx
	 */
	void setLO(const ACS::doubleSeq& lo) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
	
	/**
	 * This method will be used to configure a receiver given its mnemonic code
	 * @param code string that contains the code of the receiver to be configured
	 * @throw CORBA::SystemException
	 * @throw ManagementErrors::ConfigurationErrorEx
	 */
	void setup(const char * code) throw (CORBA::SystemException,ManagementErrors::ConfigurationErrorEx);
	
	/**
	 * This method allows to set the operating mode of the currently configured receiver. If no receiver has been configured or the mode is not valid for the current receiver,
	 * an error is issued.
	 * @param mode string identifer of the operating mode
	 * @throw CORBA::SystemException
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 */
	void setMode(const char * mode) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
	
	/**
	 * This method puts the subsystem in idle. At the moment it does nothing that resetting the receiver code.
	 */
	void park() throw (CORBA::SystemException,ManagementErrors::ParkingErrorEx);
	
	/**
	 * This method is called when the values of the calibration mark of the current receiver are required. A value is returned for every sub bands.
	 * The subbands are defined by giving the feed number, the polarization the initial frequency and the bandwidth.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 * @param freqs for each subband this is the list of the starting frequencies (in MHz). Correlated to the real initial frequency of the current receiver.
	 * @param bandwidths for each subband this is the width in MHz. Correlated to the real bandwidth of the current receiver.
	 * @param feeds for each subband this if the feed number
	 * @param ifs for each subband this indicates the proper IF, given the feed
	 * @param skyFreq for each subband this indicates the corresponding frequency observed (MHz).
	 * @param skyBw for each subband this indicates the resulting bandwidth (MHz)
	 * @param slaceFactor scaling value to be used when computing system temperature
	 * @return the list of the noise calibration value in Kelvin degrees.
	 */
    virtual ACS::doubleSeq * getCalibrationMark (const ACS::doubleSeq& freqs, const ACS::doubleSeq& bandwidths, const ACS::longSeq& feeds, const ACS::longSeq& ifs,
    		ACS::doubleSeq_out skyFreq,ACS::doubleSeq_out skyBw,CORBA::Double_out scaleFactor) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

    /**
     * this method is  called to get the all the receiver output information in one call.  An output is identified by providing the feed and the IF identifier. It can process any number of requests at a time.
     * @param feeds is a list that stores the corresponding feed of the output we are asking for
     * @param ifs is a list that identifies which IFs of the feed we are interested in, usually 0..<i>IFs</i>-1
     * @param freq used to return the start frequency of the band provided by the output  the oscillator (if present) is not  added (MHz)
     * @param bw used to return the total provided bandwidth. (MHz)
     * @param pols it specifies the polarization of the receiver output, since ACs does not support for enum sequences the correct value must be matched against the <i>Receivers::TPolarization</i> enumeration.
     * @param LO it gives (if present) the value of the local oscillator (MHz).
     */
    virtual void getIFOutput (const ACS::longSeq & feeds, const ACS::longSeq & ifs,ACS::doubleSeq_out freqs,ACS::doubleSeq_out bw,ACS::longSeq_out pols,ACS::doubleSeq_out LO) throw (
    		CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * This method is called in order to know the geometry of the currently configured receiver. The geometry is given along the X and Y axis where the central feed is the origin
	 * the axis. The relative power (normalized to one) with respect to the central feed is also given.    
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @thorw ReceiversErrors::ReceiversErrorsEx
	 * @param X the positions relative to the central beam of the feeds along the X axis (radians)
	 * @param Y the positions relative to the central beam of the feeds along the Y axis (radians) 
	 * @param power the relative power of the feeds
	 * @return the number of feeds
	 */    
    virtual CORBA::Long getFeeds(ACS::doubleSeq_out X,ACS::doubleSeq_out Y,ACS::doubleSeq_out power) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
    
	/**
	 * This method is called in order to know the taper of the current receiver. 
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 * @param freq starting frequency of the detector in MHz. Correlated to the real initial frequency of the current receiver.
	 * @param bandWidth bandwidth of the detector n MHz. Correlated to the real bandwidth of the current receiver.
	 * @param feed feed id the detector is attached to
	 * @param ifNumber Number of the IF, given the feed
	 * @param waveLen corresponding wave length in meters
	 * @return the value of the taper in db
	 */        
    virtual CORBA::Double getTaper(CORBA::Double freq,CORBA::Double bandWidth,CORBA::Long feed,CORBA::Long ifNumber,CORBA::Double_out waveLen) throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
    
	/**
	 * This method implements the command line interpreter. The interpreter allows to ask for services or to issue commands
	 * to the sub-system by human readable command lines.
	 * @param cmd string that contains the command line
	 * @param answer string containing the answer to the command
	 * @return the result of the command : true if successful
	 * @throw CORBA::SystemException
	 */
    virtual CORBA::Boolean command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException);
	//virtual char *command(const char *cmd) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx);

	/**
	 * It allows to turn the antenna unit on
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	virtual void turnAntennaUnitOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 *  It allows to turn the antenna unit off
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	virtual void turnAntennaUnitOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * called in order to get back the position of the derotator at a given epoch
	 * @param reference epoch
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	virtual CORBA::Double getDerotatorPosition (ACS::Time epoch) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * This method is a wrap a call to the derotator setup, allowing to activate the derotation. If the derotator is not supported by the current receiver an error is risen.
	 * @param mode specified the tracking mode of the derotator, if <i>RCV_UNDEF_UPDATE</i> is given the present value is kept
	 * @param rewind specifies the rewind mode in case the derotator reaches its final limit, if i<i>RCV_UNDEF_REWIND</i> is given, the present value is kept
	 * @param feeds number of feeds to derotate in case automatic rewind mode is slected, if -1 the present value if not changed
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
    virtual void derotatorSetup (Receivers::TUpdateModes mode,Receivers::TRewindModes rewind,CORBA::Long feeds) throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
    
    /**
     * It disable the derotator. It is implicitly called also by the <i>park()</i> method.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void derotatorPark () throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * Returns a reference to the LO property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property
	*/	 
	virtual ACS::ROdoubleSeq_ptr LO() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the receiverCode property implementation of the IDL interface.
	 * @return pointer to read-only string  property
	*/	 
	virtual ACS::ROstring_ptr actualSetup() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the feeds property implementation of the IDL interface.
	 * @return pointer to read-only long property
	*/	 
	virtual ACS::ROlong_ptr feeds() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the IFs property implementation of the IDL interface.
	 * @return pointer to read-only long property
	*/	 	
	virtual ACS::ROlong_ptr IFs() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the initialFrequency property implementation of the IDL interface.
	 * @return pointer to read-only doubleSeq property
	*/	 	
	virtual ACS::ROdoubleSeq_ptr initialFrequency() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the bandWidth property implementation of the IDL interface.
	 * @return pointer to read-only doubleSeq property
	*/	 	
	virtual ACS::ROdoubleSeq_ptr bandWidth() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the polarization property implementation of the IDL interface.
	 * @return pointer to read-only long sequence property
	*/	 
	virtual ACS::ROlongSeq_ptr polarization() throw (CORBA::SystemException);	
	
	/**
     * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only ROTSystemStatus property status
	*/
	virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the mode property implementation of the IDL interface.
	 * @return pointer to read-only string property
	*/
    virtual ACS::ROstring_ptr mode() throw (CORBA::SystemException);

	
private:
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_plocalOscillator;
	baci::SmartPropertyPointer<baci::ROstring> m_pactualSetup;
	baci::SmartPropertyPointer<baci::ROlong> m_pfeeds;
	baci::SmartPropertyPointer<baci::ROlong> m_pIFs;
	baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pinitialFrequency;
	baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pbandWidth;
	baci::SmartPropertyPointer<baci::ROlongSeq> m_ppolarization;
	baci::SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),
	  POA_Management::ROTSystemStatus> > m_pstatus;
	baci::SmartPropertyPointer<baci::ROstring> m_pmode;
	
	SimpleParser::CParser<CRecvBossCore> *m_parser;
	CRecvBossCore *m_core;
	CConfiguration m_config;
	CWatchingThread *m_watcher;
};



#endif /*RECEIVERSBOSSIMPL_H_*/
