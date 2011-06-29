/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __SETUP_THREAD_H__
#define __SETUP_THREAD_H__

#include <acsThread.h>
#include <acsncSimpleSupplier.h>
#include <LogFilter.h>
#include <IRA>
#include <MinorServoS.h>
#include "MSParameters.h"
#include <string>
#include <map>
#include <pthread.h>
#include "utils.h"

using namespace maci;

#define ITER_SLEEP_TIME 5000000 // Iteration sleep time: 0.5 seconds
#define INCR_SLEEP_TIME 20000000 // Increment sleep time: 2 seconds
#define MAX_PARK_TIME 80000000 // Maximum park time: 8 seconds
#define MAX_SETUP_TIME 80000000 // Maximum setup time: 8 seconds
// Time in 100ns: 10000000 == 1sec
const unsigned int SCAN_SHIFT_TIME = 50000000;

struct TrackingThread;
struct MSBossPublisher;
struct ScanThread;

struct ScanPosition {
    ACS::Time time;
    double delta_pos;
};

struct ScanData {
    vector<ScanPosition> *scan_pos;
    string comp_name;
    unsigned short axis_id;
    // Time needed to move from actual position to starting scan position
    ACS::Time positioning_time;
};


struct MSThreadParameters {
    vector<string> actions;
    map<string, bool> *tracking_list;
    map<string, MinorServo::WPServo_var> *component_refs;
    bool is_initialized;
    bool is_setup_locked;
    bool is_parking_locked;
    bool *is_scanning_ptr;
    string *actual_conf;
    string commanded_conf;
    TrackingThread *tracking_thread_ptr;
    ScanThread *scan_thread_ptr;
    ContainerServices *services;
	nc::SimpleSupplier *nchannel;
    Management::TSystemStatus *status;
    pthread_mutex_t *setup_mutex;
    ACS::Time starting_scan_time;
    bool is_tracking_enabled;
    ScanData scan_data;
};


// This class updates the status property of minor servos.
class SetupThread : public ACS::Thread
{
public:

	SetupThread(
        const ACE_CString& name, 
        MSThreadParameters& params,
        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
        const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime,
        const bool del=false
    );

    ~SetupThread();

    /// This method is executed once when the thread starts.
    virtual void onStart();

    /// This method is executed once when the thread stops.
    virtual void onStop();

    /// This method overrides the thread implementation class. The method is executed only once.
    virtual void run();

private:
    MSThreadParameters *m_params;
     
};

#endif 

