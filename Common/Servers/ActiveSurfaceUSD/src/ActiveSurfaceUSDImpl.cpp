/*******************************************************************************
 * OAC Osservatorio Astronomico di Cagliari
 *
 * This code is under GNU General Public Licence (GPL).
 *
 * Who                                          When    What
 * Giuseppe Maccaferri                          2005    Creation
 * Carlo Migoni (migoni@ca.astro.it)            2013    Revision
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)  2019    Revision
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)  2026    Redesign: wrapper over core
 ******************************************************************************/

#include <baciDB.h>
#include <maciContainerImpl.h>
#include <ActiveSurfaceUSDImpl.h>
#include "ActiveSurfaceUSDDevIO.h"

using namespace maci;

// ===========================================================================
// Constructor / destructor
// ===========================================================================

USDImpl::USDImpl(const ACE_CString& name, maci::ContainerServices* cs) :
    CharacteristicComponentImpl(name, cs),
    m_actPos_sp(this),
    m_status_sp(this),
    m_softVer_sp(this),
    m_type_sp(this),
    m_cmdPos_sp(this),
    m_Fmin_sp(this),
    m_Fmax_sp(this),
    m_acc_sp(this),
    m_delay_sp(this),
    m_uBits_sp(this),
    m_core(nullptr)
{
}

USDImpl::~USDImpl()
{
    ACS_TRACE("USDImpl::~USDImpl()");
}

// ===========================================================================
// ACS lifecycle
// ===========================================================================

void USDImpl::initialize()
{
    ACS_TRACE("USDImpl::initialize()");

    // Obtain (or create) the USDCore instance for this component.
    // Construction reads the CDB but does NOT touch hardware.
    m_core = ActiveSurface::USDCore::getInstance(std::string(name()), getContainerServices());
    m_core->initialize();

    const ACE_CString compName(name());

    m_actPos_sp = new ROlong(compName + ":actPos", getComponent(), new USDDevIO<CORBA::Long, USDProp::ActPos> (*m_core), true);
    m_status_sp = new ROpattern(compName + ":status", getComponent(), new USDDevIO<ACS::pattern, USDProp::Status>(*m_core), true);
    m_softVer_sp = new ROlong(compName + ":softVer", getComponent(), new USDDevIO<CORBA::Long, USDProp::SoftVer>(*m_core), true);
    m_type_sp = new ROlong(compName + ":type", getComponent(), new USDDevIO<CORBA::Long, USDProp::Type>(*m_core), true);
    m_cmdPos_sp = new RWlong(compName + ":cmdPos", getComponent(), new USDDevIO<CORBA::Long, USDProp::CmdPos>(*m_core), true);
    m_Fmin_sp = new RWlong(compName + ":Fmin", getComponent(), new USDDevIO<CORBA::Long, USDProp::Fmin>(*m_core), true);
    m_Fmax_sp = new RWlong(compName + ":Fmax", getComponent(), new USDDevIO<CORBA::Long, USDProp::Fmax>(*m_core), true);
    m_acc_sp = new RWlong(compName + ":acc", getComponent(), new USDDevIO<CORBA::Long, USDProp::Acc>(*m_core), true);
    m_delay_sp = new RWlong(compName + ":delay", getComponent(), new USDDevIO<CORBA::Long, USDProp::Delay>(*m_core), true);
    m_uBits_sp = new RWlong(compName + ":uBits", getComponent(), new USDDevIO<CORBA::Long, USDProp::UBits>(*m_core), true);
}

void USDImpl::execute()
{
    ACS_TRACE("USDImpl::execute()");

    // If the LAN already ran a broadcast init and marked this USD as
    // initialized, skip unicast init entirely.
    if (!m_core->isInitialized())
    {
        try
        {
            m_core->stop();

            if(m_core->needsReset())
            {
                m_core->hardwareReset();
            }

            m_core->sendUnicastConfig();
            m_core->readHardwareDetails();
        }
        catch(ASErrors::ASErrorsExImpl& impl)
        {
            impl.log(LM_WARNING);
            ACS_SHORT_LOG((LM_WARNING, "USDImpl %s: unicast boot failed.", name()));
        }
    }

    ACS_SHORT_LOG((LM_INFO, "USDImpl %s: ready.", name()));
}

void USDImpl::cleanUp()
{
    ACS_TRACE("USDImpl::cleanUp()");
}

void USDImpl::aboutToAbort()
{
    ACS_TRACE("USDImpl::aboutToAbort()");
}

// ===========================================================================
// Motion commands
// ===========================================================================

void USDImpl::stop()
{
    ACS_TRACE("USDImpl::stop()");

    try
    {
        m_core->stop();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::up()
{
    ACS_TRACE("USDImpl::up()");

    try
    {
        m_core->up();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::down()
{
    ACS_TRACE("USDImpl::down()");

    try
    {
        m_core->down();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::move(CORBA::Long incr)
{
    ACS_TRACE("USDImpl::move()");

    try
    {
        m_core->move(static_cast<int32_t>(incr));
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::refPos()
{
    ACS_TRACE("USDImpl::refPos()");

    try
    {
        m_core->refPos();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::stow()
{
    ACS_TRACE("USDImpl::stow()");

    try
    {
        m_core->stow();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::setup()
{
    ACS_TRACE("USDImpl::setup()");

    try
    {
        m_core->setup();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::top()
{
    ACS_TRACE("USDImpl::top()");

    try
    {
        m_core->top();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::bottom()
{
    ACS_TRACE("USDImpl::bottom()");

    try
    {
        m_core->bottom();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

// ===========================================================================
// Hardware management commands
// ===========================================================================

void USDImpl::reset()
{
    ACS_TRACE("USDImpl::reset()");

    try
    {
        m_core->reset();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_ERROR);
        ASErrors::USDErrorExImpl exImpl(__FILE__, __LINE__, "USDImpl::reset()");
        throw exImpl.getASErrorsEx();
    }
}

void USDImpl::calibrate()
{
    ACS_TRACE("USDImpl::calibrate()");

    try
    {
        m_core->calibrate();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_ERROR);
    }
}

void USDImpl::calVer()
{
    ACS_TRACE("USDImpl::calVer()");

    try
    {
        m_core->calVer();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_ERROR);
    }
}

void USDImpl::writeCalibration(CORBA::Double_out  cammaLenD, CORBA::Double_out  cammaPosD, CORBA::Boolean_out calibrated)
{
    ACS_TRACE("USDImpl::writeCalibration()");

    try
    {
        double len, pos;
        bool   cal;
        m_core->writeCalibration(len, pos, cal);
        cammaLenD  = len;
        cammaPosD  = pos;
        calibrated = cal;
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_ERROR);
        throw ex.getASErrorsEx();
    }
}

// ===========================================================================
// Status
// ===========================================================================

void USDImpl::readStatus()
{
    ACS_TRACE("USDImpl::readStatus()");

    try
    {
        m_core->readHWStatus();
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
    }
}

void USDImpl::getStatus(ActiveSurface::USDStatus_out currentStatus)
{
    ACS_TRACE("USDImpl::getStatus()");

    currentStatus = m_core->getUSDStatus();
}

// ===========================================================================
// CORBA property accessors
// ===========================================================================

#define RETURN_PROPERTY(type, sp) \
    if (!sp) return type::_nil(); \
    type##_var prop = type::_narrow(sp->getCORBAReference()); \
    return prop._retn();

ACS::ROlong_ptr    USDImpl::actPos()  { RETURN_PROPERTY(ACS::ROlong,    m_actPos_sp)  }
ACS::ROpattern_ptr USDImpl::status()  { RETURN_PROPERTY(ACS::ROpattern, m_status_sp)  }
ACS::ROlong_ptr    USDImpl::softVer() { RETURN_PROPERTY(ACS::ROlong,    m_softVer_sp) }
ACS::ROlong_ptr    USDImpl::type()    { RETURN_PROPERTY(ACS::ROlong,    m_type_sp)    }
ACS::RWlong_ptr    USDImpl::cmdPos()  { RETURN_PROPERTY(ACS::RWlong,    m_cmdPos_sp)  }
ACS::RWlong_ptr    USDImpl::Fmin()    { RETURN_PROPERTY(ACS::RWlong,    m_Fmin_sp)    }
ACS::RWlong_ptr    USDImpl::Fmax()    { RETURN_PROPERTY(ACS::RWlong,    m_Fmax_sp)    }
ACS::RWlong_ptr    USDImpl::acc()     { RETURN_PROPERTY(ACS::RWlong,    m_acc_sp)     }
ACS::RWlong_ptr    USDImpl::delay()   { RETURN_PROPERTY(ACS::RWlong,    m_delay_sp)   }
ACS::RWlong_ptr    USDImpl::uBits()   { RETURN_PROPERTY(ACS::RWlong,    m_uBits_sp)   }

#undef RETURN_PROPERTY

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(USDImpl)
