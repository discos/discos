#ifndef  _DBBC_H_
#define _DBBC_H_
#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <enumpropROImpl.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <baciROstring.h>
#include <baciROuLongLong.h>
#include <baciROdoubleSeq.h>
#include <baciROlongSeq.h>
#include <baciROpattern.h>
#include <bulkDataSenderImpl.h>
#include <DBBCS.h>
#include <ComponentErrors.h>
#include <BackendsErrors.h>
#include <SP_parser.h>
#include <ManagementErrors.h>
#include "Common.h"
#include "Cpld2IntfClient.h"


using namespace baci;
using namespace IRA;
using namespace ACSBulkDataError;

class DBBCImpl: public virtual BulkDataSenderDefaultImpl, public virtual POA_Backends::DBBC
{
public: 

   DBBCImpl(const ACE_CString &CompName,maci::ContainerServices    *containerServices);

virtual ~DBBCImpl(); 

virtual void LoadScan();

virtual void LoadVLBI();

virtual void initialize() throw (ACSErr::ACSbaseExImpl);
	
virtual void execute() throw (ACSErr::ACSbaseExImpl){ };
	
virtual void cleanUp(){ };
	
virtual void aboutToAbort(){ };

virtual void sendHeader() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, ComponentErrors::ComponentErrorsEx){ };

virtual void sendData (ACS::Time time) throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, ComponentErrors::ComponentErrorsEx){ };

virtual void sendStop() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,ComponentErrors::ComponentErrorsEx){ };

virtual void terminate() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, ComponentErrors::ComponentErrorsEx){ };

virtual void setSection (CORBA::Long input,CORBA::Double  freq,CORBA::Double bw,CORBA::Double sr,CORBA::Double att, CORBA::Long pol,CORBA::Long bins)throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){ };

virtual void setAttenuation (CORBA::Long input, CORBA::Double att) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){ };

virtual void enableChannels ( const ACS::longSeq & enable) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){ };

virtual void setIntegration (CORBA::Long integration) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){ };

virtual void setTime () throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){ };

virtual ACS::doubleSeq * getTpi () throw(CORBA::SystemException, ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){return 0;};

virtual ACS::doubleSeq * getZero () throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){return 0;};

virtual void setKelvinCountsRatio (const ACS::doubleSeq & ratio,
        const ACS::doubleSeq & tsys) throw (CORBA::SystemException){ };

virtual CORBA::Long getInputs (ACS::doubleSeq_out freq,ACS::doubleSeq_out bandwidth, ACS::longSeq_out feed,ACS::longSeq_out ifNumber) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){return 0;};

virtual ACS::ROdoubleSeq_ptr systemTemperature() throw(CORBA::SystemException);
		

virtual ACS::ROlong_ptr sectionsNumber() throw (CORBA::SystemException); 
		

virtual ACS::ROlongSeq_ptr inputSection() throw (CORBA::SystemException);

//virtual char * command(const char *configCommand) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx){IRA::CString out;return CORBA::string_dup((const char *)out);};  
		

virtual void setSection (CORBA::Long input,CORBA::Double freq,CORBA::Double bw, CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){ };

CORBA::Boolean command(const char *configCommand, CORBA::String_out answer) throw (CORBA::SystemException);

virtual ACS::ROuLongLong_ptr time() throw (CORBA::SystemException);
virtual ACS::ROstring_ptr backendName() throw (CORBA::SystemException);
virtual ACS::ROdoubleSeq_ptr bandWidth() throw (CORBA::SystemException);
virtual ACS::ROdoubleSeq_ptr frequency() throw (CORBA::SystemException);
virtual ACS::ROdoubleSeq_ptr sampleRate() throw (CORBA::SystemException);
virtual ACS::ROdoubleSeq_ptr attenuation() throw (CORBA::SystemException);
virtual ACS::ROlongSeq_ptr polarization() throw (CORBA::SystemException);
virtual ACS::ROlongSeq_ptr bins() throw (CORBA::SystemException);
virtual ACS::ROlongSeq_ptr feed() throw (CORBA::SystemException);
virtual ACS::ROlong_ptr inputsNumber() throw (CORBA::SystemException);
virtual ACS::ROlong_ptr integration() throw (CORBA::SystemException);
virtual ACS::ROpattern_ptr status() throw (CORBA::SystemException);
virtual Management::ROTBoolean_ptr busy() throw (CORBA::SystemException);
//virtual ACS::ROstring_ptr fileName() throw (CORBA::SystemException);





protected:

virtual void startSend() throw (CORBA::SystemException, AVStartSendErrorEx){ }; 
virtual void paceData() throw (CORBA::SystemException, AVPaceDataErrorEx){ };
virtual void stopSend() throw (CORBA::SystemException, AVStopSendErrorEx) { };
//virtual void Specific()throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx) { };
   

private:

Cpld2IntfClient *intf;
//Cpld2IntfClient intf("192.167.8.179", 4444); // qui la definisco, in modo che sia riconosciuto il tipo nel file cpp

struct THeaderRecord {
		Backends::TMainHeader header;
		Backends::TSectionHeader chHeader[MAX_INPUT_NUMBER];
	};

SmartPropertyPointer<ROuLongLong> m_ptime;
SmartPropertyPointer<ROstring> m_pbackendName;
SmartPropertyPointer<ROdoubleSeq> m_pbandWidth;
SmartPropertyPointer<ROdoubleSeq> m_pfrequency;
SmartPropertyPointer<ROdoubleSeq> m_psampleRate;
SmartPropertyPointer<ROdoubleSeq> m_pattenuation;
SmartPropertyPointer<ROlongSeq> m_ppolarization;
SmartPropertyPointer<ROlongSeq> m_pbins;
SmartPropertyPointer<ROlong> m_pinputsNumber;
SmartPropertyPointer<ROlong> m_pintegration;
SmartPropertyPointer<ROpattern> m_pstatus;
SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),
POA_Management::ROTBoolean>  > m_pbusy;
SmartPropertyPointer<ROlongSeq> m_pfeed;
SmartPropertyPointer<ROdoubleSeq> m_ptsys;
SmartPropertyPointer<ROlong> m_psectionsNumber;
SmartPropertyPointer<ROlongSeq> m_pinputSection;


      

};

#endif /*DBBC_H_*/
