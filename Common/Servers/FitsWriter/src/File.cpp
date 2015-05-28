#include "File.h"

using namespace FitsWriter_private;

double CDoubleTypeEntry::defaultValue=0.0;
IRA::CString CStringTypeEntry::defaultValue("NULL");
long CLongTypeEntry::defaultValue=0;
TFileTypeEnumeration CDoubleTypeEntry::typeSpec=_FILE_DOUBLE_TYPE_S;
TFileTypeEnumeration CStringTypeEntry::typeSpec=_FILE_STRING_TYPE_S;
TFileTypeEnumeration CLongTypeEntry::typeSpec=_FILE_LONG_TYPE_S;

CFile::CFile()
{
}

CFile::~CFile()
{
	TScroll it;
	for (it=m_file.begin();it!=m_file.end();it++) {
		if (it->second) delete it->second;
	}
	m_file.empty();
}

