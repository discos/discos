// System includes
#include <ctime>
#include <deque>
#include <exception>

// ACS includes
#include <acstime.h>
#include <NotoActiveSurfaceCore.h>

// mask pattern for status 
#define UNAV    0xFF000000
#define MRUN    0x000080
#define CAMM    0x000100
#define ENBL    0x002000
#define PAUT    0x000800
#define CAL     0x008000

#define STEP2MM 0.000714286
//1step del driver = 0.000714286mm di corsa lineare Attuatore.

using namespace IRA;

/*
void NotoActiveSurfaceCore::setGUI(NotoActiveSurfaceGUI* theGUI)
{
    tGUI=theGUI;
    tGUI->EnableButton->setPaletteBackgroundColor(QColor(0,170,0));
    actuatorcounter = circlecounter = totacts = 1;
}

void NotoActiveSurfaceCore::setSimpleClient (maci::SimpleClient* theClient)
{
    sc = theClient;
}
*/

NotoActiveSurfaceCore::NotoActiveSurfaceCore (QObject *parent) : QThread(parent)
{
}

NotoActiveSurfaceCore::~NotoActiveSurfaceCore()
{
}

void NotoActiveSurfaceCore::setASBoss (ActiveSurface::ActiveSurfaceBoss_var ASBoss)
{
    tASBoss = ASBoss;
    actuatorcounter = circlecounter = totacts = 1;
}

void NotoActiveSurfaceCore::stop (void)
{
    monitor = false;
}

void NotoActiveSurfaceCore::run(void)
{
    ACSErr::Completion_var completion;

    Management::ROTSystemStatus_var bossStatus_var;
    ActiveSurface::ROTASProfile_var asProfile_var;
    ACS::ROstring_var asLUTFileName_var;

    while (monitor == true)
    {
        TIMEVALUE clock;
        CIRATools::getTime(clock);
        ACS::Time t0 = clock.value().value;

        bossStatus_var = tASBoss->status();
        emit setGUIasStatusCode((int)bossStatus_var->get_sync(completion.out()));

        asProfile_var = tASBoss->pprofile();
        emit setGUIasProfileCode((int)asProfile_var->get_sync(completion.out()));

        asLUTFileName_var = tASBoss->LUT_filename();
        emit setGUIasLUTFileName(QString(asLUTFileName_var->get_sync(completion.out())));

        // This will have a length of 244
        ACS::longSeq_var asStatus;
        tASBoss->asStatus4GUIClient(asStatus);

        for(unsigned int actuator = 1, i, l; actuator < 269; actuator++, l++)
        {
            if (actuator >= 1 && actuator <= 24) // 1 circle
                i = 1;
            if (actuator >= 25 && actuator <= 72)  // 2 circle
                i = 2;
            if (actuator >= 73 && actuator <= 120)  // 3 circle
                i = 3;
            if (actuator >= 121 && actuator <= 168) // 4 circle
                i = 4;
            if (actuator >= 169 && actuator <= 216) // 5 circle
                i = 5;
            if (actuator >= 217 && actuator <= 264) // 6 circle
                i = 6;
            if (actuator >= 265 && actuator <= 268) // 7 circle
                i = 7;
            if (actuator == 1 || actuator == 25 || actuator == 73 ||
                actuator == 121 || actuator == 169 || actuator == 217 || actuator == 265)
            {
                l = 1;
                CIRATools::Wait(100000); //100ms
            }


            bool active, statusColor;

            if (i == 1) { // ONLY FOR NOTO
                active = true;
                statusColor = false;
            }
            else {
                CORBA::Long status = asStatus[actuator - 25]; //we need to normalize to asStatus.length() range
                if(status == UNAV)
                    active = false;             // red
                else
                {
                    active = true;

                    if((status & ENBL) == 0 || (status & CAL) == 0)
                    {
                        statusColor = true;     // yellow
                    }
                    else
                        statusColor = false;    // green
                }
            }
            emit setGUIActuatorColor(i, l, active, statusColor, true);
        }
    }
}

void NotoActiveSurfaceCore::setactuator(int circle, int actuator)
{

    CORBA::Long actPos_val; CORBA::Double mm_actPos_val;
    CORBA::Long cmdPos_val; CORBA::Double mm_cmdPos_val;
    CORBA::Long Fmin_val;
    CORBA::Long Fmax_val;
    CORBA::Long acc_val;
    CORBA::Long delay_val;
    CORBA::Long status;
    bool active;
    bool statusColor;

    /*
    ActuatorNumber_str = QString("%1_%2").arg(circle).arg(actuator);
    qApp->lock();
    tGUI->ActuatorNumberlineEdit->clear();
    qApp->unlock();
    qApp->lock();
    tGUI->ActuatorNumberlineEdit->insert(ActuatorNumber_str);
    qApp->unlock();

    if (callfromfunction == false)
    {
        qApp->lock();
        tGUI->ManagerLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
        qApp->unlock();
        qApp->lock();
        tGUI->ManagerLabel->setText(ActuatorNumber_str);
        qApp->unlock();
    }
        
    clearactuatorslineedit();
    */
    //theCircle = circle;
    //theActuator = actuator;

    emit setGUIActuator();

    try {
        tASBoss->usdStatus4GUIClient(circle, actuator, status);
        active = true;
    }
    catch (ComponentErrors::ComponentNotActiveExImpl& ex) {
        active = false;
        ex.log(LM_DEBUG);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        //clearactuatorslineedit();
        active = false;
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        active = false;
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::setactuator()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        active = false;
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::setactuator()");
        impl.log();
    }

    if (active == true)
    {
        //if ((status & ENBL) == 0) {
        /*qApp->lock();
        tGUI->ActuatorStatusEnblLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusEnblLabel->setText("UNABLED");
        qApp->unlock();*/
        //ActuatorStatusEnblLabelCode = -1;
        //emit setGUIActuatorStatusEnblLabel();
        //}
        //else {
        /*qApp->lock();
        tGUI->ActuatorStatusEnblLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusEnblLabel->setText("ENABLED");
        qApp->unlock();*/
        //ActuatorStatusEnblLabelCode = 1;
        //emit setGUIActuatorStatusEnblLabel();
        try {
            tASBoss->setActuator(circle, actuator, actPos_val, cmdPos_val, Fmin_val, Fmax_val, acc_val, delay_val);
            ActuatorActualPosition_str.setNum(actPos_val);
            //tGUI->stepActuatorActualPositionlineEdit->clear();
            //tGUI->stepActuatorActualPositionlineEdit->insert(ActuatorActualPosition_str);
            mm_actPos_val = (CORBA::Double)actPos_val*STEP2MM;
            mm_ActuatorActualPosition_str.setNum(mm_actPos_val);
            //tGUI->mmActuatorActualPositionlineEdit->clear();
            //tGUI->mmActuatorActualPositionlineEdit->insert(ActuatorActualPosition_str);

            ActuatorCommandedPosition_str.setNum(cmdPos_val);
            //tGUI->stepActuatorCommandedPositionlineEdit->clear();
            //tGUI->stepActuatorCommandedPositionlineEdit->insert(ActuatorCommandedPosition_str);
            mm_cmdPos_val = (CORBA::Double)cmdPos_val*STEP2MM;
            mm_ActuatorCommandedPosition_str.setNum(mm_cmdPos_val);
            //tGUI->mmActuatorCommandedPositionlineEdit->clear();
            //tGUI->mmActuatorCommandedPositionlineEdit->insert(ActuatorCommandedPosition_str);

            ActuatorDelay_str.setNum(delay_val);
            //tGUI->ActuatorDelaylineEdit->clear();
            //tGUI->ActuatorDelaylineEdit->insert(ActuatorDelay_str);

            ActuatorFmin_str.setNum(Fmin_val);
            //tGUI->ActuatorFminlineEdit->clear();
            //tGUI->ActuatorFminlineEdit->insert(ActuatorFmin_str);

            ActuatorFmax_str.setNum(Fmax_val);
            //tGUI->ActuatorFmaxlineEdit->clear();
            //tGUI->ActuatorFmaxlineEdit->insert(ActuatorFmax_str);

            ActuatorAcceleration_str.setNum(acc_val);
            //tGUI->ActuatorAccelerationlineEdit->clear();
            //tGUI->ActuatorAccelerationlineEdit->insert(ActuatorAcceleration_str);
    
            emit setGUIActuatorValues();

            //ActuatorStatus_color.setRgb( 0, 170, 0 );
            //setactuatorcolor(circle, actuator, ActuatorStatus_color);
            //theCircle = i; theActuator = l;
            //emit setGUIActuatorColor(circle, actuator, active, statusColor, false);

            //setactuatorstatuslabels(circle,actuator);
            //The status has been updated right before this if-else section, there is no need to ask it again
            //CORBA::Long status;
            //tASBoss->usdStatus4GUIClient(circle, actuator, status);
            if ((status & MRUN) == 0) {
                ActuatorStatusRunLabelCode = -1;
                //tGUI->ActuatorStatusRunLabel->clear();
                //tGUI->ActuatorStatusRunLabel->setText("STOPPED");
            }
            else {
                ActuatorStatusRunLabelCode = 1;
                //tGUI->ActuatorStatusRunLabel->clear();
                //tGUI->ActuatorStatusRunLabel->setText("RUNNING");
            }
            ActuatorStatusEnblLabelCode = 1;
            /*if ((status & ENBL) == 0) {
                ActuatorStatusEnblLabelCode = -1;
                //tGUI->ActuatorStatusEnblLabel->clear();
                //tGUI->ActuatorStatusEnblLabel->setText("UNABLED");
            }
            else {
                ActuatorStatusEnblLabelCode = 1;
                //tGUI->ActuatorStatusEnblLabel->clear();
                //tGUI->ActuatorStatusEnblLabel->setText("ENABLED");
            }*/
            if ((status & CAMM) == 0) {
                ActuatorStatusCammLabelCode = -1;
                //tGUI->ActuatorStatusCammLabel->clear();
                //tGUI->ActuatorStatusCammLabel->setText("NO CAMM");
            }
            else {
                ActuatorStatusCammLabelCode = 1;
                //tGUI->ActuatorStatusCammLabel->clear();
                //tGUI->ActuatorStatusCammLabel->setText("CAMM");
            }
            if ((status & PAUT) == 0) {
                ActuatorStatusLoopLabelCode = -1;
                //tGUI->ActuatorStatusLoopLabel->clear();
                //tGUI->ActuatorStatusLoopLabel->setText("NO LOOP");
            }
            else {
                ActuatorStatusLoopLabelCode = 1;
                //tGUI->ActuatorStatusLoopLabel->clear();
                //tGUI->ActuatorStatusLoopLabel->setText("LOOP");
            }
            if ((status & CAL) == 0)
            {
                ActuatorStatusCalLabelCode = -1;
                statusColor = true;
                //tGUI->ActuatorStatusCalLabel->clear();
                //tGUI->ActuatorStatusCalLabel->setText("UNCALIBRATED");
		        //printf("uncalibrated\n");
		        //emit setGUIActuatorColor(circle, actuator, false, false);
            }
            else {
                ActuatorStatusCalLabelCode = 1;
                statusColor = false;
                //tGUI->ActuatorStatusCalLabel->clear();
                //tGUI->ActuatorStatusCalLabel->setText("CALIBRATED");
            }
            //emit setGUIActuatorStatusLabels();
            //emit setGUIActuatorColor(circle, actuator, active, statusColor, false);
        }
        catch (ComponentErrors::ComponentErrorsExImpl& ex) {
            // clearactuatorslineedit();
            active = false;
            ex.log(LM_DEBUG);
        }
        catch (CORBA::SystemException& sysEx) {
            active = false;
            _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::setActuator()");
            impl.setName(sysEx._name());
            impl.setMinor(sysEx.minor());
            impl.log();
        }
        catch (...) {
            active = false;
            _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::setActuator()");
            impl.log();
        }
    }
    else {
        active = false;
        statusColor = true;
        ActuatorStatusRunLabelCode = 0;
        ActuatorStatusEnblLabelCode = -1;
        ActuatorStatusCammLabelCode = 0;
        ActuatorStatusLoopLabelCode = 0;
        ActuatorStatusCalLabelCode = 0;
    }
    emit setGUIActuatorStatusLabels();
    emit setGUIActuatorColor(circle, actuator, active, statusColor, false);
}


void NotoActiveSurfaceCore::recoverUSD(int circle, int actuator)
{
/*    callfromfunction = true;

    try {
        tASBoss->recoverUSD(circle, actuator);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::recoverUSD()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::recoverUSD()");
        impl.log();
    }

    setactuator(circle,actuator);

    callfromfunction = false; */
}

void NotoActiveSurfaceCore::stopUSD(int circle, int actuator, int radius)
{
    // disable automatic update
    disableAutoUpdate();

    try {
        tASBoss->stop(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::stop()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::stop()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        // setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) {   // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::reset(int circle, int actuator, int radius)
{
    try {
        tASBoss->reset(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::reset()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        // setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::up(int circle, int actuator, int radius)
{
    try {
        tASBoss->up(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::up()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::down(int circle, int actuator, int radius)
{
    try {
        tASBoss->down(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::down()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::top(int circle, int actuator, int radius)
{
    try {
        tASBoss->top(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::top()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::bottom(int circle, int actuator, int radius)
{
    try {
        tASBoss->bottom(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::bottom()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::calibrate(int circle, int actuator, int radius)
{
    char str[28];

    sprintf(str,"NotoAS_Calibration %d %d %d &", circle, actuator, radius);
    system(str); // external NotoAS_Calibration tool 

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::calVer(int circle, int actuator, int radius)
{
/*    callfromfunction = true;

    try {
        tASBoss->calVer(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::calver()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;*/
}

void NotoActiveSurfaceCore::park()
{
    try {
        tASBoss->park();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::park()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::park()");
        impl.log();
    }
}

void NotoActiveSurfaceCore::stow(int circle, int actuator, int radius)
{
    try {
        tASBoss->stow(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::stow()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::move(int circle, int actuator, int radius, long incr)
{
    try {
        tASBoss->move(circle, actuator, radius, incr);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::move()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

/*void NotoActiveSurfaceCore::setProfile(long int profile)
{
    if (profile == 0)
        tASBoss->setProfile(ActiveSurface::AS_SHAPED);
    if (profile == 1)
        tASBoss->setProfile(ActiveSurface::AS_SHAPED_FIXED);
    if (profile == 2)
        tASBoss->setProfile(ActiveSurface::AS_PARABOLIC);
    if (profile == 3)
        tASBoss->setProfile(ActiveSurface::AS_PARABOLIC_FIXED);
}*/

void NotoActiveSurfaceCore::correction(int circle, int actuator, int radius, double correction)
{
    try {
        tASBoss->correction(circle, actuator, radius, correction);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::correction()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::update(double elevation)
{
    try {
        tASBoss->update(elevation);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::elevation()");
        impl.log();
    }

    //setallactuators(); // ALL
    emit setGUIAllActuators(true);
}

void NotoActiveSurfaceCore::setup(int circle, int actuator, int radius)
{
    try {
        //tASBoss->setup(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::setup()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::refPos(int circle, int actuator, int radius)
{
    try {
        tASBoss->refPos(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::refPos()");
        impl.log();
    }

    if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators(true);
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius(true);
    }
    else
        setactuator(circle,actuator);
}

void NotoActiveSurfaceCore::enableAutoUpdate()
{
    tASBoss->asOn();
}

void NotoActiveSurfaceCore::disableAutoUpdate()
{
    tASBoss->asOff();
}

void NotoActiveSurfaceCore::setParabolicProfile()
{
    tASBoss->setProfile(ActiveSurface::AS_PARABOLIC);
}

void NotoActiveSurfaceCore::setParabolicFixedProfile()
{
    tASBoss->setProfile(ActiveSurface::AS_PARABOLIC_FIXED);
}

/*
void NotoActiveSurfaceCore::setactuatorstatuslabels(int circle, int actuator)
{
    CORBA::Long status;

    try {
        tASBoss->usdStatus4GUIClient(circle, actuator, status);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        clearactuatorslineedit();
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::NotoActiveSurfaceCore::run()");
        impl.log();
    }
    if ((status & MRUN) == 0) {
        qApp->lock();
        tGUI->ActuatorStatusRunLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusRunLabel->setText("STOPPED");
        qApp->unlock();
    }
    else {
        qApp->lock();
        tGUI->ActuatorStatusRunLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusRunLabel->setText("RUNNING");
        qApp->unlock();
    }
    if ((status & ENBL) == 0) {
        qApp->lock();
        tGUI->ActuatorStatusEnblLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusEnblLabel->setText("UNABLED");
        qApp->unlock();
    }
    else {
        qApp->lock();
        tGUI->ActuatorStatusEnblLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusEnblLabel->setText("ENABLED");
        qApp->unlock();
    }
    if ((status & CAMM) == 0) {
        qApp->lock();
        tGUI->ActuatorStatusCammLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusCammLabel->setText("NO CAMM");
        qApp->unlock();
    }
    else {
        qApp->lock();
        tGUI->ActuatorStatusCammLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusCammLabel->setText("CAMM");
        qApp->unlock();
    }
    if ((status & PAUT) == 0) {
        qApp->lock();
        tGUI->ActuatorStatusLoopLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusLoopLabel->setText("NO LOOP");
        qApp->unlock();
    }
    else {
        qApp->lock();
        tGUI->ActuatorStatusLoopLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusLoopLabel->setText("LOOP");
        qApp->unlock();
    }
    if ((status & CAL) == 0) {
        qApp->lock();
        tGUI->ActuatorStatusCalLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusCalLabel->setText("UNCALIBRATED");
        qApp->unlock();
    }
    else {
        qApp->lock();
        tGUI->ActuatorStatusCalLabel->clear();
        qApp->unlock();
        qApp->lock();
        tGUI->ActuatorStatusCalLabel->setText("CALIBRATED");
        qApp->unlock();
    }
}
*/
