// System includes

// ACS includes
#include <baci.h>
#include <MedicinaMountC.h>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <IRA>

// QT includes
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtGui/QColor>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtCore/QString>

/*
#define _CATCH_ALL(ROUTINE) \
	catch (ComponentErrors::ComponentErrorsEx &E) { \
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,ROUTINE); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
	catch (CORBA::SystemException &C) { \
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,ROUTINE); \
		impl.setName(C._name()); \
		impl.setMinor(C.minor()); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
	catch (...) { \
		_EXCPT(ClientErrors::UnknownExImpl,impl,ROUTINE); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
    emit setGUICatchAllErrorCode();
*/

class MedicinaMount : public QThread
{
    Q_OBJECT

	public:
        MedicinaMount(QObject *parent = 0);
        ~MedicinaMount();

	    void setMount(Antenna::MedicinaMount_var);
	    void run(void);
	    void stop(void);
	    void stow(void);
	    void unstow(void);
	    void acustop(void);
	    void Preset(double az, double el);
	    void ProgramTrack(double az, double el, ACS::Time tm, bool restart);
	    void changeAZELmode(int AZmode, int ELmode);
	    void resetfailures(void);
	    void SetTime(ACS::Time tm);
	    void Rates(double azrate, double elrate);

        class CallbackVoid;

        bool monitor;
        int AntennaMountStatusCode;
        int ACUStatusCode;
        int PositionAndErrorCode; double PositionAndErrorValue;
        QString ACUTimeString;
        QString AntennaSectionString;
        int DoorInterlockCode;
        int SafeCode;
        int EmergencyOffCode;
        int TransferErrorCode;
        int TimeErrorCode;
        int RFInhibitCode;
        int RemoteLocalControlCode;
        int AzimuthElevationServoStatusCode;
        int cwPrelimit, cwLimit, ccwPrelimit, ccwLimit;
        int EmergencyLimit, Stow;
        int pinInserted, pinRetracted;
        int servoFailure, brakeFailure, encoderFailure, motionFailure;
        int MotorOverSpeed, AuxMode, AxisDisabled, ComputerDisabled;
        int AzimuthElevationOperatingMode, AzElOperatingMode;
        int PropertyErrorCode;
        int CatchAllErrorCode; QString QCatchAllError;

    signals:
        int setGUIAntennaMountStatusColor();
        int setGUIACUStatusColor();
        int setGUIPositionAndErrorValue();
        int setGUIACUTimeValue();
        int setGUIAntennaSectionValue();
        int setGUIServoSystemColors();
        int setGUIAzimuthElevationServoStatusColors();
        int setGUIAzimuthElevationOperatingModeColors();
        int setGUIPropertyErrorCode();
        int setGUICatchAllErrorCode();

	private:
	    Antenna::MedicinaMount_var tMount;
        virtual void setACSPropertyError(ACE_CString);
        void setGUIAntennaMountStatus(ACS::pattern);
        void setGUIACUStatus(ACS::pattern);
        enum PositionAndError {AZ,AZcomm,AZerr,EL,ELcomm,ELerr};
        void setGUIPositionAndError(CORBA::Double, PositionAndError);
        void setGUIACUTime(ACS::TimeInterval);
        void setGUIAntennaSection(ACS::pattern);
        void setGUIServoSystemStatus(ACS::pattern);
        void setGUIAzimuthElevationServoStatus(ACS::pattern, PositionAndError);
        void setGUIAzimuthElevationOperatingMode(ACS::pattern, PositionAndError);
};

class MedicinaMount::CallbackVoid : public virtual POA_ACS::CBvoid
{
	public:
	CallbackVoid() {}
	~CallbackVoid() {}
	void working (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	void done (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	CORBA::Boolean negotiate (ACS::TimeInterval time_to_transmit, const ACS::CBDescOut &desc) throw (CORBA::SystemException) {
		return true;
	}

	private:
};

#define _CATCH_ALL(ROUTINE) \
	catch (ComponentErrors::ComponentErrorsEx &E) { \
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,ROUTINE); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
	catch (CORBA::SystemException &C) { \
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,ROUTINE); \
		impl.setName(C._name()); \
		impl.setMinor(C.minor()); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
	catch (...) { \
		_EXCPT(ClientErrors::UnknownExImpl,impl,ROUTINE); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
    emit setGUICatchAllErrorCode();
