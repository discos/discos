/*******************************************************************************\
 *
 *  Author: Marco Buttu, mbuttu@oa-cagliari.inaf.it
 *  Last Modified: Mon Mar 30 12:20:06 CEST 2009
 * 
 *  This module is useful to simulate responses from ICD. To use it 
 *  properly you have to:
 *      - comment the responseCheck in icdSocket and decomment
 *        the box "WRONG RESPONSE TEST". That's only if you want to simulate
 *        a wrong response flag, a cmderr or a wrong responsedata code
 *      - decomment the box "TURN STATUS SIGNALS ON" to turn the pattern status 
 *        signals on
 *      - decomment "SWITCH POLLING ERROR ON" to simulate a wrong polling
 *        response
 *   
\*******************************************************************************/

#ifndef __ICD_RESPONSE_TEST_H__
#define __ICD_RESPONSE_TEST_H__

#include "icdSocket.h"


// Use wrongResponse to simulate wrong responseflag, cmderr or wrong responsedata
void wrongResponse(BYTE *buff, BYTE *wrong_buff, int status_code) {
    
    for(int i=0; i < ICD_BUFF_LEN; i++)
        wrong_buff[i] = buff[i];
    
    switch(status_code) {

        case WRONG_RFLAG: // Set a wrong response flag
            if(buff[0] == 0x30)
                wrong_buff[0] = 0x38;
            else
                wrong_buff[0] = 0x30;
            break ;

        case CMDERR: // Set cmderr to 1
            if(buff[0] == 0x30)
                wrong_buff[0] = 0x34; //
            else
                wrong_buff[0] = 0x43;
            break ;

        case WRONG_RCODE: // Set a wrong first byte of responsedata code
            wrong_buff[0] = 0x33;
            break ;
    }
}


// Use this function to switch the pattern status signals on
void turnStatusSignalsOn(BYTE *buff, int status_code) {

    switch(status_code) {

        case IMS_FLTSIG: // Switch the pattern bit related to internal monitoring signals on (fb-statusword[5])
            // 00100000 -> 0x20 -> (2: 0x32, 0: 0x30)
            buff[6] = 0x32;
            buff[7] = 0x30;
            break ;

        case EMS_SIGN: // Switch the pattern bit related to external monitoring signals on (fb-statusword[6])
            // 01000000 -> 0x40 -> (4: 0x34, 0: 0x30)
            buff[6] = 0x34;
            buff[7] = 0x30;
            break ;

        case WARNING: // Switch the pattern bit related to warning signals on (fb-statusword[7])
            // 10000000 -> 0x80 -> (8: 0x38, 0: 0x30)
            buff[6] = 0x38;
            buff[7] = 0x30;
            break ;

        case INIT_ERR: // Switch the pattern bit related to internal error signals on (fb-statusword[3:0])
            // One of these:
            //     00000011 -> 0x03 -> (0: 0x30, 3: 0x33)
            //     00000010 -> 0x02 -> (0: 0x30, 3: 0x32)
            buff[6] = 0x30;
            buff[7] = 0x32;
            break ;

        case QS_ACTIVE: // Switch the pattern bit related to quick stop signals on (fb-statusword[3:0])
            // 00000111 -> 0x07 -> (0: 0x30, 0: 0x37)
            buff[6] = 0x30;
            buff[7] = 0x37;
            break ;
    } 

}


// Use wrongPolling to simulate wrong polling response
void wrongPolling(BYTE *buff) { buff[1] = 0x33; }

#endif
