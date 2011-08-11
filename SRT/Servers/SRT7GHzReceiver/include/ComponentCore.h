#ifndef _COMPONENTCORE_H_
#define _COMPONENTCORE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public License (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/08/2011     Creation                                         */

#include "Configuration.h"
#include <ReceiverControl.h>
#include <LocalOscillatorInterfaceC.h>
#include <ReceiversErrors.h>

/**
 * This class contains the code of almost all the features  of the component
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
  */
class CComponentCore {
public:
	/**
	 * Constructor
	 */
	CComponentCore();

	/**
	 * Destructor
	 */
	virtual ~CComponentCore();

	/**
	 * This method initializes the object
	 * @param service pointer to container services object provided by the container
	 */
	virtual void initialize(maci::ContainerServices* services);

	/**
	 * This method prepares the object for execution.
	 * @return the pointer to the configuration class
	 */
	virtual CConfiguration const * const execute() throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);

	/**
	 * This function is responsible to free all allocated resources
	 */
	virtual void cleanup();

	/*
	 * It sets the local oscillator. Only the first value is considered in this case, since the receiver has just one synthesizer. Before commanding the new value some check are done. The the correspnding signal
	 * amplitude is computed.
	 * @param lo lists of values for the local oscillator (MHz), one for each IF. In that case just the first one is significant. In a -1 is passed the present value is kept
	 * @throw  ComponentErrors::ValidationErrorExImpl
	 * @throw ComponentErrors::ValueOutofRangeExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @thorw ReceiversErrors::LocalOscillatorErrorExImpl
	 */
	void setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CouldntGetComponentExImpl,
			ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl);

	/**
	 * It allows to change the operating mode of the receiver. If the mode does not correspond to a valid mode an error is thrown.
	 * @param  mode mode code as a string
	 */
	void setMode(const char * mode) throw  (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
			ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl);

	/**
	 * It activate the receiver, in other words it allows to setup the default configuration, and to check is the LNA are turned on.
	 */
	void activate() throw (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
			ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl);

	/**
	 * It allows to compute the value of the calibration mark for any given sub bands in the IF space.
	 * @param result this the sequence of computed mark values, first entry correspond to first sub band and so on....
	 * @param freqs  list of start frequencies (MHz)
	 * @param bandwidth list of the band widths (MHz)
	 * @param feeds list of feed identifier, it allows to specifies form which feed the sub band comes from. In that case it is neglected since the receiver is a single feed
	 * @param ifs list of IF identifier, it allows to specifies from which receiver IF the sub band comes from.
	 */
	void getCalibrationMark(ACS::doubleSeq& result,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,
			const ACS::longSeq& ifs) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl);

	/**
	 * It computes the taper given a reference band.
	 * @param freq start frequency of the reference band
	 * @param bw width of the reference band
	 * @param feed feed number
	 * @param ifNumber IF chain identifier
	 * @param waveLen wave length of the reference band, the band is transformed in a real sky observed band and the the central frequency is taken
	 */
	double getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (
			ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl);

	/**
	 * It returns the feed geometry of the receiver with respect to the central one. For this implementation it is just a placeholder since there is just one feed.
	 */
	long getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power);

	/**
	 * It returns back the current local oscillator frequency settings.
	 * @param lo output sequence
	 */
	void getLO(ACS::doubleSeq& lo);

	/**
	 * It returns back the current bandwidth for each IF.
	 * @param bw output sequence
	 */
	void getBandwidth(ACS::doubleSeq& bw);

	/**
	 * It returns back the current start frequency for each IF.
	 * @param sf output sequence
	 */
	void getStartFrequency(ACS::doubleSeq& sf);

	/**
	 * It returns back the current polarization for each IF.
	 * @param pol output sequence
	 */
	void getPolarization(ACS::longSeq& pol);

	/**
	 * It returns the current operating mode of the receiver.
	 * @return output string
	 */
	const IRA::CString& getSetupMode();

	/**
	 * It returns the number of IF chains
	 * @return output value
	 */
	const DWORD& getIFs();

	/**
	 * It returns the number of feeds
	 * @return output value
	 */
	const DWORD& getFeeds();

protected:
	/**
	 * Obtain a valid reference to the local oscillator device
	 */
	void loadLocalOscillator()  throw (ComponentErrors::CouldntGetComponentExImpl);

	/**
	 * used to free the reference to the local oscillator device
	 */
	void unloadLocalOscillator();
private:
	CConfiguration m_configuration;
	maci::ContainerServices* m_services;
	BACIMutex m_mutex;
	IRA::ReceiverControl *m_control;
	Receivers::LocalOscillator_var m_localOscillatorDevice;
	bool m_localOscillatorFault;
	double m_localOscillatorValue;
	ACS::doubleSeq m_startFreq;
	ACS::doubleSeq m_bandwidth;
	ACS::longSeq m_polarization;
	IRA::CString m_setupMode;
	double linearFit(double *X,double *Y,const WORD& size,double x);
};


#endif
