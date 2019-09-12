// System includes
#include <ctime>
#include <deque>
#include <exception>

// ACS includes
#include <acstime.h>
#include <SRTActiveSurfaceCore.h>

// mask pattern for status 
#define MRUN	0x000080
#define CAMM	0x000100
#define ENBL	0x002000
#define PAUT	0x000800
#define CAL 	0x008000

#define STEP2MM 0.000714286
//1step del driver = 0.000714286mm di corsa lineare Attuatore.

using namespace IRA;

/*
void SRTActiveSurfaceCore::setGUI(SRTActiveSurfaceGUI* theGUI)
{
	tGUI=theGUI;
    tGUI->EnableButton->setPaletteBackgroundColor(QColor(0,170,0));
	actuatorcounter = circlecounter = totacts = 1;
	callfromfunction = false;
}

void SRTActiveSurfaceCore::setSimpleClient (maci::SimpleClient* theClient)
{
	sc = theClient;
}
*/

SRTActiveSurfaceCore::SRTActiveSurfaceCore (QObject *parent) : QThread(parent)
{
}

SRTActiveSurfaceCore::~SRTActiveSurfaceCore()
{
}

void SRTActiveSurfaceCore::setASBoss (ActiveSurface::SRTActiveSurfaceBoss_var ASBoss)
{
    tASBoss = ASBoss;
    actuatorcounter = circlecounter = totacts = 1;
	callfromfunction = false;
    fromRun = false;
}

void SRTActiveSurfaceCore::stop (void)
{
  	monitor = false;
}

void SRTActiveSurfaceCore::run(void)
{
	QColor ActuatorStatus_color;
	int i = circlecounter;
	int l = actuatorcounter;
	int totalactuators = totacts;
	ACSErr::Completion_var completion;
	ACS::ROpattern_var status_var;
    CORBA::Long status;

    Management::ROTSystemStatus_var bossStatus_var;
    ACS::pattern bossStatus_val;

    ActiveSurface::ROTASProfile_var asProfile_var;
    ACS::pattern asProfile_val;


    while (monitor == true) {
        TIMEVALUE clock;
        CIRATools::getTime(clock);
        ACS::Time t0 = clock.value().value;

        bossStatus_var = tASBoss->status();
        bossStatus_val = bossStatus_var->get_sync(completion.out());
        switch (bossStatus_val) {
            case (Management::MNG_OK):
                ASstatusCode = 0;
                break;
            case (Management::MNG_WARNING):
                ASstatusCode = -1;
                break;
            case (Management::MNG_FAILURE):
                ASstatusCode = -2;
                break;
        }
        emit setGUIasStatusCode();

        asProfile_var = tASBoss->pprofile();
        asProfile_val = asProfile_var->get_sync(completion.out());
        switch (asProfile_val) {
            case (ActiveSurface::AS_SHAPED):
                asProfileCode = 0;
                break;
            case (ActiveSurface::AS_SHAPED_FIXED):
                asProfileCode = 1;
                break;
            case (ActiveSurface::AS_PARABOLIC):
                asProfileCode = 2;
                break;
            case (ActiveSurface::AS_PARABOLIC_FIXED):
                asProfileCode = 3;
                break;
            case (ActiveSurface::AS_PARK):
                asProfileCode = 4;
                break;
        }
        emit setGUIasProfileCode();

        if (totalactuators >= 1 && totalactuators <= 24) // 1 circle
            i= 1;
        if (totalactuators >= 25 && totalactuators <= 48)  // 2 circle
            i= 2;
        if (totalactuators >= 49 && totalactuators <= 96)  // 3 circle
            i= 3;
        if (totalactuators >= 97 && totalactuators <= 144) // 4 circle
            i= 4;
        if (totalactuators >= 145 && totalactuators <= 192) // 5 circle
            i= 5;
        if (totalactuators >= 193 && totalactuators <= 240) // 6 circle
            i= 6;
        if (totalactuators >= 241 && totalactuators <= 336) // 7 circle
            i= 7;
        if (totalactuators >= 337 && totalactuators <= 432) // 8 circle
            i= 8;
        if (totalactuators >= 433 && totalactuators <= 528) // 9 circle
            i= 9;
        if (totalactuators >= 529 && totalactuators <= 624) // 10 circle
            i= 10;
        if (totalactuators >= 625 && totalactuators <= 720) // 11 circle
            i= 11;
        if (totalactuators >= 721 && totalactuators <= 816) // 12 circle
				i= 12;
        if (totalactuators >= 817 && totalactuators <= 912) // 13 circle
            i= 13;
        if (totalactuators >= 913 && totalactuators <= 1008) // 14 circle
            i= 14;
        if (totalactuators >= 1009 && totalactuators <= 1104) // 15 circle
            i= 15;
        if (totalactuators >= 1105 && totalactuators <= 1112) // 16 circle
            i= 16;
        if (totalactuators >= 1113 && totalactuators <= 1116) // 17 circle
            i= 17;
        if (totalactuators == 1 || totalactuators == 25 || totalactuators == 49 ||
            totalactuators == 97 || totalactuators == 145 || totalactuators == 193 ||
			totalactuators == 241 || totalactuators == 337 || totalactuators == 433 ||
			totalactuators == 529 || totalactuators == 625 || totalactuators == 721 ||
			totalactuators == 817 || totalactuators == 913 || totalactuators == 1009 ||
            totalactuators == 1105 || totalactuators == 1113 || totalactuators == 1117)      
            l = 1;

        try {
            tASBoss->usdStatus4GUIClient(i,l,status);
            if ((status & ENBL) == 0)  {// usd not enabled
                //ActuatorStatus_color.setRgb( 255, 0, 0 );
		//printf("usd %d_%d not enabled\n", i, l);
                theActuatorStatusColorString = "background-color: rgb(255, 0, 0);";
            }
            else {
		        //ActuatorStatus_color.setRgb( 0, 170, 0 );
                theActuatorStatusColorString = "background-color: rgb(85, 255, 0);";
            }
            //setactuatorcolor(i, l, ActuatorStatus_color);
            fromRun = true;
            //theCircle = i; theActuator = l;
            emit setGUIActuatorColor(i, l, theActuatorStatusColorString);
            //fromRun = false;
        }
        catch (ComponentErrors::ComponentNotActiveExImpl& ex) {
        //catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			//ActuatorStatus_color.setRgb( 255, 255, 0 );
            theActuatorStatusColorString = "background-color: rgb(255, 255, 0);";
            // setactuatorcolor(i, l, ActuatorStatus_color);
            fromRun = true;
            //theCircle = i; theActuator = l;
            emit setGUIActuatorColor(i, l, theActuatorStatusColorString);
            //fromRun = false;
            ex.log(LM_DEBUG);
        }
        catch (CORBA::SystemException& sysEx) {
			//ActuatorStatus_color.setRgb( 255, 0, 0 );
            theActuatorStatusColorString = "background-color: rgb(255, 0, 0);";
            // setactuatorcolor(i, l, ActuatorStatus_color);
            fromRun = true;
            //theCircle = i; theActuator = l;
            emit setGUIActuatorColor(i, l, theActuatorStatusColorString);
            //fromRun = false;
            _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
            impl.setName(sysEx._name());
		impl.setMinor(sysEx.minor());
		impl.log();
		//printf("usd %d_%d corba ex\n", i, l);
        }
        catch (...) {
			//ActuatorStatus_color.setRgb( 255, 0, 0 );
            theActuatorStatusColorString = "background-color: rgb(255, 0, 0);";
            // setactuatorcolor(i, l, ActuatorStatus_color);
            fromRun = true;
            //theCircle = i; theActuator = l;
            emit setGUIActuatorColor(i, l, theActuatorStatusColorString);
            //fromRun = false;
            _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
            impl.log();
		//printf("usd %d_%d unknown ex\n", i, l);
        }
		
		totalactuators++;
		l++;
		if (totalactuators == 1117) {
		    i = l = totalactuators = 1;
		}
		circlecounter = i;
		actuatorcounter = l;
		totacts = totalactuators;

        CIRATools::getTime(clock);
        ACS::Time t1 = clock.value().value;
        int elapsed = (t1 - t0) / 10; //Time is expressed in hundreds of nanoseconds, we convert it to microseconds in order to use it with the Wait function

        if(elapsed < 100000)
            CIRATools::Wait(100000 - elapsed);
    } // end of while
}

void SRTActiveSurfaceCore::setactuator(int circle, int actuator)
{

    CORBA::Long actPos_val; CORBA::Double mm_actPos_val;
    CORBA::Long cmdPos_val; CORBA::Double mm_cmdPos_val;
	CORBA::Long Fmin_val;
	CORBA::Long Fmax_val;
	CORBA::Long acc_val;
	CORBA::Long delay_val;
    CORBA::Long status;


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

    fromRun = false;

    emit setGUIActuator();

    try {
        tASBoss->usdStatus4GUIClient(circle, actuator, status);
	}
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        //clearactuatorslineedit();
        ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::setactuator()");
        impl.setName(sysEx._name());
		impl.setMinor(sysEx.minor());
		impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::setactuator()");
        impl.log();
    }

    if ((status & ENBL) == 0) {
		/*qApp->lock();
		tGUI->ActuatorStatusEnblLabel->clear();
		qApp->unlock();
		qApp->lock();
		tGUI->ActuatorStatusEnblLabel->setText("UNABLED");
		qApp->unlock();*/
        ActuatorStatusEnblLabelCode = -1;
        emit setGUIActuatorStatusEnblLabel();
    }
    else {
        /*qApp->lock();
		tGUI->ActuatorStatusEnblLabel->clear();
		qApp->unlock();
		qApp->lock();
		tGUI->ActuatorStatusEnblLabel->setText("ENABLED");
		qApp->unlock();*/
        ActuatorStatusEnblLabelCode = 1;
        emit setGUIActuatorStatusEnblLabel();
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

            theActuatorStatusColorString = "background-color: rgb(85, 255, 0);";
            //ActuatorStatus_color.setRgb( 0, 170, 0 );
            //setactuatorcolor(circle, actuator, ActuatorStatus_color);
            //theCircle = i; theActuator = l;
            emit setGUIActuatorColor(circle, actuator, theActuatorStatusColorString);

		    //setactuatorstatuslabels(circle,actuator);
            CORBA::Long status;
            tASBoss->usdStatus4GUIClient(circle, actuator, status);
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
            if ((status & ENBL) == 0) {
                ActuatorStatusEnblLabelCode = -1;
		        //tGUI->ActuatorStatusEnblLabel->clear();
	            //tGUI->ActuatorStatusEnblLabel->setText("UNABLED");
            }
            else {
                ActuatorStatusEnblLabelCode = 1;
		        //tGUI->ActuatorStatusEnblLabel->clear();
		        //tGUI->ActuatorStatusEnblLabel->setText("ENABLED");
	        }
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
	        if ((status & CAL) == 0) {
                ActuatorStatusCalLabelCode = -1;
		        //tGUI->ActuatorStatusCalLabel->clear();
		        //tGUI->ActuatorStatusCalLabel->setText("UNCALIBRATED");
	        }
            else {
                ActuatorStatusCalLabelCode = 1;
		        //tGUI->ActuatorStatusCalLabel->clear();
		        //tGUI->ActuatorStatusCalLabel->setText("CALIBRATED");
            }
            emit setGUIActuatorStatusLabels();
	    }
        catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		    // clearactuatorslineedit();
	        ex.log(LM_DEBUG);
        }
        catch (CORBA::SystemException& sysEx) {
            _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::setActuator()");
            impl.setName(sysEx._name());
            impl.setMinor(sysEx.minor());
            impl.log();
        }
        catch (...) {
            _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::setActuator()");
            impl.log();
        }
    }
}


void SRTActiveSurfaceCore::recoverUSD(int circle, int actuator)
{
/*	callfromfunction = true;

    try {
        tASBoss->recoverUSD(circle, actuator);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::recoverUSD()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::recoverUSD()");
        impl.log();
    }

    setactuator(circle,actuator);

    callfromfunction = false; */
}

void SRTActiveSurfaceCore::stopUSD(int circle, int actuator, int radius)
{
	callfromfunction = true;

    // disable automatic update
    disableAutoUpdate();

    try {
        tASBoss->stop(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::stop()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::stop()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        // setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) {   // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::reset(int circle, int actuator, int radius)
{
	callfromfunction = true;

    try {
        tASBoss->reset(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::reset()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        // setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::up(int circle, int actuator, int radius)
{
	callfromfunction = true;

    try {
        tASBoss->up(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::up()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::down(int circle, int actuator, int radius)
{
	callfromfunction = true;

    try {
        tASBoss->down(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::down()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::top(int circle, int actuator, int radius)
{
	callfromfunction = true;

    try {
        tASBoss->top(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::top()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::bottom(int circle, int actuator, int radius)
{
	callfromfunction = true;

    try {
        tASBoss->bottom(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::bottom()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::calibrate(int circle, int actuator, int radius)
{
	callfromfunction = true;
	char str[28];

	sprintf(str,"SRTAS_Calibration %d %d %d &", circle, actuator, radius);
	system(str); // external SRTAS_Calibration tool 

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::calVer(int circle, int actuator, int radius)
{
/*	callfromfunction = true;

    try {
        tASBoss->calVer(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::calver()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;*/
}

void SRTActiveSurfaceCore::park()
{
    try {
        tASBoss->park();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::park()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::park()");
        impl.log();
    }
}

void SRTActiveSurfaceCore::stow(int circle, int actuator, int radius)
{
	callfromfunction = true;

    try {
        tASBoss->stow(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::stow()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::move(int circle, int actuator, int radius, long incr)
{
	callfromfunction = true;

    try {
        tASBoss->move(circle, actuator, radius, incr);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::move()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::setProfile(long int profile)
{
    if (profile == 0)
        tASBoss->setProfile(ActiveSurface::AS_SHAPED);
    if (profile == 1)
        tASBoss->setProfile(ActiveSurface::AS_SHAPED_FIXED);
    if (profile == 2)
        tASBoss->setProfile(ActiveSurface::AS_PARABOLIC);
    if (profile == 3)
        tASBoss->setProfile(ActiveSurface::AS_PARABOLIC_FIXED);
}

void SRTActiveSurfaceCore::correction(int circle, int actuator, int radius, double correction)
{
	callfromfunction = true;

    try {
        tASBoss->correction(circle, actuator, radius, correction);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::correction()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::update(double elevation)
{
	callfromfunction = true;

    try {
        tASBoss->update(elevation);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::elevation()");
        impl.log();
    }

    //setallactuators(); // ALL
    emit setGUIAllActuators();

    callfromfunction = false;
}

void SRTActiveSurfaceCore::setup(int circle, int actuator, int radius)
{
	callfromfunction = true;

    try {
        //tASBoss->setup(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::setup()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::refPos(int circle, int actuator, int radius)
{
	callfromfunction = true;

    try {
        tASBoss->refPos(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	    ex.log(LM_DEBUG);
    }
    catch (CORBA::SystemException& sysEx) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
        impl.setMinor(sysEx.minor());
        impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::refPos()");
        impl.log();
    }

	if (circle == 0 && actuator == 0 && radius == 0)
        //setallactuators(); // ALL
        emit setGUIAllActuators();
    else if ((circle != 0 && actuator == 0 && radius == 0) || // CIRCLE
             (circle == 0 && actuator == 0 && radius != 0)) { // RADIUS
        //setcircleORradius(circle, radius);
        theCircle = circle; theRadius = radius;
        emit setGUIcircleORradius();
    }
    else
        setactuator(circle,actuator);

    callfromfunction = false;
}

void SRTActiveSurfaceCore::enableAutoUpdate()
{
    tASBoss->asOn();
}

void SRTActiveSurfaceCore::disableAutoUpdate()
{
    tASBoss->asOff();
}

/*
void SRTActiveSurfaceCore::setactuatorstatuslabels(int circle, int actuator)
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
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
        impl.setName(sysEx._name());
		impl.setMinor(sysEx.minor());
		impl.log();
    }
    catch (...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::SRTActiveSurfaceCore::run()");
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
