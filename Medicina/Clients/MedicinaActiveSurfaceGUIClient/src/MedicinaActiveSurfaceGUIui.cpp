// ACS includes
#include <MedicinaActiveSurfaceGUIui.h>

// QT includes
#include <QtGui/QPalette>
#include <QtGui/QBrush>

int thecircle, theactuator, theradius;
int tcircle, tactuator, tradius;

MedicinaActiveSurfaceGUI::MedicinaActiveSurfaceGUI(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIActuatorColor(int,int,bool,bool)), this, SLOT(changeGUIActuatorColor(int,int,bool,bool)));
    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIAllActuators(bool)), this, SLOT(changeGUIAllActuators(bool)));
    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIcircleORradius(bool)), this, SLOT(changeGUIcircleORradius(bool)));
    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIActuator(bool)), this, SLOT(changeGUIActuator(bool)));
    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIActuatorStatusEnblLabel()), this, SLOT(changeGUIActuatorStatusEnblLabel()));
    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIActuatorValues()), this, SLOT(changeGUIActuatorValues()));
    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIActuatorStatusLabels()), this, SLOT(changeGUIActuatorStatusLabels()));
    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIasStatusCode(int)), this, SLOT(changeGUIasStatusCode(int)));
    QObject::connect(&myMedicinaActiveSurfaceCore, SIGNAL(setGUIasProfileCode(int)), this, SLOT(changeGUIasProfileCode(int)));

#ifdef MANAGEMENT
    buttonGroup1->setEnabled(true);
#else
    buttonGroup1->setVisible(false);
#endif
}

void MedicinaActiveSurfaceGUI::setParameters(maci::SimpleClient* theClient, ActiveSurface::MedicinaActiveSurfaceBoss_var theASBoss)
{
    myMedicinaActiveSurfaceClientEventLoop.setSimpleClient(theClient);
    myMedicinaActiveSurfaceCore.setASBoss(theASBoss);
    // Starts the main threads
    myMedicinaActiveSurfaceClientEventLoop.start();
    myMedicinaActiveSurfaceCore.monitor = true;
    myMedicinaActiveSurfaceCore.start();
}

void MedicinaActiveSurfaceGUI::Quit()
{
    ACS_SHORT_LOG((LM_INFO,"MedicinaActiveSurfaceCoreGUI::Quit"));
    // Stops Main Threads
    myMedicinaActiveSurfaceClientEventLoop.stop();
    if (myMedicinaActiveSurfaceClientEventLoop.isRunning() == true) {
			while (myMedicinaActiveSurfaceClientEventLoop.isFinished() == false)
				myMedicinaActiveSurfaceClientEventLoop.wait();
    }
    myMedicinaActiveSurfaceClientEventLoop.terminate();

    myMedicinaActiveSurfaceCore.stop();
    if (myMedicinaActiveSurfaceCore.isRunning() == true) {
			while (myMedicinaActiveSurfaceCore.isFinished() == false)
				myMedicinaActiveSurfaceCore.wait();
    }
    myMedicinaActiveSurfaceCore.terminate();

    // Close the GUI
    close();
}

// ALL ACTUATORS
void MedicinaActiveSurfaceGUI::setallactuators()
{
    //myMedicinaActiveSurfaceCore.setallactuators();
    thecircle = 0; theactuator = 0; theradius = 0;
    changeGUIAllActuators();
}

void MedicinaActiveSurfaceGUI::setradius()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = 0;
    theactuator = 0;
    theradius = std::atoi(name.substr(1, name.find("ActuatorsButton") - 1).c_str());
    changeGUIcircleORradius();
}

void MedicinaActiveSurfaceGUI::setcircle()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = std::atoi(name.substr(1, name.find("ActuatorsButton") - 1).c_str());
    theactuator = 0;
    theradius = 0;
    changeGUIcircleORradius();
}

void MedicinaActiveSurfaceGUI::setactuator()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = std::atoi(name.substr(14, 2).c_str());
    theactuator = std::atoi(name.substr(17, 2).c_str());
    theradius=0;
    myMedicinaActiveSurfaceCore.setactuator(thecircle, theactuator);
}

void MedicinaActiveSurfaceGUI::move()
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
        myMedicinaActiveSurfaceCore.move(thecircle,theactuator,theradius,incr);
   }  
}
/*
void MedicinaActiveSurfaceGUI::setprofile()
{
    QString str;
    long profile;
    
    //profile = (long)SetProfilecomboBox->currentIndex();
    ManagerLabel->clear();
	thecircle = theactuator = theradius = 0;
	str = QString("%1 %2 %3").arg("set").arg("AS").arg("profile");
	ManagerLabel->setText(str);
	myMedicinaActiveSurfaceCore.setProfile(profile);
}
*/
void MedicinaActiveSurfaceGUI::correction()
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
	    myMedicinaActiveSurfaceCore.correction(thecircle,theactuator,theradius,correction);
    }  
}

void MedicinaActiveSurfaceGUI::update()
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
	    myMedicinaActiveSurfaceCore.update(elev);
    }
}

void MedicinaActiveSurfaceGUI::reset()
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
	    myMedicinaActiveSurfaceCore.reset(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::stop()
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
	    myMedicinaActiveSurfaceCore.stopUSD(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::up()
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
	    myMedicinaActiveSurfaceCore.up(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::down()
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
	    myMedicinaActiveSurfaceCore.down(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::top()
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
	    myMedicinaActiveSurfaceCore.top(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::bottom()
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
	    myMedicinaActiveSurfaceCore.bottom(thecircle,theactuator,theradius);
     }
}

void MedicinaActiveSurfaceGUI::calibrate()
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
	    myMedicinaActiveSurfaceCore.calibrate(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::calVer()
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
	    myMedicinaActiveSurfaceCore.calVer(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::stow()
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
	    myMedicinaActiveSurfaceCore.stow(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::setup()
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
	    myMedicinaActiveSurfaceCore.setup(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::refPos()
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
	    myMedicinaActiveSurfaceCore.refPos(thecircle,theactuator,theradius);
    }
}

void MedicinaActiveSurfaceGUI::setupAS()
{
    setallactuators();
    myMedicinaActiveSurfaceCore.setup(thecircle,theactuator,theradius);
}

void MedicinaActiveSurfaceGUI::startAS()
{
    std::string str;
    setallactuators();
    //myMedicinaActiveSurfaceCore.enableAutoUpdate();
    myMedicinaActiveSurfaceCore.setParabolicProfile();
    str = "PARABOLIC";
    StatuslineEdit_2->setText(QApplication::translate("MedicinaActiveSurfaceGUI", str.c_str(), 0, QApplication::UnicodeUTF8));
}

void MedicinaActiveSurfaceGUI::stowAS()
{
    std::string str;
    setallactuators();
    myMedicinaActiveSurfaceCore.park();
    str = "PARK";
    StatuslineEdit_2->setText(QApplication::translate("MedicinaActiveSurfaceGUI", str.c_str(), 0, QApplication::UnicodeUTF8));
}

void MedicinaActiveSurfaceGUI::stopAS()
{
    std::string str;
    setallactuators();
    //myMedicinaActiveSurfaceCore.stopUSD(thecircle,theactuator,theradius);
    myMedicinaActiveSurfaceCore.setParabolicFixedProfile();
    str = "PARABOLIC FIXED";
    StatuslineEdit_2->setText(QApplication::translate("MedicinaActiveSurfaceGUI", str.c_str(), 0, QApplication::UnicodeUTF8));
}

void MedicinaActiveSurfaceGUI::recoverUSD()
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
	        myMedicinaActiveSurfaceCore.recoverUSD(thecircle,theactuator);
	    }
	    else {
	        str = QString("%1").arg("only one actuator per time");
	        ManagerLabel->setText(str);
	    }
    }
}

void MedicinaActiveSurfaceGUI::changeGUIActuatorColor(int tcircle, int tactuator, bool active, bool fromRun)
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

    if(active)
    {
        ActuatorButton->setStyleSheet("background-color: rgb(85, 255, 0)");
    }
    else
    {
        ActuatorButton->setStyleSheet("background-color: rgb(255, 0, 0)");
    }
}


void MedicinaActiveSurfaceGUI::changeGUIAllActuators(bool callfromfunction)
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

void MedicinaActiveSurfaceGUI::clearactuatorslineedit()
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

void MedicinaActiveSurfaceGUI::changeGUIcircleORradius(bool callfromfunction)
{
    //int circle = myMedicinaActiveSurfaceCore.theCircle;
    //int radius = myMedicinaActiveSurfaceCore.theRadius;
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

void MedicinaActiveSurfaceGUI::changeGUIActuator(bool callfromfunction)
{
    //int circle = myMedicinaActiveSurfaceCore.theCircle;
    //int actuator = myMedicinaActiveSurfaceCore.theActuator;

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

void MedicinaActiveSurfaceGUI::changeGUIActuatorStatusEnblLabel()
{
    ActuatorStatusEnblLabel->clear();
    switch (myMedicinaActiveSurfaceCore.ActuatorStatusEnblLabelCode) {
        case (1):
            ActuatorStatusEnblLabel->setText("ENABLED");
            break;
        case (-1):
            ActuatorStatusEnblLabel->setText("UNABLED");
            break;
    }
}

void MedicinaActiveSurfaceGUI::changeGUIActuatorValues()
{
    stepActuatorActualPositionlineEdit->clear();
	stepActuatorActualPositionlineEdit->insert(myMedicinaActiveSurfaceCore.ActuatorActualPosition_str);
    mmActuatorActualPositionlineEdit->clear();
	mmActuatorActualPositionlineEdit->insert(myMedicinaActiveSurfaceCore.mm_ActuatorActualPosition_str);
    stepActuatorCommandedPositionlineEdit->clear();
	stepActuatorCommandedPositionlineEdit->insert(myMedicinaActiveSurfaceCore.ActuatorCommandedPosition_str);
    mmActuatorCommandedPositionlineEdit->clear();
	mmActuatorCommandedPositionlineEdit->insert(myMedicinaActiveSurfaceCore.mm_ActuatorCommandedPosition_str);
    ActuatorDelaylineEdit->clear();
	ActuatorDelaylineEdit->insert(myMedicinaActiveSurfaceCore.ActuatorDelay_str);
    ActuatorFminlineEdit->clear();
	ActuatorFminlineEdit->insert(myMedicinaActiveSurfaceCore.ActuatorFmin_str);
    ActuatorFmaxlineEdit->clear();
	ActuatorFmaxlineEdit->insert(myMedicinaActiveSurfaceCore.ActuatorFmax_str);
    ActuatorAccelerationlineEdit->clear();
	ActuatorAccelerationlineEdit->insert(myMedicinaActiveSurfaceCore.ActuatorAcceleration_str);
}

void MedicinaActiveSurfaceGUI::changeGUIasStatusCode(int asStatusCode)
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

    StatuslineEdit_2->setStyleSheet(QApplication::translate("MedicinaActiveSurfaceGUI", asStatusColorString.c_str(), 0, QApplication::UnicodeUTF8));
    StatuslineEdit_2->setText(QApplication::translate("MedicinaActiveSurfaceGUI", asStatusString.c_str(), 0, QApplication::UnicodeUTF8));
}

void MedicinaActiveSurfaceGUI::changeGUIasProfileCode(int asProfileCode)
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
    StatuslineEdit_2->setText(QApplication::translate("MedicinaActiveSurfaceGUI", asProfileString.c_str(), 0, QApplication::UnicodeUTF8));
}

void MedicinaActiveSurfaceGUI::changeGUIActuatorStatusLabels()
{
    ActuatorStatusRunLabel->clear();
    switch (myMedicinaActiveSurfaceCore.ActuatorStatusRunLabelCode) {
        case (-1):
            ActuatorStatusRunLabel->setText("STOPPED");
            break;
        case (1):
            ActuatorStatusRunLabel->setText("RUNNING");
            break;
    }

    ActuatorStatusEnblLabel->clear();
    switch (myMedicinaActiveSurfaceCore.ActuatorStatusEnblLabelCode) {
        case (-1):
            ActuatorStatusEnblLabel->setText("UNABLED");
            break;
        case (1):
            ActuatorStatusEnblLabel->setText("ENABLED");
            break;
    }

    ActuatorStatusCammLabel->clear();
    switch (myMedicinaActiveSurfaceCore.ActuatorStatusCammLabelCode) {
        case (-1):
            ActuatorStatusCammLabel->setText("NO CAMM");
            break;
        case (1):
            ActuatorStatusCammLabel->setText("CAMM");
            break;
    }

    ActuatorStatusLoopLabel->clear();
    switch (myMedicinaActiveSurfaceCore.ActuatorStatusLoopLabelCode) {
        case (-1):
            ActuatorStatusLoopLabel->setText("NO LOOP");
            break;
        case (1):
            ActuatorStatusLoopLabel->setText("LOOP");
            break;
    }
     
    ActuatorStatusCalLabel->clear();
    switch (myMedicinaActiveSurfaceCore.ActuatorStatusCalLabelCode) {
        case (-1):
            ActuatorStatusCalLabel->setText("UNCALIBRATED");
            break;
        case (1):
            ActuatorStatusCalLabel->setText("CALIBRATED");
            break;
    }
}
