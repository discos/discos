#ifndef ACTIVE_SURFACE_SOCKET_HPP
#define ACTIVE_SURFACE_SOCKET_HPP

/**
 * ActiveSurfaceSocket.hpp
 *
 * Multiton socket class for the Active Surface LAN serial converters.
 * Each LAN component (and its USD components) living in the same ACS container
 * share a single socket instance identified by a numeric LAN id.
 *
 * Usage:
 *   // In LAN component initialize() — creates and connects the socket:
 *   ActiveSurface::Socket& socket = ActiveSurface::Socket::getInstance(lan_id, ip, port);
 *
 *   // In USD component initialize() — retrieves the already-open socket:
 *   ActiveSurface::Socket& socket = ActiveSurface::Socket::getInstance(lan_id);
 *
 * Threading:
 *   - getInstance() is fully thread-safe via a static mutex on the outer map.
 *   - send() and receive() are serialized via a per-instance recursive mutex,
 *     which correctly models the half-duplex serial line constraint: only one
 *     command at a time can be in flight on a given LAN.
 *
 * Lifecycle:
 *   - The socket is created and connected on the first getInstance(id, ip, port) call.
 *   - Subsequent calls with the same id return the existing instance.
 *   - Calling getInstance(id, ip, port) with a different ip/port on an existing id throws.
 *   - The socket is destroyed when the static map is torn down at process exit.
 *     ACS containers have a well-defined shutdown sequence so this is safe.
 */

#include <IRA>
#include <mutex>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <ComponentErrors.h>
#include <ASErrors.h>


class SocketTestHelper;

namespace ActiveSurface {

static constexpr long USD_DEFAULT_DELAY = 3;

class Socket : public IRA::CSocket
{
    friend class ::SocketTestHelper;
public:
    // -----------------------------------------------------------------------
    // Multiton access
    // -----------------------------------------------------------------------

    /**
     * First-call factory: creates, connects and registers the socket instance,
     * reading the network configuration (IPAddress, port, baud rate) directly
     * from the CDB node of the specified LAN component.
     * Subsequent calls with the same id return the existing instance without
     * accessing the CDB again.
     * @param lanComponentName ACS component name of the LAN (e.g. "AS/SECTOR01/LAN03"),
     *                         used as the CDB node path to read network parameters.
     * @param cs               Container services of the calling component, used to
     *                         access the CDB.
     * @throw ASErrors::CDBAccessErrorExImpl if any network parameter cannot be read.
     * @throw ASErrors::ASErrorsEx if the connection attempt fails.
     * @return Reference to the Socket instance for the given id.
     */
    static Socket& getInstance(const std::string& lanComponentName, maci::ContainerServices* cs);

    // -----------------------------------------------------------------------
    // Communication
    // -----------------------------------------------------------------------

    /**
     * Send a binary frame on the socket, fire-and-forget: no response is
     * expected. Blocks until all bytes are sent or an error occurs, then
     * additionally blocks for a pacing delay derived from the frame's own
     * wire transmission time (frame_bytes * 10 / baud_rate) plus a fixed
     * safety margin, both empirically validated. This is necessary because
     * the USD firmware relies on a minimum period of serial-line silence to
     * correctly delimit consecutive frames; sending back-to-back without
     * this pacing can desynchronize its frame parser.
     * The call holds the per-instance mutex for its entire duration
     * (transmission + pacing wait), enforcing half-duplex serialization.
     * @param frame  Byte vector to transmit (complete protocol frame).
     * @throw ASErrors::ASErrorsEx on timeout or connection error.
     */
    void send(const std::vector<uint8_t>& frame);

    /**
     * Receive exactly n_bytes from the socket.
     * Blocks until all bytes are received or an error occurs.
     * Must be called after send() while the mutex is still held by the same thread
     * (recursive mutex), or independently when only a receive is needed.
     * @param n_bytes Number of bytes to receive.
     * @throw ASErrors::ASErrorsEx on timeout or connection error.
     * @return Received bytes as a vector.
     */
    std::vector<uint8_t> receive(size_t n_bytes);

    /**
     * Send a frame and receive the response in a single locked operation.
     * This is the preferred method for unicast commands that expect a reply,
     * as it guarantees atomicity of the send+receive pair on the serial line.
     * No pacing delay is applied here: the real network/hardware round-trip
     * time already far exceeds the pacing margin needed for fire-and-forget
     * sends, so adding it would only waste time.
     * @param frame    Byte vector to transmit.
     * @param n_bytes  Number of response bytes expected.
     * @throw ASErrors::ASErrorsEx on timeout or connection error.
     * @return Received response bytes.
     */
    std::vector<uint8_t> sendReceive(const std::vector<uint8_t>& frame, size_t n_bytes, long usd_delay = USD_DEFAULT_DELAY);

    /**
     * Check whether the socket is currently connected.
     * @return true if connected, false otherwise.
     */
    bool isConnected() const;

    // -----------------------------------------------------------------------
    // Non-copyable
    // -----------------------------------------------------------------------
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

private:
    // -----------------------------------------------------------------------
    // Lifecycle (private — only getInstance() may construct)
    // -----------------------------------------------------------------------

    Socket(const std::string& ip, int port, unsigned int baudRate);
    ~Socket();

    /**
     * (Re)connect to the remote endpoint.
     * Called from the constructor and transparently from send/receive on loss.
     * @throw ASErrors::ASErrorsEx if the connection attempt fails.
     */
    void connect();

    /**
     * Raw transmission with no pacing delay afterward. Used internally by
     * both send() (which adds pacing) and sendReceive() (which relies on
     * the real round-trip time instead).
     * @throw ASErrors::ASErrorsEx on timeout or connection error.
     */
    void rawSend(const std::vector<uint8_t>& frame);

    // -----------------------------------------------------------------------
    // Per-instance state
    // -----------------------------------------------------------------------

    std::string m_ip;
    int         m_port;
    unsigned int m_baudRate;    ///< serial line baud rate, read from CDB per LAN

    enum class SocketStatus { NOT_READY, TIMEOUT, READY } m_status;

    IRA::CError m_error;

    /// Recursive so that sendReceive() can call send() + receive() without deadlock.
    mutable std::recursive_mutex m_mutex;

    // -----------------------------------------------------------------------
    // Multiton registry (static)
    // -----------------------------------------------------------------------

    /// Protects s_map during getInstance() calls.
    static std::mutex s_map_mutex;

    /// One Socket instance per LAN id, shared across all components in the container.
    static std::map<unsigned int, std::shared_ptr<Socket>> s_map;

    /// Deleter declared as friend so shared_ptr can reach the private destructor.
    struct Deleter { void operator()(Socket* p) const { delete p; } };
    friend struct Deleter;
};

} // namespace ActiveSurface

#endif // ACTIVE_SURFACE_SOCKET_HPP
