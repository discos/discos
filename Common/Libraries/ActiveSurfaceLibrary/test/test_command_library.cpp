/**
 * @file unittest.cpp
 * @brief GTest suite for CommandLibrary::usd_command_library.
 *
 * Test strategy:
 *   Each test validates the exact byte sequence produced by a command function
 *   against a known-good reference frame computed by hand from the protocol spec,
 *   or verifies structural invariants (checksum, frame layout, exception behaviour).
 *
 * Reference frames are cross-checked against:
 *   - The checksum example in the protocol appendix (Rev. 1.2)
 *   - The doctest expected values in the original Python command_library.py
 *
 * Frame anatomy reminder:
 *   Unicast  (0xFC): [0xFC] [len<<5|addr] [cmd] [params...] [~sum]
 *   Broadcast(0xFC): [0xFC] [0x00]        [len] [cmd] [params...] [~sum]
 *   Unicast  (0xFA): [0xFA] [len<<5|addr] [cmd] [params...] [~sum]
 *   Broadcast(0xFA): [0xFA] [0x00]        [len] [cmd] [params...] [~sum]
 *
 * Broadcast frame index map (used in structural tests):
 *   [0] byte_start   [1] 0x00   [2] len   [3] cmd   [4..N-2] params   [N-1] checksum
 */

#include "gtest/gtest.h"
#include "ActiveSurfaceCommandLibrary.hpp"
#include <cstdint>
#include <vector>

using namespace ActiveSurface;

using Bytes = std::vector<uint8_t>;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::string hex_dump(const Bytes& v)
{
    std::string out;
    char buf[6];
    for (uint8_t b : v) {
        snprintf(buf, sizeof(buf), "\\x%02x", b);
        out += buf;
    }
    return out;
}

#define ASSERT_FRAME_EQ(got, expected) \
    ASSERT_EQ((got), (expected)) \
        << "got:      " << hex_dump(got) << "\n" \
        << "expected: " << hex_dump(expected)

static bool checksum_valid(const Bytes& frame)
{
    uint8_t sum = 0;
    for (uint8_t b : frame) sum += b;
    return sum == 0xFF;
}

// ===========================================================================
// Protocol appendix cross-check
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, AppendixExample_SetMaxFrequency_Addr22_3600Hz)
{
    // Protocol appendix (Rev. 1.2 pp.31-33): addr=22, freq=3600 Hz.
    // NOTE: the spec lists 0xB1 as checksum, but that is the sum itself;
    // the one's complement (correct checksum) is 0x4E. Our implementation is correct.
    const Bytes expected = {0xFC, 0x76, 0x21, 0x0E, 0x10, 0x4E};
    ASSERT_FRAME_EQ(CommandLibrary::set_max_frequency(3600, 22), expected);
}

// ===========================================================================
// Checksum invariant
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, ChecksumIsOnesComplementOfSum)
{
    EXPECT_TRUE(checksum_valid(CommandLibrary::soft_reset(5)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::get_position(12)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_absolute_position(100000, 7)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_velocity(-5000, 3)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::soft_reset()));
}

// ===========================================================================
// soft_reset  (0x01)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SoftReset_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x01, 0x01, 0x01};
    ASSERT_FRAME_EQ(CommandLibrary::soft_reset(), expected);
}

TEST(ActiveSurfaceCommandLibrary, SoftReset_Broadcast_FA)
{
    const Bytes expected = {0xFA, 0x00, 0x01, 0x01, 0x03};
    ASSERT_FRAME_EQ(CommandLibrary::soft_reset(-1, false), expected);
}

TEST(ActiveSurfaceCommandLibrary, SoftReset_Unicast_Addr1_FC)
{
    // len=1 → 001, addr=1 → 00001 → byte_nbyte_address = 0x21
    const Bytes expected = {0xFC, 0x21, 0x01, 0xE1};
    ASSERT_FRAME_EQ(CommandLibrary::soft_reset(1), expected);
}

// ===========================================================================
// soft_trigger  (0x02)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SoftTrigger_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x01, 0x02, 0x00};
    ASSERT_FRAME_EQ(CommandLibrary::soft_trigger(), expected);
}

TEST(ActiveSurfaceCommandLibrary, SoftTrigger_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x21, 0x02, 0xE0};
    ASSERT_FRAME_EQ(CommandLibrary::soft_trigger(1), expected);
}

// ===========================================================================
// get_version  (0x10)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, GetVersion_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x01, 0x10, 0xF2};
    ASSERT_FRAME_EQ(CommandLibrary::get_version(), expected);
}

TEST(ActiveSurfaceCommandLibrary, GetVersion_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x21, 0x10, 0xD2};
    ASSERT_FRAME_EQ(CommandLibrary::get_version(1), expected);
}

// ===========================================================================
// soft_stop  (0x11)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SoftStop_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x01, 0x11, 0xF1};
    ASSERT_FRAME_EQ(CommandLibrary::soft_stop(), expected);
}

TEST(ActiveSurfaceCommandLibrary, SoftStop_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x21, 0x11, 0xD1};
    ASSERT_FRAME_EQ(CommandLibrary::soft_stop(1), expected);
}

// ===========================================================================
// get_position  (0x12)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, GetPosition_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x01, 0x12, 0xF0};
    ASSERT_FRAME_EQ(CommandLibrary::get_position(), expected);
}

TEST(ActiveSurfaceCommandLibrary, GetPosition_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x21, 0x12, 0xD0};
    ASSERT_FRAME_EQ(CommandLibrary::get_position(1), expected);
}

// ===========================================================================
// get_status  (0x13)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, GetStatus_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x01, 0x13, 0xEF};
    ASSERT_FRAME_EQ(CommandLibrary::get_status(), expected);
}

TEST(ActiveSurfaceCommandLibrary, GetStatus_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x21, 0x13, 0xCF};
    ASSERT_FRAME_EQ(CommandLibrary::get_status(1), expected);
}

// ===========================================================================
// get_driver_type  (0x14)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, GetDriverType_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x01, 0x14, 0xEE};
    ASSERT_FRAME_EQ(CommandLibrary::get_driver_type(), expected);
}

TEST(ActiveSurfaceCommandLibrary, GetDriverType_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x21, 0x14, 0xCE};
    ASSERT_FRAME_EQ(CommandLibrary::get_driver_type(1), expected);
}

// ===========================================================================
// set_min_frequency  (0x20)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetMinFrequency_Broadcast_FC)
{
    // freq=20: FC 00 03 20 00 14 checksum → ~(FC+00+03+20+00+14)%256 = ~33 = CC
    const Bytes expected = {0xFC, 0x00, 0x03, 0x20, 0x00, 0x14, 0xCC};
    ASSERT_FRAME_EQ(CommandLibrary::set_min_frequency(20), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetMinFrequency_Unicast_Addr1_FC)
{
    // freq=20: FC 61 20 00 14 checksum → ~(FC+61+20+00+14)%256 = ~91 = 6E
    const Bytes expected = {0xFC, 0x61, 0x20, 0x00, 0x14, 0x6E};
    ASSERT_FRAME_EQ(CommandLibrary::set_min_frequency(20, 1), expected);
}

// ===========================================================================
// set_max_frequency  (0x21)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetMaxFrequency_Unicast_Addr1_FC)
{
    // freq=20: FC 61 21 00 14 checksum → ~(FC+61+21+00+14)%256 = ~92 = 6D
    const Bytes expected = {0xFC, 0x61, 0x21, 0x00, 0x14, 0x6D};
    ASSERT_FRAME_EQ(CommandLibrary::set_max_frequency(20, 1), expected);
}

// ===========================================================================
// set_slope_multiplier  (0x22)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetSlopeMultiplier_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x22, 0x01, 0xDE};
    ASSERT_FRAME_EQ(CommandLibrary::set_slope_multiplier(1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetSlopeMultiplier_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x41, 0x22, 0x01, 0x9F};
    ASSERT_FRAME_EQ(CommandLibrary::set_slope_multiplier(1, 1), expected);
}

// ===========================================================================
// set_reference_position  (0x23)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetReferencePosition_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x05, 0x23, 0x00, 0x00, 0x00, 0x01, 0xDA};
    ASSERT_FRAME_EQ(CommandLibrary::set_reference_position(1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetReferencePosition_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0xA1, 0x23, 0x00, 0x00, 0x00, 0x01, 0x3E};
    ASSERT_FRAME_EQ(CommandLibrary::set_reference_position(1, 1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetReferencePosition_NegativeValue)
{
    // -1 → 0xFFFFFFFF big-endian
    const Bytes expected = {0xFC, 0x00, 0x05, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF};
    ASSERT_FRAME_EQ(CommandLibrary::set_reference_position(-1), expected);
}

// ===========================================================================
// set_io_pins  (0x25)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetIoPins_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x25, 0x00, 0xDC};
    ASSERT_FRAME_EQ(CommandLibrary::set_io_pins(0x00), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetIoPins_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x41, 0x25, 0x00, 0x9D};
    ASSERT_FRAME_EQ(CommandLibrary::set_io_pins(0x00, 1), expected);
}

// ===========================================================================
// set_resolution  (0x26)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetResolution_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x26, 0x01, 0xDA};
    ASSERT_FRAME_EQ(CommandLibrary::set_resolution(0x01), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetResolution_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x41, 0x26, 0x01, 0x9B};
    ASSERT_FRAME_EQ(CommandLibrary::set_resolution(0x01, 1), expected);
}

// ===========================================================================
// reduce_current  (0x27)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, ReduceCurrent_Broadcast_FC)
{
    // Broadcast layout: [0]=FC [1]=00 [2]=len [3]=cmd [4]=param [5]=ck
    const Bytes expected = {0xFC, 0x00, 0x02, 0x27, 0x00, 0xDA};
    ASSERT_FRAME_EQ(CommandLibrary::reduce_current(0x00), expected);
}

TEST(ActiveSurfaceCommandLibrary, ReduceCurrent_Broadcast_FC_CmdBytePosition)
{
    auto frame = CommandLibrary::reduce_current(0x00);
    EXPECT_TRUE(checksum_valid(frame));
    EXPECT_EQ(frame[3], 0x27);  // cmd is at index 3 in broadcast frames
}

// ===========================================================================
// set_response_delay  (0x28)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetResponseDelay_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x28, 0x05, 0xD4};
    ASSERT_FRAME_EQ(CommandLibrary::set_response_delay(5), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetResponseDelay_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x41, 0x28, 0x05, 0x95};
    ASSERT_FRAME_EQ(CommandLibrary::set_response_delay(5, 1), expected);
}

// ===========================================================================
// toggle_delayed_execution  (0x29)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, ToggleDelayedExecution_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x29, 0x80, 0x58};
    ASSERT_FRAME_EQ(CommandLibrary::toggle_delayed_execution(true), expected);
}

TEST(ActiveSurfaceCommandLibrary, ToggleDelayedExecution_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x41, 0x29, 0x80, 0x19};
    ASSERT_FRAME_EQ(CommandLibrary::toggle_delayed_execution(true, 1), expected);
}

// ===========================================================================
// set_absolute_position  (0x30)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetAbsolutePosition_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x05, 0x30, 0x00, 0x00, 0x00, 0x01, 0xCD};
    ASSERT_FRAME_EQ(CommandLibrary::set_absolute_position(1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetAbsolutePosition_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0xA1, 0x30, 0x00, 0x00, 0x00, 0x01, 0x31};
    ASSERT_FRAME_EQ(CommandLibrary::set_absolute_position(1, 1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetAbsolutePosition_NegativeValue)
{
    // -1 → 0xFFFFFFFF big-endian
    const Bytes expected = {0xFC, 0x00, 0x05, 0x30, 0xFF, 0xFF, 0xFF, 0xFF, 0xD2};
    ASSERT_FRAME_EQ(CommandLibrary::set_absolute_position(-1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetAbsolutePosition_MaxPositive)
{
    // 2147483647 = 0x7FFFFFFF
    // Broadcast: [0]=FC [1]=00 [2]=05 [3]=30(cmd) [4..7]=params [8]=ck
    auto frame = CommandLibrary::set_absolute_position(2147483647);
    EXPECT_EQ(frame[4], 0x7F);
    EXPECT_EQ(frame[5], 0xFF);
    EXPECT_EQ(frame[6], 0xFF);
    EXPECT_EQ(frame[7], 0xFF);
    EXPECT_TRUE(checksum_valid(frame));
}

TEST(ActiveSurfaceCommandLibrary, SetAbsolutePosition_MaxNegative)
{
    // -2147483648 = 0x80000000
    auto frame = CommandLibrary::set_absolute_position(-2147483648);
    EXPECT_EQ(frame[4], 0x80);
    EXPECT_EQ(frame[5], 0x00);
    EXPECT_EQ(frame[6], 0x00);
    EXPECT_EQ(frame[7], 0x00);
    EXPECT_TRUE(checksum_valid(frame));
}

// ===========================================================================
// set_relative_position  (0x31)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetRelativePosition_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x05, 0x31, 0x00, 0x00, 0x00, 0x01, 0xCC};
    ASSERT_FRAME_EQ(CommandLibrary::set_relative_position(1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetRelativePosition_NegativeOffset)
{
    // -1 → 0xFFFFFFFF
    // Broadcast: [0]=FC [1]=00 [2]=05 [3]=31(cmd) [4..7]=params [8]=ck
    auto frame = CommandLibrary::set_relative_position(-1);
    EXPECT_EQ(frame[4], 0xFF);
    EXPECT_EQ(frame[5], 0xFF);
    EXPECT_EQ(frame[6], 0xFF);
    EXPECT_EQ(frame[7], 0xFF);
    EXPECT_TRUE(checksum_valid(frame));
}

// ===========================================================================
// rotate  (0x32)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, Rotate_Broadcast_FC_Forward)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x32, 0x01, 0xCE};
    ASSERT_FRAME_EQ(CommandLibrary::rotate(1), expected);
}

TEST(ActiveSurfaceCommandLibrary, Rotate_Backward_EncodedAsTwosComplement)
{
    // -1 → 0xFF as int8_t cast to uint8_t
    // Broadcast: [0]=FC [1]=00 [2]=02 [3]=32(cmd) [4]=param [5]=ck
    auto frame = CommandLibrary::rotate(-1);
    EXPECT_EQ(frame[3], 0x32);
    EXPECT_EQ(frame[4], 0xFF);
    EXPECT_TRUE(checksum_valid(frame));
}

// ===========================================================================
// set_velocity  (0x35)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetVelocity_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x04, 0x35, 0x00, 0x00, 0x01, 0xC9};
    ASSERT_FRAME_EQ(CommandLibrary::set_velocity(1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetVelocity_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x81, 0x35, 0x00, 0x00, 0x01, 0x4C};
    ASSERT_FRAME_EQ(CommandLibrary::set_velocity(1, 1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetVelocity_NegativeVelocity)
{
    // -1 → 0xFFFFFF (3-byte two's complement)
    // Broadcast: [0]=FC [1]=00 [2]=04 [3]=35(cmd) [4..6]=params [7]=ck
    auto frame = CommandLibrary::set_velocity(-1);
    EXPECT_EQ(frame[4], 0xFF);
    EXPECT_EQ(frame[5], 0xFF);
    EXPECT_EQ(frame[6], 0xFF);
    EXPECT_TRUE(checksum_valid(frame));
}

// ===========================================================================
// Rev 1.3 commands: set_stop_io (0x2A), set_positioning_io (0x2B),
//                   set_home_io (0x2C), set_working_mode (0x2D)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, SetStopIo_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x2A, 0x00, 0xD7};
    ASSERT_FRAME_EQ(CommandLibrary::set_stop_io(0x00), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetStopIo_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x41, 0x2A, 0x00, 0x98};
    ASSERT_FRAME_EQ(CommandLibrary::set_stop_io(0x00, 1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetPositioningIo_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x2B, 0x00, 0xD6};
    ASSERT_FRAME_EQ(CommandLibrary::set_positioning_io(0x00), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetHomeIo_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x02, 0x2C, 0x00, 0xD5};
    ASSERT_FRAME_EQ(CommandLibrary::set_home_io(0x00), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetWorkingMode_Broadcast_FC)
{
    const Bytes expected = {0xFC, 0x00, 0x03, 0x2D, 0x00, 0x00, 0xD3};
    ASSERT_FRAME_EQ(CommandLibrary::set_working_mode(0x00), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetWorkingMode_Unicast_Addr1_FC)
{
    const Bytes expected = {0xFC, 0x61, 0x2D, 0x00, 0x00, 0x75};
    ASSERT_FRAME_EQ(CommandLibrary::set_working_mode(0x00, 1), expected);
}

TEST(ActiveSurfaceCommandLibrary, SetWorkingMode_ReservedByteAlwaysZero)
{
    // byte_par1 must be 0x00 regardless of mode_byte
    // Unicast: [0]=FC [1]=nbyte_addr [2]=cmd [3]=mode [4]=reserved [5]=ck
    auto frame = CommandLibrary::set_working_mode(0x01, 5);
    EXPECT_EQ(frame[4], 0x00) << "byte_par1 (reserved) must be 0x00";
}

// ===========================================================================
// resync_sequence
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, ResyncSequence_Is8NullBytes)
{
    auto seq = CommandLibrary::resync_sequence();
    ASSERT_EQ(seq.size(), static_cast<size_t>(CommandLibrary::RESYNC_BYTE_COUNT));
    for (uint8_t b : seq)
        EXPECT_EQ(b, 0x00);
}

// ===========================================================================
// Frame structure invariants
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, UnicastFrame_StartsWithFC_WhenAddressOnResponse)
{
    EXPECT_EQ(CommandLibrary::soft_reset(0)[0],        CommandLibrary::BYTE_START_FC);
    EXPECT_EQ(CommandLibrary::get_position(15)[0],     CommandLibrary::BYTE_START_FC);
    EXPECT_EQ(CommandLibrary::set_velocity(500, 7)[0], CommandLibrary::BYTE_START_FC);
}

TEST(ActiveSurfaceCommandLibrary, UnicastFrame_StartsWithFA_WhenNotAddressOnResponse)
{
    EXPECT_EQ(CommandLibrary::soft_reset(0, false)[0],    CommandLibrary::BYTE_START_FA);
    EXPECT_EQ(CommandLibrary::get_position(15, false)[0], CommandLibrary::BYTE_START_FA);
}

TEST(ActiveSurfaceCommandLibrary, BroadcastFrame_SecondByteIsSwitchAll)
{
    EXPECT_EQ(CommandLibrary::soft_reset()[1],              CommandLibrary::BYTE_SWITCHALL);
    EXPECT_EQ(CommandLibrary::soft_trigger()[1],            CommandLibrary::BYTE_SWITCHALL);
    EXPECT_EQ(CommandLibrary::set_absolute_position(0)[1],  CommandLibrary::BYTE_SWITCHALL);
}

TEST(ActiveSurfaceCommandLibrary, UnicastFrame_AddressIsEncodedInLowBitsOfSecondByte)
{
    for (int addr = 0; addr <= 31; ++addr) {
        auto frame = CommandLibrary::soft_reset(addr);
        EXPECT_EQ(frame[1] & 0x1F, static_cast<uint8_t>(addr))
            << "address mismatch for addr=" << addr;
    }
}

TEST(ActiveSurfaceCommandLibrary, UnicastFrame_LengthIsEncodedInHighBitsOfSecondByte)
{
    // soft_reset: no params → len=1 → bits[7:5] = 001
    auto frame = CommandLibrary::soft_reset(0);
    EXPECT_EQ((frame[1] >> 5) & 0x07, 1);

    // set_absolute_position: 4 param bytes → len=5 → bits[7:5] = 101
    auto frame2 = CommandLibrary::set_absolute_position(0, 0);
    EXPECT_EQ((frame2[1] >> 5) & 0x07, 5);
}

TEST(ActiveSurfaceCommandLibrary, AllFrameChecksumsSelfConsistent)
{
    EXPECT_TRUE(checksum_valid(CommandLibrary::soft_reset(5)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::soft_trigger(12)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::get_version(0)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::soft_stop(31)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::get_position(7)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::get_status(3)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::get_driver_type(9)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_min_frequency(500, 4)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_max_frequency(8000, 4)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_slope_multiplier(50, 2)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_reference_position(-32768, 6)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_io_pins(0x54, 1)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_resolution(0x08, 8)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::reduce_current(0xC0, 10)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_response_delay(30, 11)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::toggle_delayed_execution(0x93, 5)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_absolute_position(-835071, 5)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_relative_position(65536, 13)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::rotate(-1, 2)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_velocity(-50000, 0)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_stop_io(0x09, 5)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_positioning_io(0x25, 5)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_home_io(0x01, 5)));
    EXPECT_TRUE(checksum_valid(CommandLibrary::set_working_mode(0x01, 5)));
}

// ===========================================================================
// Input validation (exceptions)
// ===========================================================================

TEST(ActiveSurfaceCommandLibrary, AddressOutOfRange_Throws)
{
    EXPECT_THROW(CommandLibrary::soft_reset(32),      std::out_of_range);
    EXPECT_THROW(CommandLibrary::get_position(100),   std::out_of_range);
}

TEST(ActiveSurfaceCommandLibrary, AddressBoundary_DoesNotThrow)
{
    EXPECT_NO_THROW(CommandLibrary::soft_reset(0));
    EXPECT_NO_THROW(CommandLibrary::soft_reset(31));
}

TEST(ActiveSurfaceCommandLibrary, SetMinFrequency_OutOfRange_Throws)
{
    EXPECT_THROW(CommandLibrary::set_min_frequency(19),    std::out_of_range);
    EXPECT_THROW(CommandLibrary::set_min_frequency(10001), std::out_of_range);
}

TEST(ActiveSurfaceCommandLibrary, SetMinFrequency_BoundaryValues_DoNotThrow)
{
    EXPECT_NO_THROW(CommandLibrary::set_min_frequency(20));
    EXPECT_NO_THROW(CommandLibrary::set_min_frequency(10000));
}

TEST(ActiveSurfaceCommandLibrary, SetMaxFrequency_OutOfRange_Throws)
{
    EXPECT_THROW(CommandLibrary::set_max_frequency(19),    std::out_of_range);
    EXPECT_THROW(CommandLibrary::set_max_frequency(10001), std::out_of_range);
}

TEST(ActiveSurfaceCommandLibrary, SetVelocity_OutOfRange_Throws)
{
    EXPECT_THROW(CommandLibrary::set_velocity(-100001), std::out_of_range);
    EXPECT_THROW(CommandLibrary::set_velocity(100001),  std::out_of_range);
}

TEST(ActiveSurfaceCommandLibrary, SetVelocity_BoundaryValues_DoNotThrow)
{
    EXPECT_NO_THROW(CommandLibrary::set_velocity(-100000));
    EXPECT_NO_THROW(CommandLibrary::set_velocity(100000));
    EXPECT_NO_THROW(CommandLibrary::set_velocity(0));
}

TEST(ActiveSurfaceCommandLibrary, SetStopIo_OutOfRange_Throws)
{
    EXPECT_THROW(CommandLibrary::set_stop_io(64),  std::out_of_range);
    EXPECT_THROW(CommandLibrary::set_stop_io(255), std::out_of_range);
}

TEST(ActiveSurfaceCommandLibrary, SetPositioningIo_OutOfRange_Throws)
{
    EXPECT_THROW(CommandLibrary::set_positioning_io(64), std::out_of_range);
}

TEST(ActiveSurfaceCommandLibrary, SetHomeIo_OutOfRange_Throws)
{
    EXPECT_THROW(CommandLibrary::set_home_io(64), std::out_of_range);
}
