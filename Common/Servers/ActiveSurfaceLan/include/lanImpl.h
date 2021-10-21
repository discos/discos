#ifndef lanImpl_h
#define lanImpl_h

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciROlong.h>
#include <baciRWdouble.h>
#include <baciSmartPropertyPointer.h>
#include <IRA>
#include <lanSocket.h>
#include <ASErrors.h>
#include <lanS.h>

using namespace baci;
using namespace IRA;
using namespace ASErrors;

/*******************************************************************************
*
* "@(#) $Id: lanImpl.h,v 1.1 2011-03-24 09:16:27 c.migoni Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* GMM       jul 2005   creation				   
*/

/**
*	@mainpage lan component
*	@date <26/01/2007> @version <1.1.0> @author gmaccaferri
*	@remarks compiled for ACS 5.0.3
*/

/**
 * This class lanImpl implements the single LAN/485 converter.
  * 
 * @author <a href=mailto:g.maccaferri@ira.inaf.it>Giuseppe Maccaferri</a>,
 * IRA, Bologna<br>
 * @version "@(#) $Id: lanImpl.h,v 1.1 2011-03-24 09:16:27 c.migoni Exp $"
 */
class lanImpl: public CharacteristicComponentImpl,public virtual POA_ActiveSurface::lan
//class lanImpl: public CharacteristicComponentImpl,public virtual POA_MOD_LAN::lan
{
  public:
    lanImpl(const ACE_CString&,maci::ContainerServices*);

    virtual ~lanImpl();

	/**
	*	send a command to an adressed USD and check for acknowledge reply
	*
	*	@param	cmd	one of the valid USD command 
	*	@param	addr  	USD address (1-32)
	*	@param	par	till 4 byte parameters for USD command
	*	@param	nBytes	the number of par bytes to be sent (max 4)
	*	@return	comp	complition
	*/
    virtual ACSErr::Completion* sendUSDCmd(CORBA::Long cmd,CORBA::Long addr,CORBA::Long par,CORBA::Long nBytes) throw (CORBA::SystemException);
	
	/**
	*  	get a value from adressed USD
	*	@param cmd	a valid USD command
	*	@param addr	the USD address (1-32)
	*	@param nBytes	number of bytes to be retrived (max 4)
	*	@param par 	the buffer for the returned bytes 
	*/
    virtual void recUSDPar(CORBA::Long cmd,CORBA::Long addr,CORBA::Long nBytes,CORBA::Long& par) throw (CORBA::SystemException,ASErrors::ASErrorsEx);
	

    virtual ACS::RWdouble_ptr 	delay() 			throw (CORBA::SystemException);
    virtual ACS::ROlong_ptr 	status()				throw (CORBA::SystemException);

 /* Override component lifecycle methods*/
    virtual void initialize() throw (ACSErr::ACSbaseExImpl);
    virtual void execute() throw (ACSErr::ACSbaseExImpl);
    virtual void cleanUp ();
    virtual void aboutToAbort ();


  private:

    /* 
     * lan socket pointer
     */
    lanSocket* m_psock;

    /*
    *  protected socket pointer
    */
    CSecureArea<lanSocket>* m_sock;

    /**
     * ALMA C++ coding standards state copy operators should be disabled.
     */
    void operator=(const lanImpl&);

  protected:
    SmartPropertyPointer<RWdouble> m_delay_sp;
    SmartPropertyPointer<ROlong> m_status_sp;
};

#endif /*!lanImpl_H*/
