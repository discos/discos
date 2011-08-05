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
	 */
	virtual void execute() throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);

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
	 */
	void setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CouldntGetComponentExImpl,
			ComponentErrors::CORBAProblemExImpl);

	/**
	 * It returns back the current local oscillator frequency settings.
	 * @param lo output sequence
	 */
	void getLO(ACS::doubleSeq& lo);

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
	double linearFit(double *X,double *Y,const WORD& size,double x);
};


#endif
