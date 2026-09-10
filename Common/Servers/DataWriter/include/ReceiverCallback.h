
#ifndef RECEIVERCALLBACK_H_
#define RECEIVERCALLBACK_H_

#include <bulkDataZMQCallback.h>
#include <Definitions.h>
#include <DataCollection.h>

namespace DataWriter_private {

/**
 * This callback class will handle all the events that are generated in response to the bulk data sender actions.
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>,
 * <br> 
 */
class ReceiverCallback: public bulkdataZMQImpl::BulkDataZMQCallback {
public:
	ReceiverCallback();
	~ReceiverCallback();
	virtual int cbStart(ACE_Message_Block * userParam_p = 0);
	virtual int cbReceive(ACE_Message_Block * frame_p);
	virtual int cbStop();
	static DataWriter_private::CDataCollection * m_dataCollection;
private:
	DDWORD m_receivedBytes;
	char *m_buffer;
	long m_bufferLen;
	long m_bufferPointer;
};

};
#endif /*RECEIVERCALLBACK_H_*/
