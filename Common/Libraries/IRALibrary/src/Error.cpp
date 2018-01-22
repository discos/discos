// $Id: Error.cpp,v 1.2 2010-04-16 08:28:38 a.orlati Exp $

#include "IRA"

using namespace IRA;

#define ERRORNUMBER 26

static struct {
	DWORD CodeMnemo;
	CString Description;
} Desc[ERRORNUMBER]=			 {      {CError::SerialType+CError::SrlPortConfig,"Port could not be configured"},
										{CError::SerialType+CError::SrlPortNOpened,"Serial port is not opened"},
										{CError::SerialType+CError::HardwareFail,"Harware failure"},
										{CError::SerialType+CError::ReadTimeout,"Timeout during read operation"},
										{CError::SerialType+CError::WriteTimeout,"Timeout during write operation"},
										{CError::SerialType+CError::MemoryError,"Buffer could not be allocated"},
										{CError::SerialType+CError::BadAnswer,"Unrecognized answer format"},
										{CError::SerialType+CError::ParityError,"Parity error during data transmission"},
										{CError::SerialType+CError::BadParameter,"Parameter doesn't exist in this controller"},
										
										{CError::SocketType+CError::InvalidIPAddr,"Address is not valid or does not exist"},
										{CError::SocketType+CError::SockCreationError,"Socket creation failed"},
										{CError::SocketType+CError::SockBindError,"Socket binding failed"},
										{CError::SocketType+CError::ListenError,"Error encountered while setting up the listening queue"},
										{CError::SocketType+CError::ConnectError,"Error encountered during connection"},
										{CError::SocketType+CError::SocketNCreated,"Socket must be created before any operation"},
										{CError::SocketType+CError::AcceptError,"Error encountered while looking for new connections"},
										{CError::SocketType+CError::SendError,"Error encountered while trying to send data"},
										{CError::SocketType+CError::ReceiveError,"Error encountered while trying to receive data"},
										{CError::SocketType+CError::UnspecifiedHost,"Destination or starting point was not specified"},
										{CError::SocketType+CError::SockConfiguration,"Socket configuration parameter was not configured"},
										{CError::SocketType+CError::OperationNPermitted,"Operation not permitted with this configuration or socket status"},
										
										{CError::DBType+CError::Memory4Parser,"Could not allocate memory for the XML parser"},
										{CError::DBType+CError::TableLocation,"Unable to lacate the table"},
										{CError::DBType+CError::ParserError,"Parser internal error"},
										{CError::DBType+CError::IllegalOpened,"Operation not permitted on an opened dataset"},
										{CError::DBType+CError::DalLocation,"Error while connecting to Data Access Layer"}
									
								 };

CError::CError()
{
	Reset();
}

CError::CError(const CError& rSrc)
{
	m_Type=rSrc.m_Type;
	m_Code=rSrc.m_Code;
	m_sFile=rSrc.m_sFile;
	m_sRoutine=rSrc.m_sRoutine;
	m_dwLine=rSrc.m_dwLine;
	for(WORD i=0;i<rSrc.m_wDataPointer;i++) {
		m_pData[i].Name=rSrc.m_pData[i].Name;
		m_pData[i].Value=rSrc.m_pData[i].Value;
	}
	m_wDataPointer=rSrc.m_wDataPointer;
	m_wDataReader=rSrc.m_wDataReader;
	CIRATools::timeCopy(m_ErrorTime,rSrc.m_ErrorTime);
}

CError::~CError()
{
}	

void CError::Reset()
{
	m_Type=NoError;
	m_Code=0;
	m_wDataPointer=0;
	m_sFile="";
	m_sRoutine="";
	m_dwLine=0;
	m_wDataReader=0;
}

void CError::operator=(const CError& rSrc)
{
	m_Type=rSrc.m_Type;
	m_Code=rSrc.m_Code;
	m_sFile=rSrc.m_sFile;
	m_sRoutine=rSrc.m_sRoutine;
	m_dwLine=rSrc.m_dwLine;
	for(WORD i=0;i<rSrc.m_wDataPointer;i++) {
		m_pData[i].Name=rSrc.m_pData[i].Name;
		m_pData[i].Value=rSrc.m_pData[i].Value;
	}
	m_wDataPointer=rSrc.m_wDataPointer;
	m_wDataReader=rSrc.m_wDataReader;
	CIRATools::timeCopy(m_ErrorTime,rSrc.m_ErrorTime);
}

void CError::setError(ErrorTypes Type,WORD Code,CString File,CString Routine,DWORD Line)
{
	m_Type=Type;
	m_Code=Code;
	m_sFile=File;
	m_sRoutine=Routine;
	m_dwLine=Line;
	CIRATools::getTime(m_ErrorTime);
}

void CError::setExtra(CString Name,DWORD Value)
{
	if (m_wDataPointer>=DataSize) return;
	m_pData[m_wDataPointer].Name=Name;
	m_pData[m_wDataPointer].Value=Value;
	m_wDataPointer++;
}

CString CError::getDescription()
{
	DWORD fCode;
	if (m_Type==NoError) {
		return "No error";
	}
	else {
		fCode=m_Type+m_Code;
		for(int i=0;i<ERRORNUMBER;i++) {
			if (Desc[i].CodeMnemo==fCode) {
				return Desc[i].Description;
			}
		}
		return "Unknow";
	}
}

DWORD CError::getErrorCode()
{
	if (m_Type==NoError) return m_Type;
	else return m_Type+m_Code;
}

void CError::getTime(TIMEVALUE& errTime)
{
	CIRATools::timeCopy(errTime,m_ErrorTime);
}

bool CError::getExtraInfo(CString& Name,DWORD& Value)
{
	if (m_wDataPointer==0) return false;
	if (m_wDataReader>=m_wDataPointer) return false;	
	Name=m_pData[m_wDataReader].Name;
	Value=m_pData[m_wDataReader].Value;
	m_wDataReader++;
	return true;
}

CString CError::getFullDescription()
{
	CString App;
	App.Format("Error: %d ",getErrorCode());
	App+=getDescription();
	if (m_sFile!="") {
		App+=", "+m_sFile;
	}
	if (m_sRoutine!="") {
		App+=", "+m_sFile;
	}
	if (m_dwLine!=0) {
		CString Liner;
		Liner.Format("%d",m_dwLine);
		App+=", line:"+Liner;
	}
	return App;
}
