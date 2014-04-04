#ifndef XARCOSTHREAD_H
#define XARCOSTHREAD_H
/** **************************************************************************************************** */
/*																										*/
/*  $Id:XarcosThread.h																				*/
/*                                                                                                     */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                				                when            What                                              */
/* Liliana Branciforti(bliliana@arcetri.astro.it)  30/07/2009     Creation 								*/
#include <acsThread.h>
#include <IRA>
#include "CommandLine.h"
#include "GroupSpectrometer.h"
#include "Timer.h"
#include "XDataThread.h"
#include "FitsWriter.h"
#include "Common.h"

using ACS::ThreadBase;
class XBackendsImpl;
class XDataThread;
class CCommandLine;

class XarcosThread : public ACS::Thread
{
public:	

	CFitsWriter *m_file;
	CString* fileName;
	ofstream t_file;
	bool m_fileOpened;
	
	struct THeaderRecord {
		Backends::TMainHeader header;
		Backends::TSectionHeader chHeader[MAX_INPUT_NUMBER];
	};
	THeaderRecord buffer;
	int countFile;
	
	struct 	TTransferParameter {
		long channels;           /*!< number of channels involved */
		long integration;       /*<! integration time in milli seconds, it applies to all channels */
		long id[MAX_INPUT_NUMBER];	/*!< channel identifier, generally 0..channels-1  */  
		double sampleRate;              /*!< sample rate in MHz */
		long sampleSize;                   /*!< sample size in bytes */
		double normalization;         /*!<normalization factor, depends on integration and sample rate */
	};
	struct TSenderParameter {
		XBackendsImpl *sender;
		GroupSpectrometer *group;
		CCommandLine *command;
		IRA::CSecureArea<CCommandLine> *commandLine;
		IRA::CSecureArea<GroupSpectrometer> *groupSpectrometer;GroupSpectrometer *groupS;
		void cc(int a);
	};	
	/**
	 * Constructor().
	 * @param name thread name
	 * @param ACUControl pointer to the XBackendsImpl component implementation 
	 * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime	thread's sleep time in 100ns unit. Default value is 100ms.
	*/
	XarcosThread(const ACE_CString& name,TSenderParameter  *par,
                        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
                        const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime); 

	/**
	 * Destructor. 
	*/
    ~XarcosThread();
	
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
     * @param mC channel header, one header for each enabled input (channel)
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
     * It set up the Kelvin Counts ratio used to compute the system temperature for each channel.
     * @param ratio the list of the conversion factor, one for each channel
     */
    void setKelvinCountsRatio(const ACS::doubleSeq& ratio);
  
    /**
     * This method is used to inform the thread that should not send data anymore
     * @param stop true the thread will not send data any more
     */
    void setStop(bool stop) { m_stop=stop; }   
    bool getStop() { return m_stop; }
    void VisualizzaSpecific();
	void VisualizzaDato();
	void Init();
	void getTp(DWORD *tpi);
	void getTpZero(DWORD *tpi);
	void AbortInt();
	void StartInt();
	bool INT;	
	
	void SaveFileFits();
	void SaveFileTxt(double * buff, int size, int sizetp=0);

	bool processData(int max);	
	void storeDataPol(DataIntegrazione *RisulInt,int i);
	
protected:

	TIMEVALUE m_startTime;
	TTransferParameter m_dataHeader;
	XDataThread *data;
	Timer *tpTimer;
	Timer *intTimer;
	XBackendsImpl* m_pControl;
	GroupSpectrometer *groupS;
	CCommandLine *commandL;
	CSecureArea<CCommandLine> *m_commandLine;
	CSecureArea<GroupSpectrometer> *m_GroupSpectrometer;

	bool m_stop;
	bool m_go;
	bool m_sending;
	
	DWORD m_zeroBuffer[MAX_INPUT_NUMBER];
	/* It stores the conversion factor  from counts to Kelvin for each of the channels */ 
	double m_KCountsRatio[MAX_INPUT_NUMBER];
	SAMPLETYPE m_tp[MAX_INPUT_NUMBER];	
	SAMPLETYPE *sample;
	/**
	 * This method is called by the thread in order to preprocess the data
	 */
	double * preprocessData(int *res, int *Ntp);
	
};

	



#endif
