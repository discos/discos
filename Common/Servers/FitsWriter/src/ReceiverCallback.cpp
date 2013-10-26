// $Id: ReceiverCallback.cpp,v 1.11 2010-09-14 08:50:08 a.orlati Exp $

#include "ReceiverCallback.h"
#include <acstimeEpochHelper.h>
#include <Definitions.h>
#include <LogFilter.h>

using namespace IRA;
using namespace FitsWriter_private;

FitsWriter_private::CDataCollection * ReceiverCallback::m_dataCollection=NULL;

_IRA_LOGFILTER_IMPORT;

ReceiverCallback::ReceiverCallback() : BulkDataCallback()
{
	setSafeTimeout(3000);
	m_receivedBytes=0;
	m_buffer=NULL;
	m_bufferLen=m_bufferPointer=0;
}

ReceiverCallback::~ReceiverCallback()
{
}

int ReceiverCallback::cbStart(ACE_Message_Block * userParam_p)
{
	Backends::TMainHeader *mainH;
	Backends::TSectionHeader *channelH;
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataCollection->Get();
	mainH=(Backends::TMainHeader *)userParam_p->rd_ptr();
	userParam_p->rd_ptr(sizeof(Backends::TMainHeader));
	channelH=(Backends::TSectionHeader *)userParam_p->rd_ptr();	
	m_dataCollection->saveMainHeaders(mainH,channelH);
	m_receivedBytes=0;
	ACS_LOG(LM_FULL_INFO, "ReceiverCallback::cbStart()",(LM_DEBUG,"START_FROM_BACKEND" ));
	return 0;
}

int ReceiverCallback::cbReceive(ACE_Message_Block * frame_p)
{	
	/*Backends::TDumpHeader *dumpH;
	char *buffer;
	DDWORD headerSize=sizeof(Backends::TDumpHeader);*/
	Backends::TDumpHeader *dumpH;
	
	if (flowNumber_m==1) {
		if (m_buffer==NULL) {
			m_buffer=new char[frame_p->total_length()];
			m_bufferLen=frame_p->total_length();
			m_bufferPointer=0;
		}
		else {
			char *newBuff=new char[m_bufferLen+frame_p->total_length()]; //create a new buffer that fits the old plus the new incoming
			memcpy(newBuff,m_buffer,m_bufferPointer); // save the old buffer;
			delete []m_buffer; //get rid of the old buffer;
			m_bufferLen+=frame_p->total_length();  //update the buffer length;
			m_buffer=newBuff;  // now keep track of the buffer pointer;
		}
		while (frame_p!=NULL) {
			memcpy(m_buffer+m_bufferPointer,frame_p->rd_ptr(),frame_p->length());
			m_bufferPointer+=frame_p->length();			
			frame_p=frame_p->cont();
		}
		dumpH=(Backends::TDumpHeader *)m_buffer;
		if (m_bufferPointer>dumpH->dumpSize) {
			//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataCollection->Get();
			if (!m_dataCollection->saveDump(m_buffer)) { ///this will delete the buffer automatically!!!!!
				_IRA_LOGFILTER_LOG(LM_WARNING,"ReceiverCallback::cbReceive()","CANT_KEEP_THROTTLE");
				m_dataCollection->setStatus(Management::MNG_WARNING);
				delete []m_buffer;
			}
			m_buffer=NULL;
			m_bufferLen=0;
			m_bufferPointer=0;
		}
		m_receivedBytes+=frame_p->total_length();
	}
	return 0;		
}

int ReceiverCallback::cbStop()
{	
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataCollection->Get();
	ACS_LOG(LM_FULL_INFO, "ReceiverCallback::cbStop()",(LM_DEBUG,"STOP_FROM_BACKEND" ));
	m_dataCollection->startStopStage();
	//data->setStatus(Management::MNG_WARNING,__LINE__);
	return 0;
}
