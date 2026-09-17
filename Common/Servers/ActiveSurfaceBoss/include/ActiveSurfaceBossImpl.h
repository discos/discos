#ifndef ACTIVESURFACEBOSSIMPL_H
#define ACTIVESURFACEBOSSIMPL_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#define _CPLUSPLUS11_PORTING_

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <ActiveSurfaceBossS.h>
#include <IRA>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <ASErrors.h>
#include <ActiveSurfaceSectorS.h>
#include <SP_parser.h>
#include <ZMQLibrary.hpp>
#include <ActiveSurfaceProxy.h>
#include <AntennaProxy.h>
#include <ActiveSurfaceCommon.h>
#include "ActiveSurfaceBossWorkingThread.h"

using namespace baci;
using namespace maci;

namespace ZMQ = ZMQLibrary;

class ActiveSurfaceBossImplDevIOProfile;
class ActiveSurfaceBossImplDevIOStatus;
class ActiveSurfaceBossImplDevIOTracking;
class ActiveSurfaceBossImplDevIOLUT;

class ActiveSurfaceBossImpl: public virtual CharacteristicComponentImpl, public virtual POA_ActiveSurface::ActiveSurfaceBoss
{
    friend class ActiveSurfaceBossImplDevIOProfile;
    friend class ActiveSurfaceBossImplDevIOStatus;
    friend class ActiveSurfaceBossImplDevIOTracking;
    friend class ActiveSurfaceBossImplDevIOLUT;
    friend class ActiveSurfaceBossWorkingThread;
public:

    /**
     * Constructor.
     * @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
     * @param containerServices pointer to the class that exposes all services offered by container
     */
    ActiveSurfaceBossImpl(const ACE_CString& CompName, maci::ContainerServices* containerServices);

    /**
     * Destructor.
     */
    virtual ~ActiveSurfaceBossImpl();

    /**
     * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection.
     * Called before execute. It is implemented as a synchronous (blocking) call.
     * @throw ACSErr::ACSbaseExImpl
     * @arg \c ComponentErrors::MemoryAllocation
     */
    virtual void initialize() throw (ACSErr::ACSbaseExImpl);

    /**
     * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls any time.
     * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL
     * @throw ACSErr::ACSbaseExImpl
     * @arg \c ComponentErrors::CDBAccess
     */
    virtual void execute() throw (ACSErr::ACSbaseExImpl);

    /**
     * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
     */
    virtual void cleanUp();

    /**
     * Called by the container in case of error or emergency situation. This function tries to free all resources even though there is no
     * warranty that the function is completely executed before the component is destroyed.
     */
    virtual void aboutToAbort();

    /**
     * Returns a reference to the status property Implementation of IDL interface.
     * @return pointer to read-only ROTSystemStatus property status
     */
    virtual Management::ROTSystemStatus_ptr status();

    /**
     * Returns a reference to the enable property implementation of IDL interface.
     * @return pointer to read-only ROTBoolean  property enabled
     */
    virtual ActiveSurface::ROTASProfile_ptr profile();

    /**
     * Returns a reference to the tracking property implementation of IDL interface.
     * @return pointer to read-only ROTBoolean  property enabled
     */
    virtual Management::ROTBoolean_ptr tracking();

    /**
     * Returns a reference to the tracking property implementation of IDL interface.
     * @return pointer to read-only ACS::ROString property enabled
     */
    virtual ACS::ROstring_ptr LUT();

    /**
     *  This method can be called in order to disable the automatic update of the surface.
     */
    void asOff();

    /**
     *  This method can be called in order to enable the automatic update of the surface.
     */
    void asOn();

    /**
     * This is the command line interpreter for the sub-system. All the attributes and all the methods exposed by the boss can be
     * called. That means a full set of operation for standard observation, but not full control of the system.
     * @param the string that contains the command line to be parsed
     * @return the string that contains the answer to the command issued by the input parameter. The caller is resposible to
     * free the returned string (@sa CORBA::string_free).
     * @todo provide e full description of the syntax and protocol (to be decided yet)
     */
    virtual CORBA::Boolean command(const char *cmd,CORBA::String_out answer);

    /**
     * This method is used to park (i.e. reference position) the active surface).
     * @throw ManagementErrors::ParkingErrorEx
     */
    void park() throw (ManagementErrors::ParkingErrorEx);

    /**
     * This method will be used to configure the ActiveSurfaceBoss before starting an observation
     * @param config mnemonic code of the required configuration
     * @throw ManagementErrors::ConfigurationErrorEx
     */
    void setup(const char *config) throw (ManagementErrors::ConfigurationErrorEx);

    void setProfile(ActiveSurface::TASProfile profile) throw (ComponentErrors::ComponentErrorsEx, ASErrors::ASErrorsEx);

    void setLUT(const char* LUTName) throw (ComponentErrors::ComponentErrorsEx, ASErrors::ASErrorsEx);

private:
    void _park() throw (ManagementErrors::ParkingErrorExImpl);
    void _setup(const char *config) throw (ManagementErrors::ConfigurationErrorExImpl);
    void _setProfile(ActiveSurface::TASProfile profile) throw (ComponentErrors::ComponentErrorsExImpl, ASErrors::ASErrorsExImpl);
    void _setLUT(const char* LUTName) throw (ComponentErrors::ComponentErrorsExImpl, ASErrors::ASErrorsExImpl);

    void pollSectorStatus(unsigned long tickIndex);
    void update(unsigned long tickIndex) throw (ComponentErrors::ComponentErrorsEx);

    void publishZMQDictionary(ACS::Time timestamp);

    inline bool checkProfile(const ActiveSurface::TASProfile& profile) const { return m_acceptedProfiles.count(profile) > 0; }

    ContainerServices& m_containerServices;

    SimpleParser::CParser<ActiveSurfaceBossImpl> m_parser;

    SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus>> m_pstatus;
    SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(ActiveSurface::TASProfile), POA_ActiveSurface::ROTASProfile>> m_pprofile;
    SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_ptracking;
    SmartPropertyPointer<ROstring> m_pLUT;

    std::atomic<ActiveSurface::TASProfile> m_profile;
    std::atomic<Management::TSystemStatus> m_status;
    std::atomic<Management::TBoolean> m_tracking;
    std::atomic<bool> m_trackingEnabled;
    std::string m_LUT;

    std::map<unsigned int, ActiveSurface::Sector_proxy> m_sectors;

    std::set<ActiveSurface::TASProfile> m_acceptedProfiles;

    unsigned int m_maxUSDCount, m_maxTickIndex;

    ActiveSurfaceBossWorkingThread* m_workingThread;

    ZMQ::ZMQPublisher m_zmqPublisher;
    ZMQ::ZMQDictionary m_zmqDictionary;

    Antenna::AntennaBoss_proxy m_antennaBoss;
};

#endif /*ACTIVESURFACEBOSSIMPL_H*/
