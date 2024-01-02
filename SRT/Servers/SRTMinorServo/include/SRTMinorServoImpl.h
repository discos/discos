#ifndef __SRTMINORSERVOIMPL_H__
#define __SRTMINORSERVOIMPL_H__

/**
 * SRTMinorServoImpl.h
 * 21/06/2023
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "Common.h"
#include <regex>
#include <IRA>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROlong.h>
#include <baciRObooleanSeq.h>
#include <baciROdoubleSeq.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <SRTMinorServoS.h>
#include <maciACSComponentDefines.h>
#include <ManagmentDefinitionsS.h>
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
    virtual void setup(const char* configuration_name = "");
    virtual ACS::doubleSeq* calcCoordinates(double elevation);

    virtual ACS::doubleSeq* getUserOffsets();
    virtual void setUserOffset(const char* axis_name, CORBA::Double offset);
    virtual void clearUserOffsets();
    virtual ACS::doubleSeq* getSystemOffsets();
    virtual void setSystemOffset(const char* axis_name, CORBA::Double offset);
    virtual void clearSystemOffsets();
    virtual void getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units);

    virtual Management::ROTBoolean_ptr enabled();
    virtual MinorServo::ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status();
    virtual Management::ROTBoolean_ptr block();
    virtual MinorServo::ROSRTMinorServoOperativeMode_ptr operative_mode();
    virtual ACS::RObooleanSeq_ptr physical_axes_enabled();
    virtual ACS::ROdoubleSeq_ptr physical_positions();
    virtual ACS::ROlong_ptr virtual_axes();
    virtual ACS::ROdoubleSeq_ptr virtual_positions();
    virtual ACS::ROdoubleSeq_ptr virtual_offsets();
    virtual ACS::ROdoubleSeq_ptr virtual_user_offsets();
    virtual ACS::ROdoubleSeq_ptr virtual_system_offsets();
    virtual Management::ROTBoolean_ptr in_use();
    virtual ACS::ROstring_ptr current_setup();

protected:
    std::atomic<unsigned int> m_physical_axes, m_virtual_axes;
    SRTMinorServoSocket* m_socket;
    std::string m_servo_name;

private:
    SRTMinorServoSocketConfiguration* m_socket_configuration;

    SRTMinorServoAnswerMap m_status;
    IRA::CSecureArea<SRTMinorServoAnswerMap>* m_status_securearea;

    std::vector<double> m_user_offsets;
    std::vector<double> m_system_offsets;

    std::vector<std::string> m_virtual_axes_names;
    std::vector<std::string> m_virtual_axes_units;
    SRTMinorServoLookupTable m_current_lookup_table;

    std::atomic<Management::TBoolean> m_in_use;
    std::string m_current_setup;

    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_enabled_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoCabinetStatus), POA_MinorServo::ROSRTMinorServoCabinetStatus> > m_drive_cabinet_status_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_block_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoOperativeMode), POA_MinorServo::ROSRTMinorServoOperativeMode> > m_operative_mode_ptr;
    baci::SmartPropertyPointer<baci::RObooleanSeq> m_physical_axes_enabled_ptr;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_physical_positions_ptr;
    baci::SmartPropertyPointer<baci::ROlong> m_virtual_axes_ptr;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_positions_ptr;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_offsets_ptr;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_user_offsets_ptr;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_system_offsets_ptr;
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_in_use_ptr;
    baci::SmartPropertyPointer<baci::ROstring> m_current_setup_ptr;

    std::vector<std::string> getPropertiesTable(const std::string& properties_name);
    void checkErrors();
};


class SRTGenericMinorServoImpl: public SRTBaseMinorServoImpl, public virtual POA_MinorServo::SRTGenericMinorServo
{
public:
    SRTGenericMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices);
    ~SRTGenericMinorServoImpl();

    void status()                                                                           { SRTBaseMinorServoImpl::status();                              };
    void stow(CORBA::Long stow_position = 1)                                                { SRTBaseMinorServoImpl::stow(stow_position);                   };
    void stop()                                                                             { SRTBaseMinorServoImpl::stop();                                };
    void preset(const ACS::doubleSeq& coordinates)                                          { SRTBaseMinorServoImpl::preset(coordinates);                   };
    void offset(const ACS::doubleSeq& offsets)                                              { SRTBaseMinorServoImpl::offset(offsets);                       };
    void setup(const char* configuration_name = "")                                         { SRTBaseMinorServoImpl::setup(configuration_name);             };
    virtual ACS::doubleSeq* calcCoordinates(double elevation)                               { return SRTBaseMinorServoImpl::calcCoordinates(elevation);     };
    virtual ACS::doubleSeq* getUserOffsets()                                                { return SRTBaseMinorServoImpl::getUserOffsets();               };
    virtual void setUserOffset(const char* axis_name, CORBA::Double offset)                 { SRTBaseMinorServoImpl::setUserOffset(axis_name, offset);      };
    virtual void clearUserOffsets()                                                         { SRTBaseMinorServoImpl::clearUserOffsets();                    };
    virtual ACS::doubleSeq* getSystemOffsets()                                              { return SRTBaseMinorServoImpl::getSystemOffsets();             };
    virtual void setSystemOffset(const char* axis_name, CORBA::Double offset)               { SRTBaseMinorServoImpl::setSystemOffset(axis_name, offset);    };
    virtual void clearSystemOffsets()                                                       { SRTBaseMinorServoImpl::clearSystemOffsets();                  };
    virtual void getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units)  { SRTBaseMinorServoImpl::getAxesInfo(axes_names, axes_units);   };

    virtual Management::ROTBoolean_ptr enabled()                                            { return SRTBaseMinorServoImpl::enabled();                      };
    virtual MinorServo::ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status()             { return SRTBaseMinorServoImpl::drive_cabinet_status();         };
    virtual Management::ROTBoolean_ptr block()                                              { return SRTBaseMinorServoImpl::block();                        };
    virtual MinorServo::ROSRTMinorServoOperativeMode_ptr operative_mode()                   { return SRTBaseMinorServoImpl::operative_mode();               };
    virtual ACS::RObooleanSeq_ptr physical_axes_enabled()                                   { return SRTBaseMinorServoImpl::physical_axes_enabled();        };
    virtual ACS::ROdoubleSeq_ptr physical_positions()                                       { return SRTBaseMinorServoImpl::physical_positions();           };
    virtual ACS::ROlong_ptr virtual_axes()                                                  { return SRTBaseMinorServoImpl::virtual_axes();                 };
    virtual ACS::ROdoubleSeq_ptr virtual_positions()                                        { return SRTBaseMinorServoImpl::virtual_positions();            };
    virtual ACS::ROdoubleSeq_ptr virtual_offsets()                                          { return SRTBaseMinorServoImpl::virtual_offsets();              };
    virtual ACS::ROdoubleSeq_ptr virtual_user_offsets()                                     { return SRTBaseMinorServoImpl::virtual_user_offsets();         };
    virtual ACS::ROdoubleSeq_ptr virtual_system_offsets()                                   { return SRTBaseMinorServoImpl::virtual_system_offsets();       };
    virtual Management::ROTBoolean_ptr in_use()                                             { return SRTBaseMinorServoImpl::in_use();                       };
    virtual ACS::ROstring_ptr current_setup()                                               { return SRTBaseMinorServoImpl::current_setup();                };
};

class SRTProgramTrackMinorServoImpl: public SRTBaseMinorServoImpl, public virtual POA_MinorServo::SRTProgramTrackMinorServo
{
public:
    SRTProgramTrackMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices);
    ~SRTProgramTrackMinorServoImpl();

    void status()                                                                           { SRTBaseMinorServoImpl::status();                              };
    void stow(CORBA::Long stow_position = 1)                                                { SRTBaseMinorServoImpl::stow(stow_position);                   };
    void stop()                                                                             { SRTBaseMinorServoImpl::stop();                                };
    void preset(const ACS::doubleSeq& coordinates)                                          { SRTBaseMinorServoImpl::preset(coordinates);                   };
    void offset(const ACS::doubleSeq& offsets)                                              { SRTBaseMinorServoImpl::offset(offsets);                       };
    void setup(const char* configuration_name = "")                                         { SRTBaseMinorServoImpl::setup(configuration_name);             };
    virtual ACS::doubleSeq* calcCoordinates(double elevation)                               { return SRTBaseMinorServoImpl::calcCoordinates(elevation);     };
    virtual ACS::doubleSeq* getUserOffsets()                                                { return SRTBaseMinorServoImpl::getUserOffsets();               };
    virtual void setUserOffset(const char* axis_name, CORBA::Double offset)                 { SRTBaseMinorServoImpl::setUserOffset(axis_name, offset);      };
    virtual void clearUserOffsets()                                                         { SRTBaseMinorServoImpl::clearUserOffsets();                    };
    virtual ACS::doubleSeq* getSystemOffsets()                                              { return SRTBaseMinorServoImpl::getSystemOffsets();             };
    virtual void setSystemOffset(const char* axis_name, CORBA::Double offset)               { SRTBaseMinorServoImpl::setSystemOffset(axis_name, offset);    };
    virtual void clearSystemOffsets()                                                       { SRTBaseMinorServoImpl::clearSystemOffsets();                  };
    virtual void getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units)  { SRTBaseMinorServoImpl::getAxesInfo(axes_names, axes_units);   };

    void programTrack(CORBA::Long trajectory_id, CORBA::Long point_id, CORBA::Double start_time, const ACS::doubleSeq& coordinates);

    virtual Management::ROTBoolean_ptr enabled()                                            { return SRTBaseMinorServoImpl::enabled();                      };
    virtual MinorServo::ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status()             { return SRTBaseMinorServoImpl::drive_cabinet_status();         };
    virtual Management::ROTBoolean_ptr block()                                              { return SRTBaseMinorServoImpl::block();                        };
    virtual MinorServo::ROSRTMinorServoOperativeMode_ptr operative_mode()                   { return SRTBaseMinorServoImpl::operative_mode();               };
    virtual ACS::RObooleanSeq_ptr physical_axes_enabled()                                   { return SRTBaseMinorServoImpl::physical_axes_enabled();        };
    virtual ACS::ROdoubleSeq_ptr physical_positions()                                       { return SRTBaseMinorServoImpl::physical_positions();           };
    virtual ACS::ROlong_ptr virtual_axes()                                                  { return SRTBaseMinorServoImpl::virtual_axes();                 };
    virtual ACS::ROdoubleSeq_ptr virtual_positions()                                        { return SRTBaseMinorServoImpl::virtual_positions();            };
    virtual ACS::ROdoubleSeq_ptr virtual_offsets()                                          { return SRTBaseMinorServoImpl::virtual_offsets();              };
    virtual ACS::ROdoubleSeq_ptr virtual_user_offsets()                                     { return SRTBaseMinorServoImpl::virtual_user_offsets();         };
    virtual ACS::ROdoubleSeq_ptr virtual_system_offsets()                                   { return SRTBaseMinorServoImpl::virtual_system_offsets();       };
    virtual Management::ROTBoolean_ptr in_use()                                             { return SRTBaseMinorServoImpl::in_use();                       };
    virtual ACS::ROstring_ptr current_setup()                                               { return SRTBaseMinorServoImpl::current_setup();                };
};

#endif
