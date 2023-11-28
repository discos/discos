#ifndef __SRTMINORSERVOBOSSIMPL_H__
#define __SRTMINORSERVOBOSSIMPL_H__

/**
 * SRTMinorServoBossImpl.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <IRA>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROboolean.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <SRTMinorServoBossS.h>
#include <SRTMinorServoS.h>
#include <SP_parser.h>
#include "ManagementErrors.h"
#include "MinorServoErrors.h"
#include "SRTMinorServoBossCore.h"
#include "SRTMinorServoStatusThread.h"
#include "MSDevIOs.h"

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr SRTMinorServoBossImpl::PROPERTYNAME()\
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
    virtual void setElevationTracking(const char*);
    virtual void setASConfiguration(const char*);
    virtual void setOffsets(const char*, const double&);
    virtual void clearOffsets();

    // MinorServoBoss interface methods
    virtual CORBA::Boolean checkScan(const ACS::Time, const MinorServo::MinorServoScan&, const Antenna::TRunTimeParameters&, MinorServo::TRunTimeParameters_out);
    virtual void startScan(ACS::Time&, const MinorServo::MinorServoScan&, const Antenna::TRunTimeParameters&);
    virtual void closeScan(ACS::Time&);

    CORBA::Boolean command(const char*, CORBA::String_out);

    // Properties methods
    virtual MinorServo::ROSRTMinorServoFocalConfiguration_ptr current_configuration();
    virtual ACS::ROboolean_ptr simulation_enabled();
    virtual ACS::ROdouble_ptr plc_time();
    virtual ACS::ROstring_ptr plc_version();
    virtual MinorServo::ROSRTMinorServoControlStatus_ptr control();
    virtual ACS::ROboolean_ptr power();
    virtual ACS::ROboolean_ptr emergency();
    virtual MinorServo::ROSRTMinorServoGregorianCoverStatus_ptr gregorian_cover();
    virtual ACS::ROdouble_ptr last_executed_command();
private:
    SRTMinorServoBossCore* m_core;
    SRTMinorServoStatusThread* m_status_thread;
    SimpleParser::CParser<SRTMinorServoBossImpl> *m_parser;

    // Properties pointers
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoFocalConfiguration), POA_MinorServo::ROSRTMinorServoFocalConfiguration> > m_current_configuration_ptr;
    baci::SmartPropertyPointer<baci::ROboolean> m_simulation_enabled_ptr;
    baci::SmartPropertyPointer<baci::ROdouble> m_plc_time_ptr;
    baci::SmartPropertyPointer<baci::ROstring> m_plc_version_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoControlStatus), POA_MinorServo::ROSRTMinorServoControlStatus> > m_control_ptr;
    baci::SmartPropertyPointer<baci::ROboolean> m_power_ptr;
    baci::SmartPropertyPointer<baci::ROboolean> m_emergency_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoGregorianCoverStatus), POA_MinorServo::ROSRTMinorServoGregorianCoverStatus> > m_gregorian_cover_ptr;
    baci::SmartPropertyPointer<baci::ROdouble> m_last_executed_command_ptr;
};

#endif
