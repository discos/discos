#ifndef LOCALOSCILLATORIMPL_H
#define LOCALOSCILLATORIMPL_H

#include <baci.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <LocalOscillatorInterfaceS.h>
#include <baciSmartPropertyPointer.h>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>
#include <cstdlib>
#include <ni488.h>
#include "CommandLine.h"
#include <Definitions.h>
#include "DevIOfrequency.h"
#include "DevIOamplitude.h"

#include <IRA>

//#define GPIBBOARD 0
//#define PRIMARYADDRESS 10
//#define SECONDARYADDRESS 0
//#define TIMEOUT T10s



using namespace baci;
using namespace maci;
using namespace IRA;


class LocalOscillatorImpl : public CharacteristicComponentImpl, public virtual POA_Receivers::LocalOscillator

{

  public:
	LocalOscillatorImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);
	virtual ~LocalOscillatorImpl();
        virtual void cleanUp() throw (ACSErr::ACSbaseExImpl);
	virtual void aboutToAbort() throw (ACSErr::ACSbaseExImpl);
        virtual void execute() throw (ACSErr::ACSbaseExImpl);
        virtual void initialize()   throw (ACSErr::ACSbaseExImpl);

	virtual void set(CORBA::Double rf_ampl, CORBA::Double rf_freq) throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx);
	virtual void get (CORBA::Double& rf_ampl, CORBA::Double& rf_freq)  throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx);
 	virtual void rfon ()  throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx);
	virtual void rfoff ()  throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx);

    virtual ACS::ROdouble_ptr  frequency ()	throw (CORBA::SystemException);
	virtual ACS::ROdouble_ptr  amplitude ()	throw (CORBA::SystemException);
	virtual ACS::ROlong_ptr  isLocked () throw (CORBA::SystemException);
	
	inline void getStoredValue(double& pow,double& rf) { rf=m_freq;pow=m_ampl; };

private:
	SmartPropertyPointer<ROdouble> m_frequency;
	SmartPropertyPointer<ROdouble> m_amplitude;
	SmartPropertyPointer<ROlong> m_isLocked;
	int m_device; //GPIB Device
 	CommandLine* m_line;
 	CSecureArea<CommandLine> *m_commandLine;
 	bool m_gpibonline;
 	double m_freq;
 	double m_ampl;


};

#include "DevIOislocked.h"

#endif

