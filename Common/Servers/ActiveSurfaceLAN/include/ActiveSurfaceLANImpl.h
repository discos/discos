#ifndef ACTIVE_SURFACE_LAN_IMPL_H
#define ACTIVE_SURFACE_LAN_IMPL_H

/*******************************************************************************
 * OAC Osservatorio Astronomico di Cagliari
 *
 * This code is under GNU General Public Licence (GPL).
 *
 * Who                                          When    What
 * Giuseppe Maccaferri                          2005    Creation
 * Carlo Migoni (migoni@ca.astro.it)            2013    Revision
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)  2026    Redesign
 ******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciROboolean.h>
#include <baciSmartPropertyPointer.h>
#include <ComponentErrors.h>
#include <ASErrors.h>
#include <ActiveSurfaceCommonS.h>
#include <ActiveSurfaceLANS.h>
#include <IRA>
#include <ActiveSurfaceSocket.hpp>
#include <ActiveSurfaceCommandLibrary.hpp>
#include <ActiveSurfaceUSDCore.hpp>
#include <ActiveSurfaceCommon.h>
#include <string>


using namespace baci;
using namespace maci;

class LANImpl : public CharacteristicComponentImpl, public virtual POA_ActiveSurface::LAN
{
public:
    LANImpl(const ACE_CString& name, maci::ContainerServices* containerServices);
    virtual ~LANImpl();

    virtual void initialize();
    virtual void execute();
    virtual void cleanUp();
    virtual void aboutToAbort();

    // -----------------------------------------------------------------------
    // CORBA property accessor
    // -----------------------------------------------------------------------
    virtual ACS::ROboolean_ptr connected();

    // -----------------------------------------------------------------------
    // Tracking commands
    // -----------------------------------------------------------------------
    void update(CORBA::Double elevation, CORBA::Long tickIndex, CORBA::Boolean force);

    // -----------------------------------------------------------------------
    // IDL broadcast commands — control (rev. 1.2)
    // -----------------------------------------------------------------------
    virtual void reset();
    virtual void trigger();
    virtual void stop();
    virtual void setAbsolutePosition(CORBA::Long position);
    virtual void setRelativePosition(CORBA::Long offset);
    virtual void rotate(CORBA::Long direction);
    virtual void setVelocity(CORBA::Long velocity);

    // -----------------------------------------------------------------------
    // IDL broadcast commands — configuration (rev. 1.2)
    // -----------------------------------------------------------------------
    virtual void setMinFrequency(CORBA::Long frequency);
    virtual void setMaxFrequency(CORBA::Long frequency);
    virtual void setSlopeMultiplier(CORBA::Long multiplier);
    virtual void setReferencePosition(CORBA::Long position);
    virtual void setIoPins(CORBA::Long io_byte);
    virtual void setResolution(CORBA::Long res_byte);
    virtual void reduceCurrent(CORBA::Long standby_byte);
    virtual void setResponseDelay(CORBA::Long delay);
    virtual void setDelayedExecution(CORBA::Boolean enable);

    // -----------------------------------------------------------------------
    // IDL broadcast commands — rev. 1.3
    // -----------------------------------------------------------------------
    virtual void setStopIo(CORBA::Long config_byte);
    virtual void setPositioningIo(CORBA::Long config_byte);
    virtual void setHomeIo(CORBA::Long config_byte);
    virtual void setWorkingMode(CORBA::Long mode_byte);

    // -----------------------------------------------------------------------
    // IDL status
    // -----------------------------------------------------------------------
    virtual CORBA::Boolean getSlotStatus(CORBA::Long slot, ActiveSurface::USDStatus_out usdStatus);

    // -----------------------------------------------------------------------
    // LUT and profile management
    // -----------------------------------------------------------------------
    virtual void setLUT(const char* LUTName);

    virtual void setProfile(ActiveSurface::TASProfile profile);

protected:
    SmartPropertyPointer<ROboolean> m_connected_sp;

private:
    unsigned int   m_lanIndex;              // local LAN index within the sector (1-12)
    ActiveSurface::Socket* m_socket;        // shared socket (not owned)

    std::vector<ActiveSurface::USDCore*> m_usds;    // sorted list of USDs

    ActiveSurface::TASProfile m_profile;

    // Broadcast configuration parameters (read from CDB at initialize())
    long m_fmin;
    long m_fmax;
    long m_acc;
    long m_delay;
    long m_uBits;
    long m_rs;

    /**
     * Send a broadcast frame. Logs and swallows exceptions — broadcast
     * commands are oneway and must never propagate errors to the caller.
     */
    void broadcast(const std::vector<uint8_t>& frame, const char* routine);

    /**
     * Parses a string containing several floating point numbers separated by a comma, and returns them in a std::vector
     */
    static std::vector<double> parseCoeffs(const std::string s);

    /**
     * Load the Active Surface correction table for this LAN from the CDB
     * DataBlock. For now this only reads and logs each row; it does not
     * yet populate USDCore correction coefficients.
     */
    static void loadCorrectionTable(const std::string& compName, const std::string& lutName, maci::ContainerServices* cs);

    /**
     * Broadcast helper: writes the given value to the specified USDCore
     * member for every USD on this line, optionally mirroring it into the
     * corresponding USDStatus field as well. Keeps each USDCore's cached
     * parameter mirror in sync with what has just been sent to hardware.
     **/
    template<typename T, typename U = int>
    void updateAllUSDCache(T ActiveSurface::USDCore::* member, T value, U ActiveSurface::USDStatus::* statusMember = nullptr, U statusValue = U{});

    void operator=(const LANImpl&) = delete;
};

#endif // ACTIVE_SURFACE_LAN_IMPL_H
