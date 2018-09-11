#ifndef  _XBACKENDS_H_
#define _XBACKENDS_H_

/* ************************************************************************************************************* */
/* 						                                            				                            */
/* $Id: XBackendsImpl.h																					         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                               				 when            What                                                       */
/* Liliana Branciforti(bliliana@arcetri.astro.it)  30/07/2009     Creation                                       */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <enumpropROImpl.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <baciROstring.h>
#include <baciROuLongLong.h>
#include <baciROdoubleSeq.h>
#include <baciROlongSeq.h>
#include <baciROpattern.h>
#include <bulkDataSenderImpl.h>
#include <XBackendsS.h>
#include <ComponentErrors.h>
#include <BackendsErrors.h>
#include <ManagementErrors.h>
#include <SP_parser.h>
#include "CommandLine.h"
#include "XarcosThread.h"
#include "Configuration.h"
#include "GroupSpectrometer.h"

using namespace baci;
using namespace IRA;
using namespace ACSBulkDataError;

/**
 * This class implements the Backends::XBackends CORBA interface.  
 * All exceptions that comes from the run-time interation with clients are at logged with LM_DEBUG priority.
 * @todo an implementation of <i>configure()</i> must be done yet.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * @todo The time synchronization maybe should be checked on regular base. The solution could be to create a monitoring thread.
 * @todo I could be necessary to plan a stopTime in sendStop in order to be more precise on stop time. The backend hardware never
 *            stops to send data and the data flux control (toward bulk data receviver) handled by the component itself.
 * <br> 
 */
class XBackendsImpl: public virtual BulkDataSenderDefaultImpl,
				   	 virtual POA_Backends::XBackends
{
public: 
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	XBackendsImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);
	
	/**
	 * Destructor.
	*/
	virtual ~XBackendsImpl(); 
	
	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void initialize() 
		throw (ACSErr::ACSbaseExImpl);
	
	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls any time. 
	 * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void execute() 
		throw (ACSErr::ACSbaseExImpl);
	
	/** 
	 * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
	*/
	virtual void cleanUp();
	
	/** 
	 * Called by the container in case of error or emergency situation. This function tries to free all resources even though there is no
	 * warranty that the function is completely executed before the component is destroyed.
	*/	
	virtual void aboutToAbort();
	
	/**
	 * It is used to start the data transfer (e.g. send parameters to the receiver)  
	 * @throw CORBA::SystemException
	 * @throw BackendsErrors::BackendsErrorsEx
	 * 		@arg \c BackendsErrors::TXErrorEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 	   @arg \c ComponentErrors::UnexpectedEx
	*/
	virtual void sendHeader() 
		throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, ComponentErrors::ComponentErrorsEx);
	
	/**
	 * Starts effectively to send the bulk of data to the receiver. The invocation of this method is forced to be far enough to the invocation
	 * of <i>sendHeader</i> (in order to respect the backend data channel latency). In order for this call to succeed, the component must be
	 * in <i>CCommandLine::TstatusFields::SUSPEND</i> mode.  
	 * @throw CORBA::SystemException
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 	   @arg \c ComponentErrors::UnexpectedEx
	 * @param startTime represent the exact time that the acquisition should start. Of caourse this has effect only if the call
	 * to that method arrives reasonably before the given time.
	 */
	virtual void sendData(ACS::Time startTiime) 
		throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,ComponentErrors::ComponentErrorsEx);
	
	/**
	 *  It suspend the data transfer. If the backend must not be in suspend mode.
	 * @throw CORBA::SystemException
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 	   @arg \c ComponentErrors::UnexpectedEx
	 */
	virtual void sendStop() 
		throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,ComponentErrors::ComponentErrorsEx);
	
	/** 
	 * This immediately terminates the transfer job (previously begun by the call to <i>sendHeader()</i>).
	 * @throw CORBA::SystemException
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 */
	virtual void terminate() 
		throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,ComponentErrors::ComponentErrorsEx);
   
	/**
     * Call this function in order to get a total power measure for each input channel.
     * @thorw CORBA::SystemException
     * 	@throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     * @param integration it represents the integration time (milliseconds) the backend will use to do  the measure. 
     *                If zero the default value of 1 second is used. A negative will forse the backend to adopt the value set by a call
     *                 to (<i>setIntegration()</i>).
     * @return a sequence of double values that reports for each backend the total power measured during the integration time.
     * 			  The caller must take care of freeing it.
     */
    virtual ACS::doubleSeq * getTpi () 
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
    virtual ACS::doubleSeq * getRms () 
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx) {return 0;};
    /**
	     * This method is used to inform the component about the proper conversion factor between Kelvin and counts. This value is used for the tsys computation during
	     * the sampling. If this method is never issued the default value of 1 will be used for all the channels.
	     * @throw CORBA::SystemException
	     * @param ratio this is the sequence of required values one for each channel.   
	     */
    virtual void setKelvinCountsRatio(const ACS::doubleSeq& ratio,const ACS::doubleSeq& tsys) 
    	throw (CORBA::SystemException);
	
	/**
	 * This method will configure an input channel of the backend. The input is identified by a numeric identifier.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @param input identifier of the input to be configured. it must be in the correct range (from 0 to <i>inputsNumber</i>)
	 *               otherwise an exception is raisen.
	 * @param freq new freqency value for the input. This value represents the start frequency of the input filter(Mhz). In this 
	 * 				  implementation it is ignored, so a negative should be given.
	 * @param bw new bandWidth value in MHz. if a value of the sequence is negative the bandwidth of the corresponding 
	 *                input is unchanged. If the value is not legal an exception is thrown.
	 * @param sr new sample rate value (Mhz). In this implementation represents the frequency of the sampling of the total power
	 *                A negative keeps the previous value unchanged. Since the backend does not support different sample rates,
	 *               the current one is used also for all other inputs.
	 * @param att new attenuation level of the input (db). if out of legal renge an exception is thorwn. A negative value keep
	 *                the previuos configuration
	 * @param pol this value ask the input to send data for one polarization or for all Stokes parameters. Possible values are
	 *               BKND_LCP, BKND_RCP or BKND_FULL_STOKE. In this implementation this parameter is ignored this backend is a
	 *               total power.
	 * @param bins number of bins produced for each input. This value is significant for spectrometers. For that implementation
	 *               this value is ignored since it must be always 1. 
	 * 
	 * @param feed indicate the feeb  number the input is connected to. 
	 * 
	 */

	virtual void setSection (CORBA::Long input,CORBA::Double freq,CORBA::Double bw,
			CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins) 
		throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

	/**
	 * This method will configure all the  input channels of the backend in one shot.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @param freq new freqency value for the input. This value represents the start frequency of the input filter(Mhz). In this 
	 * 				  implementation it is ignored, so a negative should be given.
	 * @param bw new bandWidth value in MHz. In this case a negative value (keep present configuration) is not allowed. 
	 * @param sr new sample rate value (MHz). In this implementation represents the frequency of the sampling of the total power.
	 *                In this case a negative value (keep present configuration) is not allowed.
	 * @param att new attenuation level of the input (db). if out of legal range an exception is thorwn.
	 *                In this case a negative value (keep present configuration) is not allowed. 
	 * @param pol this value ask the input to send data for one polarization or for all Stokes parameters. Possible values are
	 *               BKND_LCP, BKND_RCP or BKND_FULL_STOKE. In this implementation this parameter is ignored this backend is a
	 *               total power.
	 * @param bins number of bins produced for each input. This value is significant for spectrometers. For that implementation
	 *               this value is ignored since it must be always 1. 
	 */
//    virtual void setAllSections (CORBA::Double freq,CORBA::Double bw,CORBA::Double sr,CORBA::Double att,
//    		CORBA::Long pol,CORBA::Long bins) throw (CORBA::SystemException,
//    				ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
   
    /**
     * This method enables or disables the data streaming of the inputs. 
     * if a input is disable the input is not sampled and the data are not sent to the 
     * network.
     * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw BackendsErrors::BackendsErrorsEx
     * @param enable this must be a sequence of exactly <i>inputsNumber</i> elements. 
     * The inputs are enabled (default) if the corresponding value is positive. 
     * A negative value disables the input, whilst a zero keep the current configuration.
     */
    virtual void enableChannels(const ACS::longSeq& enable) 
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
        
	/**
	 * Sets the current integration time.
     * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @thorw BackendsErrors::BackendsErrorsEx
	 * @param integration new integration value in milliseconds. A zero disables this feature, whilst a negative has not effect.
	 */
    virtual void setIntegration(CORBA::Long Integration) 
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
	/**
	 * Sets the current Number Section.
     * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @thorw BackendsErrors::BackendsErrorsEx
	 * @param inputsNumber. A zero disables this feature, whilst a negative has not effect.
	 */
     virtual void setInputsNumber(CORBA::Long inputsNumber) 
     	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);	
 	
     /**
 	 * Sets the current Number Section.
      * @throw CORBA::SystemException
 	 * @throw ComponentErrors::ComponentErrorsEx
 	 * @thorw BackendsErrors::BackendsErrorsEx
 	 * @param inputsNumber. A zero disables this feature, whilst a negative has not effect.
 	 */
     virtual void setSectionsNumber(CORBA::Long sectionsNumber) 
     	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

     /**
      * In order  to get the inputs definition in just one call. The returned argument contains the configuration sequentially, the first element corresponds to the first input and so on.
      * @throw CORBA::SystemException
      * @throw ComponentErrors::ComponentErrorsEx
      * @throw BackendsErrors::BackendsErrorsEx  
      * @param freq given in MHz is the initial frequency of input band
      * @param bandwidth given in MHz is the band width of the input
      * @param feed dives the feed number the input belongs to
      * @param polarization given the feed it select the polarization to which the input is connected to. Plaese notice this parameter has nothing to do with the polarization of the section.
      * @return the number of inputs, in other words is the lenght of the returned sequences
      */
     virtual CORBA::Long getInputs(ACS::doubleSeq_out freq,ACS::doubleSeq_out bandWidth,
    		 ACS::longSeq_out feed,ACS::longSeq_out ifNumber/*Receivers::TPolarizationSeq_out polarization*/) 
     	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

     /**
     * This method is just a place holder for this implementation of the genericBackend interface
     */
    virtual void setTargetFileName (const char * fileName) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
     
     /**
	 * This method allows the client to interface the component by sending text commands. The command is parsed and executed according the
	 * defined command syntax and grammar.
     * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx 
	 * @param configCommand this string contains the string that will be executed
	 * @return the string that reports the command execution results or in case, errors
	 */
    virtual CORBA::Boolean command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException);
	//virtual char * command(const char *configCommand) 
    	//throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx);    
    
    /**
     * Call this function to set the current time (from the local computer) into the backend. 
     * @thorw CORBA::SystemException
     * 	@throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
	 *       @arg \c ComponentErrors::Timeout
	 *       @arg \c BackendsErrors::Connection
	 *       @arg \c ComponentErrors::SocketError
     */
    virtual void setTime() 
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
 
    /**
     * This function will initialized the parameters that must be fixed before any other operation can be carried on. Some possible configuration
     * are available according the given receiver connected to the backend.
     * @thorw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     * @param configuration identifier of the required initialization
     */
    virtual void initialize(const char * configuration) 
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
    
   /**
     * @thorw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     * @return a sequence of double values that reports for each backend the total power measured during the integration time.
     * 				  The caller must take care of freeing it.
     */
    virtual ACS::doubleSeq * getZero () 
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

	virtual void setXarcosConf(Backends::TXArcosConf conf) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

	/** 
	 * Returns a reference to the time  property Implementation of IDL interface.
	 * @return pointer to read-only unsigned long long  property time
	*/
    virtual ACS::ROuLongLong_ptr time() 
    	throw (CORBA::SystemException);
        
	/** 
	 * Returns a reference to the backendName property Implementation of IDL interface.
	 * @return pointer to read-only string property backendName
	*/
	virtual ACS::ROstring_ptr backendName() 
		throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the bandWidth property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property bandWidth
	*/	 
	virtual ACS::ROdoubleSeq_ptr bandWidth() 
		throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the frequency property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property frequency
	*/	 
	virtual ACS::ROdoubleSeq_ptr frequency() 
		throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the sampleRate property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property sampleRate
	*/	 
	virtual ACS::ROdoubleSeq_ptr sampleRate() 
		throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the attenuation property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property attenuation
	*/	 
	virtual ACS::ROdoubleSeq_ptr attenuation() 
		throw (CORBA::SystemException);

	/**
	 * Returns a reference to the polarization property implementation of the IDL interface.
	 * @return pointer to read-only long sequence property polarization
	*/	 
	virtual ACS::ROlongSeq_ptr polarization() 
		throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the bins property Implementation of IDL interface.
	 * @return pointer to read-only long property binsNumber
	*/
	virtual ACS::ROlongSeq_ptr bins() 
		throw (CORBA::SystemException);

	/** 
	 * Returns a reference to the feed property Implementation of IDL interface.
	 * @return pointer to read-only long property feedNumber
	*/
	virtual ACS::ROlongSeq_ptr feed() 
		throw (CORBA::SystemException);

	/**
	 * Returns a reference to the systemTemperature property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property systemTemperature
	*/	 
	virtual ACS::ROdoubleSeq_ptr systemTemperature()
		throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the inputsNumber  property Implementation of IDL interface.
	 * @return pointer to read-only long property inputsNumber
	 * This attribute indicates how many inputs are virtually cabled to the backend.
	*/
	virtual ACS::ROlong_ptr inputsNumber() 
		throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the sectionsNumber  property Implementation of IDL interface.
	 * @return pointer to read-only long property sectionsNumber
	 * This attributes indicates how many sections are present in the backend
	*/
	virtual ACS::ROlong_ptr sectionsNumber() 
		throw (CORBA::SystemException);
	/** 
	 * Returns a reference to the integration  property Implementation of IDL interface.
	 * @return pointer to read-only long property integration
	*/
	virtual ACS::ROlong_ptr integration() 
		throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the status implementation of IDL interface.
	 * @return pointer to read-only pattern property status
	*/	
	virtual ACS::ROpattern_ptr status() 
		throw (CORBA::SystemException);
	/** 
	 * Returns a reference to the busy implementation of IDL interface.
	 * @return pointer to read-only long property busy
	*/	
	virtual Management::ROTBoolean_ptr busy() 
		throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the 8bit implementation of IDL interface.
	 * @return pointer to read-only long property 8bit
	*/	
	virtual Management::ROTBoolean_ptr mode8bit() 
		throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the inputSection  property Implementation of IDL interface.
	 * @return pointer to read-only long property inputSection 
	*/
	virtual ACS::ROlongSeq_ptr inputSection() 
		throw (CORBA::SystemException);
	
    /**
     * This method is used to tune the attenuation level of each of the virtual input of the system.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     * @param input is the identifier number of the input that we want to configure.
     * @param att new attenuation level, It could be in the range 0-15. A negative will keep the previous value.  
     */
    virtual void setAttenuation(CORBA::Long input,CORBA::Double att) 
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
    
    /**
     * This method is used to show the HW data setting.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     */
    virtual void visualData()
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
    
    /**
     * This method is used to show th SW data setting.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     */
    virtual void visualSpecific()
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
   
    /**
     * This method is used to show th SW data setting.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     */
    virtual void visualACS()
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
  
    /**
     * This method is used to show th SW data setting.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     */
    virtual void setMode8bit(CORBA::Boolean mode)
    	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

protected:
    virtual void startSend()  throw (CORBA::SystemException, AVStartSendErrorEx) { };
    virtual void paceData()  throw (CORBA::SystemException, AVPaceDataErrorEx)  { };
    virtual void stopSend()  throw (CORBA::SystemException, AVStopSendErrorEx) { };


private:
	SmartPropertyPointer<ROuLongLong> m_ptime;
	SmartPropertyPointer<ROstring> m_pbackendName;
	SmartPropertyPointer<ROdoubleSeq> m_pbandWidth;
	SmartPropertyPointer<ROdoubleSeq> m_pfrequency;
	SmartPropertyPointer<ROdoubleSeq> m_psampleRate;
	SmartPropertyPointer<ROdoubleSeq> m_pattenuation;
	SmartPropertyPointer<ROlongSeq> m_ppolarization;
	SmartPropertyPointer<ROlongSeq> m_pbins;
	SmartPropertyPointer<ROlong> m_pinputsNumber;
	SmartPropertyPointer<ROlong> m_psectionsNumber;
	SmartPropertyPointer<ROlongSeq> m_pinputSection;
	SmartPropertyPointer<ROlong> m_pintegration;
	SmartPropertyPointer<ROpattern> m_pstatus;
	SmartPropertyPointer<ROlongSeq> m_pfeed;
	SmartPropertyPointer<ROdoubleSeq> m_ptsys;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>  > m_pbusy;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>  > m_pmode8bit;
	/** This is the socket that allows to send requests to the backends throught its command line*/
	
	CSecureArea<CCommandLine> *m_commandLine;
	GroupSpectrometer *groupS;
	CCommandLine *commandL;
	CSecureArea<GroupSpectrometer> *m_GroupSpectrometer;
	bool m_initialized;
	ACE_CString m_fullName;
	CString IPAddress,hostName;
	DWORD Port;
 	XarcosThread * m_pcontrolLoop;
 	CConfiguration m_configuration;
 	SimpleParser::CParser<CCommandLine> * m_parser;
 	void deleteAll();
    DWORD Mode8Bit;
};

#endif /*XBackendsImpl_H_*/
