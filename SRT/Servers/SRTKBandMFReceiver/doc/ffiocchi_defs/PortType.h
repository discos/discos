#pragma once

#define PORTTYPE_H

/******************************************************/
/* Definizioni                                        */
/******************************************************/

#define PORT_TYPE_ALL               0x00 /* 0000 0000 */
#define PORT_TYPE_SCI               0x01 /* 0000 0001 */
#define PORT_TYPE_SPI               0x02 /* 0000 0010 */
#define PORT_TYPE_IIC               0x03 /* 0000 0011 */
#define PORT_TYPE_DIO               0x04 /* 0000 0100 */
#define PORT_TYPE_AD                0x05 /* 0000 0101 */
#define PORT_TYPE_DA                0x06 /* 0000 0110 */
#define PORT_TYPE_CAN               0x07 /* 0000 0111 */
#define PORT_TYPE_AD24              0x08 /* 0000 1000 */

/******************************************************/

#define PORT_TYPE_USER              0x40 /* 0100 0000 */

/******************************************************/
                                                      
#define PORT_TYPE_TEMP              0x7A /* 0111 1010 */
#define PORT_TYPE_PLD               0x7B /* 0111 1011 */
#define PORT_TYPE_EEP               0x7C /* 0111 1100 */
#define PORT_TYPE_RAM               0x7D /* 0111 1101 */
#define PORT_TYPE_ROM               0x7E /* 0111 1110 */
#define PORT_TYPE_CLK               0x7F /* 0111 1111 */

