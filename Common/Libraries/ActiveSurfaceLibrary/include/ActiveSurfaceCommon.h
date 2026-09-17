#ifndef _ACTIVESURFACECOMMON_H_
#define _ACTIVESURFACECOMMON_H_

static constexpr unsigned int LANS_PER_SECTOR = 12;
static constexpr unsigned int TICK_DIVIDER    = 4;

static constexpr int UNAV                     = 0xFF0000;
static constexpr int MRUN                     = 0x000080;
static constexpr int CAMM                     = 0x000100;
static constexpr int ENBL                     = 0x002000;
static constexpr int PAUT                     = 0x000800;
static constexpr int DIFF                     = 0x000040;
static constexpr int CAL                      = 0x008000;
static constexpr int READY                    = 0x000020;

static constexpr int USxS                     = 7;
static constexpr double MM2STEP               = 1400.0;

#endif
