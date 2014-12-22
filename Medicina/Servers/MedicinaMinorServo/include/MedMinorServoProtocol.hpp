#ifndef MEDMINORSERVOPROTOCOL_HPP
#define MEDMINORSERVOPROTOCOL_HPP

#include <stdint.h>

#define MINORSERVO_SETPOS_ADDR 1024
#define SETPOSCMD_ANS_ADDR 1152
#define GETSTA_ADDR 1280
#define MINORSERVO_STATUS_ADDR 1408

typedef struct STRUCT_FSSCU_SETPOS_COMMAND
{
   uint64_t     time;
   uint64_t     ID;

   uint32_t     mode;
   uint32_t     conf;

   double     pos_x_yp;
   double     pos_y_zp;
   double     pos_z1;
   double     pos_z2;
   double     pos_z3;

   uint8_t    escs;      // 0 = system available 1=escs 2=fieldsystem
   uint8_t    enable;  // 0 = system off, 1 = system on
   uint8_t    acknowledge; // rising edge means acknowledge pulse
   uint8_t    speed_enable; // use the following speed values 

   uint32_t   Dummy;

   double     vel_x_yp;
   double     vel_y_zp;
   double     vel_z1;
   double     vel_z2;
   double     vel_z3;

   STRUCT_FSSCU_SETPOS_COMMAND():
        escs((uint8_t)1),
        enable((uint8_t)1),
        acknowledge((uint8_t)0),
        speed_enable((uint8_t)0),
        Dummy(0),
        vel_x_yp(0),
        vel_y_zp(0),
        vel_z1(0),
        vel_z2(0),
        vel_z3(0){};

} MEDMINORSERVOSETPOS;

typedef struct STRUCT_FSSCU_SETPOS_ANSWER
{
   uint64_t     time;
   uint64_t     ID;

   uint32_t     returned;
   uint32_t     error;

} MEDMINORSERVOSETPOSANSWER;

typedef struct STRUCT_FSSCU_GETSTA_COMMAND
{
   uint64_t     time;
   uint64_t     ID;

} MEDMINORSERVOGETSTACOMMAND;

typedef struct STRUCT_FSSCU_GETSTA_ANSWER
{
   uint64_t     time;
   uint64_t     ID;

   uint8_t      system_status;
   uint8_t      system_warning;
   uint8_t      pfr_status;
   uint8_t      sr_status;

   uint32_t     status;

   uint32_t     mode;
   uint32_t     conf;

   uint8_t      escs;
   uint8_t      enable;
   uint8_t      acknowledge;
   uint8_t      speed_enable;

   uint32_t     dummy;

   uint32_t     enable_x_yp;
   uint32_t     state_x_yp;
   double       pos_x_yp;

   uint32_t     enable_y_zp;
   uint32_t     state_y_zp;
   double       pos_y_zp;

   uint32_t     enable_z1;
   uint32_t     state_z1;
   double       pos_z1;

   uint32_t     enable_z2;
   uint32_t     state_z2;
   double       pos_z2;

   uint32_t     enable_z3;
   uint32_t     state_z3;
   double       pos_z3;

} MEDMINORSERVOSTATUS;

/**
 * MEDMINORSERVOSTATUS.SYSTEM_STATUS ERROR CODES
 */
#define MED_MINOR_SERVO_STATUS_LOCAL 1
#define MED_MINOR_SERVO_STATUS_INTERLOCK 2
#define MED_MINOR_SERVO_STATUS_SYSTEM_BLOCK 5
#define MED_MINOR_SERVO_STATUS_SR_BLOCK 6
#define MED_MINOR_SERVO_STATUS_PFR_BLOCK 7
#define MED_MINOR_SERVO_STATUS_PRIMARY_DISABLED 8
#define MED_MINOR_SERVO_STATUS_SECONDARY_DISABLED 9

/**
 * MEDMINORSERVOSTATUS.SYSTEM_STATUS SUCCESS CODES
 */
#define MED_MINOR_SERVO_STATUS_PRIMARY 10
#define MED_MINOR_SERVO_STATUS_SECONDARY 11
#define MED_MINOR_SERVO_STATUS_TRANSFER_TO_PRIMARY 12
#define MED_MINOR_SERVO_STATUS_TRANSFER_TO_SECONDARY 13

#endif

