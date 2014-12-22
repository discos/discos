#ifndef MEDMINORSERVOREADTHREAD_HPP
#define MEDMINORSERVOREADTHREAD_HPP

#include <acsThread.h>

#include "IRA"
#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoModbus.hpp"
#include "PositionQueue.hpp"

class PositionMonitoringThread : public ACS::Thread
{
    public:
        PositionMonitoringThread(const ACE_CString& name, 
                                const int sleepTime,
                                PositionQueue* positions,
                                MedMinorServoModbus_sp channel);
        virtual void runLoop();
    private:
        PositionQueue *_positions;
        MedMinorServoModbus_sp _channel;
};

#endif

