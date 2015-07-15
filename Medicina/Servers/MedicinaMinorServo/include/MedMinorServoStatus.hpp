#ifndef MEDMINORSERVOSTATUS_HPP
#define MEDMINORSERVOSTATUS_HPP

#include <ManagmentDefinitionsS.h>

class MedMinorServoStatus
{
    public:
        MedMinorServoStatus();
        virtual ~MedMinorServoStatus();
        bool starting;
        bool ready;
        bool scan_active;
        bool scanning;
        bool parking;
        bool parked;
        bool elevation_tracking;
        bool as_configuration;
        Management::TSystemStatus status;
        void reset();
        MedMinorServoStatus& operator=(const MedMinorServoStatus&);
        std::string getStatusString();
    private:
        MedMinorServoStatus(const MedMinorServoStatus&);
};

#endif

