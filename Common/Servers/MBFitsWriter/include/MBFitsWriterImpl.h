#ifndef __H__MBFitsWriterImpl__
#define __H__MBFitsWriterImpl__

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciROdouble.h>
#include <baciROdoubleSeq.h>
#include <baciROlong.h>
#include <baciROstring.h>
#include <baciSmartPropertyPointer.h>
#include <bulkDataReceiverImpl.h>
#include <enumpropROImpl.h>

#include <MBFitsWriterS.h>

#include "CollectorThread.h"
#include "EngineThread.h"
#include "ReceiverCallback.h"

#include "MBFitsManager.h"

using baci::ROdouble;
using baci::ROdoubleSeq;
using baci::ROlong;
using baci::ROstring;
using baci::SmartPropertyPointer;

using maci::ContainerServices;

class MBFitsWriterImpl : public virtual BulkDataReceiverImpl<MBFitsWriter_private::ReceiverCallback>,
												 public virtual POA_Management::MBFitsWriter {
	public:
		MBFitsWriterImpl( const ACE_CString& name_,
											ContainerServices* containerServices_p_ );
		virtual ~MBFitsWriterImpl();

    virtual void initialize() throw (ACSErr::ACSbaseExImpl);

    virtual void execute() throw (ACSErr::ACSbaseExImpl);

    virtual void cleanUp();

    virtual void aboutToAbort();

    virtual void startScan( const Management::TScanSetup& scanSetup_ );
    virtual void setScanLayout( const ACS::stringSeq& scanLayout_ );
    virtual void stopScan();
    virtual void startSubScan( const Management::TSubScanSetup& subscanSetup_ );
    virtual CORBA::Boolean isRecording();
    virtual void reset();

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
		 * Returns a reference to the scanIdentifier property implementation of IDL interface.
		 * @return pointer to read-only long property
		 */
		virtual ACS::ROlong_ptr scanIdentifier() throw(CORBA::SystemException);

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
		 * Returns a reference to the status property Implementation of IDL interface.
		 * @return pointer to read-only ROTSystemStatus property
		 */
		virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);

//		/**
//		 * It allows to set the current file name
//		 * @param fileName_ name of the file
//		 * @throw CORBA::SystemException
//		 * @throw ComponentErrors::ComponentErrorsEx
//		 */
//    virtual void setFileName( const char* fileName_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);
//
//    /**
//     * It allows to set the name of the project currently running.
//     * @param projectName_ project name
//     * @thorw CORBA::SystemException
//     * @throw ComponentErrors::ComponentErrorsEx
//     */
//    virtual void setProjectName( const char *projectName_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);
//
//		/**
//		 * It allows to set the name of the current observer.
//		 * @param observerName_ observer name
//     * @thorw CORBA::SystemException
//     */
//    virtual void setObserverName( const char *observerName_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);
//
//    /**
//     * It allows to set the value from which the scan numeration will start.
//     * @param scanID_ scan ID
//     * @throw CORBA::SystemException
//     * @throw ComponentErrors::ComponentErrorsEx
//     */
//    virtual void setScanIdentifier( CORBA::Long scanID_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);
//
//    /**
//     * It allows to set identifier of the device elected as primary device
//     * @param deviceID_ new device ID
//     * @throw CORBA::SystemException
//     * @throw ComponentErrors::ComponentErrorsEx
//     */
//		virtual void setDevice( CORBA::Long deviceID_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);
//
//		/**
//		 * It allows to set the current scan direction.
//		 * @param scanAxis_ the numeric code of the involved axis during the present scan
//		 * @throw CORBA::SystemException
//		 * @throw ComponentErrors::ComponentErrorsEx
//		 */
//		virtual void setScanAxis( Management::TScanAxis scanAxis_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	private:
		MBFitsWriterImpl();																												// Not implemented
		MBFitsWriterImpl( const MBFitsWriterImpl& mbFitsWriter_ );								// Not implemented

//		virtual bool operator==( const MBFitsWriterImpl& mbFitsWriter_ ) const;		// Not implemented
//		virtual bool operator!=( const MBFitsWriterImpl& mbFitsWriter_ ) const;		// Not implemented
		MBFitsWriterImpl& operator=( const MBFitsWriterImpl& mbFitsWriter_ );			// Not implemented

		SmartPropertyPointer<ROstring>		m_fileName_p;
		SmartPropertyPointer<ROstring>		m_projectName_p;
		SmartPropertyPointer<ROstring>		m_observer_p;
		SmartPropertyPointer<ROlong>			m_scanID_p;
		SmartPropertyPointer<ROlong>			m_deviceID_p;
		SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TScanAxis), POA_Management::ROTScanAxis> >					m_scanAxis_p;

		SmartPropertyPointer<ROdouble>		m_dataX_p;
		SmartPropertyPointer<ROdouble>		m_dataY_p;
		SmartPropertyPointer<ROdoubleSeq>	m_arrayDataX_p;
		SmartPropertyPointer<ROdoubleSeq> m_arrayDataY_p;

		SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus> >	m_status_p;

		MBFitsWriter_private::CConfiguration*											m_configuration_p;
		IRA::CSecureArea<MBFitsWriter_private::CDataCollection>*	m_dataWrapper_p;
		MBFitsWriter_private::EngineThread*											m_workThread_p;
		MBFitsWriter_private::CCollectorThread*									m_collectThread_p;
};

#endif	// __H__MBFitsWriterImpl__
