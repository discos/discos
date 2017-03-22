// System includes
#include <ctime>
#include <deque>
#include <exception>

// ACS includes
#include <acstime.h>
#include <MedicinaGeneralControlSystem.h>
// QT includes

MedicinaGeneralControlSystem::MedicinaGeneralControlSystem (QObject *parent) : QThread(parent)
{
}

MedicinaGeneralControlSystem::~MedicinaGeneralControlSystem()
{
}

void MedicinaGeneralControlSystem::stop ()
{
  	monitor = false;
}

void MedicinaGeneralControlSystem::setAntennaBossComponent (Antenna::AntennaBoss_var theAntennaBossComponent)
{
	tAntennaBossComponent = theAntennaBossComponent;
}

void MedicinaGeneralControlSystem::run()
{
    Management::ROTSystemStatus_var bossStatus_var;
    ACS::pattern bossStatus_val;
    ACSErr::Completion_var completion;

    while (monitor == true) {
        bossStatus_var = tAntennaBossComponent->status();
        bossStatus_val = bossStatus_var->get_sync(completion.out());
        switch (bossStatus_val) {
            case (Management::MNG_OK):
                AntennaStatusCode = 0;
                break;
            case (Management::MNG_WARNING):
                AntennaStatusCode = -1;
                break;
            case (Management::MNG_FAILURE):
                AntennaStatusCode = -2;
                break;
        }
        emit setGUIAntennaStatusColor();
    }
}
