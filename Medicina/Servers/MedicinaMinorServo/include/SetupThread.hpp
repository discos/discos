#ifndef SETUPTHREAD_HPP
#define SETUPTHREAD_HPP

#include <acsThread.h>

#include <IRA>
#include "MedMinorServoControl.hpp"
#include "MedMinorServoStatus.hpp"

#define SETUP_MAX_WAIT 6000000000 // 10 minutes

class SetupThreadParameters
{
    public:
        SetupThreadParameters(MedMinorServoControl_sp,
                              MedMinorServoStatus*);
        virtual ~SetupThreadParameters();
        MedMinorServoControl_sp m_control;
        MedMinorServoStatus *m_status;
};

class SetupThread : public ACS::Thread
{
    public:
        SetupThread(const ACE_CString& name, 
                    SetupThreadParameters& params);
        virtual ~SetupThread();
        virtual void onStart();
        virtual void onStop();
        virtual void run();
    private:
        MedMinorServoControl_sp m_control;
        MedMinorServoStatus* m_status;
};

#endif

