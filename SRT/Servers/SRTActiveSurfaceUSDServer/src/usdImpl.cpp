#include <baciDB.h>
#include <usdImpl.h>
#include <maciContainerImpl.h>
#include <usdDevIO.i>

/*******************************************************************************
*
* "@(#) $Id: usdImpl.cpp,v 1.1 2011-03-24 09:18:26 c.migoni Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* GMM       jul 2005   creation				   
* GMM 	    sep 2006	ACS 5.0.3 adaptions
*/

using namespace maci;

USDImpl::USDImpl(const ACE_CString& CompName, maci::ContainerServices* containerServices) : 
    CharacteristicComponentImpl(CompName,containerServices),
    m_pLan(SRTActiveSurface::lan::_nil()),
    //m_pLan(MOD_LAN::lan::_nil()),
	m_delay_sp(this),
    m_cmdPos_sp(this),
    m_Fmin_sp(this),
    m_Fmax_sp(this),
    m_acc_sp(this),
    m_uBits_sp(this),
    m_lmCorr_sp(this),
    m_actPos_sp(this),
    m_status_sp(this),
    m_softVer_sp(this),
    m_type_sp(this),
    m_gravCorr_sp(this),
    m_userOffset_sp(this)	
{ 
	ACS_SHORT_LOG((LM_INFO,"::USDImpl::USDImpl: constructor;Constructor!"));
}

void USDImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{	
	cs = getContainerServices();
	ACSErr::Completion_var cp_sp;
	
	if (CIRATools::getDBValue(cs,"sector",(long&)m_sector) &&
		CIRATools::getDBValue(cs,"lan",(long&)m_lanNum) &&
		CIRATools::getDBValue(cs,"serialAddress",(long&)m_addr) &&
		CIRATools::getDBValue(cs,"fullRange",(long&)m_fullRange) &&
		CIRATools::getDBValue(cs,"zeroRef",(long&)m_zeroRef) &&
		CIRATools::getDBValue(cs,"cammaLen",(long&)m_cammaLen) &&
		CIRATools::getDBValue(cs,"cammaPos",(long&)m_cammaPos) &&
		CIRATools::getDBValue(cs,"step_giro",(long&)m_step_giro) &&
		CIRATools::getDBValue(cs,"step_res",(long&) m_rs) &&
		CIRATools::getDBValue(cs,"calibrate",(long&)m_calibrate))
	{
		ACS_SHORT_LOG((LM_INFO,"USD%d: CDB parameter read",m_addr));
	}
	else
	{
		ACS_LOG(LM_SOURCE_INFO,"USDImpl:initialize()",(LM_ERROR,"Error reading CDB!"));
		ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize() - Error reading CDB parameters");
		throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
	}
	m_step2deg=360/m_step_giro;
	m_step_res=1/pow(2,m_rs);
	m_top=-m_zeroRef;
	m_bottom=m_fullRange-m_zeroRef;

    // Activate the LAN component	
	lanCobName.Format("AS/SECTOR%02d/LAN%02d",m_sector,m_lanNum);
	
	// Use container to activate the object
	ACS_SHORT_LOG((LM_INFO, "Getting component: %s", (const char*)lanCobName) );
	//MOD_LAN::lan_var obj=MOD_LAN::lan::_nil();
	SRTActiveSurface::lan_var obj=SRTActiveSurface::lan::_nil();
	try {
		obj=cs->getComponent<SRTActiveSurface::lan>((const char *)lanCobName);
	}
	catch (maciErrType::CannotGetComponentExImpl)
	{	
		ACS_LOG(LM_SOURCE_INFO,"USDImpl:initialize()",(LM_ERROR,"Error linking lan component!"));
		ASErrors::CannotGetUSDExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize()");
		throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
	}
	
	//m_pLan = MOD_LAN::lan::_narrow(obj.in());
	m_pLan = SRTActiveSurface::lan::_narrow(obj.in());
	ACS_SHORT_LOG((LM_INFO,"lan linked!"));

	m_available = true;
	
	ACE_CString CompName(this->name());
	
	ACS_SHORT_LOG((LM_INFO,"Property creation..."));
	try {
		m_delay_sp = new RWlong(CompName+":delay",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,DELAY,1,0),true);
    	m_Fmin_sp=new RWlong(CompName+":Fmin",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,FMIN,2,0),true);
    	m_Fmax_sp=new RWlong(CompName+":Fmax",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,FMAX,2,0),true);
    	m_acc_sp=new RWlong(CompName+":acc",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,ACCL,1,0),true);
        m_uBits_sp=new RWlong(CompName+":uBits",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,UBIT,1,0),true);
    	m_actPos_sp=new ROlong(CompName+":actPos",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,APOS,0,4),true);
    	m_status_sp=new ROpattern(CompName+":status",getComponent(),new USDDevIO<ACS::pattern>(this,m_pLan,m_addr,STAT,0,3),true);
    	m_softVer_sp=new ROlong(CompName+":softVer",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,SVER,0,1),true);
    	m_type_sp=new ROlong(CompName+":type",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,USDT,0,1),true);
    	m_cmdPos_sp= new RWlong(CompName+":cmdPos",getComponent(),new USDDevIO<CORBA::Long>(this,m_pLan,m_addr,CPOS,4,0),true);
    	m_gravCorr_sp=new  RWdouble(CompName+":gravCorr",getComponent());
    	m_lmCorr_sp=new  RWdouble(CompName+":lmCorr",getComponent()); 
    	m_userOffset_sp=new  RWdouble(CompName+":userOffset",getComponent());
	}
	catch (ASErrors::ASErrorsExImpl ex)
	{
		_THROW_EXCPT_FROM_EXCPT(acsErrTypeLifeCycle::LifeCycleExImpl,ex,"usdImpl::initialize()")
	}
	
	try {
		stop();			// terminate any ongoing movement
		_GET_PROP(status,m_status,"usdImpl::initialize()")
		if(!(m_calibrate && m_status&ENBL))
            		reset(); // load the default params.
	}
	catch (ASErrors::ASErrorsEx& ex)
	{
		_THROW_EXCPT_FROM_EXCPT(acsErrTypeLifeCycle::LifeCycleExImpl,ex,"usdImpl::initialize()")
	}
	catch (CORBA::SystemException& ex)
	{
		_THROW_EXCPT(acsErrTypeLifeCycle::LifeCycleExImpl,"usdImpl::initialize()")
	}
	catch (...)
	{
		ACS_DEBUG("::usdImpl::initialize","Not captuerd exception!"); 
	}
	//////////////////////////////////////////
	_SET_PROP(Fmin,200,"usdImpl::initialize()")
	_SET_PROP(Fmax,500,"usdImpl::initialize()")
	_SET_PROP(delay,5,"usdImpl::initialize()")
	_SET_PROP(acc,50,"usdImpl::initialize()")
	//////////////////////////////////////////
}

void USDImpl::execute() throw (ACSErr::ACSbaseExImpl)
{	
/*	// Activate the LAN component	
	lanCobName.Format("AS/SECTOR%02d/LAN%02d",m_sector,m_lanNum);
	
	// Use container to activate the object
	ACS_SHORT_LOG((LM_INFO, "Getting component: %s", (const char*)lanCobName) );
	//MOD_LAN::lan_var obj=MOD_LAN::lan::_nil();
	SRTActiveSurface::lan_var obj=SRTActiveSurface::lan::_nil();
	try {
		obj=cs->getComponent<SRTActiveSurface::lan>((const char *)lanCobName);
	}
	catch (maciErrType::CannotGetComponentExImpl)
	{	
		ACS_LOG(LM_SOURCE_INFO,"USDImpl:initialize()",(LM_ERROR,"Error linking lan component!"));
		ASErrors::CannotGetUSDExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize()");
		throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
	}
	
	//m_pLan = MOD_LAN::lan::_narrow(obj.in());
	m_pLan = SRTActiveSurface::lan::_narrow(obj.in());
	ACS_SHORT_LOG((LM_INFO,"lan linked!"));

	m_available = true;
	
	ACE_CString CompName(this->name());
	
	ACS_SHORT_LOG((LM_INFO,"Property creation..."));
	try {
		m_delay_sp = new RWlong(CompName+":delay",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,DELAY,1,0),true);
    	m_Fmin_sp=new RWlong(CompName+":Fmin",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,FMIN,2,0),true);
    	m_Fmax_sp=new RWlong(CompName+":Fmax",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,FMAX,2,0),true);
    	m_acc_sp=new RWlong(CompName+":acc",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,ACCL,1,0),true);
        m_uBits_sp=new RWlong(CompName+":uBits",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,UBIT,1,0),true);
    	m_actPos_sp=new ROlong(CompName+":actPos",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,APOS,0,4),true);
    	m_status_sp=new ROpattern(CompName+":status",getComponent(),new USDDevIO<ACS::pattern>(this,m_pLan,m_addr,STAT,0,3),true);
    	m_softVer_sp=new ROlong(CompName+":softVer",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,SVER,0,1),true);
    	m_type_sp=new ROlong(CompName+":type",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,USDT,0,1),true);
    	m_cmdPos_sp= new RWlong(CompName+":cmdPos",getComponent(),new USDDevIO<Long>(this,m_pLan,m_addr,CPOS,4,0),true);
    	m_gravCorr_sp=new  RWdouble(CompName+":gravCorr",getComponent());
    	m_lmCorr_sp=new  RWdouble(CompName+":lmCorr",getComponent()); 
    	m_userOffset_sp=new  RWdouble(CompName+":userOffset",getComponent());
	}
	catch (ASErrors::ASErrorsExImpl ex)
	{
		_THROW_EXCPT_FROM_EXCPT(acsErrTypeLifeCycle::LifeCycleExImpl,ex,"usdImpl::initialize()")
	}
	
	try {
		stop();			// terminate any ongoing movement
        printf("calibrate = %d\n", m_calibrate);
        printf("ENBL      = %d\n", m_status&ENBL);
		_GET_PROP(status,m_status,"usdImpl::initialize()")
        printf("calibrate = %d\n", m_calibrate);
        printf("ENBL      = %d\n", m_status&ENBL);
		if(!(m_calibrate && m_status&ENBL)) {
            reset(); // load the default params.
            printf("calibrate = %d\n", m_calibrate);
            printf("ENBL      = %d\n", m_status&ENBL);
        }
	}
	catch (ASErrors::ASErrorsEx& ex)
	{
		_THROW_EXCPT_FROM_EXCPT(acsErrTypeLifeCycle::LifeCycleExImpl,ex,"usdImpl::initialize()")
	}
	catch (CORBA::SystemException& ex)
	{
		_THROW_EXCPT(acsErrTypeLifeCycle::LifeCycleExImpl,"usdImpl::initialize()")
	}
	catch (...)
	{
		ACS_DEBUG("::usdImpl::initialize","Not captuerd exception!"); 
	}
    ////////////////////////////////////////// 22 giugno 2009
	//_SET_PROP(Fmax,250,"usdImpl::calibrate()")
    ////////////////////////////////////////// calibration test; by CM
    printf ("RS0        in execute = %d\n", m_status&0x000001);
    printf ("RS1        in execute = %d\n", m_status&0x000002);
    printf ("RS2        in execute = %d\n", m_status&0x000004);
    printf ("resolution in execute = %d\n", m_status&0x000008);
    printf ("m_rs in initialize    = %d\n", m_rs);
*/

	ACS_SHORT_LOG((LM_INFO,"::USDImpl::execute(): USD component ready!"));
}

 // Building Destructor
void USDImpl::cleanUp()
{
	ACS_TRACE("::USDImpl::cleanUp()");

	// must release the lan component
	if(CORBA::is_nil(m_pLan) == false)
	{
		ACS_LOG(LM_RUNTIME_CONTEXT, "::USDImpl::cleanUp",(LM_DEBUG, "Releasing LANx"));
		try {
			cs->releaseComponent((const char*)lanCobName);
		}
		
		catch (maciErrType::CannotReleaseComponentExImpl)
		{
			ACS_SHORT_LOG((LM_INFO, "cannot release lan component %s", (const char*)lanCobName));
		}
		 // be sure to set the reference to nil
		 //m_pLan = MOD_LAN::lan::_nil();
		 m_pLan = SRTActiveSurface::lan::_nil();
	}
}

 
void USDImpl::aboutToAbort()
{
	ACS_TRACE("::USDImpl::aboutToAbort()");

	  try {
			cs->releaseComponent((const char*)lanCobName);
		}
		
		catch (maciErrType::CannotReleaseComponentExImpl)
		{
			ACS_SHORT_LOG((LM_INFO, "cannot release lan component %s", (const char*)lanCobName));
		}
	
	 // be sure to set the reference to nil
		 //m_pLan = MOD_LAN::lan::_nil();
		 m_pLan = SRTActiveSurface::lan::_nil();
}

void USDImpl::reset() 	throw (CORBA::SystemException,ASErrors::ASErrorsEx)
{ 
	ACS_TRACE("::USDImpl::reset()");
	
	m_calibrate = 0;
	
  	try {
        _SET_CDB(calibrate,m_calibrate,"::USDImpl::reset()")
        action(RESET);

        //* restore defaults *//
        _SET_LDEF(delay,"USDImpl::reset()");
    	_SET_LDEF(Fmax,"USDImpl::reset()");
    	_SET_LDEF(Fmin,"USDImpl::reset()");
    	_SET_LDEF(acc,"USDImpl::reset()");
    	_SET_LDEF(uBits,"USDImpl::reset()");

    	action(RESL,m_rs,1);			// resolution

        /////////////////////////////////////////////////////////
        _SET_PROP(Fmin,200,"USDImpl::reset()")
        _SET_PROP(Fmax,500,"USDImpl::reset()")
	    _SET_PROP(delay,5,"USDImpl::reset()")
	    _SET_PROP(acc,50,"USDImpl::reset()")
	    _SET_PROP(uBits,34,"USDImpl::reset()")
        /////////////////////////////////////////////////////////
    }
  
  	_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"USDImpl::reset()",m_addr)
        
	ACS_SHORT_LOG((LM_WARNING,"USD %d resetted and initialized!",m_addr));

} //  reset()

ACSErr::Completion* USDImpl::calibrate() throw (CORBA::SystemException,ASErrors::ASErrorsEx)
{
	ACS_TRACE("::USDImpl::calibrate()");

	long fmax,fmin;
	ACSErr::Completion_var cp_sp; 
	
	
	int ifp=0;
	long cammaBegin=0, cammaEnd=0;
		
	m_calibrate=false;
	m_cammaLen=m_cammaPos=-1;
	_SET_CDB(calibrate,m_calibrate,"::usdImpl::calibrate");

	try {
		action(STOP);
		ACS_DEBUG("::usdImpl::calibrate","stopped!");
		action(LCNTR,m_step_giro<<USxS,4); // load the counter with know value
		ACS_DEBUG_PARAM("::usdImpl::calibrate","Loaded %d on counter",m_step_giro);
		
		_GET_PROP(Fmax,fmax,"usdImpl::calibrate()")
		_GET_PROP(Fmin,fmin,"usdImpl::calibrate()")
		_SET_PROP(Fmax,fmin,"usdImpl::calibrate()")
		ACS_DEBUG_PARAM("::usdImpl::calibrate","Fmax set to:%d",fmin);
		
		action(HSTOP,9,1); 						// sets the stop @ camma on
		ACS_DEBUG("::usdImpl::calibrate","hard stop to 1");

		action(GO,-1,1);		// down                                     
		ACS_DEBUG("::usdImpl::calibrate","down to find camma");
		
		ifp=1;
		CIRATools::Wait(3,0);
		
		_GET_PROP(status,m_status,"usdImpl::calibrate()")

		if(m_status&MRUN) {
			ACS_DEBUG("::usdImpl::calibrate","camma begin not found!");
			_THROW_EX(USDStillRunning,"::usdImpl::calibrate()",ifp);
			action(STOP);
		}
		
		_GET_PROP(actPos,cammaBegin,"usdImpl::calibrate()")
		ACS_DEBUG_PARAM("::usdImpl::calibrate","Camma begin at:%d",cammaBegin);
		
		action(HSTOP,1,1);  					 // sets the stop @ camma off
		ACS_DEBUG("::usdImpl::calibrate","hard stop to 0");
		
		action(GO,-1,1);		// down                                       
		ACS_DEBUG("::usdImpl::calibrate","down to find end of camma");
		
		ifp=2;
		CIRATools::Wait(3,0);
		_GET_PROP(status,m_status,"usdImpl::calibrate()")
		if(m_status&MRUN) {
			ACS_DEBUG("::usdImpl::calibrate","camma end not found!");
			_THROW_EX(USDStillRunning,"::usdImpl::calibrate()",ifp);
			action(STOP);
		}

		_GET_PROP(actPos,cammaEnd,"usdImpl::calibrate()")
		ACS_DEBUG_PARAM("::usdImpl::calibrate","Camma end at:%d",cammaEnd);
 
		m_cammaLen=cammaBegin-cammaEnd;
		m_cammaPos=cammaEnd+m_cammaLen/2;
 		ACS_DEBUG_PARAM("::usdImpl::calibrate","cammaPos:%d(step)",m_cammaPos);

		action(HSTOP,0,1); // disable HW stop 
		ACS_DEBUG("::usdImpl::calibrate","hstop disabled!");

		_SET_PROP(Fmax,fmax,"usdImpl::calibrate()")
		_SET_PROP(cmdPos,m_cammaPos,"usdImpl::calibrate()")	
		
		CIRATools::Wait(1,0);
		action(LCNTR,m_top<<USxS,4); // load the top_scale value on counter
	}

	_CATCH_EXCP_THROW_EX(CORBA::SystemException,corbaError,"::usdImpl::calibrate()",ifp)
	_CATCH_EXCP_THROW_EX(ASErrors::DevIOErrorEx,DevIOError,"::usdImpl::calibrate()",ifp)		// for _GET_PROP
	_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"::usdImpl::calibrate()",ifp)	// for local USD excp

	m_calibrate=true;
	
	_SET_CDB(cammaLen,m_cammaLen,"::usdImpl::calibrate()");
	_SET_CDB(cammaPos,m_cammaPos,"::usdImpl::calibrate()");
	_SET_CDB(calibrate,m_calibrate,"::usdImpl::calibrate()");
	
	ASErrors::USDCalibratedCompletion* cIp=new  ASErrors::USDCalibratedCompletion(__FILE__,__LINE__,"::usdImpl::calibrate()");
	cIp->setCammaLen(m_cammaLen*m_step2deg);
	cIp->setCammaPos(m_cammaPos*m_step2deg);	
	return cIp->returnCompletion();
}

ACSErr::Completion* USDImpl::calVer() throw (CORBA::SystemException,ASErrors::ASErrorsEx)
{
	ACS_TRACE("::USDImpl::calVer()");

	int ifp=0;
	_VAR_CHK_EX(m_available,USDUnavailable,"::usdImpl::calver()");
	_VAR_CHK_EX(m_calibrate,USDunCalibrated,"::usdImpl::calver()");
    	
	try {

			action(CPOS,m_top<<USxS,4);			// top
			_GET_PROP(status,m_status,"usdImpl::calibrate()")
		 
			ifp=10;
			if(stillRunning(m_top)) 	{	
				action(STOP);
				_THROW_EX(USDStillRunning,"::usdImpl::calver()",ifp);
			}
			if (!chkCal())
                _THROW_EX(USDunCalibrated,"::usdImpl::calver()",ifp);
				
			ifp=11;
			int incr=m_cammaLen/2+5;
			action(RPOS,incr<<USxS,4);			// move half camma len plus an istheresis to get sensor off
			CIRATools::Wait(0,500000);	// 0.5 secs
			if (!chkCal()) 							_THROW_EX(USDunCalibrated,"::usdImpl::calver()",ifp);
			
			action(CPOS, m_bottom<<USxS,4);		// to bottom
			ifp=20;
			_GET_PROP(status,m_status,"usdImpl::calibrate()")
			
			if(stillRunning(m_bottom)) {	
				action(STOP);
				_THROW_EX(USDStillRunning,"::usdImpl::calver()",ifp);
			}
			
			if (!chkCal()) 							_THROW_EX(USDunCalibrated,"::usdImpl::calver()",ifp);
		
			action(CPOS,0,4);	//to zero
			ifp=30;
			_GET_PROP(status,m_status,"usdImpl::calibrate()")
			
			if(stillRunning(0)) 	{	
				action(STOP);
				_THROW_EX(USDStillRunning,"::usdImpl::calver()",ifp);
			}
			
			if (!chkCal()) 							_THROW_EX(USDunCalibrated,"::usdImpl::calver()",ifp);
	
	}

	_CATCH_EXCP_THROW_EX(CORBA::SystemException,corbaError,"::usdImpl::calver()",ifp)	// for CORBA
	_CATCH_EXCP_THROW_EX(ASErrors::DevIOErrorEx,DevIOError,"::usdImpl::calver()",ifp)	// for _GET_PROP
	_CATCH_ACS_EXCP_THROW_EX(ASErrors::ASErrorsExImpl,USDError,"::usdImpl::calver()",ifp)	// for local USD excp
	
	CompletionImpl* cp=new  ASErrors::NoErrorCompletion();
	return cp->returnCompletion();
}

void USDImpl::exImplCheck(ASErrors::ASErrorsExImpl ex)
{
	ACS_TRACE("USDImpl::exImplCheck()");

	int err;

	switch (err=ex.getErrorCode()) {
		// warnings	
		case ASErrors::SocketReconn:
            {
            //    _THROW_EX(SocketReconn,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_WARNING,"Warning exception %d",err));
                break;
            }
		case ASErrors::Incomplete:
            {
             //   _THROW_EX(Incomplete,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_WARNING,"Warning exception %d",err));
                break;
            }
		case ASErrors::USDUnavailable: 
            {
             //   _THROW_EX(USDUnavailable,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_WARNING,"Warning exception %d",err));
                break;
            }
		case ASErrors::Nak: 
            {
             //   _THROW_EX(Nak,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_WARNING,"Warning exception %d",err));
                break;
            }
		case ASErrors::USDunCalibrated:
		    {
             //   _THROW_EX(USDunCalibrated,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_WARNING,"Warning exception %d",err));
			    break;
            }

		//critical errors compromising the USD working	
		case ASErrors::LibrarySocketError: 
            {
             //   _THROW_EX(LibrarySocketError,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_CRITICAL,"Critical exception %d",err));
                m_available=false;
                break;
            }
		case ASErrors::USDConnectionError: 
            {
             //   _THROW_EX(USDConnectionError,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_CRITICAL,"Critical exception %d",err));
                m_available=false;
                break;
            }
		case ASErrors::USDTimeout: 
            {
             //   _THROW_EX(USDTimeout,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_CRITICAL,"Critical exception %d",err));
                m_available=false;
                break;
            }
		case ASErrors::SocketTOut:
            {
             //   _THROW_EX(SocketTOut,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_CRITICAL,"Critical exception %d",err));
                m_available=false;
                break;
            }
		case ASErrors::SocketFail: 
		    {
             //   _THROW_EX(SocketFail,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_CRITICAL,"Critical exception %d",err));
                m_available=false;
			    break;
            }

		//  errors		
		default:
            {
             //   _THROW_EX(USDError,"::usdImpl::exImplCheck()",err);
			    ACS_SHORT_LOG((LM_ERROR,"Error exception %d",err));
			    break;
            }
	}
    
	ex.log();
}

void USDImpl::exCheck(ASErrors::ASErrorsEx ex) 
{
	ACS_TRACE("USDImpl::exCheck()");

	ASErrors::ASErrorsExImpl exImpl(ex);
	exImplCheck(exImpl);
}
 
bool USDImpl::compCheck(ACSErr::CompletionImpl& comp) 
{
	ACS_TRACE("USDImpl::compCheck()");

	if(comp.isErrorFree())	{
	return false;
	} else {
		// convert a completion in C++ excpt
		ASErrors::USDErrorExImpl ex(comp.getErrorTraceHelper()->getErrorTrace()); // senza accodamento
		//ASErrors::USDErrorExImpl ex(&comp,__FILE__,__LINE__,"::usdImpl::calibrate()"); con accodamento
		exImplCheck(ex);
   	return true;
	}
} 

void USDImpl::action( int act,int par,int nb) throw (ASErrors::ASErrorsExImpl)
{
	ACS_TRACE("usdImpl::action()");
	    
       _VAR_CHK_EXIMPL(m_available,USDUnavailable,"usdImpl::action()");   //throw USD unuvailable excpt

       try {
	CompletionImpl comp(m_pLan->sendUSDCmd(act,m_addr,par,nb));
        if (compCheck(comp))
        	throw ASErrors::sendCmdErrExImpl(comp,__FILE__,__LINE__,"usdImpl::action()");
        }
	_CATCH_EXCP_THROW_EXIMPL(CORBA::SystemException,ASErrors::corbaErrorExImpl,"usdImpl::action()",act)
}

bool USDImpl::stillRunning(long pos) throw (ASErrors::ASErrorsExImpl)
{
		ACS_TRACE("usdImpl::stillRunning()");

	ACSErr::Completion_var cp_sp;
	bool running;
	int tout;
	long fmax, startpos;
	time_t endt;

 	try {
		_GET_PROP(Fmax,fmax,"usdImpl::stillRunning()")
		_GET_PROP(actPos,startpos,"usdImpl::stillRunning()")
		tout=labs(pos-startpos)/fmax+2;	// adds two secs for safety
		
		for(endt=time(NULL)+tout; time(NULL)<endt;CIRATools::Wait(0,500000)) // loop every 1/2 sec
		{
			_GET_PROP(status,m_status,"usdImpl::stillRunning()")
			if(!(m_status&MRUN)) break;				// exit if stopped
		}
	}
	_CATCH_EXCP_THROW_EXIMPL(CORBA::SystemException,ASErrors::corbaErrorExImpl,"USDImpl::stillRunning()",m_status)
	_CATCH_EXCP_THROW_EXIMPL(ASErrors::DevIOErrorEx,ASErrors::DevIOErrorExImpl,"USDImpl::stillRunning()",m_status)	// for _GET_PROP

	running=m_status&MRUN;
	ACS_DEBUG_PARAM("::usdImpl::stillRunning","running:%d",running);
	if (running) ACS_SHORT_LOG((LM_WARNING,"stillRunning():USD %d still running!",m_addr));
	return running;
}

bool USDImpl::chkCal() throw (ASErrors::ASErrorsExImpl)
{
	ACS_TRACE("usdImpl::chkCal()");

	int fgiro;

 	try {
		_GET_PROP(actPos,fgiro,"USDImpl::chkCal()")
		_GET_PROP(status,m_status,"usdImpl::calibrate()")
			fgiro%=m_step_giro;

	}
	_CATCH_EXCP_THROW_EXIMPL(CORBA::SystemException,ASErrors::corbaErrorExImpl,"USDImpl::chkCal()",m_status)
	_CATCH_EXCP_THROW_EXIMPL(ASErrors::DevIOErrorEx,ASErrors::DevIOErrorExImpl,"USDImpl::chkCal()",m_status)	// for _GET_PROP

	if (fgiro > m_step_giro/2) fgiro=m_step_giro-fgiro;  //compute complement to full turn
	ACS_DEBUG_PARAM("::usdImpl::isRunning","fgiro: %d ",fgiro);
	ACS_DEBUG_PARAM("::usdImpl::isRunning"," camma: %d",m_status&CAMM);
	if(fgiro < m_cammaLen/2 && m_status&CAMM) return true;
	else if (fgiro > m_cammaLen/2 && !(m_status&CAMM)) return true;
	else return false;
}

/* ----------------------------------------------------------------*/
USDImpl::~USDImpl()
{
    // ACS_TRACE is used for debugging purposes
    ACS_TRACE("::USDImpl::~USDImpl");
    ACS_DEBUG_PARAM("::USDImpl::~USDImpl", "Destroying %s...", name());
    
}

       

/* --------------------- [ CORBA interface ] ----------------------*/


ACS::RWlong_ptr USDImpl::delay() throw (CORBA::SystemException)
{
	if(m_delay_sp==0) return ACS::RWlong::_nil();
	ACS::RWlong_var prop=ACS::RWlong::_narrow(m_delay_sp->getCORBAReference());
	return prop._retn();
}

ACS::RWlong_ptr USDImpl::cmdPos() throw (CORBA::SystemException)
{
	if(m_cmdPos_sp==0) return ACS::RWlong::_nil();
	ACS::RWlong_var prop=ACS::RWlong::_narrow(m_cmdPos_sp->getCORBAReference());
	return prop._retn();
}

ACS::RWlong_ptr USDImpl::Fmin() throw (CORBA::SystemException)
{
	if(m_Fmin_sp==0) return ACS::RWlong::_nil();
	ACS::RWlong_var prop=ACS::RWlong::_narrow(m_Fmin_sp->getCORBAReference());
	return prop._retn();
}

ACS::RWlong_ptr USDImpl::Fmax() throw (CORBA::SystemException)
{
	if(m_Fmax_sp==0) return ACS::RWlong::_nil();
	ACS::RWlong_var prop=ACS::RWlong::_narrow(m_Fmax_sp->getCORBAReference());
	return prop._retn();
}

ACS::RWlong_ptr USDImpl::acc() throw (CORBA::SystemException)
{
	if(m_acc_sp==0) return ACS::RWlong::_nil();
	ACS::RWlong_var prop=ACS::RWlong::_narrow(m_acc_sp->getCORBAReference());
	return prop._retn();
}

ACS::RWlong_ptr USDImpl::uBits() throw (CORBA::SystemException)
{
	if(m_uBits_sp==0) return ACS::RWlong::_nil();
	ACS::RWlong_var prop=ACS::RWlong::_narrow(m_uBits_sp->getCORBAReference());
	return prop._retn();
}

ACS::RWdouble_ptr USDImpl::lmCorr() throw (CORBA::SystemException)
{
	if(m_lmCorr_sp==0) return ACS::RWdouble::_nil();
	ACS::RWdouble_var prop=ACS::RWdouble::_narrow(m_lmCorr_sp->getCORBAReference());
	return prop._retn();
}

ACS::ROlong_ptr USDImpl::actPos() throw (CORBA::SystemException)
{
	if(m_actPos_sp==0) return ACS::ROlong::_nil();
	ACS::ROlong_var prop=ACS::ROlong::_narrow(m_actPos_sp->getCORBAReference());
	return prop._retn();
}

ACS::ROpattern_ptr USDImpl::status() throw (CORBA::SystemException)
{
	if(m_status_sp==0) return ACS::ROpattern::_nil();
	ACS::ROpattern_var prop=ACS::ROpattern::_narrow(m_status_sp->getCORBAReference());
	return prop._retn();
}

ACS::ROlong_ptr USDImpl::softVer() throw (CORBA::SystemException)
{
	if(m_softVer_sp==0) return ACS::ROlong::_nil();
	ACS::ROlong_var prop=ACS::ROlong::_narrow(m_softVer_sp->getCORBAReference());
	return prop._retn();
}

ACS::ROlong_ptr USDImpl::type() throw (CORBA::SystemException)
{
	if(m_type_sp==0) return ACS::ROlong::_nil();
	ACS::ROlong_var prop=ACS::ROlong::_narrow(m_type_sp->getCORBAReference());
	return prop._retn();
}

ACS::RWdouble_ptr USDImpl::gravCorr() throw (CORBA::SystemException)
{
	if(m_gravCorr_sp==0) return ACS::RWdouble::_nil();
	ACS::RWdouble_var prop=ACS::RWdouble::_narrow(m_gravCorr_sp->getCORBAReference());
	return prop._retn();
}

ACS::RWdouble_ptr USDImpl::userOffset() throw (CORBA::SystemException)
{
	if(m_userOffset_sp==0) return ACS::RWdouble::_nil();
	ACS::RWdouble_var prop=ACS::RWdouble::_narrow(m_userOffset_sp->getCORBAReference());
	return prop._retn();
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(USDImpl)
/* ----------------------------------------------------------------*/


/*___oOo___*/


