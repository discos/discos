/* ***************************************************************** */
/* This code is under GNU General Public Licence (GPL)               */
/*                                                                   */
/* Who             When            What   				     */
/* Andrea Orlati   27/08/2026      Creation       			 */

#ifndef DATAWRITERIMPL_H_
#define DATAWRITERIMPL_H_

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <Cplusplus11Helper.h>
#define _CPLUSPLUS11_PORTING_

#include <String.h>
#include <DataReceiverS.h>



C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
C11_IGNORE_WARNING("-Wmisleading-indentation")
C11_IGNORE_WARNING("-Wcatch-value=")
C11_IGNORE_WARNING("-Wsequence-point")
#include <baci.h>
#include <acscomponentImpl.h>
#include <ComponentErrors.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <enumpropROImpl.h>
#include <baciROstring.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <baciROdoubleSeq.h>
C11_IGNORE_WARNING_POP
#include <bulkDataZMQReceiverImpl.h>
#include "ReceiverCallback.h"
#include "Configuration.h"
#include "DataCollection.h"
#include "EngineThread.h"
//#include "CollectorThread.h"

using namespace maci;
using namespace baci;

class DataWriterImpl : public virtual bulkdataZMQImpl::BulkDataZMQReceiverImpl<DataWriter_private::ReceiverCallback>,
                       public virtual POA_Management::DataReceiver
{
  public:

    /**
     * Constructor
     * @param poa Poa which will activate this and also all other Components.
     * @param name component name.
     */
    DataWriterImpl(const ACE_CString& name,ContainerServices* containerServices);

    /**
     * Destructor
     */
    virtual ~DataWriterImpl();
	/**
	 * @throw throw (ACSErr::ACSbaseExImpl);
	 */
    void initialize();

	/**
	 * @throw throw (ACSErr::ACSbaseExImpl);
	 */
	void cleanUp();
	
	/**
	 * This method is called by the container when the component is about to be aborted.
	 * It can be used to perform some cleanup operation before the component is destroyed.
	 */
	void aboutToAbort();
	 
    void execute();
    
    /**
     * Returns a reference to the fileName property implementation of IDL interface.
	 * @return pointer to read-only string property
	 * @throw CORBA::SystemException
	*/	
    virtual ACS::ROstring_ptr fileName();    
    
    /**
     * Returns a reference to the projectName property implementation of IDL interface.
	 * @return pointer to read-only string property
	 * @throw CORBA::SystemException
	*/
    virtual ACS::ROstring_ptr projectName();
    
    /**
     * Returns a reference to the observer property implementation of IDL interface.
	 * @return pointer to read-only string property
	 * @throw CORBA::SystemException
	*/
    virtual ACS::ROstring_ptr observer();

    /**
     * Returns a reference to the deviceID property implementation of IDL interface.
	 * @return pointer to read-only long property
	 * @throw CORBA::SystemException
	*/
    virtual ACS::ROlong_ptr deviceID();

	/**
     * Returns a reference to the scanAxis property Implementation of IDL interface.
	 * @return pointer to read-only ROTScanAxis property
	 * @throw CORBA::SystemException
	*/
	virtual Management::ROTScanAxis_ptr scanAxis();
	
	/**
	 * Returns a reference to the dataX property implementation of IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException
	*/
	virtual ACS::ROdouble_ptr dataX();
	
	/**
	 * Returns a reference to the dataY property implementation of IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException
	*/
	virtual ACS::ROdouble_ptr dataY();

	/**
	 * Returns a reference to the arrayDataX property implementation of IDL interface.
	 * @return pointer to read-only double sequence property
	 * @throw CORBA::SystemException
	*/
	virtual ACS::ROdoubleSeq_ptr arrayDataX();

	/**
	 * Returns a reference to the arrayDataY property implementation of IDL interface.
	 * @return pointer to read-only double sequence property
	 * @throw CORBA::SystemException
	 */
	virtual ACS::ROdoubleSeq_ptr arrayDataY();

	/**
	 * called to inform the component that a new scan is going to start
	 * @param  prm scan parameters structure
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @return the path that will contain the files of the current scan
	 */
	virtual char *startScan(const Management::TScanSetup & prm);

	/**
	 * called to give extra information about scan layout. Not used by this component.
	 * @param layout scan layout definition given as a sequence of strings
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
    virtual void setScanLayout (const ACS::stringSeq & layout);

	/**
	 * called to inform the component that a new scan is going to be closed
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
    virtual void stopScan();

	/**
	 * called to inform the component that a new subscan is going to start
	 * @parm prm subscan parameters structure
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @return the name of the file that will be processed by the system
	 */
    virtual char *startSubScan(const ::Management::TSubScanSetup & prm);

    /**
     * Return the recording status of the component.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
     */
    virtual CORBA::Boolean isRecording();

    /**
     * Called in order to reset component status and make sure it is ready for start a new data acquisition loop from the scratch.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
     */
    virtual void reset();
        
private:
	SmartPropertyPointer<ROstring> m_pfileName;	
	SmartPropertyPointer<ROstring> m_pprojectName;
	SmartPropertyPointer<ROstring> m_pobserver;	
	//SmartPropertyPointer<ROlong> m_pscanIdentifier;
	SmartPropertyPointer<ROlong> m_pdeviceID;	
	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TScanAxis),POA_Management::ROTScanAxis> > m_pscanAxis;
	SmartPropertyPointer<ROdouble> m_pdataX;
	SmartPropertyPointer<ROdouble> m_pdataY;	
	SmartPropertyPointer<ROdoubleSeq> m_parrayDataX;
	SmartPropertyPointer<ROdoubleSeq> m_parrayDataY;
	
	DataWriter_private::CConfiguration m_compConfig;

	DataWriter_private::CDataCollection *m_data;
	
	DataWriter_private::CEngineThread *m_workThread;
	/*DataWriter_private::CCollectorThread *m_collectThread;*/
};



#endif /*DATAWRITERIMPL_H_*/
