#ifndef __SRTMINORSERVOIMPL_H__
#define __SRTMINORSERVOIMPL_H__

/**
 * SRTMinorServoImpl.h
 * 21/06/2023
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <regex>
#include <IRA>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROlong.h>
#include <baciROboolean.h>
#include <baciRObooleanSeq.h>
#include <baciROdoubleSeq.h>
#include <enumpropROImpl.h>
#include <SRTMinorServoS.h>
#include <maciACSComponentDefines.h>
#include "ManagementErrors.h"
#include "MinorServoErrors.h"
#include "SRTMinorServoSocket.h"
#include "MSDevIOs.h"

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr SRTBaseMinorServoImpl::PROPERTYNAME()\
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}

using SRTMinorServoLookupTable = std::map<std::string, std::vector<double> >;


class SRTBaseMinorServoImpl: public baci::CharacteristicComponentImpl
{
public:
    SRTBaseMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices);
    virtual ~SRTBaseMinorServoImpl();

    virtual void initialize();
    virtual void execute();
    virtual void cleanUp();
    virtual void aboutToAbort();

    virtual void status();
    virtual void stow(CORBA::Long stow_position = 1);
    virtual void stop();
    virtual void preset(const ACS::doubleSeq& coordinates);
    virtual void offset(const ACS::doubleSeq& offsets);
    virtual void setup(const char* configuration_name);

    virtual ACS::ROboolean_ptr enabled();
    virtual MinorServo::ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status();
    virtual ACS::ROboolean_ptr block();
    virtual MinorServo::ROSRTMinorServoOperativeMode_ptr operative_mode();
    virtual ACS::RObooleanSeq_ptr axes_enabled();
    virtual ACS::ROdoubleSeq_ptr physical_positions();
    virtual ACS::ROdoubleSeq_ptr virtual_positions();
    virtual ACS::ROdoubleSeq_ptr virtual_offsets();

protected:
    unsigned int m_physical_axes, m_virtual_axes;
    SRTMinorServoSocket* m_socket;
    std::string m_servo_name;

private:
    SRTMinorServoSocketConfiguration* m_socket_configuration;

    SRTMinorServoAnswerMap m_status;
    IRA::CSecureArea<SRTMinorServoAnswerMap>* m_status_securearea;

    std::vector<std::string> m_virtual_axes_names;
    SRTMinorServoLookupTable m_current_lookup_table;

    baci::SmartPropertyPointer<baci::ROboolean> m_enabled_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoCabinetStatus), POA_MinorServo::ROSRTMinorServoCabinetStatus> > m_drive_cabinet_status_ptr;
    baci::SmartPropertyPointer<baci::ROboolean> m_block_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoOperativeMode), POA_MinorServo::ROSRTMinorServoOperativeMode> > m_operative_mode_ptr;
    baci::SmartPropertyPointer<baci::RObooleanSeq> m_axes_enabled_ptr;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_physical_positions_ptr;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_positions_ptr;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_offsets_ptr;

    std::vector<std::string> getPropertiesTable(std::string properties_name);
    SRTMinorServoLookupTable getLookupTable(std::string configuration_name);

    void checkErrors();
};


class SRTGenericMinorServoImpl: public SRTBaseMinorServoImpl, public virtual POA_MinorServo::SRTGenericMinorServo
{
public:
    SRTGenericMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices);
    ~SRTGenericMinorServoImpl();

    void status()                                                               { SRTBaseMinorServoImpl::status();                      };
    void stow(CORBA::Long stow_position = 1)                                    { SRTBaseMinorServoImpl::stow(stow_position);           };
    void stop()                                                                 { SRTBaseMinorServoImpl::stop();                        };
    void preset(const ACS::doubleSeq& coordinates)                              { SRTBaseMinorServoImpl::preset(coordinates);           };
    void offset(const ACS::doubleSeq& offsets)                                  { SRTBaseMinorServoImpl::offset(offsets);               };
    void setup(const char* configuration_name)                                  { SRTBaseMinorServoImpl::setup(configuration_name);     };

    virtual ACS::ROboolean_ptr enabled()                                        { return SRTBaseMinorServoImpl::enabled();              };
    virtual MinorServo::ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status() { return SRTBaseMinorServoImpl::drive_cabinet_status(); };
    virtual ACS::ROboolean_ptr block()                                          { return SRTBaseMinorServoImpl::block();                };
    virtual MinorServo::ROSRTMinorServoOperativeMode_ptr operative_mode()       { return SRTBaseMinorServoImpl::operative_mode();       };
    virtual ACS::RObooleanSeq_ptr axes_enabled()                                { return SRTBaseMinorServoImpl::axes_enabled();         };
    virtual ACS::ROdoubleSeq_ptr physical_positions()                           { return SRTBaseMinorServoImpl::physical_positions();   };
    virtual ACS::ROdoubleSeq_ptr virtual_positions()                            { return SRTBaseMinorServoImpl::virtual_positions();    };
    virtual ACS::ROdoubleSeq_ptr virtual_offsets()                              { return SRTBaseMinorServoImpl::virtual_offsets();      };
};

class SRTProgramTrackMinorServoImpl: public SRTBaseMinorServoImpl, public virtual POA_MinorServo::SRTProgramTrackMinorServo
{
public:
    SRTProgramTrackMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices);
    ~SRTProgramTrackMinorServoImpl();

    void status()                                                               { SRTBaseMinorServoImpl::status();                      };
    void stow(CORBA::Long stow_position = 1)                                    { SRTBaseMinorServoImpl::stow(stow_position);           };
    void stop()                                                                 { SRTBaseMinorServoImpl::stop();                        };
    void preset(const ACS::doubleSeq& coordinates)                              { SRTBaseMinorServoImpl::preset(coordinates);           };
    void offset(const ACS::doubleSeq& offsets)                                  { SRTBaseMinorServoImpl::offset(offsets);               };
    void setup(const char* configuration_name)                                  { SRTBaseMinorServoImpl::setup(configuration_name);     };

    void programTrack(CORBA::Long trajectory_id, CORBA::Long point_id, CORBA::Double start_time, const ACS::doubleSeq& coordinates);

    virtual ACS::ROboolean_ptr enabled()                                        { return SRTBaseMinorServoImpl::enabled();              };
    virtual MinorServo::ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status() { return SRTBaseMinorServoImpl::drive_cabinet_status(); };
    virtual ACS::ROboolean_ptr block()                                          { return SRTBaseMinorServoImpl::block();                };
    virtual MinorServo::ROSRTMinorServoOperativeMode_ptr operative_mode()       { return SRTBaseMinorServoImpl::operative_mode();       };
    virtual ACS::RObooleanSeq_ptr axes_enabled()                                { return SRTBaseMinorServoImpl::axes_enabled();         };
    virtual ACS::ROdoubleSeq_ptr physical_positions()                           { return SRTBaseMinorServoImpl::physical_positions();   };
    virtual ACS::ROdoubleSeq_ptr virtual_positions()                            { return SRTBaseMinorServoImpl::virtual_positions();    };
    virtual ACS::ROdoubleSeq_ptr virtual_offsets()                              { return SRTBaseMinorServoImpl::virtual_offsets();      };
};

#endif
