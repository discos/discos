#ifndef  _TOTALPOWERIMPL_H_
#define _TOTALPOWERIMPL_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: TotalPowerImpl.h,v 1.1 2011-03-14 14:15:07 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                when            What                                                       */
/* Andrea Orlati(a.orlati@ira.inaf.it)  11/09/2008      Creation                                                  */
/* Andrea Orlati(a.orlati@ira.inaf.it)  1101/2013       changed command method according new CommandInterpreter interface                  */


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
#include <TotalPowerS.h>
#include <ComponentErrors.h>
#include <BackendsErrors.h>
#include <SP_parser.h>
#include <ManagementErrors.h>
#include "CommandLine.h"
#include "ControlThread.h"


namespace SimpleParser {
class PolarizationToString {
public:
	char *valToStr(const long& val) {
		char *c=new char[16];
		if (val==Backends::BKND_LCP) {
			strcpy(c,"LEFT");
		}
		else if (val==Backends::BKND_RCP) {
			strcpy(c,"RIGHT");
		}
		else {
			strcpy(c,"FULL");
		}
		return c;
	}
	
	long strToVal(const char* str) {
		IRA::CString strVal(str);
		strVal.MakeUpper();
		if (strVal=="LEFT") {
			return (long)Backends::BKND_LCP;
		}
		else if (strVal=="RIGHT") {
			return (long)Backends::BKND_RCP;
		}
		else if (strVal=="FULL") {
			return (long)Backends::BKND_FULL_STOKES;
		}
		else {
			return strVal.ToLong();
		}
	}
};

};

using namespace ACSBulkDataError;
using namespace baci;

/** 
 * @mainpage Medicina total power backend component documentation
 * @date 12/02/2013
 * @version 1.61.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 8.0.2
 * @remarks compiler version is 3.4.6 
*/

class CSenderThread;

/**
 * This class implements the Backends::TotalPower CORBA interface.  
 * All exceptions that comes from the run-time interation with clients are at logged with LM_DEBUG priority.
 * @todo an implementation of <i>configure()</i> must be done yet.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * @todo I could be necessary to plan a stopTime in sendStop in order to be more precise on stop time. The backend hardware never
 *            stops to send data and the data flux control (toward bulk data receviver) handled by the component itself.
 * <br> 
 */
class TotalPowerImpl: public virtual BulkDataSenderDefaultImpl,
				   										public virtual POA_Backends::TotalPower
{
public: 
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	TotalPowerImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);
	
	/**
	 * Destructor.
	*/
	virtual ~TotalPowerImpl(); 
	
	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);
	
	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls any time. 
	 * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	
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
	virtual void sendHeader() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, ComponentErrors::ComponentErrorsEx);
	
	/**
	 * Starts effectively to send the bulk of data to the receiver. The invocation of this method is forced to be far enough to the invocation
	 * of <i>sendHeader</i> (in order to respect the backend data channel latency). In order for this call to succeed, the component must be
	 * in <i>CCommandLine::TstatusFields::SUSPEND</i> mode.  
	 * @throw CORBA::SystemException
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 	   @arg \c ComponentErrors::UnexpectedEx
	 * @param startTime represent the exact time that the acquisition should start. Of course this has effect only if the call
	 * to that method arrives reasonably before the given time.
	 */
	virtual void sendData(ACS::Time startTiime) throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
			ComponentErrors::ComponentErrorsEx);
	
	/**
	 *  It suspend the data transfer. If the backend must not be in suspend mode.
	 * @throw CORBA::SystemException
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 	   @arg \c ComponentErrors::UnexpectedEx
	 */
	virtual void sendStop() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
			ComponentErrors::ComponentErrorsEx);
	
	/** 
	 * This immediately terminates the transfer job (previously begun by the call to <i>sendHeader()</i>).
	 * @throw CORBA::SystemException
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 */
	virtual void terminate() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
			ComponentErrors::ComponentErrorsEx);
	
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
	 * @param feed identifier of the feed
	 * @param pol this value ask the input to send data for one polarization or for all Stokes parameters. Possible values are
	 *               BKND_LCP, BKND_RCP or BKND_FULL_STOKE or a negative to keep the current value. In this implementation the component expects only a negative
	 *              because is not possble to change this parameter. On the contrary a warning log message is posted.
	 * @param sr new sample rate value (Mhz). In this implementation represents the frequency of the sampling of the total power
	 *                A negative keeps the previous value unchanged. Since the backend does not support different sample rates,
	 *               the current one is used also for all other inputs.
	 * @param bins number of bins produced for each input. This value is significant for spectrometers. For that implementation
	 *               this value is ignored since it must be always 1. If this is not the case a warning messagge is logged.
	 */
    virtual void setSection(CORBA::Long input,CORBA::Double freq,CORBA::Double bw,CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins) throw (CORBA::SystemException,
    				ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

	/**
	 * @deprecated This method will configure all the  input channels of the backend in one shot.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw BackendsErrors::BackendsErrorsEx
	 * @param freq new freqency value for the input. This value represents the start frequency of the input filter(Mhz). In this 
	 * 				  implementation it is ignored, so a negative should be given.
	 * @param bw new bandWidth value in MHz. if a value of the sequence is negative the bandwidth of the corresponding 
	 *                input is unchanged. If the value is not legal an exception is thrown.
	 * @param feed identifier of the feed
	 * @param pol this value ask the input to send data for one polarization or for all Stokes parameters. Possible values are
	 *               BKND_LCP, BKND_RCP or BKND_FULL_STOKE. In this implementation this parameter is ignored because this backend is a
	 *               simple total power.
	 * @param sr new sample rate value (Mhz). In this implementation represents the frequency of the sampling of the total power
	 *                A negative keeps the previous value unchanged. Since the backend does not support different sample rates,
	 *               the current one is used also for all other inputs.
	 * @param bins number of bins produced for each input. This value is significant for spectrometers. For that implementation
	 *               this value is ignored since it must be always 1. 
	 */
    //virtual void setAllSections(CORBA::Double freq,CORBA::Double bw,CORBA::Long feed,Backends::TPolarization pol,CORBA::Double sr,CORBA::Long bins) throw (CORBA::SystemException,
    //				ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
   
    /**
     * This method enables or disables the data streaming of the inputs. if a input is disable the input is not sampled and the
     * data are not sent to the network.
     * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw BackendsErrors::BackendsErrorsEx
     * @param enable this must be a sequence of exactly <i>inputsNumber</i> elements. The inputs are enabled (default) if
     *                the corresponding value is positive. A negative value disables the input, whilst a zero keep the current configuration.
     */
    virtual void enableChannels(const ACS::longSeq& enable) throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
        
	/**
	 * Sets the current integration time.
     * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @thorw BackendsErrors::BackendsErrorsEx
	 * @param integration new integration value in milliseconds. A zero disables this feature, whilst a negative has not effect.
	 */
    virtual void setIntegration(CORBA::Long Integration) throw (CORBA::SystemException, 
    		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

	/**
	 * This method allows the client to interface the component by sending text commands. The command is parsed and executed according the
	 * defined command syntax and grammar. This method is required to implement the <i>Management::CommandInterpreter</i> interface.
     * @throw CORBA::SystemException
	 * @param cmd string contains the string that will be executed
	 * @param answer  the string that reports the command execution results or in case, errors messages
	 * @return true if the command was executed correctly
	 */
    virtual CORBA::Boolean command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException);
    
    /**
     * Call this function to set the current time (from the local computer) into the backend. 
     * @thorw CORBA::SystemException
     * 	@throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
	 *       @arg \c ComponentErrors::Timeout
	 *       @arg \c BackendsErrors::Connection
	 *       @arg \c ComponentErrors::SocketError
     */
    virtual void setTime() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
    		BackendsErrors::BackendsErrorsEx);
    
    /**
     * This function will control the calibration diode switching.
     * @thorw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     * @param argument the parameter controls how the backend reacts to the command. The backend could be configured so that the switching is
     * 		   performed by the backend itself. in that case th argument is an integer value that is considered to be the interleave argument.
 	 * 		   Interleave controls the switching frequency of the calibration diode. If it is zero (default) the diode is not
	 *         switched (always turned off). if a one is given the backend will produce one sample (using the <i>sampleRate</i>)
	 *         with the mark switched off and one with the mark switched on. A two means two sample with the mark switched off and
	 * 	       one with the mark switched on.....and so on. A negative will not change the value of the parameter.
	 * 	       The other legal values are "EXT" meaning the switching is enabled but the trigger is issued by external sources and "OFF" meaning
	 * 	       the switching is turned off.
	*/
    virtual void activateNoiseCalibrationSwitching(const char * argument) throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
    /**
     * Methods used to activate and deactivate the noise calibration diode. This is intended to be used
     * for non periodic switching where a complete acquisition is performed keeping the noise mark active.
     */
    virtual void calOn() throw (CORBA::SystemException, 
                                ComponentErrors::ComponentErrorsEx,
                                BackendsErrors::BackendsErrorsEx);
    virtual void calOff() throw (CORBA::SystemException, 
                                ComponentErrors::ComponentErrorsEx,
                                BackendsErrors::BackendsErrorsEx);
    
    /**
     * This function will initialized the parameters that must be fixed before any other operation can be carried on. Some possible configuration
     * are available according the given receiver connected to the backend.
     * @thorw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     * @param configuration identifier of the required initialization
     */
    virtual void initialize(const char * configuration) throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
    
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
    virtual ACS::doubleSeq * getTpi () throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
    virtual ACS::doubleSeq * getRms () throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx) {return 0;};

    /**
     * Call this function in order to get a total power measure for each input channel. The measure is done when the inputs are directly
     * connected to the 50 Ohm.
     * @thorw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     * @param integration it represents the integration time (milliseconds) the backend will use to do  the measure. 
     *                If zero the default value of 1 second is used. A negative will forse the backend to adopt the value set by a call
     *                 to (<i>setIntegration()</i>).
     * @return a sequence of double values that reports for each backend the total power measured during the integration time.
     * 				  The caller must take care of freeing it.
     */
    virtual ACS::doubleSeq * getZero (/*CORBA::Long integration*/) throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
   
    /**
     * This method is used to inform the component about the proper conversion factor between Kelvin and counts. This value is used for the tsys computation during
     * the sampling. If this method is never issued the default value of 1 will be used for all the channels.
     * @throw CORBA::SystemException
     * @param ratio this is the sequence of required values one for each channel.   
     */
    virtual void setKelvinCountsRatio(const ACS::doubleSeq& ratio,const ACS::doubleSeq& tsys) throw (CORBA::SystemException);
    
    /**
     * This method is used to tune the attenuation level of each of the virtual input of the system.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     * @param input is the identifier number of the input that we want to configure.
     * @param att new attenuation level, It could be in the range 0-15. A negative will keep the previous value.  
     */
    virtual void setAttenuation(CORBA::Long input,CORBA::Double att) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);
    
    /**
     * In order  to get the inputs definition in just one call. The returned argument contains the configuration sequentially, the first element corresponds to the first input and so on.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx  
     * @param freq given in MHz is the initial frequency of input band
     * @param bandwidth given in MHz is the band width of the input
     * @param feed dives the feed number the input belongs to
     * @param ifNumber given the feed it select the feed channel to which the input is connected to. Plaese notice this parameter has nothing to do with the polarization of the section.
     * @return the number of inputs, in other words is the lenght of the returned sequences
     */
    virtual CORBA::Long getInputs(ACS::doubleSeq_out freq,ACS::doubleSeq_out bandWidth,ACS::longSeq_out feed,ACS::longSeq_out ifNumber) throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

    /**
     * It enables the control of the fast switching of the noise mark from an external source.
     * @param on the integer values turns on (1) and off (0) the external source
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw BackendsErrors::BackendsErrorsEx
     */
    /*virtual void externalCalibrationSwitching(CORBA::Long on) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
    		BackendsErrors::BackendsErrorsEx);*/

    /**
     * This method is just a place holder for this implementation of the genericBackend interface
     */
    virtual void setTargetFileName (const char * fileName) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx);

	/** 
	 * Returns a reference to the time  property Implementation of IDL interface.
	 * @return pointer to read-only unsigned long long  property time
	*/
    virtual ACS::ROuLongLong_ptr time() throw (CORBA::SystemException);
        
	/** 
	 * Returns a reference to the backendName property Implementation of IDL interface.
	 * @return pointer to read-only string property backendName
	*/
	virtual ACS::ROstring_ptr backendName() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the bandWidth property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property bandWidth
	*/	 
	virtual ACS::ROdoubleSeq_ptr bandWidth() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the frequency property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property frequency
	*/	 
	virtual ACS::ROdoubleSeq_ptr frequency() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the sampleRate property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property sampleRate
	*/	 
	virtual ACS::ROdoubleSeq_ptr sampleRate() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the attenuation property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property attenuation
	*/	 
	virtual ACS::ROdoubleSeq_ptr attenuation() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the polarization property implementation of the IDL interface.
	 * @return pointer to read-only long sequence property polarization
	*/	 
	virtual ACS::ROlongSeq_ptr polarization() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the bins property Implementation of IDL interface.
	 * @return pointer to read-only long property binsNumber
	*/
	virtual ACS::ROlongSeq_ptr bins() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the feed property Implementation of IDL interface.
	 * @return pointer to read-only long sequence property feed
	*/
	virtual ACS::ROlongSeq_ptr feed() throw (CORBA::SystemException);	

	/** 
	 * Returns a reference to the inputsNumber  property Implementation of IDL interface.
	 * @return pointer to read-only long property inputsNumber
	*/
	virtual ACS::ROlong_ptr inputsNumber() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the systemTemperature property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property systemTemperature
	*/	 
	virtual ACS::ROdoubleSeq_ptr systemTemperature() throw (CORBA::SystemException);
		
	/** 
	 * Returns a reference to the integration  property Implementation of IDL interface.
	 * @return pointer to read-only long property integration
	*/
	virtual ACS::ROlong_ptr integration() throw (CORBA::SystemException);

	/** 
	 * Returns a reference to the sectionsNumber  property Implementation of IDL interface.
	 * @return pointer to read-only long property sectionsNumber
	*/
	virtual ACS::ROlong_ptr sectionsNumber() throw (CORBA::SystemException);	
	
	/** 
	 * Returns a reference to the inputSection  property Implementation of IDL interface.
	 * @return pointer to read-only long property inputSection 
	*/
	virtual ACS::ROlongSeq_ptr inputSection() throw (CORBA::SystemException);	

	
	/** 
	 * Returns a reference to the status implementation of IDL interface.
	 * @return pointer to read-only pattern property status
	*/	
	virtual ACS::ROpattern_ptr status() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the busy implementation of IDL interface.
	 * @return pointer to read-only long property busy
	*/	
	virtual Management::ROTBoolean_ptr busy() throw (CORBA::SystemException);
	
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
	SmartPropertyPointer<ROlong> m_pintegration;
	SmartPropertyPointer<ROpattern> m_pstatus;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),
		  POA_Management::ROTBoolean>  > m_pbusy;
	SmartPropertyPointer<ROlongSeq> m_pfeed;
	SmartPropertyPointer<ROdoubleSeq> m_ptsys;
	SmartPropertyPointer<ROlong> m_psectionsNumber;
	SmartPropertyPointer<ROlongSeq> m_pinputSection;
	/** This is the socket that allows to send requests to the backends throught its command line*/
	CSecureArea<CCommandLine> *m_commandLine;
	CConfiguration m_configuration;
	CSenderThread *m_senderThread;
	CControlThread *m_controlThread;
	IRA::CSocket m_dataLine;
	bool m_initialized;
	SimpleParser::CParser<CCommandLine> * m_parser;
	
	void deleteAll();
};

#endif /*TotalPowerImpl_H_*/
