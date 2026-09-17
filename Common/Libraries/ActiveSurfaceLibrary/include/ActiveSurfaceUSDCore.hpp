#ifndef ACTIVE_SURFACE_USD_CORE_H
#define ACTIVE_SURFACE_USD_CORE_H

/*********************************************************************/
/* OAC Osservatorio Astronomico di Cagliari                          */
/*                                                                   */
/* This code is under GNU General Public Licence (GPL).             */
/*                                                                   */
/* Who                                          when        what     */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)  2026        Creation */
/*********************************************************************/

/**
 * USDCore — hardware logic and state for a single USD50xxx/USD60xxx actuator.
 *
 * Multiton: one instance per USD, keyed by ACS component name.
 * Access always via getInstance().
 *
 * Initialization is deliberately separated from construction:
 *
 *   - The constructor only parses the name, reads the CDB and acquires the
 *     shared socket. No hardware communication takes place.
 *
 * This avoids redundant hardware setup when the LAN initializes the whole
 * line via broadcast before the individual USD components come up.
 */

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <vector>

#include <IRA>
#include <maciContainerServices.h>
#include <ASErrors.h>
#include <ComponentErrors.h>
#include <ActiveSurfaceCommonS.h>
#include <ActiveSurfaceSocket.hpp>
#include <ActiveSurfaceCommandLibrary.hpp>
#include "ActiveSurfaceCommon.h"

// Forward declaration
class LANImpl;

namespace ActiveSurface {

class USDCore
{
public:
    // -----------------------------------------------------------------------
    // Multiton access
    // -----------------------------------------------------------------------

    /**
     * Return the USDCore instance for the given USD component name.
     * On first call: constructs the instance and reads the CDB.
     * No hardware communication takes place here.
     * On subsequent calls: returns the existing instance immediately.
     *
     * @param usdName  ACS component name, e.g. "AS/SECTOR01/LAN01/USD03".
     * @param cs       ContainerServices of the calling component (LAN or USD).
     * @throw acsErrTypeLifeCycle::LifeCycleExImpl if a mandatory CDB field is missing.
     */
    static USDCore* getInstance(const std::string& usdName);
    static USDCore* getInstance(const std::string& usdName, maci::ContainerServices* cs);


    // -----------------------------------------------------------------------
    // Initialization
    // -----------------------------------------------------------------------

    void initialize();

    /**
     * Returns true if this USD has been successfully initialized.
     */
    bool isInitialized() const;

    bool needsReset();
    void sendUnicastConfig();
    void readHardwareDetails();
    void hardwareReset();

    // -----------------------------------------------------------------------
    // Tracking commands
    // -----------------------------------------------------------------------

    /**
     * Set the position-vs-elevation coefficients for this USD.
     * Pushed externally by the LAN component after loading the correction
     * table for the whole line — USDCore never reads these from the CDB
     * on its own, since tracking only makes sense at the full-surface level.
     *
     * @param coefficients Array of 6 coefficient vectors, in order:
     *                     [0] el15 — line coefficients (a0, a1, aN) for elevation section 15-30 deg
     *                     [1] el30 — line coefficients (a0, a1, aN) for elevation section 30-45 deg
     *                     [2] el45 — line coefficients (a0, a1, aN) for elevation section 45-60 deg
     *                     [3] el60 — line coefficients (a0, a1, aN) for elevation section 60-75 deg
     *                     [4] el75 — line coefficients (a0, a1, aN) for elevation section 75-90 deg
     * @param parabolicOffset double containing the parabolic offset of the given configuration
     */
    void setCoefficients(const std::array<std::vector<double>, 5>& coefficients, const double& parabolicOffset);

    /**
     * Compute the target position for this USD given the current antenna
     * elevation and the active profile, using the coefficients set via
     * setCoefficients(). Does NOT send anything to hardware — the caller
     * (LAN) is responsible for the unicast send and the broadcast trigger.
     *
     * @param elevation  Current antenna elevation in degrees.
     * @param profile    Active Surface profile (shaped, parabolic, park...).
     * @return Target position in 1/128 step units, clamped to [m_top, m_bottom].
     **/
    long computePosition(double elevation, ActiveSurface::TASProfile profile) const;

    // -----------------------------------------------------------------------
    // Hardware commands
    // -----------------------------------------------------------------------

    void stop();
    void up();
    void down();
    void move(int32_t incr);
    void refPos();
    void stow();
    void setup();
    void top();
    void bottom();

    void reset();
    void calibrate();
    void calVer();
    void writeCalibration(double& cammaLenD, double& cammaPosD, bool& calibrated);
    bool isReady() const;
    void setDelayedExecutionMode(bool enable);

    /**
     * Read actual position and hardware status from the physical device.
     * Updates actPos, hwStatus, and the USDStatus struct.
     */
    void readHWStatus();
    void readActPos();

    // -----------------------------------------------------------------------
    // State accessors
    // -----------------------------------------------------------------------

    long getCmdPos() const;
    void setCmdPos(long pos);

    long getActPos()   const;
    int  getHwStatus() const;
    long getSoftVer()  const;
    long getType()     const;

    long getFmin()  const;
    long getFmax()  const;
    long getAcc()   const;
    long getDelay() const;
    long getUBits() const;

    void setFmin(long v);
    void setFmax(long v);
    void setAcc(long v);
    void setDelay(long v);
    void setUBits(long v);

    bool isAvailable() const;

    ActiveSurface::USDStatus getUSDStatus() const;

    uint8_t addr() const { return m_addr; }

    // -----------------------------------------------------------------------
    // Non-copyable / non-movable
    // -----------------------------------------------------------------------
    USDCore(const USDCore&)            = delete;
    USDCore& operator=(const USDCore&) = delete;

private:
    USDCore(const std::string& usdName, maci::ContainerServices* cs);

    static bool parseName(const std::string& name, uint8_t& sector, uint8_t& lan, uint8_t& addr);

    template <typename T>
    T readCDB(const char* field);

    void restoreDefault(const char* propName, uint8_t cmd);

    void sendCommand(const std::vector<uint8_t>& frame);
    std::vector<uint8_t> sendReceive(const std::vector<uint8_t>& frame, size_t responseBytes);

    bool stillRunning(long targetPos);
    bool chkCal();

    // -----------------------------------------------------------------------
    // Per-instance identity
    // -----------------------------------------------------------------------
    std::string m_name;
    maci::ContainerServices* m_containerServices;
    uint8_t     m_sector;
    uint8_t     m_lanNum;
    uint8_t     m_addr;

    // -----------------------------------------------------------------------
    // Hardware configuration (from CDB)
    // -----------------------------------------------------------------------
    int     m_fullRange;
    int     m_zeroRef;
    int     m_step_giro;
    uint8_t m_rs;
    double  m_step_res;
    double  m_step2deg;
    long    m_top;
    long    m_bottom;
    int     m_cammaLen;
    int     m_cammaPos;
    double  m_cammaLenD;
    double  m_cammaPosD;
    bool    m_calibrate;

    // -----------------------------------------------------------------------
    // Runtime state
    // -----------------------------------------------------------------------
    long    m_cmdPos;
    long    m_actPos;
    long    m_softVer;
    long    m_type;
    int     m_hwStatus;
    int     m_lastCmdStep;

    long    m_fmin;
    long    m_fmax;
    long    m_acc;
    long    m_delay;
    long    m_uBits;

    ActiveSurface::USDStatus m_usdStatus;
    bool    m_available;
    bool    m_softwareInitialized;
    bool    m_initialized;

    std::array<std::vector<double>, 5> m_coefficients;
    double m_parabolicOffset;

    // -----------------------------------------------------------------------
    // Shared socket (not owned)
    // -----------------------------------------------------------------------
    ActiveSurface::Socket* m_socket;

    mutable std::mutex m_mutex;

    // -----------------------------------------------------------------------
    // Multiton registry
    // -----------------------------------------------------------------------
    static std::mutex s_map_mutex;
    static std::map<std::string, std::unique_ptr<USDCore>> s_instances;

    friend class ::LANImpl;
    friend std::default_delete<USDCore>;
};

} // namespace ActiveSurface

#endif // ACTIVE_SURFACE_USD_CORE_H
