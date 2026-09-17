/**
 * @file usd_command_library.hpp
 * @brief Header-only command composition library for USD50xxx/USD60xxx stepper drivers.
 *
 * Composes binary command frames according to the LAM Technologies serial protocol
 * (revisions 1.2 and 1.3). Each public function returns a std::vector<uint8_t>
 * representing a complete, ready-to-send frame including start byte, address/length
 * byte, command byte, parameters, and checksum.
 *
 * Protocol summary:
 *   Unicast frame:   [byte_start] [byte_nbyte_address] [cmd] [params...] [checksum]
 *   Broadcast frame: [byte_start] [0x00] [byte_nbyte] [cmd] [params...] [checksum]
 *
 * byte_nbyte_address packs two fields into one byte:
 *   bits [7:5] = number of bytes following this byte (excluding checksum)
 *   bits [4:0] = USD address (0-31)
 *
 * The checksum is the one's complement of the truncated sum of all preceding bytes.
 *
 * Usage:
 *   // Unicast: send soft_reset to USD at address 5
 *   auto frame = ActiveSurface::soft_reset(5);
 *
 *   // Broadcast to all USDs on the line
 *   auto frame = ActiveSurface::soft_reset();
 *
 *   // Use 0xFA start byte (no address in response)
 *   auto frame = ActiveSurface::soft_reset(5, false);
 *
 * @note All multi-byte parameters are encoded big-endian, as required by the protocol.
 * @note Positions are expressed in 1/128 of a step.
 * @note This library only composes command frames; it does not handle I/O or responses.
 *
 * Protocol references:
 *   - LAM Technologies Software Reference USD50xxx/USD60xxx ver 1.2
 *   - LAM Technologies Software Reference USD50xxx/USD60xxx ver 1.3
 **/

#ifndef USD_COMMAND_LIBRARY_HPP
#define USD_COMMAND_LIBRARY_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string>
#include "ActiveSurfaceCommon.h"

namespace ActiveSurface {
namespace CommandLibrary {

// ---------------------------------------------------------------------------
// Protocol constants
// ---------------------------------------------------------------------------

static const uint8_t BYTE_START_FC      = 0xFC; //< Start byte: response includes address
static const uint8_t BYTE_START_FA      = 0xFA; //< Start byte: response without address
static const uint8_t BYTE_SWITCHALL     = 0x00; //< Broadcast marker
static const uint8_t BYTE_ACK           = 0x06; //< Acknowledgement byte from driver
static const uint8_t BYTE_NAK           = 0x15; //< Negative acknowledgement from driver
static const int RESYNC_BYTE_COUNT      = 8;    // Number of null bytes to transmit for line resync after a timeout (Rev. 1.3).

// ---------------------------------------------------------------------------
// Internal helpers (not part of the public API)
// ---------------------------------------------------------------------------

namespace detail {

/// Compute the protocol checksum: one's complement of the truncated byte sum.
inline uint8_t checksum(const std::vector<uint8_t>& bytes)
{
    uint8_t sum = 0;
    for (uint8_t b : bytes)
        sum += b;                           // intentional 8-bit wraparound
    return ~sum;
}

/**
 * Encode a signed integer into big-endian bytes using two's complement.
 * Works correctly for n_bytes in {1, 2, 3, 4}.
 **/
inline std::vector<uint8_t> int_to_bytes_be(int32_t value, int n_bytes)
{
    std::vector<uint8_t> out(n_bytes);
    for (int i = n_bytes - 1; i >= 0; --i) {
        out[i] = static_cast<uint8_t>(value & 0xFF);
        value >>= 8;
    }
    return out;
}

/**
 * Core frame composer.
 *
 * @param address_on_response  true  → use 0xFC (recommended for multi-drop)
 *                             false → use 0xFA
 * @param usd_address          Target USD address [0, 31]. Pass -1 for broadcast.
 * @param cmd                  Command byte.
 * @param params               Optional parameter bytes (may be empty).
 * @return Complete frame including start byte and checksum.
 **/
inline std::vector<uint8_t> compose(
    bool address_on_response,
    int  usd_address,
    uint8_t cmd,
    const std::vector<uint8_t>& params = {})
{
    const uint8_t byte_start = address_on_response ? BYTE_START_FC : BYTE_START_FA;

    // payload = [cmd] + [params...]
    // "payload length" is what gets encoded in the length nibble/byte
    const uint8_t payload_len = static_cast<uint8_t>(1 + params.size());

    std::vector<uint8_t> frame;
    frame.reserve(4 + params.size()); // start + addr/len + cmd + params + checksum

    frame.push_back(byte_start);

    if (usd_address < 0) {
        // Broadcast: [0x00] [byte_nbyte] [cmd] [params...]
        frame.push_back(BYTE_SWITCHALL);
        frame.push_back(payload_len);
    } else {
        // Unicast: [byte_nbyte_address] encodes length in bits[7:5], address in bits[4:0]
        if (usd_address > 31)
            throw std::out_of_range(
                "USD address must be in [0, 31], got " + std::to_string(usd_address));
        if (payload_len > 7)
            throw std::length_error(
                "Payload too long: protocol supports at most 7 bytes after nbyte_address");

        const uint8_t nbyte_address =
            (static_cast<uint8_t>(payload_len) << 5) |
            (static_cast<uint8_t>(usd_address) & 0x1F);
        frame.push_back(nbyte_address);
    }

    frame.push_back(cmd);
    for (uint8_t p : params)
        frame.push_back(p);

    frame.push_back(checksum(frame));
    return frame;
}

} // namespace detail

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

/**
 * @defgroup getters Query commands (expect a response with data)
 * @{
 **/

/**
 * 0x10 — Request firmware version.
 * Response: 1 byte (nibble_high = major, nibble_low = minor).
 **/
inline std::vector<uint8_t> get_version(
    int  usd_address = -1,
    bool address_on_response = true)
{
    return detail::compose(address_on_response, usd_address, 0x10);
}

/**
 * 0x12 — Read current motor position.
 * Response: 4 bytes, signed 32-bit big-endian, in units of 1/128 step.
 **/
inline std::vector<uint8_t> get_position(
    int  usd_address = -1,
    bool address_on_response = true)
{
    return detail::compose(address_on_response, usd_address, 0x12);
}

/**
 * 0x13 — Read full driver status.
 * Response: 3 bytes (byte_par0 reserved, byte_par1 I/O state, byte_par2 flags).
 *
 * byte_par2 bit layout:
 *   bit7 Run/Stop   bit6 DifferExec   bit5 Ready   bit4 StandbyStatus
 *   bit3 ResAutoMan bit2 RS2          bit1 RS1      bit0 RS0
 **/
inline std::vector<uint8_t> get_status(
    int  usd_address = -1,
    bool address_on_response = true)
{
    return detail::compose(address_on_response, usd_address, 0x13);
}

/**
 * 0x14 — Request driver model code.
 * Response: 1 byte (0x20 = USD50xxx, 0x21 = USD60xxx).
 **/
inline std::vector<uint8_t> get_driver_type(
    int  usd_address = -1,
    bool address_on_response = true)
{
    return detail::compose(address_on_response, usd_address, 0x14);
}

/** @} **/

/**
 * @defgroup control Motion control commands
 * @{
 **/

/**
 * 0x01 — Software reset.
 * Restores factory defaults and stops the motor immediately.
 * Wait at least 100 ms after ACK before sending further commands.
 * Can be sent at any time, including during motion.
 **/
inline std::vector<uint8_t> soft_reset(
    int  usd_address = -1,
    bool address_on_response = true)
{
    return detail::compose(address_on_response, usd_address, 0x01);
}

/**
 * 0x02 — Software trigger.
 * Fires the deferred-execution trigger if a move command is buffered (Ready bit set).
 * Sending this as a broadcast achieves synchronised multi-axis starts.
 * Can be sent at any time.
 **/
inline std::vector<uint8_t> soft_trigger(
    int  usd_address = -1,
    bool address_on_response = true)
{
    return detail::compose(address_on_response, usd_address, 0x02);
}

/**
 * 0x11 — Immediate stop (with deceleration ramp).
 * The driver executes the programmed deceleration ramp; no steps are lost
 * provided ramp slopes are correctly configured. Ignored if motor is already still.
 * Can be sent at any time.
 **/
inline std::vector<uint8_t> soft_stop(
    int  usd_address = -1,
    bool address_on_response = true)
{
    return detail::compose(address_on_response, usd_address, 0x11);
}

/**
 * 0x30 — Move to absolute position (relative to home/zero).
 * Motor travels the difference between the requested position and its current position.
 * No movement occurs if the difference is zero.
 * @param position  Target position in 1/128 step units [-2147483648, 2147483647].
 * @note Motor must be stopped; NAK is returned otherwise.
 **/
inline std::vector<uint8_t> set_absolute_position(
    int32_t position,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x30,
        detail::int_to_bytes_be(position, 4));
}

/**
 * 0x31 — Move by a relative offset from the current position.
 * Direction is determined by the sign of the offset.
 * @param offset  Displacement in 1/128 step units [-2147483648, 2147483647].
 * @note Motor must be stopped; NAK is returned otherwise.
 **/
inline std::vector<uint8_t> set_relative_position(
    int32_t offset,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x31,
        detail::int_to_bytes_be(offset, 4));
}

/**
 * 0x32 — Continuous rotation (infinite move).
 * Direction is given by the sign of @p direction. Stop with soft_stop().
 * @param direction  Any positive value → CW; any negative value → CCW.
 * @note Motor must be stopped; NAK is returned otherwise.
 **/
inline std::vector<uint8_t> rotate(
    int8_t direction,
    int    usd_address = -1,
    bool   address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x32,
        {static_cast<uint8_t>(direction)});
}

/**
 * 0x35 — Velocity reference (open-loop speed control).
 * Unlike positioning commands, no ramp is generated internally — the caller
 * is responsible for ramp management. Send 0 to stop (standby current applied
 * after time_standby). Sign determines direction.
 * @param velocity  Speed in tenths of Hz [-100000, 100000].
 *                  With fixed micro-stepping, values below ±10 are not accepted.
 * Can be sent while the motor is running (if and only if this command started it).
 **/
inline std::vector<uint8_t> set_velocity(
    int32_t velocity,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    if (velocity < -100000 || velocity > 100000)
        throw std::out_of_range(
            "velocity must be in [-100000, 100000] (tenths of Hz)");
    return detail::compose(
        address_on_response, usd_address, 0x35,
        detail::int_to_bytes_be(velocity, 3));
}

/** @} **/

/**
 * @defgroup config Configuration commands
 * @{
 **/

/**
 * 0x20 — Set minimum (start) frequency for the acceleration ramp.
 * @param frequency  Start frequency in Hz [20, 10000].
 * Takes effect at the next move if sent during motion.
 **/
inline std::vector<uint8_t> set_min_frequency(
    uint16_t frequency,
    int      usd_address = -1,
    bool     address_on_response = true)
{
    if (frequency < 20 || frequency > 10000)
        throw std::out_of_range("min_frequency must be in [20, 10000] Hz");
    return detail::compose(
        address_on_response, usd_address, 0x20,
        detail::int_to_bytes_be(frequency, 2));
}

/**
 * 0x21 — Set maximum (top) frequency for the acceleration ramp.
 * @param frequency  Top frequency in Hz [20, 10000].
 * Takes effect at the next move if sent during motion.
 **/
inline std::vector<uint8_t> set_max_frequency(
    uint16_t frequency,
    int      usd_address = -1,
    bool     address_on_response = true)
{
    if (frequency < 20 || frequency > 10000)
        throw std::out_of_range("max_frequency must be in [20, 10000] Hz");
    return detail::compose(
        address_on_response, usd_address, 0x21,
        detail::int_to_bytes_be(frequency, 2));
}

/**
 * 0x22 — Set ramp slope multiplier.
 * A multiplier of 0 gives the steepest ramp (20→1000 Hz in 10 ms).
 * A multiplier of 50 gives approx. 510 ms to reach 1 kHz.
 * @param multiplier  Slope multiplication factor [0, 255].
 * Takes effect at the next move if sent during motion.
 **/
inline std::vector<uint8_t> set_slope_multiplier(
    uint8_t multiplier,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x22,
        {multiplier});
}

/**
 * 0x23 — Set absolute position reference (define home).
 * Associates the given value with the motor's current physical position.
 * Useful for calibrating the logical zero (home) offset.
 * @param position  Value to assign to current position, in 1/128 step units.
 * @note Motor must be stopped; NAK is returned otherwise.
 **/
inline std::vector<uint8_t> set_reference_position(
    int32_t position,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x23,
        detail::int_to_bytes_be(position, 4));
}

/**
 * 0x25 — Configure I/O pin directions and output values.
 * Byte layout: bits[6:4] = DIO2..DIO0 (1=output), bits[2:0] = IO2..IO0 value.
 * @param io_byte  Packed direction+value byte as per protocol spec.
 * Can be sent at any time.
 **/
inline std::vector<uint8_t> set_io_pins(
    uint8_t io_byte,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x25,
        {io_byte});
}

/**
 * 0x26 — Set micro-stepping resolution and auto-resolution mode.
 * Byte layout: bit3 = AutoMan (1=auto), bits[2:0] = RS2..RS0 (fixed resolution).
 * Resolution table (RS2 RS1 RS0): 000=full, 001=1/2, 010=1/4 ... 111=1/128.
 * @param res_byte  Packed resolution byte as per protocol spec.
 * Takes effect at the next move if sent during motion.
 **/
inline std::vector<uint8_t> set_resolution(
    uint8_t res_byte,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x26,
        {res_byte});
}

/**
 * 0x27 — Configure standby current reduction.
 * Byte layout: bits[7:6] = StandbyMode (00/01=0%, 10=25%, 11=50%),
 *              bits[5:0] = time_standby (×4096 µs after motor stop).
 * @param standby_byte  Packed standby configuration byte.
 * Takes effect at the next stop if sent during motion.
 **/
inline std::vector<uint8_t> reduce_current(
    uint8_t standby_byte,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x27,
        {standby_byte});
}

/**
 * 0x28 — Set response delay.
 * Controls the delay between command reception and the driver's reply.
 * Base unit: 512 µs. Value 0 = immediate reply. Value 255 = no reply.
 * Useful for RS-485 half-duplex line-turnaround timing.
 * @param delay  Delay value [0, 255].
 * Takes effect immediately.
 **/
inline std::vector<uint8_t> set_response_delay(
    uint8_t delay,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x28,
        {delay});
}

/**
 * 0x29 — Configure deferred execution mode and trigger I/O lines.
 * bit7 = DifferEnable. bits[5:3] = I/Ox active level. bits[2:0] = I/Ox enable.
 * When enabled, motion commands are buffered until the TRIGGER event fires.
 * Combined with broadcast soft_trigger() this achieves synchronised starts.
 * @param config_byte  Packed configuration byte.
 * Can be sent at any time; sending it clears any pending buffered command.
 **/
inline std::vector<uint8_t> toggle_delayed_execution(
    bool    enable,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    uint8_t config_byte = enable ? 0x80 : 0x00;
    return detail::compose(
        address_on_response, usd_address, 0x29,
        {config_byte});
}

/**
 * 0x2A — Configure hardware immediate-stop inputs (Rev. 1.3).
 * bits[5:3] = active level for I/Ox. bits[2:0] = I/Ox enable.
 * The motor executes the deceleration ramp when the condition is met.
 * Useful for limit switches and emergency signals.
 * @param config_byte  Packed I/O configuration byte [0, 63].
 * Can be sent at any time, including during motion.
 **/
inline std::vector<uint8_t> set_stop_io(
    uint8_t config_byte,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    if (config_byte > 63)
        throw std::out_of_range("set_stop_io config_byte must be in [0, 63]");
    return detail::compose(
        address_on_response, usd_address, 0x2A,
        {config_byte});
}

/**
 * 0x2B — Configure "in-position" output signals (Rev. 1.3).
 * bits[5:3] = active level for I/Ox. bits[2:0] = I/Ox enable.
 * Selected I/O lines go to active level when the target position is reached
 * (or after an early stop), and to the opposite level while moving.
 * @param config_byte  Packed I/O configuration byte [0, 63].
 * Can be sent at any time, including during motion.
 **/
inline std::vector<uint8_t> set_positioning_io(
    uint8_t config_byte,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    if (config_byte > 63)
        throw std::out_of_range("set_positioning_io config_byte must be in [0, 63]");
    return detail::compose(
        address_on_response, usd_address, 0x2B,
        {config_byte});
}

/**
 * 0x2C — Configure home-sensor inputs (Rev. 1.3).
 * bits[5:3] = active level for I/Ox. bits[2:0] = I/Ox enable.
 * When the enabled lines match the active level, the internal position counter
 * is zeroed and the deceleration ramp is started.
 * Set bits[2:0] = 0 to disable the function.
 * @param config_byte  Packed I/O configuration byte [0, 63].
 * Can be sent at any time, including during motion.
 **/
inline std::vector<uint8_t> set_home_io(
    uint8_t config_byte,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    if (config_byte > 63)
        throw std::out_of_range("set_home_io config_byte must be in [0, 63]");
    return detail::compose(
        address_on_response, usd_address, 0x2C,
        {config_byte});
}

/**
 * 0x2D — Set driver working mode options (Rev. 1.3).
 * byte_par0 bit0 (BAUD0): 0 = 9600 baud, 1 = 19200 baud.
 * byte_par1 is reserved and must be 0 (added automatically).
 *
 * @warning When changing baud rate, the driver replies at the NEW rate.
 *          Send as broadcast to avoid receiving the reply, then verify
 *          each driver individually at the new rate.
 * @param mode_byte  Options byte (currently only bit0 is defined).
 * Takes effect immediately.
 **/
inline std::vector<uint8_t> set_working_mode(
    uint8_t mode_byte,
    int     usd_address = -1,
    bool    address_on_response = true)
{
    return detail::compose(
        address_on_response, usd_address, 0x2D,
        {mode_byte, 0x00}); // byte_par1 reserved, must be 0
}

/** @} **/

/**
 * @defgroup utility Utility helpers
 * @{
 **/

/**
 * Returns the line-resync sequence (8 × 0x00) required by Rev. 1.3 after a timeout.
 * Transmit this before the next command to guarantee the driver is back in sync.
 **/
inline std::vector<uint8_t> resync_sequence()
{
    return std::vector<uint8_t>(RESYNC_BYTE_COUNT, 0x00);
}

/** @} **/


/**
 * @defgroup response Response parsing
 * @{
 **/

/**
 * Parsed response from a unicast read command.
 *
 * Unicast response frame layout (0xFC start byte):
 *   [0xFC] [nbyte_addr] [params...] [checksum]
 *
 * Where nbyte_addr encodes:
 *   bits [7:5] = number of bytes following this byte (excluding checksum)
 *   bits [4:0] = USD address that replied
 **/
struct Response
{
    bool                  valid;    ///< false if frame is malformed or checksum fails
    uint8_t               address;  ///< USD address extracted from nbyte_addr field
    std::vector<uint8_t>  params;   ///< parameter bytes (payload, without framing or checksum)
};

/**
 * Parse a raw unicast response frame received from the hardware.
 *
 * Validates the start byte, verifies the checksum and extracts the
 * parameter payload. Always check Response::valid before using params.
 *
 * @param raw  Complete raw bytes as received from the socket,
 *             including start byte and trailing checksum.
 * @return     Parsed response. If valid is false, params is empty.
 **/
inline Response parse_response(const std::vector<uint8_t>& raw)
{
    Response result{false, 0, {}};
    
    if (raw.size() < 3)
       return result;
    
    // Verify checksum on the full raw frame (ACK included if present)
    const uint8_t expected = detail::checksum(std::vector<uint8_t>(raw.begin(), raw.end() - 1));

    if (raw.back() != expected)
       return result;
    
    // Strip leading ACK if present, then parse the frame
    size_t offset = 0;
    if (raw[0] == BYTE_ACK)
       offset = 1;
    
    if (raw.size() - offset < 3)
       return result;
    
    if (raw[offset] != BYTE_START_FC && raw[offset] != BYTE_START_FA)
       return result;
    
    result.address = raw[offset + 1] & 0x1F;
    
    if (raw.size() - offset > 3)
       result.params.assign(raw.begin() + offset + 2, raw.end() - 1);
    
    result.valid = true;
    return result;
}

/**
 * Extract a signed big-endian integer from a Response parameter payload.
 *
 * Reads @p length bytes starting at @p offset within @p params and
 * interprets them as a signed big-endian integer.
 *
 * @param params  Parameter bytes from a parsed Response.
 * @param offset  Byte offset within params (default 0).
 * @param length  Number of bytes to read: 1, 2, or 4 (default 4).
 * @return        Signed 32-bit integer value.
 * @throw std::out_of_range if offset + length exceeds params size.
 * @throw std::invalid_argument if length is not 1, 2 or 4.
 **/
inline int32_t params_to_int(const std::vector<uint8_t>& params,
                              size_t offset = 0,
                              size_t length = 4)
{
    if (length != 1 && length != 2 && length != 4)
        throw std::invalid_argument(
            "params_to_int: length must be 1, 2 or 4, got " + std::to_string(length));
    if (offset + length > params.size())
        throw std::out_of_range(
            "params_to_int: offset " + std::to_string(offset) +
            " + length " + std::to_string(length) +
            " exceeds params size " + std::to_string(params.size()));

    int32_t value = 0;
    for (size_t i = 0; i < length; ++i)
        value = (value << 8) | params[offset + i];

    // Sign-extend for multi-byte values
    if (length == 1) return static_cast<int8_t>(value);
    if (length == 2) return static_cast<int16_t>(value);
    return value;
}

/**
 * Extract an unsigned big-endian integer from a Response parameter payload.
 *
 * Same as params_to_int but returns an unsigned value — suitable for
 * status words and bit patterns.
 *
 * @param params  Parameter bytes from a parsed Response.
 * @param offset  Byte offset within params (default 0).
 * @param length  Number of bytes to read (default 3 — status word).
 * @return        Unsigned 32-bit integer value.
 * @throw std::out_of_range if offset + length exceeds params size.
 **/
inline uint32_t params_to_uint(const std::vector<uint8_t>& params,
                                size_t offset = 0,
                                size_t length = 3)
{
    if (offset + length > params.size())
        throw std::out_of_range(
            "params_to_uint: offset " + std::to_string(offset) +
            " + length " + std::to_string(length) +
            " exceeds params size " + std::to_string(params.size()));

    uint32_t value = 0;
    for (size_t i = 0; i < length; ++i)
        value = (value << 8) | params[offset + i];
    return value;
}

/**
 * 0x30 — Move to absolute position (relative to home/zero), expressed in
 * whole steps instead of raw 1/128-step units. Internally converts to
 * microsteps (position_steps << 7) before composing the frame — see
 * set_absolute_position() for the raw microstep variant and full command
 * semantics.
 * @param position_steps  Target position in whole steps.
 * @note Motor must be stopped; NAK is returned otherwise.
 **/
inline std::vector<uint8_t> set_absolute_position_steps(int32_t position_steps, int usd_address = -1, bool address_on_response = true)
{
    return set_absolute_position(position_steps << USxS, usd_address, address_on_response);
}

/**
 * 0x31 — Move by a relative offset from the current position, expressed in
 * whole steps instead of raw 1/128-step units. Internally converts to
 * microsteps (offset_steps << 7) before composing the frame — see
 * set_relative_position() for the raw microstep variant and full command
 * semantics.
 * @param offset_steps  Displacement in whole steps.
 * @note Motor must be stopped; NAK is returned otherwise.
 **/
inline std::vector<uint8_t> set_relative_position_steps(int32_t offset_steps, int usd_address = -1, bool address_on_response = true)
{
    return set_relative_position(offset_steps << USxS, usd_address, address_on_response);
}

/**
 * 0x23 — Set absolute position reference (define home), expressed in
 * whole steps instead of raw 1/128-step units. Internally converts to
 * microsteps (position_steps << 7) before composing the frame — see
 * set_reference_position() for the raw microstep variant and full command
 * semantics.
 * @param position_steps  Value to assign to current position, in whole steps.
 * @note Motor must be stopped; NAK is returned otherwise.
 **/
inline std::vector<uint8_t> set_reference_position_steps(int32_t position_steps, int usd_address = -1, bool address_on_response = true)
{
    return set_reference_position(position_steps << USxS, usd_address, address_on_response);
}

/**
 * Convert a raw position value (1/128-step units, as returned by
 * get_position() responses) into whole steps. Unlike the rest of this
 * library, this function does not build a command — it is meant to be
 * called by the reading side (typically after parse_response() +
 * params_to_int() on a get_position() reply), to convert the raw
 * microstep value back into the same whole-step unit used by
 * set_absolute_position_steps()/set_relative_position_steps().
 * @param microsteps  Raw position value in 1/128-step units.
 * @return            Equivalent position in whole steps (truncated toward zero).
 **/
inline int32_t microsteps_to_steps(int32_t microsteps)
{
    return microsteps >> 7;
}

} // namespace CommandLibrary
} // namespace ActiveSurface

#endif // USD_COMMAND_LIBRARY_HPP
