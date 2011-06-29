/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __SRTKBANDMFPARAMETERS_H__
#define __SRTKBANDMFPARAMETERS_H__

#define BUFF_LIMIT 500


/// A structure that holds receiver CDB parameters
struct RCDBParameters {
    DWORD NUMBER_OF_FEEDS;
    CString IP;
    DWORD PORT;
    DWORD TIMEOUT;
    CString RECEIVER_NAME;
};


/// The latest property values and the corresponding time in `second.useconds`
struct ExpireTime {
	double vacuum_value;
    double vacuum_time;
};


#endif
