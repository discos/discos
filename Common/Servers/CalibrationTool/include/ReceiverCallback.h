#ifndef RECEIVERCALLBACK_H_
#define RECEIVERCALLBACK_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: ReceiverCallback.h,v 1.1 2011-01-04 09:40:59 c.migoni Exp $      */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */

#include <bulkDataCallback.h>
#include <DataCollection.h>
#include <SecureArea.h>

namespace CalibrationTool_private {

/**
 * This callback class will handle all the events that are generated in response to the bulk data sender actions.
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>
 * <br> 
 */
class ReceiverCallback: public BulkDataCallback {
public:
	ReceiverCallback();
	~ReceiverCallback();
	virtual int cbStart(ACE_Message_Block * userParam_p = 0);
	virtual int cbReceive(ACE_Message_Block * frame_p);
	virtual int cbStop();
	static CalibrationTool_private::CDataCollection * m_dataCollection;
private:
	DDWORD m_receivedBytes;
	char *m_buffer;
	long m_bufferLen;
	long m_bufferPointer;
};

};
#endif /*RECEIVERCALLBACK_H_*/
