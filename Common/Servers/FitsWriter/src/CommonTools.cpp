// $Id: CommonTools.cpp,v 1.2 2011-04-18 13:42:37 a.orlati Exp $

#include "CommonTools.h"


void CCommonTools::getAntennaBoss(Antenna::AntennaBoss_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (
		ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Antenna::AntennaBoss::_nil();
	}	
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {  
			ref=services->getDefaultComponent<Antenna::AntennaBoss>((const char*)name);
			errorDetected=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getAntennaBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;		
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getAntennaBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;		
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getAntennaBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;				
		}
	}	
}

void CCommonTools::unloadAntennaBoss(Antenna::AntennaBoss_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (!CORBA::is_nil(ref)) {
		IRA::CString name((const char *)ref->name());
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,impl,ex,"CCommonTools::unloadAntennaBoss()");
			impl.setComponentName((const char *)name);
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCommonTools::unloadAntennaBoss())");
			throw impl;
		}
		ref=Antenna::AntennaBoss::_nil();
	}	
}

void CCommonTools::getReceiversBoss(Receivers::ReceiversBoss_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (
		ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Receivers::ReceiversBoss::_nil();
	}	
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {  
			ref=services->getDefaultComponent<Receivers::ReceiversBoss>((const char*)name);
			errorDetected=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getReceiversBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;		
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getReceiversBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;		
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getReceiversBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;				
		}
	}
}

void CCommonTools::unloadReceiversBoss(Receivers::ReceiversBoss_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (!CORBA::is_nil(ref)) {
		IRA::CString name((const char *)ref->name());
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,impl,ex,"CCommonTools::unloadReceiversBoss()");
			impl.setComponentName((const char *)name);
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCommonTools::unloadReceiversBoss())");
			throw impl;
		}
		ref=Receivers::ReceiversBoss::_nil();
	}
}

void CCommonTools::getMSBoss(MinorServo::MinorServoBoss_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (
		ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=MinorServo::MinorServoBoss::_nil();
	}
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {
			ref=services->getDefaultComponent<MinorServo::MinorServoBoss>((const char*)name);
			errorDetected=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getMSBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getMSBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getMSBoss()");
			Impl.setComponentName((const char*)name);
			throw Impl;
		}
	}
}

void CCommonTools::unloadMSBoss(MinorServo::MinorServoBoss_var& ref,maci::ContainerServices *& services) throw (
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (!CORBA::is_nil(ref)) {
		IRA::CString name((const char *)ref->name());
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,impl,ex,"CCommonTools::unloadMSBoss()");
			impl.setComponentName((const char *)name);
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCommonTools::unloadMSBoss())");
			throw impl;
		}
		ref=MinorServo::MinorServoBoss::_nil();
	}
}

void CCommonTools::getScheduler(Management::Scheduler_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (
		ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Management::Scheduler::_nil();
	}	
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {  
			ref=services->getDefaultComponent<Management::Scheduler>((const char*)name);
			errorDetected=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getScheduler()");
			Impl.setComponentName((const char*)name);
			throw Impl;		
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getScheduler()");
			Impl.setComponentName((const char*)name);
			throw Impl;		
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getScheduler()");
			Impl.setComponentName((const char*)name);
			throw Impl;				
		}
	}
}

void CCommonTools::unloadScheduler(Management::Scheduler_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (!CORBA::is_nil(ref)) {
		IRA::CString name((const char *)ref->name());
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,impl,ex,"CCommonTools::unloadScheduler()");
			impl.setComponentName((const char *)name);
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCommonTools::unloadScheduler())");
			throw impl;
		}
		ref=Management::Scheduler::_nil();
	}
}

void CCommonTools::getMeteoComponent(Weather::GenericWeatherStation_var& ref,maci::ContainerServices *& services,const IRA::CString& instance,bool& errorDetected) throw (
		ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Weather::GenericWeatherStation::_nil();
	}	
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {  
			ref=services->getComponent<Weather::GenericWeatherStation>((const char *)instance);
			errorDetected=false;
		}		
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getMeteoComponent()");
			Impl.setComponentName((const char*)instance);
			throw Impl;		
		}
	}
}

void CCommonTools::unloadMeteoComponent(Weather::GenericWeatherStation_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (!CORBA::is_nil(ref)) {
		IRA::CString name((const char *)ref->name());
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,impl,ex,"CCommonTools::unloadMeteoComponent()");
			impl.setComponentName((const char *)name);
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCommonTools::unloadMeteoComponent())");
			throw impl;
		}
		ref=Weather::GenericWeatherStation::_nil();
	}
}

void CCommonTools::map(const ACS::doubleSeq& seq,std::list<double>& va)
{
	for(unsigned i=0;i<seq.length();i++) {
		va.push_back(seq[i]);
	}
}

