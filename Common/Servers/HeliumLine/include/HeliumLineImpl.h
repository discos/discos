/*
 * HeliumLineImpl.h
 *
 *  Created on: Mar 21, 2014
 *      Author: spoppi
 */

#ifndef HELIUMLINEIMPL_H_
#define HELIUMLINEIMPL_H_
#include <IRA>

#include <baci.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <HeliumLineInterfaceS.h>
#include <baciSmartPropertyPointer.h>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>
#include <cstdlib>
#include <modbus/modbus.h>

#include "CommandLine.h"
#include <Definitions.h>
#include "DevIOsupplyPressure.h"
#include "DevIOreturnPressure.h"

using namespace baci;
using namespace maci;
using namespace IRA;


class HeliumLineImpl : public CharacteristicComponentImpl, public virtual POA_Supervision::HeliumLine
{
public:
	HeliumLineImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);
	 virtual ~HeliumLineImpl();

	 virtual void cleanUp() throw (ACSErr::ACSbaseExImpl);
     virtual void aboutToAbort() throw (ACSErr::ACSbaseExImpl);
     virtual void execute() throw (ACSErr::ACSbaseExImpl);
     virtual void initialize()   throw (ACSErr::ACSbaseExImpl);


     virtual void getSupplyPressure(CORBA::Double& supplyPrs)   throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx);
     virtual void getReturnPressure(CORBA::Double& returnPrs)   throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx);

private:

     SmartPropertyPointer<ROdouble> m_supplyPressure;
     SmartPropertyPointer<ROdouble> m_returnPressure;
     SmartPropertyPointer<ROdouble> m_amplitude;

  	CommandLine* m_line;
  	CSecureArea<CommandLine> *m_commandLine;



};


#endif /* HELIUMLINEIMPL_H_ */
