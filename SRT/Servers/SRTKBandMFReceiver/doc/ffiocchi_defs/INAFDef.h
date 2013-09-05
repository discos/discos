#pragma once

#define INAFDEF_H

//***********************************************************
// Definizioni
//***********************************************************

#define CMD_SOH                 0x01
#define CMD_STX                 0x02
#define CMD_ETX                 0x03
#define CMD_EOT                 0x04

//***********************************************************
// Address
#define CMD_ADD_MUSK            0x7F

#define CMD_ADD_MUTE            0x00
#define CMD_ADD_ALL             CMD_ADD_MUSK

//***********************************************************
// Type
#define CMD_TYPE_REPETITIVE     0x10
#define CMD_TYPE_NOCHECKSUM     0x20

#define CMD_TYPE_MUSK           (CMD_TYPE_NOCHECKSUM | CMD_TYPE_REPETITIVE)

//***********************************************************

#define CMD_TYPE_EXTENDED       0x41 // A
#define CMD_TYPE_ABBREVIATED    (CMD_TYPE_EXTENDED | CMD_TYPE_NOCHECKSUM) // 0x61 // a

//***********************************************************

#define CMD_TYPE_INQUIRY       0x00
#define CMD_TYPE_RESET         0x01
#define CMD_TYPE_VERSION       0x02
#define CMD_TYPE_SAVE          0x03
#define CMD_TYPE_RESTORE       0x04
#define CMD_TYPE_GET_ADDR      0x05
#define CMD_TYPE_SET_ADDR      0x06
#define CMD_TYPE_GET_TIME      0x07
#define CMD_TYPE_SET_TIME      0x08
#define CMD_TYPE_GET_FRAME     0x09
#define CMD_TYPE_SET_FRAME     0x0A
#define CMD_TYPE_GET_PORT      0x0B
#define CMD_TYPE_SET_PORT      0x0C
#define CMD_TYPE_GET_DATA      0x0D
#define CMD_TYPE_SET_DATA      0x0E
#define CMD_TYPE_ERROR         0x0F

#define CMD_INQUIRY            (CMD_TYPE_EXTENDED + CMD_TYPE_INQUIRY)   // A - 0x41
#define CMD_RESET              (CMD_TYPE_EXTENDED + CMD_TYPE_RESET)     // B - 0x42
#define CMD_VERSION            (CMD_TYPE_EXTENDED + CMD_TYPE_VERSION)   // C - 0x43
#define CMD_SAVE               (CMD_TYPE_EXTENDED + CMD_TYPE_SAVE)      // D - 0x44
#define CMD_RESTORE            (CMD_TYPE_EXTENDED + CMD_TYPE_RESTORE)   // E - 0x45
#define CMD_GET_ADDR           (CMD_TYPE_EXTENDED + CMD_TYPE_GET_ADDR)  // F - 0x46
#define CMD_SET_ADDR           (CMD_TYPE_EXTENDED + CMD_TYPE_SET_ADDR)  // G - 0x47
#define CMD_GET_TIME           (CMD_TYPE_EXTENDED + CMD_TYPE_GET_TIME)  // H - 0x48
#define CMD_SET_TIME           (CMD_TYPE_EXTENDED + CMD_TYPE_SET_TIME)  // I - 0x49
#define CMD_GET_FRAME          (CMD_TYPE_EXTENDED + CMD_TYPE_GET_FRAME) // J - 0x4A
#define CMD_SET_FRAME          (CMD_TYPE_EXTENDED + CMD_TYPE_SET_FRAME) // K - 0x4B
#define CMD_GET_PORT           (CMD_TYPE_EXTENDED + CMD_TYPE_GET_PORT)  // L - 0x4C
#define CMD_SET_PORT           (CMD_TYPE_EXTENDED + CMD_TYPE_SET_PORT)  // M - 0x4D
#define CMD_GET_DATA           (CMD_TYPE_EXTENDED + CMD_TYPE_GET_DATA)  // N - 0x4E
#define CMD_SET_DATA           (CMD_TYPE_EXTENDED + CMD_TYPE_SET_DATA)  // O - 0x4F

//#define CMD_TYPE_MIN_EXT       CMD_INQUIRY
//#define CMD_TYPE_MAX_EXT       CMD_SET_DATA

//#define CMD_TYPE_MIN_ABB       (CMD_TYPE_MIN_EXT | CMD_TYPE_NOCHECKSUM)
//#define CMD_TYPE_MAX_ABB       (CMD_TYPE_MAX_EXT | CMD_TYPE_NOCHECKSUM)

//***********************************************************
// Data Type

#define CMD_DATA_TYPE_TIME      0x40
#define CMD_DATA_TYPE_SIZE_7    0x20

#define CMD_DATA_TYPE_MASK      (~(CMD_DATA_TYPE_TIME | CMD_DATA_TYPE_SIZE_7))

//***********************************************************

#define CMD_DATA_TYPE_STRING    0x00
#define CMD_DATA_TYPE_CHAR      0x01
#define CMD_DATA_TYPE_STRUCT    0x02

#define CMD_DATA_TYPE_B01       0x03
#define CMD_DATA_TYPE_B08       0x04
#define CMD_DATA_TYPE_B16       0x05
#define CMD_DATA_TYPE_B32       0x06
#define CMD_DATA_TYPE_B64       0x07

#define CMD_DATA_TYPE_U08       0x08
#define CMD_DATA_TYPE_S08       0x09
#define CMD_DATA_TYPE_H08       0x0A
#define CMD_DATA_TYPE_O08       0x0B

#define CMD_DATA_TYPE_U16       0x0C
#define CMD_DATA_TYPE_S16       0x0D
#define CMD_DATA_TYPE_H16       0x0E
#define CMD_DATA_TYPE_O16       0x0F

#define CMD_DATA_TYPE_U32       0x10
#define CMD_DATA_TYPE_S32       0x11
#define CMD_DATA_TYPE_H32       0x12
#define CMD_DATA_TYPE_O32       0x13

#define CMD_DATA_TYPE_U64       0x14
#define CMD_DATA_TYPE_S64       0x15
#define CMD_DATA_TYPE_H64       0x16
#define CMD_DATA_TYPE_O64       0x17

#define CMD_DATA_TYPE_F32       0x18
#define CMD_DATA_TYPE_F64       0x19 

#define CMD_DATA_TYPE_PTR       0x1A // void pointer

//#define CMD_DATA_TYPE_TMR       0x1B // struttura STTIME

//***********************************************************

#define CMD_DATA_TYPE_7STRING   (CMD_DATA_TYPE_STRING | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7CHAR     (CMD_DATA_TYPE_CHAR   | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7STRUCT   (CMD_DATA_TYPE_STRUCT | CMD_DATA_TYPE_SIZE_7)

#define CMD_DATA_TYPE_7B01      (CMD_DATA_TYPE_B01    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7B08      (CMD_DATA_TYPE_B08    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7B16      (CMD_DATA_TYPE_B16    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7B32      (CMD_DATA_TYPE_B32    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7B64      (CMD_DATA_TYPE_B64    | CMD_DATA_TYPE_SIZE_7)

#define CMD_DATA_TYPE_7U08      (CMD_DATA_TYPE_U08    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7S08      (CMD_DATA_TYPE_S08    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7H08      (CMD_DATA_TYPE_H08    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7O08      (CMD_DATA_TYPE_O08    | CMD_DATA_TYPE_SIZE_7)

#define CMD_DATA_TYPE_7U16      (CMD_DATA_TYPE_U16    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7S16      (CMD_DATA_TYPE_S16    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7H16      (CMD_DATA_TYPE_H16    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7O16      (CMD_DATA_TYPE_O16    | CMD_DATA_TYPE_SIZE_7)

#define CMD_DATA_TYPE_7U32      (CMD_DATA_TYPE_U32    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7S32      (CMD_DATA_TYPE_S32    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7H32      (CMD_DATA_TYPE_H32    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7O32      (CMD_DATA_TYPE_O32    | CMD_DATA_TYPE_SIZE_7)

#define CMD_DATA_TYPE_7U64      (CMD_DATA_TYPE_U64    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7S64      (CMD_DATA_TYPE_S64    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7H64      (CMD_DATA_TYPE_H64    | CMD_DATA_TYPE_SIZE_7)
#define CMD_DATA_TYPE_7O64      (CMD_DATA_TYPE_O64    | CMD_DATA_TYPE_SIZE_7)

//***********************************************************

#define CMD_HEADER              0
#define CMD_SLAVE               1
#define CMD_MASTER              2

#define CMD_COMMAND             3
#define CMD_COMMAND_ID          4
#define CMD_COMMAND_CHECKSUM    5
#define CMD_COMMAND_ENDING      6

#define CMD_COMMAND_DATASIZE    5
#define CMD_COMMAND_DATA        6

#define CMD_COMMAND_DATATYPE    6
#define CMD_COMMAND_PORTTYPE    7
#define CMD_COMMAND_PORTNUMBER  8
#define CMD_COMMAND_PORTDATA    9

//***********************************************************

#define CMD_ANSWER              3 
#define CMD_ANSWER_ID           4 
#define CMD_ANSWER_RESULT       5
#define CMD_ANSWER_CHECKSUM     6
#define CMD_ANSWER_ENDING       7

#define CMD_ANSWER_DATASIZE     6
#define CMD_ANSWER_DATA         7

#define CMD_ANSWER_DATATYPE     7
#define CMD_ANSWER_PORTTYPE     8
#define CMD_ANSWER_PORTNUMBER   9
#define CMD_ANSWER_PORTDATA     10

#define CMD_ANSWER_INQ_COMMAND  6
#define CMD_ANSWER_INQ_RESULT   7
#define CMD_ANSWER_INQ_TIME     8

//***********************************************************
// Errors
#define CMD_ACK                 0x00 /* Tutto OK */
#define CMD_ERR_CMD             0x01
#define CMD_ERR_CHKS            0x02
#define CMD_ERR_FORM            0x03
#define CMD_ERR_DATA            0x04
#define CMD_ERR_TOUT            0x05
#define CMD_ERR_ADDR            0x06
#define CMD_ERR_TIME            0x07
#define CMD_ERR_FRAME_SIZE      0x08
#define CMD_ERR_DATA_TYPE       0x09
#define CMD_ERR_PORT_TYPE       0x0A
#define CMD_ERR_PORT_NUMBER     0x0B
#define CMD_ERR_DATA_SIZE       0x0C

#define CMD_ERR_NEXT_VALID      0x10

//***********************************************************
// Status
#define CMD_COMPLETE            TRUE
#define CMD_NOTCOMPLETE         FALSE
#define CMD_ISACOMMAND          TRUE
#define CMD_ISANANSWER          FALSE
#define CMD_EXTENDED            TRUE
#define CMD_ABBREVIATED         FALSE
#define CMD_PARAMETERS          TRUE
#define CMD_NOPARAMETERS        FALSE
#define CMD_BYTE_7BIT           TRUE
#define CMD_BYTE_8BIT           FALSE
#define CMD_DATA_TYPE           TRUE
#define CMD_NODATA_TYPE         FALSE
#define CMD_REPETITIVE          TRUE
#define CMD_NOREPETITIVE        FALSE

//***********************************************************

#define CMD_NOPARAMETERS_START  0x40
#define CMD_NOPARAMETERS_END    CMD_SET_ADDR
#define CMD_PARAMETERS_START    CMD_GET_FRAME
#define CMD_PARAMETERS_END      0x50

//***********************************************************

#define CMD_WAIT_CHECKSUM       0x7E
#define CMD_WAIT_ENDING         0x7F

//***********************************************************
// Size
#ifdef TIME_SIZE
    #define CMD_TIME_SIZE_MAX  TIME_SIZE
#else
    #define CMD_TIME_SIZE_MAX  8
#endif

#ifdef FRAME_SIZE
    #define CMD_FRAME_SIZE_MAX  FRAME_SIZE
#else
    #define CMD_FRAME_SIZE_MAX  127
#endif

#define CMD_FORM_SIZE_MAX       (CMD_ANSWER_INQ_TIME + 1) // (CMD_ANSWER_INQ_TIME)
// #define CMD_FORM_SIZE_MAX       (CMD_TIME_SIZE_MAX  + CMD_ANSWER_INQ_TIME)
#define CMD_TOTAL_SIZE_MAX      (CMD_FRAME_SIZE_MAX + CMD_FORM_SIZE_MAX)
#define CMD_TIMEOUT             ((WORD)100) /* in msec */
