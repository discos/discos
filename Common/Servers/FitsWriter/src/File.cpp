#include "File.h"
#include "FitsTools.h"

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
	TMHscroll it;
	for (it=m_mainHeader.begin();it!=m_mainHeader.end();it++) {
		if (it->second) delete it->second;
	}
	TTscroll in;
	for (in=m_tables.begin();in!=m_tables.end();in++) {
		if (in->second) delete in->second;
	}
	m_mainHeader.clear();
	m_tables.clear();
}

bool CFile::write(CCfits::FITS * const fits,IRA::CString& lastError)
{
	CEntry *cc;
	CTable *tt;
	begin();
	while ((tt=getNextTEntry())!=NULL) {
		std::vector<string> name,form,unit;
		name.push_back("gino");
		name.push_back("pino");
		form.push_back("D");
		form.push_back("D");
		unit.push_back("Jy");
		unit.push_back("K");
		CCfits::Table *p=CFitsTools::createTable(fits,(const char *)tt->getTableName(),name,form,unit,lastError);
		if (p!=NULL) {
			tt->setTableRef(p);
		}
		else {
			return false;
		}
	}
	while ((cc=getNextMHEntry())!=NULL) {
		if (cc->getTable()==0) {
			if (cc->getType()==_FILE_STRING_TYPE_S) {
				if (cc->isSimple()) {
					CHeaderEntry<_FILE_STRING_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,false> *>(cc);
					_FILE_STRING_TYPE::TEntryReference val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(fits,p->getKeyword(),val,p->getDescription(),lastError)) return false;
				}
				else {
					CHeaderEntry<_FILE_STRING_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,true> *>(cc);
					std::list<_FILE_STRING_TYPE::TEntryReference> val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(fits,p->getKeyword(),val,p->getDescription(),lastError)) return false;
				}
			} else if (cc->getType()==_FILE_DOUBLE_TYPE_S) {
				if (cc->isSimple()) {
					CHeaderEntry<_FILE_DOUBLE_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,false> *>(cc);
					_FILE_DOUBLE_TYPE::TEntryReference val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(fits,p->getKeyword(),val,p->getDescription(),lastError)) return false;
				}
				else {
					CHeaderEntry<_FILE_DOUBLE_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,true> *>(cc);
					std::list<_FILE_DOUBLE_TYPE::TEntryReference> val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(fits,p->getKeyword(),val,p->getDescription(),lastError)) return false;
				}
			} else {  // _FILE_LONG_TYPE_S
				if (cc->isSimple()) {
					CHeaderEntry<_FILE_LONG_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,false> *>(cc);
					_FILE_LONG_TYPE::TEntryReference val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(fits,p->getKeyword(),val,p->getDescription(),lastError)) return false;
				}
				else {
					CHeaderEntry<_FILE_LONG_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,true> *>(cc);
					std::list<_FILE_LONG_TYPE::TEntryReference> val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(fits,p->getKeyword(),val,p->getDescription(),lastError)) return false;
				}
			}
		}
		else {
			TTscroll in=m_tables.find(cc->getTable()) ;
			if (in!=m_tables.end()) {
				CTable *tab=in->second;
				CCfits::Table *fitsTab=tab->getTableRef();
				if (cc->getType()==_FILE_STRING_TYPE_S) {
					if (cc->isSimple()) {
						CHeaderEntry<_FILE_STRING_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,false> *>(cc);
						_FILE_STRING_TYPE::TEntryReference val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getKeyword(),val,p->getDescription(),lastError)) return false;
					}
					else {
						CHeaderEntry<_FILE_STRING_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,true> *>(cc);
						std::list<_FILE_STRING_TYPE::TEntryReference> val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getKeyword(),val,p->getDescription(),lastError)) return false;
					}
				} else if (cc->getType()==_FILE_DOUBLE_TYPE_S) {
					if (cc->isSimple()) {
						CHeaderEntry<_FILE_DOUBLE_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,false> *>(cc);
						_FILE_DOUBLE_TYPE::TEntryReference val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getKeyword(),val,p->getDescription(),lastError)) return false;
					}
					else {
						CHeaderEntry<_FILE_DOUBLE_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,true> *>(cc);
						std::list<_FILE_DOUBLE_TYPE::TEntryReference> val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getKeyword(),val,p->getDescription(),lastError)) return false;
					}
				} else {  // _FILE_LONG_TYPE_S
					if (cc->isSimple()) {
						CHeaderEntry<_FILE_LONG_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,false> *>(cc);
						_FILE_LONG_TYPE::TEntryReference val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getKeyword(),val,p->getDescription(),lastError)) return false;
					}
					else {
						CHeaderEntry<_FILE_LONG_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,true> *>(cc);
						std::list<_FILE_LONG_TYPE::TEntryReference> val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getKeyword(),val,p->getDescription(),lastError)) return false;
					}
				}
			}
			else {
				lastError.Format("table %ld, referenced by the keyword, %s is missing",cc->getTable(),(const char *)cc->getDescription());
				return false;
			}
		}
	}
	return true;
}

