// ACS includes
#include <NotoActiveSurfaceGUIui.h>

// QT includes
#include <QtGui/QPalette>
#include <QtGui/QBrush>

int thecircle, theactuator, theradius;
int tcircle, tactuator, tradius;

NotoActiveSurfaceGUI::NotoActiveSurfaceGUI(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIActuatorColor(int,int,bool,bool,bool)), this, SLOT(changeGUIActuatorColor(int,int,bool,bool,bool)));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIAllActuators(bool)), this, SLOT(changeGUIAllActuators(bool)));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIcircleORradius(bool)), this, SLOT(changeGUIcircleORradius(bool)));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIActuator(bool)), this, SLOT(changeGUIActuator(bool)));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIActuatorStatusEnblLabel()), this, SLOT(changeGUIActuatorStatusEnblLabel()));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIActuatorValues()), this, SLOT(changeGUIActuatorValues()));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIActuatorStatusLabels()), this, SLOT(changeGUIActuatorStatusLabels()));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIasStatusCode(int)), this, SLOT(changeGUIasStatusCode(int)));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIasProfileCode(int)), this, SLOT(changeGUIasProfileCode(int)));
    QObject::connect(&myNotoActiveSurfaceCore, SIGNAL(setGUIasLUTFileName(QString)), this, SLOT(changeGUIasLUTFileName(QString)));

#ifdef MANAGEMENT
    buttonGroup1->setEnabled(true);
#else
    buttonGroup1->setVisible(false);
#endif
}

void NotoActiveSurfaceGUI::setParameters(maci::SimpleClient* theClient, ActiveSurface::ActiveSurfaceBoss_var theASBoss)
{
    myNotoActiveSurfaceClientEventLoop.setSimpleClient(theClient);
    myNotoActiveSurfaceCore.setASBoss(theASBoss);
    // Starts the main threads
    myNotoActiveSurfaceClientEventLoop.start();
    myNotoActiveSurfaceCore.monitor = true;
    myNotoActiveSurfaceCore.start();
}

void NotoActiveSurfaceGUI::Quit()
{
    ACS_SHORT_LOG((LM_INFO,"NotoActiveSurfaceCoreGUI::Quit"));
    // Stops Main Threads
    myNotoActiveSurfaceClientEventLoop.stop();
    if (myNotoActiveSurfaceClientEventLoop.isRunning() == true) {
			while (myNotoActiveSurfaceClientEventLoop.isFinished() == false)
				myNotoActiveSurfaceClientEventLoop.wait();
    }
    myNotoActiveSurfaceClientEventLoop.terminate();

    myNotoActiveSurfaceCore.stop();
    if (myNotoActiveSurfaceCore.isRunning() == true) {
			while (myNotoActiveSurfaceCore.isFinished() == false)
				myNotoActiveSurfaceCore.wait();
    }
    myNotoActiveSurfaceCore.terminate();

    // Close the GUI
    close();
}

// ALL ACTUATORS
void NotoActiveSurfaceGUI::setallactuators()
{
    //myNotoActiveSurfaceCore.setallactuators();
    thecircle = 0; theactuator = 0; theradius = 0;
    changeGUIAllActuators();
}

void NotoActiveSurfaceGUI::setradius()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = 0;
    theactuator = 0;
    theradius = std::atoi(name.substr(1, name.find("ActuatorsButton") - 1).c_str());
    changeGUIcircleORradius();
}

void NotoActiveSurfaceGUI::setcircle()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = std::atoi(name.substr(1, name.find("ActuatorsButton") - 1).c_str());
    theactuator = 0;
    theradius = 0;
    changeGUIcircleORradius();
}

void NotoActiveSurfaceGUI::setactuator()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = std::atoi(name.substr(14, 2).c_str());
    theactuator = std::atoi(name.substr(17, 2).c_str());
    theradius=0;
    myNotoActiveSurfaceCore.setactuator(thecircle, theactuator);
}

void NotoActiveSurfaceGUI::move()
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
        myNotoActiveSurfaceCore.move(thecircle,theactuator,theradius,incr);
   }  
}
/*
void NotoActiveSurfaceGUI::setprofile()
{
    QString str;
    long profile;
    
    //profile = (long)SetProfilecomboBox->currentIndex();
    ManagerLabel->clear();
	thecircle = theactuator = theradius = 0;
	str = QString("%1 %2 %3").arg("set").arg("AS").arg("profile");
	ManagerLabel->setText(str);
	myNotoActiveSurfaceCore.setProfile(profile);
}
*/
void NotoActiveSurfaceGUI::correction()
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
	    myNotoActiveSurfaceCore.correction(thecircle,theactuator,theradius,correction);
    }  
}

void NotoActiveSurfaceGUI::update()
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
	    myNotoActiveSurfaceCore.update(elev);
    }
}

void NotoActiveSurfaceGUI::reset()
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
	    myNotoActiveSurfaceCore.reset(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::stop()
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
	    myNotoActiveSurfaceCore.stopUSD(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::up()
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
	    myNotoActiveSurfaceCore.up(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::down()
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
	    myNotoActiveSurfaceCore.down(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::top()
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
	    myNotoActiveSurfaceCore.top(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::bottom()
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
	    myNotoActiveSurfaceCore.bottom(thecircle,theactuator,theradius);
     }
}

void NotoActiveSurfaceGUI::calibrate()
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
	    myNotoActiveSurfaceCore.calibrate(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::calVer()
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
	    myNotoActiveSurfaceCore.calVer(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::stow()
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
	    myNotoActiveSurfaceCore.stow(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::setup()
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
	    myNotoActiveSurfaceCore.setup(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::refPos()
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
	    myNotoActiveSurfaceCore.refPos(thecircle,theactuator,theradius);
    }
}

void NotoActiveSurfaceGUI::setupAS()
{
    setallactuators();
    myNotoActiveSurfaceCore.setup(thecircle,theactuator,theradius);
}

void NotoActiveSurfaceGUI::startAS()
{
    std::string str;
    setallactuators();
    //myNotoActiveSurfaceCore.enableAutoUpdate();
    myNotoActiveSurfaceCore.setParabolicProfile();
    str = "PARABOLIC";
    StatuslineEdit_2->setText(QApplication::translate("NotoActiveSurfaceGUI", str.c_str(), 0, QApplication::UnicodeUTF8));
}

void NotoActiveSurfaceGUI::onAS()
{
    std::string str;
    setallactuators();
    myNotoActiveSurfaceCore.enableAutoUpdate();
    str = "ON";
    StatuslineEdit_2->setText(QApplication::translate("NotoActiveSurfaceGUI", str.c_str(), 0, QApplication::UnicodeUTF8));
}

void NotoActiveSurfaceGUI::offAS()
{
    std::string str;
    setallactuators();
    myNotoActiveSurfaceCore.disableAutoUpdate();
    str = "OFF";
    StatuslineEdit_2->setText(QApplication::translate("NotoActiveSurfaceGUI", str.c_str(), 0, QApplication::UnicodeUTF8));
}

void NotoActiveSurfaceGUI::stowAS()
{
    std::string str;
    setallactuators();
    myNotoActiveSurfaceCore.park();
    str = "PARK";
    StatuslineEdit_2->setText(QApplication::translate("NotoActiveSurfaceGUI", str.c_str(), 0, QApplication::UnicodeUTF8));
}

void NotoActiveSurfaceGUI::stopAS()
{
    std::string str;
    setallactuators();
    //myNotoActiveSurfaceCore.stopUSD(thecircle,theactuator,theradius);
    myNotoActiveSurfaceCore.setParabolicFixedProfile();
    str = "PARABOLIC FIXED";
    StatuslineEdit_2->setText(QApplication::translate("NotoActiveSurfaceGUI", str.c_str(), 0, QApplication::UnicodeUTF8));
}

void NotoActiveSurfaceGUI::recoverUSD()
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
	        myNotoActiveSurfaceCore.recoverUSD(thecircle,theactuator);
	    }
	    else {
	        str = QString("%1").arg("only one actuator per time");
	        ManagerLabel->setText(str);
	    }
    }
}

void NotoActiveSurfaceGUI::changeGUIActuatorColor(int tcircle, int tactuator, bool active, bool statusColor, bool fromRun)
{
    if (!fromRun)
    {
        tcircle = thecircle;
        tactuator = theactuator;
    }

    std::stringstream ActuatorButtonName;
    ActuatorButtonName << "ActuatorButton";
    ActuatorButtonName << std::setfill('0') << std::setw(2) << tcircle;
    ActuatorButtonName << "_";
    ActuatorButtonName << std::setfill('0') << std::setw(2) << tactuator;

    QPushButton* ActuatorButton = this->findChild<QPushButton*>(ActuatorButtonName.str().c_str());

    if(active == true && statusColor == false) // attivo e calibrato
    {
        ActuatorButton->setStyleSheet("background-color: rgb(85, 255, 0)"); // verde
    }
    else if (active == true && statusColor == true) // attivo e scalibrato
    {
        ActuatorButton->setStyleSheet("background-color: rgb(255, 255, 0)"); // giallo
    }
    else
    {
        ActuatorButton->setStyleSheet("background-color: rgb(255, 0, 0)"); // rosso
    }
}


void NotoActiveSurfaceGUI::changeGUIAllActuators(bool callfromfunction)
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

void NotoActiveSurfaceGUI::clearactuatorslineedit()
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

void NotoActiveSurfaceGUI::changeGUIcircleORradius(bool callfromfunction)
{
    //int circle = myNotoActiveSurfaceCore.theCircle;
    //int radius = myNotoActiveSurfaceCore.theRadius;
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

void NotoActiveSurfaceGUI::changeGUIActuator(bool callfromfunction)
{
    //int circle = myNotoActiveSurfaceCore.theCircle;
    //int actuator = myNotoActiveSurfaceCore.theActuator;

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

void NotoActiveSurfaceGUI::changeGUIActuatorStatusEnblLabel()
{
    ActuatorStatusEnblLabel->clear();
    switch (myNotoActiveSurfaceCore.ActuatorStatusEnblLabelCode) {
        case (1):
            ActuatorStatusEnblLabel->setText("ENABLED");
            break;
        case (-1):
            ActuatorStatusEnblLabel->setText("UNABLED");
            break;
    }
}

void NotoActiveSurfaceGUI::changeGUIActuatorValues()
{
    stepActuatorActualPositionlineEdit->clear();
	stepActuatorActualPositionlineEdit->insert(myNotoActiveSurfaceCore.ActuatorActualPosition_str);
    mmActuatorActualPositionlineEdit->clear();
	mmActuatorActualPositionlineEdit->insert(myNotoActiveSurfaceCore.mm_ActuatorActualPosition_str);
    stepActuatorCommandedPositionlineEdit->clear();
	stepActuatorCommandedPositionlineEdit->insert(myNotoActiveSurfaceCore.ActuatorCommandedPosition_str);
    mmActuatorCommandedPositionlineEdit->clear();
	mmActuatorCommandedPositionlineEdit->insert(myNotoActiveSurfaceCore.mm_ActuatorCommandedPosition_str);
    ActuatorDelaylineEdit->clear();
	ActuatorDelaylineEdit->insert(myNotoActiveSurfaceCore.ActuatorDelay_str);
    ActuatorFminlineEdit->clear();
	ActuatorFminlineEdit->insert(myNotoActiveSurfaceCore.ActuatorFmin_str);
    ActuatorFmaxlineEdit->clear();
	ActuatorFmaxlineEdit->insert(myNotoActiveSurfaceCore.ActuatorFmax_str);
    ActuatorAccelerationlineEdit->clear();
	ActuatorAccelerationlineEdit->insert(myNotoActiveSurfaceCore.ActuatorAcceleration_str);
}

void NotoActiveSurfaceGUI::changeGUIasStatusCode(int asStatusCode)
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

    StatuslineEdit_2->setStyleSheet(QApplication::translate("NotoActiveSurfaceGUI", asStatusColorString.c_str(), 0, QApplication::UnicodeUTF8));
    StatuslineEdit_2->setText(QApplication::translate("NotoActiveSurfaceGUI", asStatusString.c_str(), 0, QApplication::UnicodeUTF8));
}

void NotoActiveSurfaceGUI::changeGUIasProfileCode(int asProfileCode)
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
    StatuslineEdit_2->setText(QApplication::translate("NotoActiveSurfaceGUI", asProfileString.c_str(), 0, QApplication::UnicodeUTF8));
}

void NotoActiveSurfaceGUI::changeGUIasLUTFileName(QString filename)
{
    LUTLabelFileName->setText(filename);
}

void NotoActiveSurfaceGUI::changeGUIActuatorStatusLabels()
{
    ActuatorStatusRunLabel->clear();
    switch (myNotoActiveSurfaceCore.ActuatorStatusRunLabelCode) {
        case (0):
            ActuatorStatusRunLabel->setText("");
            break;
        case (-1):
            ActuatorStatusRunLabel->setText("STOPPED");
            break;
        case (1):
            ActuatorStatusRunLabel->setText("RUNNING");
            break;
    }

    ActuatorStatusEnblLabel->clear();
    switch (myNotoActiveSurfaceCore.ActuatorStatusEnblLabelCode) {
        case (-1):
            ActuatorStatusEnblLabel->setText("UNABLED");
            break;
        case (1):
            ActuatorStatusEnblLabel->setText("ENABLED");
            break;
    }

    ActuatorStatusCammLabel->clear();
    switch (myNotoActiveSurfaceCore.ActuatorStatusCammLabelCode) {
        case (0):
            ActuatorStatusCammLabel->setText("");
            break;
        case (-1):
            ActuatorStatusCammLabel->setText("NO CAMM");
            break;
        case (1):
            ActuatorStatusCammLabel->setText("CAMM");
            break;
    }

    ActuatorStatusLoopLabel->clear();
    switch (myNotoActiveSurfaceCore.ActuatorStatusLoopLabelCode) {
        case (0):
            ActuatorStatusLoopLabel->setText("");
            break;
        case (-1):
            ActuatorStatusLoopLabel->setText("NO LOOP");
            break;
        case (1):
            ActuatorStatusLoopLabel->setText("LOOP");
            break;
    }
     
    ActuatorStatusCalLabel->clear();
    switch (myNotoActiveSurfaceCore.ActuatorStatusCalLabelCode) {
        case (0):
            ActuatorStatusCalLabel->setText("");
            break;
        case (-1):
            ActuatorStatusCalLabel->setText("UNCALIBRATED");
            break;
        case (1):
            ActuatorStatusCalLabel->setText("CALIBRATED");
            break;
    }
}
