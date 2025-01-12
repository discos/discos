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

    QObject::connect(&mySRTActiveSurfaceCore, SIGNAL(setGUIActuatorColor(int,int,bool,bool,bool)), this, SLOT(changeGUIActuatorColor(int,int,bool,bool,bool)));
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

void SRTActiveSurfaceGUI::setradius()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = 0;
    theactuator = 0;
    theradius = std::atoi(name.substr(1, name.find("ActuatorsButton") - 1).c_str());
    changeGUIcircleORradius();
}

void SRTActiveSurfaceGUI::setcircle()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = std::atoi(name.substr(1, name.find("ActuatorsButton") - 1).c_str());
    theactuator = 0;
    theradius = 0;
    changeGUIcircleORradius();
}

void SRTActiveSurfaceGUI::setactuator()
{
    QObject *obj = QObject::sender();
    std::string name = obj->objectName().toStdString();
    thecircle = std::atoi(name.substr(14, 2).c_str());
    theactuator = std::atoi(name.substr(17, 2).c_str());
    theradius=0;
    mySRTActiveSurfaceCore.setactuator(thecircle, theactuator);
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

void SRTActiveSurfaceGUI::changeGUIActuatorColor(int tcircle, int tactuator, bool active, bool statusColor, bool fromRun)
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
    switch (mySRTActiveSurfaceCore.ActuatorStatusLoopLabelCode) {
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
    switch (mySRTActiveSurfaceCore.ActuatorStatusCalLabelCode) {
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
