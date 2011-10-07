// $Id: String.cpp,v 1.4 2010-09-20 11:48:20 a.orlati Exp $

#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "String.h"

using namespace IRA;

CString::CString() : m_iSize(0), m_cpString(NULL)
{
}

CString::CString(const CString & src)
{
	if (src.m_cpString) {
		m_cpString=(char *)new char[src.m_iSize+1];
		assert(m_cpString);
		strcpy(m_cpString,src.m_cpString);
		m_iSize=src.m_iSize;
	}
	else {
		m_iSize=0;
		m_cpString=NULL;
	}
}

CString::CString(const char *src)
{
	if (src) {
		m_cpString=(char *)new char[strlen(src)+1];
		assert(m_cpString);
		strcpy(m_cpString,src);
		m_iSize=strlen(m_cpString);
	}
	else {
		m_cpString=NULL;
		m_iSize=0;
	}
}

CString::CString(char ch)
{
	m_cpString=(char *) new char[2];
	assert(m_cpString);
	m_cpString[0]=ch;
	m_cpString[1]=0;
	m_iSize=1;
}

CString::CString(ACE_CString str)
{
	const char *src;
	src=str.c_str();
	if (src) {
		m_cpString=(char *)new char[strlen(src)+1];
		assert(m_cpString);
		strcpy(m_cpString,src);
		m_iSize=strlen(m_cpString);
	}
	else {
		m_cpString=NULL;
		m_iSize=0;
	}
}

CString::CString(const int& max,const char *format,...) : m_iSize(0), m_cpString(NULL)
{
	char *str;
	int size;
	int msize;
	if (max>0) {
		msize=max;	
	}
	else {
		msize=8192;
	}
	str=(char *)new char[msize];		
	va_list ap;
	va_start(ap,format);	
	size=vsnprintf(str,msize,format,ap);
	va_end(ap);
	if (size>-1) {
		m_cpString=(char *)new char[size+1];
		assert(m_cpString);
		strcpy(m_cpString,str);
		m_iSize=strlen(m_cpString);
	}
	delete []str;
}

CString::CString(int num)
{
	m_cpString=(char *)new char[num+1];
	assert(m_cpString);
	m_iSize=num;
	memset(m_cpString,32,num);
	m_cpString[num]=0;
}

CString::~CString()
{
	if (m_cpString) {
		delete [] m_cpString;
		m_cpString=NULL;
		m_iSize=0;
	}
}

int CString::GetLength() const
{
	return m_iSize;
}

bool CString::IsEmpty() const
{
	return (m_iSize==0);
}

void CString::Empty()
{
	if (m_cpString) delete [] m_cpString;
	m_cpString=NULL;
	m_iSize=0;
}


CString & CString::Concat(const CString & add)
{
	char *Tmp;
	if (add.m_iSize==0) return *this;
	Tmp=(char *)new char[m_iSize+add.m_iSize+1];
	assert(Tmp);
	if (m_cpString) {
		strcpy(Tmp,m_cpString);
		delete [] m_cpString;
	}
	strcat(Tmp,add.m_cpString);
	m_cpString=Tmp;
	m_iSize=strlen(m_cpString);
    return *this;
}

CString & CString::Concat(const char *add)
{
	char *Tmp;
	if (!add) return *this;
	Tmp=(char *)new char[m_iSize+strlen(add)+1];
	assert(Tmp);
	if (m_cpString) {
		strcpy(Tmp,m_cpString);
		delete [] m_cpString;
	}
	strcat(Tmp,add);
	m_cpString=Tmp;
	m_iSize=strlen(m_cpString);
    return *this;
}

CString & CString::Concat(char add)
{
	Concat(&add);
    return *this;
}

CString & CString::Concat(int nValue)
{
	char str[255];
	snprintf((char*)str,255,"%d",nValue);
	Concat(str);
    return *this;
}

CString & CString::Concat(double dValue)
{
	char str[1024];
	snprintf((char*)str,1024,"%f",dValue);
	Concat(str);
    return *this;
}

CString & CString::Concat(double dValue,int digits)
{
	char str[1024];
	char frm[16];
	snprintf((char*)str,16,"%d",digits);
	frm[0]='\%';
	frm[1]='.';
	frm[2]=0;
	strcat(frm,str);
	strcat(frm,"f");
	snprintf((char*)str,1024,frm,dValue);
	Concat(str);
    return *this;
}

CString & CString::Concat(long double dValue)
{
	char str[1024];
	snprintf((char*)str,1024,"%Lf",dValue);
	Concat(str);
    return *this;
}

CString & CString::Concat(long double dValue,int digits)
{
	char str[1024];
	char frm[16];
	snprintf((char*)str,16,"%d",digits);
	frm[0]='\%';
	frm[1]='.';
	frm[2]=0;
	strcat(frm,str);
	strcat(frm,"Lf");
	snprintf((char*)str,1024,frm,dValue);
	Concat(str);
    return *this;
}

CString & CString::Concat(float fValue)
{
	char str[1024];
	snprintf((char*)str,1024,"%f",fValue);
	Concat(str);
    return *this;
}

CString & CString::Concat(float fValue,int digits)
{
	char str[1024];
	char frm[16];
	snprintf((char*)str,16,"%d",digits);
	frm[0]='\%';
	frm[1]='.';
	frm[2]=0;
	strcat(frm,str);
	strcat(frm,"f");
	snprintf((char*)str,1024,frm,fValue);
	Concat(str);
    return *this;
}

void CString::LTrim() 
{
	int i=0;
	while  (( i<m_iSize) &&  (isspace(m_cpString[i]))) {
		i++;
	}
	if (i>0) {
		char *tmp=new char[m_iSize+1];
		assert(tmp);
		memcpy(tmp,m_cpString+i,(m_iSize-i)+1);
		delete [] m_cpString;
		m_cpString=new char[(m_iSize-i)+1];
		assert(m_cpString);
		strcpy(m_cpString,tmp);
		delete [] tmp;
		m_iSize=strlen(m_cpString);
	}
}

void CString::RTrim()
{
	int i=m_iSize-1;
	while ((i>0) && (isspace(m_cpString[i]))) {
		i--;
	}
	if (i<m_iSize-1) {
		char *tmp=new char[m_iSize+1];
		assert(tmp);
		memcpy(tmp,m_cpString,i+1);
		tmp[i+1]=0;
		delete [] m_cpString;
		m_cpString=new char[strlen(tmp)+1]; // number of character of tmp plus teh trailing '\0'
		assert(m_cpString);
		strcpy(m_cpString,tmp);
		delete [] tmp;
		m_iSize=strlen(m_cpString);		
	}
}

void CString::Format(const char *Frm,...)
{
	char str[8192];
	int size;
	va_list ap;
	va_start(ap,Frm);
	size=vsnprintf(str,8192,Frm,ap);
	va_end(ap);
	if (size>-1) {
		Empty();
		m_cpString=(char *)new char[size+1];
		assert(m_cpString);
		strcpy(m_cpString,str);
		m_iSize=strlen(m_cpString);
	}
}

char CString::CharAt(int index) const
{
	if ((index<0)||(index>=m_iSize)) return 0;
	return m_cpString[index];
}

void CString::SetAt(int index,char ch)
{
	if ((index<0)||(index>=m_iSize)) return;
	m_cpString[index]=ch;
}

int CString::Compare(const char *str) const
{
	if ((!str) && (!m_cpString)) return 0;
	if (!str) return 1;
	if (!m_cpString) return -1;
	return strcmp(m_cpString,str);
}

int CString::CompareNoCase(const char *str) const
{
	if ((!str) && (!m_cpString)) return 0;
	if (!str) return 1;
	if (!m_cpString) return -1;
	return strcasecmp(m_cpString,str);
}

CString CString::Left(int count) const
{
	CString Tmp;
	if (count<0) return Tmp;
	if (count>m_iSize) count=m_iSize;
	if (count==0) return Tmp;
	Tmp.m_cpString=(char *)new char[count+1];
	assert(Tmp.m_cpString);
	strncpy(Tmp.m_cpString,m_cpString,count);
	Tmp.m_cpString[count]=0;
	Tmp.m_iSize=count;
	return Tmp;
}

CString CString::Right(int count) const
{
	CString Tmp;
	if (count<0) return Tmp;
	if (count>m_iSize) count=m_iSize;
	if (count==0) return Tmp;
	Tmp.m_cpString=(char *)new char[count+1];
	assert(Tmp.m_cpString);
	strcpy(Tmp.m_cpString,m_cpString+(m_iSize-count));
	Tmp.m_cpString[count]=0;
	Tmp.m_iSize=count;
	return Tmp;
}

CString CString::Mid(int start, int count) const
{
	CString Tmp;
	if (count<0) return Tmp;
	if (start<0) start=0;
	if (start>m_iSize) start=m_iSize;
	if (count>(m_iSize-start)) count=m_iSize-start;
	if (count==0) return Tmp;
	Tmp.m_cpString=(char *) new char[count + 1];
	assert(Tmp.m_cpString);
	strncpy(Tmp.m_cpString,m_cpString+start,count);
	Tmp.m_cpString[count]=0;
	Tmp.m_iSize=count;
	return Tmp;
}

void CString::MakeUpper()
{
	register int i;
	for (i=0;i<m_iSize;i++) m_cpString[i]=toupper(m_cpString[i]);
}

void CString::MakeLower()
{
	register int i;
	for (i=0;i<m_iSize; i++) m_cpString[i]=tolower(m_cpString[i]);
}

int CString::Find(char ch,int start) const
{
	if (start<0) start=0;
	if (start>=m_iSize) return -1;
	for (int i=start;i<m_iSize;i++) {
		if (m_cpString[i]==ch) return i;
	}
	return -1;
}

int CString::Find(const char *str, int start) const
{
	if (!str) return -1;
	if (start<0) start=0;
	int StrLen=strlen(str);
	if (start>=(m_iSize-StrLen)) return -1;
	for (int i=start;i<(m_iSize-StrLen);i++) {
		if (strncmp(&m_cpString[i],str,StrLen)==0) return i;
	}
	return -1;
}

int CString::Find(char ch) const
{
	return Find(ch,0);
}

int CString::Find(const char *str) const
{
	return Find(str,0);
}

int CString::Replace(char ch1,char ch2,int start)
{
	if (ch1==ch2) return -1;
	if (!m_cpString) return -1;
	if (start<0) start = 0;
	int FirstPos=-1;
	for (int i=start;i<m_iSize;i++) {
		if (m_cpString[i]==ch1) {
			m_cpString[i]=ch2;
			if (FirstPos<0) FirstPos=i;
		}
	}
	return FirstPos;
}

int CString::ToInt() const
{
	if (IsEmpty()) return -1;
	return atoi(m_cpString);
}

long CString::ToLong() const
{
	if (IsEmpty()) return -1;
	return atol(m_cpString);	
}

long long CString::ToLongLong() const
{
	if (IsEmpty()) return -1;
	return atoll(m_cpString);
}


float CString::ToFloat() const
{
	if (IsEmpty()) return 0.0;
	return (float)atof(m_cpString);
}

double CString::ToDouble() const
{
	if (IsEmpty()) return 0.0;
	return atof(m_cpString);
}

long double CString::ToLongDouble() const
{
	if (IsEmpty()) return 0.0;
	return (long double)atof(m_cpString);
}

//************** OPERATORS DEFINITIONS *****************

char CString::operator [] (int index) const
{
	return CharAt(index);
}

CString::operator const char *() const
{
	return m_cpString;
}

const CString & CString::operator =(const CString & src)
{
	Empty();
	if (src.m_cpString) {
		m_cpString=(char *)new char[src.m_iSize+1];
		assert(m_cpString);
		strcpy(m_cpString,src.m_cpString);
		m_iSize=src.m_iSize;
	}
	return *(this);
}

const CString & CString::operator =(char ch)
{
	Empty();
	m_iSize=1;
	m_cpString=(char *) new char[2];
	m_cpString[0]=ch;
	m_cpString[1]=0;
	return *this;
}

const CString & CString::operator =(const char *str)
{
	Empty();
	if (str) {
		m_cpString=(char *)new char[strlen(str)+1];
		assert(m_cpString);
		strcpy(m_cpString,str);
		m_iSize=strlen(m_cpString);
	}
	return *this;
}

CString IRA::operator +(CString const& s1,CString const& s2)
{
	CString Tmp(s1);
	Tmp.Concat(s2);
	return Tmp;
}

CString IRA::operator +(const CString & s1, char ch)
{
	CString Tmp(s1);
	Tmp.Concat(ch);
	return Tmp;
}

CString IRA::operator +(const CString & s1, const char *str)
{
	CString Tmp(s1);
	Tmp.Concat(str);
	return Tmp;
}

CString IRA::operator +(char ch, const CString & s2)
{
	CString Tmp(ch);
	Tmp.Concat(s2);
	return Tmp;
}

CString IRA::operator +(const char *str, const CString & s2)
{
	CString Tmp(str);
	Tmp.Concat(s2);
	return Tmp;
}

const CString & CString::operator +=(const CString & src)
{
	Concat(src);
	return *this;
}

const CString & CString::operator +=(char ch)
{
	Concat(ch);
	return *this;
}

const CString & CString::operator +=(const char *str)
{
	Concat(str);
	return *this;
}

bool IRA::operator ==(const CString & s1, const CString & s2)
{
	return (s1.Compare(s2) == 0);
}

bool IRA::operator ==(const CString & s1, const char *s2)
{
	return (s1.Compare(s2) == 0);
}

bool IRA::operator ==(const char *s1, const CString & s2)
{
	return (s2.Compare(s1) == 0);
}

bool IRA::operator !=(const CString & s1, const CString & s2)
{
	return (s1.Compare(s2));
}

bool IRA::operator !=(const CString & s1, const char *s2)
{
	return (s1.Compare(s2));
}

bool IRA::operator !=(const char *s1, const CString & s2)
{
	return (s2.Compare(s1));
}

bool IRA::operator <(const CString & s1, const CString & s2)
{
	return (s1.Compare(s2) < 0);
}

bool IRA::operator <(const CString & s1, const char *s2)
{
	return (s1.Compare(s2) < 0);
}

bool IRA::operator <(const char *s1, const CString & s2)
{
	return (s2.Compare(s1) >= 0);
}

bool IRA::operator >(const CString & s1, const CString & s2)
{
	return (s1.Compare(s2) > 0);
}

bool IRA::operator >(const CString & s1, const char *s2)
{
	return (s1.Compare(s2) > 0);
}

bool IRA::operator >(const char *s1, const CString & s2)
{
	return (s2.Compare(s1) <= 0);
}

bool IRA::operator <=(const CString & s1, const CString & s2)
{
	return (s1.Compare(s2) <= 0);
}

bool IRA::operator <=(const CString & s1, const char *s2)
{
	return (s1.Compare(s2) <= 0);
}

bool IRA::operator <=(const char *s1, const CString & s2)
{
	return (s2.Compare(s1) > 0);
}

bool IRA::operator >=(const CString & s1, const CString & s2)
{
	return (s1.Compare(s2) >= 0);
}

bool IRA::operator >=(const CString & s1, const char *s2)
{
	return (s1.Compare(s2) >= 0);
}

bool IRA::operator >=(const char *s1, const CString & s2)
{
	return (s2.Compare(s1) < 0);
}
