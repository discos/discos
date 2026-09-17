/**
 * ActiveSurfaceSocket.cpp
 */

#include "ActiveSurfaceSocket.hpp"

namespace ActiveSurface {

// ---------------------------------------------------------------------------
// Pacing constants
// ---------------------------------------------------------------------------

/// Conversion helpers
static constexpr unsigned long BITS_TO_US = 10000000UL;
static constexpr long USD_DELAY_STEP_US = 256;

/// Fixed safety margin added on top of a frame's theoretical wire
/// transmission time before the next command may be sent. Empirically
/// validated (19200 baud, converter Data Packing Time = 0): below ~1ms the
/// USD firmware's frame parser desynchronizes probabilistically; 2ms gives
/// a solid margin (0/900 failures across repeated statistical trials).
static constexpr long PACING_MARGIN_US = 2000;

/// Standard timeout for connection
static constexpr long DEFAULT_TIMEOUT_US = 500000;

/// Timeout for sending bytes to the kernel
static constexpr long SND_TIMEOUT_US = 10000;

/// Timeout margin for receiving answers
static constexpr long RCV_TIMEOUT_MARGIN_US = 5000;

// ---------------------------------------------------------------------------
// Static member definitions
// ---------------------------------------------------------------------------

std::mutex Socket::s_map_mutex;
std::map<unsigned int, std::shared_ptr<Socket>> Socket::s_map;

// ---------------------------------------------------------------------------
// Multiton access
// ---------------------------------------------------------------------------

Socket& Socket::getInstance(const std::string& lanComponentName, maci::ContainerServices* cs)
{
    // Extract numeric LAN id from component name (e.g. "AS/SECTOR01/LAN03" -> 3)
    const auto pos = lanComponentName.rfind("LAN");
    if (pos == std::string::npos)
    {
        _EXCPT(ASErrors::CDBAccessErrorExImpl, impl, "Socket::getInstance(name, cs)");
        impl.setFieldName("lanComponentName");
        throw impl;
    }
    const unsigned int id = static_cast<unsigned int>(std::atoi(lanComponentName.c_str() + pos + 3));

    // Fast path: if already registered, return existing instance
    {
        std::lock_guard<std::mutex> guard(s_map_mutex);
        auto it = s_map.find(id);
        if (it != s_map.end())
            return *it->second;
    }

    // Read network parameters from the LAN component's CDB node
    IRA::CString ip;
    DWORD port;
    DWORD baudRate;

    if (!IRA::CIRATools::getDBValue(cs, "IPAddress", ip, "alma/", lanComponentName.c_str()))
    {
        _EXCPT(ASErrors::CDBAccessErrorExImpl, impl, "Socket::getInstance(name, cs)");
        impl.setFieldName("IPAddress");
        throw impl;
    }

    if (!IRA::CIRATools::getDBValue(cs, "port", port, "alma/", lanComponentName.c_str()))
    {
        _EXCPT(ASErrors::CDBAccessErrorExImpl, impl, "Socket::getInstance(name, cs)");
        impl.setFieldName("port");
        throw impl;
    }

    if (!IRA::CIRATools::getDBValue(cs, "baudRate", baudRate, "alma/", lanComponentName.c_str()))
    {
        _EXCPT(ASErrors::CDBAccessErrorExImpl, impl, "Socket::getInstance(name, cs)");
        impl.setFieldName("baudRate");
        throw impl;
    }

    std::lock_guard<std::mutex> guard(s_map_mutex);

    // Double-check after acquiring logk
    auto it = s_map.find(id);
    if (it != s_map.end())
        return *it->second;

    std::shared_ptr<Socket> instance(new Socket(std::string(ip), static_cast<int>(port), static_cast<unsigned int>(baudRate)), Deleter{});
    s_map[id] = instance;
    return *instance;
}

// ---------------------------------------------------------------------------
// Constructor / destructor
// ---------------------------------------------------------------------------

Socket::Socket(const std::string& ip, int port, unsigned int baudRate) :
    m_ip(ip),
    m_port(port),
    m_baudRate(baudRate),
    m_status(SocketStatus::NOT_READY)
{
    try
    {
        connect();
    }
    catch (...)
    {
        // Connection failure is non-fatal at construction time.
        // send/receive will attempt reconnection transparently.
    }
}

Socket::~Socket()
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);
    IRA::CError tmp;
    Close(tmp);
}

// ---------------------------------------------------------------------------
// Connection
// ---------------------------------------------------------------------------

void Socket::connect()
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    if (isConnected())
        return;

    m_error.Reset();
    IRA::CError tmp;
    Close(tmp);

    if (Create(m_error, STREAM) == FAIL)
    {
        Close(tmp);
        _EXCPT(ASErrors::LANConnectionErrorExImpl, impl, "Socket::connect()");
        impl.setReason(("Cannot create socket for LAN " + m_ip + ":" + std::to_string(m_port)).c_str());
        throw impl;
    }

    if (setSockMode(m_error, BLOCKINGTIMEO, DEFAULT_TIMEOUT_US, SND_TIMEOUT_US) != SUCCESS)
    {
        m_status = SocketStatus::NOT_READY;
        Close(tmp);
        _EXCPT(ASErrors::LANConnectionErrorExImpl, impl, "Socket::connect()");
        impl.setReason("Cannot set socket to blocking-with-timeout mode.");
        throw impl;
    }

    if (Connect(m_error, m_ip.c_str(), m_port) == FAIL)
    {
        m_status = SocketStatus::TIMEOUT;
        Close(tmp);
        _EXCPT(ASErrors::LANConnectionErrorExImpl, impl, "Socket::connect()");
        impl.setReason(("Cannot connect to " + m_ip + ":" + std::to_string(m_port)).c_str());
        throw impl;
    }

    m_status = SocketStatus::READY;
}

bool Socket::isConnected() const
{
    return m_status == SocketStatus::READY;
}

// ---------------------------------------------------------------------------
// Communication
// ---------------------------------------------------------------------------

void Socket::rawSend(const std::vector<uint8_t>& frame)
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    connect(); // no-op if already connected, reconnects otherwise

    size_t sent = 0;
    while (sent < frame.size())
    {
        int res = Send(m_error,
                       reinterpret_cast<const char*>(frame.data()) + sent,
                       static_cast<DWORD>(frame.size() - sent));
        if (res > 0)
        {
            sent += static_cast<size_t>(res);
        }
        else if (res == WOULDBLOCK)
        {
            m_status = SocketStatus::TIMEOUT;
            IRA::CError tmp; Close(tmp);
            _EXCPT(ASErrors::SocketTOutExImpl, impl, "Socket::rawSend()");
            throw impl;
        }
        else
        {
            m_status = SocketStatus::NOT_READY;
            IRA::CError tmp; Close(tmp);
            _EXCPT(ASErrors::SocketFailExImpl, impl, "Socket::rawSend()");
            throw impl;
        }
    }
}

void Socket::send(const std::vector<uint8_t>& frame)
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    rawSend(frame);

    // Pacing: the USD firmware's frame parser relies on a minimum period of
    // serial-line silence between consecutive frames. Without this wait,
    // back-to-back fire-and-forget sends can desynchronize it (validated
    // empirically — see PACING_MARGIN_US comment above).
    const long tx_time_us = (frame.size() * BITS_TO_US) / m_baudRate;
    const long wait_us = tx_time_us + PACING_MARGIN_US;
    IRA::CIRATools::Wait(0, wait_us);
}

std::vector<uint8_t> Socket::receive(size_t n_bytes)
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    connect();

    std::vector<uint8_t> buf;
    buf.reserve(n_bytes);

    while (buf.size() < n_bytes)
    {
        char byte;
        int res = Receive(m_error, &byte, 1);

        if (res == 1)
        {
            buf.push_back(static_cast<uint8_t>(byte));
        }
        else if (res == WOULDBLOCK)
        {
            m_status = SocketStatus::TIMEOUT;
            IRA::CError tmp; Close(tmp);
            _EXCPT(ASErrors::SocketTOutExImpl, impl, "Socket::receive()");
            throw impl;
        }
        else if (res == 0)
        {
            m_status = SocketStatus::NOT_READY;
            IRA::CError tmp; Close(tmp);
            _EXCPT(ASErrors::SocketReconnExImpl, impl, "Socket::receive()");
            throw impl;
        }
        else
        {
            m_status = SocketStatus::NOT_READY;
            IRA::CError tmp; Close(tmp);
            _EXCPT(ASErrors::SocketFailExImpl, impl, "Socket::receive()");
            throw impl;
        }
    }

    return buf;
}

std::vector<uint8_t> Socket::sendReceive(const std::vector<uint8_t>& frame, size_t n_bytes, long usd_delay)
{
    // A single lock covers both send and receive, guaranteeing atomicity
    // of the full exchange on the half-duplex serial line. rawSend() is
    // used here (not the paced send()): the real round-trip time (tens of
    // ms) already far exceeds any pacing margin, so adding it would only
    // waste time.
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    const long wire_time_us = ((frame.size() + n_bytes) * BITS_TO_US) / m_baudRate;
    const long hw_delay_us = usd_delay * USD_DELAY_STEP_US;

    const long dynamic_timeout_us = wire_time_us + hw_delay_us + RCV_TIMEOUT_MARGIN_US;

    IRA::CError tmp;
    setSockMode(tmp, BLOCKINGTIMEO, dynamic_timeout_us, SND_TIMEOUT_US);

    rawSend(frame);
    std::vector<uint8_t> response = receive(n_bytes);
    setSockMode(tmp, BLOCKINGTIMEO, DEFAULT_TIMEOUT_US, SND_TIMEOUT_US);

    return response;
}

} // namespace ActiveSurface
