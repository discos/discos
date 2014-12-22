#include "PositionMonitoringThread.hpp"

PositionMonitoringThread::PositionMonitoringThread(const ACE_CString& name, 
                                const int sleepTime,
                                PositionQueue* positions,
                                MedMinorServoModbus_sp channel) : 
    ACS::Thread(name, sleepTime),
    _positions(positions),
    _channel(channel)
    {}

void 
PositionMonitoringThread::runLoop()
{
    _actual_status =_modbus->read_status();
    _actual_position = MedMinorServoGeometry::positionFromAxes(_actual_status);
    _position_queue.push(_actual_position);
}

