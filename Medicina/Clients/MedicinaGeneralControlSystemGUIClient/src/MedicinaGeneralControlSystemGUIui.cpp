// ACS includes
#include <MedicinaGeneralControlSystemGUIui.h>
// QT includes
#include <QtGui/QPalette>
#include <QtGui/QBrush>

MedicinaGeneralControlSystemGUI::MedicinaGeneralControlSystemGUI(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

//    QObject::connect(QuitButton, SIGNAL(clicked()), this, SLOT(Quit()));
//    QObject::connect(AntennaMountButton, SIGNAL(clicked()), this, SLOT(AntennaMount()));
    QObject::connect(&myMedicinaGeneralControlSystem, SIGNAL(setGUIAntennaStatusColor()), this, SLOT(changeGUIAntennaStatusColor()));
}

//void MedicinaGeneralControlSystemGUI::setParameters(maci::SimpleClient* theClient, Antenna::MedicinaMount_var theMount)
void MedicinaGeneralControlSystemGUI::setParameters(maci::SimpleClient* theClient, Antenna::AntennaBoss_var theAntennaBossComponent)
{
    myMedicinaGeneralControlSystemClientEventLoop.setSimpleClient(theClient);
    myMedicinaGeneralControlSystem.setAntennaBossComponent(theAntennaBossComponent);
    // Starts the main threads
    myMedicinaGeneralControlSystemClientEventLoop.start();
    myMedicinaGeneralControlSystem.monitor = true;
    myMedicinaGeneralControlSystem.start();
}

void MedicinaGeneralControlSystemGUI::Quit()
{
    ACS_SHORT_LOG((LM_INFO,"MedicinaGeneralControlSystemGUI::Quit"));
    // Stops Main Threads
    myMedicinaGeneralControlSystemClientEventLoop.stop();
    if (myMedicinaGeneralControlSystemClientEventLoop.isRunning() == true) {
			while (myMedicinaGeneralControlSystemClientEventLoop.isFinished() == false)
				myMedicinaGeneralControlSystemClientEventLoop.wait();
    }
    myMedicinaGeneralControlSystemClientEventLoop.terminate();

    myMedicinaGeneralControlSystem.stop();
    if (myMedicinaGeneralControlSystem.isRunning() == true) {
			while (myMedicinaGeneralControlSystem.isFinished() == false)
				myMedicinaGeneralControlSystem.wait();
    }
    myMedicinaGeneralControlSystem.terminate();

    // Close the GUI
    close();
}

void MedicinaGeneralControlSystemGUI::Antenna()
{
    system("MedicinaMountGUIClient &");
}

void MedicinaGeneralControlSystemGUI::changeGUIAntennaStatusColor()
{
    switch(myMedicinaGeneralControlSystem.AntennaStatusCode) {
        case (0):
            AntennaStatusLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
            break;
        case(-1):
            AntennaStatusLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
            break;
        case(-2):
            AntennaStatusLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
            break;
    }
}

/*
void theMedicinaGeneralControlSystemGUI::ServoMinor()
{
    system("MedicinaServoMinorGUIClient &");
}

void theMedicinaGeneralControlSystemGUI::Services()
{
    system("MedicinaServicesGUIClient &");
}

void theMedicinaGeneralControlSystemGUI::FrontEnd()
{
    system("MedicinaFrontEndGUIClient &");
}

void theMedicinaGeneralControlSystemGUI::BackEnd()
{
    system("MedicinaBackEndGUIClient &");
}

void theMedicinaGeneralControlSystemGUI::FieldSystem()
{
    system("MedicinaFieldSystemGUIClient &");
}

void theMedicinaGeneralControlSystemGUI::Metrology()
{
    system("MedicinaMetrologyGUIClient &");
}
*/
