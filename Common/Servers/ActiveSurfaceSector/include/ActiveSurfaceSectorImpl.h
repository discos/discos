#ifndef ACTIVESURFACESECTORIMPL_H
#define ACTIVESURFACESECTORIMPL_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROlong.h>
#include <baciROboolean.h>
#include <enumpropROImpl.h>
#include <ActiveSurfaceSectorS.h>
#include <ActiveSurfaceLANS.h>
#include <ComponentErrors.h>
#include <ASErrors.h>
#include <ActiveSurfaceCommon.h>
#include "Definitions.h"


class ActiveSurfaceSectorImpl: public virtual baci::CharacteristicComponentImpl, public virtual POA_ActiveSurface::Sector
{
public:
    /**
    * Constructor.
    * @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
    * @param containerServices pointer to the class that exposes all services offered by container
    */
    ActiveSurfaceSectorImpl(const ACE_CString& CompName, maci::ContainerServices* containerServices);

    /**
     * Destructor.
     */
    virtual ~ActiveSurfaceSectorImpl();

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

    virtual ACS::ROlong_ptr index() throw (CORBA::SystemException);
    virtual ACS::ROboolean_ptr ready() throw (CORBA::SystemException);

    void update(CORBA::Double elevation, CORBA::Long tickIndex, CORBA::Boolean force) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

    void setLUT(const char* LUTName) throw (ComponentErrors::ComponentErrorsEx);

    void setProfile(ActiveSurface::TASProfile profile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ASErrors::UnknownProfileEx);

    void park() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ASErrors::ASErrorsEx);

    virtual void getTickStatus(CORBA::Long tickIndex, CORBA::BooleanSeq_out connected, ActiveSurface::USDStatusSeq_out USDSeq) throw (ComponentErrors::ComponentErrorsEx);

private:
    /**
     * Pointer to Container Services
     */
    maci::ContainerServices* m_containerServices;

    /* *
     * Active Surface profile
     */
    ActiveSurface::TASProfile m_profile;

    /**
     * Pointers to LANs of this sector
     */
    std::map<unsigned int, ActiveSurface::LAN_var> m_lans;

    baci::SmartPropertyPointer<baci::ROlong> m_index_sp;
    baci::SmartPropertyPointer<baci::ROboolean> m_ready_sp;
};

#endif /*ACTIVESURFACESECTORIMPL_H*/
