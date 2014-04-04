// $Id: DBTable.cpp,v 1.4 2010-08-10 13:54:30 a.orlati Exp $

#include "DBTable.h"

using namespace IRA;


CDBTable::CDBTable(maci::ContainerServices *services,const CString& tableName,const CString& name,const CString& domain) : m_containerServices(services), 
	m_simpleClient(NULL), m_tableName(tableName), m_domain(domain)
{
	m_fieldList.clear();
	m_opened=false;
	if (name=="") {
		m_fullName=m_domain+services->getName();
	}
	else {
		m_fullName=m_domain+name;
	}
	m_recCounter=0;
}

CDBTable::CDBTable(maci::SimpleClient *client,const CString& tableName,const CString& name,const CString& domain) : m_containerServices(NULL), 
	m_simpleClient(client), m_tableName(tableName), m_domain(domain)
{
	m_fieldList.clear();
	m_opened=false;
	m_fullName=m_domain+name;
	m_recCounter=0;
}

CDBTable::~CDBTable()
{
	closeTable();
}

bool CDBTable::addField(CError& err,const CString& fieldName,const CDataField::TFieldType& type)
{
	if (!err.isNoError()) return false;
	if (m_opened) {
		_SET_ERROR(err,CError::DBType,CError::IllegalOpened,"CDBTable::addField()");
		err.setExtra(fieldName,0);
		return false;
	}
	m_fieldList.push_back(new CDataField(fieldName,type));
	return true;
}

WORD CDBTable::recordCount() const
{
	return m_recCounter;
}

void CDBTable::First()
{
	for(unsigned i=0;i<m_fieldList.size();i++) {
		m_fieldList[i]->First();
	}
}

void CDBTable::Last()
{
	for(unsigned i=0;i<m_fieldList.size();i++) {
		m_fieldList[i]->Last();
	}	
}

void CDBTable::Next()
{
	for(unsigned i=0;i<m_fieldList.size();i++) {
		m_fieldList[i]->Next();
	}	
}

void CDBTable::Prev()
{
	for(unsigned i=0;i<m_fieldList.size();i++) {
		m_fieldList[i]->Prev();
	}	
}

const CDataField* CDBTable::operator[] (const CString& fieldName) const
{
	return CDBTable::getField(fieldName,&m_fieldList);
}

void CDBTable::closeTable()
{
	unsigned i;
	for(i=0;i<m_fieldList.size();i++) {
		delete m_fieldList[i];
	}
	m_fieldList.clear();
	m_opened=false;	
}

bool CDBTable::openTable(CError& err)
{
	char* dao;
	CDB::DAL_var dal=CDB::DAL::_nil();
	if (!err.isNoError()) return false;
	if ((m_containerServices==NULL) && (m_simpleClient==NULL)) {
		_SET_ERROR(err,CError::DBType,CError::TableLocation,"CDBTable::openTable()");
		return false;
	}
	if (m_containerServices!=NULL) {
		try {
			dal=m_containerServices->getCDB();
		}
		catch (acsErrTypeContainerServices::CanNotGetCDBExImpl& ex) {
			_SET_ERROR(err,CError::DBType,CError::DalLocation,"CDBTable::openTable()");
			return false;
		}
			
	}
	else if (m_simpleClient!=NULL) {
		try {
			CORBA::Object_var obj=m_simpleClient->getComponent("CDB",0,false);
			dal=CDB::DAL::_narrow(obj);
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_SET_ERROR(err,CError::DBType,CError::DalLocation,"CDBTable::openTable()");
			return false;			
		}
	}
	if (dal==CDB::DAL::_nil()) {
		_SET_ERROR(err,CError::DBType,CError::TableLocation,"CDBTable::openTable()");
		return false;		
	}
	try {
		dao=dal->get_DAO((const char*)m_fullName);
	}
	catch(...) {
		_SET_ERROR(err,CError::DBType,CError::TableLocation,"CDBTable::openTable()");
		return false;				
	}
	//now release the reference
	if (m_containerServices!=NULL) {
		try {
			m_containerServices->releaseComponent("CDB");
		}
		catch (...) { // take no countermesure
		}
	}
	else if (m_simpleClient!=NULL) {
		try {
			m_simpleClient->releaseComponent("CDB");
		}
		catch (...) { //take no countermeasure
		}
	}
	XML_Parser parser=XML_ParserCreate(NULL);
	if (!parser) {
		_SET_ERROR(err,CError::DBType,CError::Memory4Parser,"CDBTable::openTable()");
		return false;				
	}
	m_recCounter=0;
	XML_SetElementHandler(parser,startHandler,endHandler);
	XML_SetCharacterDataHandler(parser,charHandler);
	m_parserStruct.fieldList=&m_fieldList;
	m_parserStruct.actualField=NULL;
	m_parserStruct.tableName=m_tableName;
	m_parserStruct.started=false;
	m_parserStruct.fieldValue="";
	m_parserStruct.counter=m_recCounter;
	XML_SetUserData(parser,&m_parserStruct);
	if (XML_Parse(parser,dao,strlen(dao),true)==0) {
		_SET_ERROR(err,CError::DBType,CError::ParserError,"CDBTable::openTable()");
		return false;	
	}
	XML_ParserFree(parser);
	m_recCounter=m_parserStruct.counter;
	First();
	m_opened=true;
	return m_opened;
}

void CDBTable::startHandler(void* data,const XML_Char* el, const XML_Char** attr)
{
	TParserStruct *ps;
	CDataField *field;
	ps=(TParserStruct*) data;	
	if (ps->started) {
		field=getField(CString(el),ps->fieldList);
		if (field!=NULL) {
			ps->actualField=field;
		}
	}	
	else if (ps->tableName==el) {
		ps->started=true;
		ps->counter++;
		for(unsigned i=0;i<ps->fieldList->size();i++) {
			(*ps->fieldList)[i]->addValue();	
		}
		for(unsigned t=0;attr[t];t+=2) {
			if ((field=getField(CString(attr[t]),ps->fieldList))!=NULL) {
				field->setValue(attr[t+1]);
			}
		}
	}
}

void CDBTable::endHandler(void* data,const XML_Char* el)
{
	TParserStruct *ps;
	ps=(TParserStruct*) data;
	if (ps->tableName==el) {
		ps->started=false;
		ps->actualField=NULL;
	}
	else if ((ps->started) && (ps->actualField!=NULL) && (ps->actualField->m_title==el)) {
		ps->actualField->setValue(ps->fieldValue);
		ps->actualField=NULL;
		ps->fieldValue="";
	}
}

void CDBTable::charHandler(void* data,const XML_Char* s,int len)
{
	char tmp[2048];
	TParserStruct *ps;
	ps=(TParserStruct*) data;	
	strncpy(tmp,s,len);	
	tmp[len]=0;
	if ((ps->actualField!=NULL) && (len>0)) {
		ps->fieldValue+=tmp;
		//ps->actualField->setValue(CString(tmp));
	}
}

CDataField* CDBTable::getField(const CString& name,const TFields *fields)
{
	unsigned i;
	for (i=0;i<fields->size();i++) {
		if ((*fields)[i]->m_title==name) {
			return (*fields)[i];
		}
	}
	return NULL;
}
