#include "MedicinaVertex.h"

CMedicinaVertex::CMedicinaVertex(const IRA::CString& addr,const DWORD& port):
  m_vertexAddr(addr),m_vertexPort(port),m_lastErrorMessage("")
{
}

CMedicinaVertex::~CMedicinaVertex()
{
}

bool CMedicinaVertex::sendTo(const void *buffer,int size)
{
	int count;
	char readout[128];
	m_err.Reset();
	try {
		if (m_sock.Create(m_err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
			m_lastErrorMessage.Format("Cannot create socket with error code %d",m_err.getErrorCode());
			return false;
		}
		if (m_sock.Connect(m_err,m_vertexAddr,m_vertexPort)==IRA::CSocket::FAIL) {
			m_lastErrorMessage.Format("Cannot connect with error code %d",m_err.getErrorCode());
			return false;
		}
		if (m_sock.Send(m_err,buffer,size)!=(int)size) {
			m_lastErrorMessage.Format("Error code %d while sending data",m_err.getErrorCode());
			return false;
		}
		count=m_sock.Receive(m_err,(void *)readout,128);
		if (count==IRA::CSocket::FAIL) {
			m_lastErrorMessage.Format("Error code %d while receiving data",m_err.getErrorCode());
			return false;
		}
		readout[count]=0;
		if (strcmp(readout,"ACK\n")!=0) {
			m_lastErrorMessage.Format("Vertex did not acknowledge");
			return false;
		} 	
		m_sock.Close(m_err);
	}
	catch (...) {
		m_lastErrorMessage.Format("Something weired happened during communication to Vertex");
		return false;		
	}
	return true;
}