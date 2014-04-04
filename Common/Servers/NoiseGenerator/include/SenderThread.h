#ifndef _SENDERTHREAD_H_
#define _SENDERTHREAD_H_
/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: SenderThread.h,v 1.1 2011-03-14 15:16:22 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  			  11/06/2010      Creation                                       */

#include <acsThread.h>
#include <IRA>

/**
 * This class implements a working thread. This thread is in charge of generating and sending the data through the ACS bulk data channel. 
*/
class CSenderThread : public ACS::Thread
{
public:
	struct TSenderParameter {
		NoiseGeneratorImpl *sender;
		CCommandLine *command;
		CConfiguration *configuration;
	};
	struct 	TTransferParameter {
		long sections;          			 /*!< number of channels involved */
		long integration;       			/*<! integration time in milli seconds, it applies to all channels */
		long id[MAX_SECTION_NUMBER];       /*!< channel identifier, generally 0..channels-1  */  
		long sampleSize;                  /*!< sample size in bytes */
	};

	/**
     * Constructor().
     * @param name thread name
     * @param par thread parameter 
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CSenderThread(const ACE_CString& name,TSenderParameter  *par, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CSenderThread();

     /**
     * This method is executed once when the thread starts.
    */
    virtual void onStart();

    /**
     * This method is executed once when the thread stops.
     */
     virtual void onStop();

     /**
      * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive.
      * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
     */
     virtual void runLoop();
     
     /**
      * This method is used to let the thread know the configuration of the backend in order to perform the proper operations over
      * the incoming data.
      * @param mH main header of the data
      * @param mC section header, one header for each enabled section)
      */ 
     void saveDataHeader(Backends::TMainHeader* mH,Backends::TSectionHeader* cH);
     
     /**
      * This method is used to inform the theread about the requested start time.
      * @param time current start time.
      */
     void saveStartTime(const ACS::Time& time);
         
     /**
      * This method is used to inform the thread that should not send data anymore
      * @param stop true the thread will not send data any more
      */
    // void setStop(bool stop) { m_stop=stop; }
     
     void resumeTransfer() { m_go=true; }
     
     void suspendTransfer() { m_stop=true; }
     
     /**
      * It set up the Kelvin Counts ratio used to compute the system temperature for each channel.
      * @param ratio the list of the conversion factor, one for each channel
      */
     void setKelvinCountsRatio(const ACS::doubleSeq& ratio);

private:
	NoiseGeneratorImpl *m_sender;
	CCommandLine *m_commandLine;
	CConfiguration *m_configuration;
	
	TIMEVALUE m_startTime; 
	
	Backends::TMainHeader m_dataHeader;
	Backends::TSectionHeader m_sectionsHeader[MAX_SECTION_NUMBER];

	bool m_stop;
	bool m_go;
	bool m_sending;
	/* It stores the conversion factor  from counts to Kelvin for each of the inputs */ 
	double m_KCountsRatio[MAX_INPUT_NUMBER];
	
	void generateBuffer(double *& tsys,SAMPLETYPE *&data,long& tsysSize,long& dataSize);
	
};

#endif /*_SENDERTHREAD_H_*/
