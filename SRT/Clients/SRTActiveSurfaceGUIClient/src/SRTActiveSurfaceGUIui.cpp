// ACS includes
#include <SRTActiveSurfaceGUIui.h>

// QT includes
#include <QtGui/QPalette>
#include <QtGui/QBrush>

int thecircle, theactuator, theradius;
int tcircle, tactuator, tradius;

SRTActiveSurfaceGUI::SRTActiveSurfaceGUI(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIActuatorColor(int,int,const char*,bool)), this, SLOT(changeGUIActuatorColor(int,int,const char*,bool)));
    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIAllActuators(bool)), this, SLOT(changeGUIAllActuators(bool)));
    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIcircleORradius(bool)), this, SLOT(changeGUIcircleORradius(bool)));
    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIActuator(bool)), this, SLOT(changeGUIActuator(bool)));
    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIActuatorStatusEnblLabel()), this, SLOT(changeGUIActuatorStatusEnblLabel()));
    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIActuatorValues()), this, SLOT(changeGUIActuatorValues()));
    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIActuatorStatusLabels()), this, SLOT(changeGUIActuatorStatusLabels()));
    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIasStatusCode(int)), this, SLOT(changeGUIasStatusCode(int)));
    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIasProfileCode(int)), this, SLOT(changeGUIasProfileCode(int)));

#ifdef MANAGEMENT
    buttonGroup1->setEnabled(true);
#else
    buttonGroup1->setVisible(false);
#endif
}

void SRTActiveSurfaceGUI::setParameters(maci::SimpleClient* theClient, ActiveSurface::SRTActiveSurfaceBoss_var theASBoss)
{
    mySRTActiveSurfaceClientEventLoop.setSimpleClient(theClient);
    mySRTActiveSurfaceCore.setASBoss(theASBoss);
    // Starts the main threads
    mySRTActiveSurfaceClientEventLoop.start();
    mySRTActiveSurfaceCore.monitor = true;
    mySRTActiveSurfaceCore.start();
}

void SRTActiveSurfaceGUI::Quit()
{
    ACS_SHORT_LOG((LM_INFO,"SRTActiveSurfaceCoreGUI::Quit"));
    // Stops Main Threads
    mySRTActiveSurfaceClientEventLoop.stop();
    if (mySRTActiveSurfaceClientEventLoop.isRunning() == true) {
			while (mySRTActiveSurfaceClientEventLoop.isFinished() == false)
				mySRTActiveSurfaceClientEventLoop.wait();
    }
    mySRTActiveSurfaceClientEventLoop.terminate();

    mySRTActiveSurfaceCore.stop();
    if (mySRTActiveSurfaceCore.isRunning() == true) {
			while (mySRTActiveSurfaceCore.isFinished() == false)
				mySRTActiveSurfaceCore.wait();
    }
    mySRTActiveSurfaceCore.terminate();

    // Close the GUI
    close();
}

// ALL ACTUATORS
void SRTActiveSurfaceGUI::setallactuators()
{
    //mySRTActiveSurfaceCore.setallactuators();
    thecircle = 0; theactuator = 0; theradius = 0;
    changeGUIAllActuators();
}
// RADIUS
void SRTActiveSurfaceGUI::setradius1()
{
    thecircle = 0; theactuator = 0; theradius = 1;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius2()
{
    thecircle = 0; theactuator = 0; theradius = 2;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius3()
{
    thecircle = 0; theactuator = 0; theradius = 3;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius4()
{
    thecircle = 0; theactuator = 0; theradius = 4;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius5()
{
    thecircle = 0; theactuator = 0; theradius = 5;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius6()
{
    thecircle = 0; theactuator = 0; theradius = 6;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius7()
{
    thecircle = 0; theactuator = 0; theradius = 7;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius8()
{
    thecircle = 0; theactuator = 0; theradius = 8;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius9()
{
    thecircle = 0; theactuator = 0; theradius = 9;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius10()
{
    thecircle = 0; theactuator = 0; theradius = 10;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius11()
{
    thecircle = 0; theactuator = 0; theradius = 11;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius12()
{
    thecircle = 0; theactuator = 0; theradius = 12;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius13()
{
    thecircle = 0; theactuator = 0; theradius = 13;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius14()
{
    thecircle = 0; theactuator = 0; theradius = 14;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius15()
{
    thecircle = 0; theactuator = 0; theradius = 15;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius16()
{
    thecircle = 0; theactuator = 0; theradius = 16;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius17()
{
    thecircle = 0; theactuator = 0; theradius = 17;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius18()
{
    thecircle = 0; theactuator = 0; theradius = 18;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius19()
{
    thecircle = 0; theactuator = 0; theradius = 19;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius20()
{
    thecircle = 0; theactuator = 0; theradius = 20;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius21()
{
    thecircle = 0; theactuator = 0; theradius = 21;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius22()
{
    thecircle = 0; theactuator = 0; theradius = 22;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius23()
{
    thecircle = 0; theactuator = 0; theradius = 23;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius24()
{
    thecircle = 0; theactuator = 0; theradius = 24;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius25()
{
    thecircle = 0; theactuator = 0; theradius = 25;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius26()
{
    thecircle = 0; theactuator = 0; theradius = 26;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius27()
{
    thecircle = 0; theactuator = 0; theradius = 27;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius28()
{
    thecircle = 0; theactuator = 0; theradius = 28;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius29()
{
    thecircle = 0; theactuator = 0; theradius = 29;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius30()
{
    thecircle = 0; theactuator = 0; theradius = 30;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius31()
{
    thecircle = 0; theactuator = 0; theradius = 31;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius32()
{
    thecircle = 0; theactuator = 0; theradius = 32;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius33()
{
    thecircle = 0; theactuator = 0; theradius = 33;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius34()
{
    thecircle = 0; theactuator = 0; theradius = 34;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius35()
{
    thecircle = 0; theactuator = 0; theradius = 35;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius36()
{
    thecircle = 0; theactuator = 0; theradius = 36;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius37()
{
    thecircle = 0; theactuator = 0; theradius = 37;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius38()
{
    thecircle = 0; theactuator = 0; theradius = 38;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius39()
{
    thecircle = 0; theactuator = 0; theradius = 39;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius40()
{
    thecircle = 0; theactuator = 0; theradius = 40;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius41()
{
    thecircle = 0; theactuator = 0; theradius = 41;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius42()
{
    thecircle = 0; theactuator = 0; theradius = 42;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius43()
{
    thecircle = 0; theactuator = 0; theradius = 43;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius44()
{
    thecircle = 0; theactuator = 0; theradius = 44;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius45()
{
    thecircle = 0; theactuator = 0; theradius = 45;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius46()
{
    thecircle = 0; theactuator = 0; theradius = 46;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius47()
{
    thecircle = 0; theactuator = 0; theradius = 47;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius48()
{
    thecircle = 0; theactuator = 0; theradius = 48;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius49()
{
    thecircle = 0; theactuator = 0; theradius = 49;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius50()
{
    thecircle = 0; theactuator = 0; theradius = 50;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius51()
{
    thecircle = 0; theactuator = 0; theradius = 51;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius52()
{
    thecircle = 0; theactuator = 0; theradius = 52;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius53()
{
    thecircle = 0; theactuator = 0; theradius = 53;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius54()
{
    thecircle = 0; theactuator = 0; theradius = 54;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius55()
{
    thecircle = 0; theactuator = 0; theradius = 55;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius56()
{
    thecircle = 0; theactuator = 0; theradius = 56;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius57()
{
    thecircle = 0; theactuator = 0; theradius = 57;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius58()
{
    thecircle = 0; theactuator = 0; theradius = 58;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius59()
{
    thecircle = 0; theactuator = 0; theradius = 59;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius60()
{
    thecircle = 0; theactuator = 0; theradius = 60;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius61()
{
    thecircle = 0; theactuator = 0; theradius = 61;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius62()
{
    thecircle = 0; theactuator = 0; theradius = 62;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius63()
{
    thecircle = 0; theactuator = 0; theradius = 63;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius64()
{
    thecircle = 0; theactuator = 0; theradius = 64;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius65()
{
    thecircle = 0; theactuator = 0; theradius = 65;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius66()
{
    thecircle = 0; theactuator = 0; theradius = 66;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius67()
{
    thecircle = 0; theactuator = 0; theradius = 67;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius68()
{
    thecircle = 0; theactuator = 0; theradius = 68;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius69()
{
    thecircle = 0; theactuator = 0; theradius = 69;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius70()
{
    thecircle = 0; theactuator = 0; theradius = 70;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius71()
{
    thecircle = 0; theactuator = 0; theradius = 71;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius72()
{
    thecircle = 0; theactuator = 0; theradius = 72;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius73()
{
    thecircle = 0; theactuator = 0; theradius = 73;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius74()
{
    thecircle = 0; theactuator = 0; theradius = 74;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius75()
{
    thecircle = 0; theactuator = 0; theradius = 75;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius76()
{
    thecircle = 0; theactuator = 0; theradius = 76;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius77()
{
    thecircle = 0; theactuator = 0; theradius = 77;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius78()
{
    thecircle = 0; theactuator = 0; theradius = 78;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius79()
{
    thecircle = 0; theactuator = 0; theradius = 79;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius80()
{
    thecircle = 0; theactuator = 0; theradius = 80;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius81()
{
    thecircle = 0; theactuator = 0; theradius = 81;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius82()
{
    thecircle = 0; theactuator = 0; theradius = 82;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius83()
{
    thecircle = 0; theactuator = 0; theradius = 83;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius84()
{
    thecircle = 0; theactuator = 0; theradius = 84;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius85()
{
    thecircle = 0; theactuator = 0; theradius = 85;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius86()
{
    thecircle = 0; theactuator = 0; theradius = 86;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius87()
{
    thecircle = 0; theactuator = 0; theradius = 87;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius88()
{
    thecircle = 0; theactuator = 0; theradius = 88;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius89()
{
    thecircle = 0; theactuator = 0; theradius = 89;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius90()
{
    thecircle = 0; theactuator = 0; theradius = 90;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius91()
{
    thecircle = 0; theactuator = 0; theradius = 91;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius92()
{
    thecircle = 0; theactuator = 0; theradius = 92;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius93()
{
    thecircle = 0; theactuator = 0; theradius = 93;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius94()
{
    thecircle = 0; theactuator = 0; theradius = 94;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius95()
{
    thecircle = 0; theactuator = 0; theradius = 95;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setradius96()
{
    thecircle = 0; theactuator = 0; theradius = 96;
    changeGUIcircleORradius();
}

// CIRCLES
void SRTActiveSurfaceGUI::setcircle1()
{
    thecircle = 1; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle2()
{
    thecircle = 2; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle3()
{
    thecircle = 3; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle4()
{
    thecircle = 4; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle5()
{
    thecircle = 5; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle6()
{
    thecircle = 6; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle7()
{
    thecircle = 7; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle8()
{
    thecircle = 8; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle9()
{
    thecircle = 9; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle10()
{
    thecircle = 10; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle11()
{
    thecircle = 11; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle12()
{
    thecircle = 12; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle13()
{
    thecircle = 13; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle14()
{
    thecircle = 14; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle15()
{
    thecircle = 15; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle16()
{
    thecircle = 16; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
void SRTActiveSurfaceGUI::setcircle17()
{
    thecircle = 17; theactuator = 0; theradius = 0;
    changeGUIcircleORradius();
}
// 1 CIRCLE
void SRTActiveSurfaceGUI::setactuator1_1()
{
    thecircle=1; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 1);
}
void SRTActiveSurfaceGUI::setactuator1_2()
{
    thecircle=1; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 2);
}
void SRTActiveSurfaceGUI::setactuator1_3()
{
    thecircle=1; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 3);
}
void SRTActiveSurfaceGUI::setactuator1_4()
{
    thecircle=1; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 4);
}
void SRTActiveSurfaceGUI::setactuator1_5()
{
    thecircle=1; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 5);
}
void SRTActiveSurfaceGUI::setactuator1_6()
{
    thecircle=1; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 6);
}
void SRTActiveSurfaceGUI::setactuator1_7()
{
    thecircle=1; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 7);
}
void SRTActiveSurfaceGUI::setactuator1_8()
{
    thecircle=1; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 8);
}
void SRTActiveSurfaceGUI::setactuator1_9()
{
    thecircle=1; theactuator=9; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 9);
}
void SRTActiveSurfaceGUI::setactuator1_10()
{
    thecircle=1; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 10);
}
void SRTActiveSurfaceGUI::setactuator1_11()
{
    thecircle=1; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 11);
}
void SRTActiveSurfaceGUI::setactuator1_12()
{
    thecircle=1; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 12);
}
void SRTActiveSurfaceGUI::setactuator1_13()
{
    thecircle=1; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 13);
}
void SRTActiveSurfaceGUI::setactuator1_14()
{
    thecircle=1; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 14);
}
void SRTActiveSurfaceGUI::setactuator1_15()
{
    thecircle=1; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 15);
}
void SRTActiveSurfaceGUI::setactuator1_16()
{
    thecircle=1; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 16);
}
void SRTActiveSurfaceGUI::setactuator1_17()
{
    thecircle=1; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 17);
}
void SRTActiveSurfaceGUI::setactuator1_18()
{
    thecircle=1; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 18);
}
void SRTActiveSurfaceGUI::setactuator1_19()
{
    thecircle=1; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 19);
}
void SRTActiveSurfaceGUI::setactuator1_20()
{
    thecircle=1; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 20);
}
void SRTActiveSurfaceGUI::setactuator1_21()
{
    thecircle=1; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 21);
}
void SRTActiveSurfaceGUI::setactuator1_22()
{
    thecircle=1; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 22);
}
void SRTActiveSurfaceGUI::setactuator1_23()
{
    thecircle=1; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 23);
}
void SRTActiveSurfaceGUI::setactuator1_24()
{
    thecircle=1; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(1, 24);
}

// 2 CIRCLE
void SRTActiveSurfaceGUI::setactuator2_1()
{
    thecircle=2; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 1);
}
void SRTActiveSurfaceGUI::setactuator2_2()
{
    thecircle=2; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 2);
}
void SRTActiveSurfaceGUI::setactuator2_3()
{
    thecircle=2; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 3);
}
void SRTActiveSurfaceGUI::setactuator2_4()
{
    thecircle=2; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 4);
}
void SRTActiveSurfaceGUI::setactuator2_5()
{
    thecircle=2; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 5);
}
void SRTActiveSurfaceGUI::setactuator2_6()
{
    thecircle=2; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 6);
}
void SRTActiveSurfaceGUI::setactuator2_7()
{
    thecircle=2; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 7);
}
void SRTActiveSurfaceGUI::setactuator2_8()
{
    thecircle=2; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 8);
}
void SRTActiveSurfaceGUI::setactuator2_9()
{
    thecircle=2; theactuator=9; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 9);
}
void SRTActiveSurfaceGUI::setactuator2_10()
{
    thecircle=2; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 10);
}
void SRTActiveSurfaceGUI::setactuator2_11()
{
    thecircle=2; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 11);
}
void SRTActiveSurfaceGUI::setactuator2_12()
{
    thecircle=2; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 12);
}
void SRTActiveSurfaceGUI::setactuator2_13()
{
    thecircle=2; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 13);
}
void SRTActiveSurfaceGUI::setactuator2_14()
{
    thecircle=2; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 14);
}
void SRTActiveSurfaceGUI::setactuator2_15()
{
    thecircle=2; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 15);
}
void SRTActiveSurfaceGUI::setactuator2_16()
{
    thecircle=2; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 16);
}
void SRTActiveSurfaceGUI::setactuator2_17()
{
    thecircle=2; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 17);
}
void SRTActiveSurfaceGUI::setactuator2_18()
{
    thecircle=2; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 18);
}
void SRTActiveSurfaceGUI::setactuator2_19()
{
    thecircle=2; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 19);
}
void SRTActiveSurfaceGUI::setactuator2_20()
{
    thecircle=2; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 20);
}
void SRTActiveSurfaceGUI::setactuator2_21()
{
    thecircle=2; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 21);
}
void SRTActiveSurfaceGUI::setactuator2_22()
{
    thecircle=2; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 22);
}
void SRTActiveSurfaceGUI::setactuator2_23()
{
    thecircle=2; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 23);
}
void SRTActiveSurfaceGUI::setactuator2_24()
{
    thecircle=2; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(2, 24);
}

// 3 CIRCLE
void SRTActiveSurfaceGUI::setactuator3_1()
{
    thecircle=3; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 1);
}
void SRTActiveSurfaceGUI::setactuator3_2()
{
    thecircle=3; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 2);
}
void SRTActiveSurfaceGUI::setactuator3_3()
{
    thecircle=3; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 3);
}
void SRTActiveSurfaceGUI::setactuator3_4()
{
    thecircle=3; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 4);
}
void SRTActiveSurfaceGUI::setactuator3_5()
{
    thecircle=3; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 5);
}
void SRTActiveSurfaceGUI::setactuator3_6()
{
    thecircle=3; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 6);
}
void SRTActiveSurfaceGUI::setactuator3_7()
{
    thecircle=3; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 7);
}
void SRTActiveSurfaceGUI::setactuator3_8()
{
    thecircle=3; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 8);
}
void SRTActiveSurfaceGUI::setactuator3_9()
{
   thecircle=3; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(3, 9);
}
void SRTActiveSurfaceGUI::setactuator3_10()
{
    thecircle=3; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 10);
}
void SRTActiveSurfaceGUI::setactuator3_11()
{
    thecircle=3; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 11);
}
void SRTActiveSurfaceGUI::setactuator3_12()
{
    thecircle=3; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 12);
}
void SRTActiveSurfaceGUI::setactuator3_13()
{
    thecircle=3; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 13);
}
void SRTActiveSurfaceGUI::setactuator3_14()
{
    thecircle=3; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 14);
}
void SRTActiveSurfaceGUI::setactuator3_15()
{
    thecircle=3; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 15);
}
void SRTActiveSurfaceGUI::setactuator3_16()
{
    thecircle=3; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 16);
}
void SRTActiveSurfaceGUI::setactuator3_17()
{
    thecircle=3; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 17);
}
void SRTActiveSurfaceGUI::setactuator3_18()
{
    thecircle=3; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 18);
}
void SRTActiveSurfaceGUI::setactuator3_19()
{
    thecircle=3; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 19);
}
void SRTActiveSurfaceGUI::setactuator3_20()
{
    thecircle=3; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 20);
}
void SRTActiveSurfaceGUI::setactuator3_21()
{
    thecircle=3; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 21);
}
void SRTActiveSurfaceGUI::setactuator3_22()
{
    thecircle=3; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 22);
}
void SRTActiveSurfaceGUI::setactuator3_23()
{
    thecircle=3; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 23);
}
void SRTActiveSurfaceGUI::setactuator3_24()
{
    thecircle=3; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 24);
}
void SRTActiveSurfaceGUI::setactuator3_25()
{
    thecircle=3; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 25);
}
void SRTActiveSurfaceGUI::setactuator3_26()
{
    thecircle=3; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 26);
}
void SRTActiveSurfaceGUI::setactuator3_27()
{
    thecircle=3; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 27);
}
void SRTActiveSurfaceGUI::setactuator3_28()
{
    thecircle=3; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 28);
}
void SRTActiveSurfaceGUI::setactuator3_29()
{
    thecircle=3; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 29);
}
void SRTActiveSurfaceGUI::setactuator3_30()
{
    thecircle=3; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 30);
}
void SRTActiveSurfaceGUI::setactuator3_31()
{
    thecircle=3; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 31);
}
void SRTActiveSurfaceGUI::setactuator3_32()
{
    thecircle=3; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 32);
}
void SRTActiveSurfaceGUI::setactuator3_33()
{
    thecircle=3; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 33);
}
void SRTActiveSurfaceGUI::setactuator3_34()
{
    thecircle=3; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 34);
}
void SRTActiveSurfaceGUI::setactuator3_35()
{
    thecircle=3; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 35);
}
void SRTActiveSurfaceGUI::setactuator3_36()
{
    thecircle=3; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 36);
}
void SRTActiveSurfaceGUI::setactuator3_37()
{
    thecircle=3; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 37);
}
void SRTActiveSurfaceGUI::setactuator3_38()
{
    thecircle=3; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 38);
}
void SRTActiveSurfaceGUI::setactuator3_39()
{
    thecircle=3; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 39);
}
void SRTActiveSurfaceGUI::setactuator3_40()
{
    thecircle=3; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 40);
}
void SRTActiveSurfaceGUI::setactuator3_41()
{
    thecircle=3; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 41);
}
void SRTActiveSurfaceGUI::setactuator3_42()
{
    thecircle=3; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 42);
}
void SRTActiveSurfaceGUI::setactuator3_43()
{
    thecircle=3; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 43);
}
void SRTActiveSurfaceGUI::setactuator3_44()
{
    thecircle=3; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 44);
}
void SRTActiveSurfaceGUI::setactuator3_45()
{
    thecircle=3; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 45);
}
void SRTActiveSurfaceGUI::setactuator3_46()
{
    thecircle=3; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 46);
}
void SRTActiveSurfaceGUI::setactuator3_47()
{
    thecircle=3; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 47);
}
void SRTActiveSurfaceGUI::setactuator3_48()
{
    thecircle=3; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(3, 48);
}

// 4 CIRCLE
void SRTActiveSurfaceGUI::setactuator4_1()
{
    thecircle=4; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 1);
}
void SRTActiveSurfaceGUI::setactuator4_2()
{
    thecircle=4; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 2);
}
void SRTActiveSurfaceGUI::setactuator4_3()
{
    thecircle=4; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 3);
}
void SRTActiveSurfaceGUI::setactuator4_4()
{
    thecircle=4; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 4);
}
void SRTActiveSurfaceGUI::setactuator4_5()
{
    thecircle=4; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 5);
}
void SRTActiveSurfaceGUI::setactuator4_6()
{
    thecircle=4; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 6);
}
void SRTActiveSurfaceGUI::setactuator4_7()
{
    thecircle=4; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 7);
}
void SRTActiveSurfaceGUI::setactuator4_8()
{
    thecircle=4; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 8);
}
void SRTActiveSurfaceGUI::setactuator4_9()
{
   thecircle=4; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(4, 9);
}
void SRTActiveSurfaceGUI::setactuator4_10()
{
    thecircle=4; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 10);
}
void SRTActiveSurfaceGUI::setactuator4_11()
{
    thecircle=4; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 11);
}
void SRTActiveSurfaceGUI::setactuator4_12()
{
    thecircle=4; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 12);
}
void SRTActiveSurfaceGUI::setactuator4_13()
{
    thecircle=4; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 13);
}
void SRTActiveSurfaceGUI::setactuator4_14()
{
    thecircle=4; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 14);
}
void SRTActiveSurfaceGUI::setactuator4_15()
{
    thecircle=4; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 15);
}
void SRTActiveSurfaceGUI::setactuator4_16()
{
    thecircle=4; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 16);
}
void SRTActiveSurfaceGUI::setactuator4_17()
{
    thecircle=4; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 17);
}
void SRTActiveSurfaceGUI::setactuator4_18()
{
    thecircle=4; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 18);
}
void SRTActiveSurfaceGUI::setactuator4_19()
{
    thecircle=4; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 19);
}
void SRTActiveSurfaceGUI::setactuator4_20()
{
    thecircle=4; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 20);
}
void SRTActiveSurfaceGUI::setactuator4_21()
{
    thecircle=4; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 21);
}
void SRTActiveSurfaceGUI::setactuator4_22()
{
    thecircle=4; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 22);
}
void SRTActiveSurfaceGUI::setactuator4_23()
{
    thecircle=4; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 23);
}
void SRTActiveSurfaceGUI::setactuator4_24()
{
    thecircle=4; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 24);
}
void SRTActiveSurfaceGUI::setactuator4_25()
{
    thecircle=4; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 25);
}
void SRTActiveSurfaceGUI::setactuator4_26()
{
    thecircle=4; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 26);
}
void SRTActiveSurfaceGUI::setactuator4_27()
{
    thecircle=4; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 27);
}
void SRTActiveSurfaceGUI::setactuator4_28()
{
    thecircle=4; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 28);
}
void SRTActiveSurfaceGUI::setactuator4_29()
{
    thecircle=4; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 29);
}
void SRTActiveSurfaceGUI::setactuator4_30()
{
    thecircle=4; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 30);
}
void SRTActiveSurfaceGUI::setactuator4_31()
{
    thecircle=4; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 31);
}
void SRTActiveSurfaceGUI::setactuator4_32()
{
    thecircle=4; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 32);
}
void SRTActiveSurfaceGUI::setactuator4_33()
{
    thecircle=4; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 33);
}
void SRTActiveSurfaceGUI::setactuator4_34()
{
    thecircle=4; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 34);
}
void SRTActiveSurfaceGUI::setactuator4_35()
{
    thecircle=4; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 35);
}
void SRTActiveSurfaceGUI::setactuator4_36()
{
    thecircle=4; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 36);
}
void SRTActiveSurfaceGUI::setactuator4_37()
{
    thecircle=4; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 37);
}
void SRTActiveSurfaceGUI::setactuator4_38()
{
    thecircle=4; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 38);
}
void SRTActiveSurfaceGUI::setactuator4_39()
{
    thecircle=4; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 39);
}
void SRTActiveSurfaceGUI::setactuator4_40()
{
    thecircle=4; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 40);
}
void SRTActiveSurfaceGUI::setactuator4_41()
{
    thecircle=4; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 41);
}
void SRTActiveSurfaceGUI::setactuator4_42()
{
    thecircle=4; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 42);
}
void SRTActiveSurfaceGUI::setactuator4_43()
{
    thecircle=4; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 43);
}
void SRTActiveSurfaceGUI::setactuator4_44()
{
    thecircle=4; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 44);
}
void SRTActiveSurfaceGUI::setactuator4_45()
{
    thecircle=4; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 45);
}
void SRTActiveSurfaceGUI::setactuator4_46()
{
    thecircle=4; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 46);
}
void SRTActiveSurfaceGUI::setactuator4_47()
{
    thecircle=4; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 47);
}
void SRTActiveSurfaceGUI::setactuator4_48()
{
    thecircle=4; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(4, 48);
}
// 5 CIRCLE
void SRTActiveSurfaceGUI::setactuator5_1()
{
    thecircle=5; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 1);
}
void SRTActiveSurfaceGUI::setactuator5_2()
{
    thecircle=5; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 2);
}
void SRTActiveSurfaceGUI::setactuator5_3()
{
    thecircle=5; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 3);
}
void SRTActiveSurfaceGUI::setactuator5_4()
{
    thecircle=5; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 4);
}
void SRTActiveSurfaceGUI::setactuator5_5()
{
    thecircle=5; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 5);
}
void SRTActiveSurfaceGUI::setactuator5_6()
{
    thecircle=5; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 6);
}
void SRTActiveSurfaceGUI::setactuator5_7()
{
    thecircle=5; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 7);
}
void SRTActiveSurfaceGUI::setactuator5_8()
{
    thecircle=5; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 8);
}
void SRTActiveSurfaceGUI::setactuator5_9()
{
   thecircle=5; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(5, 9);
}
void SRTActiveSurfaceGUI::setactuator5_10()
{
    thecircle=5; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 10);
}
void SRTActiveSurfaceGUI::setactuator5_11()
{
    thecircle=5; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 11);
}
void SRTActiveSurfaceGUI::setactuator5_12()
{
    thecircle=5; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 12);
}
void SRTActiveSurfaceGUI::setactuator5_13()
{
    thecircle=5; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 13);
}
void SRTActiveSurfaceGUI::setactuator5_14()
{
    thecircle=5; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 14);
}
void SRTActiveSurfaceGUI::setactuator5_15()
{
    thecircle=5; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 15);
}
void SRTActiveSurfaceGUI::setactuator5_16()
{
    thecircle=5; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 16);
}
void SRTActiveSurfaceGUI::setactuator5_17()
{
    thecircle=5; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 17);
}
void SRTActiveSurfaceGUI::setactuator5_18()
{
    thecircle=5; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 18);
}
void SRTActiveSurfaceGUI::setactuator5_19()
{
    thecircle=5; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 19);
}
void SRTActiveSurfaceGUI::setactuator5_20()
{
    thecircle=5; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 20);
}
void SRTActiveSurfaceGUI::setactuator5_21()
{
    thecircle=5; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 21);
}
void SRTActiveSurfaceGUI::setactuator5_22()
{
    thecircle=5; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 22);
}
void SRTActiveSurfaceGUI::setactuator5_23()
{
    thecircle=5; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 23);
}
void SRTActiveSurfaceGUI::setactuator5_24()
{
    thecircle=5; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 24);
}
void SRTActiveSurfaceGUI::setactuator5_25()
{
    thecircle=5; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 25);
}
void SRTActiveSurfaceGUI::setactuator5_26()
{
    thecircle=5; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 26);
}
void SRTActiveSurfaceGUI::setactuator5_27()
{
    thecircle=5; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 27);
}
void SRTActiveSurfaceGUI::setactuator5_28()
{
    thecircle=5; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 28);
}
void SRTActiveSurfaceGUI::setactuator5_29()
{
    thecircle=5; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 29);
}
void SRTActiveSurfaceGUI::setactuator5_30()
{
    thecircle=5; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 30);
}
void SRTActiveSurfaceGUI::setactuator5_31()
{
    thecircle=5; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 31);
}
void SRTActiveSurfaceGUI::setactuator5_32()
{
    thecircle=5; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 32);
}
void SRTActiveSurfaceGUI::setactuator5_33()
{
    thecircle=5; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 33);
}
void SRTActiveSurfaceGUI::setactuator5_34()
{
    thecircle=5; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 34);
}
void SRTActiveSurfaceGUI::setactuator5_35()
{
    thecircle=5; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 35);
}
void SRTActiveSurfaceGUI::setactuator5_36()
{
    thecircle=5; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 36);
}
void SRTActiveSurfaceGUI::setactuator5_37()
{
    thecircle=5; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 37);
}
void SRTActiveSurfaceGUI::setactuator5_38()
{
    thecircle=5; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 38);
}
void SRTActiveSurfaceGUI::setactuator5_39()
{
    thecircle=5; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 39);
}
void SRTActiveSurfaceGUI::setactuator5_40()
{
    thecircle=5; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 40);
}
void SRTActiveSurfaceGUI::setactuator5_41()
{
    thecircle=5; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 41);
}
void SRTActiveSurfaceGUI::setactuator5_42()
{
    thecircle=5; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 42);
}
void SRTActiveSurfaceGUI::setactuator5_43()
{
    thecircle=5; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 43);
}
void SRTActiveSurfaceGUI::setactuator5_44()
{
    thecircle=5; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 44);
}
void SRTActiveSurfaceGUI::setactuator5_45()
{
    thecircle=5; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 45);
}
void SRTActiveSurfaceGUI::setactuator5_46()
{
    thecircle=5; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 46);
}
void SRTActiveSurfaceGUI::setactuator5_47()
{
    thecircle=5; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 47);
}
void SRTActiveSurfaceGUI::setactuator5_48()
{
    thecircle=5; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(5, 48);
}
// 6 CIRCLE
void SRTActiveSurfaceGUI::setactuator6_1()
{
    thecircle=6; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 1);
}
void SRTActiveSurfaceGUI::setactuator6_2()
{
    thecircle=6; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 2);
}
void SRTActiveSurfaceGUI::setactuator6_3()
{
    thecircle=6; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 3);
}
void SRTActiveSurfaceGUI::setactuator6_4()
{
    thecircle=6; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 4);
}
void SRTActiveSurfaceGUI::setactuator6_5()
{
    thecircle=6; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 5);
}
void SRTActiveSurfaceGUI::setactuator6_6()
{
    thecircle=6; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 6);
}
void SRTActiveSurfaceGUI::setactuator6_7()
{
    thecircle=6; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 7);
}
void SRTActiveSurfaceGUI::setactuator6_8()
{
    thecircle=6; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 8);
}
void SRTActiveSurfaceGUI::setactuator6_9()
{
   thecircle=6; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(6, 9);
}
void SRTActiveSurfaceGUI::setactuator6_10()
{
    thecircle=6; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 10);
}
void SRTActiveSurfaceGUI::setactuator6_11()
{
    thecircle=6; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 11);
}
void SRTActiveSurfaceGUI::setactuator6_12()
{
    thecircle=6; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 12);
}
void SRTActiveSurfaceGUI::setactuator6_13()
{
    thecircle=6; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 13);
}
void SRTActiveSurfaceGUI::setactuator6_14()
{
    thecircle=6; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 14);
}
void SRTActiveSurfaceGUI::setactuator6_15()
{
    thecircle=6; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 15);
}
void SRTActiveSurfaceGUI::setactuator6_16()
{
    thecircle=6; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 16);
}
void SRTActiveSurfaceGUI::setactuator6_17()
{
    thecircle=6; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 17);
}
void SRTActiveSurfaceGUI::setactuator6_18()
{
    thecircle=6; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 18);
}
void SRTActiveSurfaceGUI::setactuator6_19()
{
    thecircle=6; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 19);
}
void SRTActiveSurfaceGUI::setactuator6_20()
{
    thecircle=6; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 20);
}
void SRTActiveSurfaceGUI::setactuator6_21()
{
    thecircle=6; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 21);
}
void SRTActiveSurfaceGUI::setactuator6_22()
{
    thecircle=6; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 22);
}
void SRTActiveSurfaceGUI::setactuator6_23()
{
    thecircle=6; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 23);
}
void SRTActiveSurfaceGUI::setactuator6_24()
{
    thecircle=6; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 24);
}
void SRTActiveSurfaceGUI::setactuator6_25()
{
    thecircle=6; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 25);
}
void SRTActiveSurfaceGUI::setactuator6_26()
{
    thecircle=6; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 26);
}
void SRTActiveSurfaceGUI::setactuator6_27()
{
    thecircle=6; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 27);
}
void SRTActiveSurfaceGUI::setactuator6_28()
{
    thecircle=6; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 28);
}
void SRTActiveSurfaceGUI::setactuator6_29()
{
    thecircle=6; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 29);
}
void SRTActiveSurfaceGUI::setactuator6_30()
{
    thecircle=6; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 30);
}
void SRTActiveSurfaceGUI::setactuator6_31()
{
    thecircle=6; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 31);
}
void SRTActiveSurfaceGUI::setactuator6_32()
{
    thecircle=6; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 32);
}
void SRTActiveSurfaceGUI::setactuator6_33()
{
    thecircle=6; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 33);
}
void SRTActiveSurfaceGUI::setactuator6_34()
{
    thecircle=6; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 34);
}
void SRTActiveSurfaceGUI::setactuator6_35()
{
    thecircle=6; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 35);
}
void SRTActiveSurfaceGUI::setactuator6_36()
{
    thecircle=6; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 36);
}
void SRTActiveSurfaceGUI::setactuator6_37()
{
    thecircle=6; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 37);
}
void SRTActiveSurfaceGUI::setactuator6_38()
{
    thecircle=6; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 38);
}
void SRTActiveSurfaceGUI::setactuator6_39()
{
    thecircle=6; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 39);
}
void SRTActiveSurfaceGUI::setactuator6_40()
{
    thecircle=6; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 40);
}
void SRTActiveSurfaceGUI::setactuator6_41()
{
    thecircle=6; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 41);
}
void SRTActiveSurfaceGUI::setactuator6_42()
{
    thecircle=6; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 42);
}
void SRTActiveSurfaceGUI::setactuator6_43()
{
    thecircle=6; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 43);
}
void SRTActiveSurfaceGUI::setactuator6_44()
{
    thecircle=6; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 44);
}
void SRTActiveSurfaceGUI::setactuator6_45()
{
    thecircle=6; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 45);
}
void SRTActiveSurfaceGUI::setactuator6_46()
{
    thecircle=6; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 46);
}
void SRTActiveSurfaceGUI::setactuator6_47()
{
    thecircle=6; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 47);
}
void SRTActiveSurfaceGUI::setactuator6_48()
{
    thecircle=6; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(6, 48);
}
// 7 CIRCLE
void SRTActiveSurfaceGUI::setactuator7_1()
{
    thecircle=7; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 1);
}
void SRTActiveSurfaceGUI::setactuator7_2()
{
    thecircle=7; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 2);
}
void SRTActiveSurfaceGUI::setactuator7_3()
{
    thecircle=7; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 3);
}
void SRTActiveSurfaceGUI::setactuator7_4()
{
    thecircle=7; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 4);
}
void SRTActiveSurfaceGUI::setactuator7_5()
{
    thecircle=7; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 5);
}
void SRTActiveSurfaceGUI::setactuator7_6()
{
    thecircle=7; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 6);
}
void SRTActiveSurfaceGUI::setactuator7_7()
{
    thecircle=7; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 7);
}
void SRTActiveSurfaceGUI::setactuator7_8()
{
    thecircle=7; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 8);
}
void SRTActiveSurfaceGUI::setactuator7_9()
{
   thecircle=7; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(7, 9);
}
void SRTActiveSurfaceGUI::setactuator7_10()
{
    thecircle=7; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 10);
}
void SRTActiveSurfaceGUI::setactuator7_11()
{
    thecircle=7; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 11);
}
void SRTActiveSurfaceGUI::setactuator7_12()
{
    thecircle=7; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 12);
}
void SRTActiveSurfaceGUI::setactuator7_13()
{
    thecircle=7; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 13);
}
void SRTActiveSurfaceGUI::setactuator7_14()
{
    thecircle=7; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 14);
}
void SRTActiveSurfaceGUI::setactuator7_15()
{
    thecircle=7; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 15);
}
void SRTActiveSurfaceGUI::setactuator7_16()
{
    thecircle=7; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 16);
}
void SRTActiveSurfaceGUI::setactuator7_17()
{
    thecircle=7; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 17);
}
void SRTActiveSurfaceGUI::setactuator7_18()
{
    thecircle=7; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 18);
}
void SRTActiveSurfaceGUI::setactuator7_19()
{
    thecircle=7; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 19);
}
void SRTActiveSurfaceGUI::setactuator7_20()
{
    thecircle=7; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 20);
}
void SRTActiveSurfaceGUI::setactuator7_21()
{
    thecircle=7; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 21);
}
void SRTActiveSurfaceGUI::setactuator7_22()
{
    thecircle=7; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 22);
}
void SRTActiveSurfaceGUI::setactuator7_23()
{
    thecircle=7; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 23);
}
void SRTActiveSurfaceGUI::setactuator7_24()
{
    thecircle=7; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 24);
}
void SRTActiveSurfaceGUI::setactuator7_25()
{
    thecircle=7; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 25);
}
void SRTActiveSurfaceGUI::setactuator7_26()
{
    thecircle=7; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 26);
}
void SRTActiveSurfaceGUI::setactuator7_27()
{
    thecircle=7; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 27);
}
void SRTActiveSurfaceGUI::setactuator7_28()
{
    thecircle=7; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 28);
}
void SRTActiveSurfaceGUI::setactuator7_29()
{
    thecircle=7; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 29);
}
void SRTActiveSurfaceGUI::setactuator7_30()
{
    thecircle=7; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 30);
}
void SRTActiveSurfaceGUI::setactuator7_31()
{
    thecircle=7; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 31);
}
void SRTActiveSurfaceGUI::setactuator7_32()
{
    thecircle=7; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 32);
}
void SRTActiveSurfaceGUI::setactuator7_33()
{
    thecircle=7; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 33);
}
void SRTActiveSurfaceGUI::setactuator7_34()
{
    thecircle=7; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 34);
}
void SRTActiveSurfaceGUI::setactuator7_35()
{
    thecircle=7; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 35);
}
void SRTActiveSurfaceGUI::setactuator7_36()
{
    thecircle=7; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 36);
}
void SRTActiveSurfaceGUI::setactuator7_37()
{
    thecircle=7; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 37);
}
void SRTActiveSurfaceGUI::setactuator7_38()
{
    thecircle=7; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 38);
}
void SRTActiveSurfaceGUI::setactuator7_39()
{
    thecircle=7; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 39);
}
void SRTActiveSurfaceGUI::setactuator7_40()
{
    thecircle=7; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 40);
}
void SRTActiveSurfaceGUI::setactuator7_41()
{
    thecircle=7; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 41);
}
void SRTActiveSurfaceGUI::setactuator7_42()
{
    thecircle=7; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 42);
}
void SRTActiveSurfaceGUI::setactuator7_43()
{
    thecircle=7; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 43);
}
void SRTActiveSurfaceGUI::setactuator7_44()
{
    thecircle=7; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 44);
}
void SRTActiveSurfaceGUI::setactuator7_45()
{
    thecircle=7; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 45);
}
void SRTActiveSurfaceGUI::setactuator7_46()
{
    thecircle=7; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 46);
}
void SRTActiveSurfaceGUI::setactuator7_47()
{
    thecircle=7; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 47);
}
void SRTActiveSurfaceGUI::setactuator7_48()
{
    thecircle=7; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 48);
}
void SRTActiveSurfaceGUI::setactuator7_49()
{
    thecircle=7; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 49);
}
void SRTActiveSurfaceGUI::setactuator7_50()
{
    thecircle=7; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 50);
}
void SRTActiveSurfaceGUI::setactuator7_51()
{
    thecircle=7; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 51);
}
void SRTActiveSurfaceGUI::setactuator7_52()
{
    thecircle=7; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 52);
}
void SRTActiveSurfaceGUI::setactuator7_53()
{
    thecircle=7; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 53);
}
void SRTActiveSurfaceGUI::setactuator7_54()
{
    thecircle=7; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 54);
}
void SRTActiveSurfaceGUI::setactuator7_55()
{
    thecircle=7; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 55);
}
void SRTActiveSurfaceGUI::setactuator7_56()
{
    thecircle=7; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 56);
}
void SRTActiveSurfaceGUI::setactuator7_57()
{
    thecircle=7; theactuator=57; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 57);
}
void SRTActiveSurfaceGUI::setactuator7_58()
{
    thecircle=7; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 58);
}
void SRTActiveSurfaceGUI::setactuator7_59()
{
    thecircle=7; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 59);
}
void SRTActiveSurfaceGUI::setactuator7_60()
{
    thecircle=7; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 60);
}
void SRTActiveSurfaceGUI::setactuator7_61()
{
    thecircle=7; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 61);
}
void SRTActiveSurfaceGUI::setactuator7_62()
{
    thecircle=7; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 62);
}
void SRTActiveSurfaceGUI::setactuator7_63()
{
    thecircle=7; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 63);
}
void SRTActiveSurfaceGUI::setactuator7_64()
{
    thecircle=7; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 64);
}
void SRTActiveSurfaceGUI::setactuator7_65()
{
    thecircle=7; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 65);
}
void SRTActiveSurfaceGUI::setactuator7_66()
{
    thecircle=7; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 66);
}
void SRTActiveSurfaceGUI::setactuator7_67()
{
    thecircle=7; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 67);
}
void SRTActiveSurfaceGUI::setactuator7_68()
{
    thecircle=7; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 68);
}
void SRTActiveSurfaceGUI::setactuator7_69()
{
    thecircle=7; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 69);
}
void SRTActiveSurfaceGUI::setactuator7_70()
{
    thecircle=7; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 70);
}
void SRTActiveSurfaceGUI::setactuator7_71()
{
    thecircle=7; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 71);
}
void SRTActiveSurfaceGUI::setactuator7_72()
{
    thecircle=7; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 72);
}
void SRTActiveSurfaceGUI::setactuator7_73()
{
    thecircle=7; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 73);
}
void SRTActiveSurfaceGUI::setactuator7_74()
{
    thecircle=7; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 74);
}
void SRTActiveSurfaceGUI::setactuator7_75()
{
    thecircle=7; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 75);
}
void SRTActiveSurfaceGUI::setactuator7_76()
{
    thecircle=7; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 76);
}
void SRTActiveSurfaceGUI::setactuator7_77()
{
    thecircle=7; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 77);
}
void SRTActiveSurfaceGUI::setactuator7_78()
{
    thecircle=7; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 78);
}
void SRTActiveSurfaceGUI::setactuator7_79()
{
    thecircle=7; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 79);
}
void SRTActiveSurfaceGUI::setactuator7_80()
{
    thecircle=7; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 80);
}
void SRTActiveSurfaceGUI::setactuator7_81()
{
    thecircle=7; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 81);
}
void SRTActiveSurfaceGUI::setactuator7_82()
{
    thecircle=7; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 82);
}
void SRTActiveSurfaceGUI::setactuator7_83()
{
    thecircle=7; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 83);
}
void SRTActiveSurfaceGUI::setactuator7_84()
{
    thecircle=7; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 84);
}
void SRTActiveSurfaceGUI::setactuator7_85()
{
    thecircle=7; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 85);
}
void SRTActiveSurfaceGUI::setactuator7_86()
{
    thecircle=7; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 86);
}
void SRTActiveSurfaceGUI::setactuator7_87()
{
    thecircle=7; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 87);
}
void SRTActiveSurfaceGUI::setactuator7_88()
{
    thecircle=7; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 88);
}
void SRTActiveSurfaceGUI::setactuator7_89()
{
    thecircle=7; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 89);
}
void SRTActiveSurfaceGUI::setactuator7_90()
{
    thecircle=7; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 90);
}
void SRTActiveSurfaceGUI::setactuator7_91()
{
    thecircle=7; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 91);
}
void SRTActiveSurfaceGUI::setactuator7_92()
{
    thecircle=7; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 92);
}
void SRTActiveSurfaceGUI::setactuator7_93()
{
    thecircle=7; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 93);
}
void SRTActiveSurfaceGUI::setactuator7_94()
{
    thecircle=7; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 94);
}
void SRTActiveSurfaceGUI::setactuator7_95()
{
    thecircle=7; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 95);
}
void SRTActiveSurfaceGUI::setactuator7_96()
{
    thecircle=7; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(7, 96);
}
// 8 CIRCLE
void SRTActiveSurfaceGUI::setactuator8_1()
{
    thecircle=8; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 1);
}
void SRTActiveSurfaceGUI::setactuator8_2()
{
    thecircle=8; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 2);
}
void SRTActiveSurfaceGUI::setactuator8_3()
{
    thecircle=8; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 3);
}
void SRTActiveSurfaceGUI::setactuator8_4()
{
    thecircle=8; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 4);
}
void SRTActiveSurfaceGUI::setactuator8_5()
{
    thecircle=8; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 5);
}
void SRTActiveSurfaceGUI::setactuator8_6()
{
    thecircle=8; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 6);
}
void SRTActiveSurfaceGUI::setactuator8_7()
{
    thecircle=8; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 7);
}
void SRTActiveSurfaceGUI::setactuator8_8()
{
    thecircle=8; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 8);
}
void SRTActiveSurfaceGUI::setactuator8_9()
{
   thecircle=8; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(8, 9);
}
void SRTActiveSurfaceGUI::setactuator8_10()
{
    thecircle=8; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 10);
}
void SRTActiveSurfaceGUI::setactuator8_11()
{
    thecircle=8; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 11);
}
void SRTActiveSurfaceGUI::setactuator8_12()
{
    thecircle=8; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 12);
}
void SRTActiveSurfaceGUI::setactuator8_13()
{
    thecircle=8; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 13);
}
void SRTActiveSurfaceGUI::setactuator8_14()
{
    thecircle=8; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 14);
}
void SRTActiveSurfaceGUI::setactuator8_15()
{
    thecircle=8; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 15);
}
void SRTActiveSurfaceGUI::setactuator8_16()
{
    thecircle=8; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 16);
}
void SRTActiveSurfaceGUI::setactuator8_17()
{
    thecircle=8; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 17);
}
void SRTActiveSurfaceGUI::setactuator8_18()
{
    thecircle=8; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 18);
}
void SRTActiveSurfaceGUI::setactuator8_19()
{
    thecircle=8; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 19);
}
void SRTActiveSurfaceGUI::setactuator8_20()
{
    thecircle=8; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 20);
}
void SRTActiveSurfaceGUI::setactuator8_21()
{
    thecircle=8; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 21);
}
void SRTActiveSurfaceGUI::setactuator8_22()
{
    thecircle=8; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 22);
}
void SRTActiveSurfaceGUI::setactuator8_23()
{
    thecircle=8; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 23);
}
void SRTActiveSurfaceGUI::setactuator8_24()
{
    thecircle=8; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 24);
}
void SRTActiveSurfaceGUI::setactuator8_25()
{
    thecircle=8; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 25);
}
void SRTActiveSurfaceGUI::setactuator8_26()
{
    thecircle=8; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 26);
}
void SRTActiveSurfaceGUI::setactuator8_27()
{
    thecircle=8; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 27);
}
void SRTActiveSurfaceGUI::setactuator8_28()
{
    thecircle=8; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 28);
}
void SRTActiveSurfaceGUI::setactuator8_29()
{
    thecircle=8; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 29);
}
void SRTActiveSurfaceGUI::setactuator8_30()
{
    thecircle=8; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 30);
}
void SRTActiveSurfaceGUI::setactuator8_31()
{
    thecircle=8; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 31);
}
void SRTActiveSurfaceGUI::setactuator8_32()
{
    thecircle=8; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 32);
}
void SRTActiveSurfaceGUI::setactuator8_33()
{
    thecircle=8; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 33);
}
void SRTActiveSurfaceGUI::setactuator8_34()
{
    thecircle=8; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 34);
}
void SRTActiveSurfaceGUI::setactuator8_35()
{
    thecircle=8; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 35);
}
void SRTActiveSurfaceGUI::setactuator8_36()
{
    thecircle=8; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 36);
}
void SRTActiveSurfaceGUI::setactuator8_37()
{
    thecircle=8; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 37);
}
void SRTActiveSurfaceGUI::setactuator8_38()
{
    thecircle=8; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 38);
}
void SRTActiveSurfaceGUI::setactuator8_39()
{
    thecircle=8; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 39);
}
void SRTActiveSurfaceGUI::setactuator8_40()
{
    thecircle=8; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 40);
}
void SRTActiveSurfaceGUI::setactuator8_41()
{
    thecircle=8; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 41);
}
void SRTActiveSurfaceGUI::setactuator8_42()
{
    thecircle=8; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 42);
}
void SRTActiveSurfaceGUI::setactuator8_43()
{
    thecircle=8; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 43);
}
void SRTActiveSurfaceGUI::setactuator8_44()
{
    thecircle=8; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 44);
}
void SRTActiveSurfaceGUI::setactuator8_45()
{
    thecircle=8; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 45);
}
void SRTActiveSurfaceGUI::setactuator8_46()
{
    thecircle=8; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 46);
}
void SRTActiveSurfaceGUI::setactuator8_47()
{
    thecircle=8; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 47);
}
void SRTActiveSurfaceGUI::setactuator8_48()
{
    thecircle=8; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 48);
}
void SRTActiveSurfaceGUI::setactuator8_49()
{
    thecircle=8; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 49);
}
void SRTActiveSurfaceGUI::setactuator8_50()
{
    thecircle=8; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 50);
}
void SRTActiveSurfaceGUI::setactuator8_51()
{
    thecircle=8; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 51);
}
void SRTActiveSurfaceGUI::setactuator8_52()
{
    thecircle=8; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 52);
}
void SRTActiveSurfaceGUI::setactuator8_53()
{
    thecircle=8; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 53);
}
void SRTActiveSurfaceGUI::setactuator8_54()
{
    thecircle=8; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 54);
}
void SRTActiveSurfaceGUI::setactuator8_55()
{
    thecircle=8; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 55);
}
void SRTActiveSurfaceGUI::setactuator8_56()
{
    thecircle=8; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 56);
}
void SRTActiveSurfaceGUI::setactuator8_57()
{
   thecircle=8; theactuator=57; theradius=0;
   mySRTActiveSurfaceCore.setactuator(8, 57);
}
void SRTActiveSurfaceGUI::setactuator8_58()
{
    thecircle=8; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 58);
}
void SRTActiveSurfaceGUI::setactuator8_59()
{
    thecircle=8; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 59);
}
void SRTActiveSurfaceGUI::setactuator8_60()
{
    thecircle=8; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 60);
}
void SRTActiveSurfaceGUI::setactuator8_61()
{
    thecircle=8; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 61);
}
void SRTActiveSurfaceGUI::setactuator8_62()
{
    thecircle=8; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 62);
}
void SRTActiveSurfaceGUI::setactuator8_63()
{
    thecircle=8; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 63);
}
void SRTActiveSurfaceGUI::setactuator8_64()
{
    thecircle=8; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 64);
}
void SRTActiveSurfaceGUI::setactuator8_65()
{
    thecircle=8; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 65);
}
void SRTActiveSurfaceGUI::setactuator8_66()
{
    thecircle=8; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 66);
}
void SRTActiveSurfaceGUI::setactuator8_67()
{
    thecircle=8; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 67);
}
void SRTActiveSurfaceGUI::setactuator8_68()
{
    thecircle=8; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 68);
}
void SRTActiveSurfaceGUI::setactuator8_69()
{
    thecircle=8; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 69);
}
void SRTActiveSurfaceGUI::setactuator8_70()
{
    thecircle=8; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 70);
}
void SRTActiveSurfaceGUI::setactuator8_71()
{
    thecircle=8; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 71);
}
void SRTActiveSurfaceGUI::setactuator8_72()
{
    thecircle=8; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 72);
}
void SRTActiveSurfaceGUI::setactuator8_73()
{
    thecircle=8; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 73);
}
void SRTActiveSurfaceGUI::setactuator8_74()
{
    thecircle=8; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 74);
}
void SRTActiveSurfaceGUI::setactuator8_75()
{
    thecircle=8; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 75);
}
void SRTActiveSurfaceGUI::setactuator8_76()
{
    thecircle=8; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 76);
}
void SRTActiveSurfaceGUI::setactuator8_77()
{
    thecircle=8; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 77);
}
void SRTActiveSurfaceGUI::setactuator8_78()
{
    thecircle=8; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 78);
}
void SRTActiveSurfaceGUI::setactuator8_79()
{
    thecircle=8; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 79);
}
void SRTActiveSurfaceGUI::setactuator8_80()
{
    thecircle=8; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 80);
}
void SRTActiveSurfaceGUI::setactuator8_81()
{
    thecircle=8; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 81);
}
void SRTActiveSurfaceGUI::setactuator8_82()
{
    thecircle=8; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 82);
}
void SRTActiveSurfaceGUI::setactuator8_83()
{
    thecircle=8; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 83);
}
void SRTActiveSurfaceGUI::setactuator8_84()
{
    thecircle=8; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 84);
}
void SRTActiveSurfaceGUI::setactuator8_85()
{
    thecircle=8; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 85);
}
void SRTActiveSurfaceGUI::setactuator8_86()
{
    thecircle=8; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 86);
}
void SRTActiveSurfaceGUI::setactuator8_87()
{
    thecircle=8; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 87);
}
void SRTActiveSurfaceGUI::setactuator8_88()
{
    thecircle=8; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 88);
}
void SRTActiveSurfaceGUI::setactuator8_89()
{
    thecircle=8; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 89);
}
void SRTActiveSurfaceGUI::setactuator8_90()
{
    thecircle=8; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 90);
}
void SRTActiveSurfaceGUI::setactuator8_91()
{
    thecircle=8; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 91);
}
void SRTActiveSurfaceGUI::setactuator8_92()
{
    thecircle=8; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 92);
}
void SRTActiveSurfaceGUI::setactuator8_93()
{
    thecircle=8; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 93);
}
void SRTActiveSurfaceGUI::setactuator8_94()
{
    thecircle=8; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 94);
}
void SRTActiveSurfaceGUI::setactuator8_95()
{
    thecircle=8; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 95);
}
void SRTActiveSurfaceGUI::setactuator8_96()
{
    thecircle=8; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(8, 96);
}
// 9 CIRCLE
void SRTActiveSurfaceGUI::setactuator9_1()
{
    thecircle=9; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 1);
}
void SRTActiveSurfaceGUI::setactuator9_2()
{
    thecircle=9; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 2);
}
void SRTActiveSurfaceGUI::setactuator9_3()
{
    thecircle=9; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 3);
}
void SRTActiveSurfaceGUI::setactuator9_4()
{
    thecircle=9; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 4);
}
void SRTActiveSurfaceGUI::setactuator9_5()
{
    thecircle=9; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 5);
}
void SRTActiveSurfaceGUI::setactuator9_6()
{
    thecircle=9; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 6);
}
void SRTActiveSurfaceGUI::setactuator9_7()
{
    thecircle=9; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 7);
}
void SRTActiveSurfaceGUI::setactuator9_8()
{
    thecircle=9; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 8);
}
void SRTActiveSurfaceGUI::setactuator9_9()
{
   thecircle=9; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(9, 9);
}
void SRTActiveSurfaceGUI::setactuator9_10()
{
    thecircle=9; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 10);
}
void SRTActiveSurfaceGUI::setactuator9_11()
{
    thecircle=9; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 11);
}
void SRTActiveSurfaceGUI::setactuator9_12()
{
    thecircle=9; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 12);
}
void SRTActiveSurfaceGUI::setactuator9_13()
{
    thecircle=9; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 13);
}
void SRTActiveSurfaceGUI::setactuator9_14()
{
    thecircle=9; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 14);
}
void SRTActiveSurfaceGUI::setactuator9_15()
{
    thecircle=9; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 15);
}
void SRTActiveSurfaceGUI::setactuator9_16()
{
    thecircle=9; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 16);
}
void SRTActiveSurfaceGUI::setactuator9_17()
{
    thecircle=9; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 17);
}
void SRTActiveSurfaceGUI::setactuator9_18()
{
    thecircle=9; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 18);
}
void SRTActiveSurfaceGUI::setactuator9_19()
{
    thecircle=9; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 19);
}
void SRTActiveSurfaceGUI::setactuator9_20()
{
    thecircle=9; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 20);
}
void SRTActiveSurfaceGUI::setactuator9_21()
{
    thecircle=9; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 21);
}
void SRTActiveSurfaceGUI::setactuator9_22()
{
    thecircle=9; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 22);
}
void SRTActiveSurfaceGUI::setactuator9_23()
{
    thecircle=9; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 23);
}
void SRTActiveSurfaceGUI::setactuator9_24()
{
    thecircle=9; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 24);
}
void SRTActiveSurfaceGUI::setactuator9_25()
{
    thecircle=9; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 25);
}
void SRTActiveSurfaceGUI::setactuator9_26()
{
    thecircle=9; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 26);
}
void SRTActiveSurfaceGUI::setactuator9_27()
{
    thecircle=9; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 27);
}
void SRTActiveSurfaceGUI::setactuator9_28()
{
    thecircle=9; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 28);
}
void SRTActiveSurfaceGUI::setactuator9_29()
{
    thecircle=9; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 29);
}
void SRTActiveSurfaceGUI::setactuator9_30()
{
    thecircle=9; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 30);
}
void SRTActiveSurfaceGUI::setactuator9_31()
{
    thecircle=9; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 31);
}
void SRTActiveSurfaceGUI::setactuator9_32()
{
    thecircle=9; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 32);
}
void SRTActiveSurfaceGUI::setactuator9_33()
{
    thecircle=9; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 33);
}
void SRTActiveSurfaceGUI::setactuator9_34()
{
    thecircle=9; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 34);
}
void SRTActiveSurfaceGUI::setactuator9_35()
{
    thecircle=9; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 35);
}
void SRTActiveSurfaceGUI::setactuator9_36()
{
    thecircle=9; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 36);
}
void SRTActiveSurfaceGUI::setactuator9_37()
{
    thecircle=9; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 37);
}
void SRTActiveSurfaceGUI::setactuator9_38()
{
    thecircle=9; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 38);
}
void SRTActiveSurfaceGUI::setactuator9_39()
{
    thecircle=9; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 39);
}
void SRTActiveSurfaceGUI::setactuator9_40()
{
    thecircle=9; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 40);
}
void SRTActiveSurfaceGUI::setactuator9_41()
{
    thecircle=9; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 41);
}
void SRTActiveSurfaceGUI::setactuator9_42()
{
    thecircle=9; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 42);
}
void SRTActiveSurfaceGUI::setactuator9_43()
{
    thecircle=9; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 43);
}
void SRTActiveSurfaceGUI::setactuator9_44()
{
    thecircle=9; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 44);
}
void SRTActiveSurfaceGUI::setactuator9_45()
{
    thecircle=9; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 45);
}
void SRTActiveSurfaceGUI::setactuator9_46()
{
    thecircle=9; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 46);
}
void SRTActiveSurfaceGUI::setactuator9_47()
{
    thecircle=9; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 47);
}
void SRTActiveSurfaceGUI::setactuator9_48()
{
    thecircle=9; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 48);
}
void SRTActiveSurfaceGUI::setactuator9_49()
{
    thecircle=9; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 49);
}
void SRTActiveSurfaceGUI::setactuator9_50()
{
    thecircle=9; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 50);
}
void SRTActiveSurfaceGUI::setactuator9_51()
{
    thecircle=9; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 51);
}
void SRTActiveSurfaceGUI::setactuator9_52()
{
    thecircle=9; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 52);
}
void SRTActiveSurfaceGUI::setactuator9_53()
{
    thecircle=9; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 53);
}
void SRTActiveSurfaceGUI::setactuator9_54()
{
    thecircle=9; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 54);
}
void SRTActiveSurfaceGUI::setactuator9_55()
{
    thecircle=9; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 55);
}
void SRTActiveSurfaceGUI::setactuator9_56()
{
    thecircle=9; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 56);
}
void SRTActiveSurfaceGUI::setactuator9_57()
{
   thecircle=9; theactuator=57; theradius=0;
   mySRTActiveSurfaceCore.setactuator(9, 57);
}
void SRTActiveSurfaceGUI::setactuator9_58()
{
    thecircle=9; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 58);
}
void SRTActiveSurfaceGUI::setactuator9_59()
{
    thecircle=9; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 59);
}
void SRTActiveSurfaceGUI::setactuator9_60()
{
    thecircle=9; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 60);
}
void SRTActiveSurfaceGUI::setactuator9_61()
{
    thecircle=9; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 61);
}
void SRTActiveSurfaceGUI::setactuator9_62()
{
    thecircle=9; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 62);
}
void SRTActiveSurfaceGUI::setactuator9_63()
{
    thecircle=9; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 63);
}
void SRTActiveSurfaceGUI::setactuator9_64()
{
    thecircle=9; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 64);
}
void SRTActiveSurfaceGUI::setactuator9_65()
{
    thecircle=9; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 65);
}
void SRTActiveSurfaceGUI::setactuator9_66()
{
    thecircle=9; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 66);
}
void SRTActiveSurfaceGUI::setactuator9_67()
{
    thecircle=9; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 67);
}
void SRTActiveSurfaceGUI::setactuator9_68()
{
    thecircle=9; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 68);
}
void SRTActiveSurfaceGUI::setactuator9_69()
{
    thecircle=9; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 69);
}
void SRTActiveSurfaceGUI::setactuator9_70()
{
    thecircle=9; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 70);
}
void SRTActiveSurfaceGUI::setactuator9_71()
{
    thecircle=9; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 71);
}
void SRTActiveSurfaceGUI::setactuator9_72()
{
    thecircle=9; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 72);
}
void SRTActiveSurfaceGUI::setactuator9_73()
{
    thecircle=9; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 73);
}
void SRTActiveSurfaceGUI::setactuator9_74()
{
    thecircle=9; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 74);
}
void SRTActiveSurfaceGUI::setactuator9_75()
{
    thecircle=9; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 75);
}
void SRTActiveSurfaceGUI::setactuator9_76()
{
    thecircle=9; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 76);
}
void SRTActiveSurfaceGUI::setactuator9_77()
{
    thecircle=9; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 77);
}
void SRTActiveSurfaceGUI::setactuator9_78()
{
    thecircle=9; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 78);
}
void SRTActiveSurfaceGUI::setactuator9_79()
{
    thecircle=9; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 79);
}
void SRTActiveSurfaceGUI::setactuator9_80()
{
    thecircle=9; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 80);
}
void SRTActiveSurfaceGUI::setactuator9_81()
{
    thecircle=9; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 81);
}
void SRTActiveSurfaceGUI::setactuator9_82()
{
    thecircle=9; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 82);
}
void SRTActiveSurfaceGUI::setactuator9_83()
{
    thecircle=9; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 83);
}
void SRTActiveSurfaceGUI::setactuator9_84()
{
    thecircle=9; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 84);
}
void SRTActiveSurfaceGUI::setactuator9_85()
{
    thecircle=9; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 85);
}
void SRTActiveSurfaceGUI::setactuator9_86()
{
    thecircle=9; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 86);
}
void SRTActiveSurfaceGUI::setactuator9_87()
{
    thecircle=9; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 87);
}
void SRTActiveSurfaceGUI::setactuator9_88()
{
    thecircle=9; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 88);
}
void SRTActiveSurfaceGUI::setactuator9_89()
{
    thecircle=9; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 89);
}
void SRTActiveSurfaceGUI::setactuator9_90()
{
    thecircle=9; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 90);
}
void SRTActiveSurfaceGUI::setactuator9_91()
{
    thecircle=9; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 91);
}
void SRTActiveSurfaceGUI::setactuator9_92()
{
    thecircle=9; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 92);
}
void SRTActiveSurfaceGUI::setactuator9_93()
{
    thecircle=9; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 93);
}
void SRTActiveSurfaceGUI::setactuator9_94()
{
    thecircle=9; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 94);
}
void SRTActiveSurfaceGUI::setactuator9_95()
{
    thecircle=9; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 95);
}
void SRTActiveSurfaceGUI::setactuator9_96()
{
    thecircle=9; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(9, 96);
}
// 10 CIRCLE
void SRTActiveSurfaceGUI::setactuator10_1()
{
    thecircle=10; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 1);
}
void SRTActiveSurfaceGUI::setactuator10_2()
{
    thecircle=10; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 2);
}
void SRTActiveSurfaceGUI::setactuator10_3()
{
    thecircle=10; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 3);
}
void SRTActiveSurfaceGUI::setactuator10_4()
{
    thecircle=10; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 4);
}
void SRTActiveSurfaceGUI::setactuator10_5()
{
    thecircle=10; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 5);
}
void SRTActiveSurfaceGUI::setactuator10_6()
{
    thecircle=10; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 6);
}
void SRTActiveSurfaceGUI::setactuator10_7()
{
    thecircle=10; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 7);
}
void SRTActiveSurfaceGUI::setactuator10_8()
{
    thecircle=10; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 8);
}
void SRTActiveSurfaceGUI::setactuator10_9()
{
   thecircle=10; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(10, 9);
}
void SRTActiveSurfaceGUI::setactuator10_10()
{
    thecircle=10; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 10);
}
void SRTActiveSurfaceGUI::setactuator10_11()
{
    thecircle=10; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 11);
}
void SRTActiveSurfaceGUI::setactuator10_12()
{
    thecircle=10; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 12);
}
void SRTActiveSurfaceGUI::setactuator10_13()
{
    thecircle=10; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 13);
}
void SRTActiveSurfaceGUI::setactuator10_14()
{
    thecircle=10; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 14);
}
void SRTActiveSurfaceGUI::setactuator10_15()
{
    thecircle=10; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 15);
}
void SRTActiveSurfaceGUI::setactuator10_16()
{
    thecircle=10; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 16);
}
void SRTActiveSurfaceGUI::setactuator10_17()
{
    thecircle=10; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 17);
}
void SRTActiveSurfaceGUI::setactuator10_18()
{
    thecircle=10; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 18);
}
void SRTActiveSurfaceGUI::setactuator10_19()
{
    thecircle=10; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 19);
}
void SRTActiveSurfaceGUI::setactuator10_20()
{
    thecircle=10; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 20);
}
void SRTActiveSurfaceGUI::setactuator10_21()
{
    thecircle=10; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 21);
}
void SRTActiveSurfaceGUI::setactuator10_22()
{
    thecircle=10; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 22);
}
void SRTActiveSurfaceGUI::setactuator10_23()
{
    thecircle=10; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 23);
}
void SRTActiveSurfaceGUI::setactuator10_24()
{
    thecircle=10; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 24);
}
void SRTActiveSurfaceGUI::setactuator10_25()
{
    thecircle=10; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 25);
}
void SRTActiveSurfaceGUI::setactuator10_26()
{
    thecircle=10; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 26);
}
void SRTActiveSurfaceGUI::setactuator10_27()
{
    thecircle=10; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 27);
}
void SRTActiveSurfaceGUI::setactuator10_28()
{
    thecircle=10; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 28);
}
void SRTActiveSurfaceGUI::setactuator10_29()
{
    thecircle=10; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 29);
}
void SRTActiveSurfaceGUI::setactuator10_30()
{
    thecircle=10; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 30);
}
void SRTActiveSurfaceGUI::setactuator10_31()
{
    thecircle=10; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 31);
}
void SRTActiveSurfaceGUI::setactuator10_32()
{
    thecircle=10; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 32);
}
void SRTActiveSurfaceGUI::setactuator10_33()
{
    thecircle=10; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 33);
}
void SRTActiveSurfaceGUI::setactuator10_34()
{
    thecircle=10; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 34);
}
void SRTActiveSurfaceGUI::setactuator10_35()
{
    thecircle=10; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 35);
}
void SRTActiveSurfaceGUI::setactuator10_36()
{
    thecircle=10; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 36);
}
void SRTActiveSurfaceGUI::setactuator10_37()
{
    thecircle=10; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 37);
}
void SRTActiveSurfaceGUI::setactuator10_38()
{
    thecircle=10; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 38);
}
void SRTActiveSurfaceGUI::setactuator10_39()
{
    thecircle=10; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 39);
}
void SRTActiveSurfaceGUI::setactuator10_40()
{
    thecircle=10; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 40);
}
void SRTActiveSurfaceGUI::setactuator10_41()
{
    thecircle=10; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 41);
}
void SRTActiveSurfaceGUI::setactuator10_42()
{
    thecircle=10; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 42);
}
void SRTActiveSurfaceGUI::setactuator10_43()
{
    thecircle=10; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 43);
}
void SRTActiveSurfaceGUI::setactuator10_44()
{
    thecircle=10; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 44);
}
void SRTActiveSurfaceGUI::setactuator10_45()
{
    thecircle=10; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 45);
}
void SRTActiveSurfaceGUI::setactuator10_46()
{
    thecircle=10; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 46);
}
void SRTActiveSurfaceGUI::setactuator10_47()
{
    thecircle=10; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 47);
}
void SRTActiveSurfaceGUI::setactuator10_48()
{
    thecircle=10; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 48);
}
void SRTActiveSurfaceGUI::setactuator10_49()
{
    thecircle=10; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 49);
}
void SRTActiveSurfaceGUI::setactuator10_50()
{
    thecircle=10; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 50);
}
void SRTActiveSurfaceGUI::setactuator10_51()
{
    thecircle=10; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 51);
}
void SRTActiveSurfaceGUI::setactuator10_52()
{
    thecircle=10; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 52);
}
void SRTActiveSurfaceGUI::setactuator10_53()
{
    thecircle=10; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 53);
}
void SRTActiveSurfaceGUI::setactuator10_54()
{
    thecircle=10; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 54);
}
void SRTActiveSurfaceGUI::setactuator10_55()
{
    thecircle=10; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 55);
}
void SRTActiveSurfaceGUI::setactuator10_56()
{
    thecircle=10; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 56);
}
void SRTActiveSurfaceGUI::setactuator10_57()
{
   thecircle=10; theactuator=57; theradius=0;
   mySRTActiveSurfaceCore.setactuator(10, 57);
}
void SRTActiveSurfaceGUI::setactuator10_58()
{
    thecircle=10; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 58);
}
void SRTActiveSurfaceGUI::setactuator10_59()
{
    thecircle=10; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 59);
}
void SRTActiveSurfaceGUI::setactuator10_60()
{
    thecircle=10; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 60);
}
void SRTActiveSurfaceGUI::setactuator10_61()
{
    thecircle=10; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 61);
}
void SRTActiveSurfaceGUI::setactuator10_62()
{
    thecircle=10; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 62);
}
void SRTActiveSurfaceGUI::setactuator10_63()
{
    thecircle=10; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 63);
}
void SRTActiveSurfaceGUI::setactuator10_64()
{
    thecircle=10; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 64);
}
void SRTActiveSurfaceGUI::setactuator10_65()
{
    thecircle=10; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 65);
}
void SRTActiveSurfaceGUI::setactuator10_66()
{
    thecircle=10; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 66);
}
void SRTActiveSurfaceGUI::setactuator10_67()
{
    thecircle=10; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 67);
}
void SRTActiveSurfaceGUI::setactuator10_68()
{
    thecircle=10; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 68);
}
void SRTActiveSurfaceGUI::setactuator10_69()
{
    thecircle=10; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 69);
}
void SRTActiveSurfaceGUI::setactuator10_70()
{
    thecircle=10; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 70);
}
void SRTActiveSurfaceGUI::setactuator10_71()
{
    thecircle=10; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 71);
}
void SRTActiveSurfaceGUI::setactuator10_72()
{
    thecircle=10; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 72);
}
void SRTActiveSurfaceGUI::setactuator10_73()
{
    thecircle=10; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 73);
}
void SRTActiveSurfaceGUI::setactuator10_74()
{
    thecircle=10; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 74);
}
void SRTActiveSurfaceGUI::setactuator10_75()
{
    thecircle=10; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 75);
}
void SRTActiveSurfaceGUI::setactuator10_76()
{
    thecircle=10; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 76);
}
void SRTActiveSurfaceGUI::setactuator10_77()
{
    thecircle=10; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 77);
}
void SRTActiveSurfaceGUI::setactuator10_78()
{
    thecircle=10; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 78);
}
void SRTActiveSurfaceGUI::setactuator10_79()
{
    thecircle=10; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 79);
}
void SRTActiveSurfaceGUI::setactuator10_80()
{
    thecircle=10; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 80);
}
void SRTActiveSurfaceGUI::setactuator10_81()
{
    thecircle=10; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 81);
}
void SRTActiveSurfaceGUI::setactuator10_82()
{
    thecircle=10; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 82);
}
void SRTActiveSurfaceGUI::setactuator10_83()
{
    thecircle=10; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 83);
}
void SRTActiveSurfaceGUI::setactuator10_84()
{
    thecircle=10; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 84);
}
void SRTActiveSurfaceGUI::setactuator10_85()
{
    thecircle=10; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 85);
}
void SRTActiveSurfaceGUI::setactuator10_86()
{
    thecircle=10; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 86);
}
void SRTActiveSurfaceGUI::setactuator10_87()
{
    thecircle=10; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 87);
}
void SRTActiveSurfaceGUI::setactuator10_88()
{
    thecircle=10; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 88);
}
void SRTActiveSurfaceGUI::setactuator10_89()
{
    thecircle=10; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 89);
}
void SRTActiveSurfaceGUI::setactuator10_90()
{
    thecircle=10; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 90);
}
void SRTActiveSurfaceGUI::setactuator10_91()
{
    thecircle=10; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 91);
}
void SRTActiveSurfaceGUI::setactuator10_92()
{
    thecircle=10; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 92);
}
void SRTActiveSurfaceGUI::setactuator10_93()
{
    thecircle=10; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 93);
}
void SRTActiveSurfaceGUI::setactuator10_94()
{
    thecircle=10; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 94);
}
void SRTActiveSurfaceGUI::setactuator10_95()
{
    thecircle=10; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 95);
}
void SRTActiveSurfaceGUI::setactuator10_96()
{
    thecircle=10; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(10, 96);
}
// 11 CIRCLE
void SRTActiveSurfaceGUI::setactuator11_1()
{
    thecircle=11; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 1);
}
void SRTActiveSurfaceGUI::setactuator11_2()
{
    thecircle=11; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 2);
}
void SRTActiveSurfaceGUI::setactuator11_3()
{
    thecircle=11; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 3);
}
void SRTActiveSurfaceGUI::setactuator11_4()
{
    thecircle=11; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 4);
}
void SRTActiveSurfaceGUI::setactuator11_5()
{
    thecircle=11; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 5);
}
void SRTActiveSurfaceGUI::setactuator11_6()
{
    thecircle=11; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 6);
}
void SRTActiveSurfaceGUI::setactuator11_7()
{
    thecircle=11; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 7);
}
void SRTActiveSurfaceGUI::setactuator11_8()
{
    thecircle=11; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 8);
}
void SRTActiveSurfaceGUI::setactuator11_9()
{
   thecircle=11; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(11, 9);
}
void SRTActiveSurfaceGUI::setactuator11_10()
{
    thecircle=11; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 10);
}
void SRTActiveSurfaceGUI::setactuator11_11()
{
    thecircle=11; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 11);
}
void SRTActiveSurfaceGUI::setactuator11_12()
{
    thecircle=11; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 12);
}
void SRTActiveSurfaceGUI::setactuator11_13()
{
    thecircle=11; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 13);
}
void SRTActiveSurfaceGUI::setactuator11_14()
{
    thecircle=11; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 14);
}
void SRTActiveSurfaceGUI::setactuator11_15()
{
    thecircle=11; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 15);
}
void SRTActiveSurfaceGUI::setactuator11_16()
{
    thecircle=11; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 16);
}
void SRTActiveSurfaceGUI::setactuator11_17()
{
    thecircle=11; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 17);
}
void SRTActiveSurfaceGUI::setactuator11_18()
{
    thecircle=11; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 18);
}
void SRTActiveSurfaceGUI::setactuator11_19()
{
    thecircle=11; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 19);
}
void SRTActiveSurfaceGUI::setactuator11_20()
{
    thecircle=11; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 20);
}
void SRTActiveSurfaceGUI::setactuator11_21()
{
    thecircle=11; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 21);
}
void SRTActiveSurfaceGUI::setactuator11_22()
{
    thecircle=11; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 22);
}
void SRTActiveSurfaceGUI::setactuator11_23()
{
    thecircle=11; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 23);
}
void SRTActiveSurfaceGUI::setactuator11_24()
{
    thecircle=11; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 24);
}
void SRTActiveSurfaceGUI::setactuator11_25()
{
    thecircle=11; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 25);
}
void SRTActiveSurfaceGUI::setactuator11_26()
{
    thecircle=11; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 26);
}
void SRTActiveSurfaceGUI::setactuator11_27()
{
    thecircle=11; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 27);
}
void SRTActiveSurfaceGUI::setactuator11_28()
{
    thecircle=11; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 28);
}
void SRTActiveSurfaceGUI::setactuator11_29()
{
    thecircle=11; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 29);
}
void SRTActiveSurfaceGUI::setactuator11_30()
{
    thecircle=11; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 30);
}
void SRTActiveSurfaceGUI::setactuator11_31()
{
    thecircle=11; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 31);
}
void SRTActiveSurfaceGUI::setactuator11_32()
{
    thecircle=11; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 32);
}
void SRTActiveSurfaceGUI::setactuator11_33()
{
    thecircle=11; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 33);
}
void SRTActiveSurfaceGUI::setactuator11_34()
{
    thecircle=11; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 34);
}
void SRTActiveSurfaceGUI::setactuator11_35()
{
    thecircle=11; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 35);
}
void SRTActiveSurfaceGUI::setactuator11_36()
{
    thecircle=11; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 36);
}
void SRTActiveSurfaceGUI::setactuator11_37()
{
    thecircle=11; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 37);
}
void SRTActiveSurfaceGUI::setactuator11_38()
{
    thecircle=11; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 38);
}
void SRTActiveSurfaceGUI::setactuator11_39()
{
    thecircle=11; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 39);
}
void SRTActiveSurfaceGUI::setactuator11_40()
{
    thecircle=11; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 40);
}
void SRTActiveSurfaceGUI::setactuator11_41()
{
    thecircle=11; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 41);
}
void SRTActiveSurfaceGUI::setactuator11_42()
{
    thecircle=11; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 42);
}
void SRTActiveSurfaceGUI::setactuator11_43()
{
    thecircle=11; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 43);
}
void SRTActiveSurfaceGUI::setactuator11_44()
{
    thecircle=11; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 44);
}
void SRTActiveSurfaceGUI::setactuator11_45()
{
    thecircle=11; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 45);
}
void SRTActiveSurfaceGUI::setactuator11_46()
{
    thecircle=11; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 46);
}
void SRTActiveSurfaceGUI::setactuator11_47()
{
    thecircle=11; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 47);
}
void SRTActiveSurfaceGUI::setactuator11_48()
{
    thecircle=11; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 48);
}
void SRTActiveSurfaceGUI::setactuator11_49()
{
    thecircle=11; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 49);
}
void SRTActiveSurfaceGUI::setactuator11_50()
{
    thecircle=11; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 50);
}
void SRTActiveSurfaceGUI::setactuator11_51()
{
    thecircle=11; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 51);
}
void SRTActiveSurfaceGUI::setactuator11_52()
{
    thecircle=11; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 52);
}
void SRTActiveSurfaceGUI::setactuator11_53()
{
    thecircle=11; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 53);
}
void SRTActiveSurfaceGUI::setactuator11_54()
{
    thecircle=11; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 54);
}
void SRTActiveSurfaceGUI::setactuator11_55()
{
    thecircle=11; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 55);
}
void SRTActiveSurfaceGUI::setactuator11_56()
{
    thecircle=11; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 56);
}
void SRTActiveSurfaceGUI::setactuator11_57()
{
   thecircle=11; theactuator=57; theradius=0;
   mySRTActiveSurfaceCore.setactuator(11, 57);
}
void SRTActiveSurfaceGUI::setactuator11_58()
{
    thecircle=11; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 58);
}
void SRTActiveSurfaceGUI::setactuator11_59()
{
    thecircle=11; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 59);
}
void SRTActiveSurfaceGUI::setactuator11_60()
{
    thecircle=11; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 60);
}
void SRTActiveSurfaceGUI::setactuator11_61()
{
    thecircle=11; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 61);
}
void SRTActiveSurfaceGUI::setactuator11_62()
{
    thecircle=11; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 62);
}
void SRTActiveSurfaceGUI::setactuator11_63()
{
    thecircle=11; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 63);
}
void SRTActiveSurfaceGUI::setactuator11_64()
{
    thecircle=11; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 64);
}
void SRTActiveSurfaceGUI::setactuator11_65()
{
    thecircle=11; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 65);
}
void SRTActiveSurfaceGUI::setactuator11_66()
{
    thecircle=11; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 66);
}
void SRTActiveSurfaceGUI::setactuator11_67()
{
    thecircle=11; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 67);
}
void SRTActiveSurfaceGUI::setactuator11_68()
{
    thecircle=11; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 68);
}
void SRTActiveSurfaceGUI::setactuator11_69()
{
    thecircle=11; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 69);
}
void SRTActiveSurfaceGUI::setactuator11_70()
{
    thecircle=11; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 70);
}
void SRTActiveSurfaceGUI::setactuator11_71()
{
    thecircle=11; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 71);
}
void SRTActiveSurfaceGUI::setactuator11_72()
{
    thecircle=11; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 72);
}
void SRTActiveSurfaceGUI::setactuator11_73()
{
    thecircle=11; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 73);
}
void SRTActiveSurfaceGUI::setactuator11_74()
{
    thecircle=11; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 74);
}
void SRTActiveSurfaceGUI::setactuator11_75()
{
    thecircle=11; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 75);
}
void SRTActiveSurfaceGUI::setactuator11_76()
{
    thecircle=11; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 76);
}
void SRTActiveSurfaceGUI::setactuator11_77()
{
    thecircle=11; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 77);
}
void SRTActiveSurfaceGUI::setactuator11_78()
{
    thecircle=11; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 78);
}
void SRTActiveSurfaceGUI::setactuator11_79()
{
    thecircle=11; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 79);
}
void SRTActiveSurfaceGUI::setactuator11_80()
{
    thecircle=11; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 80);
}
void SRTActiveSurfaceGUI::setactuator11_81()
{
    thecircle=11; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 81);
}
void SRTActiveSurfaceGUI::setactuator11_82()
{
    thecircle=11; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 82);
}
void SRTActiveSurfaceGUI::setactuator11_83()
{
    thecircle=11; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 83);
}
void SRTActiveSurfaceGUI::setactuator11_84()
{
    thecircle=11; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 84);
}
void SRTActiveSurfaceGUI::setactuator11_85()
{
    thecircle=11; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 85);
}
void SRTActiveSurfaceGUI::setactuator11_86()
{
    thecircle=11; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 86);
}
void SRTActiveSurfaceGUI::setactuator11_87()
{
    thecircle=11; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 87);
}
void SRTActiveSurfaceGUI::setactuator11_88()
{
    thecircle=11; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 88);
}
void SRTActiveSurfaceGUI::setactuator11_89()
{
    thecircle=11; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 89);
}
void SRTActiveSurfaceGUI::setactuator11_90()
{
    thecircle=11; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 90);
}
void SRTActiveSurfaceGUI::setactuator11_91()
{
    thecircle=11; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 91);
}
void SRTActiveSurfaceGUI::setactuator11_92()
{
    thecircle=11; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 92);
}
void SRTActiveSurfaceGUI::setactuator11_93()
{
    thecircle=11; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 93);
}
void SRTActiveSurfaceGUI::setactuator11_94()
{
    thecircle=11; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 94);
}
void SRTActiveSurfaceGUI::setactuator11_95()
{
    thecircle=11; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 95);
}
void SRTActiveSurfaceGUI::setactuator11_96()
{
    thecircle=11; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(11, 96);
}
// 12 CIRCLE
void SRTActiveSurfaceGUI::setactuator12_1()
{
    thecircle=12; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 1);
}
void SRTActiveSurfaceGUI::setactuator12_2()
{
    thecircle=12; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 2);
}
void SRTActiveSurfaceGUI::setactuator12_3()
{
    thecircle=12; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 3);
}
void SRTActiveSurfaceGUI::setactuator12_4()
{
    thecircle=12; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 4);
}
void SRTActiveSurfaceGUI::setactuator12_5()
{
    thecircle=12; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 5);
}
void SRTActiveSurfaceGUI::setactuator12_6()
{
    thecircle=12; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 6);
}
void SRTActiveSurfaceGUI::setactuator12_7()
{
    thecircle=12; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 7);
}
void SRTActiveSurfaceGUI::setactuator12_8()
{
    thecircle=12; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 8);
}
void SRTActiveSurfaceGUI::setactuator12_9()
{
   thecircle=12; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(12, 9);
}
void SRTActiveSurfaceGUI::setactuator12_10()
{
    thecircle=12; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 10);
}
void SRTActiveSurfaceGUI::setactuator12_11()
{
    thecircle=12; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 11);
}
void SRTActiveSurfaceGUI::setactuator12_12()
{
    thecircle=12; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 12);
}
void SRTActiveSurfaceGUI::setactuator12_13()
{
    thecircle=12; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 13);
}
void SRTActiveSurfaceGUI::setactuator12_14()
{
    thecircle=12; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 14);
}
void SRTActiveSurfaceGUI::setactuator12_15()
{
    thecircle=12; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 15);
}
void SRTActiveSurfaceGUI::setactuator12_16()
{
    thecircle=12; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 16);
}
void SRTActiveSurfaceGUI::setactuator12_17()
{
    thecircle=12; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 17);
}
void SRTActiveSurfaceGUI::setactuator12_18()
{
    thecircle=12; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 18);
}
void SRTActiveSurfaceGUI::setactuator12_19()
{
    thecircle=12; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 19);
}
void SRTActiveSurfaceGUI::setactuator12_20()
{
    thecircle=12; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 20);
}
void SRTActiveSurfaceGUI::setactuator12_21()
{
    thecircle=12; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 21);
}
void SRTActiveSurfaceGUI::setactuator12_22()
{
    thecircle=12; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 22);
}
void SRTActiveSurfaceGUI::setactuator12_23()
{
    thecircle=12; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 23);
}
void SRTActiveSurfaceGUI::setactuator12_24()
{
    thecircle=12; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 24);
}
void SRTActiveSurfaceGUI::setactuator12_25()
{
    thecircle=12; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 25);
}
void SRTActiveSurfaceGUI::setactuator12_26()
{
    thecircle=12; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 26);
}
void SRTActiveSurfaceGUI::setactuator12_27()
{
    thecircle=12; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 27);
}
void SRTActiveSurfaceGUI::setactuator12_28()
{
    thecircle=12; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 28);
}
void SRTActiveSurfaceGUI::setactuator12_29()
{
    thecircle=12; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 29);
}
void SRTActiveSurfaceGUI::setactuator12_30()
{
    thecircle=12; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 30);
}
void SRTActiveSurfaceGUI::setactuator12_31()
{
    thecircle=12; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 31);
}
void SRTActiveSurfaceGUI::setactuator12_32()
{
    thecircle=12; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 32);
}
void SRTActiveSurfaceGUI::setactuator12_33()
{
    thecircle=12; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 33);
}
void SRTActiveSurfaceGUI::setactuator12_34()
{
    thecircle=12; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 34);
}
void SRTActiveSurfaceGUI::setactuator12_35()
{
    thecircle=12; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 35);
}
void SRTActiveSurfaceGUI::setactuator12_36()
{
    thecircle=12; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 36);
}
void SRTActiveSurfaceGUI::setactuator12_37()
{
    thecircle=12; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 37);
}
void SRTActiveSurfaceGUI::setactuator12_38()
{
    thecircle=12; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 38);
}
void SRTActiveSurfaceGUI::setactuator12_39()
{
    thecircle=12; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 39);
}
void SRTActiveSurfaceGUI::setactuator12_40()
{
    thecircle=12; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 40);
}
void SRTActiveSurfaceGUI::setactuator12_41()
{
    thecircle=12; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 41);
}
void SRTActiveSurfaceGUI::setactuator12_42()
{
    thecircle=12; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 42);
}
void SRTActiveSurfaceGUI::setactuator12_43()
{
    thecircle=12; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 43);
}
void SRTActiveSurfaceGUI::setactuator12_44()
{
    thecircle=12; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 44);
}
void SRTActiveSurfaceGUI::setactuator12_45()
{
    thecircle=12; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 45);
}
void SRTActiveSurfaceGUI::setactuator12_46()
{
    thecircle=12; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 46);
}
void SRTActiveSurfaceGUI::setactuator12_47()
{
    thecircle=12; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 47);
}
void SRTActiveSurfaceGUI::setactuator12_48()
{
    thecircle=12; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 48);
}
void SRTActiveSurfaceGUI::setactuator12_49()
{
    thecircle=12; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 49);
}
void SRTActiveSurfaceGUI::setactuator12_50()
{
    thecircle=12; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 50);
}
void SRTActiveSurfaceGUI::setactuator12_51()
{
    thecircle=12; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 51);
}
void SRTActiveSurfaceGUI::setactuator12_52()
{
    thecircle=12; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 52);
}
void SRTActiveSurfaceGUI::setactuator12_53()
{
    thecircle=12; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 53);
}
void SRTActiveSurfaceGUI::setactuator12_54()
{
    thecircle=12; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 54);
}
void SRTActiveSurfaceGUI::setactuator12_55()
{
    thecircle=12; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 55);
}
void SRTActiveSurfaceGUI::setactuator12_56()
{
    thecircle=12; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 56);
}
void SRTActiveSurfaceGUI::setactuator12_57()
{
   thecircle=12; theactuator=57; theradius=0;
   mySRTActiveSurfaceCore.setactuator(12, 57);
}
void SRTActiveSurfaceGUI::setactuator12_58()
{
    thecircle=12; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 58);
}
void SRTActiveSurfaceGUI::setactuator12_59()
{
    thecircle=12; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 59);
}
void SRTActiveSurfaceGUI::setactuator12_60()
{
    thecircle=12; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 60);
}
void SRTActiveSurfaceGUI::setactuator12_61()
{
    thecircle=12; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 61);
}
void SRTActiveSurfaceGUI::setactuator12_62()
{
    thecircle=12; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 62);
}
void SRTActiveSurfaceGUI::setactuator12_63()
{
    thecircle=12; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 63);
}
void SRTActiveSurfaceGUI::setactuator12_64()
{
    thecircle=12; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 64);
}
void SRTActiveSurfaceGUI::setactuator12_65()
{
    thecircle=12; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 65);
}
void SRTActiveSurfaceGUI::setactuator12_66()
{
    thecircle=12; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 66);
}
void SRTActiveSurfaceGUI::setactuator12_67()
{
    thecircle=12; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 67);
}
void SRTActiveSurfaceGUI::setactuator12_68()
{
    thecircle=12; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 68);
}
void SRTActiveSurfaceGUI::setactuator12_69()
{
    thecircle=12; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 69);
}
void SRTActiveSurfaceGUI::setactuator12_70()
{
    thecircle=12; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 70);
}
void SRTActiveSurfaceGUI::setactuator12_71()
{
    thecircle=12; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 71);
}
void SRTActiveSurfaceGUI::setactuator12_72()
{
    thecircle=12; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 72);
}
void SRTActiveSurfaceGUI::setactuator12_73()
{
    thecircle=12; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 73);
}
void SRTActiveSurfaceGUI::setactuator12_74()
{
    thecircle=12; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 74);
}
void SRTActiveSurfaceGUI::setactuator12_75()
{
    thecircle=12; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 75);
}
void SRTActiveSurfaceGUI::setactuator12_76()
{
    thecircle=12; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 76);
}
void SRTActiveSurfaceGUI::setactuator12_77()
{
    thecircle=12; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 77);
}
void SRTActiveSurfaceGUI::setactuator12_78()
{
    thecircle=12; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 78);
}
void SRTActiveSurfaceGUI::setactuator12_79()
{
    thecircle=12; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 79);
}
void SRTActiveSurfaceGUI::setactuator12_80()
{
    thecircle=12; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 80);
}
void SRTActiveSurfaceGUI::setactuator12_81()
{
    thecircle=12; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 81);
}
void SRTActiveSurfaceGUI::setactuator12_82()
{
    thecircle=12; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 82);
}
void SRTActiveSurfaceGUI::setactuator12_83()
{
    thecircle=12; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 83);
}
void SRTActiveSurfaceGUI::setactuator12_84()
{
    thecircle=12; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 84);
}
void SRTActiveSurfaceGUI::setactuator12_85()
{
    thecircle=12; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 85);
}
void SRTActiveSurfaceGUI::setactuator12_86()
{
    thecircle=12; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 86);
}
void SRTActiveSurfaceGUI::setactuator12_87()
{
    thecircle=12; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 87);
}
void SRTActiveSurfaceGUI::setactuator12_88()
{
    thecircle=12; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 88);
}
void SRTActiveSurfaceGUI::setactuator12_89()
{
    thecircle=12; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 89);
}
void SRTActiveSurfaceGUI::setactuator12_90()
{
    thecircle=12; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 90);
}
void SRTActiveSurfaceGUI::setactuator12_91()
{
    thecircle=12; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 91);
}
void SRTActiveSurfaceGUI::setactuator12_92()
{
    thecircle=12; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 92);
}
void SRTActiveSurfaceGUI::setactuator12_93()
{
    thecircle=12; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 93);
}
void SRTActiveSurfaceGUI::setactuator12_94()
{
    thecircle=12; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 94);
}
void SRTActiveSurfaceGUI::setactuator12_95()
{
    thecircle=12; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 95);
}
void SRTActiveSurfaceGUI::setactuator12_96()
{
    thecircle=12; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(12, 96);
}
// 13 CIRCLE
void SRTActiveSurfaceGUI::setactuator13_1()
{
    thecircle=13; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 1);
}
void SRTActiveSurfaceGUI::setactuator13_2()
{
    thecircle=13; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 2);
}
void SRTActiveSurfaceGUI::setactuator13_3()
{
    thecircle=13; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 3);
}
void SRTActiveSurfaceGUI::setactuator13_4()
{
    thecircle=13; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 4);
}
void SRTActiveSurfaceGUI::setactuator13_5()
{
    thecircle=13; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 5);
}
void SRTActiveSurfaceGUI::setactuator13_6()
{
    thecircle=13; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 6);
}
void SRTActiveSurfaceGUI::setactuator13_7()
{
    thecircle=13; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 7);
}
void SRTActiveSurfaceGUI::setactuator13_8()
{
    thecircle=13; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 8);
}
void SRTActiveSurfaceGUI::setactuator13_9()
{
   thecircle=13; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(13, 9);
}
void SRTActiveSurfaceGUI::setactuator13_10()
{
    thecircle=13; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 10);
}
void SRTActiveSurfaceGUI::setactuator13_11()
{
    thecircle=13; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 11);
}
void SRTActiveSurfaceGUI::setactuator13_12()
{
    thecircle=13; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 12);
}
void SRTActiveSurfaceGUI::setactuator13_13()
{
    thecircle=13; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 13);
}
void SRTActiveSurfaceGUI::setactuator13_14()
{
    thecircle=13; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 14);
}
void SRTActiveSurfaceGUI::setactuator13_15()
{
    thecircle=13; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 15);
}
void SRTActiveSurfaceGUI::setactuator13_16()
{
    thecircle=13; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 16);
}
void SRTActiveSurfaceGUI::setactuator13_17()
{
    thecircle=13; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 17);
}
void SRTActiveSurfaceGUI::setactuator13_18()
{
    thecircle=13; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 18);
}
void SRTActiveSurfaceGUI::setactuator13_19()
{
    thecircle=13; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 19);
}
void SRTActiveSurfaceGUI::setactuator13_20()
{
    thecircle=13; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 20);
}
void SRTActiveSurfaceGUI::setactuator13_21()
{
    thecircle=13; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 21);
}
void SRTActiveSurfaceGUI::setactuator13_22()
{
    thecircle=13; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 22);
}
void SRTActiveSurfaceGUI::setactuator13_23()
{
    thecircle=13; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 23);
}
void SRTActiveSurfaceGUI::setactuator13_24()
{
    thecircle=13; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 24);
}
void SRTActiveSurfaceGUI::setactuator13_25()
{
    thecircle=13; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 25);
}
void SRTActiveSurfaceGUI::setactuator13_26()
{
    thecircle=13; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 26);
}
void SRTActiveSurfaceGUI::setactuator13_27()
{
    thecircle=13; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 27);
}
void SRTActiveSurfaceGUI::setactuator13_28()
{
    thecircle=13; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 28);
}
void SRTActiveSurfaceGUI::setactuator13_29()
{
    thecircle=13; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 29);
}
void SRTActiveSurfaceGUI::setactuator13_30()
{
    thecircle=13; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 30);
}
void SRTActiveSurfaceGUI::setactuator13_31()
{
    thecircle=13; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 31);
}
void SRTActiveSurfaceGUI::setactuator13_32()
{
    thecircle=13; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 32);
}
void SRTActiveSurfaceGUI::setactuator13_33()
{
    thecircle=13; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 33);
}
void SRTActiveSurfaceGUI::setactuator13_34()
{
    thecircle=13; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 34);
}
void SRTActiveSurfaceGUI::setactuator13_35()
{
    thecircle=13; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 35);
}
void SRTActiveSurfaceGUI::setactuator13_36()
{
    thecircle=13; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 36);
}
void SRTActiveSurfaceGUI::setactuator13_37()
{
    thecircle=13; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 37);
}
void SRTActiveSurfaceGUI::setactuator13_38()
{
    thecircle=13; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 38);
}
void SRTActiveSurfaceGUI::setactuator13_39()
{
    thecircle=13; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 39);
}
void SRTActiveSurfaceGUI::setactuator13_40()
{
    thecircle=13; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 40);
}
void SRTActiveSurfaceGUI::setactuator13_41()
{
    thecircle=13; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 41);
}
void SRTActiveSurfaceGUI::setactuator13_42()
{
    thecircle=13; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 42);
}
void SRTActiveSurfaceGUI::setactuator13_43()
{
    thecircle=13; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 43);
}
void SRTActiveSurfaceGUI::setactuator13_44()
{
    thecircle=13; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 44);
}
void SRTActiveSurfaceGUI::setactuator13_45()
{
    thecircle=13; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 45);
}
void SRTActiveSurfaceGUI::setactuator13_46()
{
    thecircle=13; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 46);
}
void SRTActiveSurfaceGUI::setactuator13_47()
{
    thecircle=13; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 47);
}
void SRTActiveSurfaceGUI::setactuator13_48()
{
    thecircle=13; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 48);
}
void SRTActiveSurfaceGUI::setactuator13_49()
{
    thecircle=13; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 49);
}
void SRTActiveSurfaceGUI::setactuator13_50()
{
    thecircle=13; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 50);
}
void SRTActiveSurfaceGUI::setactuator13_51()
{
    thecircle=13; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 51);
}
void SRTActiveSurfaceGUI::setactuator13_52()
{
    thecircle=13; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 52);
}
void SRTActiveSurfaceGUI::setactuator13_53()
{
    thecircle=13; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 53);
}
void SRTActiveSurfaceGUI::setactuator13_54()
{
    thecircle=13; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 54);
}
void SRTActiveSurfaceGUI::setactuator13_55()
{
    thecircle=13; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 55);
}
void SRTActiveSurfaceGUI::setactuator13_56()
{
    thecircle=13; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 56);
}
void SRTActiveSurfaceGUI::setactuator13_57()
{
   thecircle=13; theactuator=57; theradius=0;
   mySRTActiveSurfaceCore.setactuator(13, 57);
}
void SRTActiveSurfaceGUI::setactuator13_58()
{
    thecircle=13; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 58);
}
void SRTActiveSurfaceGUI::setactuator13_59()
{
    thecircle=13; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 59);
}
void SRTActiveSurfaceGUI::setactuator13_60()
{
    thecircle=13; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 60);
}
void SRTActiveSurfaceGUI::setactuator13_61()
{
    thecircle=13; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 61);
}
void SRTActiveSurfaceGUI::setactuator13_62()
{
    thecircle=13; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 62);
}
void SRTActiveSurfaceGUI::setactuator13_63()
{
    thecircle=13; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 63);
}
void SRTActiveSurfaceGUI::setactuator13_64()
{
    thecircle=13; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 64);
}
void SRTActiveSurfaceGUI::setactuator13_65()
{
    thecircle=13; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 65);
}
void SRTActiveSurfaceGUI::setactuator13_66()
{
    thecircle=13; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 66);
}
void SRTActiveSurfaceGUI::setactuator13_67()
{
    thecircle=13; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 67);
}
void SRTActiveSurfaceGUI::setactuator13_68()
{
    thecircle=13; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 68);
}
void SRTActiveSurfaceGUI::setactuator13_69()
{
    thecircle=13; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 69);
}
void SRTActiveSurfaceGUI::setactuator13_70()
{
    thecircle=13; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 70);
}
void SRTActiveSurfaceGUI::setactuator13_71()
{
    thecircle=13; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 71);
}
void SRTActiveSurfaceGUI::setactuator13_72()
{
    thecircle=13; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 72);
}
void SRTActiveSurfaceGUI::setactuator13_73()
{
    thecircle=13; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 73);
}
void SRTActiveSurfaceGUI::setactuator13_74()
{
    thecircle=13; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 74);
}
void SRTActiveSurfaceGUI::setactuator13_75()
{
    thecircle=13; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 75);
}
void SRTActiveSurfaceGUI::setactuator13_76()
{
    thecircle=13; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 76);
}
void SRTActiveSurfaceGUI::setactuator13_77()
{
    thecircle=13; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 77);
}
void SRTActiveSurfaceGUI::setactuator13_78()
{
    thecircle=13; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 78);
}
void SRTActiveSurfaceGUI::setactuator13_79()
{
    thecircle=13; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 79);
}
void SRTActiveSurfaceGUI::setactuator13_80()
{
    thecircle=13; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 80);
}
void SRTActiveSurfaceGUI::setactuator13_81()
{
    thecircle=13; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 81);
}
void SRTActiveSurfaceGUI::setactuator13_82()
{
    thecircle=13; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 82);
}
void SRTActiveSurfaceGUI::setactuator13_83()
{
    thecircle=13; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 83);
}
void SRTActiveSurfaceGUI::setactuator13_84()
{
    thecircle=13; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 84);
}
void SRTActiveSurfaceGUI::setactuator13_85()
{
    thecircle=13; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 85);
}
void SRTActiveSurfaceGUI::setactuator13_86()
{
    thecircle=13; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 86);
}
void SRTActiveSurfaceGUI::setactuator13_87()
{
    thecircle=13; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 87);
}
void SRTActiveSurfaceGUI::setactuator13_88()
{
    thecircle=13; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 88);
}
void SRTActiveSurfaceGUI::setactuator13_89()
{
    thecircle=13; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 89);
}
void SRTActiveSurfaceGUI::setactuator13_90()
{
    thecircle=13; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 90);
}
void SRTActiveSurfaceGUI::setactuator13_91()
{
    thecircle=13; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 91);
}
void SRTActiveSurfaceGUI::setactuator13_92()
{
    thecircle=13; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 92);
}
void SRTActiveSurfaceGUI::setactuator13_93()
{
    thecircle=13; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 93);
}
void SRTActiveSurfaceGUI::setactuator13_94()
{
    thecircle=13; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 94);
}
void SRTActiveSurfaceGUI::setactuator13_95()
{
    thecircle=13; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 95);
}
void SRTActiveSurfaceGUI::setactuator13_96()
{
    thecircle=13; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(13, 96);
}
// 14 CIRCLE
void SRTActiveSurfaceGUI::setactuator14_1()
{
    thecircle=14; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 1);
}
void SRTActiveSurfaceGUI::setactuator14_2()
{
    thecircle=14; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 2);
}
void SRTActiveSurfaceGUI::setactuator14_3()
{
    thecircle=14; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 3);
}
void SRTActiveSurfaceGUI::setactuator14_4()
{
    thecircle=14; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 4);
}
void SRTActiveSurfaceGUI::setactuator14_5()
{
    thecircle=14; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 5);
}
void SRTActiveSurfaceGUI::setactuator14_6()
{
    thecircle=14; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 6);
}
void SRTActiveSurfaceGUI::setactuator14_7()
{
    thecircle=14; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 7);
}
void SRTActiveSurfaceGUI::setactuator14_8()
{
    thecircle=14; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 8);
}
void SRTActiveSurfaceGUI::setactuator14_9()
{
   thecircle=14; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(14, 9);
}
void SRTActiveSurfaceGUI::setactuator14_10()
{
    thecircle=14; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 10);
}
void SRTActiveSurfaceGUI::setactuator14_11()
{
    thecircle=14; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 11);
}
void SRTActiveSurfaceGUI::setactuator14_12()
{
    thecircle=14; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 12);
}
void SRTActiveSurfaceGUI::setactuator14_13()
{
    thecircle=14; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 13);
}
void SRTActiveSurfaceGUI::setactuator14_14()
{
    thecircle=14; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 14);
}
void SRTActiveSurfaceGUI::setactuator14_15()
{
    thecircle=14; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 15);
}
void SRTActiveSurfaceGUI::setactuator14_16()
{
    thecircle=14; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 16);
}
void SRTActiveSurfaceGUI::setactuator14_17()
{
    thecircle=14; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 17);
}
void SRTActiveSurfaceGUI::setactuator14_18()
{
    thecircle=14; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 18);
}
void SRTActiveSurfaceGUI::setactuator14_19()
{
    thecircle=14; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 19);
}
void SRTActiveSurfaceGUI::setactuator14_20()
{
    thecircle=14; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 20);
}
void SRTActiveSurfaceGUI::setactuator14_21()
{
    thecircle=14; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 21);
}
void SRTActiveSurfaceGUI::setactuator14_22()
{
    thecircle=14; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 22);
}
void SRTActiveSurfaceGUI::setactuator14_23()
{
    thecircle=14; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 23);
}
void SRTActiveSurfaceGUI::setactuator14_24()
{
    thecircle=14; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 24);
}
void SRTActiveSurfaceGUI::setactuator14_25()
{
    thecircle=14; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 25);
}
void SRTActiveSurfaceGUI::setactuator14_26()
{
    thecircle=14; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 26);
}
void SRTActiveSurfaceGUI::setactuator14_27()
{
    thecircle=14; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 27);
}
void SRTActiveSurfaceGUI::setactuator14_28()
{
    thecircle=14; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 28);
}
void SRTActiveSurfaceGUI::setactuator14_29()
{
    thecircle=14; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 29);
}
void SRTActiveSurfaceGUI::setactuator14_30()
{
    thecircle=14; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 30);
}
void SRTActiveSurfaceGUI::setactuator14_31()
{
    thecircle=14; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 31);
}
void SRTActiveSurfaceGUI::setactuator14_32()
{
    thecircle=14; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 32);
}
void SRTActiveSurfaceGUI::setactuator14_33()
{
    thecircle=14; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 33);
}
void SRTActiveSurfaceGUI::setactuator14_34()
{
    thecircle=14; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 34);
}
void SRTActiveSurfaceGUI::setactuator14_35()
{
    thecircle=14; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 35);
}
void SRTActiveSurfaceGUI::setactuator14_36()
{
    thecircle=14; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 36);
}
void SRTActiveSurfaceGUI::setactuator14_37()
{
    thecircle=14; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 37);
}
void SRTActiveSurfaceGUI::setactuator14_38()
{
    thecircle=14; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 38);
}
void SRTActiveSurfaceGUI::setactuator14_39()
{
    thecircle=14; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 39);
}
void SRTActiveSurfaceGUI::setactuator14_40()
{
    thecircle=14; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 40);
}
void SRTActiveSurfaceGUI::setactuator14_41()
{
    thecircle=14; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 41);
}
void SRTActiveSurfaceGUI::setactuator14_42()
{
    thecircle=14; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 42);
}
void SRTActiveSurfaceGUI::setactuator14_43()
{
    thecircle=14; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 43);
}
void SRTActiveSurfaceGUI::setactuator14_44()
{
    thecircle=14; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 44);
}
void SRTActiveSurfaceGUI::setactuator14_45()
{
    thecircle=14; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 45);
}
void SRTActiveSurfaceGUI::setactuator14_46()
{
    thecircle=14; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 46);
}
void SRTActiveSurfaceGUI::setactuator14_47()
{
    thecircle=14; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 47);
}
void SRTActiveSurfaceGUI::setactuator14_48()
{
    thecircle=14; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 48);
}
void SRTActiveSurfaceGUI::setactuator14_49()
{
    thecircle=14; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 49);
}
void SRTActiveSurfaceGUI::setactuator14_50()
{
    thecircle=14; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 50);
}
void SRTActiveSurfaceGUI::setactuator14_51()
{
    thecircle=14; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 51);
}
void SRTActiveSurfaceGUI::setactuator14_52()
{
    thecircle=14; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 52);
}
void SRTActiveSurfaceGUI::setactuator14_53()
{
    thecircle=14; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 53);
}
void SRTActiveSurfaceGUI::setactuator14_54()
{
    thecircle=14; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 54);
}
void SRTActiveSurfaceGUI::setactuator14_55()
{
    thecircle=14; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 55);
}
void SRTActiveSurfaceGUI::setactuator14_56()
{
    thecircle=14; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 56);
}
void SRTActiveSurfaceGUI::setactuator14_57()
{
   thecircle=14; theactuator=57; theradius=0;
   mySRTActiveSurfaceCore.setactuator(14, 57);
}
void SRTActiveSurfaceGUI::setactuator14_58()
{
    thecircle=14; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 58);
}
void SRTActiveSurfaceGUI::setactuator14_59()
{
    thecircle=14; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 59);
}
void SRTActiveSurfaceGUI::setactuator14_60()
{
    thecircle=14; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 60);
}
void SRTActiveSurfaceGUI::setactuator14_61()
{
    thecircle=14; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 61);
}
void SRTActiveSurfaceGUI::setactuator14_62()
{
    thecircle=14; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 62);
}
void SRTActiveSurfaceGUI::setactuator14_63()
{
    thecircle=14; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 63);
}
void SRTActiveSurfaceGUI::setactuator14_64()
{
    thecircle=14; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 64);
}
void SRTActiveSurfaceGUI::setactuator14_65()
{
    thecircle=14; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 65);
}
void SRTActiveSurfaceGUI::setactuator14_66()
{
    thecircle=14; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 66);
}
void SRTActiveSurfaceGUI::setactuator14_67()
{
    thecircle=14; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 67);
}
void SRTActiveSurfaceGUI::setactuator14_68()
{
    thecircle=14; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 68);
}
void SRTActiveSurfaceGUI::setactuator14_69()
{
    thecircle=14; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 69);
}
void SRTActiveSurfaceGUI::setactuator14_70()
{
    thecircle=14; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 70);
}
void SRTActiveSurfaceGUI::setactuator14_71()
{
    thecircle=14; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 71);
}
void SRTActiveSurfaceGUI::setactuator14_72()
{
    thecircle=14; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 72);
}
void SRTActiveSurfaceGUI::setactuator14_73()
{
    thecircle=14; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 73);
}
void SRTActiveSurfaceGUI::setactuator14_74()
{
    thecircle=14; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 74);
}
void SRTActiveSurfaceGUI::setactuator14_75()
{
    thecircle=14; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 75);
}
void SRTActiveSurfaceGUI::setactuator14_76()
{
    thecircle=14; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 76);
}
void SRTActiveSurfaceGUI::setactuator14_77()
{
    thecircle=14; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 77);
}
void SRTActiveSurfaceGUI::setactuator14_78()
{
    thecircle=14; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 78);
}
void SRTActiveSurfaceGUI::setactuator14_79()
{
    thecircle=14; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 79);
}
void SRTActiveSurfaceGUI::setactuator14_80()
{
    thecircle=14; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 80);
}
void SRTActiveSurfaceGUI::setactuator14_81()
{
    thecircle=14; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 81);
}
void SRTActiveSurfaceGUI::setactuator14_82()
{
    thecircle=14; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 82);
}
void SRTActiveSurfaceGUI::setactuator14_83()
{
    thecircle=14; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 83);
}
void SRTActiveSurfaceGUI::setactuator14_84()
{
    thecircle=14; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 84);
}
void SRTActiveSurfaceGUI::setactuator14_85()
{
    thecircle=14; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 85);
}
void SRTActiveSurfaceGUI::setactuator14_86()
{
    thecircle=14; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 86);
}
void SRTActiveSurfaceGUI::setactuator14_87()
{
    thecircle=14; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 87);
}
void SRTActiveSurfaceGUI::setactuator14_88()
{
    thecircle=14; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 88);
}
void SRTActiveSurfaceGUI::setactuator14_89()
{
    thecircle=14; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 89);
}
void SRTActiveSurfaceGUI::setactuator14_90()
{
    thecircle=14; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 90);
}
void SRTActiveSurfaceGUI::setactuator14_91()
{
    thecircle=14; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 91);
}
void SRTActiveSurfaceGUI::setactuator14_92()
{
    thecircle=14; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 92);
}
void SRTActiveSurfaceGUI::setactuator14_93()
{
    thecircle=14; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 93);
}
void SRTActiveSurfaceGUI::setactuator14_94()
{
    thecircle=14; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 94);
}
void SRTActiveSurfaceGUI::setactuator14_95()
{
    thecircle=14; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 95);
}
void SRTActiveSurfaceGUI::setactuator14_96()
{
    thecircle=14; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(14, 96);
}
// 15 CIRCLE
void SRTActiveSurfaceGUI::setactuator15_1()
{
    thecircle=15; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 1);
}
void SRTActiveSurfaceGUI::setactuator15_2()
{
    thecircle=15; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 2);
}
void SRTActiveSurfaceGUI::setactuator15_3()
{
    thecircle=15; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 3);
}
void SRTActiveSurfaceGUI::setactuator15_4()
{
    thecircle=15; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 4);
}
void SRTActiveSurfaceGUI::setactuator15_5()
{
    thecircle=15; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 5);
}
void SRTActiveSurfaceGUI::setactuator15_6()
{
    thecircle=15; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 6);
}
void SRTActiveSurfaceGUI::setactuator15_7()
{
    thecircle=15; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 7);
}
void SRTActiveSurfaceGUI::setactuator15_8()
{
    thecircle=15; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 8);
}
void SRTActiveSurfaceGUI::setactuator15_9()
{
   thecircle=15; theactuator=9; theradius=0;
   mySRTActiveSurfaceCore.setactuator(15, 9);
}
void SRTActiveSurfaceGUI::setactuator15_10()
{
    thecircle=15; theactuator=10; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 10);
}
void SRTActiveSurfaceGUI::setactuator15_11()
{
    thecircle=15; theactuator=11; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 11);
}
void SRTActiveSurfaceGUI::setactuator15_12()
{
    thecircle=15; theactuator=12; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 12);
}
void SRTActiveSurfaceGUI::setactuator15_13()
{
    thecircle=15; theactuator=13; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 13);
}
void SRTActiveSurfaceGUI::setactuator15_14()
{
    thecircle=15; theactuator=14; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 14);
}
void SRTActiveSurfaceGUI::setactuator15_15()
{
    thecircle=15; theactuator=15; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 15);
}
void SRTActiveSurfaceGUI::setactuator15_16()
{
    thecircle=15; theactuator=16; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 16);
}
void SRTActiveSurfaceGUI::setactuator15_17()
{
    thecircle=15; theactuator=17; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 17);
}
void SRTActiveSurfaceGUI::setactuator15_18()
{
    thecircle=15; theactuator=18; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 18);
}
void SRTActiveSurfaceGUI::setactuator15_19()
{
    thecircle=15; theactuator=19; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 19);
}
void SRTActiveSurfaceGUI::setactuator15_20()
{
    thecircle=15; theactuator=20; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 20);
}
void SRTActiveSurfaceGUI::setactuator15_21()
{
    thecircle=15; theactuator=21; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 21);
}
void SRTActiveSurfaceGUI::setactuator15_22()
{
    thecircle=15; theactuator=22; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 22);
}
void SRTActiveSurfaceGUI::setactuator15_23()
{
    thecircle=15; theactuator=23; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 23);
}
void SRTActiveSurfaceGUI::setactuator15_24()
{
    thecircle=15; theactuator=24; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 24);
}
void SRTActiveSurfaceGUI::setactuator15_25()
{
    thecircle=15; theactuator=25; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 25);
}
void SRTActiveSurfaceGUI::setactuator15_26()
{
    thecircle=15; theactuator=26; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 26);
}
void SRTActiveSurfaceGUI::setactuator15_27()
{
    thecircle=15; theactuator=27; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 27);
}
void SRTActiveSurfaceGUI::setactuator15_28()
{
    thecircle=15; theactuator=28; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 28);
}
void SRTActiveSurfaceGUI::setactuator15_29()
{
    thecircle=15; theactuator=29; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 29);
}
void SRTActiveSurfaceGUI::setactuator15_30()
{
    thecircle=15; theactuator=30; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 30);
}
void SRTActiveSurfaceGUI::setactuator15_31()
{
    thecircle=15; theactuator=31; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 31);
}
void SRTActiveSurfaceGUI::setactuator15_32()
{
    thecircle=15; theactuator=32; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 32);
}
void SRTActiveSurfaceGUI::setactuator15_33()
{
    thecircle=15; theactuator=33; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 33);
}
void SRTActiveSurfaceGUI::setactuator15_34()
{
    thecircle=15; theactuator=34; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 34);
}
void SRTActiveSurfaceGUI::setactuator15_35()
{
    thecircle=15; theactuator=35; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 35);
}
void SRTActiveSurfaceGUI::setactuator15_36()
{
    thecircle=15; theactuator=36; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 36);
}
void SRTActiveSurfaceGUI::setactuator15_37()
{
    thecircle=15; theactuator=37; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 37);
}
void SRTActiveSurfaceGUI::setactuator15_38()
{
    thecircle=15; theactuator=38; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 38);
}
void SRTActiveSurfaceGUI::setactuator15_39()
{
    thecircle=15; theactuator=39; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 39);
}
void SRTActiveSurfaceGUI::setactuator15_40()
{
    thecircle=15; theactuator=40; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 40);
}
void SRTActiveSurfaceGUI::setactuator15_41()
{
    thecircle=15; theactuator=41; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 41);
}
void SRTActiveSurfaceGUI::setactuator15_42()
{
    thecircle=15; theactuator=42; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 42);
}
void SRTActiveSurfaceGUI::setactuator15_43()
{
    thecircle=15; theactuator=43; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 43);
}
void SRTActiveSurfaceGUI::setactuator15_44()
{
    thecircle=15; theactuator=44; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 44);
}
void SRTActiveSurfaceGUI::setactuator15_45()
{
    thecircle=15; theactuator=45; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 45);
}
void SRTActiveSurfaceGUI::setactuator15_46()
{
    thecircle=15; theactuator=46; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 46);
}
void SRTActiveSurfaceGUI::setactuator15_47()
{
    thecircle=15; theactuator=47; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 47);
}
void SRTActiveSurfaceGUI::setactuator15_48()
{
    thecircle=15; theactuator=48; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 48);
}
void SRTActiveSurfaceGUI::setactuator15_49()
{
    thecircle=15; theactuator=49; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 49);
}
void SRTActiveSurfaceGUI::setactuator15_50()
{
    thecircle=15; theactuator=50; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 50);
}
void SRTActiveSurfaceGUI::setactuator15_51()
{
    thecircle=15; theactuator=51; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 51);
}
void SRTActiveSurfaceGUI::setactuator15_52()
{
    thecircle=15; theactuator=52; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 52);
}
void SRTActiveSurfaceGUI::setactuator15_53()
{
    thecircle=15; theactuator=53; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 53);
}
void SRTActiveSurfaceGUI::setactuator15_54()
{
    thecircle=15; theactuator=54; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 54);
}
void SRTActiveSurfaceGUI::setactuator15_55()
{
    thecircle=15; theactuator=55; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 55);
}
void SRTActiveSurfaceGUI::setactuator15_56()
{
    thecircle=15; theactuator=56; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 56);
}
void SRTActiveSurfaceGUI::setactuator15_57()
{
   thecircle=15; theactuator=57; theradius=0;
   mySRTActiveSurfaceCore.setactuator(15, 57);
}
void SRTActiveSurfaceGUI::setactuator15_58()
{
    thecircle=15; theactuator=58; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 58);
}
void SRTActiveSurfaceGUI::setactuator15_59()
{
    thecircle=15; theactuator=59; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 59);
}
void SRTActiveSurfaceGUI::setactuator15_60()
{
    thecircle=15; theactuator=60; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 60);
}
void SRTActiveSurfaceGUI::setactuator15_61()
{
    thecircle=15; theactuator=61; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 61);
}
void SRTActiveSurfaceGUI::setactuator15_62()
{
    thecircle=15; theactuator=62; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 62);
}
void SRTActiveSurfaceGUI::setactuator15_63()
{
    thecircle=15; theactuator=63; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 63);
}
void SRTActiveSurfaceGUI::setactuator15_64()
{
    thecircle=15; theactuator=64; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 64);
}
void SRTActiveSurfaceGUI::setactuator15_65()
{
    thecircle=15; theactuator=65; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 65);
}
void SRTActiveSurfaceGUI::setactuator15_66()
{
    thecircle=15; theactuator=66; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 66);
}
void SRTActiveSurfaceGUI::setactuator15_67()
{
    thecircle=15; theactuator=67; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 67);
}
void SRTActiveSurfaceGUI::setactuator15_68()
{
    thecircle=15; theactuator=68; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 68);
}
void SRTActiveSurfaceGUI::setactuator15_69()
{
    thecircle=15; theactuator=69; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 69);
}
void SRTActiveSurfaceGUI::setactuator15_70()
{
    thecircle=15; theactuator=70; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 70);
}
void SRTActiveSurfaceGUI::setactuator15_71()
{
    thecircle=15; theactuator=71; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 71);
}
void SRTActiveSurfaceGUI::setactuator15_72()
{
    thecircle=15; theactuator=72; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 72);
}
void SRTActiveSurfaceGUI::setactuator15_73()
{
    thecircle=15; theactuator=73; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 73);
}
void SRTActiveSurfaceGUI::setactuator15_74()
{
    thecircle=15; theactuator=74; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 74);
}
void SRTActiveSurfaceGUI::setactuator15_75()
{
    thecircle=15; theactuator=75; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 75);
}
void SRTActiveSurfaceGUI::setactuator15_76()
{
    thecircle=15; theactuator=76; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 76);
}
void SRTActiveSurfaceGUI::setactuator15_77()
{
    thecircle=15; theactuator=77; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 77);
}
void SRTActiveSurfaceGUI::setactuator15_78()
{
    thecircle=15; theactuator=78; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 78);
}
void SRTActiveSurfaceGUI::setactuator15_79()
{
    thecircle=15; theactuator=79; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 79);
}
void SRTActiveSurfaceGUI::setactuator15_80()
{
    thecircle=15; theactuator=80; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 80);
}
void SRTActiveSurfaceGUI::setactuator15_81()
{
    thecircle=15; theactuator=81; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 81);
}
void SRTActiveSurfaceGUI::setactuator15_82()
{
    thecircle=15; theactuator=82; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 82);
}
void SRTActiveSurfaceGUI::setactuator15_83()
{
    thecircle=15; theactuator=83; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 83);
}
void SRTActiveSurfaceGUI::setactuator15_84()
{
    thecircle=15; theactuator=84; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 84);
}
void SRTActiveSurfaceGUI::setactuator15_85()
{
    thecircle=15; theactuator=85; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 85);
}
void SRTActiveSurfaceGUI::setactuator15_86()
{
    thecircle=15; theactuator=86; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 86);
}
void SRTActiveSurfaceGUI::setactuator15_87()
{
    thecircle=15; theactuator=87; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 87);
}
void SRTActiveSurfaceGUI::setactuator15_88()
{
    thecircle=15; theactuator=88; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 88);
}
void SRTActiveSurfaceGUI::setactuator15_89()
{
    thecircle=15; theactuator=89; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 89);
}
void SRTActiveSurfaceGUI::setactuator15_90()
{
    thecircle=15; theactuator=90; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 90);
}
void SRTActiveSurfaceGUI::setactuator15_91()
{
    thecircle=15; theactuator=91; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 91);
}
void SRTActiveSurfaceGUI::setactuator15_92()
{
    thecircle=15; theactuator=92; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 92);
}
void SRTActiveSurfaceGUI::setactuator15_93()
{
    thecircle=15; theactuator=93; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 93);
}
void SRTActiveSurfaceGUI::setactuator15_94()
{
    thecircle=15; theactuator=94; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 94);
}
void SRTActiveSurfaceGUI::setactuator15_95()
{
    thecircle=15; theactuator=95; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 95);
}
void SRTActiveSurfaceGUI::setactuator15_96()
{
    thecircle=15; theactuator=96; theradius=0;
    mySRTActiveSurfaceCore.setactuator(15, 96);
}
// 16 CIRCLE
void SRTActiveSurfaceGUI::setactuator16_1()
{
    thecircle=16; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(16, 1);
}
void SRTActiveSurfaceGUI::setactuator16_2()
{
    thecircle=16; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(16, 2);
}
void SRTActiveSurfaceGUI::setactuator16_3()
{
    thecircle=16; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(16, 3);
}
void SRTActiveSurfaceGUI::setactuator16_4()
{
    thecircle=16; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(16, 4);
}
void SRTActiveSurfaceGUI::setactuator16_5()
{
    thecircle=16; theactuator=5; theradius=0;
    mySRTActiveSurfaceCore.setactuator(16, 5);
}
void SRTActiveSurfaceGUI::setactuator16_6()
{
    thecircle=16; theactuator=6; theradius=0;
    mySRTActiveSurfaceCore.setactuator(16, 6);
}
void SRTActiveSurfaceGUI::setactuator16_7()
{
    thecircle=16; theactuator=7; theradius=0;
    mySRTActiveSurfaceCore.setactuator(16, 7);
}
void SRTActiveSurfaceGUI::setactuator16_8()
{
    thecircle=16; theactuator=8; theradius=0;
    mySRTActiveSurfaceCore.setactuator(16, 8);
}
// 17 CIRCLE
void SRTActiveSurfaceGUI::setactuator17_1()
{
    thecircle=17; theactuator=1; theradius=0;
    mySRTActiveSurfaceCore.setactuator(17, 1);
}
void SRTActiveSurfaceGUI::setactuator17_2()
{
    thecircle=17; theactuator=2; theradius=0;
    mySRTActiveSurfaceCore.setactuator(17, 2);
}
void SRTActiveSurfaceGUI::setactuator17_3()
{
    thecircle=17; theactuator=3; theradius=0;
    mySRTActiveSurfaceCore.setactuator(17, 3);
}
void SRTActiveSurfaceGUI::setactuator17_4()
{
    thecircle=17; theactuator=4; theradius=0;
    mySRTActiveSurfaceCore.setactuator(17, 4);
}

void SRTActiveSurfaceGUI::move()
{
    QString str, str1;
    long incr;
    
    str1 = ActuatorNumberlineEdit->text();
    if (str1.length()==0) {
        ManagerLabel->clear();
        ////ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
        ManagerLabel->setText("choose actuator");
    }
    else {
        str = ActuatorMovelineEdit->text();
        if (str.length()==0)
            incr = 0;
        else
            incr = str.toLong();
        ManagerLabel->clear();
        ////ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
        if (thecircle != 0 && theactuator == 0 && theradius == 0)
            str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("move");
        if (thecircle == 0 && theactuator == 0 && theradius != 0)
            str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("move");
        if (thecircle != 0 && theactuator != 0 && theradius == 0)
            str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("move");
        if (thecircle == 0 && theactuator == 0 && theradius == 0)
            str = QString("%1 %2").arg("ALL").arg("move");
        ManagerLabel->setText(str);
        mySRTActiveSurfaceCore.move(thecircle,theactuator,theradius,incr);
   }  
}

void SRTActiveSurfaceGUI::setprofile()
{
    QString str;
    long profile;
    
    profile = (long)SetProfilecomboBox->currentIndex();
    ManagerLabel->clear();
	thecircle = theactuator = theradius = 0;
	str = QString("%1 %2 %3").arg("set").arg("AS").arg("profile");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore.setProfile(profile);
}

void SRTActiveSurfaceGUI::correction()
{
    QString str, str1;
    double correction;
    
    str1 = ActuatorNumberlineEdit->text();
    if (str1.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    str = ActuatorCorrectionlineEdit->text();
	    if (str.length()==0)
	        correction = 0;
	    else
	        correction = str.toDouble();
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("correction");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("correction");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("correction");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("correction");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.correction(thecircle,theactuator,theradius,correction);
    }  
}

void SRTActiveSurfaceGUI::update()
{
    QString str, str1;
    double elev;
    
    str = ActuatorUpdatelineEdit->text();
    if (str.length()==0)
	    elev = 0.0;
    else
	    elev = str.toDouble();
    if (elev < 0.0 || elev > 90.0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
	    str1 = QString("%1").arg("Elevation out of range");
	    ManagerLabel->setText(str1);
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    thecircle = theactuator = theradius = 0;
	    str = QString("%1 %2").arg("ALL").arg("update");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.update(elev);
    }
}

void SRTActiveSurfaceGUI::reset()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("reset");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("reset");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("reset");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("reset");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.reset(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::stop()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("stop");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("stop");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("stop");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("stop");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.stopUSD(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::up()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("up");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("up");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("up");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("up");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.up(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::down()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("down");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("down");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("down");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("down");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.down(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::top()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("top");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("top");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("top");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("top");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.top(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::bottom()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("bottom");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("bottom");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("bottom");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("bottom");	
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.bottom(thecircle,theactuator,theradius);
     }
}

void SRTActiveSurfaceGUI::calibrate()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("calibrate");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("calibrate");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("calibrate");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("calibrate");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.calibrate(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::calVer()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("verify calibration");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("verify calibration");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("verify calibration");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("verify calibration");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.calVer(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::stow()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("stow");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("stow");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("stow");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("stow");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.stow(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::setup()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("setup");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("setup");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("setup");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("setup");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.setup(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::refPos()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	    if (thecircle != 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("reference position");
	    if (thecircle == 0 && theactuator == 0 && theradius != 0)
	        str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("reference position");
	    if (thecircle != 0 && theactuator != 0 && theradius == 0)
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("reference position");
	    if (thecircle == 0 && theactuator == 0 && theradius == 0)
	        str = QString("%1 %2").arg("ALL").arg("reference position");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore.refPos(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::setupAS()
{
    setallactuators();
    mySRTActiveSurfaceCore.setup(thecircle,theactuator,theradius);
}

void SRTActiveSurfaceGUI::startAS()
{
    setallactuators();
    mySRTActiveSurfaceCore.enableAutoUpdate();
}

void SRTActiveSurfaceGUI::stowAS()
{
    setallactuators();
    mySRTActiveSurfaceCore.park();
}

void SRTActiveSurfaceGUI::stopAS()
{
    setallactuators();
    mySRTActiveSurfaceCore.stopUSD(thecircle,theactuator,theradius);
}

void SRTActiveSurfaceGUI::recoverUSD()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
	    ManagerLabel->setText("choose actuator");
    }
    else {
	    ManagerLabel->clear();
	    //ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
	
    	if (thecircle != 0 && theactuator != 0 && theradius == 0) {
	        str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("USD recover");
	        ManagerLabel->setText(str);
	        mySRTActiveSurfaceCore.recoverUSD(thecircle,theactuator);
	    }
	    else {
	        str = QString("%1").arg("only one actuator per time");
	        ManagerLabel->setText(str);
	    }
    }
}

void SRTActiveSurfaceGUI::changeGUIActuatorColor(int tcircle, int tactuator, const char* theActuatorStatusColorString, bool fromRun)
{
    if (!fromRun)
    {
        tcircle = thecircle;
        tactuator = theactuator;
    }

	switch (tcircle) {
		case 1:
			switch (tactuator) {
				case 1:
					ActuatorButton01_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton01_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton01_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton01_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton01_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton01_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton01_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton01_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton01_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton01_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton01_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton01_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton01_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton01_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton01_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton01_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton01_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton01_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton01_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton01_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton01_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton01_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton01_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton01_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 2:
			switch (tactuator) {
				case 1:
					ActuatorButton02_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton02_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton02_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton02_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton02_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton02_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton02_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton02_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton02_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton02_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton02_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton02_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton02_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton02_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton02_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton02_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton02_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton02_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton02_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton02_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton02_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton02_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton02_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton02_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 3:
			switch (tactuator) {
				case 1:
					ActuatorButton03_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton03_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton03_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton03_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton03_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton03_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton03_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton03_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton03_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton03_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton03_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton03_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton03_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton03_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton03_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton03_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton03_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton03_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton03_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton03_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton03_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton03_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton03_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton03_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton03_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton03_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton03_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton03_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton03_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton03_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton03_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton03_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton03_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton03_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton03_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton03_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton03_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton03_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton03_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton03_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton03_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton03_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton03_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton03_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton03_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton03_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton03_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton03_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 4:
			switch (tactuator) {
				case 1:
					ActuatorButton04_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton04_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton04_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton04_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton04_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton04_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton04_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton04_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton04_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton04_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton04_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton04_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton04_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton04_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton04_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton04_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton04_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton04_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton04_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton04_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton04_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton04_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton04_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton04_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton04_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton04_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton04_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton04_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton04_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton04_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton04_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton04_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton04_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton04_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton04_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton04_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton04_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton04_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton04_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton04_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton04_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton04_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton04_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton04_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton04_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton04_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton04_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton04_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 5:
			switch (tactuator) {
				case 1:
					ActuatorButton05_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton05_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton05_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton05_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton05_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton05_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton05_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton05_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton05_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton05_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton05_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton05_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton05_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton05_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton05_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton05_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton05_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton05_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton05_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton05_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton05_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton05_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton05_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton05_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton05_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton05_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton05_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton05_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton05_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton05_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton05_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton05_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton05_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton05_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton05_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton05_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton05_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton05_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton05_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton05_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton05_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton05_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton05_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton05_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton05_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton05_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton05_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton05_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 6:
			switch (tactuator) {
				case 1:
					ActuatorButton06_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton06_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton06_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton06_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton06_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton06_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton06_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton06_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton06_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton06_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton06_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton06_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton06_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton06_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton06_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton06_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton06_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton06_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton06_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton06_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton06_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton06_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton06_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton06_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton06_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton06_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton06_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton06_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton06_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton06_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton06_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton06_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton06_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton06_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton06_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton06_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton06_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton06_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton06_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton06_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton06_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton06_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton06_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton06_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton06_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton06_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton06_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton06_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 7:
			switch (tactuator) {
				case 1:
					ActuatorButton07_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton07_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton07_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton07_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton07_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton07_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton07_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton07_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton07_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton07_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton07_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton07_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton07_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton07_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton07_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton07_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton07_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton07_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton07_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton07_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton07_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton07_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton07_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton07_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton07_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton07_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton07_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton07_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton07_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton07_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton07_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton07_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton07_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton07_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton07_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton07_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton07_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton07_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton07_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton07_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton07_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton07_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton07_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton07_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton07_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton07_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton07_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton07_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton07_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton07_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton07_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton07_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton07_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton07_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton07_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton07_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton07_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton07_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton07_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton07_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton07_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton07_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton07_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton07_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton07_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton07_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton07_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton07_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton07_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton07_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton07_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton07_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton07_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton07_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton07_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton07_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton07_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton07_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton07_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton07_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton07_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton07_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton07_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton07_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton07_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton07_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton07_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton07_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton07_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton07_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton07_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton07_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton07_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton07_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton07_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton07_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 8:
			switch (tactuator) {
				case 1:
					ActuatorButton08_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton08_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton08_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton08_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton08_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton08_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton08_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton08_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton08_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton08_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton08_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton08_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton08_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton08_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton08_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton08_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton08_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton08_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton08_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton08_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton08_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton08_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton08_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton08_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton08_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton08_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton08_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton08_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton08_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton08_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton08_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton08_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton08_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton08_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton08_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton08_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton08_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton08_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton08_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton08_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton08_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton08_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton08_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton08_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton08_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton08_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton08_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton08_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton08_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton08_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton08_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton08_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton08_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton08_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton08_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton08_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton08_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton08_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton08_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton08_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton08_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton08_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton08_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton08_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton08_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton08_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton08_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton08_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton08_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton08_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton08_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton08_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton08_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton08_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton08_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton08_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton08_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton08_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton08_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton08_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton08_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton08_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton08_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton08_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton08_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton08_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton08_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton08_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton08_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton08_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton08_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton08_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton08_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton08_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton08_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton08_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 9:
			switch (tactuator) {
				case 1:
					ActuatorButton09_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton09_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton09_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton09_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton09_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton09_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton09_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton09_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton09_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton09_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton09_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton09_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton09_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton09_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton09_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton09_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton09_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton09_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton09_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton09_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton09_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton09_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton09_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton09_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton09_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton09_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton09_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton09_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton09_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton09_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton09_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton09_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton09_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton09_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton09_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton09_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton09_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton09_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton09_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton09_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton09_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton09_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton09_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton09_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton09_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton09_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton09_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton09_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton09_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton09_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton09_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton09_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton09_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton09_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton09_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton09_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton09_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton09_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton09_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton09_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton09_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton09_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton09_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton09_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton09_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton09_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton09_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton09_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton09_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton09_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton09_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton09_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton09_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton09_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton09_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton09_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton09_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton09_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton09_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton09_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton09_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton09_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton09_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton09_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton09_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton09_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton09_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton09_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton09_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton09_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton09_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton09_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton09_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton09_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton09_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton09_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 10:
			switch (tactuator) {
				case 1:
					ActuatorButton10_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton10_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton10_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton10_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton10_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton10_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton10_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton10_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton10_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton10_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton10_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton10_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton10_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton10_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton10_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton10_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton10_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton10_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton10_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton10_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton10_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton10_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton10_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton10_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton10_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton10_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton10_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton10_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton10_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton10_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton10_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton10_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton10_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton10_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton10_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton10_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton10_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton10_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton10_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton10_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton10_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton10_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton10_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton10_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton10_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton10_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton10_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton10_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton10_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton10_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton10_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton10_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton10_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton10_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton10_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton10_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton10_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton10_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton10_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton10_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton10_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton10_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton10_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton10_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton10_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton10_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton10_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton10_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton10_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton10_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton10_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton10_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton10_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton10_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton10_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton10_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton10_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton10_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton10_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton10_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton10_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton10_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton10_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton10_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton10_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton10_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton10_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton10_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton10_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton10_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton10_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton10_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton10_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton10_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton10_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton10_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 11:
			switch (tactuator) {
				case 1:
					ActuatorButton11_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton11_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton11_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton11_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton11_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton11_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton11_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton11_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton11_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton11_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton11_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton11_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton11_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton11_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton11_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton11_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton11_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton11_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton11_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton11_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton11_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton11_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton11_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton11_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton11_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton11_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton11_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton11_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton11_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton11_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton11_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton11_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton11_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton11_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton11_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton11_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton11_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton11_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton11_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton11_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton11_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton11_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton11_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton11_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton11_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton11_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton11_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton11_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton11_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton11_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton11_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton11_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton11_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton11_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton11_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton11_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton11_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton11_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton11_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton11_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton11_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton11_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton11_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton11_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton11_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton11_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton11_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton11_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton11_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton11_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton11_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton11_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton11_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton11_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton11_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton11_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton11_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton11_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton11_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton11_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton11_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton11_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton11_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton11_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton11_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton11_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton11_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton11_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton11_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton11_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton11_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton11_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton11_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton11_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton11_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton11_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 12:
			switch (tactuator) {
				case 1:
					ActuatorButton12_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton12_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton12_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton12_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton12_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton12_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton12_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton12_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton12_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton12_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton12_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton12_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton12_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton12_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton12_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton12_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton12_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton12_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton12_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton12_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton12_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton12_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton12_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton12_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton12_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton12_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton12_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton12_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton12_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton12_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton12_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton12_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton12_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton12_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton12_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton12_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton12_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton12_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton12_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton12_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton12_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton12_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton12_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton12_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton12_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton12_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton12_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton12_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton12_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton12_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton12_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton12_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton12_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton12_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton12_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton12_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton12_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton12_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton12_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton12_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton12_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton12_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton12_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton12_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton12_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton12_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton12_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton12_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton12_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton12_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton12_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton12_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton12_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton12_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton12_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton12_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton12_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton12_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton12_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton12_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton12_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton12_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton12_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton12_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton12_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton12_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton12_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton12_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton12_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton12_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton12_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton12_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton12_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton12_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton12_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton12_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 13:
			switch (tactuator) {
				case 1:
					ActuatorButton13_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton13_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton13_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton13_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton13_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton13_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton13_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton13_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton13_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton13_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton13_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton13_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton13_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton13_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton13_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton13_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton13_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton13_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton13_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton13_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton13_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton13_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton13_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton13_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton13_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton13_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton13_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton13_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton13_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton13_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton13_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton13_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton13_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton13_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton13_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton13_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton13_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton13_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton13_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton13_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton13_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton13_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton13_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton13_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton13_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton13_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton13_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton13_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton13_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton13_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton13_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton13_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton13_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton13_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton13_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton13_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton13_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton13_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton13_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton13_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton13_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton13_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton13_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton13_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton13_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton13_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton13_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton13_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton13_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton13_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton13_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton13_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton13_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton13_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton13_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton13_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton13_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton13_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton13_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton13_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton13_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton13_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton13_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton13_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton13_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton13_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton13_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton13_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton13_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton13_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton13_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton13_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton13_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton13_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton13_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton13_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 14:
			switch (tactuator) {
				case 1:
					ActuatorButton14_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton14_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton14_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton14_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton14_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton14_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton14_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton14_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton14_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton14_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton14_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton14_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton14_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton14_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton14_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton14_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton14_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton14_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton14_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton14_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton14_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton14_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton14_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton14_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton14_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton14_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton14_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton14_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton14_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton14_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton14_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton14_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton14_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton14_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton14_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton14_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton14_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton14_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton14_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton14_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton14_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton14_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton14_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton14_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton14_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton14_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton14_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton14_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton14_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton14_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton14_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton14_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton14_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton14_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton14_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton14_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton14_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton14_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton14_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton14_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton14_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton14_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton14_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton14_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton14_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton14_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton14_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton14_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton14_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton14_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton14_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton14_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton14_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton14_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton14_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton14_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton14_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton14_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton14_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton14_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton14_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton14_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton14_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton14_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton14_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton14_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton14_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton14_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton14_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton14_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton14_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton14_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton14_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton14_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton14_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton14_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 15:
			switch (tactuator) {
				case 1:
					ActuatorButton15_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton15_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton15_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton15_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton15_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton15_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton15_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton15_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 9:
					ActuatorButton15_09->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 10:
					ActuatorButton15_10->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 11:
					ActuatorButton15_11->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 12:
					ActuatorButton15_12->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 13:
					ActuatorButton15_13->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 14:
					ActuatorButton15_14->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 15:
					ActuatorButton15_15->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 16:
					ActuatorButton15_16->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 17:
					ActuatorButton15_17->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 18:
					ActuatorButton15_18->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 19:
					ActuatorButton15_19->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 20:
					ActuatorButton15_20->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 21:
					ActuatorButton15_21->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 22:
					ActuatorButton15_22->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 23:
					ActuatorButton15_23->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 24:
					ActuatorButton15_24->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 25:
					ActuatorButton15_25->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 26:
					ActuatorButton15_26->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 27:
					ActuatorButton15_27->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 28:
					ActuatorButton15_28->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 29:
					ActuatorButton15_29->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 30:
					ActuatorButton15_30->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 31:
					ActuatorButton15_31->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 32:
					ActuatorButton15_32->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 33:
					ActuatorButton15_33->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 34:
					ActuatorButton15_34->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 35:
					ActuatorButton15_35->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 36:
					ActuatorButton15_36->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 37:
					ActuatorButton15_37->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 38:
					ActuatorButton15_38->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 39:
					ActuatorButton15_39->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 40:
					ActuatorButton15_40->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 41:
					ActuatorButton15_41->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 42:
					ActuatorButton15_42->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 43:
					ActuatorButton15_43->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 44:
					ActuatorButton15_44->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 45:
					ActuatorButton15_45->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 46:
					ActuatorButton15_46->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 47:
					ActuatorButton15_47->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 48:
					ActuatorButton15_48->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 49:
					ActuatorButton15_49->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 50:
					ActuatorButton15_50->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 51:
					ActuatorButton15_51->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 52:
					ActuatorButton15_52->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 53:
					ActuatorButton15_53->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 54:
					ActuatorButton15_54->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 55:
					ActuatorButton15_55->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 56:
					ActuatorButton15_56->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 57:
					ActuatorButton15_57->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 58:
					ActuatorButton15_58->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 59:
					ActuatorButton15_59->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 60:
					ActuatorButton15_60->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 61:
					ActuatorButton15_61->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 62:
					ActuatorButton15_62->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 63:
					ActuatorButton15_63->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 64:
					ActuatorButton15_64->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 65:
					ActuatorButton15_65->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 66:
					ActuatorButton15_66->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 67:
					ActuatorButton15_67->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 68:
					ActuatorButton15_68->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 69:
					ActuatorButton15_69->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 70:
					ActuatorButton15_70->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 71:
					ActuatorButton15_71->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 72:
					ActuatorButton15_72->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 73:
					ActuatorButton15_73->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 74:
					ActuatorButton15_74->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 75:
					ActuatorButton15_75->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 76:
					ActuatorButton15_76->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 77:
					ActuatorButton15_77->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 78:
					ActuatorButton15_78->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 79:
					ActuatorButton15_79->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 80:
					ActuatorButton15_80->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 81:
					ActuatorButton15_81->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 82:
					ActuatorButton15_82->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 83:
					ActuatorButton15_83->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 84:
					ActuatorButton15_84->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 85:
					ActuatorButton15_85->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 86:
					ActuatorButton15_86->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 87:
					ActuatorButton15_87->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 88:
					ActuatorButton15_88->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 89:
					ActuatorButton15_89->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 90:
					ActuatorButton15_90->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 91:
					ActuatorButton15_91->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 92:
					ActuatorButton15_92->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 93:
					ActuatorButton15_93->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 94:
					ActuatorButton15_94->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 95:
					ActuatorButton15_95->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 96:
					ActuatorButton15_96->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 16:
			switch (tactuator) {
				case 1:
					ActuatorButton16_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton16_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton16_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton16_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 5:
					ActuatorButton16_05->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 6:
					ActuatorButton16_06->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 7:
					ActuatorButton16_07->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 8:
					ActuatorButton16_08->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
		case 17:
			switch (tactuator) {
				case 1:
					ActuatorButton17_01->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 2:
					ActuatorButton17_02->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 3:
					ActuatorButton17_03->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
				case 4:
					ActuatorButton17_04->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", theActuatorStatusColorString, 0, QApplication::UnicodeUTF8));
					break;
			}
		break;
	}
}


void SRTActiveSurfaceGUI::changeGUIAllActuators(bool callfromfunction)
{
	QString ActuatorNumber_str;
	ActuatorNumber_str = QString("%1").arg("ALL");
	ActuatorNumberlineEdit->clear();
	ActuatorNumberlineEdit->insert(ActuatorNumber_str);

	if (!callfromfunction)
	{
		QString str;
		str = QString("%1").arg("ALL");
		ManagerLabel->clear();
		////ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
		ManagerLabel->setText(str);
	}

	clearactuatorslineedit();
}

void SRTActiveSurfaceGUI::clearactuatorslineedit()
{
	mmActuatorActualPositionlineEdit->clear();
	stepActuatorCommandedPositionlineEdit->clear();
	stepActuatorActualPositionlineEdit->clear();
	mmActuatorCommandedPositionlineEdit->clear();
	ActuatorDelaylineEdit->clear();
	ActuatorFminlineEdit->clear();
	ActuatorFmaxlineEdit->clear();
	ActuatorAccelerationlineEdit->clear();
	ActuatorStatusCammLabel->clear();
	ActuatorStatusEnblLabel->clear();
	ActuatorStatusLoopLabel->clear();
	ActuatorStatusCalLabel->clear();
	ActuatorStatusRunLabel->clear();
}

void SRTActiveSurfaceGUI::changeGUIcircleORradius(bool callfromfunction)
{
    //int circle = mySRTActiveSurfaceCore.theCircle;
    //int radius = mySRTActiveSurfaceCore.theRadius;
	QString ActuatorNumber_str;

	if (thecircle && !theradius)
		ActuatorNumber_str = QString("%1_%2").arg("CIRCLE").arg(thecircle);
	if (!thecircle && theradius)
		ActuatorNumber_str = QString("%1_%2").arg("RADIUS").arg(theradius);

	ActuatorNumberlineEdit->clear();
	ActuatorNumberlineEdit->insert(ActuatorNumber_str);

	if (!callfromfunction)
	{
		ManagerLabel->clear();
		////ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
		ManagerLabel->setText(ActuatorNumber_str);
	}

	clearactuatorslineedit();
}

void SRTActiveSurfaceGUI::changeGUIActuator(bool callfromfunction)
{
    //int circle = mySRTActiveSurfaceCore.theCircle;
    //int actuator = mySRTActiveSurfaceCore.theActuator;

    QString ActuatorNumber_str = QString("%1_%2").arg(thecircle).arg(theactuator);

    ActuatorNumberlineEdit->clear();
	ActuatorNumberlineEdit->insert(ActuatorNumber_str);

	if (!callfromfunction)
	{
		ManagerLabel->clear();
		////ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
		ManagerLabel->setText(ActuatorNumber_str);
	}

	clearactuatorslineedit();
}

void SRTActiveSurfaceGUI::changeGUIActuatorStatusEnblLabel()
{
    ActuatorStatusEnblLabel->clear();
    switch (mySRTActiveSurfaceCore.ActuatorStatusEnblLabelCode) {
        case (1):
            ActuatorStatusEnblLabel->setText("ENABLED");
            break;
        case (-1):
            ActuatorStatusEnblLabel->setText("UNABLED");
            break;
    }
}

void SRTActiveSurfaceGUI::changeGUIActuatorValues()
{
    stepActuatorActualPositionlineEdit->clear();
	stepActuatorActualPositionlineEdit->insert(mySRTActiveSurfaceCore.ActuatorActualPosition_str);
    mmActuatorActualPositionlineEdit->clear();
	mmActuatorActualPositionlineEdit->insert(mySRTActiveSurfaceCore.mm_ActuatorActualPosition_str);
    stepActuatorCommandedPositionlineEdit->clear();
	stepActuatorCommandedPositionlineEdit->insert(mySRTActiveSurfaceCore.ActuatorCommandedPosition_str);
    mmActuatorCommandedPositionlineEdit->clear();
	mmActuatorCommandedPositionlineEdit->insert(mySRTActiveSurfaceCore.mm_ActuatorCommandedPosition_str);
    ActuatorDelaylineEdit->clear();
	ActuatorDelaylineEdit->insert(mySRTActiveSurfaceCore.ActuatorDelay_str);
    ActuatorFminlineEdit->clear();
	ActuatorFminlineEdit->insert(mySRTActiveSurfaceCore.ActuatorFmin_str);
    ActuatorFmaxlineEdit->clear();
	ActuatorFmaxlineEdit->insert(mySRTActiveSurfaceCore.ActuatorFmax_str);
    ActuatorAccelerationlineEdit->clear();
	ActuatorAccelerationlineEdit->insert(mySRTActiveSurfaceCore.ActuatorAcceleration_str);
}

void SRTActiveSurfaceGUI::changeGUIasStatusCode(int asStatusCode)
{
    std::string asStatusColorString, asStatusString;

    switch (asStatusCode) {
        case (Management::MNG_OK):
            asStatusColorString = "background-color: rgb(85, 255, 0);";
            asStatusString = "OK";
            break;
        case (Management::MNG_WARNING):
            asStatusColorString = "background-color: rgb(255, 255, 0);";
            asStatusString = "WARNING";
            break;
        case (Management::MNG_FAILURE):
            asStatusColorString = "background-color: rgb(255, 0, 0);";
            asStatusString = "FAILURE";
            break;
    }

    StatuslineEdit_2->setStyleSheet(QApplication::translate("SRTActiveSurfaceGUI", asStatusColorString.c_str(), 0, QApplication::UnicodeUTF8));
    StatuslineEdit_2->setText(QApplication::translate("SRTActiveSurfaceGUI", asStatusString.c_str(), 0, QApplication::UnicodeUTF8));
}

void SRTActiveSurfaceGUI::changeGUIasProfileCode(int asProfileCode)
{
    std::string asProfileString;
    switch (asProfileCode) {
        case (ActiveSurface::AS_SHAPED):
            asProfileString = "SHAPED";
            break;
        case (ActiveSurface::AS_SHAPED_FIXED):
            asProfileString = "SHAPED FIXED";
            break;
        case (ActiveSurface::AS_PARABOLIC):
            asProfileString = "PARABOLIC";
            break;
        case (ActiveSurface::AS_PARABOLIC_FIXED):
            asProfileString = "PARABOLIC FIXED";
            break;
        case (ActiveSurface::AS_PARK):
            asProfileString = "PARK";
            break;
    }
    ProfilelineEdit_2->setText(QApplication::translate("SRTActiveSurfaceGUI", asProfileString.c_str(), 0, QApplication::UnicodeUTF8));
}

void SRTActiveSurfaceGUI::changeGUIActuatorStatusLabels()
{
    ActuatorStatusRunLabel->clear();
    switch (mySRTActiveSurfaceCore.ActuatorStatusRunLabelCode) {
        case (-1):
            ActuatorStatusRunLabel->setText("STOPPED");
            break;
        case (1):
            ActuatorStatusRunLabel->setText("RUNNING");
            break;
    }

    ActuatorStatusEnblLabel->clear();
    switch (mySRTActiveSurfaceCore.ActuatorStatusEnblLabelCode) {
        case (-1):
            ActuatorStatusEnblLabel->setText("UNABLED");
            break;
        case (1):
            ActuatorStatusEnblLabel->setText("ENABLED");
            break;
    }

    ActuatorStatusCammLabel->clear();
    switch (mySRTActiveSurfaceCore.ActuatorStatusCammLabelCode) {
        case (-1):
            ActuatorStatusCammLabel->setText("NO CAMM");
            break;
        case (1):
            ActuatorStatusCammLabel->setText("CAMM");
            break;
    }

    ActuatorStatusLoopLabel->clear();
    switch (mySRTActiveSurfaceCore.ActuatorStatusLoopLabelCode) {
        case (-1):
            ActuatorStatusLoopLabel->setText("NO LOOP");
            break;
        case (1):
            ActuatorStatusLoopLabel->setText("LOOP");
            break;
    }
     
    ActuatorStatusCalLabel->clear();
    switch (mySRTActiveSurfaceCore.ActuatorStatusCalLabelCode) {
        case (-1):
            ActuatorStatusCalLabel->setText("UNCALIBRATED");
            break;
        case (1):
            ActuatorStatusCalLabel->setText("CALIBRATED");
            break;
    }
}
