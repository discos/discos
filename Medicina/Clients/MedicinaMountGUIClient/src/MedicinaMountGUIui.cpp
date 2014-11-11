// ACS includes
#include <MedicinaMountGUIui.h>
// QT includes
#include <QtGui/QPalette>
#include <QtGui/QBrush>

MedicinaMountGUI::MedicinaMountGUI(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
/*
    QObject::connect(StowButton, SIGNAL(clicked()), this, SLOT(Stow()));
    QObject::connect(UnStowButton, SIGNAL(clicked()), this, SLOT(UnStow()));
    QObject::connect(StopButton, SIGNAL(clicked()), this, SLOT(Stop()));
    QObject::connect(RatesButton, SIGNAL(clicked()), this, SLOT(Rates()));
    QObject::connect(ChangeModeButton, SIGNAL(clicked()), this, SLOT(AzElMode()));
    QObject::connect(PresetButton, SIGNAL(clicked()), this, SLOT(Preset()));
    QObject::connect(ProgramTrackButton, SIGNAL(clicked()), this, SLOT(ProgramTrack()));
    QObject::connect(ACUResetFailuresButton, SIGNAL(clicked()), this, SLOT(ResetFailures()));
    QObject::connect(ACUSetTimeButton, SIGNAL(clicked()), this, SLOT(SetTime()));
    QObject::connect(QuitButton, SIGNAL(clicked()), this, SLOT(Quit()));
*/
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIAntennaMountStatusColor()), this, SLOT(changeGUIAntennaMountStatusColor()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIACUStatusColor()), this, SLOT(changeGUIACUStatusColor()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIPositionAndErrorValue()), this, SLOT(changeGUIPositionAndErrorValue()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIACUTimeValue()), this, SLOT(changeGUIACUTimeValue()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIAntennaSectionValue()), this, SLOT(changeGUIAntennaSectionValue()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIServoSystemColors()), this, SLOT(changeGUIServoSystemColors()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIAzimuthElevationServoStatusColors()), this, SLOT(changeGUIAzimuthElevationServoStatusColors()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIAzimuthElevationOperatingModeColors()), this, SLOT(changeGUIAzimuthElevationOperatingModeColors()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUIPropertyErrorCode()), this, SLOT(changeGUIPropertyErrorCode()));
    QObject::connect(&myMedicinaMount, SIGNAL(setGUICatchAllErrorCode()), this, SLOT(changeGUICatchAllErrorCode()));
}

void MedicinaMountGUI::setParameters(maci::SimpleClient* theClient, Antenna::MedicinaMount_var theMount)
{
    myMedicinaMountClientEventLoop.setSimpleClient(theClient);
    myMedicinaMount.setMount(theMount);
    // Starts the main threads
    myMedicinaMountClientEventLoop.start();
    myMedicinaMount.monitor = true;
    myMedicinaMount.start();
}

void MedicinaMountGUI::Stow()
{
    myMedicinaMount.stow();
}

void MedicinaMountGUI::UnStow()
{
    myMedicinaMount.unstow();
}

void MedicinaMountGUI::Stop()
{
    myMedicinaMount.acustop();
}

void MedicinaMountGUI::ResetFailures()
{
    myMedicinaMount.resetfailures();
}

void MedicinaMountGUI::Rates()
{
    double azrate, elrate;
    QString AZratestr, ELratestr;
    AZratestr = AZratelineEdit->text();
    azrate = AZratestr.toDouble();
    ELratestr = ELratelineEdit->text();
    elrate = ELratestr.toDouble();
    myMedicinaMount.Rates(azrate,elrate);
}

void MedicinaMountGUI::SetTime()
{
    ACS::Time tm;
    QString Timestr;
    Timestr = ACUsetTimelineEdit->text();
    tm = Timestr.toLongLong();
    myMedicinaMount.SetTime(tm);
}

void MedicinaMountGUI::AzElMode()
{
    int AZmode;
    int ELmode;
    AZmode = AZModecomboBox->currentIndex();
    ELmode = ELModecomboBox->currentIndex();
    myMedicinaMount.changeAZELmode(AZmode, ELmode);
}

void MedicinaMountGUI::ProgramTrack()
{
    double az, el;
    QString AZstr, ELstr, Timestr;
    AZstr = ComAZlineEdit->text();
    az = AZstr.toDouble();
    ELstr = ComELlineEdit->text();
    el = ELstr.toDouble();
    ACS::Time tm;
    Timestr = TimeCommandedlineEdit->text();
    tm = Timestr.toLongLong();
    myMedicinaMount.ProgramTrack(az, el, tm, true);
}

void MedicinaMountGUI::Preset()
{
    double az, el;
    QString AZstr, ELstr;
    AZstr = ComAZlineEdit->text();
    az = AZstr.toDouble();
    ELstr = ComELlineEdit->text();
    el = ELstr.toDouble();
    myMedicinaMount.Preset(az,el);
}

void MedicinaMountGUI::Quit()
{
    ACS_SHORT_LOG((LM_INFO,"MedicinaMountGUI::Quit"));
    // Stops Main Threads
    myMedicinaMountClientEventLoop.stop();
    if (myMedicinaMountClientEventLoop.isRunning() == true) {
			while (myMedicinaMountClientEventLoop.isFinished() == false)
				myMedicinaMountClientEventLoop.wait();
    }
    myMedicinaMountClientEventLoop.terminate();

    myMedicinaMount.stop();
    if (myMedicinaMount.isRunning() == true) {
			while (myMedicinaMount.isFinished() == false)
				myMedicinaMount.wait();
    }
    myMedicinaMount.terminate();

    // Close the GUI
    close();
}

void MedicinaMountGUI::changeGUIAntennaMountStatusColor()
{
    switch(myMedicinaMount.AntennaMountStatusCode) {
        case (1):
            AntennaMountStatusLabel_1->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
            AntennaMountStatusLabel_1->setText(QApplication::translate("MedicinaMountGUI", "OK", 0, QApplication::UnicodeUTF8));
            break;
        case(0):
            AntennaMountStatusLabel_1->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
            AntennaMountStatusLabel_1->setText(QApplication::translate("MedicinaMountGUI", "WARNING", 0, QApplication::UnicodeUTF8));
            break;
        case(-1):
            AntennaMountStatusLabel_1->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
            AntennaMountStatusLabel_1->setText(QApplication::translate("MedicinaMountGUI", "FAILURE", 0, QApplication::UnicodeUTF8));
            break;
    }
}

void MedicinaMountGUI::changeGUIACUStatusColor()
{
    switch(myMedicinaMount.ACUStatusCode) {
        case (1):
            ACUStatusLabel_1->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 255, 0);"));
            ACUStatusLabel_1->setText(QApplication::translate("MedicinaMountGUI", "CONNECTED", 0, QApplication::UnicodeUTF8));
            break;
        case(0):
            ACUStatusLabel_1->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
            ACUStatusLabel_1->setText(QApplication::translate("MedicinaMountGUI", "CONNECTING", 0, QApplication::UnicodeUTF8));
            break;
        case(-1):
            ACUStatusLabel_1->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
            ACUStatusLabel_1->setText(QApplication::translate("MedicinaMountGUI", "NOT CONNECTED", 0, QApplication::UnicodeUTF8));
            break;
        case(-2):
            ACUStatusLabel_1->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
            ACUStatusLabel_1->setText(QApplication::translate("MedicinaMountGUI", "BUSY", 0, QApplication::UnicodeUTF8));
            break;
    }
}

void MedicinaMountGUI::changeGUIPositionAndErrorValue()
{
    QString PositionAndErrorValueString;

    PositionAndErrorValueString.setNum(myMedicinaMount.PositionAndErrorValue,'f',4);

    switch (myMedicinaMount.PositionAndErrorCode) {
        case(1):
            ActAZlineEdit->clear();
            ActAZlineEdit->insert(PositionAndErrorValueString);
            break;
        case(2):
            ComAZlineEdit->clear();
            ComAZlineEdit->insert(PositionAndErrorValueString);
            break;
        case(3):
            AzErrPoslineEdit->clear();
            AzErrPoslineEdit->insert(PositionAndErrorValueString);
            break;
        case(4):
            ActELlineEdit->clear();
            ActELlineEdit->insert(PositionAndErrorValueString);
            break;
        case(5):
            ComELlineEdit->clear();
            ComELlineEdit->insert(PositionAndErrorValueString);
            break;
        case(6):
            ElErrPoslineEdit->clear();
            ElErrPoslineEdit->insert(PositionAndErrorValueString);
            break;
    }
}

void MedicinaMountGUI::changeGUIACUTimeValue()
{
    TimeLineEdit->clear();
    TimeLineEdit->insert(myMedicinaMount.ACUTimeString);
}

void MedicinaMountGUI::changeGUIAntennaSectionValue()
{
    cwccwtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
    cwccwtextLabel->setText(myMedicinaMount.AntennaSectionString);
}

void MedicinaMountGUI::changeGUIServoSystemColors()
{
    if (myMedicinaMount.DoorInterlockCode == 1)
        doorinterlocktextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
    else
        doorinterlocktextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));
    
    if (myMedicinaMount.SafeCode == 1)
        safetextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
    else
        safetextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

    if (myMedicinaMount.EmergencyOffCode == 1)
        emergencyofftextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
    else
        emergencyofftextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

    if (myMedicinaMount.TransferErrorCode == 1)
        transfererrortextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
    else
        transfererrortextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

    if (myMedicinaMount.TimeErrorCode == 1)
        timeerrortextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
    else
        timeerrortextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

    if (myMedicinaMount.RFInhibitCode == 1)
        rfinhibittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
    else
        rfinhibittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

    if (myMedicinaMount.RemoteLocalControlCode == 1) {
        remotetextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
        RemoteControlLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
        LocalControlLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));
    }
    else {
        remotetextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));
        RemoteControlLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));
        LocalControlLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
    }
}

void MedicinaMountGUI::changeGUIAzimuthElevationServoStatusColors()
{
    switch(myMedicinaMount.AzimuthElevationServoStatusCode) {
        case(1): // AZ
            if (myMedicinaMount.EmergencyLimit == 1)
                AZemergencylimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZemergencylimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.ccwLimit == 1)
                AZccwlimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZccwlimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.cwLimit == 1)
                AZcwlimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZcwlimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.ccwPrelimit == 1)
                AZccwprelimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZccwprelimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.cwPrelimit == 1)
                AZcwprelimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZcwprelimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.Stow == 1)
                AZstowtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZstowtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.pinInserted == 1)
                AZpininsertedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZpininsertedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.pinRetracted == 1)
                AZpinretractedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZpinretractedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.servoFailure == 1)
                AZservofailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZservofailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.brakeFailure == 1)
                AZbrakefailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZbrakefailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.encoderFailure == 1)
                AZencoderfailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZencoderfailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.motionFailure == 1)
                AZmotionfailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZmotionfailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.MotorOverSpeed == 1)
                AZmotoroverspeedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZmotoroverspeedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.AuxMode == 1)
                AZauxiliarymodetextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZauxiliarymodetextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.AxisDisabled == 1)
                AZaxisdisabledtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZaxisdisabledtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.ComputerDisabled == 1)
                AZcomputerdisabledtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                AZcomputerdisabledtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));
            break;

        case (2): // EL
            if (myMedicinaMount.EmergencyLimit == 1)
                ELemergencylimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELemergencylimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.ccwLimit == 1)
                ELccwlimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELccwlimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.cwLimit == 1)
                ELcwlimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELcwlimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.ccwPrelimit == 1)
                ELccwprelimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELccwprelimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.cwPrelimit == 1)
                ELcwprelimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELcwprelimittextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.Stow == 1)
                ELstowtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELstowtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.pinInserted == 1)
                ELpininsertedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELpininsertedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.pinRetracted == 1)
                ELpinretractedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(0, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELpinretractedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.servoFailure == 1)
                ELservofailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELservofailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.brakeFailure == 1)
                ELbrakefailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELbrakefailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.encoderFailure == 1)
                ELencoderfailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELencoderfailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.motionFailure == 1)
                ELmotionfailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELmotionfailuretextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.MotorOverSpeed == 1)
                ELmotoroverspeedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELmotoroverspeedtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.AuxMode == 1)
                ELauxiliarymodetextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELauxiliarymodetextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.AxisDisabled == 1)
                ELaxisdisabledtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 0, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELaxisdisabledtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));

            if (myMedicinaMount.ComputerDisabled == 1)
                ELcomputerdisabledtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(255, 255, 0);", 0, QApplication::UnicodeUTF8));
            else
                ELcomputerdisabledtextLabel->setStyleSheet(QApplication::translate("MedicinaMountGUI", "background-color: rgb(211, 211, 211);", 0, QApplication::UnicodeUTF8));
            break;
    }
}

void MedicinaMountGUI::changeGUIAzimuthElevationOperatingModeColors()
{
    switch (myMedicinaMount.AzimuthElevationOperatingMode) {
        case (1): // AZ
            if (myMedicinaMount.AzElOperatingMode == 1) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "STANDBY", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 2) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "STOP", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 3) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "PRESET", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 4) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "ProTRACK", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 5) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "RATE", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 6) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "STOW", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 7) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "PosTRACK", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 8) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "UNSTOW", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 9) {
                AZmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
                AZmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "UNKNOWN", 0, QApplication::UnicodeUTF8));
            }
            break;
        case (2): // EL
            if (myMedicinaMount.AzElOperatingMode == 1) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "STANDBY", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 2) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "STOP", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 3) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "PRESET", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 4) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "ProTRACK", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 5) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "RATE", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 6) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "STOW", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 7) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "PosTRACK", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 8) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "UNSTOW", 0, QApplication::UnicodeUTF8));
            }
            else if (myMedicinaMount.AzElOperatingMode == 9) {
                ELmodetextLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
                ELmodetextLabel->setText(QApplication::translate("MedicinaMountGUI", "UNKNOWN", 0, QApplication::UnicodeUTF8));
            }
            break;
    }
}

void MedicinaMountGUI::changeGUIPropertyErrorCode()
{
    ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));

    switch (myMedicinaMount.PropertyErrorCode) {
        case(1):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to ACU Status", 0, QApplication::UnicodeUTF8));
            break;
        case(2):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Antenna Mount Status", 0, QApplication::UnicodeUTF8));
            break;
        case(3):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to ACU Time", 0, QApplication::UnicodeUTF8));
            break;
        case(4):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Antenna Section", 0, QApplication::UnicodeUTF8));
            break;
        case(5):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Servo System Status", 0, QApplication::UnicodeUTF8));
            break;
        case(6):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Azimuth Servo Status", 0, QApplication::UnicodeUTF8));
            break;
        case(7):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Elevation Servo Status", 0, QApplication::UnicodeUTF8));
            break;
        case(8):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Actual Azimuth Position", 0, QApplication::UnicodeUTF8));
            break;
        case(9):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Commanded Azimuth Position", 0, QApplication::UnicodeUTF8));
            break;
        case(10):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Azimuth Error Position", 0, QApplication::UnicodeUTF8));
            break;
        case(11):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Actual Elevation Position", 0, QApplication::UnicodeUTF8));
            break;
        case(12):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Commanded Elevation Position", 0, QApplication::UnicodeUTF8));
            break;
        case(13):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Elevation Error Position", 0, QApplication::UnicodeUTF8));
            break;
        case(14):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Azimuth Operation Mode", 0, QApplication::UnicodeUTF8));
            break;
        case(15):
            ManagerLabel->setText(QApplication::translate("MedicinaMountGUI", "No access to Elevation Operation Mode", 0, QApplication::UnicodeUTF8));
            break;
    }
}

void MedicinaMountGUI::changeGUICatchAllErrorCode()
{
    ManagerLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
    ManagerLabel->setText(myMedicinaMount.QCatchAllError);
}
