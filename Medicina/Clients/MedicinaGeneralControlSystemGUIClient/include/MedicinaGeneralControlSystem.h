// System includes

// ACS includes
#include <baci.h>
#include <AntennaBossC.h>
#include <AntennaDefinitionsC.h>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <IRA>

// QT includes
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtGui/QColor>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>

class MedicinaGeneralControlSystem : public QThread
{
    Q_OBJECT

	public:
        MedicinaGeneralControlSystem(QObject *parent = 0);
        ~MedicinaGeneralControlSystem();

	    void setAntennaBossComponent(Antenna::AntennaBoss_var);
	    void run ();
	    void stop ();

        bool monitor;

        int AntennaStatusCode;
        
    signals:
        int setGUIAntennaStatusColor();

    private:
        Antenna::AntennaBoss_var tAntennaBossComponent;
};
