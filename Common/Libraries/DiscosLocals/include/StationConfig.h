/* ************************************************************************************************************* */
/* DISCOS project                                                                                                */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                   */
/* Andrea Orlati (andrea.orlati@inaf.it)  30/10/2020
*/

#include <IRA>

#ifndef DISCOSLOCALS_H_
#define DISCOSLOCALS_H_


namespace DiscosLocals {

class StationConfig {
public:
	StationConfig();
	~StationConfig();
	bool initialize(maci::ContainerServices *Services);
	bool sendMail(const IRA::CString& subject,const IRA::CString& body);
	const IRA::CString& getWelcomeMessage() const { return m_welcome; }
	const IRA::CString& getAdmins() const { return m_admins; }
	const IRA::CString& getDomain() const { return m_domain; }
private:
	IRA::CString m_welcome;
	IRA::CString m_admins;
	IRA::CString m_domain;
	IRA::CString m_receipients;	
};
 
}


#endif /* DISCOSLOCALS_H_ */