#ifndef _SENDERTHREAD_H_
#define _SENDERTHREAD_H_
/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: SenderThread.h,v 1.1 2011-03-14 14:15:07 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  21/11/2008      Creation                                                  */

#include <acsThread.h>
#include <IRA>

/**
 * This class implements a working thread. This thread is in charge of reading the data from the backend, doing a pre processing and then
 * publishing them into the ACS bulk data channel. 
*/
class CSenderThread : public ACS::Thread
{
public:
	struct TSenderParameter {
		TotalPowerImpl *sender;
		IRA::CSecureArea<CCommandLine> *command;
		CConfiguration *configuration;
		IRA::CSocket *dataLine;
	};
	struct 	TTransferParameter {
		long channels;           /*!< number of channels involved */
		long integration;       /*<! integration time in milli seconds, it applies to all channels */
		long id[MAX_SECTION_NUMBER];                                 /*!< channel identifier, generally 0..sections-1  */
		double sampleRate;              /*!< sample rate in MHz */
		long sampleSize;                   /*!< sample size in bytes */
		double normalization;         /*!<normalization factor, depends on integration and sample rate */
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
      * This method is used to let the thread know the configuration of teh backend in order to perform the proper operations over
      * the coming data.
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
      * This method is used to inform the thread about the values of each input when the attenuator are all inserted.
      * This will allows to subtract the zero scale to the data.
      */
     void saveZero(DWORD *zeros);
     
     /**
      * This method is used to inform the thread that should not send data anymore
      * @param stop true the thread will not send data any more
      */
    // void setStop(bool stop) { m_stop=stop; }
     
     void resumeTransfer() { m_go=true; }
     
     /** 
      * suspend the data transfer and waits until it is suspended.
      */
     void suspendTransfer() { m_stop=true; while (m_stop) { IRA::CIRATools::Wait(100000); } }
     
     /**
      * It set up the Kelvin Counts ratio used to compute the system temperature for each channel.
      * @param ratio the list of the conversion factor, one for each channel
      */
     //void setKelvinCountsRatio(const ACS::doubleSeq& ratio);
     
 	/**
 	 * Force the socket to be closed and to reinitialize everything. Sending data is also stopped
 	 */
 	void closeSocket();

private:
	struct TSampleRecord {
		TIMEVALUE start;
		bool started;
		DWORD counts;
		DWORD accumulations;
		SAMPLETYPE tpi[MAX_SECTION_NUMBER];
	};
	struct TDumpRecord {
		Backends::TDumpHeader hd;
		SAMPLETYPE sample[MAX_SECTION_NUMBER];
	};
	IRA::CSocket *m_dataLine;
	TotalPowerImpl *m_sender;
	IRA::CSecureArea<CCommandLine> *m_commandLine;
	CConfiguration *m_configuration;
	bool m_isConnected;
	IRA::CSocket m_backendSock;
	BYTE *m_inputBuffer;
	BYTE *m_tempBuffer;
	DWORD m_tempBufferPointer;
	DWORD *m_zeroBuffer;
	TTransferParameter m_dataHeader;
	WORD m_previousStatus;
	WORD m_previousCounter;
	TIMEVALUE m_startTime;
	//bool m_immediateStart;
	TSampleRecord m_tpi; //data regarding the normal data
	TSampleRecord m_cal; // data regarding the calibration diode on data
	TIMEVALUE m_lastReceiveEpoch;
	bool m_stop;
	bool m_go;
	bool m_sending;
	/* It stores the conversion factor  from counts to Kelvin for each of the inputs */ 
	ACS::doubleSeq m_KCountsRatio;
	/**
	 * This method is called by the thread in order to process and ship the data
	 */
	void processData(DWORD dataSize,const long& sectionNumber,long *boards);
	
	/** 
	 * it clears of the temporary data related to one integration
	 */
	void clearIntegration(TSampleRecord& samp);
	
	/**
	 * This function will set up thread data structure in order to start data receiving from the backend
	 */
	void initTransfer();
	
	/**
	 * Performs computation over each sample before transfering it toword the network
	 */
	void computeSample(TSampleRecord& samp,TIMEVALUE& sampleTime,DWORD *data,bool isCal);
		
};

#endif /*_SENDERTHREAD_H_*/
