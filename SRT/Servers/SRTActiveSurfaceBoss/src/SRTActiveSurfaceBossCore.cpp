#include "SRTActiveSurfaceBossCore.h"
#include <Definitions.h>
#include <cstdio>

int actuatorsInCircle[] = {0,24,24,48,48,48,48,96,96,96,96,96,96,96,96,96,8,4};

CSRTActiveSurfaceBossCore::CSRTActiveSurfaceBossCore(ContainerServices *service, acscomponent::ACSComponentImpl *me) :
    m_services(service),
    m_thisIsMe(me)
{
    m_error_strings[ASErrors::NoError           ] = "NoError";
    m_error_strings[ASErrors::USDCalibrated     ] = "USD calibrated";
    m_error_strings[ASErrors::LibrarySocketError] = "LibrarySocketError";
    m_error_strings[ASErrors::SocketReconn      ] = "SocketReconn";
    m_error_strings[ASErrors::SocketFail        ] = "SocketFail";
    m_error_strings[ASErrors::SocketTOut        ] = "SocketTOut";
    m_error_strings[ASErrors::SocketNotRdy      ] = "SocketNotRdy";
    m_error_strings[ASErrors::MemoryAllocation  ] = "MemoryAllocation";
    m_error_strings[ASErrors::LANConnectionError] = "LANConnectionError";
    m_error_strings[ASErrors::LAN_Unavailable   ] = "LANUnavailable";
    m_error_strings[ASErrors::sendCmdErr        ] = "sendCmdErr";
    m_error_strings[ASErrors::Incomplete        ] = "Incomplete";
    m_error_strings[ASErrors::InvalidResponse   ] = "InvalidResponse";
    m_error_strings[ASErrors::Nak               ] = "Nak";
    m_error_strings[ASErrors::CDBAccessError    ] = "CDBAccessError";
    m_error_strings[ASErrors::USDConnectionError] = "USDConnectionError";
    m_error_strings[ASErrors::USDTimeout        ] = "USDTimeout";
    m_error_strings[ASErrors::USDUnavailable    ] = "USDUnavailable";
    m_error_strings[ASErrors::USDError          ] = "USDError";
    m_error_strings[ASErrors::DevIOError        ] = "DevIOError";
    m_error_strings[ASErrors::corbaError        ] = "corbaError";
    m_error_strings[ASErrors::USDStillRunning   ] = "USDStillRunning";
    m_error_strings[ASErrors::USDunCalibrated   ] = "USDunCalibrated";
    m_error_strings[ASErrors::CannotGetUSD      ] = "CannotGetUSD";
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
    AutoUpdate = false;
    actuatorcounter = circlecounter = totacts = 1;
    for(int i = 0; i < SECTORS; i++)
    {
        m_sector.push_back(false);
        usdCounters.push_back(0);
    }
    m_profileSetted = false;
    m_ASup = false;
}

void CSRTActiveSurfaceBossCore::execute() throw (ComponentErrors::CouldntGetComponentExImpl)
{
/*
    char serial_usd[23];
    char graf[5], mecc[4];
    char * value;
    char * value2;

    //s_usdTable = getenv ("ACS_CDB");
    //strcat(s_usdTable,USDTABLE);
    value2 = USDTABLE;
    //ifstream usdTable(s_usdTable);
    ifstream usdTable(value2);
    if(!usdTable)
    {
        //ACS_SHORT_LOG ((LM_INFO, "File %s not found", s_usdTable));
        ACS_SHORT_LOG ((LM_INFO, "File %s not found", value2));
        exit(-1);
    }

    value = USDTABLECORRECTIONS;
    ifstream usdCorrections (value);
    if(!usdCorrections)
    {
        ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
        exit(-1);
    }
    actuatorsCorrections.length(NPOSITIONS);

    // get reference to lan components
    for (int s = 1; s <= 8; s++)
    {
        for (int l = 1; l <= 12; l++)
        {
            lanCobName.Format("AS/SECTOR%02d/LAN%02d",s, l);
            ACS_SHORT_LOG((LM_INFO, "Getting component: %s", (const char*)lanCobName));
            printf("lan = %s\n", (const char*)lanCobName);
            lan[s][l] = ActiveSurface::lan::_nil();
            try
            {
                lan[s][l] = m_services->getComponent<ActiveSurface::lan>((const char *)lanCobName);
            }
            catch (maciErrType::CannotGetComponentExImpl& ex)
            {
                _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::execute()");
                Impl.setComponentName((const char*)lanCobName);
                Impl.log(LM_DEBUG);
            }
            CIRATools::Wait(LOOPTIME);
        }
    }
    ACS_LOG(LM_FULL_INFO, "CSRTActiveSurfaceBossCore::execute()", (LM_INFO,"CSRTActiveSurfaceBossCore::LAN_LOCATED"));

    // Get reference to usd components
    for (int i = firstUSD; i <= lastUSD; i++)
    {
        usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc;
        usd[circleIndex][usdCircleIndex] = ActiveSurface::USD::_nil();
        try
        {
            //usd[circleIndex][usdCircleIndex] = m_services->getComponent<ActiveSurface::USD>(serial_usd);
            //lanradius[circleIndex][lanIndex] = usd[circleIndex][usdCircleIndex];
            usdCounter++;
        }
        catch (maciErrType::CannotGetComponentExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::execute()");
            Impl.setComponentName(serial_usd);
            Impl.log(LM_DEBUG);
        }
        //CIRATools::Wait(LOOPTIME);
    }

    for (int i = 1; i <= CIRCLES; i++)
    {
        for (int l = 1; l <= actuatorsInCircle[i]; l++)
        {
            //printf ("Corrections = ");
            for (int s = 0; s < NPOSITIONS; s++)
            {
                usdCorrections >> actuatorsCorrections[s];
                //printf ("%f ", actuatorsCorrections[s]);
            }
            //printf("\n");
            if(!CORBA::is_nil(usd[i][l]))
            {
                //usd[i][l]->posTable(actuatorsCorrections, NPOSITIONS, DELTAEL, THRESHOLDPOS);
            }
        }
    }
    ACS_LOG(LM_FULL_INFO, "CSRTActiveSurfaceBossCore::execute()", (LM_INFO,"CSRTActiveSurfaceBossCore::USD_LOCATED"));

    if(usdCounter < (int)lastUSD*WARNINGUSDPERCENT)
    {
        m_status=Management::MNG_WARNING;
    }
    if(usdCounter < (int)lastUSD*ERRORUSDPERCENT)
    {
        m_status=Management::MNG_FAILURE;
    }
*/

    m_antennaBoss = Antenna::AntennaBoss::_nil();
    try
    {
        m_antennaBoss = m_services->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
    }
    catch (maciErrType::CannotGetComponentExImpl& ex)
    {
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
    int lanIndex, circleIndex, usdCircleIndex;

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

void CSRTActiveSurfaceBossCore::calibrate (int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl)
{
    double cammaPos, cammaLen;
    bool calibrated;
    int i, l;

    if(circle == 0 && actuator == 0 && radius == 0) // ALL
    {
        printf("top.....\n");
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(!CORBA::is_nil(usd[i][l]))
                {
                    try
                    {
                        usd[i][l]->top();
                        printf ("actuator n.%d_%d top\n", i, l);
                    }
                    catch (ASErrors::ASErrorsEx & Ex)
                    {
                        checkASerrors ("actuator", i, l, Ex);
                    }
                }
            }
        }
        ACE_OS::sleep (90);

        printf("move to upper mechanical position.....\n");
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(!CORBA::is_nil(usd[i][l]))
                {
                    try
                    {
                        usd[i][l]->move(1400);
                        printf ("actuator n.%d_%d move\n", i, l);
                    }
                    catch (ASErrors::ASErrorsEx & Ex)
                    {
                        checkASerrors ("actuator", i, l, Ex);
                    }
                }
            }
        }
        ACE_OS::sleep (5);

        /*printf("stop.....\n");
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(!CORBA::is_nil(usd[i][l]))
                {
                    try
                    {
                        usd[i][l]->stop();
                        printf ("actuator n.%d_%d stop\n", i, l);
                    }
                    catch (ASErrors::ASErrorsEx & Ex)
                    {
                        checkASerrors ("actuator", i, l, Ex);
                    }
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf("calibration.....\n");
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(!CORBA::is_nil(usd[i][l]))
                {
                    try
                    {
                        usd[i][l]->calibrate ();
                        printf ("actuator n.%d_%d calibrate\n", i, l);
                    }
                    catch (ASErrors::ASErrorsEx & Ex)
                    {
                        checkASerrors ("actuator", i, l, Ex);
                    }
                }
            }
        }
        ACE_OS::sleep (15);

        /*printf("stop.....\n");
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(!CORBA::is_nil(usd[i][l]))
                {
                    try
                    {
                        usd[i][l]->stop();
                        printf ("actuator n.%d_%d stop\n", i, l);
                    }
                    catch (ASErrors::ASErrorsEx & Ex)
                    {
                        checkASerrors ("actuator", i, l, Ex);
                    }
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf ("calibration verification.....\n");
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(!CORBA::is_nil(usd[i][l]))
                {
                    try
                    {
                        usd[i][l]->calVer ();
                        printf ("actuator n.%d_%d calVer\n", i, l);
                    }
                    catch (ASErrors::ASErrorsEx & Ex)
                    {
                        checkASerrors ("actuator", i, l, Ex);
                    }
                }
            }
        }
        ACE_OS::sleep (150);

        /*printf("stop.....\n");
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(!CORBA::is_nil(usd[i][l]))
                {
                    try
                    {
                        usd[i][l]->stop();
                        printf ("actuator n.%d_%d stop\n", i, l);
                    }
                    catch (ASErrors::ASErrorsEx & Ex)
                    {
                        checkASerrors ("actuator", i, l, Ex);
                    }
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf ("write calibration results.....\n");
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(!CORBA::is_nil(usd[i][l]))
                {
                    try
                    {
                        CIRATools::Wait(LOOPTIME);
                        usd[i][l]->writeCalibration(cammaLen,cammaPos,calibrated);
                        printf ("actuator %02d_%02d %04.1f %05.1f %d\n", i, l, cammaLen, cammaPos, calibrated);
                    }
                    catch (ASErrors::ASErrorsEx & Ex)
                    {
                        checkASerrors ("actuator", i, l, Ex);
                    }
                }
            }
        }
        ACE_OS::sleep (1);
    }
    else if(circle != 0 && actuator == 0 && radius == 0) // CIRCLE
    {
        printf("top.....\n");
        for (l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(!CORBA::is_nil(usd[circle][l]))
            {
                try
                {
                    usd[circle][l]->top();
                    printf ("actuator n.%d_%d top\n", circle, l);
                }
                catch (ASErrors::ASErrorsEx & Ex)
                {
                    checkASerrors ("actuator", circle, l, Ex);
                }
            }
        }
        ACE_OS::sleep (90);

        printf("move to upper mechanical position.....\n");
        for (l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(!CORBA::is_nil(usd[circle][l]))
            {
                try
                {
                    usd[circle][l]->move(1400);
                    printf ("actuator n.%d_%d move\n", circle, l);
                }
                catch (ASErrors::ASErrorsEx & Ex)
                {
                    checkASerrors ("actuator", circle, l, Ex);
                }
            }
        }
        ACE_OS::sleep (5);

        /*printf("stop.....\n");
        for (l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(!CORBA::is_nil(usd[circle][l]))
            {
                try
                {
                    usd[circle][l]->stop();
                    printf ("actuator n.%d_%d stop\n", circle, l);
                }
                catch (ASErrors::ASErrorsEx & Ex)
                {
                    checkASerrors ("actuator", circle, l, Ex);
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf("calibration.....\n");
        for (l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(!CORBA::is_nil(usd[circle][l]))
            {
                try
                {
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
        for (l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(!CORBA::is_nil(usd[circle][l]))
            {
                try
                {
                    usd[circle][l]->stop();
                    printf ("actuator n.%d_%d stop\n", circle, l);
                }
                catch (ASErrors::ASErrorsEx & Ex)
                {
                    checkASerrors ("actuator", circle, l, Ex);
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf ("calibration verification.....\n");
        for (l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(!CORBA::is_nil(usd[circle][l]))
            {
                try
                {
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
        for (l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(!CORBA::is_nil(usd[circle][l]))
            {
                try
                {
                    usd[circle][l]->stop();
                    printf ("actuator n.%d_%d stop\n", circle, l);
                }
                catch (ASErrors::ASErrorsEx & Ex)
                {
                    checkASerrors ("actuator", circle, l, Ex);
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf ("write calibration results.....\n");
        for (l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(!CORBA::is_nil(usd[circle][l]))
            {
                try
                {
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
    else if(circle == 0 && actuator == 0 && radius != 0) // RADIUS
    {
        int actuatorsradius;
        int jumpradius;
        setradius(radius, actuatorsradius, jumpradius);
        int l;

        printf("top.....\n");
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(!CORBA::is_nil(lanradius[l+jumpradius][radius]))
            {
                try
                {
                    lanradius[l+jumpradius][radius]->top();
                    printf ("actuator n.%d_%d top\n", l+jumpradius, radius);
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
        }
        ACE_OS::sleep (90);

        printf("move to upper mechanical position.....\n");
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(!CORBA::is_nil(lanradius[l+jumpradius][radius]))
            {
                try
                {
                    lanradius[l+jumpradius][radius]->move(1400);
                    printf ("actuator n.%d_%d move\n", l+jumpradius, radius);
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
        }
        ACE_OS::sleep (5);

        /*printf("stop.....\n");
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(!CORBA::is_nil(lanradius[l+jumpradius][radius]))
            {
                try
                {
                    lanradius[l+jumpradius][radius]->stop();
                    printf ("actuator n.%d_%d stop\n", l+jumpradius, radius);
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf("calibration.....\n");
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(!CORBA::is_nil(lanradius[l+jumpradius][radius]))
            {
                try
                {
                    lanradius[l+jumpradius][radius]->calibrate();
                    printf ("actuator n.%d_%d calibrate\n", l+jumpradius, radius);
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
        }
        ACE_OS::sleep (15);

        /*printf("stop.....\n");
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(!CORBA::is_nil(lanradius[l+jumpradius][radius]))
            {
                try
                {
                    lanradius[l+jumpradius][radius]->stop();
                    printf ("actuator n.%d_%d stop\n", l+jumpradius, radius);
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf ("calibration verification.....\n");
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(!CORBA::is_nil(lanradius[l+jumpradius][radius]))
            {
                try
                {
                    lanradius[l+jumpradius][radius]->calVer();
                    printf ("actuator n.%d_%d calVer\n", l+jumpradius, radius);
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
        }
        ACE_OS::sleep (150);

        /*printf("stop.....\n");
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(!CORBA::is_nil(lanradius[l+jumpradius][radius]))
            {
                try
                {
                    lanradius[l+jumpradius][radius]->stop();
                    printf ("actuator n.%d_%d stop\n", l+jumpradius, radius);
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
        }
        ACE_OS::sleep (1);*/

        printf ("write calibration results.....\n");
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(!CORBA::is_nil(lanradius[l+jumpradius][radius]))
            {
                try
                {
                    CIRATools::Wait(LOOPTIME);
                    lanradius[l+jumpradius][radius]->writeCalibration(cammaLen,cammaPos,calibrated);
                    printf ("actuator %02d_%02d %04.1f %05.1f %d\n", l+jumpradius, radius, cammaLen, cammaPos, calibrated);
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
        }
        ACE_OS::sleep (1);
    }
    else
    {
        if(!CORBA::is_nil(usd[circle][actuator])) // SINGLE ACTUATOR
        {
            printf("top.....\n");
            try
            {
                usd[circle][actuator]->top();
                printf ("actuator n.%d_%d top\n", circle, actuator);
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
            ACE_OS::sleep (90);
            printf("move to upper mechanical position.....\n");
            try
            {
                usd[circle][actuator]->move(1400);
                printf ("actuator n.%d_%d move\n", circle, actuator);
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
            ACE_OS::sleep (5);
            /*printf("stop.....\n");
            try
            {
                usd[circle][actuator]->stop();
                printf ("actuator n.%d_%d stop\n", circle, actuator);
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
            ACE_OS::sleep (1);*/
            printf("calibration.....\n");
            try
            {
                usd[circle][actuator]->calibrate();
                printf ("actuator n.%d_%d calibrate\n", circle, actuator);
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
            ACE_OS::sleep (15);
            /*printf("stop.....\n");
            try
            {
                usd[circle][actuator]->stop();
                printf ("actuator n.%d_%d stop\n", circle, actuator);
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
            ACE_OS::sleep (1);*/
            printf ("calibration verification.....\n");
            try
            {
                usd[circle][actuator]->calVer();
                printf ("actuator n.%d_%d calVer\n", circle, actuator);
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
            ACE_OS::sleep (150);
            printf ("write calibration results.....\n");
            try
            {
                usd[circle][actuator]->writeCalibration(cammaLen,cammaPos,calibrated);
                printf ("actuator %02d_%02d %04.1f %05.1f %d\n", circle, actuator, cammaLen, cammaPos, calibrated);
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
            ACE_OS::sleep (1);
        }
    }

/*
    if(circle == 0 && actuator == 0 && radius == 0) // ALL
    {
        int i, l;
        for (i = 1; i <= CIRCLES; i++)
        {
            for (l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(usdinitialized[i][l] == 0)
                {
                    try
                    {
                        usd[i][l]->top();
                        actPos_var = usd[i][l]->actPos ();
                        while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000)
                        {
                            printf("actuator n.%d_%d waiting to reach top position...\n", i, l);
                        }
                        CompletionImpl comp = usd[i][l]->calibrate();
                        if(comp.isErrorFree() == false)
                        {
                            checkASerrors("usd", i, l, comp);
                        }
                    }
                    catch (ASErrors::ASErrorsEx &Ex)
                    {
                        checkASerrors("actuator", i, l, Ex);
                    }
                }
                else
                {
                    printf ("actuator n.%d_%d not initialized!\n", i, l);
                }
            }
        }
    }
    else if(circle != 0 && actuator == 0 && radius == 0) // CIRCLE
    {
        for (int l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(usdinitialized[circle][l] == 0)
            {
                try
                {
                    usd[circle][l]->top();
                    actPos_var = usd[circle][l]->actPos ();
                    while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000)
                    {
                        printf("actuator n.%d_%d waiting to reach top position...\n", circle, l);
                    }
                    CompletionImpl comp = usd[circle][l]->calibrate();
                    if(comp.isErrorFree() == false)
                    {
                        checkASerrors("usd", circle, l, comp);
                    }
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", circle, l, Ex);
                }
            }
            else
            {
                printf ("actuator n.%d_%d not initialized!\n", circle, l);
            }
        }
    }
    else if(circle == 0 && actuator == 0 && radius != 0) // RADIUS
    {
        int actuatorsradius;
        int jumpradius;
        setradius(radius, actuatorsradius, jumpradius);
        int l;
        for (l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(lanradiusinitialized[l+jumpradius][radius] == 0)
            {
                try
                {
                    lanradius[l+jumpradius][radius]->top();
                    actPos_var = lanradius[l+jumpradius][radius]->actPos ();
                    while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000)
                    {
                        printf("actuator n.%d_%d waiting to reach top position...\n", l+jumpradius, radius);
                    }
                    CompletionImpl comp = lanradius[l+jumpradius][radius]->calibrate();
                    if(comp.isErrorFree() == false)
                    {
                        checkASerrors("lanradius", l+jumpradius, radius, comp);
                    }
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
            else
            {
                printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
            }
        }
    }
    else
    {
        if(!CORBA::is_nil(usd[circle][actuator])) // SINGLE ACTUATOR
        {
            try
            {
                usd[circle][actuator]->top();
                actPos_var = usd[circle][actuator]->actPos ();
                while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000)
                {
                    ACE_OS::sleep (1);
                    printf("actuator n.%d_%d waiting to reach top position...\n", circle, actuator);
                }
                CompletionImpl comp = usd[circle][actuator]->calibrate();
                if(comp.isErrorFree() == false)
                {
                    checkASerrors("usd", circle, actuator, comp);
                }
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
        }
        else
        {
            printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
        }
    }
*/
}

void CSRTActiveSurfaceBossCore::calVer (int circle, int actuator, int radius) throw (ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntCallOperationExImpl, ComponentErrors::CORBAProblemExImpl)
{
/*
    if(circle == 0 && actuator == 0 && radius == 0) // ALL
    {
        for (int i = 1; i <= CIRCLES; i++)
        {
            for (int l = 1; l <= actuatorsInCircle[i]; l++)
            {
                if(usdinitialized[i][l] == 0)
                {
                    try
                    {
                        printf("actuator n.%d_%d verification of calibration\n", i, l);
                        CompletionImpl comp = usd[i][l]->calVer();
                        if(comp.isErrorFree() == false)
                        {
                            checkASerrors("usd", i, l, comp);
                        }
                    }
                    catch (ASErrors::ASErrorsEx &Ex)
                    {
                        checkASerrors("actuator", i, l, Ex);
                    }
                }
                else
                {
                    printf ("actuator n.%d_%d not initialized!\n", i, l);
                }
            }
        }
    }
    else if(circle != 0 && actuator == 0 && radius == 0) // CIRCLE
    {
        for (int l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            if(usdinitialized[circle][l] == 0)
            {
                try
                {
                    printf("actuator n.%d_%d verification of calibration\n", circle, l);
                    CompletionImpl comp = usd[circle][l]->calVer();
                    if(comp.isErrorFree() == false)
                    {
                        checkASerrors("usd", circle, l, comp);
                    }
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", circle, l, Ex);
                }
            }
            else
            {
                printf ("actuator n.%d_%d not initialized!\n", circle, l);
            }
        }
    }
    else if(circle == 0 && actuator == 0 && radius != 0) // RADIUS
    {
        int actuatorsradius;
        int jumpradius;
        setradius(radius, actuatorsradius, jumpradius);
        for (int l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }
            if(lanradiusinitialized[l+jumpradius][radius] == 0)
            {
                try
                {
                    printf("actuator n.%d_%d verification of calibration\n", l+jumpradius, radius);
                    CompletionImpl comp = lanradius[l+jumpradius][radius]->calVer();
                    if(!comp.isErrorFree())
                    {
                        checkASerrors("lanradius", l+jumpradius, radius, comp);
                    }
                }
                catch (ASErrors::ASErrorsEx &Ex)
                {
                    checkASerrors("actuator", l+jumpradius, radius, Ex);
                }
            }
            else
            {
                printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
            }
        }
    }
    else
    {
        if(!CORBA::is_nil(usd[circle][actuator])) // SINGLE ACTUATOR
        {
            try
            {
                printf("actuator n.%d_%d verification of calibration\n", circle, actuator);
                CompletionImpl comp = usd[circle][actuator]->calVer();
                if(comp.isErrorFree() == false)
                {
                    checkASerrors("usd", circle, actuator, comp);
                }
                else
                {
                    printf("usd %d %d calibration verification OK\n", circle, actuator);
                }
            }
            catch (ASErrors::ASErrorsEx &Ex)
            {
                checkASerrors("actuator", circle, actuator, Ex);
            }
        }
        else
        {
            printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
        }
    }
*/
}

void CSRTActiveSurfaceBossCore::singleUSDonewayAction(ActiveSurface::TASOneWayAction action, ActiveSurface::USD_var usd, double elevation, double correction, long incr, ActiveSurface::TASProfile profile)
{
    if(!CORBA::is_nil(usd))
    {
        std::string operationName = "onewayAction(";
        try
        {
            switch (action)
            {
                case ActiveSurface::AS_STOP:
                    operationName += "AS_STOP)";
                    usd->stop();
                    break;
                case ActiveSurface::AS_SETUP:
                    operationName += "AS_SETUP)";
                    usd->setup();
                    break;
                case ActiveSurface::AS_STOW:
                    operationName += "AS_STOW)";
                    usd->stow();
                    break;
                case ActiveSurface::AS_REFPOS:
                    operationName += "AS_REFPOS)";
                    usd->refPos();
                    break;
                case ActiveSurface::AS_UP:
                    operationName += "AS_UP)";
                    usd->up();
                    break;
                case ActiveSurface::AS_DOWN:
                    operationName += "AS_DOWN)";
                    usd->down();
                    break;
                case ActiveSurface::AS_BOTTOM:
                    operationName += "AS_BOTTOM)";
                    usd->bottom();
                    break;
                case ActiveSurface::AS_TOP:
                    operationName += "AS_TOP)";
                    usd->top();
                    break;
                case ActiveSurface::AS_UPDATE:
                    operationName += "AS_UPDATE)";
                    usd->update(elevation);
                    break;
                case ActiveSurface::AS_CORRECTION:
                    operationName += "AS_CORRECTION)";
                    usd->correction(correction*MM2STEP);
                    break;
                case ActiveSurface::AS_MOVE:
                    operationName += "AS_MOVE)";
                    usd->move(incr*MM2STEP);
                    break;
                case ActiveSurface::AS_PROFILE:
                    operationName += "AS_PROFILE)";
                    usd->setProfile(profile);
                    break;
                case ActiveSurface::AS_RESET:
                    operationName += "AS_RESET)";
                    usd->reset();
                    CIRATools::Wait(LOOPTIME);
                    break;
            }
        }
        catch (ASErrors::ASErrorsEx& E)
        {
            _ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CSRTActiveSurfaceBossCore::singleUSDonewayAction()");
            impl.setComponentName((const char*)usd->name());
            impl.setOperationName(operationName.c_str());
            impl.log();
        }
        catch(CORBA::SystemException &E)
        {
            _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::singleUSDonewayAction()");
            impl.setName(E._name());
            impl.setMinor(E.minor());
            impl.log();
        }
        catch(...)
        {
            _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CSRTActiveSurfaceBossCore::singleUSDonewayAction()");
            impl.log();
        }
    }
    else
    {
        _EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CSRTActiveSurfaceBossCore::singleUSDonewayAction()");
        impl.log();
    }

}

void CSRTActiveSurfaceBossCore::onewayAction(ActiveSurface::TASOneWayAction action, int circle, int actuator, int radius, double elevation, double correction, long incr, ActiveSurface::TASProfile profile)
{
    if(action == ActiveSurface::AS_UPDATE && !m_profileSetted)
    {
        printf("you must set the profile first\n");
        return;
    }

    if(circle == 0 && actuator == 0 && radius == 0) // ALL
    {
        for (int i = 1; i <= CIRCLES; i++)
        {
            for (int l = 1; l <= actuatorsInCircle[i]; l++)
            {
                singleUSDonewayAction(action, usd[i][l], elevation, correction, incr, profile);
            }
        }
    }
    else if(circle != 0 && actuator == 0 && radius == 0 && action != ActiveSurface::AS_UPDATE && action != ActiveSurface::AS_PROFILE) // CIRCLE
    {
        std::cout << "onewayaction CIRCLE" << std::endl;
        for (int l = 1; l <= actuatorsInCircle[circle]; l++)
        {
            singleUSDonewayAction(action, usd[circle][l], elevation, correction, incr, profile);
        }
    }
    else if(circle == 0 && actuator == 0 && radius != 0 && action != ActiveSurface::AS_UPDATE && action != ActiveSurface::AS_PROFILE) // RADIUS
    {
        std::cout << "onewayaction RADIUS" << std::endl;
        int actuatorsradius;
        int jumpradius;
        setradius(radius, actuatorsradius, jumpradius);

        for (int l = 1; l <= actuatorsradius; l++)
        {
            if((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
            {
                jumpradius++;  // 17 circle
            }

            singleUSDonewayAction(action, lanradius[l+jumpradius][radius], elevation, correction, incr, profile);
        }
    }
    else if(action != ActiveSurface::AS_UPDATE && action != ActiveSurface::AS_PROFILE) // SINGLE
    {
        std::cout << "onewayaction SINGLE" << std::endl;
        singleUSDonewayAction(action, usd[circle][actuator], elevation, correction, incr, profile);
    }
}

void CSRTActiveSurfaceBossCore::watchingActiveSurfaceStatus() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
    ACS::ROpattern_var status_var;
    ACSErr::Completion_var completion;

    int i = circlecounter;
    int l = actuatorcounter;
    int totalactuators = totacts;

    if (totalactuators >= 1 && totalactuators <= 24)      // 1 circle
        i= 1;
    if (totalactuators >= 25 && totalactuators <= 48)     // 2 circle
        i= 2;
    if (totalactuators >= 49 && totalactuators <= 96)     // 3 circle
        i= 3;
    if (totalactuators >= 97 && totalactuators <= 144)    // 4 circle
        i= 4;
    if (totalactuators >= 145 && totalactuators <= 192)   // 5 circle
        i= 5;
    if (totalactuators >= 193 && totalactuators <= 240)   // 6 circle
        i= 6;
    if (totalactuators >= 241 && totalactuators <= 336)   // 7 circle
        i= 7;
    if (totalactuators >= 337 && totalactuators <= 432)   // 8 circle
        i= 8;
    if (totalactuators >= 433 && totalactuators <= 528)   // 9 circle
        i= 9;
    if (totalactuators >= 529 && totalactuators <= 624)   // 10 circle
        i= 10;
    if (totalactuators >= 625 && totalactuators <= 720)   // 11 circle
        i= 11;
    if (totalactuators >= 721 && totalactuators <= 816)   // 12 circle
        i= 12;
    if (totalactuators >= 817 && totalactuators <= 912)   // 13 circle
        i= 13;
    if (totalactuators >= 913 && totalactuators <= 1008)  // 14 circle
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

    if (!CORBA::is_nil(usd[i][l]))
    {
        try
        {
            status_var = usd[i][l]->status ();
        }
        catch (CORBA::SystemException& ex)
        {
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
    if (totalactuators == 1117)
    {
        i = l = totalactuators = 1;
    }
    circlecounter = i;
    actuatorcounter = l;
    totacts = totalactuators;

    if (corbaEx == true)
    {
        //printf("corba error\n");
        _THROW_EXCPT(ComponentErrors::CORBAProblemExImpl,"CSRTActiveSurfaceBossCore::watchingActiveSurfaceStatus()");
    }
    if (propertyEx == true)
    {
        //printf("property error\n");
        _THROW_EXCPT(ComponentErrors::CouldntGetAttributeExImpl,"CSRTActiveSurfaceBossCore::watchingActiveSurfaceStatus()");
    }
    if (notActivEx == true )
    {
        //printf("not active\n");
        _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CSRTActiveSurfaceBossCore::watchingActiveSurfaceStatus()");
    }
    //printf("NO error\n");
}

void CSRTActiveSurfaceBossCore::workingActiveSurface() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::ComponentErrorsEx)
{
    if(AutoUpdate)
    {
        if(!CORBA::is_nil(m_antennaBoss))
        {
            double azimuth, elevation;

            TIMEVALUE now;
            IRA::CIRATools::getTime(now);

            try
            {
                m_antennaBoss->getRawCoordinates(now.value().value, azimuth, elevation);
                onewayAction(ActiveSurface::AS_UPDATE, 0, 0, 0, elevation*DR2D, 0, 0, m_profile);
            }
            catch (CORBA::SystemException& ex)
            {
                _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CSRTActiveSurfaceBossCore::workingActiveSurface()");
                impl.setName(ex._name());
                impl.setMinor(ex.minor());
                m_status=Management::MNG_WARNING;
                throw impl;
            }
            catch (ComponentErrors::ComponentErrorsExImpl& ex)
            {
                ex.log(LM_DEBUG);
                throw ex.getComponentErrorsEx();
            }
        }
    }
}

void CSRTActiveSurfaceBossCore::setProfile(const ActiveSurface::TASProfile& newProfile) throw (ComponentErrors::ComponentErrorsExImpl)
{
    bool all_sectors = true;
    for(unsigned int i = 0; i < SECTORS; i++)
    {
        if(!m_sector[i]) all_sectors = false;
    }

    if(all_sectors) // USD tables has not been loaded yet
    {
        ifstream usdCorrections (USDTABLECORRECTIONS);
        if(!usdCorrections)
        {
            ACS_SHORT_LOG ((LM_INFO, "File %s not found", USDTABLECORRECTIONS));
            exit(-1);
        }
        actuatorsCorrections.length(NPOSITIONS);
        for (int i = 1; i <= CIRCLES; i++)
        {
            for (int l = 1; l <= actuatorsInCircle[i]; l++)
            {
                for (int s = 0; s < NPOSITIONS; s++)
                {
                    usdCorrections >> actuatorsCorrections[s];
                }
                if(!CORBA::is_nil(usd[i][l]))
                {
                    usd[i][l]->posTable(actuatorsCorrections, NPOSITIONS, DELTAEL, THRESHOLDPOS);
                }
            }
        }

        usdCounter = 0;
        for(unsigned int i = 0; i < SECTORS; i++)
        {
            m_sector[i] = false;
            usdCounter += usdCounters[i];
        }

        m_ASup=true;
        m_status=Management::MNG_OK;

        if(usdCounter < (int)lastUSD*WARNINGUSDPERCENT)
        {
            m_status=Management::MNG_WARNING;
        }
        if(usdCounter < (int)lastUSD*ERRORUSDPERCENT)
        {
            m_status=Management::MNG_FAILURE;
            m_ASup=false;
        }
    }

    if(m_ASup == true)
    {
        asOff();
        CIRATools::Wait(1000000);
        _SET_CDB_CORE(profile, newProfile,"SRTActiveSurfaceBossCore::setProfile")
        m_profile = newProfile;
        try
        {
            onewayAction(ActiveSurface::AS_PROFILE, 0, 0, 0, 0, 0, 0, newProfile);
        }
        catch (ComponentErrors::ComponentErrorsExImpl& ex)
        {
            ex.log(LM_DEBUG);
            throw ex.getComponentErrorsEx();
        }
        m_profileSetted = true;
        asOn();
    }
}

void CSRTActiveSurfaceBossCore::asSetup() throw (ComponentErrors::ComponentErrorsEx)
{
    try
    {
        onewayAction(ActiveSurface::AS_SETUP, 0, 0, 0, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void CSRTActiveSurfaceBossCore::asOn()
{
    if(m_profileSetted == true)
    {
        if((m_profile != ActiveSurface::AS_PARABOLIC_FIXED) && (m_profile != ActiveSurface::AS_SHAPED_FIXED) && (m_profile != ActiveSurface::AS_PARK))
        {
            enableAutoUpdate();
            m_tracking = true;
        }
        else
        {
            asOff();
            CIRATools::Wait(1000000);
            m_tracking = false;
            try
            {
                onewayAction(ActiveSurface::AS_UPDATE, 0, 0, 0, 45.0, 0, 0, m_profile);
            }
            catch (ComponentErrors::ComponentErrorsExImpl& ex)
            {
                ex.log(LM_DEBUG);
                throw ex.getComponentErrorsEx();
            }
        }
    }
    else
    {
        printf("you must set the profile first\n");
    }
}

void CSRTActiveSurfaceBossCore::asPark() throw (ComponentErrors::ComponentErrorsEx)
{
    if(m_profileSetted == true)
    {
        asOff();
        m_tracking = false;
        CIRATools::Wait(1000000);
        _SET_CDB_CORE(profile, ActiveSurface::AS_PARK,"SRTActiveSurfaceBossCore::asPark()")
        m_profile = ActiveSurface::AS_PARK;
        try
        {
            onewayAction(ActiveSurface::AS_REFPOS, 0, 0, 0, 0.0, 0, 0, m_profile);
        }
        catch (ComponentErrors::ComponentErrorsExImpl& ex)
        {
            ex.log(LM_DEBUG);
            throw ex.getComponentErrorsEx();
        }
    }
    else
    {
        printf("you must set the profile first\n");
    }
}

void CSRTActiveSurfaceBossCore::asOff() throw (ComponentErrors::ComponentErrorsEx)
{
    if(m_profileSetted == true)
    {
        disableAutoUpdate();
        try
        {
            onewayAction(ActiveSurface::AS_STOP, 0, 0, 0, 0, 0, 0, m_profile);
        }
        catch (ComponentErrors::ComponentErrorsExImpl& ex)
        {
            ex.log(LM_DEBUG);
            throw ex.getComponentErrorsEx();
        }
    }
    else
    {
        printf("you must set the profile first\n");
    }
}

void CSRTActiveSurfaceBossCore::setActuator(int circle, int actuator, long int& actPos, long int& cmdPos, long int& Fmin, long int& Fmax, long int& acc,long int& delay) throw (ComponentErrors::PropertyErrorExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
    if(!CORBA::is_nil(usd[circle][actuator]))
    {
        ACSErr::Completion_var completion;
        ACS::ROlong_var actPos_var;
        ACS::RWlong_var cmdPos_var;
        ACS::RWlong_var Fmin_var;
        ACS::RWlong_var Fmax_var;
        ACS::RWlong_var acc_var;
        ACS::RWlong_var delay_var;

        actPos_var = usd[circle][actuator]->actPos ();
        if(actPos_var.ptr() != ACS::ROlong::_nil())
        {
            actPos = actPos_var->get_sync (completion.out ());
        }
        else
        {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd actual position");
            throw impl;
        }

        cmdPos_var = usd[circle][actuator]->cmdPos ();
        if(cmdPos_var.ptr() != ACS::RWlong::_nil())
        {
            cmdPos = cmdPos_var->get_sync (completion.out ());
        }
        else
        {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd commanded position");
            throw impl;
        }

        Fmin_var = usd[circle][actuator]->Fmin ();
        if(Fmin_var.ptr() != ACS::RWlong::_nil())
        {
            Fmin = Fmin_var->get_sync (completion.out ());
        }
        else
        {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd Fmin");
            throw impl;
        }

        Fmax_var = usd[circle][actuator]->Fmax ();
        if(Fmax_var.ptr() != ACS::RWlong::_nil())
        {
            Fmax = Fmax_var->get_sync (completion.out ());
        }
        else
        {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd Fmax");
            throw impl;
        }

        acc_var = usd[circle][actuator]->acc ();
        if(acc_var.ptr() != ACS::RWlong::_nil())
        {
            acc = acc_var->get_sync (completion.out ());
        }
        else
        {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd acceleration");
            throw impl;
        }

        delay_var = usd[circle][actuator]->delay ();
        if(delay_var.ptr() != ACS::RWlong::_nil())
        {
            delay = delay_var->get_sync (completion.out ());
        }
        else
        {
            _EXCPT(ComponentErrors::PropertyErrorExImpl,impl,"CSRTActiveSurfaceBossCore::setActuator()");
            impl.setPropertyName("usd delay");
            throw impl;
        }
    }
    else
    {
        _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CSRTActiveSurfaceBossCore::setActuator()");
    }
}

void CSRTActiveSurfaceBossCore::usdStatus4GUIClient(int circle, int actuator, CORBA::Long_out status) throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl, ComponentErrors::ComponentNotActiveExImpl)
{
    if(!CORBA::is_nil(usd[circle][actuator]))
    {
        usd[circle][actuator]->getStatus(status);
    }
    else
    {
        _THROW_EXCPT(ComponentErrors::ComponentNotActiveExImpl,"CSRTActiveSurfaceBossCore::usdStatus4GUIClient()");
    }
}

void CSRTActiveSurfaceBossCore::recoverUSD(int circleIndex, int usdCircleIndex) throw (ComponentErrors::CouldntGetComponentExImpl)
{
    char serial_usd[23];
    int lanIndex;
    setserial (circleIndex, usdCircleIndex, lanIndex, serial_usd);

    usd[circleIndex][usdCircleIndex] = ActiveSurface::USD::_nil();
    try
    {
        usd[circleIndex][usdCircleIndex] = m_services->getComponent<ActiveSurface::USD>(serial_usd);
        lanradius[circleIndex][lanIndex] = usd[circleIndex][usdCircleIndex];
    }
    catch (maciErrType::CannotGetComponentExImpl& ex)
    {
        _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CSRTActiveSurfaceBossCore::recoverUSD()");
        Impl.setComponentName(serial_usd);
        Impl.log(LM_DEBUG);
    }
}

void CSRTActiveSurfaceBossCore::setradius(int radius, int &actuatorsradius, int &jumpradius)
{
    if(radius == 1 || radius == 5 || radius == 9 || radius == 13 ||
        radius == 17 || radius == 21 || radius == 25 || radius == 29 ||
        radius == 33 || radius == 37 || radius == 41 || radius == 45 ||
        radius == 49 || radius == 53 || radius == 57 || radius == 61 ||
        radius == 65 || radius == 69 || radius == 73 || radius == 77 ||
        radius == 81 || radius == 85 || radius == 89 || radius == 93)
    {
        actuatorsradius = 13;
        if(radius == 13 || radius == 37 || radius == 61 || radius == 85)
        {
            actuatorsradius++;
        }
        jumpradius = 2;
    }
    else if(radius == 3 || radius == 7 || radius == 11 || radius == 15 ||
             radius == 19 || radius == 23 || radius == 27 || radius == 31 ||
             radius == 35 || radius == 39 || radius == 43 || radius == 47 ||
             radius == 51 || radius == 55 || radius == 59 || radius == 63 ||
             radius == 67 || radius == 71 || radius == 75 || radius == 79 ||
             radius == 83 || radius == 87 || radius == 91 || radius == 95)
    {
        actuatorsradius = 15;
        if(radius == 11 || radius == 15 || radius == 35 || radius == 39 ||
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

    if(circle >= 1 && circle <= 2)
    {
        if(actuator >= 1 && actuator <= 3)
            sector = 1;
        if(actuator >= 4 && actuator <= 6)
            sector = 2;
        if(actuator >= 7 && actuator <= 9)
            sector = 3;
        if(actuator >= 10 && actuator <= 12)
            sector = 4;
        if(actuator >= 13 && actuator <= 15)
            sector = 5;
        if(actuator >= 16 && actuator <= 18)
            sector = 6;
        if(actuator >= 19 && actuator <= 21)
            sector = 7;
        if(actuator >= 22 && actuator <= 24)
            sector = 8;

        lan = actuator*4 - 1;
    }
    if(circle >= 3 && circle <= 6)
    {
        if(actuator >= 1 && actuator <= 6)
            sector = 1;
        if(actuator >= 7 && actuator <= 12)
            sector = 2;
        if(actuator >= 13 && actuator <= 18)
            sector = 3;
        if(actuator >= 19 && actuator <= 24)
            sector = 4;
        if(actuator >= 25 && actuator <= 30)
            sector = 5;
        if(actuator >= 31 && actuator <= 36)
            sector = 6;
        if(actuator >= 37 && actuator <= 42)
            sector = 7;
        if(actuator >= 43 && actuator <= 48)
            sector = 8;

        lan = actuator*2 -1;
    }
    if(circle >= 7 && circle <= 15)
    {
        if(actuator >= 1 && actuator <= 12)
            sector = 1;
        if(actuator >= 13 && actuator <= 24)
            sector = 2;
        if(actuator >= 25 && actuator <= 36)
            sector = 3;
        if(actuator >= 37 && actuator <= 48)
            sector = 4;
        if(actuator >= 49 && actuator <= 60)
            sector = 5;
        if(actuator >= 61 && actuator <= 72)
            sector = 6;
        if(actuator >= 73 && actuator <= 84)
            sector = 7;
        if(actuator >= 85 && actuator <= 96)
            sector = 8;

        lan = actuator;
    }
    if(circle == 16)
    {
        sector = actuator;
        if(actuator == 1)
            lan = 11;
        if(actuator == 2)
            lan = 15;
        if(actuator == 3)
            lan = 35;
        if(actuator == 4)
            lan = 39;
        if(actuator == 5)
            lan = 59;
        if(actuator == 6)
            lan = 63;
        if(actuator == 7)
            lan = 83;
        if(actuator == 8)
            lan = 87;
    }
    if(circle == 17)
    {
        sector = actuator*2;
        if(actuator == 1)
            lan = 13;
        if(actuator == 2)
            lan = 37;
        if(actuator == 3)
            lan = 61;
        if(actuator == 4)
            lan = 85;
    }
    lan -= 12*(sector-1);
    sprintf (serial_usd,"AS/SECTOR%02d/LAN%02d/USD%02d",sector, lan, circle);
}

void CSRTActiveSurfaceBossCore::enableAutoUpdate()
{
    if(!AutoUpdate)
    {
        AutoUpdate=true;
        ACS_LOG(LM_FULL_INFO,"CSRTActiveSurfaceBossCore::enableAutoUpdate()",(LM_NOTICE,"SRTActiveSurfaceBoss::AUTOMATIC_UPDATE_ENABLED"));
    }
}

void CSRTActiveSurfaceBossCore::disableAutoUpdate()
{
    if(AutoUpdate)
    {
        AutoUpdate=false;
        ACS_LOG(LM_FULL_INFO,"CSRTActiveSurfaceBossCore::disableAutoUpdate()",(LM_NOTICE,"SRTActiveSurfaceBoss::AUTOMATIC_UPDATE_DISABLED"));
    }
}

void CSRTActiveSurfaceBossCore::checkASerrors(const char* str, int circle, int actuator, ASErrors::ASErrorsEx Ex)
{
    ASErrors::ASErrorsExImpl exImpl(Ex);
    checkASerrors(str, circle, actuator, exImpl.getErrorCode());
}

void CSRTActiveSurfaceBossCore::checkASerrors(const char* str, int circle, int actuator, CompletionImpl comp)
{
    checkASerrors(str, circle, actuator, comp.getCode());
}

void CSRTActiveSurfaceBossCore::checkASerrors(const char* str, int circle, int actuator, int code)
{
    if(m_error_strings.find(code) != m_error_strings.end())
    {
        std::cout << "checkASerrors: " << str << " " << circle << "_" << actuator << " " << m_error_strings[code] << std::endl;
    }
}
