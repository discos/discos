#ifndef OBSERVATORYCLIENT_H
#define OBSERVATORYCLIENT_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $id$                                                                                                 */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 12/10/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 04/08/2010      removed all commands a part from the "exit"       */


#include <MedicinaMountC.h>
#include <maciSimpleClient.h>
#include <Thread.h>
#include <IRA>
#include <TW_Tools.h>
#include <TW_Frame.h>
#include <TW_PropertyText.h>
#include <TW_Label.h>
#include <TW_InputCommand.h>
#include <TW_PropertyLedDisplay.h>
#include <TW_PropertyStatusBox.h>
#include <TW_StatusBox.h> 
#include <TW_LedDisplay.h>
#include <ClientErrors.h>

#define GUARDINTERVAL 1000

#define _GET_ACS_PROPERTY(TYPE,NAME,COMPONENT)  TYPE##_var NAME; \
{	\
	ACS_SHORT_LOG((LM_INFO,"Trying to get property "#NAME"...")); \
	NAME=COMPONENT->NAME(); \
	if (NAME.ptr()!=TYPE::_nil()) { \
		ACS_LOG(LM_SOURCE_INFO,"mountTui::Main()",(LM_DEBUG,"OK, reference is: %lx",(unsigned long)NAME.ptr())); \
	} \
	else { \
		_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"mountTui::Main()"); \
		impl.setPropertyName(#NAME); \
		impl.log(); \
		goto ErrorLabel; \
	} \
}

#define _INSTALL_MONITOR(COMP,TRIGGERTIME) { \
	if (!COMP->installAutomaticMonitor(GUARDINTERVAL)) { \
		_EXCPT(ClientErrors::CouldntPerformActionExImpl,impl,"mountTui::Main()"); \
		impl.setAction("Install monitor"); \
		impl.setReason((const char*)COMP->getLastError()); \
		impl.log(); \
		ACE_OS::sleep(1); \
		goto ErrorLabel; \
	} \
	COMP->setTriggerTime(TRIGGERTIME); \
}

#define _CATCH_ALL(OUTPUT,ROUTINE) \
	catch (ComponentErrors::ComponentErrorsEx &E) { \
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,ROUTINE); \
		IRA::CString Message; \
		_EXCPT_TO_CSTRING(Message,impl); \
		OUTPUT->setValue(Message); \
		OUTPUT->Refresh(); \
		impl.log(); \
	} \
	catch (CORBA::SystemException &C) { \
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,ROUTINE); \
		impl.setName(C._name()); \
		impl.setMinor(C.minor()); \
		IRA::CString Message; \
		_EXCPT_TO_CSTRING(Message,impl); \
		OUTPUT->setValue(Message); \
		OUTPUT->Refresh(); \
		impl.log(); \
	} \
	catch (...) { \
		_EXCPT(ClientErrors::UnknownExImpl,impl,ROUTINE); \
		IRA::CString Message; \
		_EXCPT_TO_CSTRING(Message,impl); \
		OUTPUT->setValue(Message); \
		OUTPUT->Refresh(); \
		impl.log(); \
	}


enum TTracking {
	TRACKING=1,
	SLEWING=2
};

class CCallbackVoid : public virtual POA_ACS::CBvoid
{
public:
	CCallbackVoid() {}
	~CCallbackVoid() {}
	void working (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	void done (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	CORBA::Boolean negotiate (ACS::TimeInterval time_to_transmit, const ACS::CBDescOut &desc) throw (CORBA::SystemException) {
		return true;
	}
	void setControls(const void* control,const void* output) { m_control=control; m_output=output; }
private:
	const void* m_control;
	const void* m_output; 
};

#endif
