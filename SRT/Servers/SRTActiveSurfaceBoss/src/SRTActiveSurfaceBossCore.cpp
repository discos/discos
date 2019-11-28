#include "SRTActiveSurfaceBossCore.h"
#include <Definitions.h>
#include <cstdio>

int actuatorsInCircle[] = {0,24,24,48,48,48,48,96,96,96,96,96,96,96,96,96,8,4};

CSRTActiveSurfaceBossCore::CSRTActiveSurfaceBossCore(ContainerServices *service, acscomponent::ACSComponentImpl *me) :
	m_services(service),
    m_thisIsMe(me)
{
}

CSRTActiveSurfaceBossCore::~CSRTActiveSurfaceBossCore()
{
}

void CSRTActiveSurfaceBossCore::initialize()
{
	ACS_LOG(LM_FULL_INFO,"CSRTActiveSurfaceBossCore::initialize()",(LM_INFO,"CSRTActiveSurfaceBossCore::initialize"));

	m_enable = false;
	m_tracking = false;
	m_status = Management::MNG_WARNING;
	m_profile = ActiveSurface::AS_SHAPED_FIXED;
	AutoUpdate = false;
	actuatorcounter = circlecounter = totacts = 1;
	for(int i = 0; i < SECTORS; i++)
	{
		m_sector.push_back(false);
		usdCounters.push_back(0);
		lanIndexes.push_back(0);
		circleIndexes.push_back(0);
		usdCircleIndexes.push_back(0);
	}
	m_profileSetted = false;
	m_ASup = false;
}

void CSRTActiveSurfaceBossCore::execute() throw (ComponentErrors::CouldntGetComponentExImpl)
{
/*    	char serial_usd[23];
	char graf[5], mecc[4];
	char * value;
	char * value2;
	int s, i, l;

	//s_usdTable = getenv ("ACS_CDB");
	//strcat(s_usdTable,USDTABLE);
	value2 = USDTABLE;
	//ifstream usdTable(s_usdTable);
	ifstream usdTable(value2);
	if (!usdTable) {
		//ACS_SHORT_LOG ((LM_INFO, "File %s not found", s_usdTable));
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value2));
		exit(-1);
	}

	value = USDTABLECORRECTIONS;
    	ifstream usdCorrections (value);
    	if (!usdCorrections) {
        	ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}

	actuatorsCorrections.length(NPOSITIONS);
*/
/*
	// get reference to lan components
	for (int s = 1; s <= 8; s++)
	{
		for (int l = 1; l <= 12; l++)
		{
			lanCobName.Format("AS/SECTOR%02d/LAN%02d",s, l);
			ACS_SHORT_LOG((LM_INFO, "Getting component: %s", (const char*)lanCobName));
			printf("lan = %s\n", (const char*)lanCobName);
			lan[s][l] = ActiveSurface::lan::_nil();
			try {
				lan[s][l] = m_services->getComponent<ActiveSurface::lan>((const char *)lanCobName);
			}
        		catch (maciErrType::CannotGetComponentExImpl& ex) {
            			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::execute()");
            			Impl.setComponentName((const char*)lanCobName);
            			Impl.log(LM_DEBUG);
	    		}
        		CIRATools::Wait(LOOPTIME);
		}
    	}
	ACS_LOG(LM_FULL_INFO, "CSRTActiveSurfaceBossCore::execute()", (LM_INFO,"CSRTActiveSurfaceBossCore::LAN_LOCATED"));
*/
    	// Get reference to usd components
/*	for (i = firstUSD; i <= lastUSD; i++) {
		usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc;
        	usd[circleIndex][usdCircleIndex] = ActiveSurface::USD::_nil();
        	try {
            		//usd[circleIndex][usdCircleIndex] = m_services->getComponent<ActiveSurface::USD>(serial_usd);
			//lanradius[circleIndex][lanIndex] = usd[circleIndex][usdCircleIndex];
			usdCounter++;
        	}
        	catch (maciErrType::CannotGetComponentExImpl& ex) {
            		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::execute()");
            		Impl.setComponentName(serial_usd);
            		Impl.log(LM_DEBUG);
	    	}
        	//CIRATools::Wait(LOOPTIME);
    	}
*/
/*	for (i = 1; i <= CIRCLES; i++) {
		for (l = 1; l <= actuatorsInCircle[i]; l++) {
			//printf ("Corrections = ");
			for (s = 0; s < NPOSITIONS; s++) {
        			usdCorrections >> actuatorsCorrections[s];
			//	printf ("%f ", actuatorsCorrections[s]);
        		}
			//printf("\n");
			if (!CORBA::is_nil(usd[i][l])) {
				//usd[i][l]->posTable(actuatorsCorrections, NPOSITIONS, DELTAEL, THRESHOLDPOS);
			}
		}
	}
	ACS_LOG(LM_FULL_INFO, "CSRTActiveSurfaceBossCore::execute()", (LM_INFO,"CSRTActiveSurfaceBossCore::USD_LOCATED"));

    	if (usdCounter < (int)lastUSD*WARNINGUSDPERCENT)
        	m_status=Management::MNG_WARNING;
    	if (usdCounter < (int)lastUSD*ERRORUSDPERCENT)
        	m_status=Management::MNG_FAILURE;
*/
    	m_antennaBoss = Antenna::AntennaBoss::_nil();
    	try {
        	m_antennaBoss = m_services->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
    	}
    	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::execute()");
		Impl.setComponentName("ANTENNA/Boss");
        	m_status=Management::MNG_WARNING;
		throw Impl;
	}
    	m_enable = true;
    	ACS_LOG(LM_FULL_INFO, "CSRTActiveSurfaceBossCore::execute()", (LM_INFO,"CSRTActiveSurfaceBossCore::SRTActiveSurfaceBoss_LOCATED"));
}

void CSRTActiveSurfaceBossCore::cleanUp()
{
	ACS_LOG(LM_FULL_INFO, "CSRTActiveSurfaceBossCore::cleanUp()", (LM_INFO,"CSRTActiveSurfaceBossCore::cleanUp"));

	char serial_usd[23], graf[5], mecc[4];

	ACS_LOG(LM_FULL_INFO, "CSRTActiveSurfaceBossCore::cleanUp()", (LM_INFO,"Releasing usd...wait"));

    for(int sector = 0; sector < SECTORS; sector++)
    {
        std::stringstream value;
        value << CDBPATH;
        value << "alma/AS/tab_convUSD_S";
        value << sector+1;
        value << ".txt";

        ifstream usdTable(value.str().c_str());
        std::string buffer;

        while(getline(usdTable, buffer))
        {
            std::stringstream line;
            line << buffer;
            line >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc;

            try
            {
                if(!CORBA::is_nil(usd[circleIndex][usdCircleIndex]))
                {
                    printf("releasing usd = %s\n", serial_usd);
                    m_services->releaseComponent((const char*)serial_usd);
                }
            }
            catch(maciErrType::CannotReleaseComponentExImpl& ex)
            {
                _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::cleanUp()");
                Impl.setComponentName((const char *)serial_usd);
                Impl.log(LM_DEBUG);
            }
        }
    }

    try
    {
        m_services->releaseComponent((const char*)m_antennaBoss->name());
    }
    catch(maciErrType::CannotReleaseComponentExImpl& ex)
    {
        _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::cleanUp()");
        Impl.setComponentName((const char *)m_antennaBoss->name());
        Impl.log(LM_DEBUG);
    }
}

void CSRTActiveSurfaceBossCore::reset (int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentNotActiveExImpl)
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
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::reset()");
                        impl.setComponentName((const char*)usd[i][l]->name());
			            impl.setOperationName("reset()");
			            impl.log();
					}
					catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                    }
		            catch(...) {
                        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
                        impl.log();
                    }
				}
				else {
                    _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
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
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::reset()");
                        impl.setComponentName((const char*)usd[circle][l]->name());
			            impl.setOperationName("reset()");
			            impl.log();
				}
                catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                }
		        catch(...) {
                    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
                    impl.log();
                }
            }
		    else {
                _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
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
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::reset()");
                        impl.setComponentName((const char*)usd[l+jumpradius][radius]->name());
			            impl.setOperationName("reset()");
			            impl.log();
				}
                catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                }
		        catch(...) {
                    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
                    impl.log();
                }
            }
            else {
                _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
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
		        _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::reset()");
                impl.setComponentName((const char*)usd[circle][actuator]->name());
			    impl.setOperationName("reset()");
			    throw impl;
			}
            catch(CORBA::SystemException &E) {
                _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::reset()");
                impl.setName(E._name());
                impl.setMinor(E.minor());
                throw impl;
            }
            catch(...) {
                _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CSRTActiveSurfaceBossCore::reset()");
            }
        }
        else {
            _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CSRTActiveSurfaceBossCore::reset()");
        }
    }
}

void CSRTActiveSurfaceBossCore::calibrate (int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl)
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

		/*printf("stop.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (!CORBA::is_nil(usd[i][l])) {
					try {
						usd[i][l]->stop();
						printf ("actuator n.%d_%d stop\n", i, l);
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (1);*/

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

		/*printf("stop.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (!CORBA::is_nil(usd[i][l])) {
					try {
						usd[i][l]->stop();
						printf ("actuator n.%d_%d stop\n", i, l);
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (1);*/

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

		/*printf("stop.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (!CORBA::is_nil(usd[i][l])) {
					try {
						usd[i][l]->stop();
						printf ("actuator n.%d_%d stop\n", i, l);
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (1);*/

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

		/*printf("stop.....\n");
		for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            		if (!CORBA::is_nil(usd[circle][l])) {
                		try {
					usd[circle][l]->stop();
					printf ("actuator n.%d_%d stop\n", circle, l);
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", circle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (1);*/

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

		/*printf("stop.....\n");
		for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            		if (!CORBA::is_nil(usd[circle][l])) {
                		try {
					usd[circle][l]->stop();
					printf ("actuator n.%d_%d stop\n", circle, l);
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", circle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (1);*/

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

		/*printf("stop.....\n");
		for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            		if (!CORBA::is_nil(usd[circle][l])) {
                		try {
					usd[circle][l]->stop();
					printf ("actuator n.%d_%d stop\n", circle, l);
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", circle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (1);*/

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

		/*printf("stop.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                		try {
					lanradius[l+jumpradius][radius]->stop();
					printf ("actuator n.%d_%d stop\n", l+jumpradius, radius);
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, radius, Ex);
				}
			}
		}
		ACE_OS::sleep (1);*/

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

		/*printf("stop.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                		try {
					lanradius[l+jumpradius][radius]->stop();
					printf ("actuator n.%d_%d stop\n", l+jumpradius, radius);
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, radius, Ex);
				}
			}
		}
		ACE_OS::sleep (1);*/

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

		/*printf("stop.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][radius])) {
                		try {
					lanradius[l+jumpradius][radius]->stop();
					printf ("actuator n.%d_%d stop\n", l+jumpradius, radius);
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, radius, Ex);
				}
			}
		}
		ACE_OS::sleep (1);*/

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
			/*printf("stop.....\n");
			try {
				usd[circle][actuator]->stop();
				printf ("actuator n.%d_%d stop\n", circle, actuator);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
			ACE_OS::sleep (1);*/
			printf("calibration.....\n");
			try {
				usd[circle][actuator]->calibrate();
				printf ("actuator n.%d_%d calibrate\n", circle, actuator);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
			ACE_OS::sleep (15);
			/*printf("stop.....\n");
			try {
				usd[circle][actuator]->stop();
				printf ("actuator n.%d_%d stop\n", circle, actuator);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", circle, actuator, Ex);
			}
			ACE_OS::sleep (1);*/
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
/*
    if (circle == 0 && actuator == 0 && radius == 0) { // ALL
        int i, l;
        for (i = 1; i <= CIRCLES; i++) {
            for (l = 1; l <= actuatorsInCircle[i]; l++) {
                if (usdinitialized[i][l] == 0) {
                    try {
                        usd[i][l]->top();
                        actPos_var = usd[i][l]->actPos ();
                        while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000) {
                            printf("actuator n.%d_%d waiting to reach top position...\n", i, l);
                        }
                        CompletionImpl comp = usd[i][l]->calibrate();
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
        for (l = 1; l <= actuatorsInCircle[circle]; l++) {
            if (usdinitialized[circle][l] == 0) {
                try {
                    usd[circle][l]->top();
                    actPos_var = usd[circle][l]->actPos ();
                    while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000) {
                        printf("actuator n.%d_%d waiting to reach top position...\n", circle, l);
                    }
					CompletionImpl comp = usd[circle][l]->calibrate();
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
					lanradius[l+jumpradius][radius]->top();
					actPos_var = lanradius[l+jumpradius][radius]->actPos ();
					while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000) {
						printf("actuator n.%d_%d waiting to reach top position...\n", l+jumpradius, radius);
					}
					CompletionImpl comp = lanradius[l+jumpradius][radius]->calibrate();
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
				usd[circle][actuator]->top();
				actPos_var = usd[circle][actuator]->actPos ();
				while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000) {
		    		ACE_OS::sleep (1);
					printf("actuator n.%d_%d waiting to reach top position...\n", circle, actuator);
				}
				CompletionImpl comp = usd[circle][actuator]->calibrate();
				if (comp.isErrorFree() == false)
				    checkAScompletionerrors("usd", circle, actuator, comp);
			}
			catch (ASErrors::ASErrorsEx &Ex) {
			    checkASerrors("actuator", circle, actuator, Ex);
			}
		}
		else
		    printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
		}
*/
}

void CSRTActiveSurfaceBossCore::calVer (int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl)
{
/*
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
*/
}

void CSRTActiveSurfaceBossCore::onewayAction(ActiveSurface::TASOneWayAction onewayAction, int circle, int actuator, int radius, double elevation, double correction, long incr, ActiveSurface::TASProfile profile) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentNotActiveExImpl)
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
                            				case ActiveSurface::AS_STOP:
                                				usd[i][l]->stop();
                                			break;
                            				case ActiveSurface::AS_SETUP:
                                				usd[i][l]->setup();
                                			break;
                            				case ActiveSurface::AS_STOW:
                                				usd[i][l]->stow();
                                			break;
                            				case ActiveSurface::AS_REFPOS:
                                				usd[i][l]->refPos();
                                			break;
                            				case ActiveSurface::AS_UP:
                                				usd[i][l]->up();
                                			break;
                            				case ActiveSurface::AS_DOWN:
                                				usd[i][l]->down();
                                			break;
                            				case ActiveSurface::AS_BOTTOM:
                                				usd[i][l]->bottom();
                                			break;
                            				case ActiveSurface::AS_TOP:
                                				usd[i][l]->top();
                                			break;
                            				case ActiveSurface::AS_UPDATE:
								usd[i][l]->update(elevation);
                                			break;
                            				case ActiveSurface::AS_CORRECTION:
                                				usd[i][l]->correction(correction*MM2STEP);
                                			break;
                            				case ActiveSurface::AS_MOVE:
                                				usd[i][l]->move(incr*MM2STEP);
                                			break;
                            				case ActiveSurface::AS_PROFILE:
                                				usd[i][l]->setProfile(profile);
                                			break;
                        			}
						//CIRATools::Wait(500);
					}
					catch (ASErrors::ASErrorsEx& E) {
						_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::onewayAction()");
                       				impl.setComponentName((const char*)usd[i][l]->name());
			            		impl.setOperationName("onewayAction()");
			            		impl.log();
					}
					catch(CORBA::SystemException &E) {
						_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::onewayAction()");
						impl.setName(E._name());
						impl.setMinor(E.minor());
						impl.log();
					}
					catch(...) {
						_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSRTActiveSurfaceBossCore::onewayAction()");
						impl.log();
					}
				}
				else {
                    			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CSRTActiveSurfaceBossCore::onewayAction()");
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
                        case ActiveSurface::AS_STOP:
                            usd[circle][l]->stop();
                            break;
                        case ActiveSurface::AS_SETUP:
                            usd[circle][l]->setup();
                            break;
                        case ActiveSurface::AS_STOW:
                            usd[circle][l]->stow();
                            break;
                        case ActiveSurface::AS_REFPOS:
                            usd[circle][l]->refPos();
                            break;
                        case ActiveSurface::AS_UP:
                            usd[circle][l]->up();
                            break;
                        case ActiveSurface::AS_DOWN:
                            usd[circle][l]->down();
                            break;
                        case ActiveSurface::AS_BOTTOM:
                            usd[circle][l]->bottom();
                            break;
                        case ActiveSurface::AS_TOP:
                            usd[circle][l]->top();
                            break;
                        case ActiveSurface::AS_UPDATE:
                            break;
                        case ActiveSurface::AS_CORRECTION:
                            usd[circle][l]->correction(correction*MM2STEP);
                            break;
                        case ActiveSurface::AS_MOVE:
                            usd[circle][l]->move(incr*MM2STEP);
                            break;
                        case ActiveSurface::AS_PROFILE:
                            break;
                    }
					//CIRATools::Wait(LOOPTIME);
                }
				catch (ComponentErrors::ComponentErrorsEx& E) {
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::oneWayAction()");
                        impl.setComponentName((const char*)usd[circle][l]->name());
			            impl.setOperationName("oneWayAction()");
			            impl.log();
				}
                catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::oneWayAction()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                }
		        catch(...) {
                    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSRTActiveSurfaceBossCore::oneWayAction()");
                    impl.log();
                }
            }
		    else {
                _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CSRTActiveSurfaceBossCore::oneWayAction()");
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
                        case ActiveSurface::AS_STOP:
                            lanradius[l+jumpradius][radius]->stop();
                            break;
                        case ActiveSurface::AS_SETUP:
                            lanradius[l+jumpradius][radius]->setup();
                            break;
                        case ActiveSurface::AS_STOW:
                            lanradius[l+jumpradius][radius]->stow();
                            break;
                        case ActiveSurface::AS_REFPOS:
                            lanradius[l+jumpradius][radius]->refPos();
                            break;
                        case ActiveSurface::AS_UP:
                            lanradius[l+jumpradius][radius]->up();
                            break;
                        case ActiveSurface::AS_DOWN:
                            lanradius[l+jumpradius][radius]->down();
                            break;
                        case ActiveSurface::AS_BOTTOM:
                            lanradius[l+jumpradius][radius]->bottom();
                            break;
                        case ActiveSurface::AS_TOP:
                            lanradius[l+jumpradius][radius]->top();
                            break;
                        case ActiveSurface::AS_UPDATE:
                            break;
                        case ActiveSurface::AS_CORRECTION:
                            lanradius[l+jumpradius][radius]->correction(correction*MM2STEP);
                            break;
                        case ActiveSurface::AS_MOVE:
                            lanradius[l+jumpradius][radius]->move(incr*MM2STEP);
                            break;
                        case ActiveSurface::AS_PROFILE:
                            break;
                    }
					//CIRATools::Wait(LOOPTIME);
                }
                catch (ComponentErrors::ComponentErrorsEx& E) {
					    _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::oneWayAction()");
                        impl.setComponentName((const char*)lanradius[l+jumpradius][radius]->name());
			            impl.setOperationName("oneWayAction()");
			            impl.log();
				}
                catch(CORBA::SystemException &E) {
                        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::oneWayAction()");
                        impl.setName(E._name());
                        impl.setMinor(E.minor());
			            impl.log();
                }
		        catch(...) {
                    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSRTActiveSurfaceBossCore::oneWayAction()");
                    impl.log();
                }
            }
            else {
                _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CSRTActiveSurfaceBossCore::oneWayAction()");
                impl.log();
            }
        }
    }
    else {
	printf ("onewayaction SINGLE\n");
        if (!CORBA::is_nil(usd[circle][actuator])) { // SINGLE ACTUATOR
            try {
                switch (onewayAction) {
                    case ActiveSurface::AS_STOP:
                        usd[circle][actuator]->stop();
                        break;
                    case ActiveSurface::AS_SETUP:
                        usd[circle][actuator]->setup();
                        break;
                     case ActiveSurface::AS_STOW:
                        usd[circle][actuator]->stow();
                        break;
                     case ActiveSurface::AS_REFPOS:
                        usd[circle][actuator]->refPos();
                        break;
                     case ActiveSurface::AS_UP:
                        usd[circle][actuator]->up();
                        break;
                     case ActiveSurface::AS_DOWN:
                        usd[circle][actuator]->down();
                        break;
                     case ActiveSurface::AS_BOTTOM:
                        usd[circle][actuator]->bottom();
                        break;
                     case ActiveSurface::AS_TOP:
                        usd[circle][actuator]->top();
                        break;
                     case ActiveSurface::AS_UPDATE:
                        break;
                     case ActiveSurface::AS_CORRECTION:
                        usd[circle][actuator]->correction(correction*MM2STEP);
                        break;
                     case ActiveSurface::AS_MOVE:
                        usd[circle][actuator]->move(incr*MM2STEP);
                        break;
                     case ActiveSurface::AS_PROFILE:
                        break;
                }
            }
            catch (ComponentErrors::ComponentErrorsEx& E) {
		        _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::oneWayAction()");
                impl.setComponentName((const char*)usd[circle][actuator]->name());
			    impl.setOperationName("oneWayAction()");
			    throw impl;
			}
            catch(CORBA::SystemException &E) {
                _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::oneWayAction()");
                impl.setName(E._name());
                impl.setMinor(E.minor());
                throw impl;
            }
            catch(...) {
                _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CSRTActiveSurfaceBossCore::oneWayAction()");
            }
        }
        else {
            _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CSRTActiveSurfaceBossCore::oneWayAction()");
        }
    }
}

void CSRTActiveSurfaceBossCore::watchingActiveSurfaceStatus() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
    ACS::ROpattern_var status_var;
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
        _THROW_EXCPT(ComponentErrors::CORBAProblemExImpl,"CSRTActiveSurfaceBossCore::watchingActiveSurfaceStatus()");
    }
	if (propertyEx == true) {
        //printf("property error\n");
        _THROW_EXCPT(ComponentErrors::CouldntGetAttributeExImpl,"CSRTActiveSurfaceBossCore::watchingActiveSurfaceStatus()");
    }
	if (notActivEx == true ) {
        //printf("not active\n");
	    _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CSRTActiveSurfaceBossCore::watchingActiveSurfaceStatus()");
    }
    //printf("NO error\n");
}

void CSRTActiveSurfaceBossCore::sectorActiveSurface(int sector) throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	if (sector == -1) {
		ACS_SHORT_LOG ((LM_INFO, "You have to set a sector first!"));
		exit(-1);
	}

	printf("sector%d start\n", sector+1);
	char serial_usd[23];
	char graf[5], mecc[4];
	std::stringstream value;
	value << CDBPATH;
	value << "alma/AS/tab_convUSD_S";
	value << sector+1;
	value << ".txt";

	ifstream usdTable(value.str().c_str());
	if (!usdTable) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value.str().c_str()));
		exit(-1);
	}

	// Get reference to usd components
	while (usdTable >> lanIndexes[sector] >> circleIndexes[sector] >> usdCircleIndexes[sector] >> serial_usd >> graf >> mecc)
	{
		usd[circleIndexes[sector]][usdCircleIndexes[sector]] = ActiveSurface::USD::_nil();

		try
		{
			printf("S%d: circleIndexS%d = %d, usdCircleIndexS%d = %d\n", sector+1, sector+1, circleIndexes[sector], sector+1, usdCircleIndexes[sector]);
			usd[circleIndexes[sector]][usdCircleIndexes[sector]] = m_services->getComponent<ActiveSurface::USD>(serial_usd);
			lanradius[circleIndexes[sector]][lanIndexes[sector]] = usd[circleIndexes[sector]][usdCircleIndexes[sector]];
			usdCounters[sector]++;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex)
		{
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::sectorActiveSurface()");
			Impl.setComponentName(serial_usd);
			Impl.log(LM_DEBUG);
		}
	}

	printf("sector%d done\n", sector+1);
	m_sector[sector] = true;
}

void CSRTActiveSurfaceBossCore::workingActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
	if (AutoUpdate) {
		TIMEVALUE now;
		double azimuth=0.0;
		double elevation=0.0;

		IRA::CIRATools::getTime(now);

		if (!CORBA::is_nil(m_antennaBoss)) {
			try {
				m_antennaBoss->getRawCoordinates(now.value().value, azimuth, elevation);
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::workingActiveSurface()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				m_status=Management::MNG_WARNING;
				//asPark();
				throw impl;
			}
			azimuth = 0.0;
			try {
				onewayAction(ActiveSurface::AS_UPDATE, 0, 0, 0, elevation*DR2D, 0, 0, m_profile);
			}
			catch (ComponentErrors::ComponentErrorsExImpl& ex) {
				ex.log(LM_DEBUG);
				throw ex.getComponentErrorsEx();
			}
		}
	}
}

void CSRTActiveSurfaceBossCore::setProfile(const ActiveSurface::TASProfile& newProfile) throw (ComponentErrors::ComponentErrorsExImpl)
{
	int s, i, l;

	bool all_sectors = true;
	for(unsigned int i = 0; i < SECTORS; i++)
		if(!m_sector[i]) all_sectors = false;

	if (all_sectors)
	{
		ifstream usdCorrections (USDTABLECORRECTIONS);
		if (!usdCorrections) {
			ACS_SHORT_LOG ((LM_INFO, "File %s not found", USDTABLECORRECTIONS));
			exit(-1);
		}
		actuatorsCorrections.length(NPOSITIONS);
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				//printf ("Corrections = ");
				for (s = 0; s < NPOSITIONS; s++) {
					usdCorrections >> actuatorsCorrections[s];
					//printf ("%f ", actuatorsCorrections[s]);
				}
				//printf("\n");
				if (!CORBA::is_nil(usd[i][l])) {
					//printf("i = %d, l = %d\n", i, l);
					usd[i][l]->posTable(actuatorsCorrections, NPOSITIONS, DELTAEL, THRESHOLDPOS);
				}
			}
		}

		usdCounter = 0;
		for(unsigned int i = 0; i < SECTORS; i++) {
			m_sector[i] = false;
			usdCounter += usdCounters[i];
		}

		m_ASup=true;
		m_status=Management::MNG_OK;

		if (usdCounter < (int)lastUSD*WARNINGUSDPERCENT) {
			m_status=Management::MNG_WARNING;
		}
		if (usdCounter < (int)lastUSD*ERRORUSDPERCENT) {
			m_status=Management::MNG_FAILURE;
			m_ASup=false;
		}
	}

	if (m_ASup == true) {
		asOff();
		CIRATools::Wait(1000000);
		_SET_CDB_CORE(profile, newProfile,"SRTActiveSurfaceBossCore::setProfile")
		m_profile = newProfile;
		try {
			onewayAction(ActiveSurface::AS_PROFILE, 0, 0, 0, 0, 0, 0, newProfile);
		}
		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
		}
		m_profileSetted = true;
		asOn();
	}
}

void CSRTActiveSurfaceBossCore::asSetup() throw (ComponentErrors::ComponentErrorsEx)
{
    try {
            onewayAction(ActiveSurface::AS_SETUP, 0, 0, 0, 0, 0, 0, m_profile);
        }
        catch (ComponentErrors::ComponentErrorsExImpl& ex) {
            ex.log(LM_DEBUG);
            throw ex.getComponentErrorsEx();
        }
}

void CSRTActiveSurfaceBossCore::asOn()
{
	if (m_profileSetted == true ) {
		if ((m_profile != ActiveSurface::AS_PARABOLIC_FIXED) && (m_profile != ActiveSurface::AS_SHAPED_FIXED) && (m_profile != ActiveSurface::AS_PARK)) {
			enableAutoUpdate();
			m_tracking = true;
		}
		else {
		    asOff();
		    CIRATools::Wait(1000000);
            //disableAutoUpdate();
			m_tracking = false;
			try {
				onewayAction(ActiveSurface::AS_UPDATE, 0, 0, 0, 45.0, 0, 0, m_profile);
			}
			catch (ComponentErrors::ComponentErrorsExImpl& ex) {
				ex.log(LM_DEBUG);
				throw ex.getComponentErrorsEx();
			}
		}
	}
	else {
		printf("you must set the profile first\n");
	}
}

void CSRTActiveSurfaceBossCore::asPark() throw (ComponentErrors::ComponentErrorsEx)
{
	if (m_profileSetted == true ) {
		asOff();
		m_tracking = false;
        CIRATools::Wait(1000000);
		_SET_CDB_CORE(profile, ActiveSurface::AS_PARK,"SRTActiveSurfaceBossCore::asPark()")
		m_profile = ActiveSurface::AS_PARK;
		try {
			onewayAction(ActiveSurface::AS_REFPOS, 0, 0, 0, 0.0, 0, 0, m_profile);
		}
		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
		}
	}
	else {
		printf("you must set the profile first\n");
	}
}

void CSRTActiveSurfaceBossCore::asOff() throw (ComponentErrors::ComponentErrorsEx)
{
	if (m_profileSetted == true ) {
		disableAutoUpdate();
		try {
			onewayAction(ActiveSurface::AS_STOP, 0, 0, 0, 0, 0, 0, m_profile);
		}
		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
		}
	}
	else {
		printf("you must set the profile first\n");
	}
}

void CSRTActiveSurfaceBossCore::setActuator(int circle, int actuator, long int& actPos, long int& cmdPos, long int& Fmin, long int& Fmax, long int& acc,long int& delay/*CORBA::Long_out actPos, CORBA::Long_out cmdPos, CORBA::Long_out Fmin, CORBA::Long_out Fmax, CORBA::Long_out acc, CORBA::Long_out delay*/) throw (ComponentErrors::PropertyErrorExImpl, ComponentErrors::ComponentNotActiveExImpl)
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
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd actual position");
            throw impl;
        }
        cmdPos_var = usd[circle][actuator]->cmdPos ();
        if (cmdPos_var.ptr() != ACS::RWlong::_nil()) {
            cmdPos = cmdPos_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd commanded position");
            throw impl;
        }
        Fmin_var = usd[circle][actuator]->Fmin ();
        if (Fmin_var.ptr() != ACS::RWlong::_nil()) {
            Fmin = Fmin_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd Fmin");
            throw impl;
        }
        Fmax_var = usd[circle][actuator]->Fmax ();
        if (Fmax_var.ptr() != ACS::RWlong::_nil()) {
            Fmax = Fmax_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd Fmax");
            throw impl;
        }
        acc_var = usd[circle][actuator]->acc ();
        if (acc_var.ptr() != ACS::RWlong::_nil()) {
            acc = acc_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd acceleration");
            throw impl;
        }
        delay_var = usd[circle][actuator]->delay ();
        if (delay_var.ptr() != ACS::RWlong::_nil()) {
            delay = delay_var->get_sync (completion.out ());
        }
        else {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd delay");
            throw impl;
        }
    }
    else {
        _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CSRTActiveSurfaceBossCore::setActuator()");
    }
}

void CSRTActiveSurfaceBossCore::usdStatus4GUIClient(int circle, int actuator, CORBA::Long_out status) throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
    	if (!CORBA::is_nil(usd[circle][actuator])) {
        	try {
                usd[circle][actuator]->getStatus(status);
        	}
        	catch (CORBA::SystemException& ex) {
            		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::usdStatus4GUIClient()");
            		impl.setName(ex._name());
			impl.setMinor(ex.minor());
            		throw impl;
		}
        /*CompletionImpl local(completion);
        if (!local.isErrorFree()) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,local,"CSRTActiveSurfaceBossCore::usdStatus4GUIClient()");
			impl.setComponentName((const char*)usd[circle][actuator]->name());
			impl.setAttributeName("usd status");
			throw impl;
		}*/
    	}
    	else {
        	_THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CSRTActiveSurfaceBossCore::usdStatus4GUIClient()");
    	}
}

void CSRTActiveSurfaceBossCore::recoverUSD(int circleIndex, int usdCircleIndex) throw (ComponentErrors::CouldntGetComponentExImpl)
{
    char serial_usd[23];

    setserial (circleIndex, usdCircleIndex, lanIndex, serial_usd);

    usd[circleIndex][usdCircleIndex] = ActiveSurface::USD::_nil();
    try {
        usd[circleIndex][usdCircleIndex] = m_services->getComponent<ActiveSurface::USD>(serial_usd);
        lanradius[circleIndex][lanIndex] = usd[circleIndex][usdCircleIndex];
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::recoverUSD()");
        Impl.setComponentName(serial_usd);
        Impl.log(LM_DEBUG);
    }
}

void CSRTActiveSurfaceBossCore::setradius(int radius, int &actuatorsradius, int &jumpradius)
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

void CSRTActiveSurfaceBossCore::setserial (int circle, int actuator, int &lan, char* serial_usd)
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

void CSRTActiveSurfaceBossCore::enableAutoUpdate()
{
	if (!AutoUpdate) {
		AutoUpdate=true;
		ACS_LOG(LM_FULL_INFO,"CSRTActiveSurfaceBossCore::enableAutoUpdate()",(LM_NOTICE,"SRTActiveSurfaceBoss::AUTOMATIC_UPDATE_ENABLED"));
	}
}

void CSRTActiveSurfaceBossCore::disableAutoUpdate()
{
	if (AutoUpdate) {
		AutoUpdate=false;
		ACS_LOG(LM_FULL_INFO,"CSRTActiveSurfaceBossCore::disableAutoUpdate()",(LM_NOTICE,"SRTActiveSurfaceBoss::AUTOMATIC_UPDATE_DISABLED"));
	}
}

void CSRTActiveSurfaceBossCore::checkASerrors(const char* str, int circle, int actuator, ASErrors::ASErrorsEx Ex)
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

void CSRTActiveSurfaceBossCore::checkAScompletionerrors (char *str, int circle, int actuator, CompletionImpl comp)
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
