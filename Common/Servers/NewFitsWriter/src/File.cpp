#include "File.h"
#include "FitsTools.h"


using namespace FitsWriter_private;

double CDoubleTypeEntry::defaultValue=-0.0;
//IRA::CString CStringTypeEntry::defaultValue("NULL");
std::string CStringTypeEntry::defaultValue("NULL");
long CLongTypeEntry::defaultValue=0;
TFileTypeEnumeration CDoubleTypeEntry::typeSpec=_FILE_DOUBLE_TYPE_S;
TFileTypeEnumeration CStringTypeEntry::typeSpec=_FILE_STRING_TYPE_S;
TFileTypeEnumeration CLongTypeEntry::typeSpec=_FILE_LONG_TYPE_S;

char CDoubleTypeEntry::format='D';
char CStringTypeEntry::format='A';
char CLongTypeEntry::format='J';

CFile::CFile()
{
	m_fits=NULL;
	m_fileName="";
}

using namespace IRA;

CFile::~CFile()
{
	TMHscroll it;
	for (it=m_mainHeader.begin();it!=m_mainHeader.end();it++) {
		if (it->second) delete it->second;
	}
	TTscroll in;
	for (in=m_tables.begin();in!=m_tables.end();in++) {
		if (in->second) {
			in->second->flushAll();
			delete in->second;
		}
	}
	m_mainHeader.clear();
	m_tables.clear();
	CFitsTools::closeFile(m_fits);
}

CTable *CFile::getTable(const IRA::CString& name,IRA::CString& lastError)
{
	TTscroll in;
	for (in=m_tables.begin();in!=m_tables.end();in++) {
		CTable *p=in->second;
		if (name==p->getTableName()) {
			return in->second;
		}
	}
	lastError.Format("table %s does not exist",(const char *)name);
	return NULL;
}

CTable *CFile::getTable(const long& id,IRA::CString& lastError)
{
	TTscroll it=m_tables.find(id);
	if (it!=m_tables.end()) return it->second;
	else {
		lastError.Format("table %ld does not exist",id);
		return NULL;
	}
}

bool CFile::create(IRA::CString& lastError)
{
	CEntry *cc;
	begin();
	while ((cc=getNextMHEntry())!=NULL) {
		if (cc->getTable()==0) {
			if (cc->getType()==_FILE_STRING_TYPE_S) {
				if (cc->isSimple()) {
					CHeaderEntry<_FILE_STRING_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,false> *>(cc);
					//_FILE_STRING_TYPE::TEntryReference val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(m_fits,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
				}
				else {
					CHeaderEntry<_FILE_STRING_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,true> *>(cc);
					//std::list<_FILE_STRING_TYPE::TEntryReference> val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(m_fits,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
				}
			} else if (cc->getType()==_FILE_DOUBLE_TYPE_S) {
				if (cc->isSimple()) {
					CHeaderEntry<_FILE_DOUBLE_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,false> *>(cc);
					//_FILE_DOUBLE_TYPE::TEntryReference val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(m_fits,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
				}
				else {
					CHeaderEntry<_FILE_DOUBLE_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,true> *>(cc);
					//std::list<_FILE_DOUBLE_TYPE::TEntryReference> val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(m_fits,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
				}
			} else {  // _FILE_LONG_TYPE_S
				if (cc->isSimple()) {
					CHeaderEntry<_FILE_LONG_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,false> *>(cc);
					//_FILE_LONG_TYPE::TEntryReference val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(m_fits,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
				}
				else {
					CHeaderEntry<_FILE_LONG_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,true> *>(cc);
					//std::list<_FILE_LONG_TYPE::TEntryReference> val=p->getValue();
					if (!CFitsTools::setPrimaryHeaderKey(m_fits,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
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
						//_FILE_STRING_TYPE::TEntryReference val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
					}
					else {
						CHeaderEntry<_FILE_STRING_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,true> *>(cc);
						//std::list<_FILE_STRING_TYPE::TEntryReference> val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
					}
				} else if (cc->getType()==_FILE_DOUBLE_TYPE_S) {
					if (cc->isSimple()) {
						CHeaderEntry<_FILE_DOUBLE_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,false> *>(cc);
						//_FILE_DOUBLE_TYPE::TEntryReference val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
					}
					else {
						CHeaderEntry<_FILE_DOUBLE_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,true> *>(cc);
						//std::list<_FILE_DOUBLE_TYPE::TEntryReference> val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
					}
				} else {  // _FILE_LONG_TYPE_S
					if (cc->isSimple()) {
						CHeaderEntry<_FILE_LONG_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,false> *>(cc);
						//_FILE_LONG_TYPE::TEntryReference val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
					}
					else {
						CHeaderEntry<_FILE_LONG_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,true> *>(cc);
						//std::list<_FILE_LONG_TYPE::TEntryReference> val=p->getValue();
						if (!CFitsTools::setTableHeaderKey(fitsTab,p->getName(),p->getValue(),p->getDescription(),lastError)) return false;
					}
				}
			}
			else {
				lastError.Format("table %ld, referenced by the keyword %s is missing",cc->getTable(),(const char *)cc->getDescription());
				return false;
			}
		}
	}
	return true;
}

bool CFile::open(IRA::CString& lastError)
{
	CTable *tt;
	begin();
	if (!CFitsTools::createFile(m_fits,m_fileName,lastError)) {
		return false;
	}
	while ((tt=getNextTEntry())!=NULL) {
		std::vector<string> name,form,unit;
		tt->getColumnDefinition(name,form,unit);
		//for (int i=0;i<name.size();i++) printf("%d, %s %s %s\n",i,name[i].c_str(),form[i].c_str(),unit[i].c_str());
		CCfits::Table *p=CFitsTools::createTable(m_fits,(const char *)tt->getTableName(),name,form,unit,lastError);
		if (p!=NULL) {
			tt->setTableRef(p);
		}
		else {
			return false;
		}
	}
	return true;
}

bool CTable::nextRow(IRA::CString& errMsg) {
	TCscroll it;
	TCDscroll dit;
	bool res=true;
	if (m_cachePointer==_FILE_ENTRY_CACHE_SIZE-1) { //end of cache.......
		res=write(errMsg);
		m_cachePointer=0;
	} else {
		m_cachePointer++;
	}
	for (it=m_columns.begin();it!=m_columns.end();it++) {
		CEntry *base=it->second;
		if (base->isSimple()) {
			if (base->getType()==_FILE_STRING_TYPE_S) {
				CColumnEntry<_FILE_STRING_TYPE,false>* c=dynamic_cast< CColumnEntry<_FILE_STRING_TYPE,false> * >(base);
				c->nextRow();
			}
			else if (base->getType()==_FILE_DOUBLE_TYPE_S) {
				CColumnEntry<_FILE_DOUBLE_TYPE,false>* c=dynamic_cast< CColumnEntry<_FILE_DOUBLE_TYPE,false> * >(base);
				c->nextRow();
			}
			else if (base->getType()==_FILE_LONG_TYPE_S) {
				CColumnEntry<_FILE_LONG_TYPE,false>* c=dynamic_cast< CColumnEntry<_FILE_LONG_TYPE,false> * >(base);
				c->nextRow();
			}
		}
		else {
			if (base->getType()==_FILE_STRING_TYPE_S) {
				CColumnEntry<_FILE_STRING_TYPE,true>* c=dynamic_cast< CColumnEntry<_FILE_STRING_TYPE,true> * >(base);
				c->nextRow();
			}
			else if (base->getType()==_FILE_DOUBLE_TYPE_S) {
				CColumnEntry<_FILE_DOUBLE_TYPE,true>* c=dynamic_cast< CColumnEntry<_FILE_DOUBLE_TYPE,true> * >(base);
				c->nextRow();
			}
			else if (base->getType()==_FILE_LONG_TYPE_S) {
				CColumnEntry<_FILE_LONG_TYPE,true>* c=dynamic_cast< CColumnEntry<_FILE_LONG_TYPE,true> * >(base);
				c->nextRow();
			}
		}
	}
	for (dit=m_dataColumns.begin();dit!=m_dataColumns.end();dit++) {
		CEntry *base=dit->second;
		if (base->isSimple()) {
			CColumnEntry<_FILE_DOUBLE_TYPE,false>* c=dynamic_cast< CColumnEntry<_FILE_DOUBLE_TYPE,false> * >(base);
			c->nextRow();
		}
		else {
			CColumnEntry<_FILE_DOUBLE_TYPE,true>* c=dynamic_cast< CColumnEntry<_FILE_DOUBLE_TYPE,true> * >(base);
			c->nextRow();
		}
	}
	return res;
}

bool CTable::write(IRA::CString& errMsg)
{
	TCscroll it;
	TCDscroll dit;
	for (it=m_columns.begin();it!=m_columns.end();it++) {
		CEntry *base=it->second;
		if (base->isSimple()) {
			if (base->getType()==_FILE_STRING_TYPE_S) {
				CColumnEntry<_FILE_STRING_TYPE,false>* c=dynamic_cast< CColumnEntry<_FILE_STRING_TYPE,false> * >(base);
				if (!CFitsTools::writeTable(m_reference,base->getName(),c->popValues(),m_row+1,errMsg)) {
					return false;
				}
			}
			else if (base->getType()==_FILE_DOUBLE_TYPE_S) {
				CColumnEntry<_FILE_DOUBLE_TYPE,false>* c=dynamic_cast< CColumnEntry<_FILE_DOUBLE_TYPE,false> * >(base);
				if (!CFitsTools::writeTable(m_reference,base->getName(),c->popValues(),m_row+1,errMsg)) {
					return false;
				}
			}
			else if (base->getType()==_FILE_LONG_TYPE_S) {
				CColumnEntry<_FILE_LONG_TYPE,false>* c=dynamic_cast< CColumnEntry<_FILE_LONG_TYPE,false> * >(base);
				if (!CFitsTools::writeTable(m_reference,base->getName(),c->popValues(),m_row+1,errMsg)) {
					return false;
				}
			}
		}
		else {
			if (base->getType()==_FILE_STRING_TYPE_S) {
			}
			else if (base->getType()==_FILE_DOUBLE_TYPE_S) {
				CColumnEntry<_FILE_DOUBLE_TYPE,true>* c=dynamic_cast< CColumnEntry<_FILE_DOUBLE_TYPE,true> * >(base);
				if (!CFitsTools::writeTable(m_reference,base->getName(),c->popValues(),m_row+1,errMsg)) {
					return false;
				}
			}
			else if (base->getType()==_FILE_LONG_TYPE_S) {
				CColumnEntry<_FILE_LONG_TYPE,true>* c=dynamic_cast< CColumnEntry<_FILE_LONG_TYPE,true> * >(base);
				if (!CFitsTools::writeTable(m_reference,base->getName(),c->popValues(),m_row+1,errMsg)) {
					return false;
				}
			}
		}
	}
	for (dit=m_dataColumns.begin();dit!=m_dataColumns.end();dit++) {
		CEntry *base=dit->second;
		if (base->isSimple()) {
			CColumnEntry<_FILE_DOUBLE_TYPE,false>* c=dynamic_cast< CColumnEntry<_FILE_DOUBLE_TYPE,false> * >(base);
			if (!CFitsTools::writeTable(m_reference,base->getName(),c->popValues(),m_row+1,errMsg)) {
				return false;
			}
		}
		else {
			CColumnEntry<_FILE_DOUBLE_TYPE,true>* c=dynamic_cast< CColumnEntry<_FILE_DOUBLE_TYPE,true> * >(base);
			if (!CFitsTools::writeTable(m_reference,base->getName(),c->popValues(),m_row+1,errMsg)) {
				return false;
			}
		}
	}
	m_row+=m_cachePointer+1;
	return true;
}

void CTable::flushAll()
{
	IRA::CString errMsg;
	write(errMsg);
}
