#include "NotoMinorServoBossCore.h"
#include <Definitions.h>
#include <cstdio>
#include <CustomLoggerUtils.h>

int actuatorsInCircle[] = {0,24,24,48,48,48,48,96,96,96,96,96,96,96,96,96,8,4};

CNotoMinorServoBossCore::CNotoMinorServoBossCore(ContainerServices *service) : CSocket(),
m_services(service)
{
}

CNotoMinorServoBossCore::~CNotoMinorServoBossCore()
{
}

void CNotoMinorServoBossCore::initialize()
{
	ACS_LOG(LM_FULL_INFO,"CNotoMinorServoBossCore::initialize()",(LM_INFO,"CNotoMinorServoBossCore::initialize"));
    
    	m_enable = false;
	m_tracking = false;
    	m_status = Management::MNG_WARNING;
//	m_profile = MinorServo::AS_SHAPED_FIXED;
    	AutoUpdate = false;
    	actuatorcounter = circlecounter = totacts = 1;
	m_sector1 = false;
	m_sector2 = false;
	m_sector3 = false;
	m_sector4 = false;
	m_sector5 = false;
	m_sector6 = false;
	m_sector7 = false;
	m_sector8 = false;
	m_profileSetted = false;
	m_ASup = false;
	m_elevation = 0.0;
}

void CNotoMinorServoBossCore::execute(CConfiguration *config) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::SocketErrorExImpl,
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::CDBAccessExImpl)
{
	AUTO_TRACE("CNotoMinorServoBossCore::execute()");
	m_configuration=config;
	// this will create the socket in blocking mode.....
	if (Create(m_Error,STREAM)==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CNotoMinorServoBossCore::execute()");
	}
	// the first time, perform a blocking connection....
	if (Connect(m_Error,m_configuration->getAddress(),m_configuration->getPort())==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CNotoMinorServoBossCore::execute()");
	}	
	else {
		setStatus(CNTD);
	}
	// set socket send buffer!!!!
	int Val=SENDBUFFERSIZE;
	if (setSockOption(m_Error,SO_SNDBUF,&Val,sizeof(int))==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CNotoMinorServoBossCore::execute()");
	}

    	m_antennaBoss = Antenna::AntennaBoss::_nil();
    	try {
        	m_antennaBoss = m_services->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
    	}
    	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::execute()");
		Impl.setComponentName("ANTENNA/Boss");
        	m_status=Management::MNG_WARNING;
		throw Impl;
	}
    	m_enable = true;
    	ACS_LOG(LM_FULL_INFO, "CNotoMinorServoBossCore::execute()", (LM_INFO,"CNotoMinorServoBossCore::NotoMinorServoBoss_LOCATED"));
}
	
void CNotoMinorServoBossCore::cleanUp()
{
	ACS_LOG(LM_FULL_INFO, "CNotoMinorServoBossCore::cleanUp()", (LM_INFO,"CNotoMinorServoBossCore::cleanUp"));

    	try {
			m_services->releaseComponent((const char*)m_antennaBoss->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::cleanUp()");
			Impl.setComponentName((const char *)m_antennaBoss->name());
			Impl.log(LM_DEBUG);
		}
		Close (m_Error);
}
/*
void CNotoMinorServoBossCore::reset (int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
    if (circle == 0 && actuator == 0 &&radius == 0) // ALL 
	{
        	int i, l;
		for (i = 1; i <= CIRCLES; i++)
    		{
		    for (l = 1; l <= actuatorsInCircle[i]; l++) 
			{
                if (!CORBA::is_nil(usd[i][l]))
				{
			    	try {
				    	usd[i][l]->reset();
						CIRATools::Wait(LOOPTIME);
					}
					catch (ASErrors::ASErrorsEx& E) {
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CNotoMinorServoBossCore::reset()");
                        impl.setComponentName((const char*)usd[i][l]->name());
			            impl.setOperationName("reset()");
			            impl.log();
					}
					catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::reset()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                    }
		            catch(...) {
                        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CNotoMinorServoBossCore::reset()");
                        impl.log();
                    }
				}
				else { 
                    _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CNotoMinorServoBossCore::reset()");
                    impl.log();
                }
			}
        }
	}
	else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
        int l;
        for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            if (!CORBA::is_nil(usd[circle][l])) {
                try {
                    usd[circle][l]->reset();
					CIRATools::Wait(LOOPTIME);
                }
				catch (ComponentErrors::ComponentErrorsEx& E) {
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CNotoMinorServoBossCore::reset()");
                        impl.setComponentName((const char*)usd[circle][l]->name());
			            impl.setOperationName("reset()");
			            impl.log();
				}
                catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::reset()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                }
		        catch(...) {
                    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CNotoMinorServoBossCore::reset()");
                    impl.log();
                }
            }
		    else {
                _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CNotoMinorServoBossCore::reset()");
                impl.log();
            }
        }
    }
    else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
        int actuatorsradius;
        int jumpradius;
        setradius(radius, actuatorsradius, jumpradius);
        int l;
        for (l = 1; l <= actuatorsradius; l++) {
            if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                jumpradius++;  // 17 circle
            if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                try {
                    lanradius[l+jumpradius][radius]->reset();
					CIRATools::Wait(LOOPTIME);
                }
                catch (ComponentErrors::ComponentErrorsEx& E) {
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CNotoMinorServoBossCore::reset()");
                        impl.setComponentName((const char*)usd[l+jumpradius][radius]->name());
			            impl.setOperationName("reset()");
			            impl.log();
				}
                catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::reset()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                }
		        catch(...) {
                    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CNotoMinorServoBossCore::reset()");
                    impl.log();
                }
            }
            else {
                _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CNotoMinorServoBossCore::reset()");
                impl.log();
            }
        }
    }
    else {
        if (!CORBA::is_nil(usd[circle][actuator])) { // SINGLE ACTUATOR
            try {
                usd[circle][actuator]->reset();
            }
            catch (ComponentErrors::ComponentErrorsEx& E) {
		        _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CNotoMinorServoBossCore::reset()");
                impl.setComponentName((const char*)usd[circle][actuator]->name());
			    impl.setOperationName("reset()");
			    throw impl;
			}
            catch(CORBA::SystemException &E) {
                _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::reset()");
                impl.setName(E._name());
                impl.setMinor(E.minor());
                throw impl;
            }
            catch(...) {
                _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CNotoMinorServoBossCore::reset()");
            }
        }
        else {
            _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CNotoMinorServoBossCore::reset()");
        }
    }
}


void CNotoMinorServoBossCore::calibrate (int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl)
{
	double cammaPos, cammaLen;
	bool calibrated;
	int i, l;

	if (circle == 0 && actuator == 0 && radius == 0) { // ALL
		printf("top.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (!CORBA::is_nil(usd[i][l])) {
					try {
						usd[i][l]->top();
						printf ("actuator n.%d_%d top\n", i, l);
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (90);

		printf("move to upper mechanical position.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (!CORBA::is_nil(usd[i][l])) {
					try {
						usd[i][l]->move(1400);
						printf ("actuator n.%d_%d move\n", i, l);
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (5);

		printf("calibration.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (!CORBA::is_nil(usd[i][l])) {
					try {
						usd[i][l]->calibrate ();
						printf ("actuator n.%d_%d calibrate\n", i, l);
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (15);

		printf ("calibration verification.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (!CORBA::is_nil(usd[i][l])) {
					try {
						usd[i][l]->calVer ();
						printf ("actuator n.%d_%d calVer\n", i, l);
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
        	}
		ACE_OS::sleep (150);

		printf ("write calibration results.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (!CORBA::is_nil(usd[i][l])) {
					try {
						CIRATools::Wait(LOOPTIME);
						usd[i][l]->writeCalibration(cammaLen,cammaPos,calibrated);
						printf ("actuator %02d_%02d %04.1f %05.1f %d\n", i, l, cammaLen, cammaPos, calibrated);
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (1);
	}
	else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
		printf("top.....\n");
		for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            		if (!CORBA::is_nil(usd[circle][l])) {
                		try {
					usd[circle][l]->top();
					printf ("actuator n.%d_%d top\n", circle, l);
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", circle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (90);

		printf("move to upper mechanical position.....\n");
		for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            		if (!CORBA::is_nil(usd[circle][l])) {
                		try {
					usd[circle][l]->move(1400);
					printf ("actuator n.%d_%d move\n", circle, l);
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", circle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (5);

		printf("calibration.....\n");
		for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            		if (!CORBA::is_nil(usd[circle][l])) {
                		try {
					usd[circle][l]->calibrate ();
					printf ("actuator n.%d_%d calibrate\n", circle, l);
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", circle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (15);

		printf ("calibration verification.....\n");
		for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            		if (!CORBA::is_nil(usd[circle][l])) {
                		try {
					usd[circle][l]->calVer ();
					printf ("actuator n.%d_%d calVer\n", circle, l);
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", circle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (150);

		printf ("write calibration results.....\n");
		for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            		if (!CORBA::is_nil(usd[circle][l])) {
                		try {
					CIRATools::Wait(LOOPTIME);
					usd[circle][l]->writeCalibration(cammaLen,cammaPos,calibrated);
					printf ("actuator %02d_%02d %04.1f %05.1f %d\n", circle, l, cammaLen, cammaPos, calibrated);
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", circle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (1);
	}
	else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
		int actuatorsradius;
        	int jumpradius;
        	setradius(radius, actuatorsradius, jumpradius);
        	int l;

		printf("top.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                		try {
					lanradius[l+jumpradius][radius]->top();
					printf ("actuator n.%d_%d top\n", l+jumpradius, radius);
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, radius, Ex);
				}
			}
		}
		ACE_OS::sleep (90);

		printf("move to upper mechanical position.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                		try {
					lanradius[l+jumpradius][radius]->move(1400);
					printf ("actuator n.%d_%d move\n", l+jumpradius, radius);
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, radius, Ex);
				}
			}
		}
		ACE_OS::sleep (5);

		printf("calibration.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                		try {
					lanradius[l+jumpradius][radius]->calibrate();
					printf ("actuator n.%d_%d calibrate\n", l+jumpradius, radius);
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, radius, Ex);
				}
			}
		}
		ACE_OS::sleep (15);

		printf ("calibration verification.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                		try {
					lanradius[l+jumpradius][radius]->calVer();
					printf ("actuator n.%d_%d calVer\n", l+jumpradius, radius);
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, radius, Ex);
				}
			}
		}
		ACE_OS::sleep (150);

		printf ("write calibration results.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                		try {
					CIRATools::Wait(LOOPTIME);
					lanradius[l+jumpradius][radius]->writeCalibration(cammaLen,cammaPos,calibrated);
					printf ("actuator %02d_%02d %04.1f %05.1f %d\n", l+jumpradius, radius, cammaLen, cammaPos, calibrated);
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, radius, Ex);
				}
			}
		}
		ACE_OS::sleep (1);
	}
	else {
		if (!CORBA::is_nil(usd[circle][actuator])) { // SINGLE ACTUATOR
			printf("top.....\n");
			try {
				usd[circle][actuator]->top();
				printf ("actuator n.%d_%d top\n", circle, actuator);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
			ACE_OS::sleep (90);
			printf("move to upper mechanical position.....\n");
			try {
				usd[circle][actuator]->move(1400);
				printf ("actuator n.%d_%d move\n", circle, actuator);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
			ACE_OS::sleep (5);
			printf("calibration.....\n");
			try {
				usd[circle][actuator]->calibrate();
				printf ("actuator n.%d_%d calibrate\n", circle, actuator);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
			ACE_OS::sleep (15);
			printf ("calibration verification.....\n");
			try {
				usd[circle][actuator]->calVer();
				printf ("actuator n.%d_%d calVer\n", circle, actuator);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
			ACE_OS::sleep (150);
			printf ("write calibration results.....\n");
			try {
				usd[circle][actuator]->writeCalibration(cammaLen,cammaPos,calibrated);
				printf ("actuator %02d_%02d %04.1f %05.1f %d\n", circle, actuator, cammaLen, cammaPos, calibrated);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
			ACE_OS::sleep (1);
		}
	}
}

void CNotoMinorServoBossCore::calVer (int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl)
{

    if (circle == 0 && actuator == 0 && radius == 0) { // ALL
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++)
								{
									if (usdinitialized[i][l] == 0) {
										try {
											printf("actuator n.%d_%d verification of calibration\n", i, l);
											CompletionImpl comp = usd[i][l]->calVer();
											if (comp.isErrorFree() == false)
												checkAScompletionerrors("usd", i, l, comp);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
	}
    else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0) {
									try {
										printf("actuator n.%d_%d verification of calibration\n", circle, l);
										CompletionImpl comp = usd[circle][l]->calVer();
										if (comp.isErrorFree() == false)
											checkAScompletionerrors("usd", circle, l, comp);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
	}
    else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++)  {
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0) {
									try {
										printf("actuator n.%d_%d verification of calibration\n", l+jumpradius, radius);
										CompletionImpl comp = lanradius[l+jumpradius][radius]->calVer();
										if (comp.isErrorFree() == false)
											checkAScompletionerrors("lanradius", l+jumpradius, radius, comp);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
	}
	else {
        if (!CORBA::is_nil(usd[circle][actuator])) { // SINGLE ACTUATOR
			try {
				printf("actuator n.%d_%d verification of calibration\n", circle, actuator);
				CompletionImpl comp = usd[circle][actuator]->calVer();
				if (comp.isErrorFree() == false)
					checkAScompletionerrors("usd", circle, actuator, comp);
                else
                    printf("usd %d %d calibration verification OK\n", circle, actuator);
			}
		    catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
		}
		else
			printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
	}
}

void CNotoMinorServoBossCore::onewayAction(MinorServo::TASOneWayAction onewayAction, int circle, int actuator, int radius, double elevation, double correction, long incr, MinorServo::TASProfile profile) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
	if (circle == 0 && actuator == 0 &&radius == 0) // ALL
	{
        	int i, l;
        	int counter = 0;
        	ACSErr::Completion_var completion;
	    	ACS::ROlong_var actPos_var; 
	    	ACS::RWlong_var cmdPos_var;
		time_t start;
		time_t stop;

		time(&start);
		for (i = 1; i <= CIRCLES; i++)
    		{
			for (l = 1; l <= actuatorsInCircle[i]; l++) 
			{
				if (!CORBA::is_nil(usd[i][l]))
				{
			    		try {
                        			switch (onewayAction) {
                            				case MinorServo::AS_STOP:
                                				usd[i][l]->stop();
                                			break;
                            				case MinorServo::AS_SETUP:
                                				usd[i][l]->setup();
                                			break;
                            				case MinorServo::AS_STOW:
                                				usd[i][l]->stow();
                                			break;
                            				case MinorServo::AS_REFPOS:
                                				usd[i][l]->refPos();
                                			break;
                            				case MinorServo::AS_UP:
                                				usd[i][l]->up();
                                			break;
                            				case MinorServo::AS_DOWN:
                                				usd[i][l]->down();
                                			break;
                            				case MinorServo::AS_BOTTOM:
                                				usd[i][l]->bottom();
                                			break;
                            				case MinorServo::AS_TOP:
                                				usd[i][l]->top();
                                			break;
                            				case MinorServo::AS_UPDATE:
								usd[i][l]->update(elevation);
                                			break;
                            				case MinorServo::AS_CORRECTION:
                                				usd[i][l]->correction(correction*MM2STEP);
                                			break;
                            				case MinorServo::AS_MOVE:
                                				usd[i][l]->move(incr*MM2STEP);
                                			break;
                            				case MinorServo::AS_PROFILE:
                                				usd[i][l]->setProfile(profile);
                                			break;
                        			}
						//CIRATools::Wait(500);
					}
					catch (ASErrors::ASErrorsEx& E) {
						_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CNotoMinorServoBossCore::onewayAction()");
                       				impl.setComponentName((const char*)usd[i][l]->name());
			            		impl.setOperationName("onewayAction()");
			            		impl.log();
					}
					catch(CORBA::SystemException &E) {
						_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::onewayAction()");
						impl.setName(E._name());
						impl.setMinor(E.minor());
						impl.log();
					}
					catch(...) {
						_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CNotoMinorServoBossCore::onewayAction()");
						impl.log();
					}
				}
				else {
                    			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CNotoMinorServoBossCore::onewayAction()");
                    			impl.log();
                		}
				counter++;
				if (counter == 1116)
					counter = 0;
			}
        	}
		time(&stop);
		printf("execution time = %f\n", difftime(stop,start));
	}
	else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
		printf ("onewayaction CIRCLE\n");
        int l;
        for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            if (!CORBA::is_nil(usd[circle][l])) {
                try {
                    switch (onewayAction) {
                        case MinorServo::AS_STOP:
                            usd[circle][l]->stop();
                            break;
                        case MinorServo::AS_SETUP:
                            usd[circle][l]->setup();
                            break;
                        case MinorServo::AS_STOW:
                            usd[circle][l]->stow();
                            break;
                        case MinorServo::AS_REFPOS:
                            usd[circle][l]->refPos();
                            break;
                        case MinorServo::AS_UP:
                            usd[circle][l]->up();
                            break;
                        case MinorServo::AS_DOWN:
                            usd[circle][l]->down();
                            break;
                        case MinorServo::AS_BOTTOM:
                            usd[circle][l]->bottom();
                            break;
                        case MinorServo::AS_TOP:
                            usd[circle][l]->top();
                            break;
                        case MinorServo::AS_UPDATE:
                            break;
                        case MinorServo::AS_CORRECTION:
                            usd[circle][l]->correction(correction*MM2STEP);
                            break;
                        case MinorServo::AS_MOVE:
                            usd[circle][l]->move(incr*MM2STEP);
                            break;
                        case MinorServo::AS_PROFILE:
                            break;
                    }
					//CIRATools::Wait(LOOPTIME);
                }
				catch (ComponentErrors::ComponentErrorsEx& E) {
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CNotoMinorServoBossCore::oneWayAction()");
                        impl.setComponentName((const char*)usd[circle][l]->name());
			            impl.setOperationName("oneWayAction()");
			            impl.log();
				}
                catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::oneWayAction()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                }
		        catch(...) {
                    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CNotoMinorServoBossCore::oneWayAction()");
                    impl.log();
                }
            }
		    else {
                _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CNotoMinorServoBossCore::oneWayAction()");
                impl.log();
            }
        }
    }
    else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
		printf ("onewayaction RADIUS\n");
        int actuatorsradius;
        int jumpradius;
        setradius(radius, actuatorsradius, jumpradius);
        int l;
        for (l = 1; l <= actuatorsradius; l++) {
            if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                jumpradius++;  // 17 circle
            if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                try {
                    switch (onewayAction) {
                        case MinorServo::AS_STOP:
                            lanradius[l+jumpradius][radius]->stop();
                            break;
                        case MinorServo::AS_SETUP:
                            lanradius[l+jumpradius][radius]->setup();
                            break;
                        case MinorServo::AS_STOW:
                            lanradius[l+jumpradius][radius]->stow();
                            break;
                        case MinorServo::AS_REFPOS:
                            lanradius[l+jumpradius][radius]->refPos();
                            break;
                        case MinorServo::AS_UP:
                            lanradius[l+jumpradius][radius]->up();
                            break;
                        case MinorServo::AS_DOWN:
                            lanradius[l+jumpradius][radius]->down();
                            break;
                        case MinorServo::AS_BOTTOM:
                            lanradius[l+jumpradius][radius]->bottom();
                            break;
                        case MinorServo::AS_TOP:
                            lanradius[l+jumpradius][radius]->top();
                            break;
                        case MinorServo::AS_UPDATE:
                            break;
                        case MinorServo::AS_CORRECTION:
                            lanradius[l+jumpradius][radius]->correction(correction*MM2STEP);
                            break;
                        case MinorServo::AS_MOVE:
                            lanradius[l+jumpradius][radius]->move(incr*MM2STEP);
                            break;
                        case MinorServo::AS_PROFILE:
                            break;
                    }
					//CIRATools::Wait(LOOPTIME);
                }
                catch (ComponentErrors::ComponentErrorsEx& E) {
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CNotoMinorServoBossCore::oneWayAction()");
                        impl.setComponentName((const char*)lanradius[l+jumpradius][radius]->name());
			            impl.setOperationName("oneWayAction()");
			            impl.log();
				}
                catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::oneWayAction()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                }
		        catch(...) {
                    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CNotoMinorServoBossCore::oneWayAction()");
                    impl.log();
                }
            }
            else {
                _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CNotoMinorServoBossCore::oneWayAction()");
                impl.log();
            }
        }
    }
    else {
	printf ("onewayaction SINGLE\n");
        if (!CORBA::is_nil(usd[circle][actuator])) { // SINGLE ACTUATOR
            try {
                switch (onewayAction) {
                    case MinorServo::AS_STOP:
                        usd[circle][actuator]->stop();
                        break;
                    case MinorServo::AS_SETUP:
                        usd[circle][actuator]->setup();
                        break;
                     case MinorServo::AS_STOW:
                        usd[circle][actuator]->stow();
                        break;
                     case MinorServo::AS_REFPOS:
                        usd[circle][actuator]->refPos();
                        break;
                     case MinorServo::AS_UP:
                        usd[circle][actuator]->up();
                        break;
                     case MinorServo::AS_DOWN:
                        usd[circle][actuator]->down();
                        break;
                     case MinorServo::AS_BOTTOM:
                        usd[circle][actuator]->bottom();
                        break;
                     case MinorServo::AS_TOP:
                        usd[circle][actuator]->top();
                        break;
                     case MinorServo::AS_UPDATE:
                        break;
                     case MinorServo::AS_CORRECTION:
                        usd[circle][actuator]->correction(correction*MM2STEP);
                        break;
                     case MinorServo::AS_MOVE:
                        usd[circle][actuator]->move(incr*MM2STEP);
                        break;
                     case MinorServo::AS_PROFILE:
                        break;
                }
            }
            catch (ComponentErrors::ComponentErrorsEx& E) {
		        _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CNotoMinorServoBossCore::oneWayAction()");
                impl.setComponentName((const char*)usd[circle][actuator]->name());
			    impl.setOperationName("oneWayAction()");
			    throw impl;
			}
            catch(CORBA::SystemException &E) {
                _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::oneWayAction()");
                impl.setName(E._name());
                impl.setMinor(E.minor());
                throw impl;
            }
            catch(...) {
                _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CNotoMinorServoBossCore::oneWayAction()");
            }
        }
        else {
            _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CNotoMinorServoBossCore::oneWayAction()");
        }
    }
}
*/

void CNotoMinorServoBossCore::watchingMinorServoStatus() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
/*    ACS::ROpattern_var status_var;
    ACSErr::Completion_var completion;

    int i = circlecounter;
    int l = actuatorcounter;
    int totalactuators = totacts;

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

	bool propertyEx = false;
	bool notActivEx = false;
	bool corbaEx = false;

    if (!CORBA::is_nil(usd[i][l])) {
        try {
            status_var = usd[i][l]->status ();
        }
        catch (CORBA::SystemException& ex) {
            corbaEx = true;
            usdCounter--;
            if (usdCounter < (int)lastUSD*WARNINGUSDPERCENT)
                m_status=Management::MNG_WARNING;
            if (usdCounter < (int)lastUSD*ERRORUSDPERCENT)
                m_status=Management::MNG_FAILURE;
		}
    }
    else 
	    notActivEx = true;
        
    //printf("component %d_%d: ",i,l);

    totalactuators++;
    l++;
    if (totalactuators == 1117) {
        i = l = totalactuators = 1;
    }
    circlecounter = i;
    actuatorcounter = l;
    totacts = totalactuators;

    if (corbaEx == true) {
        //printf("corba error\n");
        _THROW_EXCPT(ComponentErrors::CORBAProblemExImpl,"CNotoMinorServoBossCore::watchingMinorServoStatus()");
    }
	if (propertyEx == true) {
        //printf("property error\n");
        _THROW_EXCPT(ComponentErrors::CouldntGetAttributeExImpl,"CNotoMinorServoBossCore::watchingMinorServoStatus()");
    }
	if (notActivEx == true ) {
        //printf("not active\n");
	    _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CNotoMinorServoBossCore::watchingMinorServoStatus()");
    }
    //printf("NO error\n");*/
}

/*
void CNotoMinorServoBossCore::sector1MinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	printf("sector1 start\n");
	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	int i;

	value = "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD_S1.txt\0";
	ifstream usdTableS1(value);
	if (!usdTableS1) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}
	// Get reference to usd components
	for (i = 1; i <= 139; i++) {
		usdTableS1 >> lanIndexS1 >> circleIndexS1 >> usdCircleIndexS1 >> serial_usd >> graf >> mecc;
        	usd[circleIndexS1][usdCircleIndexS1] = MinorServo::USD::_nil();
        	try {
			printf("S1: circleIndexS1 = %d, usdCircleIndexS1 = %d\n", circleIndexS1, usdCircleIndexS1);
            		usd[circleIndexS1][usdCircleIndexS1] = m_services->getComponent<MinorServo::USD>(serial_usd);
			lanradius[circleIndexS1][lanIndexS1] = usd[circleIndexS1][usdCircleIndexS1];
			usdCounterS1++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::sector1MinorServo()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
    	}
	printf("sector1 done\n");
	m_sector1 = true;
}

void CNotoMinorServoBossCore::sector2MinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	printf("sector2 start\n");
	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	int i;

	value = "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD_S2.txt\0";
	ifstream usdTableS2(value);
	if (!usdTableS2) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}
	// Get reference to usd components
	for (i = 1; i <= 140; i++) {
		usdTableS2 >> lanIndexS2 >> circleIndexS2 >> usdCircleIndexS2 >> serial_usd >> graf >> mecc;
        	usd[circleIndexS2][usdCircleIndexS2] = MinorServo::USD::_nil();
        	try {
			printf("S2: circleIndexS2 = %d, usdCircleIndexS2 = %d\n", circleIndexS2, usdCircleIndexS2);
            		usd[circleIndexS2][usdCircleIndexS2] = m_services->getComponent<MinorServo::USD>(serial_usd);
			lanradius[circleIndexS2][lanIndexS2] = usd[circleIndexS2][usdCircleIndexS2];
			usdCounterS2++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::sector2MinorServo()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
    	}

	printf("sector2 done\n");
	m_sector2 = true;
}

void CNotoMinorServoBossCore::sector3MinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	printf("sector3 start\n");
	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	int i;

	value = "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD_S3.txt\0";
	ifstream usdTableS3(value);
	if (!usdTableS3) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}
	// Get reference to usd components
	for (i = 1; i <= 139; i++) {
		usdTableS3 >> lanIndexS3 >> circleIndexS3 >> usdCircleIndexS3 >> serial_usd >> graf >> mecc;
        	usd[circleIndexS3][usdCircleIndexS3] = MinorServo::USD::_nil();
        	try {
			printf("S3: circleIndexS3 = %d, usdCircleIndexS3 = %d\n", circleIndexS3, usdCircleIndexS3);
            		usd[circleIndexS3][usdCircleIndexS3] = m_services->getComponent<MinorServo::USD>(serial_usd);
			lanradius[circleIndexS3][lanIndexS3] = usd[circleIndexS3][usdCircleIndexS3];
			usdCounterS3++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::sector3MinorServo()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
    	}

	printf("sector3 done\n");
	m_sector3 = true;
}

void CNotoMinorServoBossCore::sector4MinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	printf("sector4 start\n");
	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	int i;

	value = "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD_S4.txt\0";
	ifstream usdTableS4(value);
	if (!usdTableS4) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}
	// Get reference to usd components
	for (i = 1; i <= 140; i++) {
		usdTableS4 >> lanIndexS4 >> circleIndexS4 >> usdCircleIndexS4 >> serial_usd >> graf >> mecc;
        	usd[circleIndexS4][usdCircleIndexS4] = MinorServo::USD::_nil();
        	try {
			printf("S4: circleIndexS4 = %d, usdCircleIndexS4 = %d\n", circleIndexS4, usdCircleIndexS4);
            		usd[circleIndexS4][usdCircleIndexS4] = m_services->getComponent<MinorServo::USD>(serial_usd);
			lanradius[circleIndexS4][lanIndexS4] = usd[circleIndexS4][usdCircleIndexS4];
			usdCounterS4++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::sector2MinorServo()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
    	}

	printf("sector4 done\n");
	m_sector4 = true;
}

void CNotoMinorServoBossCore::sector5MinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	printf("sector5 start\n");
	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	int i;

	value = "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD_S5.txt\0";
	ifstream usdTableS5(value);
	if (!usdTableS5) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}
	// Get reference to usd components
	for (i = 1; i <= 139; i++) {
		usdTableS5 >> lanIndexS5 >> circleIndexS5 >> usdCircleIndexS5 >> serial_usd >> graf >> mecc;
        	usd[circleIndexS5][usdCircleIndexS5] = MinorServo::USD::_nil();
        	try {
			printf("S5: circleIndexS5 = %d, usdCircleIndexS5 = %d\n", circleIndexS5, usdCircleIndexS5);
            		usd[circleIndexS5][usdCircleIndexS5] = m_services->getComponent<MinorServo::USD>(serial_usd);
			lanradius[circleIndexS5][lanIndexS5] = usd[circleIndexS5][usdCircleIndexS5];
			usdCounterS5++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::sector5MinorServo()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
    	}

	printf("sector5 done\n");
	m_sector5 = true;
}

void CNotoMinorServoBossCore::sector6MinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	printf("sector6 start\n");
	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	int i;

	value = "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD_S6.txt\0";
	ifstream usdTableS6(value);
	if (!usdTableS6) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}
	// Get reference to usd components
	for (i = 1; i <= 140; i++) {
		usdTableS6 >> lanIndexS6 >> circleIndexS6 >> usdCircleIndexS6 >> serial_usd >> graf >> mecc;
        	usd[circleIndexS6][usdCircleIndexS6] = MinorServo::USD::_nil();
        	try {
			printf("S6: circleIndexS6 = %d, usdCircleIndexS6 = %d\n", circleIndexS6, usdCircleIndexS6);
            		usd[circleIndexS6][usdCircleIndexS6] = m_services->getComponent<MinorServo::USD>(serial_usd);
			lanradius[circleIndexS6][lanIndexS6] = usd[circleIndexS6][usdCircleIndexS6];
			usdCounterS6++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::sector2MinorServo()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
    	}

	printf("sector6 done\n");
	m_sector6 = true;
}

void CNotoMinorServoBossCore::sector7MinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	printf("sector7 start\n");
	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	int i;

	value = "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD_S7.txt\0";
	ifstream usdTableS7(value);
	if (!usdTableS7) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}
	// Get reference to usd components
	for (i = 1; i <= 139; i++) {
		usdTableS7 >> lanIndexS7 >> circleIndexS7 >> usdCircleIndexS7 >> serial_usd >> graf >> mecc;
        	usd[circleIndexS7][usdCircleIndexS7] = MinorServo::USD::_nil();
        	try {
			printf("S7: circleIndexS7 = %d, usdCircleIndexS7 = %d\n", circleIndexS7, usdCircleIndexS7);
            		usd[circleIndexS7][usdCircleIndexS7] = m_services->getComponent<MinorServo::USD>(serial_usd);
			lanradius[circleIndexS7][lanIndexS7] = usd[circleIndexS7][usdCircleIndexS7];
			usdCounterS7++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::sector3MinorServo()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
    	}

	printf("sector7 done\n");
	m_sector7 = true;
}

void CNotoMinorServoBossCore::sector8MinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	printf("sector8 start\n");
	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	int i;

	value = "/home/gavino/Nuraghe/ACS/trunk/Noto/Configuration/CDB/alma/AS/tab_convUSD_S8.txt\0";
	ifstream usdTableS8(value);
	if (!usdTableS8) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}
	// Get reference to usd components
	for (i = 1; i <= 140; i++) {
		usdTableS8 >> lanIndexS8 >> circleIndexS8 >> usdCircleIndexS8 >> serial_usd >> graf >> mecc;
        	usd[circleIndexS8][usdCircleIndexS8] = MinorServo::USD::_nil();
        	try {
			printf("S8: circleIndexS8 = %d, usdCircleIndexS8 = %d\n", circleIndexS8, usdCircleIndexS8);
            		usd[circleIndexS8][usdCircleIndexS8] = m_services->getComponent<MinorServo::USD>(serial_usd);
			lanradius[circleIndexS8][lanIndexS8] = usd[circleIndexS8][usdCircleIndexS8];
			usdCounterS8++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::sector2MinorServo()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
    	}

	printf("sector8 done\n");
	m_sector8 = true;
}
*/

void CNotoMinorServoBossCore::workingMinorServo() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
		char buff[SENDBUFFERSIZE];
		int res;
		int n;
		char str[80];

    	if (AutoUpdate) {
		TIMEVALUE now;
		double azimuth=0.0;
     	double elevation=0.0;
		double diff=0.0;
		int elev;

		IRA::CIRATools::getTime(now);

		if (!CORBA::is_nil(m_antennaBoss)) {
			try {
	            		m_antennaBoss->getRawCoordinates(now.value().value, azimuth, elevation);
        		}	
        		catch (CORBA::SystemException& ex) {
            			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::workingMinorServo()");
            			impl.setName(ex._name());
				    		impl.setMinor(ex.minor());
            			m_status=Management::MNG_WARNING;
							//asPark(); TBC!!!
            			throw impl;
	    		}
        		azimuth = 0.0;
				diff = fabs(m_elevation - elevation*DR2D);
				m_elevation=elevation*DR2D;
        		try {
            			//onewayAction(MinorServo::AS_UPDATE, 0, 0, 0, elevation*DR2D, 0, 0, m_profile);
							if ((elevation*DR2D > 5.0) && (elevation*DR2D < 90.0) && (diff > 0.5)) {
								//sprintf(buff,"@-%02.0lf\n", elevation*DR2D);
								elev = (int)(floor(elevation*DR2D+0.5));
								sprintf (buff, "0,%d,%d", mode, m_config);
								for (n = 0; n < ((mode) ? 5 : 2); n++) {
            					sprintf (str, ",%6.2f", scupos[elev][n]);      /* comanded position + user offset */
            					strcat (buff, str);
          					}
								res = sendBuffer(buff,strlen(buff));
								//printf("diff = %f, buff = %s", diff, buff);
							}
							else
								printf("bad elevation = %lf\n", elevation*DR2D);
        		}
        		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
            			ex.log(LM_DEBUG);
            			throw ex.getComponentErrorsEx();
        		}
		}
    	}
}
/*
void CNotoMinorServoBossCore::setProfile(const MinorServo::TASProfile& newProfile) throw (ComponentErrors::ComponentErrorsExImpl)
{
	int s, i, l;
	char * value;

	if ((m_sector1 == true)&&(m_sector2 == true)&&(m_sector3 == true)&&(m_sector4 == true)&&(m_sector5 == true)&&(m_sector6 == true)&&(m_sector7 == true)&&(m_sector8 == true)) {
		//printf("setProfile corrections\n");
		value = USDTABLECORRECTIONS;
    		ifstream usdCorrections (value);
    		if (!usdCorrections) {
        		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
			exit(-1);
		}
		actuatorsCorrections.length(NPOSITIONS);
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
		//		printf ("Corrections = ");
				for (s = 0; s < NPOSITIONS; s++) {
        				usdCorrections >> actuatorsCorrections[s];
					//printf ("%f ", actuatorsCorrections[s]);
        			}
		//		printf("\n");
				if (!CORBA::is_nil(usd[i][l])) {
		//			printf("i = %d, l = %d\n", i, l);
					usd[i][l]->posTable(actuatorsCorrections, NPOSITIONS, DELTAEL, THRESHOLDPOS);
				}
			}
		}
		m_sector1 = false;
		m_sector2 = false;
		m_sector3 = false;
		m_sector4 = false;
		m_sector5 = false;
		m_sector6 = false;
		m_sector7 = false;
		m_sector8 = false;
		usdCounter = usdCounterS1 + usdCounterS2 + usdCounterS3 + usdCounterS4 + usdCounterS5 + usdCounterS6 + usdCounterS7 + usdCounterS8;
		
		m_ASup=true;
        	m_status=Management::MNG_OK;

		if (usdCounter < (int)lastUSD*WARNINGUSDPERCENT) {
        		m_status=Management::MNG_WARNING;
		}
    		if (usdCounter < (int)lastUSD*ERRORUSDPERCENT) {
        		m_status=Management::MNG_FAILURE;
			m_ASup=false;
		}
		//printf("usdCounter = %d\n", usdCounter);
	}	

	if (m_ASup == true) {
		CIRATools::Wait(1000000);
		_SET_CDB_CORE(profile, newProfile,"NotoMinorServoBossCore::setProfile")
		m_profile = newProfile;
		try {
        		onewayAction(MinorServo::AS_PROFILE, 0, 0, 0, 0, 0, 0, newProfile);
        	}
        	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
            		ex.log(LM_DEBUG);
            		throw ex.getComponentErrorsEx();
        	}

		m_profileSetted = true;

        	CIRATools::Wait(1000000);
		asOn();
	}
}
*/

void CNotoMinorServoBossCore::msSetup(const char *conf) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CDBAccessExImpl)
{
	setDefaultAxesValues(conf);
}

void CNotoMinorServoBossCore::setDefaultAxesValues(const IRA::CString & conf)
{
	int axis;
	float xValues[POLY_COEFF];
	float yValues[POLY_COEFF];
	float z1Values[POLY_COEFF];
	float z2Values[POLY_COEFF];
	float z3Values[POLY_COEFF];

	if (conf.Compare("KAP")==0) {
     m_config = 1; mode = 0; axis = 2;
	}
	if (conf.Compare("SXP")==0) {
     m_config = 2; mode = 0; axis = 2;
	}
	if (conf.Compare("KUP")==0) {
     m_config = 3; mode = 0; axis = 2;
	}
	if (conf.Compare("UHP")==0) {
     m_config = 4; mode = 0; axis = 2;
	}
	if (conf.Compare("PPP")==0) {
     m_config = 5; mode = 0; axis = 2;
	}
	if (conf.Compare("WWP")==0) {
     m_config = 6; mode = 0; axis = 2;
	}
	if (conf.Compare("KKC")==0) {
     m_config = 1; mode = 1; axis = 5;
	}
	if (conf.Compare("CCC")==0) {
		m_config = 2; mode = 1; axis = 5;
		xValues[4]=0; xValues[3]=0; xValues[2]=0; xValues[1]=0; xValues[0]=-0.5;
		PolyEvaluation (0, xValues, POLY_COEFF);
		yValues[4]=0; yValues[3]=0; yValues[2]=8.3689e-4; yValues[1]=.152495; yValues[0]=-11.7;
		PolyEvaluation (1, yValues, POLY_COEFF);
		z1Values[4]=0; z1Values[3]=0; z1Values[2]=0.00168640; z1Values[1]=-0.271430; z1Values[0]=17.3;
		PolyEvaluation (2, z1Values, POLY_COEFF);
		z2Values[4]=0; z2Values[3]=0; z2Values[2]=0.00168640; z2Values[1]=-0.271430; z2Values[0]=9.8;
		PolyEvaluation (3, z2Values, POLY_COEFF);
		z3Values[4]=0; z3Values[3]=0; z3Values[2]=0.00168640; z3Values[1]=-0.271430; z3Values[0]=12.6;
		PolyEvaluation (4, z3Values, POLY_COEFF);
	}
	if (conf.Compare("MMC")==0) {
     m_config = 3; mode = 1; axis = 5;
	}
	if (conf.Compare("LLC")==0) {
     m_config = 4; mode = 1; axis = 5;
	}
	if (conf.Compare("LHC")==0) {
     m_config = 5; mode = 1; axis = 5;
	}
	if (conf.Compare("QQC")==0) {
     m_config = 6; mode = 1; axis = 5;
	}
}

void CNotoMinorServoBossCore::PolyEvaluation (int Axis, float *Coeff, int Dim)
/* computes scu axis position table */
{
  int i, j;
  float Xi, l=0.0;
  for (i = 0; i <= 90; i++) {   /* make 1 point every deg from 0 to 90 */
    scupos[i][Axis] = 0;
    for (j = 0; j < Dim; j++) {
      Xi = pow ((float)i, j);
      scupos[i][Axis] += Coeff[j] * Xi;
		//printf("j= %d, pos = %f\n", j, scupos[i][Axis]);
    }
  }
}
/*
void
PolyEvaluation (Axis, Coeff, Dim)
     int Axis;
     float *Coeff;
     int Dim;
// computes scu axis position table
{
  int i, j;
  float Xi;
  for (i = 0; i <= 90; i++) {   // make 1 point every deg from 0 to 90
    st->scupos[i][Axis] = 0;
    for (j = 0; j < Dim; j++) {
      Xi = pow (i, j);
      st->scupos[i][Axis] += Coeff[j] * Xi;
    }
  }
}
*/

void CNotoMinorServoBossCore::msOn()
{
	//if (m_profileSetted == true ) {
	//	if ((m_profile != MinorServo::AS_PARABOLIC_FIXED) && (m_profile != MinorServo::AS_SHAPED_FIXED)) {
			enableAutoUpdate(); // as=enable nel FS di Noto
			m_tracking = true;
	//	}
	//	else {
	//		m_tracking = false;
	//		try {
	//			onewayAction(MinorServo::AS_UPDATE, 0, 0, 0, 45.0, 0, 0, m_profile);
	//		}
	//		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
	//			ex.log(LM_DEBUG);
	//			throw ex.getComponentErrorsEx();
	//		}
	//	}
	//}
	//else {
	//	printf("you must set the profile first\n");
	//}
}

void CNotoMinorServoBossCore::msPark() throw (ComponentErrors::ComponentErrorsEx)
{
	char buff[SENDBUFFERSIZE];
	int res;

	//if (m_profileSetted == true ) {
		try {
	//		onewayAction(MinorServo::AS_STOP, 0, 0, 0, 0, 0, 0, m_profile);
			//send @-45 string to the server
			sprintf(buff,"@-1\n"); // as=stow nel FS di Noto
			res = sendBuffer(buff,4);
		}
		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
		}
		disableAutoUpdate();
	//}
	//else {
	//	printf("you must set the profile first\n");
	//}
}

void CNotoMinorServoBossCore::msOff() throw (ComponentErrors::ComponentErrorsEx)
{
	char buff[SENDBUFFERSIZE];
	int res;

	//if (m_profileSetted == true ) {
		try {
	//		onewayAction(MinorServo::AS_STOP, 0, 0, 0, 0, 0, 0, m_profile);
			//send @-45 string to the server
			sprintf(buff,"@-45\n"); // as=disable nel FS di Noto
			res = sendBuffer(buff,5);
		}
		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
		}
		disableAutoUpdate();
	//}
	//else {
	//	printf("you must set the profile first\n");
	//}
}
/*
void CNotoMinorServoBossCore::setActuator(int circle, int actuator, long int& actPos, long int& cmdPos, long int& Fmin, long int& Fmax, long int& acc,long int& delay) throw (ComponentErrors::PropertyErrorExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
    if (!CORBA::is_nil(usd[circle][actuator])) {
        ACSErr::Completion_var completion;
	    ACS::ROlong_var actPos_var; 
	    ACS::RWlong_var cmdPos_var;
	    ACS::RWlong_var Fmin_var;
	    ACS::RWlong_var Fmax_var;
	    ACS::RWlong_var acc_var;
	    ACS::RWlong_var delay_var;

        actPos_var = usd[circle][actuator]->actPos ();
        if (actPos_var.ptr() != ACS::ROlong::_nil()) {
            actPos = actPos_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CNotoMinorServoBossCore::setActuator()");
            impl.setPropertyName("usd actual position");
            throw impl;
        }
        cmdPos_var = usd[circle][actuator]->cmdPos ();
        if (cmdPos_var.ptr() != ACS::RWlong::_nil()) {
            cmdPos = cmdPos_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CNotoMinorServoBossCore::setActuator()");
            impl.setPropertyName("usd commanded position");
            throw impl;
        }
        Fmin_var = usd[circle][actuator]->Fmin ();
        if (Fmin_var.ptr() != ACS::RWlong::_nil()) {
            Fmin = Fmin_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CNotoMinorServoBossCore::setActuator()");
            impl.setPropertyName("usd Fmin");
            throw impl;
        }
        Fmax_var = usd[circle][actuator]->Fmax ();
        if (Fmax_var.ptr() != ACS::RWlong::_nil()) {
            Fmax = Fmax_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CNotoMinorServoBossCore::setActuator()");
            impl.setPropertyName("usd Fmax");
            throw impl;
        }
        acc_var = usd[circle][actuator]->acc ();
        if (acc_var.ptr() != ACS::RWlong::_nil()) {
            acc = acc_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CNotoMinorServoBossCore::setActuator()");
            impl.setPropertyName("usd acceleration");
            throw impl;
        }
        delay_var = usd[circle][actuator]->delay ();
        if (delay_var.ptr() != ACS::RWlong::_nil()) {
            delay = delay_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CNotoMinorServoBossCore::setActuator()");
            impl.setPropertyName("usd delay");
            throw impl;
        }
    }
    else {
        _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CNotoMinorServoBossCore::setActuator()");
    }
}

void CNotoMinorServoBossCore::usdStatus4GUIClient(int circle, int actuator, CORBA::Long_out status) throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
	ACS::ROpattern_var status_var;
    	ACSErr::Completion_var completion;

    	if (!CORBA::is_nil(usd[circle][actuator])) {
        	try {
            		status_var = usd[circle][actuator]->status ();
            		status = status_var->get_sync (completion.out ());
        	}
        	catch (CORBA::SystemException& ex) {
            		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CNotoMinorServoBossCore::usdStatus4GUIClient()");
            		impl.setName(ex._name());
			impl.setMinor(ex.minor());
            		throw impl;
		}
    	}
    	else {
        	_THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CNotoMinorServoBossCore::usdStatus4GUIClient()");
    	}
}

void CNotoMinorServoBossCore::recoverUSD(int circleIndex, int usdCircleIndex) throw (ComponentErrors::CouldntGetComponentExImpl)
{
    char serial_usd[23];

    setserial (circleIndex, usdCircleIndex, lanIndex, serial_usd);

    usd[circleIndex][usdCircleIndex] = MinorServo::USD::_nil();
    try {
        usd[circleIndex][usdCircleIndex] = m_services->getComponent<MinorServo::USD>(serial_usd);
        lanradius[circleIndex][lanIndex] = usd[circleIndex][usdCircleIndex];
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CNotoMinorServoBossCore::recoverUSD()");
        Impl.setComponentName(serial_usd);
        Impl.log(LM_DEBUG);
    }
}

void CNotoMinorServoBossCore::setradius(int radius, int &actuatorsradius, int &jumpradius)
{
	if (radius ==  1 || radius ==  5 || radius ==  9 || radius == 13 ||
	    radius == 17 || radius == 21 || radius == 25 || radius == 29 ||
	    radius == 33 || radius == 37 || radius == 41 || radius == 45 ||
	    radius == 49 || radius == 53 || radius == 57 || radius == 61 ||
	    radius == 65 || radius == 69 || radius == 73 || radius == 77 ||
	    radius == 81 || radius == 85 || radius == 89 || radius == 93)
	{
		actuatorsradius = 13;
		if (radius == 13 || radius == 37 || radius == 61 || radius == 85)
			actuatorsradius++;
		jumpradius = 2;
	}
	else if (radius ==  3 || radius ==  7 || radius == 11 || radius == 15 ||
	    	 radius == 19 || radius == 23 || radius == 27 || radius == 31 ||
	    	 radius == 35 || radius == 39 || radius == 43 || radius == 47 ||
	    	 radius == 51 || radius == 55 || radius == 59 || radius == 63 ||
	    	 radius == 67 || radius == 71 || radius == 75 || radius == 79 ||
	    	 radius == 83 || radius == 87 || radius == 91 || radius == 95)
	{
		actuatorsradius = 15;
		if (radius == 11 || radius == 15 || radius == 35 || radius == 39 ||
		    radius == 59 || radius == 63 || radius == 83 || radius == 87)
			actuatorsradius++;
		jumpradius = 0;
	}
	else
	{
		actuatorsradius = 9;
		jumpradius = 6;
	}
}

void CNotoMinorServoBossCore::setserial (int circle, int actuator, int &lan, char* serial_usd)
{
    int sector = 0;

    if (circle >= 1 && circle <= 2) {
        if (actuator >= 1 && actuator <= 3)
            sector = 1;
        if (actuator >= 4 && actuator <= 6)
                sector = 2;
        if (actuator >= 7 && actuator <= 9)
                sector = 3;
        if (actuator >= 10 && actuator <= 12)
                sector = 4;
        if (actuator >= 13 && actuator <= 15)
                sector = 5;
        if (actuator >= 16 && actuator <= 18)
                sector = 6;
        if (actuator >= 19 && actuator <= 21)
                sector = 7;
        if (actuator >= 22 && actuator <= 24)
                sector = 8;

        lan = actuator*4 - 1;
    }
    if (circle >= 3 && circle <= 6) {
        if (actuator >= 1 && actuator <= 6)
                sector = 1;
        if (actuator >= 7 && actuator <= 12)
                sector = 2;
        if (actuator >= 13 && actuator <= 18)
                sector = 3;
        if (actuator >= 19 && actuator <= 24)
                sector = 4;
        if (actuator >= 25 && actuator <= 30)
                sector = 5;
        if (actuator >= 31 && actuator <= 36)
                sector = 6;
        if (actuator >= 37 && actuator <= 42)
                sector = 7;
        if (actuator >= 43 && actuator <= 48)
                sector = 8;

        lan = actuator*2 -1;
    }
    if (circle >= 7 && circle <= 15) {
        if (actuator >= 1 && actuator <= 12)
                sector = 1;
        if (actuator >= 13 && actuator <= 24)
                sector = 2;
        if (actuator >= 25 && actuator <= 36)
                sector = 3;
        if (actuator >= 37 && actuator <= 48)
                sector = 4;
        if (actuator >= 49 && actuator <= 60)
                sector = 5;
        if (actuator >= 61 && actuator <= 72)
                sector = 6;
        if (actuator >= 73 && actuator <= 84)
                sector = 7;
        if (actuator >= 85 && actuator <= 96)
                sector = 8;

        lan = actuator;
    }
    if (circle == 16) {
        sector = actuator;
        if (actuator == 1)
            lan = 11;
        if (actuator == 2)
            lan = 15;
        if (actuator == 3)
            lan = 35;
        if (actuator == 4)
            lan = 39;
        if (actuator == 5)
            lan = 59;
        if (actuator == 6)
            lan = 63;
        if (actuator == 7)
            lan = 83;
        if (actuator == 8)
            lan = 87;
    }
    if (circle == 17) {
        sector = actuator*2;
        if (actuator == 1)
            lan = 13;
        if (actuator == 2)
            lan = 37;
        if (actuator == 3)
            lan = 61;
        if (actuator == 4)
            lan = 85;
    }
    lan -= 12*(sector-1);
    sprintf (serial_usd,"AS/SECTOR%02d/LAN%02d/USD%02d",sector, lan, circle);
}
*/
void CNotoMinorServoBossCore::enableAutoUpdate()
{
	if (!AutoUpdate) {
		AutoUpdate=true;
		ACS_LOG(LM_FULL_INFO,"CNotoMinorServoBossCore::enableAutoUpdate()",(LM_NOTICE,"NotoMinorServoBoss::AUTOMATIC_UPDATE_ENABLED"));		
	}
}

void CNotoMinorServoBossCore::disableAutoUpdate()
{
	if (AutoUpdate) {
		AutoUpdate=false;
		ACS_LOG(LM_FULL_INFO,"CNotoMinorServoBossCore::disableAutoUpdate()",(LM_NOTICE,"NotoMinorServoBoss::AUTOMATIC_UPDATE_DISABLED"));		
	}
}
/*
void CNotoMinorServoBossCore::checkASerrors(char* str, int circle, int actuator, ASErrors::ASErrorsEx Ex)
{
	ASErrors::ASErrorsExImpl exImpl(Ex);

	printf("checkASerrors: ");
	printf("%s %d_%d: ", str, circle, actuator);

	switch(exImpl.getErrorCode()) {
		case ASErrors::NoError:
      printf ("NoError\n");
      break;
    case ASErrors::LibrarySocketError:
      printf ("LibrarySocketError\n");
      break;
    case ASErrors::SocketReconn:
      printf ("SocketReconn\n");
      break;
    case ASErrors::SocketFail:
      printf ("SocketFail\n");
      break;
    case ASErrors::SocketTOut:
      printf ("SocketTOut\n");
      break;
    case ASErrors::SocketNotRdy:
      printf ("SocketNotRdy\n");
      break;
    case ASErrors::MemoryAllocation:
      printf ("MemoryAllocation\n");
      break;
    case ASErrors::Incomplete:
      printf ("Incomplete\n");
      break;
    case ASErrors::InvalidResponse:
      printf ("InvalideResponse\n");
      break;
    case ASErrors::Nak:
      printf ("Nak\n");
      break;
    case ASErrors::CDBAccessError:
      printf ("CDBAccessError\n");
      break;
    case ASErrors::USDConnectionError:
      printf ("USDConnectionError\n");
      break;
    case ASErrors::USDTimeout:
      printf ("USDTimeout\n");
      break;
    case ASErrors::LANConnectionError:
      printf ("LANConnectionError\n");
      break;
    case ASErrors::LAN_Unavailable:
      printf ("LANUnavailable\n");
      break;
    case ASErrors::sendCmdErr:
      printf ("sendCmdErr\n");
      break;
    case ASErrors::USDUnavailable:
      printf ("USDUnavailable\n");
      break;
    case ASErrors::USDError:
      printf ("USDError\n");
      break;
    case ASErrors::DevIOError:
      printf ("DevIOError\n");
      break;
    case ASErrors::corbaError:
      printf ("corbaError\n");
      break;
    case ASErrors::USDStillRunning:
      printf ("USDStillRunning\n");
      break;
    case ASErrors::USDunCalibrated:
      printf ("USDunCalibrated\n");
      break;
    case ASErrors::CannotGetUSD:
      printf ("CannotGetUSD\n");
      break;
	}
}

void CNotoMinorServoBossCore::checkAScompletionerrors (char *str, int circle, int actuator, CompletionImpl comp)
{
  printf ("%s %d_%d ", str, circle, actuator);

  switch (comp.getCode ())
    {case 0:
      printf ("NoError\n");
      break;
    case 1:
      printf ("USD calibrated\n");
      break;
    case 2:
      printf ("LibrarySocketError\n");
      break;
    case 3:
      printf ("SocketReconn\n");
      break;
    case 4:
      printf ("SocketFail\n");
      break;
    case 5:
      printf ("SocketTOut\n");
      break;
    case 6:
      printf ("SocketNotRdy\n");
      break;
    case 7:
      printf ("MemoryAllocation\n");
      break;
    case 8:
      printf ("LANConnectionError\n");
      break;
    case 9:
      printf ("LANUnavailable\n");
      break;
    case 10:
      printf ("sendCmdErr\n");
      break;
    case 11:
      printf ("Incomplete\n");
      break;
    case 12:
      printf ("InvalidResponse\n");
      break;
    case 13:
      printf ("Nak\n");
      break;
    case 14:
      printf ("CDBAccessError\n");
      break;
    case 15:
      printf ("USDConnectionError\n");
      break;
    case 16:
      printf ("USDTimeout\n");
      break;
    case 17:
      printf ("USDUnavailable\n");
      break;
    case 18:
      printf ("USDError\n");
      break;
    case 19:
      printf ("DevIOError\n");
      break;
    case 20:
      printf ("corbaError\n");
      break;
    case 21:
      printf ("USDStillRunning\n");
      break;
    case 22:
      printf ("USDunCalibrated\n");
      break;
    case 23:
      printf ("CannotGetUSD\n");
      break;
    }
}
*/
void  CNotoMinorServoBossCore::setStatus(TLineStatus sta)
{
	m_Linestatus=sta;
	if (m_Linestatus!=CNTD) {
		setStatusField(CMDLINERROR);
	}
	else {
		clearStatusField(CMDLINERROR);
	}
}

bool CNotoMinorServoBossCore::checkConnection()
{
	CError Tmp;
	char sBuffer[RECBUFFERSIZE];
	int rBytes;
	if (m_bTimedout) { // if a previous time out happend....try if the buffer has still to be received 
		rBytes=receiveBuffer(sBuffer,RECBUFFERSIZE);
		if (rBytes==WOULDBLOCK) { // if the operation would have block again.....declare the disconnection
			setStatus(NOTCNTD);	// another timeout! something has happend
			//_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::checkConnection()","SOCKET_DISCONNECTED - timeout expired");
		}
		else if (rBytes==FAIL) {
			// Nothing to do, this error will be handled below....
		}
		else if (rBytes==0) {
			// Nothing to do, this error will be handled below....
		}
		else {
			m_bTimedout=false; // timeout recovered
		}
	}
	if (getLineStatus()==CNTD)  {
		return true;
	}
	else if (getLineStatus()==CNTDING) {
		return false;
	}
	else {  // socket is not connected....
		// try to close the socket, if it is already closed : never known....
		Close(Tmp);
		Tmp.Reset();
		m_bTimedout=false;
		// this will create the socket in blocking mode.....
		if (Create(Tmp,STREAM)==SUCCESS) {
			// Put the socket in non-blocking mode, registers the onconnect event....and allow some seconds to complete!
			if (EventSelect(Tmp,E_CONNECT,true,m_configuration->getConnectTimeout())==SUCCESS) {
				OperationResult Res;
				ACS_DEBUG("CCommandLine::checkConnection()","Trying to reconnect");
				Res=Connect(Tmp,m_configuration->getAddress(),m_configuration->getPort());
				if (Res==WOULDBLOCK) {
					setStatus(CNTDING);
				}
				else if (Res==SUCCESS) {
					setStatus(CNTD);
					/*try {
						stopDataAcquisitionForced(); // ask the backend to exit the data transfering mode......
					}
					catch (ACSErr::ACSbaseExImpl& ex) {
						
					}*/
					clearStatusField(BUSY);
					m_reiniting=true;
					ACS_LOG(LM_FULL_INFO,"CCommandLine::checkConnection()",(LM_NOTICE,"SOCKET_RECONNECTED")); //we do not want to filter this info so no logfilter.....				
					return true;
				}
			}
		}
		return false;
	}
}

IRA::CSocket::OperationResult CNotoMinorServoBossCore::sendBuffer(char *Msg,WORD Len)
{
	int NWrite;
	int BytesSent;
	BytesSent=0;
	while (BytesSent<Len) {
		if ((NWrite=Send(m_Error,(const void *)(Msg+BytesSent),Len-BytesSent))<0) {
			if (NWrite==WOULDBLOCK) {
				setStatus(NOTCNTD);
				//_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::sendBuffer()","SOCKET_DISCONNECTED - remote side shutdown");
				return WOULDBLOCK;
			}
			else {
				setStatus(NOTCNTD);
				CString app;
				app.Format("SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
				//_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::sendBuffer()",(const char*)app);
				return FAIL;
			}
		}
		else { // success
			BytesSent+=NWrite;
		}
	}
	if (BytesSent==Len) {
		return SUCCESS;
   }
	else {
		_SET_ERROR(m_Error,CError::SocketType,CError::SendError,"CCommandLine::SendBuffer()");
		return FAIL;
	}
}

int CNotoMinorServoBossCore::receiveBuffer(char *Msg,WORD Len)
{
	char inCh;
	int res;
	int nRead=0;
	TIMEVALUE Now;
	TIMEVALUE Start;
	CIRATools::getTime(Start);
	while(true) {
		res=Receive(m_Error,&inCh,1);
		if (res==WOULDBLOCK) {
			CIRATools::getTime(Now);
			if (CIRATools::timeDifference(Start,Now)>m_configuration->getCommandLineTimeout()) {
				m_bTimedout=true;
				return WOULDBLOCK;
			}
			else {
				CIRATools::Wait(0,20000);
				continue;
			}
		}
		else if (res==FAIL) { 
			setStatus(NOTCNTD);
			CString app;
			app.Format("SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
			//_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::receiveBuffer()",(const char*)app);
			return res;
		}
		else if (res==0) {
			setStatus(NOTCNTD);
			//_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::receiveBuffer()","SOCKET_DISCONNECTED - remote side shutdown");			
			return res;
		}
		else {
			if (inCh!=PROT_TERMINATOR_CH) {
				Msg[nRead]=inCh;
				nRead++;
			}
			else {
				Msg[nRead]=0;
				return nRead;
			}
		}
	}
}
