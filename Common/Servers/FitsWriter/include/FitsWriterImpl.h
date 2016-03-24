/* ***************************************************************** */
/* IRA Istituto di Radioastronomia                                   */
/* $Id: FitsWriterImpl.h,v 1.13 2011-04-22 18:51:48 a.orlati Exp $															 */
/* This code is under GNU General Public Licence (GPL)               */
/*                                                                   */
/* Who                      		   When            What   				     */
/* Simona Righini					   10/11/2008      created      			 */
/* Andrea Orlati(a.orlati@ira.inaf.it) 13/09/2010      new keyword added, interface different, since many fields are now read directly */
/* Andrea Orlati(a.orlati@ira.inaf.it) 28/02/2011      the frequency information are now reported referred to the real observed freuqncy not referred to the backend filter*/
/* Andrea Orlati(a.orlati@ira.inaf.it) 22/04/2011      Added support for sub directory creation based on current date */

#ifndef FITSWRITERIMPL_H_
#define FITSWRITERIMPL_H_

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/** 
 * @mainpage Header file of "FitsWriter" module
 * @date 22/04/2011
 * @version 0.87.0
 * @author <a href=mailto:righin_s@ira.inaf.it>Simona Righini</a>,
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,  
 * @remarks Last compiled under ACS 7.0.2
 * @remarks compiler version is 4.1.2 
 */

#include <baci.h>
#include <FitsWriterS.h>
#include <ComponentErrors.h>
#include <acscomponentImpl.h>
#include <String.h>
#include <SecureArea.h>
#include <bulkDataReceiverImpl.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <enumpropROImpl.h>
#include <baciROstring.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <baciROdoubleSeq.h>
//#include <acsncSimpleConsumer.h>
#include "ReceiverCallback.h"
#include "DataCollection.h"
#include "EngineThread.h"
#include "CollectorThread.h"

using namespace maci;
using namespace baci;

class FitsWriterImpl : public virtual BulkDataReceiverImpl<FitsWriter_private::ReceiverCallback>,
                       public virtual POA_Management::FitsWriter
{
  public:

    /**
     * Constructor
     * @param poa Poa which will activate this and also all other Components.
     * @param name component name.
     */
    FitsWriterImpl(const ACE_CString& name,ContainerServices* containerServices);

    /**
     * Destructor
     */
    virtual ~FitsWriterImpl();

    void initialize() throw (ACSErr::ACSbaseExImpl);

    void execute() throw (ACSErr::ACSbaseExImpl);
    
    void cleanUp();

    void aboutToAbort();
    
	/**
     * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only ROTSystemStatus property
	*/
	virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);

    /**
     * Returns a reference to the fileName property implementation of IDL interface.
	 * @return pointer to read-only string property
	*/	
    virtual ACS::ROstring_ptr fileName() throw(CORBA::SystemException);    
    
    /**
     * Returns a reference to the projectName property implementation of IDL interface.
	 * @return pointer to read-only string property
	*/
    virtual ACS::ROstring_ptr projectName() throw(CORBA::SystemException);
    
    /**
     * Returns a reference to the observer property implementation of IDL interface.
	 * @return pointer to read-only string property
	*/
    virtual ACS::ROstring_ptr observer() throw(CORBA::SystemException);

    /**
     * Returns a reference to the deviceID property implementation of IDL interface.
	 * @return pointer to read-only long property
	*/
    virtual ACS::ROlong_ptr deviceID() throw(CORBA::SystemException);

	/**
     * Returns a reference to the scanAxis property Implementation of IDL interface.
	 * @return pointer to read-only ROTScanAxis property
	*/
	virtual Management::ROTScanAxis_ptr scanAxis() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the dataX property implementation of IDL interface.
	 * @return pointer to read-only double property
	*/
	virtual ACS::ROdouble_ptr dataX() throw(CORBA::SystemException);
	
	/**
	 * Returns a reference to the dataY property implementation of IDL interface.
	 * @return pointer to read-only double property
	*/
	virtual ACS::ROdouble_ptr dataY() throw(CORBA::SystemException);
	
	/**
	 * Returns a reference to the arrayDataX property implementation of IDL interface.
	 * @return pointer to read-only double sequence property
	*/
	virtual ACS::ROdoubleSeq_ptr arrayDataX() throw(CORBA::SystemException);

	/**
	 * Returns a reference to the arrayDataY property implementation of IDL interface.
	 * @return pointer to read-only double sequence property
	*/
	virtual ACS::ROdoubleSeq_ptr arrayDataY() throw(CORBA::SystemException);

	/**
	 * called to inform the component that a new scan is going to start
	 * @param  prm scan parameters structure
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
	virtual void startScan(const Management::TScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

	/**
	 * called to give extra information about scan layout. Not used by this component.
	 * @param layout scan layout definition given as a sequence of strings
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
    virtual void setScanLayout (const ACS::stringSeq & layout) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

	/**
	 * called to inform the component that a new scan is going to be closed
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
    virtual void stopScan() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

	/**
	 * called to inform the component that a new subscan is going to start
	 * @parm prm subscan parameters structure
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @return the name of the file that will be processed by the system
	 */
    virtual char *startSubScan(const ::Management::TSubScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

    /**
     * Return the recording status of the component.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
     */
    virtual CORBA::Boolean isRecording() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

    /**
     * Called in order to reset component status and make sure it is ready for start a new data acquisition loop from the scratch.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
     */
    virtual void reset() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);
        
private:
	SmartPropertyPointer<ROstring> m_pfileName;	
	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus> > m_pstatus;
	SmartPropertyPointer<ROstring> m_pprojectName;
	SmartPropertyPointer<ROstring> m_pobserver;	
	//SmartPropertyPointer<ROlong> m_pscanIdentifier;
	SmartPropertyPointer<ROlong> m_pdeviceID;	
	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TScanAxis),POA_Management::ROTScanAxis> > m_pscanAxis;
	SmartPropertyPointer<ROdouble> m_pdataX;
	SmartPropertyPointer<ROdouble> m_pdataY;	
	SmartPropertyPointer<ROdoubleSeq> m_parrayDataX;
	SmartPropertyPointer<ROdoubleSeq> m_parrayDataY;
	
	//nc::SimpleConsumer<Antenna::AntennaDataBlock> *m_antennaNC;
	//IRA::CSecureArea<FitsWriter_private::CDataCollection> *m_dataWrapper;
	FitsWriter_private::CDataCollection *m_data;
	FitsWriter_private::CConfiguration m_config;
	FitsWriter_private::CEngineThread *m_workThread;
	FitsWriter_private::CCollectorThread *m_collectThread;
};



#endif /*FITSWRITERIMPL_H_*/
