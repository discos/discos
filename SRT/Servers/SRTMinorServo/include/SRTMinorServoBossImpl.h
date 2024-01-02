#ifndef __SRTMINORSERVOBOSSIMPL_H__
#define __SRTMINORSERVOBOSSIMPL_H__

/**
 * SRTMinorServoBossImpl.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "Common.h"
#include <IRA>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <MinorServoBossS.h>
#include <SRTMinorServoS.h>
#include <SP_parser.h>
#include "ManagementErrors.h"
#include "MinorServoErrors.h"
#include "SRTMinorServoBossCore.h"
#include "SRTMinorServoStatusThread.h"
#include "MSDevIOs.h"

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr SRTMinorServoBossImpl::PROPERTYNAME() \
{ \
    if (PROPERTY==0) return TYPE::_nil(); \
    TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
    return tmp._retn(); \
}

// Forward classes definitions
class SRTMinorServoBossCore;
class SRTMinorServoStatusThread;

class SRTMinorServoBossImpl: public baci::CharacteristicComponentImpl, public virtual POA_MinorServo::SRTMinorServoBoss
{
friend class SRTMinorServoBossCore;
public:
    // Constructor and destructor
    SRTMinorServoBossImpl(const ACE_CString&, maci::ContainerServices*);
    virtual ~SRTMinorServoBossImpl();

    // Characteristic component methods
    virtual void initialize();
    virtual void execute();
    virtual void cleanUp();
    virtual void aboutToAbort();

    // Parser methods
    virtual void setup(const char*);
    virtual void park();
    virtual void setElevationTracking(const char* elevation_tracking);
    virtual void setASConfiguration(const char* as_configuration);
    virtual void setOffsets(const char* axis_name, const double& offset);
    virtual void clearOffsets();
    CORBA::Boolean command(const char*, CORBA::String_out);

    // MinorServoBoss interface methods
    virtual CORBA::Boolean isElevationTrackingEn();
    virtual CORBA::Boolean isElevationTracking();
    virtual CORBA::Boolean isTracking();
    virtual CORBA::Boolean isStarting();
    virtual CORBA::Boolean isASConfiguration();
    virtual CORBA::Boolean isParking();
    virtual CORBA::Boolean isReady();
    virtual CORBA::Boolean isScanning();
    virtual CORBA::Boolean isScanActive();
    virtual char * getActualSetup();
    virtual char * getCommandedSetup();
    virtual CORBA::Double getCentralScanPosition();
    virtual void clearUserOffset(const char* servo_name);
    virtual void setUserOffset(const char* servo_axis_name, CORBA::Double offset);
    virtual ACS::doubleSeq* getUserOffset();
    virtual void clearSystemOffset(const char* servo_name);
    virtual void setSystemOffset(const char* servo_axis_name, CORBA::Double offset);
    virtual ACS::doubleSeq* getSystemOffset();
    virtual void getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units);
    virtual char* getScanAxis();
    virtual ACS::doubleSeq* getAxesPosition(ACS::Time);

    virtual CORBA::Boolean checkScan(const ACS::Time, const MinorServo::MinorServoScan&, const Antenna::TRunTimeParameters&, MinorServo::TRunTimeParameters_out);
    virtual void startScan(ACS::Time&, const MinorServo::MinorServoScan&, const Antenna::TRunTimeParameters&);
    virtual void closeScan(ACS::Time&);

    virtual void preset(double elevation);

    // Properties methods
    virtual Management::ROTSystemStatus_ptr status();
    virtual Management::ROTBoolean_ptr ready();
    virtual ACS::ROstring_ptr actualSetup();
    virtual ACS::ROstring_ptr motionInfo();
    virtual Management::ROTBoolean_ptr starting();
    virtual Management::ROTBoolean_ptr asConfiguration();
    virtual Management::ROTBoolean_ptr elevationTrack();
    virtual Management::ROTBoolean_ptr scanActive();
    virtual Management::ROTBoolean_ptr scanning();
    virtual Management::ROTBoolean_ptr tracking();

    virtual MinorServo::ROSRTMinorServoFocalConfiguration_ptr current_configuration();
    virtual Management::ROTBoolean_ptr simulation_enabled();
    virtual ACS::ROdouble_ptr plc_time();
    virtual ACS::ROstring_ptr plc_version();
    virtual MinorServo::ROSRTMinorServoControlStatus_ptr control();
    virtual Management::ROTBoolean_ptr power();
    virtual Management::ROTBoolean_ptr emergency();
    virtual MinorServo::ROSRTMinorServoGregorianCoverStatus_ptr gregorian_cover();
    virtual ACS::ROdouble_ptr last_executed_command();

private:
    SRTMinorServoBossCore* m_core;
    SRTMinorServoStatusThread* m_status_thread;
    SimpleParser::CParser<SRTMinorServoBossImpl> *m_parser;

    // Properties pointers
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus> > m_status_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_ready_ptr;
    baci::SmartPropertyPointer<baci::ROstring> m_actual_setup_ptr;
    baci::SmartPropertyPointer<baci::ROstring> m_motion_info_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_starting_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_as_configuration_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_elevation_tracking_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_scan_active_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_scanning_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_tracking_ptr;

    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoFocalConfiguration), POA_MinorServo::ROSRTMinorServoFocalConfiguration> > m_current_configuration_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_simulation_enabled_ptr;
    baci::SmartPropertyPointer<baci::ROdouble> m_plc_time_ptr;
    baci::SmartPropertyPointer<baci::ROstring> m_plc_version_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoControlStatus), POA_MinorServo::ROSRTMinorServoControlStatus> > m_control_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_power_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_emergency_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoGregorianCoverStatus), POA_MinorServo::ROSRTMinorServoGregorianCoverStatus> > m_gregorian_cover_ptr;
    baci::SmartPropertyPointer<baci::ROdouble> m_last_executed_command_ptr;
};

#endif
