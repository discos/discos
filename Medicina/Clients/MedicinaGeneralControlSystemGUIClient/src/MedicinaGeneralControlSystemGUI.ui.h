/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include <iostream>
#include <cstdlib>
#include <logging.h>
#include <acserr.h>
//#include <MedicinaGeneralControlSystem.h>
#include <MedicinaGeneralControlSystemGUI.h>

extern MedicinaGeneralControlSystem* myMedicinaGeneralControlSystem;

class theMedicinaGeneralControlSystemGUI : public QWidget, public Ui_MedicinaGeneralControlSystemGUI
 {
     Q_OBJECT

 public:
         theMedicinaGeneralControlSystemGUI(QWidget *parent = 0)
            : QWidget(parent)
              { setupUi(this); }

 public slots:
        void Quit();
        void AntennaMount();
 };

void theMedicinaGeneralControlSystemGUI::Quit()
{
    ACS_SHORT_LOG((LM_INFO,"theMedicinaGeneralControlSystemGUI::Quit"));
    // Stops Main Threads
    myMedicinaGeneralControlSystem->stop();
    close();
}

void theMedicinaGeneralControlSystemGUI::AntennaMount()
{
    system("MedicinaMountGUIClient &");
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

void theMedicinaGeneralControlSystemGUI::init()
{

}

void theMedicinaGeneralControlSystemGUI::destroy()
{

}
*/
