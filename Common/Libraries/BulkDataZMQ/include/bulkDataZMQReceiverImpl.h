#ifndef _BULKDATAZMQ_RECEIVER_IMPL_H
#define _BULKDATAZMQ_RECEIVER_IMPL_H

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

//#include "ACSBulkDataStatus.h"

#include "bulkDataZMQReceiverS.h"
#include "bulkDataZMQCallback.h"



//#include "bulkDataReceiver.h"


namespace bulkdataZMQImpl
{ 

/** 

 */


template<class TCallback>
class BulkDataZMQReceiverImpl : public virtual POA_bulkdataZMQ::BulkDataZMQReceiver
{
  public:
    
    /**
     * Constructor
     */
    BulkDataZMQReceiverImpl();
  
    /**
     * Destructor
     */
    virtual ~BulkDataZMQReceiverImpl();

    /**
    */
    virtual void openReceiver() ;
    
    virtual void closeReceiver();

    void resetReceiver();

  protected: 

  private:

};

#include "bulkDataZMQReceiverImpl.i"


};

#endif /*!_BULKDATAZMQ_RECEIVER_IMPL_H*/
