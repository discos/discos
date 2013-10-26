/* ***************************************************************** */
/* IRA Istituto di Radioastronomia                                   */
/* $Id: ReceiverCallback.h,v 1.6 2010-09-14 08:50:08 a.orlati Exp $															 */
/* This code is under GNU General Public Licence (GPL)               */
/*                                                                   */
/* Who                      When            What   				     */
/* Simona Righini			10/11/2008      created      			 */

#ifndef RECEIVERCALLBACK_H_
#define RECEIVERCALLBACK_H_

#include <bulkDataCallback.h>
#include <DataCollection.h>
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
	static FitsWriter_private::CDataCollection * m_dataCollection;
private:
	DDWORD m_receivedBytes;
	char *m_buffer;
	long m_bufferLen;
	long m_bufferPointer;
};

};
#endif /*RECEIVERCALLBACK_H_*/
