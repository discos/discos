#ifndef CALIBRATIONTOOLIMPL_H
#define CALIBRATIONTOOLIMPL_H

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: CalibrationToolImpl.h,v 1.5 2011-03-11 12:26:51 c.migoni Exp $      */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/** 
 * @mainpage CalibrationTool component documentation
 * @date 23/11/2010
 * @version 1.0.0
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>
 * @remarks Last compiled under ACS 7.0.2
*/

#include <baci.h>
#include <CalibrationToolS.h>
#include <DataReceiverS.h>
#include <ComponentErrors.h>
#include <acscomponentImpl.h>
#include <String.h>
#include <SecureArea.h>
#include <bulkDataReceiverImpl.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <enumpropROImpl.h>
#include <baciROstring.h>
#include <baciROlong.h>
#include <baciROdouble.h>
#include <baciROdoubleSeq.h>
#include "ReceiverCallback.h"
#include "DataCollection.h"
#include "EngineThread.h"
#include "CollectorThread.h"

using namespace baci;
using namespace maci;

class CalibrationToolImpl : public virtual BulkDataReceiverImpl<CalibrationTool_private::ReceiverCallback>,
                       public virtual POA_Management::CalibrationTool
{
  public:

    /**
     * Constructor
     * @param poa Poa which will activate this and also all other Components.
     * @param name component name.
     */
    CalibrationToolImpl(const ACE_CString& name,maci::ContainerServices* containerServices);

    /**
     * Destructor
     */
    virtual ~CalibrationToolImpl();

    void initialize() throw (ACSErr::ACSbaseExImpl);

    void execute() throw (ACSErr::ACSbaseExImpl);
    
    void cleanUp();

    void aboutToAbort();
    
	/**
     * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only ROTSystemStatus property status
	*/
	virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);

    /**
     * Returns a reference to the fileName property Implementation of IDL interface.
	 * @return pointer to read-only string property fileName
	*/
    virtual ACS::ROstring_ptr fileName() throw(CORBA::SystemException);

    /**
     * Returns a reference to the projectName property Implementation of IDL interface.
	 * @return pointer to read-only string property projectName
	*/
    virtual ACS::ROstring_ptr projectName() throw(CORBA::SystemException);

    /**
     * Returns a reference to the observer property Implementation of IDL interface.
	 * @return pointer to read-only string property observer
	*/
    virtual ACS::ROstring_ptr observer() throw(CORBA::SystemException);

    /**
     * Returns a reference to the deviceID property Implementation of IDL interface.
	 * @return pointer to read-only long property deviceID
	*/
    virtual ACS::ROlong_ptr deviceID() throw(CORBA::SystemException);

    /**
     * Returns a reference to the scan axis property Implementation of IDL interface.
	 * @return pointer to read-only ROTScanAxis property
	*/
	virtual Management::ROTScanAxis_ptr scanAxis() throw (CORBA::SystemException);

    /**
     * Returns a reference to the dataY property Implementation of IDL interface.
	 * @return pointer to read-only double property dataY
	*/
    virtual ACS::ROdouble_ptr dataY() throw(CORBA::SystemException);

    /**
     * Returns a reference to the dataX property Implementation of IDL interface.
	 * @return pointer to read-only double property dataX
	*/
    virtual ACS::ROdouble_ptr dataX() throw(CORBA::SystemException);

    /**
     * Returns a reference to the arrayDataY property Implementation of IDL interface.
	 * @return pointer to read-only doubleSeq property arrayDataY
	*/
    virtual ACS::ROdoubleSeq_ptr arrayDataY() throw(CORBA::SystemException);

    /**
     * Returns a reference to the arrayDataX property Implementation of IDL interface.
	 * @return pointer to read-only doubleSeq property arrayDataX
	*/
    virtual ACS::ROdoubleSeq_ptr arrayDataX() throw(CORBA::SystemException);

    /**
     * Returns a reference to the hpbw property Implementation of IDL interface.
	 * @return pointer to read-only double property hpbw
	*/
	virtual ACS::ROdouble_ptr hpbw() throw (CORBA::SystemException);

    /**
     * Returns a reference to the amplitude property Implementation of IDL interface.
	 * @return pointer to read-only double property amplitude
	*/
	virtual ACS::ROdouble_ptr amplitude() throw (CORBA::SystemException);

    /**
     * Returns a reference to the peakOffset property Implementation of IDL interface.
	 * @return pointer to read-only double property peakOffset
	*/
	virtual ACS::ROdouble_ptr peakOffset() throw (CORBA::SystemException);

    /**
     * Returns a reference to the slope property Implementation of IDL interface.
	 * @return pointer to read-only double property slope
	*/
	virtual ACS::ROdouble_ptr slope() throw (CORBA::SystemException);

    /**
     * Returns a reference to the offset property Implementation of IDL interface.
	 * @return pointer to read-only double property offset
	*/
	virtual ACS::ROdouble_ptr offset() throw (CORBA::SystemException);

    /**
     * Returns a reference to the sourceFlux property Implementation of IDL interface.
	 * @return pointer to read-only double property sourceFlux
	*/
	virtual ACS::ROdouble_ptr sourceFlux() throw (CORBA::SystemException);

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
	 */
    virtual void startSubScan(const ::Management::TSubScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

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
	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus> > m_pstatus;
	SmartPropertyPointer < ROstring > m_pfileName;	
    SmartPropertyPointer < ROstring > m_pprojectName;
    SmartPropertyPointer < ROstring > m_pobserver;
    SmartPropertyPointer < ROlong > m_pdeviceID;
	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TScanAxis), POA_Management::ROTScanAxis> > m_pscanAxis;
	SmartPropertyPointer < ROdouble > m_pdataY;
	SmartPropertyPointer < ROdouble > m_pdataX;
	SmartPropertyPointer < ROdoubleSeq > m_parrayDataY;
	SmartPropertyPointer < ROdoubleSeq > m_parrayDataX;
    SmartPropertyPointer < ROdouble > m_phpbw;
	SmartPropertyPointer < ROdouble > m_pamplitude;
	SmartPropertyPointer < ROdouble > m_ppeakOffset;
	SmartPropertyPointer < ROdouble > m_pslope;
	SmartPropertyPointer < ROdouble > m_poffset;
	SmartPropertyPointer < ROdouble > m_psourceFlux;

	IRA::CSecureArea<CalibrationTool_private::CDataCollection> *m_dataWrapper;
	CalibrationTool_private::CConfiguration m_config;
	CalibrationTool_private::CEngineThread *m_workThread;
	CalibrationTool_private::CCollectorThread *m_collectThread;
};

#endif /*CALIBRATIONTOOLIMPL_H_*/
