#ifndef __SRTMINORSERVOBOSSCORE_H__
#define __SRTMINORSERVOBOSSCORE_H__

/**
 * SRTMinorServoBossCore.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "Common.h"
#include <IRA>
#include <baciSmartPropertyPointer.h>
#include <baciROboolean.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <SRTMinorServoBossS.h>
#include <SRTMinorServoS.h>
#include <ManagmentDefinitionsS.h>
#include "ManagementErrors.h"
#include "MinorServoErrors.h"
#include "SRTMinorServoSocket.h"
#include "MSDevIOs.h"
#include "SRTMinorServoBossImpl.h"
#include "SRTMinorServoSetupThread.h"
#include "SRTMinorServoParkThread.h"
#include "SRTMinorServoTrackingThread.h"

class SRTMinorServoBossImpl;
class SRTMinorServoSetupThread;
class SRTMinorServoParkThread;
class SRTMinorServoTrackingThread;


class SRTMinorServoBossCore
{
friend class SRTMinorServoBossImpl;
friend class SRTMinorServoStatusThread;
friend class SRTMinorServoSetupThread;
friend class SRTMinorServoParkThread;
friend class SRTMinorServoTrackingThread;

public:
    SRTMinorServoBossCore(SRTMinorServoBossImpl* component);
    virtual ~SRTMinorServoBossCore();

private:
    void setup(std::string configuration);
    void park();
    void status();
    void checkControl();
    void checkEmergency();
    void setElevationTracking(std::string configuration);
    void setASConfiguration(std::string configuration);
    void preset(double elevation);

    void clearUserOffsets(std::string servo_name);
    void setUserOffset(std::string servo_axis_name, double offset);
    std::vector<double> getUserOffsets();
    void clearSystemOffsets(std::string servo_name);
    void setSystemOffset(std::string servo_axis_name, double offset);
    std::vector<double> getSystemOffsets();

    void getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units);

    bool checkScan(const ACS::Time start_time, const MinorServo::MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info, MinorServo::TRunTimeParameters_out ms_parameters);
    void startScan(ACS::Time& start_time, const MinorServo::MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info);
    void closeScan(ACS::Time& close_time);

    SRTMinorServoBossImpl* m_component;

    SRTMinorServoSocketConfiguration* m_socket_configuration;
    SRTMinorServoSocket* m_socket;

    SRTMinorServoSetupThread* m_setup_thread;
    SRTMinorServoParkThread* m_park_thread;
    SRTMinorServoTrackingThread* m_tracking_thread;

    IRA::CSecureArea<SRTMinorServoAnswerMap>* m_status_secure_area;
    SRTMinorServoAnswerMap m_status;

    std::map<std::string, MinorServo::SRTBaseMinorServo_ptr> m_servos;
    std::map<std::string, MinorServo::SRTProgramTrackMinorServo_ptr> m_tracking_servos;

    std::atomic<MinorServo::SRTMinorServoMotionStatus> m_motion_status;
    std::string m_commanded_setup;
    std::atomic<MinorServo::SRTMinorServoFocalConfiguration> m_commanded_configuration;
    std::atomic<MinorServo::SRTMinorServoFocalConfiguration> m_current_configuration;

    std::atomic<Management::TSystemStatus> m_subsystem_status;
    std::string m_actual_setup;
    std::atomic<Management::TBoolean> m_ready;
    std::atomic<Management::TBoolean> m_starting;
    std::atomic<Management::TBoolean> m_as_configuration;
    std::atomic<Management::TBoolean> m_elevation_tracking;
    std::atomic<Management::TBoolean> m_elevation_tracking_enabled;
    std::atomic<Management::TBoolean> m_scan_active;
    std::atomic<Management::TBoolean> m_scanning;
    std::atomic<Management::TBoolean> m_tracking;
};

#endif
