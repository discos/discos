#include "DiscosVersion.h"

using namespace DiscosVersion;


IRA::CString CurrentVersion::getVersion()
{
	IRA::CString value;
	value.Format("%s-%d.%d.%s",(const char *)CurrentVersion::product,CurrentVersion::major,CurrentVersion::minor,(const char *)CurrentVersion::station);
	return value;
}

IRA::CString CurrentVersion::product = IRA::CString(_CURRENT_PRODUCT);
IRA::CString CurrentVersion::station = IRA::CString(_STATION);
WORD CurrentVersion::minor = _CURRENT_VERSION_MINOR; 
WORD CurrentVersion::major = _CURRENT_VERSION_MAJOR;



