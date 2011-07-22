#ifndef ENGINETHREAD_H_
#define ENGINETHREAD_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: EngineThread.h,v 1.10 2011-06-21 16:38:30 a.orlati Exp $      */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */

#include <acsThread.h>
#include <IRA>
#include <DataCollection.h>
#include <iostream>
#include <fstream>
#include <slamac.h>
#include "Configuration.h"
#include <f2c.h>
#include <Site.h>
#include <ObservatoryC.h>

#define DATACOORDINATESSEQLENGTH 50000
#define DATATSYSSEQLENGTH 50000
#define PARAMETERNUMBER 5

namespace CalibrationTool_private {

/**
 * This class implements a working thread.
 * This thread is in charge of collecting the data from the backend(sender) and mark them with the telescope coordinates.
 * The whole packet is sent to the calibration tool engine in order to produce the output file.
*/
class CEngineThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CEngineThread(const ACE_CString& name,CSecureArea<CDataCollection> *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CEngineThread();

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
          * This method is used to initialize the thread.
      */
      virtual void initialize();

     /**
      * It sets the slice of time the thread is allowed to run.
      * @param timeSlice time slice given in 100 ns  
      */
     void setTimeSlice(const long& timeSlice) { m_timeSlice=timeSlice; }
     
     void setConfiguration(CConfiguration * const conf) { m_config=conf; }
     
     void setServices(maci::ContainerServices * const service) { m_service=service; }
     
private:
	CSecureArea<CDataCollection> *m_dataWrapper;
	ofstream m_file;
	CConfiguration *m_config;
	maci::ContainerServices * m_service;
	bool m_fileOpened;
	long m_timeSlice;
	double *m_ptsys;
	float *m_ptsys2;
    long m_device;
	bool antennaBossError;
	Antenna::AntennaBoss_var m_antennaBoss;
	bool checkTime(const ACS::Time& currentTime);
	bool checkTimeSlot(const ACS::Time& slotStart);
	bool processData();
    integer m_dataSeqCounter;
    ACS::doubleSeq m_dataSeq;
    ACS::doubleSeq m_tsysDataSeq;
    double m_coordinate;
    double m_lastCoordinate;
    double m_LatPos, m_LonPos;
    double m_LatOff, m_LonOff;
    double m_LatErr, m_LonErr;
    float *m_off;
    float *m_secsFromMidnight;
    float *m_Par;
    float *m_errPar;
    float m_reducedCHI;
    integer m_ierr;
    double m_cosLat;
    int m_CoordIndex;
    double m_raUserOff, m_decUserOff;
	double m_azUserOff, m_elUserOff;
	double m_lonUserOff, m_latUserOff;
	int m_latResult, m_lonResult;
	int m_lonDone, m_latDone;
    double * m_latPositions;
	ACS::Time m_time;
	/**
	 * This object is in charge of storing the site information
	*/
	CSite m_site;
	/**
	 * This is the reference to the observatory component
	 */
	Antenna::Observatory_var m_observatory;
	bool observatoryError;

	Antenna::TSiteInformation_var site;
};

};

#endif /*ENGINETHREAD_H_*/

