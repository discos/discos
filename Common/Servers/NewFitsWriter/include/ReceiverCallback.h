/* ************************************************************************************************************* */
/* DISCOS project                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                      		   When            What   				     */
/* Andrea Orlati(a.orlati@ira.inaf.it) 02/10/2015      creation  */

#ifndef RECEIVERCALLBACK_H_
#define RECEIVERCALLBACK_H_

#include <bulkDataCallback.h>
//#include "DataCollection.h"
#include <SecureArea.h>

namespace FitsWriter_private {

/**
 * This callback class will handle all the events that are generated in response to the bulk data sender actions.
 * @author <a href=mailto:righin_s@ira.cnr.it>Simona Righini</a>,
 * <br> 
 */
class ReceiverCallback: public BulkDataCallback {
public:
	ReceiverCallback();
	~ReceiverCallback();
	virtual int cbStart(ACE_Message_Block * userParam_p = 0);
	virtual int cbReceive(ACE_Message_Block * frame_p);
	virtual int cbStop();
	/*static FitsWriter_private::CDataCollection * m_dataCollection;*/
private:
	/*DDWORD m_receivedBytes;*/
	char *m_buffer;
	long m_bufferLen;
	long m_bufferPointer;
};

};
#endif /*RECEIVERCALLBACK_H_*/
