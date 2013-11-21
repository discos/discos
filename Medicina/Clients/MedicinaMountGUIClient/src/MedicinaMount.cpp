// System includes
#include <ctime>
#include <deque>
#include <exception>

// ACS includes
#include <acstime.h>
#include <MedicinaMount.h>

// QT includes

MedicinaMount::MedicinaMount (QObject *parent) : QThread(parent)
{
}

MedicinaMount::~MedicinaMount()
{
}

void MedicinaMount::stop (void)
{
  	monitor = false;
}

void MedicinaMount::setMount (Antenna::MedicinaMount_var theMount)
{
	tMount = theMount;
}

void MedicinaMount::run(void)
{
    ACSErr::Completion_var completion;
    Antenna::ROTStatus_var ACUStatus_var; ACS::pattern ACUStatus_val;
    Management::ROTSystemStatus_var AntennaMountStatus_var; ACS::pattern AntennaMountStatus_val;
    ACS::ROuLongLong_var ACUTime_var; ACS::uLongLong ACUTime_val;
    Antenna::ROTSections_var AntennaSection_var; ACS::pattern AntennaSection_val;
    ACS::ROpattern_var ServoSystemStatus_var; ACS::pattern ServoSystemStatus_val;
    ACS::ROpattern_var AzimuthServoStatus_var; ACS::pattern AzimuthServoStatus_val;
    ACS::ROpattern_var ElevationServoStatus_var; ACS::pattern ElevationServoStatus_val;
    ACS::ROdouble_var ActualAzimuth_var; CORBA::Double ActualAzimuth_val;
    ACS::ROdouble_var CommandedAzimuth_var; CORBA::Double CommandedAzimuth_val;
    ACS::ROdouble_var AzimuthError_var; CORBA::Double AzimuthError_val;
    ACS::ROdouble_var ActualElevation_var; CORBA::Double ActualElevation_val;
    ACS::ROdouble_var CommandedElevation_var; CORBA::Double CommandedElevation_val;
    ACS::ROdouble_var ElevationError_var; CORBA::Double ElevationError_val;
    Antenna::ROTCommonModes_var AzimuthOperatingMode_var; ACS::pattern AzimuthOperatingMode_val;
    Antenna::ROTCommonModes_var ElevationOperatingMode_var; ACS::pattern ElevationOperatingMode_val;

	try {
        while (monitor == true) {
            // MOUNT properties
            ACUStatus_var = tMount->status();
            if (ACUStatus_var.ptr() == Antenna::ROTStatus::_nil()) {
                ACE_CString ACE_Error_str("ACUStatus");
                PropertyErrorCode = 1;
                setACSPropertyError(ACE_Error_str);
            }
            else {
                ACUStatus_val = ACUStatus_var->get_sync(completion.out());
                setGUIACUStatus(ACUStatus_val);
            }

            AntennaMountStatus_var = tMount->mountStatus();
		    if (AntennaMountStatus_var.ptr() == Management::ROTSystemStatus::_nil()) {
			    ACE_CString ACE_Error_str("AntennaMountStatus");
                PropertyErrorCode = 2;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                AntennaMountStatus_val = AntennaMountStatus_var->get_sync (completion.out());
                setGUIAntennaMountStatus(AntennaMountStatus_val);
            }

		    ACUTime_var = tMount->time();
		    if (ACUTime_var.ptr() == ACS::ROuLongLong::_nil()) {
			    ACE_CString ACE_Error_str("ACUTime");
                PropertyErrorCode = 3;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                ACUTime_val = ACUTime_var->get_sync(completion.out());
                setGUIACUTime(ACUTime_val);
            }

		    AntennaSection_var = tMount->section();
		    if (AntennaSection_var.ptr() == Antenna::ROTSections::_nil()) {
			    ACE_CString ACE_Error_str("AntennaSection");
                PropertyErrorCode = 4;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                AntennaSection_val = AntennaSection_var->get_sync(completion.out());
                setGUIAntennaSection(AntennaSection_val);
            }

		    ServoSystemStatus_var = tMount->servoSystemStatus();
		    if (ServoSystemStatus_var.ptr() == ACS::ROpattern::_nil()) {
			    ACE_CString ACE_Error_str("ServoSystemStatus");
                PropertyErrorCode = 5;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                ServoSystemStatus_val = ServoSystemStatus_var->get_sync(completion.out());
                setGUIServoSystemStatus(ServoSystemStatus_val);
            }

		    AzimuthServoStatus_var = tMount->azimuthServoStatus();
		    if (AzimuthServoStatus_var.ptr() == ACS::ROpattern::_nil()) {
			    ACE_CString ACE_Error_str("AzimuthServoStatus");
                PropertyErrorCode = 6;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                AzimuthServoStatus_val = AzimuthServoStatus_var->get_sync(completion.out());
                setGUIAzimuthElevationServoStatus(AzimuthServoStatus_val, MedicinaMount::AZ);
            }

		    ElevationServoStatus_var = tMount->elevationServoStatus();
		    if (ElevationServoStatus_var.ptr() == ACS::ROpattern::_nil()) {
			    ACE_CString ACE_Error_str("ElevationServoStatus");
                PropertyErrorCode = 7;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                ElevationServoStatus_val = ElevationServoStatus_var->get_sync(completion.out());
                setGUIAzimuthElevationServoStatus(ElevationServoStatus_val, MedicinaMount::EL);
            }

		    ActualAzimuth_var = tMount->azimuth ();
		    if (ActualAzimuth_var.ptr() == ACS::ROdouble::_nil()) {
			    ACE_CString ACE_Error_str("ActualAzimuth");
                PropertyErrorCode = 8;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                ActualAzimuth_val = ActualAzimuth_var->get_sync(completion.out());
                setGUIPositionAndError(ActualAzimuth_val, MedicinaMount::AZ);
            }

            CommandedAzimuth_var = tMount->commandedAzimuth ();
		    if (CommandedAzimuth_var.ptr() == ACS::ROdouble::_nil()) {
			    ACE_CString ACE_Error_str("CommandedAzimuth");
                PropertyErrorCode = 9;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                CommandedAzimuth_val = CommandedAzimuth_var->get_sync(completion.out());
                setGUIPositionAndError(CommandedAzimuth_val, MedicinaMount::AZcomm);
            }

    	    AzimuthError_var = tMount->azimuthError ();
		    if (AzimuthError_var.ptr() == ACS::ROdouble::_nil()) {
			    ACE_CString ACE_Error_str("AzimuthError");
                PropertyErrorCode = 10;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                AzimuthError_val = AzimuthError_var->get_sync(completion.out());
                setGUIPositionAndError(AzimuthError_val, MedicinaMount::AZerr);
            }

    	    ActualElevation_var = tMount->elevation ();
		    if (ActualElevation_var.ptr() == ACS::ROdouble::_nil()) {
			    ACE_CString ACE_Error_str("ActualElevation");
                PropertyErrorCode = 11;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                ActualElevation_val = ActualElevation_var->get_sync(completion.out());
                setGUIPositionAndError(ActualElevation_val, MedicinaMount::EL);
            }

    	    CommandedElevation_var = tMount->commandedElevation ();
		    if (CommandedElevation_var.ptr() == ACS::ROdouble::_nil()) {
			    ACE_CString ACE_Error_str("CommandedElevation");
                PropertyErrorCode = 12;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                CommandedElevation_val = CommandedElevation_var->get_sync(completion.out());
                setGUIPositionAndError(CommandedElevation_val, MedicinaMount::ELcomm);
            }

    	    ElevationError_var = tMount->elevationError ();
		    if (ElevationError_var.ptr() == ACS::ROdouble::_nil()) {
			    ACE_CString ACE_Error_str("ElevationError");
                PropertyErrorCode = 13;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                ElevationError_val = ElevationError_var->get_sync(completion.out());
                setGUIPositionAndError(ElevationError_val, MedicinaMount::ELerr);
            }

		    AzimuthOperatingMode_var = tMount->azimuthMode();
		    if (AzimuthOperatingMode_var.ptr() == Antenna::ROTCommonModes::_nil()) {
			    ACE_CString ACE_Error_str("AzimuthOperatingMode");
                PropertyErrorCode = 14;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                AzimuthOperatingMode_val = AzimuthOperatingMode_var->get_sync(completion.out());
                setGUIAzimuthElevationOperatingMode(AzimuthOperatingMode_val, MedicinaMount::AZ);
            }

		    ElevationOperatingMode_var = tMount->elevationMode();
		    if (ElevationOperatingMode_var.ptr() == Antenna::ROTCommonModes::_nil()) {
			    ACE_CString ACE_Error_str("ElevationOperatingMode");
                PropertyErrorCode = 15;
			    setACSPropertyError(ACE_Error_str);
		    }
            else {
                ElevationOperatingMode_val = ElevationOperatingMode_var->get_sync(completion.out());
                setGUIAzimuthElevationOperatingMode(ElevationOperatingMode_val, MedicinaMount::EL);
            }

		    ACE_OS::sleep(1);
        }
	}
    _CATCH_ALL("MedicinaMount::run()")
}

void MedicinaMount::setACSPropertyError(ACE_CString ACE_Error_str)
{
	_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"MedicinaMountClient::Main()");
	impl.setPropertyName(ACE_Error_str);
	impl.log();

    emit setGUIPropertyErrorCode();
}


void MedicinaMount::stow(void)
{
	/** Callbacks */
	ACS::CBDescIn desc;
	CallbackVoid cbStow;
	ACS::CBvoid_var cbvoid;
	desc.normal_timeout=4000000;
	cbvoid=cbStow._this();
	try {
		tMount->stow(cbvoid.in(),desc);
	}
	_CATCH_ALL("MedicinaMount::stow()")
}

void MedicinaMount::unstow(void)
{
	/** Callbacks */
	ACS::CBDescIn desc;
	CallbackVoid cbUnstow;
	ACS::CBvoid_var cbvoid;
	desc.normal_timeout=4000000;
	cbvoid=cbUnstow._this();
	try {
		tMount->unstow(cbvoid.in(),desc);
	}
    _CATCH_ALL("MedicinaMount::unstow()")
}

void MedicinaMount::acustop(void)
{
	try {
		tMount->stop();
	}
    _CATCH_ALL("MedicinaMount::acustop()")
}
void MedicinaMount::resetfailures(void)
{
	try {
		tMount->resetFailures();
	}
    _CATCH_ALL("MedicinaMount::resetfailures()")
}

void MedicinaMount::Preset(double az, double el)
{
	try {
		tMount->preset(az, el);
	}
    _CATCH_ALL("MedicinaMount::Preset()")
}

void MedicinaMount::ProgramTrack(double az, double el, ACS::Time tm, bool restart)
{
	try {
		tMount->programTrack(az, el, tm, restart);
	}
    _CATCH_ALL("MedicinaMount::ProgramTrack()")
}

void MedicinaMount::Rates(double azrate, double elrate)
{
	try {
		tMount->rates(azrate, elrate);
	}
    _CATCH_ALL("MedicinaMount::Rates()")
}
void MedicinaMount::SetTime(ACS::Time tm)
{
	try {
		tMount->setTime(tm);
	}
    _CATCH_ALL("MedicinaMount::SetTime()")
}

void MedicinaMount::changeAZELmode(int AZmode, int ELmode)
{
	Antenna::TCommonModes azMode = Antenna::ACU_PRESET;
	Antenna::TCommonModes elMode = Antenna::ACU_PRESET;

	try {
		switch(AZmode)
		{
			case 0:
			azMode = Antenna::ACU_PRESET;
			break;
			case 1:
			azMode = Antenna::ACU_PROGRAMTRACK;
			break;
			case 2:
			azMode = Antenna::ACU_RATE;
			break;
			//case 3:
			//azMode = Antenna::ACU_POSITIONTRACK;
			//break;
		}
		switch(ELmode)
		{
			case 0:
			elMode = Antenna::ACU_PRESET;
			break;
			case 1:
			elMode = Antenna::ACU_PROGRAMTRACK;
			break;
			case 2:
			elMode = Antenna::ACU_RATE;
			break;
			//case 3:
			//elMode = Antenna::ACU_POSITIONTRACK;
			//break;
		}

		tMount->changeMode(azMode,elMode);
	}
    _CATCH_ALL("MedicinaMount::changeMode()")
}

void MedicinaMount::CallbackVoid::working(const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
    CompletionImpl comp(c);
    comp.log();
}

void MedicinaMount::CallbackVoid::done (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
    CompletionImpl comp(c);
    comp.log();
}

void MedicinaMount::setGUIAntennaMountStatus(ACS::pattern AntennaMountStatus_val)
{
	switch(AntennaMountStatus_val)
	{
		case (Management::MNG_OK):
            AntennaMountStatusCode = 1;
		    break;

		case (Management::MNG_WARNING):
            AntennaMountStatusCode = 0;
		    break;

        case (Management::MNG_FAILURE):
            AntennaMountStatusCode = -1;
		    break;
	}
    emit setGUIAntennaMountStatusColor();
}

void MedicinaMount::setGUIPositionAndError(CORBA::Double thePositionAndErrorValue, PositionAndError PosErr)
{
    switch (PosErr)
    {
        case (MedicinaMount::AZ):
            PositionAndErrorCode = 1;
            break;

        case (MedicinaMount::AZcomm):
            PositionAndErrorCode = 2;
            break;

        case (MedicinaMount::AZerr):
            PositionAndErrorCode = 3;
            break;

        case (MedicinaMount::EL):
            PositionAndErrorCode = 4;
            break;

        case (MedicinaMount::ELcomm):
            PositionAndErrorCode = 5;
            break;

        case (MedicinaMount::ELerr):
            PositionAndErrorCode = 6;
            break;
    }
    PositionAndErrorValue = thePositionAndErrorValue;
    emit setGUIPositionAndErrorValue();
}

void MedicinaMount::setGUIACUStatus(ACS::pattern ACUStatus_val)
{
	switch(ACUStatus_val)
	{
		case (Antenna::ACU_CNTD):
            ACUStatusCode = 1;
		    break;
		case (Antenna::ACU_CNTDING):
            ACUStatusCode = 0;
		    break;
		case (Antenna::ACU_NOTCNTD):
            ACUStatusCode = -1;
		    break;
		case (Antenna::ACU_BSY):
            ACUStatusCode = -2;
		    break;
	}
    emit setGUIACUStatusColor();
}

void MedicinaMount::setGUIAzimuthElevationOperatingMode(ACS::pattern OperatingMode_val, PositionAndError PosErr)
{
    switch (PosErr) {
        case (MedicinaMount::AZ):
            AzimuthElevationOperatingMode = 1;
            break;
        case (MedicinaMount::EL):
            AzimuthElevationOperatingMode = 2;
            break;
        case(MedicinaMount::AZcomm):
            break;
        case(MedicinaMount::AZerr):
            break;
        case(MedicinaMount::ELcomm):
            break;
        case(MedicinaMount::ELerr):
            break;
    }

	switch(OperatingMode_val)
	{
		case Antenna::ACU_STANDBY:
            AzElOperatingMode = 1;
		break;
		case Antenna::ACU_STOP:
            AzElOperatingMode = 2;
		break;
		case Antenna::ACU_PRESET:
            AzElOperatingMode = 3;
		break;
		case Antenna::ACU_PROGRAMTRACK:
            AzElOperatingMode = 4;
		break;
		case Antenna::ACU_RATE:
            AzElOperatingMode = 5;
		break;
		case Antenna::ACU_STOW:
            AzElOperatingMode = 6;
		break;
		//case Antenna::ACU_POSITIONTRACK:
        //    AzElOperatingMode = 7;
		break;
		case Antenna::ACU_UNSTOW:
            AzElOperatingMode = 8;
		break;
		case Antenna::ACU_UNKNOWN:
            AzElOperatingMode = 9;
		break;
	}
	
    emit setGUIAzimuthElevationOperatingModeColors();
}

void MedicinaMount::setGUIAntennaSection(ACS::pattern AntennaSection_val)
{
	switch(AntennaSection_val)
	{
		case (Antenna::ACU_CW):
		    AntennaSectionString = "cw";
		    break;

		case (Antenna::ACU_CCW):
		    AntennaSectionString = "ccw";
		    break;
	}
    emit setGUIAntennaSectionValue();
}

void MedicinaMount::setGUIServoSystemStatus(ACS::pattern ServoSystemStatus_val)
{
	if (ServoSystemStatus_val & 0x01)
        DoorInterlockCode = 1;
	else
        DoorInterlockCode = -1;

	if (ServoSystemStatus_val & 0x02)
        SafeCode = 1;
	else
        SafeCode = -1;

	if (ServoSystemStatus_val & 0x04)
        EmergencyOffCode = 1;
	else
        EmergencyOffCode = -1;

	if (ServoSystemStatus_val & 0x08)
        TransferErrorCode = 1;
	else
        TransferErrorCode = -1;

	if (ServoSystemStatus_val & 0x10)
        TimeErrorCode = 1;
	else
        TimeErrorCode = -1;

	if (ServoSystemStatus_val & 0x20)
        RFInhibitCode = 1;
	else
        RFInhibitCode = -1;

	if (ServoSystemStatus_val & 0x40)
        RemoteLocalControlCode = 1;
	else 
        RemoteLocalControlCode = -1;

    emit setGUIServoSystemColors();
}

void MedicinaMount::setGUIAzimuthElevationServoStatus(ACS::pattern AxisServoStatus_val, PositionAndError PosErr)
{
    switch (PosErr)
    {
        case (MedicinaMount::AZ):
            AzimuthElevationServoStatusCode = 1; // AZ
            break;

        case (MedicinaMount::EL):
            AzimuthElevationServoStatusCode = 2; // EL
            break;
        case(MedicinaMount::AZcomm):
            break;
        case(MedicinaMount::AZerr):
            break;
        case(MedicinaMount::ELcomm):
            break;
        case(MedicinaMount::ELerr):
            break;
    }

	if (AxisServoStatus_val & 0x01)
        EmergencyLimit = 1;
	else
        EmergencyLimit = -1;

	if (AxisServoStatus_val & 0x02)
        ccwLimit = 1;
	else
        ccwLimit = -1;

	if (AxisServoStatus_val & 0x04)
        cwLimit = 1;
	else
        cwLimit = -1;

	if (AxisServoStatus_val & 0x08)
        ccwPrelimit = 1;
	else
        ccwPrelimit = -1;

	if (AxisServoStatus_val & 0x10)
        cwPrelimit = 1;
	else
        cwPrelimit = -1;

	if (AxisServoStatus_val & 0x20)
        Stow = 1;
	else
        Stow = -1;

	if (AxisServoStatus_val & 0x40)
        pinInserted = 1;
	else
        pinInserted = -1;

	if (AxisServoStatus_val & 0x80)
        pinRetracted = 1;
	else
        pinRetracted = -1;

	if (AxisServoStatus_val & 0x100)
        servoFailure = 1;
	else
        servoFailure = -1;

	if (AxisServoStatus_val & 0x200)
        brakeFailure = 1;
	else
        brakeFailure = -1;

	if (AxisServoStatus_val & 0x400)
        encoderFailure = 1;
	else
        encoderFailure = -1;

	if (AxisServoStatus_val & 0x800)
        motionFailure = 1;
	else
        motionFailure = -1;

	if (AxisServoStatus_val & 0x1000)
        MotorOverSpeed = 1;
	else
        MotorOverSpeed = -1;

	if (AxisServoStatus_val & 0x2000)
        AuxMode = 1;
	else
        AuxMode = -1;

	if (AxisServoStatus_val & 0x4000)
        AxisDisabled = 1;
	else
        AxisDisabled = -1;

	if (AxisServoStatus_val & 0x8000)
        ComputerDisabled = 1;
	else
        ComputerDisabled = -1;
    
    emit setGUIAzimuthElevationServoStatusColors();
}

void MedicinaMount::setGUIACUTime(ACS::TimeInterval ACUTime)
{
	DurationHelper acutime(ACUTime);
	ACUTimeString = QString("%1%2%3%4%5%6%7%8%9").arg(acutime.day(),3).arg(" ",1).arg(acutime.hour(),2).arg(":",1).arg(acutime.minute(),2).arg(":",1).arg(acutime.second(),2).arg(".",1).arg(acutime.microSecond(),3);
    emit setGUIACUTimeValue();
}
