#ifndef _MEDICINAVERTEXLIBRARY_H
#define _MEDICINAVERTEXLIBRARY_H

/* ***************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                       */
/*                                                                                                       */
/* This code is under GNU General Public Licence (GPL).                                                  */
/*                                                                                                       */
/*                                                                                                       */
/* Who                                when            What                                               */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/09/2021	  Creation       
*/
#include <IRA>

class CMedicinaVertex {
public:
	CMedicinaVertex(const IRA::CString& addr,const DWORD& port);
	virtual ~CMedicinaVertex();
	inline IRA::CString getLastErrorMessage() const { return m_lastErrorMessage; }
	
	bool sendTo(const void *buffer,int size);
	
protected:
	IRA::CString m_vertexAddr;
	DWORD m_vertexPort;
	IRA::CString m_lastErrorMessage;
	IRA::CSocket m_sock;
	IRA::CError m_err;
};



#endif