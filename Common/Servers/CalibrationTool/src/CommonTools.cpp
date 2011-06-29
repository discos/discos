// $Id: CommonTools.cpp,v 1.4 2011-04-05 13:02:14 c.migoni Exp $

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

void CCommonTools::getObservatory(Antenna::Observatory_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (
		ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Antenna::Observatory::_nil();
	}	
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {  
			ref=services->getDefaultComponent<Antenna::Observatory>((const char*)name);
			errorDetected=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getObservatory()");
			Impl.setComponentName((const char*)name);
			throw Impl;		
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getObservatory()");
			Impl.setComponentName((const char*)name);
			throw Impl;		
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommonTools::getObservatory()");
			Impl.setComponentName((const char*)name);
			throw Impl;				
		}
	}	
}

void CCommonTools::unloadObservatory(Antenna::Observatory_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (!CORBA::is_nil(ref)) {
		IRA::CString name((const char *)ref->name());
		try {
			services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,impl,ex,"CCommonTools::unloadObservatory()");
			impl.setComponentName((const char *)name);
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCommonTools::unloadObservatory())");
			throw impl;
		}
		ref=Antenna::Observatory::_nil();
	}	
}
