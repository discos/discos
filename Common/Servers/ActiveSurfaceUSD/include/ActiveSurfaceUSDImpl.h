#ifndef ACTIVE_SURFACE_USD_IMPL_H
#define ACTIVE_SURFACE_USD_IMPL_H

/*******************************************************************************
 * OAC Osservatorio Astronomico di Cagliari
 *
 * This code is under GNU General Public Licence (GPL).
 *
 * Who                                          When    What
 * Giuseppe Maccaferri                          2005    Creation
 * Carlo Migoni (migoni@ca.astro.it)            2013    Revision
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)  2019    Revision
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)  2026    Redesign
 ******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciROlong.h>
#include <baciRWlong.h>
#include <baciROpattern.h>
#include <baciSmartPropertyPointer.h>
#include <ComponentErrors.h>
#include <ASErrors.h>
#include <ActiveSurfaceCommonS.h>
#include <ActiveSurfaceUSDS.h>
#include <ActiveSurfaceUSDCore.hpp>

using namespace baci;
using namespace maci;

class USDImpl : public CharacteristicComponentImpl, public virtual POA_ActiveSurface::USD
{
public:
    USDImpl(const ACE_CString& name, maci::ContainerServices* containerServices);
    virtual ~USDImpl();

    virtual void initialize();
    virtual void execute();
    virtual void cleanUp();
    virtual void aboutToAbort();

    // -----------------------------------------------------------------------
    // CORBA property accessors
    // -----------------------------------------------------------------------
    virtual ACS::ROlong_ptr    actPos();
    virtual ACS::ROpattern_ptr status();
    virtual ACS::ROlong_ptr    softVer();
    virtual ACS::ROlong_ptr    type();
    virtual ACS::RWlong_ptr    cmdPos();
    virtual ACS::RWlong_ptr    Fmin();
    virtual ACS::RWlong_ptr    Fmax();
    virtual ACS::RWlong_ptr    acc();
    virtual ACS::RWlong_ptr    delay();
    virtual ACS::RWlong_ptr    uBits();

    // -----------------------------------------------------------------------
    // IDL motion commands
    // -----------------------------------------------------------------------
    virtual void stop();
    virtual void up();
    virtual void down();
    virtual void move(CORBA::Long incr);
    virtual void refPos();
    virtual void stow();
    virtual void setup();
    virtual void top();
    virtual void bottom();

    // -----------------------------------------------------------------------
    // IDL hardware management commands
    // -----------------------------------------------------------------------
    virtual void reset();
    virtual void calibrate();
    virtual void calVer();
    virtual void writeCalibration(CORBA::Double_out cammaLenD, CORBA::Double_out cammaPosD, CORBA::Boolean_out calibrated);

    // -----------------------------------------------------------------------
    // IDL status
    // -----------------------------------------------------------------------
    virtual void readStatus();
    virtual void getStatus(ActiveSurface::USDStatus_out currentStatus);

protected:
    SmartPropertyPointer<ROlong>    m_actPos_sp;
    SmartPropertyPointer<ROpattern> m_status_sp;
    SmartPropertyPointer<ROlong>    m_softVer_sp;
    SmartPropertyPointer<ROlong>    m_type_sp;
    SmartPropertyPointer<RWlong>    m_cmdPos_sp;
    SmartPropertyPointer<RWlong>    m_Fmin_sp;
    SmartPropertyPointer<RWlong>    m_Fmax_sp;
    SmartPropertyPointer<RWlong>    m_acc_sp;
    SmartPropertyPointer<RWlong>    m_delay_sp;
    SmartPropertyPointer<RWlong>    m_uBits_sp;

private:
    /// The core object that owns all hardware logic and state.
    /// Obtained via USDCore::getInstance() — not owned by this component.
    ActiveSurface::USDCore* m_core;

    void operator=(const USDImpl&) = delete;
};

#endif // ACTIVE_SURFACE_USD_IMPL_H
