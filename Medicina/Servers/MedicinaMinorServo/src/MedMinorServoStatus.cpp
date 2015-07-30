#include <string>
#include "MedMinorServoStatus.hpp"

MedMinorServoStatus::MedMinorServoStatus():
    starting(false),
    ready(false),
    scan_active(false),
    scanning(false),
    parking(false),
    parked(false),
    elevation_tracking(false),
    as_configuration(false),
    status(Management::MNG_OK)
{}

MedMinorServoStatus::~MedMinorServoStatus()
{}

std::string
MedMinorServoStatus::getStatusString()
{
    if(starting)
        return "STARTING";
    if(scan_active)
        return "SCAN_ACTIVE";
    if(scanning)
        return "SCANNING";
    if(parking)
        return "PARKING";
    if(parked)
        return "PARKED";
    if(ready)
        return "READY";
    return "UNDEFINED";
}

void
MedMinorServoStatus::reset()
{
    starting = false;
    ready = false;
    scan_active = false;
    scanning = false;
    parking = false;
    parked = false;
    elevation_tracking = false;
    as_configuration = false;
    status = Management::MNG_OK;
}

MedMinorServoStatus& 
MedMinorServoStatus::operator=(const MedMinorServoStatus& other)
{
    starting = other.starting;
    ready = other.ready;
    scan_active = other.scan_active;
    scanning = other.scan_active;
    parking = other.parking;
    parked = other.parked;
    elevation_tracking = other.elevation_tracking;
    as_configuration = other.as_configuration;
    status = other.status;
    return *this;
}
