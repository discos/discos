#include "StationConfig.h"

#include <IRA>

using namespace DiscosLocals;

#define LOCALSETTINGS "DataBlock/LocalSettings"

StationConfig::StationConfig() : m_welcome(""), m_admins(""), m_domain(""), m_receipients("")
{
}

StationConfig::~StationConfig()
{
}

bool StationConfig::initialize(maci::ContainerServices *services)
{
	bool res=true;
	std::vector<IRA::CString> adminNames;
	if (!services) return false;
	if (!IRA::CIRATools::getDBValue(services,"welcomeMessage",m_welcome,"alma/",LOCALSETTINGS)) {
		m_welcome="";
		res=false;
	}
	else {
		std::string welcomeMessage=std::string(m_welcome);
		//Replace every occurrence of the string "\n" (2 characters) with a new line character
		while(true) {
			std::size_t found = welcomeMessage.find("\\n");
			if(found == std::string::npos)
				break;
			welcomeMessage.replace(found,2,"\n");
		}
		m_welcome=welcomeMessage.c_str();
	}
	if (!IRA::CIRATools::getDBValue(services,"admins",m_admins,"alma/",LOCALSETTINGS)) {
		m_admins="";
		res=false;
	}
	if (!IRA::CIRATools::getDBValue(services,"domain",m_domain ,"alma/",LOCALSETTINGS)) {
		m_domain="";
		res=false;
	}	
	if ((m_admins!="") && (m_domain!="")) {
		if (IRA::CIRATools::matchRegExp(m_admins,"[A-Za-z]+\\.[A-Za-z]+",adminNames)) {
			m_receipients="";
			if (adminNames.size()>0) {
				for (unsigned i=0;i<adminNames.size();i++) {
					if (m_receipients!="") m_receipients+=" ";
					m_receipients+=adminNames[i]+"@"+m_domain;
				}
			}		
		}
		else {
			m_receipients="";
			res=false;
		}
	}
	else {
		m_receipients="";
	}
	return res;
}

bool StationConfig::sendMail(const IRA::CString& subject,const IRA::CString& body)
{ 
	if (m_receipients=="") {
		return true;
	}
	else {
		return IRA::CIRATools::sendMail(subject,body,m_receipients);
	}
}

