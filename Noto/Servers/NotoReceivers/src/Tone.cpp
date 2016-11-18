#include "Tone.h"

#define TURN_ON_CMD "A9\n"
#define TURN_OFF_CMD "B9\n"

using namespace IRA;

CTone::CTone(const IRA::CString& ip,const long& port, const long& timeo) : m_address(ip), m_port(port), m_timeout(timeo)
{
}

CTone::~CTone()
{
}

void CTone::init() throw (ComponentErrors::IRALibraryResourceExImpl)
{
	CError err;
	m_socket.Close(err);
	err.Reset();
	if (m_socket.Create(err,IRA::CSocket::STREAM,0,NULL,true)!=CSocket::SUCCESS) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription((const char*)err.getDescription());
		err.Reset();
		throw dummy;
	}
	if (m_socket.setSockMode(err,CSocket::BLOCKINGTIMEO,m_timeout,m_timeout)!=CSocket::SUCCESS) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription((const char*)err.getDescription());
		err.Reset();
		throw dummy;
	}
}

void CTone::turnOn() throw (ComponentErrors::IRALibraryResourceExImpl)
{
	CError err;
	char buffer[128];
	int total;
	if (m_socket.Connect(err,m_address,m_port)==IRA::CSocket::FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription( (const char*)err.getDescription());
		err.Reset();
		throw dummy;
	}
	strcpy(buffer,TURN_ON_CMD);
	total=strlen(buffer)+1;
	if (m_socket.Send(err,(const void *)buffer,total)<total) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription((const char*)err.getDescription());
		err.Reset();
		throw dummy;
	}
	//NO ASNWER!
	m_socket.Close(err);
}

void CTone::turnOff() throw (ComponentErrors::IRALibraryResourceExImpl)
{
	CError err;
	char buffer[128];
	int total;
	if (m_socket.Connect(err,m_address,m_port)==IRA::CSocket::FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription( (const char*)err.getDescription());
		err.Reset();
		throw dummy;
	}
	strcpy(buffer,TURN_OFF_CMD);
	total=strlen(buffer)+1;
	if (m_socket.Send(err,(const void *)buffer,total)<total) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription((const char*)err.getDescription());
		err.Reset();
		throw dummy;
	}
	//NO ASNWER!
	m_socket.Close(err);
}



