#ifndef __SRTMINORSERVOBOSSCORE_H__
#define __SRTMINORSERVOBOSSCORE_H__

/**
 * SRTMinorServoBossCore.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <IRA>
#include <baciSmartPropertyPointer.h>
#include <baciROboolean.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <SRTMinorServoBossS.h>
#include <SRTMinorServoS.h>
#include <boost/bimap.hpp>
#include "ManagementErrors.h"
#include "MinorServoErrors.h"
#include "SRTMinorServoSocket.h"
#include "MSDevIOs.h"
#include "SRTMinorServoBossImpl.h"
#include "SRTMinorServoSetupThread.h"
#include "SRTMinorServoParkThread.h"

class SRTMinorServoBossImpl;
class SRTMinorServoSetupThread;
class SRTMinorServoParkThread;

using SRTMinorServoFocalConfigurationsTable = boost::bimap<std::string, MinorServo::SRTMinorServoFocalConfiguration>;


class SRTMinorServoBossCore
{
friend class SRTMinorServoBossImpl;
friend class SRTMinorServoStatusThread;
friend class SRTMinorServoSetupThread;
friend class SRTMinorServoParkThread;

public:
    SRTMinorServoBossCore(SRTMinorServoBossImpl* component);
    virtual ~SRTMinorServoBossCore();

private:
    void setup(std::string configuration);
    void park();
    void status();
    bool checkControl();

    SRTMinorServoSocketConfiguration* m_socket_configuration;
    SRTMinorServoSocket* m_socket;

    SRTMinorServoSetupThread* m_setup_thread;
    SRTMinorServoParkThread* m_park_thread;

    SRTMinorServoAnswerMap m_status;

    SRTMinorServoFocalConfigurationsTable m_focal_configurations_table;

    IRA::CSecureArea<SRTMinorServoAnswerMap>* m_status_secure_area;

    MinorServo::SRTMinorServoFocalConfiguration m_requested_configuration;
    MinorServo::SRTMinorServoFocalConfiguration m_current_configuration;

    MinorServo::SRTGenericMinorServo_var m_GFR;
    MinorServo::SRTProgramTrackMinorServo_var m_SRP;

    MinorServo::SRTMinorServoBossStatus m_boss_status;

    SRTMinorServoBossImpl* m_component;

    bool m_ready;

    std::vector<MinorServo::SRTBaseMinorServo_ptr> m_servos;
};

#endif
