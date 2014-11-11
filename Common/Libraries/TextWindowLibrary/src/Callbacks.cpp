// $Id: Callbacks.cpp,v 1.6 2010-09-13 10:30:50 a.orlati Exp $

#include "TW_Callbacks.h"
#include <math.h>

using namespace TW;

IRA::CString CFormatFunctions::angleFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted="";
	baci::BACIValue::Type currentType;
	currentType=value.getType();
	if ((currentType==baci::BACIValue::type_double)) {
		double app;
		app=value.doubleValue();
		if (arg!=NULL) {  // hour angle
			IRA::CIRATools::radToHourAngle(app,formatted);			
		}
		else {
			IRA::CIRATools::radToSexagesimalAngle(app,formatted);
		}
	}
	else {
		formatted="????";
	}
	return formatted;		
}

IRA::CString CFormatFunctions::coordinateFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted="";
	baci::BACIValue::Type currentType;
	currentType=value.getType();
	if ((currentType==baci::BACIValue::type_double)) {
		double app;
		app=value.doubleValue();
		IRA::CIRATools::radToAngle(app,formatted);			
	}
	else {
		formatted="????";
	}
	return formatted;		
}

IRA::CString CFormatFunctions::defaultFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted;
	//new implementation to fix a segmentation fault when the associated type is a string
	const char *p;
	if ((p=value.stringValue())!=NULL) formatted=IRA::CString(value.stringValue());
	else {
		ACE_CString temp;
		value.toString(temp);
		formatted=IRA::CString(temp);
	}
	/*ACE_CString temp;  // old implementation
	value.toString(temp);
	formatted=IRA::CString(temp);*/   
	return formatted;
}

IRA::CString CFormatFunctions::floatingPointFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted="";
	baci::BACIValue::Type currentType;
	currentType=value.getType();
	if ((currentType==baci::BACIValue::type_double)) {
		double app;
		app=value.doubleValue();
		if (arg==NULL) {
			formatted.Format("%lf",app);
		}
		else {
			formatted.Format(static_cast<const char*>(arg),app);
		}
	}
	else {
		formatted="????";
	}
	return formatted;	
}

IRA::CString CFormatFunctions::integerFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted="";
	baci::BACIValue::Type currentType;
	currentType=value.getType();
	if ((currentType==baci::BACIValue::type_long) ) {
		long app;
		app=value.longValue();
		if (arg==NULL) {
			formatted.Format("%ld",app);
		}
		else {
			formatted.Format(static_cast<const char*>(arg),app);
		}
	}
	else if ((currentType==baci::BACIValue::type_longLong) ) {
		long long app;
		app=value.longLongValue();
		if (arg==NULL) {
			formatted.Format("%ld",app);
		}
		else {
			formatted.Format(static_cast<const char*>(arg),app);
		}
	}
	else if ((currentType==baci::BACIValue::type_uLongLong) ) {
		unsigned long long app;
		app=value.uLongLongValue();
		if (arg==NULL) {
			formatted.Format("%lu",app);
		}
		else {
			formatted.Format(static_cast<const char*>(arg),app);
		}
	}	
	else {
		formatted="????";
	}
	return formatted;	
}

IRA::CString CFormatFunctions::dateTimeClockFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted="";
	baci::BACIValue::Type currentType;
	currentType=value.getType();
	switch(currentType) {
		case baci::BACIValue::type_uLongLong : {
			ACS::Time app;
			app=value.uLongLongValue();
			IRA::CIRATools::timeToStr(app,formatted);
			break;			
		}
		default : {
			formatted="????";
		}
	}
	return formatted;	
}

IRA::CString CFormatFunctions::timeClockFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted="";
	baci::BACIValue::Type currentType;
	currentType=value.getType();
	switch(currentType) {
		case baci::BACIValue::type_uLongLong : {
			ACS::TimeInterval app;
			app=value.uLongLongValue();
			IRA::CIRATools::intervalToStr(app,formatted);
			break;			
		}
		default : {
			formatted="????";
		}
	}
	return formatted;	
}

IRA::CString CFormatFunctions::clockFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted="";
	baci::BACIValue::Type currentType;
	currentType=value.getType();
	switch(currentType) {
		case baci::BACIValue::type_uLongLong : {
			ACS::TimeInterval app;
			app=value.uLongLongValue();
			TIMEDIFFERENCE tm(app);
			formatted.Format("%02d:%02d:%02d.%03d",tm.hour(),tm.minute(),tm.second(),(long)(tm.microSecond()/1000));
			break;			
		}
		default : {
			formatted="????";
		}
	}
	return formatted;		
}

CCommonCallback::CCommonCallback(const bool& isSequence) : m_formatFunction(&CFormatFunctions::defaultFormat), m_formatArgument(NULL), 
   m_started(false), m_frameComponent(NULL),m_isSequence(isSequence)
{
	m_started=false;
	m_frameComponent=NULL;
	m_formatFunction=&(CFormatFunctions::defaultFormat);
	m_formatArgument=NULL;
}

CCommonCallback::~CCommonCallback()
{
}

CFrameComponent *CCommonCallback::getFrameComponent() const 
{
	return m_frameComponent;
}

void CCommonCallback::setFrameComponent(CFrameComponent* comp)
{
	m_frameComponent=comp;
}

void CCommonCallback::Start()
{
	m_started=true;
}

void CCommonCallback::Stop()
{
	m_started=false;
}

bool CCommonCallback::isStopped() const
{
	return !m_started;
}

void CCommonCallback::setFormatFunction(TFormatFunction func,const void* arg)
{
	if (func==NULL) m_formatFunction=&(CFormatFunctions::defaultFormat);
	else m_formatFunction=func;
	m_formatArgument=arg;
}

