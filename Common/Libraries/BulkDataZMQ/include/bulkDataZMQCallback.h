#ifndef _BULKDATAZMQ_CALLBACK_H
#define _BULKDATAZMQ_CALLBACK_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


#include <Cplusplus11Helper.h>
C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
C11_IGNORE_WARNING("-Wmisleading-indentation")
C11_IGNORE_WARNING("-Wcatch-value=")
#include <baciDB.h>
C11_IGNORE_WARNING_POP



//#include "bulkDataReceiver.h"

#include <iostream>

namespace bulkdataZMQImpl
{ 

class BulkDataZMQCallback 
{

  public:

    BulkDataZMQCallback();

    virtual ~BulkDataZMQCallback();

    /*virtual int handle_start(void);

    virtual int handle_stop (void);

    virtual int handle_destroy (void);

    virtual int receive_frame (ACE_Message_Block *frame);*/

    /********************* methods to be implemented by the user *****************/

    virtual int cbStart(ACE_Message_Block * userParam_p = 0) = 0;

    virtual int cbReceive(ACE_Message_Block * frame_p) = 0;

    virtual int cbStop() = 0;

  protected:
    virtual void printBuffer(const std::string message, const std::vector<uint8_t>& buffer);
  private:

};

};
#endif /*!_BULKDATAZMQ_CALLBACK_H*/
