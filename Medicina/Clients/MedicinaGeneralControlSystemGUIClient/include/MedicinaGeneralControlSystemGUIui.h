// ACS includes
#include <MedicinaGeneralControlSystemGUI.h>
#include <MedicinaGeneralControlSystem.h>
#include <MedicinaGeneralControlSystemClientEventLoop.h>
// QT includes
#include <QtCore/QObject>
#include <QtGui/QWidget>

class MedicinaGeneralControlSystemGUI : public QWidget, public Ui_MedicinaGeneralControlSystemGUI
{
    Q_OBJECT

    public:
        MedicinaGeneralControlSystemGUI(QWidget *parent = 0);

        void setParameters(maci::SimpleClient* theClient, Antenna::AntennaBoss_var theAntennaBossComponent);

    public slots:
        void Quit();
        void Antenna();

    private slots:
        void changeGUIAntennaStatusColor();

    private:
       MedicinaGeneralControlSystem myMedicinaGeneralControlSystem;
       MedicinaGeneralControlSystemClientEventLoop myMedicinaGeneralControlSystemClientEventLoop;
 };
