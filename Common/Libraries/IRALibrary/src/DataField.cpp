// $Id: DataField.cpp,v 1.2 2007-10-24 15:09:50 a.orlati Exp $

#include "DataField.h"

using namespace IRA;

CDataField::CDataField(const CString& name,const TFieldType& type) : m_title(name), m_type(type), m_listPointer(-1)
{
	m_valueList.clear();
	m_listPointer=-1;
}

CDataField::~CDataField()
{
	for(unsigned i=0;i<m_valueList.size();i++) {
		if (m_valueList[i]) delete m_valueList[i];
	}
	m_valueList.clear();
}

bool CDataField::isNull() const
{
	if (m_listPointer>=0) {
		IRA::CString *tmp=m_valueList[m_listPointer];
		if ((*tmp)!="") return true;
		else return false;
	}
	else {
		return true;
	}
}

IRA::CString CDataField::getColumnName() const
{
	return m_title;
}

IRA::CString CDataField::asString() const
{
	IRA::CString *tmp;
	if (m_listPointer>=0) {
		tmp=m_valueList[m_listPointer];
		if (tmp==NULL) return IRA::CString("");
		else return *tmp;
	}
	else {
		return IRA::CString("");
	}
}

long long CDataField::asLongLong() const
{
	IRA::CString *tmp;
	char *pend;
	if (m_listPointer>=0) {
		tmp=m_valueList[m_listPointer];
		if (tmp==NULL) return 0;
		return (strtoll((const char *)(*tmp),&pend,10));
	}
	else {
		return 0;
	}	
}

DDWORD CDataField::asDoubleDoubleWord() const
{
	IRA::CString *tmp;
	char *pend;
	if (m_listPointer>=0) {
		tmp=m_valueList[m_listPointer];
		if (tmp==NULL) return 0;
		return (strtoul((const char *)(*tmp),&pend,10));
	}
	else {
		return 0;
	}		
}

double CDataField::asDouble() const
{
	IRA::CString *tmp;
	char *pend;
	if (m_listPointer>=0) {
		tmp=m_valueList[m_listPointer];
		if (tmp==NULL) return 0.0;
		return (strtod((const char *)(*tmp),&pend));
	}
	else {
		return 0.0;
	}	
}

void CDataField::addValue()
{
//	m_valueList.push_back(new baci::BACIValue());
	m_valueList.push_back(new IRA::CString(""));
}

void CDataField::setValue(const CString& value)
{
	IRA::CString *tmp=m_valueList.back();
	(*tmp)=value;
	/*if (value!="") {
		m_valueList.back()->setValue((const char*)value);
		switch (m_type) {
			case LONGLONG : {
				m_valueList.back()->setType(baci::BACIValue::type_longLong);
				//m_valueList.back()->setValue((const BACIlongLong)atoll((const char*)value));
			}
			case DOUBLEDOUBLEWORD : {
				m_valueList.back()->setType(baci::BACIValue::type_uLongLong);
				m_valueList.back()->setValue((const BACIuLongLong)atoll((const char*)value));
				break;
			}	
			case DOUBLE : {
				m_valueList.back()->setType(baci::BACIValue::type_double);
				m_valueList.back()->setValue(atof((const char*)value));
				break;
			}
			case STRING : {
				m_valueList.back()->setType(baci::BACIValue::type_string);
				m_valueList.back()->setValue((const char*)value);
			}	
		}
	}*/
}

void CDataField::setPointer(const WORD& pos)
{
	if ((pos>0) || (pos<m_valueList.size())) {
		m_listPointer=pos;
	}
}

void CDataField::First()
{
	m_listPointer=0;
}

void CDataField::Last()
{
	m_listPointer=m_valueList.size()-1;
}

void CDataField::Next()
{
	if ((unsigned int)m_listPointer<m_valueList.size()-1) {
		m_listPointer++;
	}
}

void CDataField::Prev()
{
	if (m_listPointer>=1) {
		m_listPointer--;
	}
}
