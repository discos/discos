#ifndef USDImpl_h
#define USDImpl_h

/*******************************************************************************
*
* "@(#) $Id: usdImpl.h,v 1.1 2011-03-24 09:18:00 c.migoni Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* GMM       jul 2005   creation				   
* CM	    jan 2013  ACS 8.0.1 adaptions
* CM	    jan 2013  All code revised and adapted to be used from SRTActiveSurfaceBoss component
*/


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif
#include <baciCharacteristicComponentImpl.h>
#include <baciROdouble.h>
#include <baciRWdouble.h>
#include <baciROlong.h>
#include <baciRWlong.h>
#include <baciROpattern.h>
#include <baciSmartPropertyPointer.h>
#include <IRA>

//CORBA stubs
#include <usdS.h>			
#include <lanS.h>

//error
#include <ASErrors.h>

//others
#include <time.h>
#include <math.h> //used for pow()
#include <usdDevIO.h>

/// ustep to step exp. 2 factor (2^7=128)
#define USxS 		7	

// mask pattern for status 
#define MRUN	0x000080
#define CAMM	0x000100
#define ENBL	0x002000
#define PAUT	0x000800
#define CAL 	0x008000

// USD driver command
#define RESET   0x01
#define STOP 	0x11
#define	FMIN	0x20
#define	FMAX	0x21
#define	ACCL	0x22
#define LCNTR   0x23
#define UBIT	0x25
#define	RESL	0x26
#define	DELAY	0x28
#define HSTOP   0x2A
#define	APOS	0x12
#define STAT	0x13
#define	SVER	0x10
#define	USDT	0x14
#define CPOS	0x30
#define RPOS	0x31
#define GO	0x32

#define MM2STEP	1400 //(42000 STEP / 30 MM)

#define MAX_FAILURES 5

// specific macro
  /** 	@#define _ADD_MEMBER(OBJ,MEMB)
     *  This macro add extra data to an error/compl. object. MEMB must be real variable or constant
     *	@param OBJ error or completion object
		 *	@param MEMB costant or variable added as (name,value) pair
     */
#define _ADD_MEMBER(OBJ,MEMB) { \
	CString mName(#MEMB); \
	CString mVal;mVal.Format("%02x",MEMB); \
	_ADD_EXTRA(OBJ,mName,mVal); \
}

  /** 	@#define _THROW_EX(ERR,ROUTINE,VAR)
     *  Create and launch a remote (CORBA) exception, adding USD address and VAR as addedd data
     *	@param ERR error 
     *	@param ROUTINE the calling routine 
		 *	@param VAR costant or variable added as (name,value) pair
     */
#define _THROW_EX(ERR,ROUTINE,VAR) { \
				ASErrors::ERR##ExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
				_ADD_MEMBER(exImpl,m_addr); \
				_ADD_MEMBER(exImpl,VAR); \
				throw exImpl.getASErrorsEx(); \
		}

  /** 	@#define _VAR_CHK_EXIMPL(VAR,ERR,ROUTINE)
     *  Launch a local exception if VAR is not true.
     *	@param VAR variable to be checked
		 *	@param ERR error 
     *	@param ROUTINE the calling routine 
     */
#define _VAR_CHK_EXIMPL(VAR,ERR,ROUTINE) { \
					if(!VAR) throw ASErrors::ERR##ExImpl(__FILE__,__LINE__,ROUTINE);\
} \

  /** 	@#define _VAR_CHK_EX(VAR,ERR,ROUTINE)
     *  Launch a remote(CORBA) exception if VAR is not true.
     *	@param VAR variable to be checked
		 *	@param ERR error 
     *	@param ROUTINE the calling routine 
     */
#define _VAR_CHK_EX(VAR,ERR,ROUTINE) \
					if(!VAR) throw ASErrors::ERR##ExImpl(__FILE__,__LINE__,ROUTINE).getASErrorsEx();

  /** 	@#define _COMPL_CHK_THROW_EX(CP_SP,OUT_TYPE,ROUTINE)
     *  Check a remote(CORBA) completion_var. Launch an remotem OUT_TYPE CORBA exception 
		 *	if it is not error free.
     *	@param CP_SP Complition_var smart pointer 
		 *	@param OUT_TYPE out type exception 
     *	@param ROUTINE the calling routine 
     */
		 #define _COMPL_CHK_THROW_EX(CP_SP,OUT_TYPE,ROUTINE) { \
			 ACSErr::CompletionImpl comp(CP_SP.in());\
			 if (compCheck(comp)) \
				 throw ASErrors::OUT_TYPE##ExImpl(comp,__FILE__,__LINE__,ROUTINE).getASErrorsEx();\
}

 /** 	@#define _SET_CDB(PROP,LVAL,ROUTINE) throw (ASErrors::CDBAccessErrorExImpl)
     *  Set a long CDB property or attribute.
     *	@param PROP property to be setted
		 *	@param LVAL the value 
     *	@param ROUTINE the calling routine 
     */
#define _SET_CDB(PROP,LVAL,ROUTINE) {	\
	maci::ContainerServices* cs=getContainerServices();\
		if (!CIRATools::setDBValue(cs,#PROP,(const long&) LVAL)) \
		{ ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
			exImpl.setFieldName(#PROP); throw exImpl; \
		} \
}

 /** 	@#define _SET_CDB_D(PROP,LVAL,ROUTINE) throw (ASErrors::CDBAccessErrorExImpl)
     *  Set a double CDB property or attribute.
     *	@param PROP property to be setted
		 *	@param LVAL the value 
     *	@param ROUTINE the calling routine 
     */
#define _SET_CDB_D(PROP,LVAL,ROUTINE) {	\
	maci::ContainerServices* cs=getContainerServices();\
		if (!CIRATools::setDBValue(cs,#PROP,(const double&) LVAL)) \
		{ ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
			exImpl.setFieldName(#PROP); throw exImpl; \
		} \
}
/*		{ ASErrors::CDBAccessErrorExImpl ex=ASErrors::CDBAccessErrorExImpl(__FILE__,__LINE__,ROUTINE); 
			ex.setFieldName(#PROP); throw ex.getASErrorsEx(); */

 /** 	@#define _GET_PROP(PROP,RETVAR,ROUTINE)
     *  Get a property value.
     *	@param PROP property to be read
		 *	@param RETVAR the value 
     *	@param ROUTINE the calling routine 
		 *	@throw a DevIOError remote exception if return completion is not error free
     */
#define _GET_PROP(PROP,RETVAR,ROUTINE) { \
			ACSErr::Completion_var sm_cp; \
			RETVAR=m_##PROP##_sp->get_sync(sm_cp.out()); \
			 ACSErr::CompletionImpl comp(sm_cp.in());\
			 if (compCheck(comp)) \
				 throw ASErrors::DevIOErrorExImpl(comp,__FILE__,__LINE__,ROUTINE).getDevIOErrorEx();\
}

 /** 	@#define _SET_PROP(PROP,RETVAR,ROUTINE)
     *  Set a property value.
     *	@param PROP property to be read
	 *	@param VALUE property value to be changed
     *	@param ROUTINE the calling routine 
	 *	@throw a DevIOError remote exception if return completion is not error free
     */
#define _SET_PROP(PROP,VALUE,ROUTINE) {\
			ACSErr::CompletionImpl comp=m_##PROP##_sp->set_sync(VALUE);\
			 if (compCheck(comp)) \
				 throw ASErrors::DevIOErrorExImpl(comp,__FILE__,__LINE__,ROUTINE).getDevIOErrorEx();\
}

 /** 	@#define _SET_LDEF(PROP,ROUTINE)
     *  Set the default value for given property. All type except double.
		 *	Launch a remote exception if set_sync return completion is not error free.
     *	@param PROP property 
     *	@param ROUTINE the calling routine 
     */
#define _SET_LDEF(PROP,ROUTINE) {	\
	long tmpd;\
	ACSErr::CompletionImpl comp;\
	maci::ContainerServices* cs=getContainerServices();\
		if (!CIRATools::getDBValue(cs,#PROP"/default_value",tmpd)) \
		{ ASErrors::CDBAccessErrorExImpl ex=ASErrors::CDBAccessErrorExImpl(__FILE__,__LINE__,ROUTINE); \
			ex.setFieldName(#PROP"/default_value"); throw ex.getASErrorsEx(); \
		}  else comp=m_##PROP##_sp->set_sync(tmpd); \
		if (compCheck(comp)) throw ASErrors::USDErrorExImpl(__FILE__,__LINE__,ROUTINE).getASErrorsEx(); \
}

 /** 	@#define _CATCH_ACS_EXCP_THROW_EXIMPL(IN_TYPE,OUT_TYPE,ROUTINE,VAR)
     *  Catch an ACS(with error trace) exception and launch a local one (ExImpl).
     *	@param IN_TYPE the exception to catch 
     *	@param OUT_TYPE the exception to be launched 
     *	@param VAR variable name and value added as addiotional data 
     *	@param ROUTINE the calling routine 
     */
#define _CATCH_ACS_EXCP_THROW_EXIMPL(IN_TYPE,OUT_TYPE,ROUTINE,VAR) catch(IN_TYPE& Ex) { \
				OUT_TYPE exImpl(Ex,__FILE__,__LINE__,ROUTINE); \
				_ADD_MEMBER(exImpl,m_addr); \
				_ADD_MEMBER(exImpl,VAR); \
				throw exImpl; \
		}

 /** 	@#define _CATCH_EXCP_THROW_EXIMPL(IN_TYPE,OUT_TYPE,ROUTINE,VAR)
     *  Catch an CORBA(w/o error trace) exception and launch a local one (ExImpl).
     *	@param IN_TYPE the exception to catch 
     *	@param OUT_TYPE the exception to be launched 
     *	@param VAR variable name and value added as addiotional data 
     *	@param ROUTINE the calling routine 
     */
#define _CATCH_EXCP_THROW_EXIMPL(IN_TYPE,OUT_TYPE,ROUTINE,VAR) catch(IN_TYPE& Ex) { \
				OUT_TYPE exImpl(__FILE__,__LINE__,ROUTINE); \
				_ADD_MEMBER(exImpl,m_addr); \
				_ADD_MEMBER(exImpl,VAR); \
				throw exImpl; \
		}

 /** 	@#define _CATCH_ACS_EXCP_THROW_EX(IN_TYPE,OUT_TYPE,ROUTINE,VAR)
     *  Catch an ACS(with error trace) exception and launch a remote one (Ex).
     *	@param IN_TYPE the exception to catch 
     *	@param OUT_TYPE the exception to be launched 
     *	@param VAR variable name and value added as additional data 
     *	@param ROUTINE the calling routine 
     */
#define _CATCH_ACS_EXCP_THROW_EX(IN_TYPE,_ERR,ROUTINE,VAR) catch(IN_TYPE& Ex) { \
				 ASErrors::_ERR##ExImpl exImpl(Ex,__FILE__,__LINE__,ROUTINE); \
				_ADD_MEMBER(exImpl,m_addr); \
				_ADD_MEMBER(exImpl,VAR); \
				throw exImpl.getASErrorsEx(); \
		}
		
 /** 	@#define _CATCH_EXCP_THROW_EX(IN_TYPE,OUT_TYPE,ROUTINE,VAR)
     *  Catch an CORBA(w/o error trace) exception and launch a remote one (Ex).
     *	@param IN_TYPE the exception to catch 
     *	@param OUT_TYPE the exception to be launched 
     *	@param VAR variable name and value added as addiotional data 
     *	@param ROUTINE the calling routine 
     */
#define _CATCH_EXCP_THROW_EX(IN_TYPE,_ERR,ROUTINE,VAR) catch(IN_TYPE& Ex) { \
				 ASErrors::_ERR##ExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
				_ADD_MEMBER(exImpl,m_addr); \
				_ADD_MEMBER(exImpl,VAR); \
				throw exImpl.getASErrorsEx(); \
		}

using namespace baci;
using namespace maci;
using namespace ASErrors;
using namespace std;


/**
*	@mainpage USD component
*	@date <26/11/2006> @version <1.2.0> @author gmaccaferri
*	@remarks compiled for ACS 6.0.2
*/

/**
 * This class USDImpl implements the single actuator of SRT AS.
 * It provides some asynchronous methods: up,down, stop, reset.
 * It also provides the property rappresenting some important characteristic.
  * 
 * @author <a href=mailto:g.maccaferri@ira.cnr.it>Giuseppe Maccaferri</a>,
 * IRA, Bologna<br>
 * @version "@(#) $Id: usdImpl.h,v 1.1 2011-03-24 09:18:00 c.migoni Exp $"
 */
class USDImpl: public CharacteristicComponentImpl,public virtual POA_ActiveSurface::USD
{
	public:
  
     	/**
     	* Constructor
     	* @param poa Poa which will activate this and also all other components.
     	* @param name component's name. This is also the name that will be used to find the
     	* configuration data for the component in the Configuration Database.
     	*/
	USDImpl(const ACE_CString &, maci::ContainerServices* );

    	/**
     	* Destructor
     	*/
    	virtual ~USDImpl();
	
    	/*Override component lifecycle methods*/
    	virtual void initialize() throw (ACSErr::ACSbaseExImpl);
    	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	virtual void cleanUp();
	virtual void aboutToAbort();

	/**
     	* Following functions returns a reference to the property implementation of IDL interface 
     	*/ 
    	virtual ACS::RWlong_ptr delay() throw (CORBA::SystemException);
    	virtual ACS::RWlong_ptr cmdPos() throw (CORBA::SystemException);
    	virtual ACS::RWlong_ptr Fmin() throw (CORBA::SystemException);
    	virtual ACS::RWlong_ptr Fmax() throw (CORBA::SystemException);
    	virtual ACS::RWlong_ptr acc() throw (CORBA::SystemException);
    	virtual ACS::RWlong_ptr uBits() throw (CORBA::SystemException);
    	virtual ACS::RWdouble_ptr lmCorr() throw (CORBA::SystemException);
    	virtual ACS::ROlong_ptr actPos() throw (CORBA::SystemException);
    	virtual ACS::ROpattern_ptr status() throw (CORBA::SystemException);
    	virtual ACS::ROlong_ptr softVer() throw (CORBA::SystemException);
    	virtual ACS::ROlong_ptr type() throw (CORBA::SystemException);
    	virtual ACS::RWdouble_ptr gravCorr() throw (CORBA::SystemException);
    	virtual ACS::RWdouble_ptr userOffset() throw (CORBA::SystemException);
	
	/**
	* Implementation of unparametrized synchrnous methods
	* There are the function that makes some typical operation with the actuator.
	*/
	void stop() throw (CORBA::SystemException, ASErrors::ASErrorsEx) { 	
		try {
			action(STOP);
		}
		_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::stop()",m_status)
	} 
		
	void up() throw (CORBA::SystemException,ASErrors::ASErrorsEx) {		
		try {
			action(GO,1,1);
		}
		_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::up()",m_status)
	} 
		
	void down() throw (CORBA::SystemException,ASErrors::ASErrorsEx) {  
		try {
			action(GO,-1,1);
		}
		_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::down()",m_status)
	} 
	 
	void reset() 	throw (CORBA::SystemException,ASErrors::ASErrorsEx);
		 
     	/**
     	* upperMost() & bottomMost().Put the USD against the mechanical range limits (upper and lower).
	* This intrinsecally makes the scale uncalibrated. 
	* Be carefull to avoid mech. stress due to max diffs between adiacent nearest actuators
	*/
	void bottom() throw (CORBA::SystemException,ASErrors::ASErrorsEx) { 
		try {
			action(CPOS, m_bottom<<USxS,4);
		}
		_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::bottom()",m_status)
	}
	void top() throw (CORBA::SystemException,ASErrors::ASErrorsEx) { 
		try {
			action(CPOS,m_top<<USxS,4);
		}
		_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::top()",m_status)
	} 

	/**
	* Implementation of move methods
	* This function moves the actuator of incr step relative to actual position.
	* @param incr Amount of increment in ustep.If negative the movement will be toward the bottom.
	*/
	void move(CORBA::Long incr) throw (CORBA::SystemException,ASErrors::ASErrorsEx) {	
		try {
			action(RPOS,incr<<USxS,4);
		}
		_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::move()",m_status)
	} 
	
	/**
	* Calibrate the scale. USD already must be already against the uppermost edge.
	* This function moves the actuator back untill the camma trailing edge.
	* Records the position and move futher to record the camma falling edge.
	* For the best measure "stop by hardware" USD function is used.
	* Then computes the len of camma zone, the central point and distance from uppermost edge.
	* Finally move USD in camma central zone, load on USD counter the zeroRef value, moves the USD to 0 position and mark it as calibrated.
	* @return 
	*/
	//virtual ACSErr::Completion* calibrate() throw (CORBA::SystemException,ASErrors::ASErrorsEx);
	void calibrate() throw (CORBA::SystemException,ASErrors::ASErrorsEx);
	
	/**
	* Calibration verification. USD already must be already calibrated.
	* This function moves the actuator to zero ref position and check the camma on status.
	* Moves to uppermost edge and check again the camma on-status.
	* Adds half camma lenght and check the off status.
	* Makes the same control on bottom edge.
	* Be aware that other nearest usd are at middle scale, to avoid mechanical stresses
	* @return 
	*/
	//virtual ACSErr::Completion* calVer() throw (CORBA::SystemException,ASErrors::ASErrorsEx) ;
	void calVer() throw (CORBA::SystemException,ASErrors::ASErrorsEx);

	/**
	* Write calibration results on xml configuration file
	*/
	void writeCalibration(CORBA::Double_out cammaLenD, CORBA::Double_out cammaPosD, CORBA::Boolean_out calibrate) throw (CORBA::SystemException,ASErrors::ASErrorsEx);

	/**
	* This function gets parameters useful for update function
	*/
	void posTable (const ACS::doubleSeq& theActuatorsCorrections, CORBA::Long theParPositions, CORBA::Double theDeltaEL, CORBA::Long theThreshold);
			 
     	/**
     	* home. bring the USD to zero reference position.
     	* This function moves the actuator to zero reference position, tipically at usd middlescale
     	* @return 
	*/
	void refPos() throw (CORBA::SystemException,ASErrors::ASErrorsEx) {
		try {
			action(CPOS,0,4);
		}
		_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::refPos()",m_status)
	} 
	 
     	/**
     	* setProfile().  Set the surface active profile.
     	* @param prof	0 shaped, 1 parabolic, 2 parabolic fixed
	*/
	void setProfile(CORBA::Long prof) throw (CORBA::SystemException,ASErrors::ASErrorsEx) {
		m_profile = prof;
	} 
	 
     	/**
     	* correction().The last minute coorection to be applied.
     	* They will be added to actual computed USD position
     	* @param corr	the correction in mm 
     	*/
	void correction(CORBA::Double corr) throw (CORBA::SystemException,ASErrors::ASErrorsEx) {
		ACSErr::Completion_var comp;
		try {
			comp = lmCorr()->set_sync(corr);
		}
		catch (CORBA::SystemException& Ex) 
		{
			ACS_SHORT_LOG((LM_CRITICAL, "CORBA::SystemException thrown! "));
		}
	} 

     	/**
     	* update(). Applay the antenna elevation dependant corrections.
     	* They are the gravity and user-offset
     	* @param elevation	antenna elevation in degrees. 
     	*/
	void update(CORBA::Double elevation) throw (CORBA::SystemException,ASErrors::ASErrorsEx);

     	/**
     	* stow() & setup(). General management functions.
	* stow: Stop the USD position loop control and bring it to bottom most position (full closed).
	* setup: bring the usd to zeroRef position and starts the pos. control loop
	*/
	void stow() throw (CORBA::SystemException,ASErrors::ASErrorsEx) {
		try {
                	action(STOP);
                }
                _CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::stow()",m_status)
			 
                try {
                	action(CPOS, m_bottom<<USxS,4);
                }
                _CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::stow()",m_status)
                 
	}
	void setup() throw (CORBA::SystemException,ASErrors::ASErrorsEx) { 
		try {
			action(STOP);
                }
                _CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::setup()",m_status)
				
                try {
                    action(CPOS,0,4);
                }
                _CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::setup()",m_status)
	}

	/**
     	* check a corba exception.
     	* Convert a CORBA exception in  C++ exception and check it.
     	* @param Ex CORBA exception.
    	*/
	void exCheck(ASErrors::ASErrorsEx Ex);

     	/**
     	* Check a CORBA completion passed by reference.
     	* Convert an error completion in  C++ exception and log it.
     	* @param comp completion.
     	* @return true if comp is an error completion else false
    	*/
	bool compCheck(ACSErr::CompletionImpl& );

    	/** 
    	* this counts the consecutive times the USD does not respond
    	*/
	int m_failures;

    	/** 
    	* flag rappresenting the availability of the module.
    	* It is available if is comunicating. After five times USD doesn't respond, the flag is set to FALSE to inhibit any further activity.
    	*/	
	bool m_available;

    	/** 
    	* stepper motor positioning resolution (step)
    	*/	
	double m_step_res;

	/** 
    	* calibrate flag
    	*/	
	BYTE m_calibrate;

	/** 
    	* position loop on
    	*/	
	BYTE m_ploop;

	private:

	CORBA::Long m_profile;
   	/**
    	* pointer to LAN/485 component
    	*/
	//MOD_LAN::lan* m_pLan;
	ActiveSurface::lan* m_pLan;
	
	/** 
    	* sector of usd
    	*/	
	BYTE m_sector;

    	/**
    	* lan address of usd. 
    	* Each lan rappresent a radius. Looking frontally at parabola, the first start from north and the other follow in cw mode.
    	*/	
	BYTE m_lanNum;

    	/** 
    	* serial address of usd
    	*/	
	BYTE m_addr;

	/** 
    	* camma lenght in step
    	*/	
	int m_cammaLen;

	/**
    	* camma position in step
    	*/
	int m_cammaPos;

	/**
    	* camma end in step
    	*/
        int m_cammaEnd;

	/**
	* array of actuator corrections
	* number of update table parameters
	* delta elevation between two measures
	* threshold between actual and updated position
	*/
	double *actuatorsCorrections;
	double *elevations;
	int parPositions;
	double deltaEL;
	int threshold;

	/**
	* camma len and camma pos in degrees
	*/
	double m_cammaLenD;
	double m_cammaPosD;

	/**
	* last commanded position (steps)
	*/
	int m_lastCmdStep;

    	/**
    	* usefull range in step
    	*/
	int m_fullRange;

    	/**                                      
    	* zero offset
    	*/
	int m_zeroRef;

	/** 
    	* step x turn (step)
    	*/	
	int m_step_giro;

	/** 
    	* step resolution (1/2^rs) pag 7 manuale USD
    	*/	
	BYTE m_rs;

	/**
	* step to deg conversion factor
    	*/
	double m_step2deg;

    	/**
    	* top position (step)
    	*/
	int m_top;

    	/**
    	* bottom position (step)
    	*/
	int m_bottom;

	/**
    	* status bytes
    	*/
	int m_status;

     	/**
     	* Check if USD motor is still running after given psition
	* The required time is computed from position displacement and Fmax value,
	* plus some safety seconds.
	* @param allowed secs for motion.
     	* @return true if still running 0 if stopped.
    	*/
	bool stillRunning(long pos) throw (ASErrors::ASErrorsExImpl);
	
     	/**
     	* Check calibration comparing quote and cal status
	* Must be used only when not running
     	* @return true if calibrated 0 if not.
    	*/
	bool chkCal() throw (ASErrors::ASErrorsExImpl);
	
	/**
     	* Check the thrown exception and logging it.
     	* Basing on  thrown C++ exception, it set to FALSE the m_available flag to inhibit any further activity 
     	* @param exImpl C++ exception.
    	*/
	void exImplCheck(ASErrors::ASErrorsExImpl);

  	/**
	* Perform an action on USD
	* @param cmd one of USD available command
	* @param parameter an otional parameter for the command
	* @param nbytes lenght in bytes of parameter
	*/
	void action( int cmd,int parameter=0,int nbytes=0) throw (ASErrors::ASErrorsExImpl);

	/**
	* ALMA C++ coding standards state copy operators should be disabled.
     	*/
    	void operator=(const USDImpl&);
 
 	/**
     	* cob name of the LAN component.
     	*/
 	IRA::CString lanCobName;
	
	/**
	* pointer to Container Services
	*/
	ContainerServices* cs;

  	protected:
    	SmartPropertyPointer<RWlong> m_delay_sp;
    	SmartPropertyPointer<RWlong> m_cmdPos_sp;
    	SmartPropertyPointer<RWlong> m_Fmin_sp;
    	SmartPropertyPointer<RWlong> m_Fmax_sp;
    	SmartPropertyPointer<RWlong> m_acc_sp;
    	SmartPropertyPointer<RWlong> m_uBits_sp;
    	SmartPropertyPointer<RWdouble> m_lmCorr_sp;
    	SmartPropertyPointer<ROlong> m_actPos_sp;
    	SmartPropertyPointer<ROpattern> m_status_sp;
    	SmartPropertyPointer<ROlong> m_softVer_sp;
    	SmartPropertyPointer<ROlong> m_type_sp;
    	SmartPropertyPointer<RWdouble> m_gravCorr_sp;
    	SmartPropertyPointer<RWdouble> m_userOffset_sp;
};

#endif /*!USDImpl_H*/
