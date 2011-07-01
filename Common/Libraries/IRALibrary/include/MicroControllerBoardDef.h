#ifndef __MICRO_CONTROLLER_BOARD_DEF__H
#define __MICRO_CONTROLLER_BOARD_DEF__H

const unsigned MCB_TOUT = 200000; // 2 seconds
const unsigned MCB_BUFF_LIMIT = 2048;
// MCB_HT_COUNTER means Header or Terminator COUNTER
const unsigned short MCB_HT_COUNTER = 10; // The maximum number of attempts we wait for a header or terminator
const unsigned short MCB_BASE_ANSWER_LENGTH = 6; // The minimum lenght of the answer. It should to be at least 1

const BYTE MCB_CMD_SOH = 0x01; // Request header (master -> slave)
const BYTE MCB_CMD_STX = 0x02; // Answer header (slave -> master)
// Short commands don't have a terminator byte
const BYTE MCB_CMD_ETX = 0x03; // Answer terminator, only for extended commands (slave -> master)
const BYTE MCB_CMD_EOT = 0x04; // Request terminator, only for extended commands (master -> slave)

const BYTE MCB_CMD_TYPE_NOCHECKSUM  =   0x20;
const BYTE MCB_CMD_TYPE_EXTENDED    =   0x41; // A
const BYTE MCB_CMD_TYPE_ABBREVIATED =   (MCB_CMD_TYPE_EXTENDED | MCB_CMD_TYPE_NOCHECKSUM); // 0x61 // a

const BYTE MCB_CMD_TYPE_INQUIRY     =   0x00; // Answer with data
const BYTE MCB_CMD_TYPE_RESET       =   0x01;
const BYTE MCB_CMD_TYPE_VERSION     =   0x02; // Answer with data
const BYTE MCB_CMD_TYPE_SAVE        =   0x03;
const BYTE MCB_CMD_TYPE_RESTORE     =   0x04;
const BYTE MCB_CMD_TYPE_GET_ADDR    =   0x05; // Answer with data
const BYTE MCB_CMD_TYPE_SET_ADDR    =   0x06;
const BYTE MCB_CMD_TYPE_GET_TIME    =   0x07; // Answer with data
const BYTE MCB_CMD_TYPE_SET_TIME    =   0x08;
const BYTE MCB_CMD_TYPE_GET_FRAME   =   0x09; // Answer with data
const BYTE MCB_CMD_TYPE_SET_FRAME   =   0x0A;
const BYTE MCB_CMD_TYPE_GET_PORT    =   0x0B; // Answer with data
const BYTE MCB_CMD_TYPE_SET_PORT    =   0x0C;
const BYTE MCB_CMD_TYPE_GET_DATA    =   0x0D; // Answer with data
const BYTE MCB_CMD_TYPE_SET_DATA    =   0x0E; 
const BYTE MCB_CMD_TYPE_ERROR       =   0x0F;

const BYTE MCB_CMD_INQUIRY          =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_INQUIRY)  ; // A - 0x41
const BYTE MCB_CMD_RESET            =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_RESET)    ; // B - 0x42
const BYTE MCB_CMD_VERSION          =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_VERSION)  ; // C - 0x43
const BYTE MCB_CMD_SAVE             =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SAVE)     ; // D - 0x44
const BYTE MCB_CMD_RESTORE          =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_RESTORE)  ; // E - 0x45
const BYTE MCB_CMD_GET_ADDR         =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_ADDR) ; // F - 0x46
const BYTE MCB_CMD_SET_ADDR         =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_ADDR) ; // G - 0x47
const BYTE MCB_CMD_GET_TIME         =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_TIME) ; // H - 0x48
const BYTE MCB_CMD_SET_TIME         =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_TIME) ; // I - 0x49
const BYTE MCB_CMD_GET_FRAME        =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_FRAME); // J - 0x4A
const BYTE MCB_CMD_SET_FRAME        =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_FRAME); // K - 0x4B
const BYTE MCB_CMD_GET_PORT         =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_PORT) ; // L - 0x4C
const BYTE MCB_CMD_SET_PORT         =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_PORT) ; // M - 0x4D
const BYTE MCB_CMD_GET_DATA         =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_DATA) ; // N - 0x4E
const BYTE MCB_CMD_SET_DATA         =   (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_DATA) ; // O - 0x4F

const BYTE MCB_CMD_TYPE_MIN_EXT     =   MCB_CMD_INQUIRY;
const BYTE MCB_CMD_TYPE_MAX_EXT     =   MCB_CMD_SET_DATA;

const BYTE MCB_CMD_TYPE_MIN_ABB     =   (MCB_CMD_TYPE_MIN_EXT | MCB_CMD_TYPE_NOCHECKSUM);
const BYTE MCB_CMD_TYPE_MAX_ABB     =   (MCB_CMD_TYPE_MAX_EXT | MCB_CMD_TYPE_NOCHECKSUM);

// Index of the command and id byte, both in answer and request
const unsigned short MCB_CMD_HEADER       =   0; 
const unsigned short MCB_CMD_SLAVE        =   1;
const unsigned short MCB_CMD_MASTER       =   2;
const unsigned short MCB_CMD_COMMAND      =   3;
const unsigned short MCB_CMD_COMMAND_ID   =   4;

// Data Type
const BYTE MCB_CMD_DATA_TYPE_TIME     =    0x40;
const BYTE MCB_CMD_DATA_TYPE_SIZE_7   =    0x20;

const BYTE MCB_CMD_DATA_TYPE_STRING   =    0x00;
const BYTE MCB_CMD_DATA_TYPE_CHAR     =    0x01; 
const BYTE MCB_CMD_DATA_TYPE_STRUCT   =    0x02;

const BYTE MCB_CMD_DATA_TYPE_B01      =    0x03;
const BYTE MCB_CMD_DATA_TYPE_B08      =    0x04;
const BYTE MCB_CMD_DATA_TYPE_B16      =    0x05;
const BYTE MCB_CMD_DATA_TYPE_B32      =    0x06;
const BYTE MCB_CMD_DATA_TYPE_B64      =    0x07;

const BYTE MCB_CMD_DATA_TYPE_U08      =    0x08;
const BYTE MCB_CMD_DATA_TYPE_S08      =    0x09;
const BYTE MCB_CMD_DATA_TYPE_H08      =    0x0A;
const BYTE MCB_CMD_DATA_TYPE_O08      =    0x0B;

const BYTE MCB_CMD_DATA_TYPE_U16      =    0x0C;
const BYTE MCB_CMD_DATA_TYPE_S16      =    0x0D;
const BYTE MCB_CMD_DATA_TYPE_H16      =    0x0E;
const BYTE MCB_CMD_DATA_TYPE_O16      =    0x0F;

const BYTE MCB_CMD_DATA_TYPE_U32      =    0x10;
const BYTE MCB_CMD_DATA_TYPE_S32      =    0x11;
const BYTE MCB_CMD_DATA_TYPE_H32      =    0x12;
const BYTE MCB_CMD_DATA_TYPE_O32      =    0x13;

const BYTE MCB_CMD_DATA_TYPE_U64      =    0x14;
const BYTE MCB_CMD_DATA_TYPE_S64      =    0x15;
const BYTE MCB_CMD_DATA_TYPE_H64      =    0x16;
const BYTE MCB_CMD_DATA_TYPE_O64      =    0x17;

const BYTE MCB_CMD_DATA_TYPE_F32      =    0x18;
const BYTE MCB_CMD_DATA_TYPE_F64      =    0x19; 

const BYTE MCB_CMD_DATA_TYPE_PTR      =    0x1A; // void pointer

const BYTE MCB_CMD_DATA_TYPE_7STRING  =    (MCB_CMD_DATA_TYPE_STRING | MCB_CMD_DATA_TYPE_SIZE_7); 
const BYTE MCB_CMD_DATA_TYPE_7CHAR    =    (MCB_CMD_DATA_TYPE_CHAR   | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7STRUCT  =    (MCB_CMD_DATA_TYPE_STRUCT | MCB_CMD_DATA_TYPE_SIZE_7);

const BYTE MCB_CMD_DATA_TYPE_7B01     =    (MCB_CMD_DATA_TYPE_B01    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7B08     =    (MCB_CMD_DATA_TYPE_B08    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7B16     =    (MCB_CMD_DATA_TYPE_B16    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7B32     =    (MCB_CMD_DATA_TYPE_B32    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7B64     =    (MCB_CMD_DATA_TYPE_B64    | MCB_CMD_DATA_TYPE_SIZE_7);

const BYTE MCB_CMD_DATA_TYPE_7U08     =    (MCB_CMD_DATA_TYPE_U08    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7S08     =    (MCB_CMD_DATA_TYPE_S08    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7H08     =    (MCB_CMD_DATA_TYPE_H08    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7O08     =    (MCB_CMD_DATA_TYPE_O08    | MCB_CMD_DATA_TYPE_SIZE_7);

const BYTE MCB_CMD_DATA_TYPE_7U16     =    (MCB_CMD_DATA_TYPE_U16    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7S16     =    (MCB_CMD_DATA_TYPE_S16    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7H16     =    (MCB_CMD_DATA_TYPE_H16    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7O16     =    (MCB_CMD_DATA_TYPE_O16    | MCB_CMD_DATA_TYPE_SIZE_7);

const BYTE MCB_CMD_DATA_TYPE_7U32     =    (MCB_CMD_DATA_TYPE_U32    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7S32     =    (MCB_CMD_DATA_TYPE_S32    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7H32     =    (MCB_CMD_DATA_TYPE_H32    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7O32     =    (MCB_CMD_DATA_TYPE_O32    | MCB_CMD_DATA_TYPE_SIZE_7);

const BYTE MCB_CMD_DATA_TYPE_7U64     =    (MCB_CMD_DATA_TYPE_U64    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7S64     =    (MCB_CMD_DATA_TYPE_S64    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7H64     =    (MCB_CMD_DATA_TYPE_H64    | MCB_CMD_DATA_TYPE_SIZE_7);
const BYTE MCB_CMD_DATA_TYPE_7O64     =    (MCB_CMD_DATA_TYPE_O64    | MCB_CMD_DATA_TYPE_SIZE_7);

// Port Type
const BYTE MCB_PORT_TYPE_ALL          =     0x00; /* 0000 0000 */
const BYTE MCB_PORT_TYPE_SCI          =     0x01; /* 0000 0001 */
const BYTE MCB_PORT_TYPE_SPI          =     0x02; /* 0000 0010 */
const BYTE MCB_PORT_TYPE_IIC          =     0x03; /* 0000 0011 */
const BYTE MCB_PORT_TYPE_DIO          =     0x04; /* 0000 0100 */
const BYTE MCB_PORT_TYPE_AD           =     0x05; /* 0000 0101 */
const BYTE MCB_PORT_TYPE_DA           =     0x06; /* 0000 0110 */
const BYTE MCB_PORT_TYPE_CAN          =     0x07; /* 0000 0111 */
const BYTE MCB_PORT_TYPE_AD24         =     0x08; /* 0000 1000 */

const BYTE MCB_PORT_TYPE_USER         =     0x40; /* 0100 0000 */

const BYTE MCB_PORT_TYPE_TEMP         =     0x7A; /* 0111 1010 */
const BYTE MCB_PORT_TYPE_PLD          =     0x7B; /* 0111 1011 */
const BYTE MCB_PORT_TYPE_EEP          =     0x7C; /* 0111 1100 */
const BYTE MCB_PORT_TYPE_RAM          =     0x7D; /* 0111 1101 */
const BYTE MCB_PORT_TYPE_ROM          =     0x7E; /* 0111 1110 */
const BYTE MCB_PORT_TYPE_CLK          =     0x7F; /* 0111 1111 */

// Port Number
const BYTE MCB_PORT_NUMBER_00         =     0x00; /* 0000 0000 */
const BYTE MCB_PORT_NUMBER_01         =     0x01; /* 0000 0001 */
const BYTE MCB_PORT_NUMBER_02         =     0x02; /* 0000 0010 */
const BYTE MCB_PORT_NUMBER_03         =     0x03; /* 0000 0011 */
const BYTE MCB_PORT_NUMBER_04         =     0x04; /* 0000 0100 */
const BYTE MCB_PORT_NUMBER_05         =     0x05; /* 0000 0101 */
const BYTE MCB_PORT_NUMBER_06         =     0x06; /* 0000 0110 */
const BYTE MCB_PORT_NUMBER_07         =     0x07; /* 0000 0111 */
const BYTE MCB_PORT_NUMBER_08         =     0x08; /* 0000 1000 */
const BYTE MCB_PORT_NUMBER_09         =     0x09; /* 0000 1001 */
const BYTE MCB_PORT_NUMBER_10         =     0x0A; /* 0000 1010 */
const BYTE MCB_PORT_NUMBER_11         =     0x0B; /* 0000 1011 */
const BYTE MCB_PORT_NUMBER_12         =     0x0C; /* 0000 1100 */
const BYTE MCB_PORT_NUMBER_13         =     0x0D; /* 0000 1101 */
const BYTE MCB_PORT_NUMBER_14         =     0x0E; /* 0000 1110 */
const BYTE MCB_PORT_NUMBER_15         =     0x0F; /* 0000 1111 */
const BYTE MCB_PORT_NUMBER_16         =     0x10; /* 0001 0000 */
const BYTE MCB_PORT_NUMBER_17         =     0x11; /* 0001 0001 */
const BYTE MCB_PORT_NUMBER_18         =     0x12; /* 0001 0010 */
const BYTE MCB_PORT_NUMBER_19         =     0x13; /* 0001 0011 */
const BYTE MCB_PORT_NUMBER_20         =     0x14; /* 0001 0100 */
const BYTE MCB_PORT_NUMBER_21         =     0x15; /* 0001 0101 */
const BYTE MCB_PORT_NUMBER_22         =     0x16; /* 0001 0110 */
const BYTE MCB_PORT_NUMBER_23         =     0x17; /* 0001 0111 */
const BYTE MCB_PORT_NUMBER_24         =     0x18; /* 0001 1000 */
const BYTE MCB_PORT_NUMBER_25         =     0x19; /* 0001 1001 */
const BYTE MCB_PORT_NUMBER_26         =     0x1A; /* 0001 1010 */
const BYTE MCB_PORT_NUMBER_27         =     0x1B; /* 0001 1011 */
const BYTE MCB_PORT_NUMBER_28         =     0x1C; /* 0001 1100 */
const BYTE MCB_PORT_NUMBER_29         =     0x1D; /* 0001 1101 */
const BYTE MCB_PORT_NUMBER_30         =     0x1E; /* 0001 1110 */
const BYTE MCB_PORT_NUMBER_31         =     0x1F; /* 0001 1111 */

const BYTE MCB_PORT_NUMBER_32         =     0x20; /* 0010 0000 */
const BYTE MCB_PORT_NUMBER_33         =     0x21; /* 0010 0001 */
const BYTE MCB_PORT_NUMBER_34         =     0x22; /* 0010 0010 */
const BYTE MCB_PORT_NUMBER_35         =     0x23; /* 0010 0011 */
const BYTE MCB_PORT_NUMBER_36         =     0x24; /* 0010 0100 */
const BYTE MCB_PORT_NUMBER_37         =     0x25; /* 0010 0101 */
const BYTE MCB_PORT_NUMBER_38         =     0x26; /* 0010 0110 */
const BYTE MCB_PORT_NUMBER_39         =     0x27; /* 0010 0111 */
const BYTE MCB_PORT_NUMBER_40         =     0x28; /* 0010 1000 */
const BYTE MCB_PORT_NUMBER_41         =     0x29; /* 0010 1001 */
const BYTE MCB_PORT_NUMBER_42         =     0x2A; /* 0010 1010 */
const BYTE MCB_PORT_NUMBER_43         =     0x2B; /* 0010 1011 */
const BYTE MCB_PORT_NUMBER_44         =     0x2C; /* 0010 1100 */
const BYTE MCB_PORT_NUMBER_45         =     0x2D; /* 0010 1101 */
const BYTE MCB_PORT_NUMBER_46         =     0x2E; /* 0010 1110 */
const BYTE MCB_PORT_NUMBER_47         =     0x2F; /* 0010 1111 */
const BYTE MCB_PORT_NUMBER_48         =     0x30; /* 0011 0000 */
const BYTE MCB_PORT_NUMBER_49         =     0x31; /* 0011 0001 */
const BYTE MCB_PORT_NUMBER_50         =     0x32; /* 0011 0010 */
const BYTE MCB_PORT_NUMBER_51         =     0x33; /* 0011 0011 */
const BYTE MCB_PORT_NUMBER_52         =     0x34; /* 0011 0100 */
const BYTE MCB_PORT_NUMBER_53         =     0x35; /* 0011 0101 */
const BYTE MCB_PORT_NUMBER_54         =     0x36; /* 0011 0110 */
const BYTE MCB_PORT_NUMBER_55         =     0x37; /* 0011 0111 */
const BYTE MCB_PORT_NUMBER_56         =     0x38; /* 0011 1000 */
const BYTE MCB_PORT_NUMBER_57         =     0x39; /* 0011 1001 */
const BYTE MCB_PORT_NUMBER_58         =     0x3A; /* 0011 1010 */
const BYTE MCB_PORT_NUMBER_59         =     0x3B; /* 0011 1011 */
const BYTE MCB_PORT_NUMBER_60         =     0x3C; /* 0011 1100 */
const BYTE MCB_PORT_NUMBER_61         =     0x3D; /* 0011 1101 */
const BYTE MCB_PORT_NUMBER_62         =     0x3E; /* 0011 1110 */
const BYTE MCB_PORT_NUMBER_63         =     0x3F; /* 0011 1111 */

const BYTE MCB_PORT_NUMBER_64         =     0x40; /* 0100 0000 */
const BYTE MCB_PORT_NUMBER_65         =     0x41; /* 0100 0001 */
const BYTE MCB_PORT_NUMBER_66         =     0x42; /* 0100 0010 */
const BYTE MCB_PORT_NUMBER_67         =     0x43; /* 0100 0011 */
const BYTE MCB_PORT_NUMBER_68         =     0x44; /* 0100 0100 */
const BYTE MCB_PORT_NUMBER_69         =     0x45; /* 0100 0101 */
const BYTE MCB_PORT_NUMBER_70         =     0x46; /* 0100 0110 */
const BYTE MCB_PORT_NUMBER_71         =     0x47; /* 0100 0111 */
const BYTE MCB_PORT_NUMBER_72         =     0x48; /* 0100 1000 */
const BYTE MCB_PORT_NUMBER_73         =     0x49; /* 0100 1001 */
const BYTE MCB_PORT_NUMBER_74         =     0x4A; /* 0100 1010 */
const BYTE MCB_PORT_NUMBER_75         =     0x4B; /* 0100 1011 */
const BYTE MCB_PORT_NUMBER_76         =     0x4C; /* 0100 1100 */
const BYTE MCB_PORT_NUMBER_77         =     0x4D; /* 0100 1101 */
const BYTE MCB_PORT_NUMBER_78         =     0x4E; /* 0100 1110 */
const BYTE MCB_PORT_NUMBER_79         =     0x4F; /* 0100 1111 */
const BYTE MCB_PORT_NUMBER_80         =     0x50; /* 0101 0000 */
const BYTE MCB_PORT_NUMBER_81         =     0x51; /* 0101 0001 */
const BYTE MCB_PORT_NUMBER_82         =     0x52; /* 0101 0010 */
const BYTE MCB_PORT_NUMBER_83         =     0x53; /* 0101 0011 */
const BYTE MCB_PORT_NUMBER_84         =     0x54; /* 0101 0100 */
const BYTE MCB_PORT_NUMBER_85         =     0x55; /* 0101 0101 */
const BYTE MCB_PORT_NUMBER_86         =     0x56; /* 0101 0110 */
const BYTE MCB_PORT_NUMBER_87         =     0x57; /* 0101 0111 */
const BYTE MCB_PORT_NUMBER_88         =     0x58; /* 0101 1000 */
const BYTE MCB_PORT_NUMBER_89         =     0x59; /* 0101 1001 */
const BYTE MCB_PORT_NUMBER_90         =     0x5A; /* 0101 1010 */
const BYTE MCB_PORT_NUMBER_91         =     0x5B; /* 0101 1011 */
const BYTE MCB_PORT_NUMBER_92         =     0x5C; /* 0101 1100 */
const BYTE MCB_PORT_NUMBER_93         =     0x5D; /* 0101 1101 */
const BYTE MCB_PORT_NUMBER_94         =     0x5E; /* 0101 1110 */
const BYTE MCB_PORT_NUMBER_95         =     0x5F; /* 0101 1111 */

#endif
