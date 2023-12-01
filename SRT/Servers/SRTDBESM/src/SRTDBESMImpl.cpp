#include <new>
#include <baciDB.h>
#include <ComponentErrors.h>
#include <ManagmentDefinitionsS.h>
#include "SRTDBESMImpl.h"

#include <sstream>

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr SRTDBESMImpl::PROPERTYNAME()\
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}
	
using namespace BackendsErrors;
using namespace ComponentErrors;
using namespace IRA;
using namespace std;


/*static char *rcsId="@(#) $Id: SRTDBESMImpl.cpp,v 1.7 2011-06-03 18:02:49 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);*/

_IRA_LOGFILTER_DECLARE;

SRTDBESMImpl::SRTDBESMImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_compConfiguration(),
	m_commandSocket(),
	m_paddr_1(this),
	m_paddr_2(this),
	m_paddr_3(this),
	m_paddr_4(this),
	m_pregs_1(this),
	m_pregs_2(this),
	m_pregs_3(this),
	m_pregs_4(this),
	m_patts_1(this),
	m_patts_2(this),
	m_patts_3(this),
   m_patts_4(this),
   m_pamps_1(this),
   m_pamps_2(this),
   m_pamps_3(this),
   m_pamps_4(this),
   m_peqs_1(this),
   m_peqs_2(this),
   m_peqs_3(this),
   m_peqs_4(this),
   m_pbpfs_1(this),
   m_pbpfs_2(this),
   m_pbpfs_3(this),
   m_pbpfs_4(this),
   m_pvolts_1(this),
   m_pvolts_2(this),
   m_pvolts_3(this),
   m_pvolts_4(this),
   m_ptemps_1(this),
   m_ptemps_2(this),
   m_ptemps_3(this),
   m_ptemps_4(this),
   m_pcfg_1(this),
   m_pcfg_2(this),
   m_pcfg_3(this),
   m_pcfg_4(this)
{	
	AUTO_TRACE("SRTDBESMImpl::SRTDBESMImpl()");
}

SRTDBESMImpl::~SRTDBESMImpl()
{
	AUTO_TRACE("SRTDBESMImpl::~SRTDBESMImpl()");
}

void SRTDBESMImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SRTDBESMImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"READING_CONFIGURATION"));	
	
	m_compConfiguration.init(getContainerServices()); // throw ComponentErrors::CDBAccessExImpl
	
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"CONFIGURATION_OK"));

	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"PROPERTY_CREATION"));
	
	try {		
		m_paddr_1=new ROlong(getContainerServices()->getName()+":addr_1",getComponent());
		m_paddr_2=new ROlong(getContainerServices()->getName()+":addr_2",getComponent());
		m_paddr_3=new ROlong(getContainerServices()->getName()+":addr_3",getComponent());
		m_paddr_4=new ROlong(getContainerServices()->getName()+":addr_4",getComponent());
		
		m_pregs_1=new ROlongSeq(getContainerServices()->getName()+":regs_1",getComponent());
      m_pregs_2=new ROlongSeq(getContainerServices()->getName()+":regs_2",getComponent());
      m_pregs_3=new ROlongSeq(getContainerServices()->getName()+":regs_3",getComponent());
	   m_pregs_4=new ROlongSeq(getContainerServices()->getName()+":regs_4",getComponent());
	   
		m_patts_1=new ROdoubleSeq(getContainerServices()->getName()+":atts_1",getComponent());
		m_patts_2=new ROdoubleSeq(getContainerServices()->getName()+":atts_2",getComponent());
		m_patts_3=new ROdoubleSeq(getContainerServices()->getName()+":atts_3",getComponent());
		m_patts_4=new ROdoubleSeq(getContainerServices()->getName()+":atts_4",getComponent());
		
		m_pamps_1=new ROlongSeq(getContainerServices()->getName()+":amps_1",getComponent());
		m_pamps_2=new ROlongSeq(getContainerServices()->getName()+":amps_2",getComponent());
		m_pamps_3=new ROlongSeq(getContainerServices()->getName()+":amps_3",getComponent());
		m_pamps_4=new ROlongSeq(getContainerServices()->getName()+":amps_4",getComponent());
		
		m_peqs_1=new ROlongSeq(getContainerServices()->getName()+":eqs_1",getComponent());
		m_peqs_2=new ROlongSeq(getContainerServices()->getName()+":eqs_2",getComponent());
		m_peqs_3=new ROlongSeq(getContainerServices()->getName()+":eqs_3",getComponent());
      m_peqs_4=new ROlongSeq(getContainerServices()->getName()+":eqs_4",getComponent());
      
      m_pbpfs_1=new ROlongSeq(getContainerServices()->getName()+":bpfs_1",getComponent());
      m_pbpfs_2=new ROlongSeq(getContainerServices()->getName()+":bpfs_2",getComponent());
      m_pbpfs_3=new ROlongSeq(getContainerServices()->getName()+":bpfs_3",getComponent());
      m_pbpfs_4=new ROlongSeq(getContainerServices()->getName()+":bpfs_4",getComponent());
		      
      m_pvolts_1=new ROdoubleSeq(getContainerServices()->getName()+":volts_1",getComponent());
      m_pvolts_2=new ROdoubleSeq(getContainerServices()->getName()+":volts_2",getComponent());
      m_pvolts_3=new ROdoubleSeq(getContainerServices()->getName()+":volts_3",getComponent());
      m_pvolts_4=new ROdoubleSeq(getContainerServices()->getName()+":volts_4",getComponent());
      
      m_ptemps_1=new ROdouble(getContainerServices()->getName()+":temps_1",getComponent());
      m_ptemps_2=new ROdouble(getContainerServices()->getName()+":temps_2",getComponent());
      m_ptemps_3=new ROdouble(getContainerServices()->getName()+":temps_3",getComponent());
      m_ptemps_4=new ROdouble(getContainerServices()->getName()+":temps_4",getComponent());
      
      m_pcfg_1=new ROstring(getContainerServices()->getName()+":cfg_1",getComponent());
      m_pcfg_2=new ROstring(getContainerServices()->getName()+":cfg_2",getComponent());
      m_pcfg_3=new ROstring(getContainerServices()->getName()+":cfg_3",getComponent());
      m_pcfg_4=new ROstring(getContainerServices()->getName()+":cfg_4",getComponent());
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"SRTDBESMImpl::initialize()");
		throw dummy;
		} 
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SRTDBESMImpl::execute() //throw (ACSErr::ACSbaseExImpl)
{
	ACS::Time timestamp;
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::execute()",(LM_INFO,"CONNECTING_CONTROL_SOCKET"));
	
	m_commandSocket.init(&m_compConfiguration); // this could throw an exception.....
	
   try {
			m_paddr_1->getDevIO()->write(m_compConfiguration.addr_1(),timestamp);
			m_paddr_2->getDevIO()->write(m_compConfiguration.addr_2(),timestamp);
			m_paddr_3->getDevIO()->write(m_compConfiguration.addr_3(),timestamp);
			m_paddr_4->getDevIO()->write(m_compConfiguration.addr_4(),timestamp);

			m_pregs_1->getDevIO()->write( m_compConfiguration.regs_1(),timestamp);
			m_pregs_2->getDevIO()->write( m_compConfiguration.regs_2(),timestamp);
			m_pregs_3->getDevIO()->write( m_compConfiguration.regs_3(),timestamp);
			m_pregs_4->getDevIO()->write( m_compConfiguration.regs_4(),timestamp);

			m_patts_1->getDevIO()->write( m_compConfiguration.atts_1(),timestamp);
			m_patts_2->getDevIO()->write( m_compConfiguration.atts_2(),timestamp);
			m_patts_3->getDevIO()->write( m_compConfiguration.atts_3(),timestamp);
			m_patts_4->getDevIO()->write( m_compConfiguration.atts_4(),timestamp);	
	
			m_pamps_1->getDevIO()->write( m_compConfiguration.amps_1(),timestamp);
			m_pamps_2->getDevIO()->write( m_compConfiguration.amps_2(),timestamp);
			m_pamps_3->getDevIO()->write( m_compConfiguration.amps_3(),timestamp);
   		m_pamps_4->getDevIO()->write( m_compConfiguration.amps_4(),timestamp);
   
   		m_peqs_1->getDevIO()->write( m_compConfiguration.eqs_1(),timestamp);
  			m_peqs_2->getDevIO()->write( m_compConfiguration.eqs_2(),timestamp);
			m_peqs_3->getDevIO()->write( m_compConfiguration.eqs_3(),timestamp);
			m_peqs_4->getDevIO()->write( m_compConfiguration.eqs_4(),timestamp);
	
			m_pbpfs_1->getDevIO()->write( m_compConfiguration.bpfs_1(),timestamp);
			m_pbpfs_2->getDevIO()->write( m_compConfiguration.bpfs_2(),timestamp);
			m_pbpfs_3->getDevIO()->write( m_compConfiguration.bpfs_3(),timestamp);
			m_pbpfs_4->getDevIO()->write( m_compConfiguration.bpfs_4(),timestamp);
	
			m_pvolts_1->getDevIO()->write( m_compConfiguration.volts_1(),timestamp);
			m_pvolts_2->getDevIO()->write( m_compConfiguration.volts_2(),timestamp);
			m_pvolts_3->getDevIO()->write( m_compConfiguration.volts_3(),timestamp);
   		m_pvolts_4->getDevIO()->write( m_compConfiguration.volts_4(),timestamp);
   
   		m_ptemps_1->getDevIO()->write( m_compConfiguration.temps_1(),timestamp);
   		m_ptemps_2->getDevIO()->write( m_compConfiguration.temps_2(),timestamp);
   		m_ptemps_3->getDevIO()->write( m_compConfiguration.temps_3(),timestamp);
   		m_ptemps_4->getDevIO()->write( m_compConfiguration.temps_4(),timestamp);
   		
   		m_pcfg_1->getDevIO()->write( m_compConfiguration.cfg_1(),timestamp);
   		m_pcfg_2->getDevIO()->write( m_compConfiguration.cfg_2(),timestamp);
   		m_pcfg_3->getDevIO()->write( m_compConfiguration.cfg_3(),timestamp);
   		m_pcfg_4->getDevIO()->write( m_compConfiguration.cfg_4(),timestamp);
   		}
			catch (ACSErr::ACSbaseExImpl& ex) {
		   	_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"SRTDBESMImpl::execute()");
				throw impl;
				}	
	
   ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::execute()",(LM_INFO,"COMMAND_SOCKET INITIALIZED"));
	
	try {
		
		startPropertiesMonitoring();
		
		}
		catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRTDBESMImpl::execute");
			throw __dummy;
		}
		catch (ACSErrTypeCommon::NullPointerExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRTDBESMImpl::execute");
			throw __dummy;		
		}
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SRTDBESMImpl::cleanUp()
{
	AUTO_TRACE("SRTDBESMImpl::cleanUp()");
	
	stopPropertiesMonitoring();	

	m_commandSocket.cleanUp();
	
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::cleanUp()",(LM_INFO,"CONTROL_SOCKET_CLOSED"));
	
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	
	CharacteristicComponentImpl::cleanUp();	
}

void SRTDBESMImpl::aboutToAbort()
{
	AUTO_TRACE("SRTDBESMImpl::aboutToAbort()");

	m_commandSocket.cleanUp();
	
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
}

void SRTDBESMImpl::set_all(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
	 AUTO_TRACE("SRTDBESMImpl::set_all()");
	 try {	
	 
    	m_commandSocket.set_all(cfg_name);
    	
    	  }
	 	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	}
 		catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		}	
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::set_all()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
		}
		
	CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::set_all()",(LM_INFO,"Whole DBESM configuration set"));	
}

void SRTDBESMImpl::set_mode(short b_addr, const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SRTDBESMImpl::set_mode()");
	try {
		
   	 m_commandSocket.set_mode(b_addr, cfg_name);
   	 
    	 }
	 	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	}
 		catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		}	
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::set_mode()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
		}
	CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::set_mode()",(LM_INFO,"Single board configuration set"));
}

void SRTDBESMImpl::set_att(short b_addr, short out_ch, double att_val) //throw (BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SRTDBESMImpl::set_att()");
	try {
		
    	m_commandSocket.set_att(b_addr, out_ch, att_val);
    	
   	 }
		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	}
 		catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		}	
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::set_att()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
		}
	CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::set_att()",(LM_INFO,"Attenuator set"));	
}

void SRTDBESMImpl::store_allmode(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SRTDBESMImpl::store_allmode()");
	try {
		
     	m_commandSocket.store_allmode(cfg_name);

      }
	 	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	}
 		catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		}	
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::store_allmode()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
			}
	CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::store_allmode()",(LM_INFO,"Configuration file written"));			 
}

void SRTDBESMImpl::delete_file(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SRTDBESMImpl::delete_file()");
	try {
		
    	m_commandSocket.delete_file(cfg_name);
    	
    	 }
		 catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	}
 		catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		}	
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::delete_file()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
			}
	CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::delete_file()",(LM_INFO,"Configuration file deleted"));		
}

void SRTDBESMImpl::get_status(short b_addr)// throw (BackendsErrors::BackendsErrorsEx)
{
	 AUTO_TRACE("SRTDBESMImpl::get_status()");
	 ACS::Time timestamp, timestamp2, timestamp3;
	 ACS::longSeq new_reg_vals;
	 ACS::doubleSeq new_att_vals;
	 new_reg_vals.length(10);
	 new_att_vals.length(17);
    string output;
    
    try {
    	
    	output= m_commandSocket.get_status(b_addr);
    	
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
		 }
 		catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		}
   	catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::get_status()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
		}
    
    char *c = new char[output.length()-2 + 1];  // discard \r\n
    std::copy(output.begin(), (output.end()-2), c);
    c[output.length()-2] = '\0';
    
    try {
    	m_commandSocket.parse_longSeq_response(output, "REG=[ ", " ]\n\n", &new_reg_vals);    // parsing errors are thrown
    	m_commandSocket.parse_doubleSeq_response(output, "ATT=[ ", " ]\r\n", &new_att_vals);  // by the CommandSocket
    }
    catch (std::out_of_range) {
			_EXCPT(BackendsErrors::MalformedAnswerExImpl,impl,"SRTDBESMImpl::get_status()");
			throw impl; }
 
   try {	 
   
   	 if (b_addr == m_paddr_1->getDevIO()->read(timestamp2))
    			{
    				m_pregs_1->getDevIO()->write(new_reg_vals,timestamp);
    				m_patts_1->getDevIO()->write(new_att_vals,timestamp3); 
    			}
    	else if (b_addr == m_paddr_2->getDevIO()->read(timestamp2))
   	  	 {
    				m_pregs_2->getDevIO()->write(new_reg_vals,timestamp);
    				m_patts_2->getDevIO()->write(new_att_vals,timestamp3); 
    			}
   	 else if (b_addr == m_paddr_3->getDevIO()->read(timestamp2))
   	  	 {
    				m_pregs_3->getDevIO()->write(new_reg_vals,timestamp);
    				m_patts_3->getDevIO()->write(new_att_vals,timestamp3); 
    		 }	
   	 else if (b_addr == m_paddr_4->getDevIO()->read(timestamp2))
   	  	 {
    				m_pregs_4->getDevIO()->write(new_reg_vals,timestamp);
    				m_patts_4->getDevIO()->write(new_att_vals,timestamp3); 
    		 }
    		
      }
	 catch (ComponentErrors::ComponentErrorsExImpl& e) {
		e.log(LM_DEBUG);
		throw e.getComponentErrorsEx();
	  }       
	 catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"SRTDBESMImpl::get_status()");
		throw impl;
				}	    
	 CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::get_status()",(LM_INFO,c));
}

void SRTDBESMImpl::get_comp(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
	 AUTO_TRACE("SRTDBESMImpl::get_comp()");
	 ACS::Time timestamp, timestamp2, timestamp3, timestamp4;
	 ACS::longSeq new_amp_vals, new_eq_vals, new_bpf_vals;
	 new_amp_vals.length(10);
	 new_eq_vals.length(10);
	 new_bpf_vals.length(11);
	 string output;
	 
	 try {
	 	
      output = m_commandSocket.get_comp(b_addr);
      
           }
	 	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	 }
 	 	catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
	 	}	
		 catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::get_diag()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
	  	}
    
    char *c = new char[output.length()-2 + 1];  // discard \r\n
    std::copy(output.begin(), (output.end()-2), c);
    c[output.length()-2] = '\0';
    
    try {
    	m_commandSocket.parse_longSeq_response(output, "AMP=[ ", " ]\n", &new_amp_vals);    
 		m_commandSocket.parse_longSeq_response(output, "EQ=[ ", " ]\nBPF", &new_eq_vals);
  		m_commandSocket.parse_longSeq_response(output, "BPF=[ ", " ]\r\n", &new_bpf_vals);
  	 } 
  	 catch (std::out_of_range) {
			_EXCPT(BackendsErrors::MalformedAnswerExImpl,impl,"SRTDBESMImpl::get_comp()");
			throw impl; }
    
	try {  
	   
    	if (b_addr == m_paddr_1->getDevIO()->read(timestamp))
    			{
    				m_pamps_1->getDevIO()->write(new_amp_vals,timestamp2);
    				m_peqs_1->getDevIO()->write(new_eq_vals,timestamp3);
    				m_pbpfs_1->getDevIO()->write(new_bpf_vals,timestamp4); 
    			}
  	   else if (b_addr == m_paddr_2->getDevIO()->read(timestamp))
   		   {
    				m_pamps_2->getDevIO()->write(new_amp_vals,timestamp2);
    				m_peqs_2->getDevIO()->write(new_eq_vals,timestamp3);
    				m_pbpfs_2->getDevIO()->write(new_bpf_vals,timestamp4);  
    			}
    	else if (b_addr == m_paddr_3->getDevIO()->read(timestamp))
   		   {
    				m_pamps_3->getDevIO()->write(new_amp_vals,timestamp2);
    				m_peqs_3->getDevIO()->write(new_eq_vals,timestamp3);
    				m_pbpfs_3->getDevIO()->write(new_bpf_vals,timestamp4);  
    			}	
   	 else if (b_addr == m_paddr_4->getDevIO()->read(timestamp))
   		   {
    				m_pamps_4->getDevIO()->write(new_amp_vals,timestamp2);
    				m_peqs_4->getDevIO()->write(new_eq_vals,timestamp3);
    				m_pbpfs_4->getDevIO()->write(new_bpf_vals,timestamp4);  
    			}
    			
    	 }
	 	 catch (ComponentErrors::ComponentErrorsExImpl& e) {
			e.log(LM_DEBUG);
			throw e.getComponentErrorsEx();   
		   		} 	 
		 catch (ACSErr::ACSbaseExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"SRTDBESMImpl::get_comp()");
			throw impl;
					}	        	
	 CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::get_comp()",(LM_INFO,c));
}

void SRTDBESMImpl::get_diag(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
	 AUTO_TRACE("SRTDBESMImpl::get_diag()");
	 ACS::Time timestamp, timestamp2, timestamp3;
	 ACS::doubleSeq new_volt_vals;
	 new_volt_vals.length(2);
	 double new_volt_val, new_volt_val2;
	 double new_temp_val;
	 string output;
	 
	 try {
	 	
    	 output = m_commandSocket.get_diag(b_addr);
    	 
        }
		 catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	 }
 		 catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		 }	
		 catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::get_diag()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
	 	 }
    
    char *c = new char[output.length()-2 + 1];  // discard \r\n
    std::copy(output.begin(), (output.end()-2), c);
    c[output.length()-2] = '\0';
    
    try {
    	 m_commandSocket.parse_double_response(output, "5V ", " 3V3", &new_volt_val);
  		 new_volt_vals[0] = new_volt_val;  														
         
   	 m_commandSocket.parse_double_response(output, "3V3 ", "\nT0", &new_volt_val2);
       new_volt_vals[1] = new_volt_val2;
         
   	 m_commandSocket.parse_double_response(output, "T0 ", "\r\n", &new_temp_val);
    }
    catch (std::out_of_range) {
			_EXCPT(BackendsErrors::MalformedAnswerExImpl,impl,"SRTDBESMImpl::get_diag()");
			throw impl; }
			
   try {
   	 
    	if (b_addr == m_paddr_1->getDevIO()->read(timestamp)) 
   		 {
    			 m_pvolts_1->getDevIO()->write(new_volt_vals,timestamp2);
    			 m_ptemps_1->getDevIO()->write(new_temp_val,timestamp3);
      	 }
       
   	 else if (b_addr == m_paddr_2->getDevIO()->read(timestamp)) 
   		 {
    			 m_pvolts_2->getDevIO()->write(new_volt_vals,timestamp2);
    			 m_ptemps_2->getDevIO()->write(new_temp_val,timestamp3);
       	}
       
    	else if (b_addr == m_paddr_3->getDevIO()->read(timestamp)) 
   		 {
    			 m_pvolts_3->getDevIO()->write(new_volt_vals,timestamp2);
    			 m_ptemps_3->getDevIO()->write(new_temp_val,timestamp3);
      	 }
       
    	else if (b_addr == m_paddr_4->getDevIO()->read(timestamp)) 
   		 {
    			 m_pvolts_4->getDevIO()->write(new_volt_vals,timestamp2);
    			 m_ptemps_4->getDevIO()->write(new_temp_val,timestamp3);
      	 }  
      	     
    	}
		 catch (ComponentErrors::ComponentErrorsExImpl& e) {
			e.log(LM_DEBUG);
			throw e.getComponentErrorsEx();
	 	 }    
		 catch (ACSErr::ACSbaseExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"SRTDBESMImpl::get_diag()");
			throw impl;
					}	    
	 CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::get_diag()",(LM_INFO,c));
}

void SRTDBESMImpl::get_cfg()
{
	 AUTO_TRACE("SRTDBESMImpl::get_cfg()");
	 string cfg_message;
	 int newlines =0;
	 try {
	 	
    	 cfg_message = m_commandSocket.get_cfg();
    	 
        }
		 catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	 }
 		 catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		 }	
		 catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::get_cfg()()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
	 	 }
    
    char *c = new char[cfg_message.length()+ 1];  // here we keep \r\n
    std::copy(cfg_message.begin(), (cfg_message.end()), c);
    c[cfg_message.length()] = '\0';
    
    int i =0; 
    int v[5];
    v[0] = 0;
    
    while (c[i] != '\0'){
    		if (( c[i] == '\n' && c[i+1] == '\n' ) || ( c[i] == '\r' && c[i+1] == '\n' )){
    			 v[newlines+1] = i;
    		    newlines++;
    		    }
    		i++;    
    		 }
    
    for (int j=0; j< newlines; j++) {
    	
	 	string new_str;
	 	ACS::Time timestamp, timestamp2, timestamp3;
	 	double b_addr=0;
	   string str = "BOARD xx";
	   
    	try {
    			m_commandSocket.parse_double_response(cfg_message.substr(v[j],v[j+1]), "BOARD ", " ", &b_addr);
   	 		m_commandSocket.parse_string_response(cfg_message.substr(str.length()+2,v[j+1]), " ", "\n", &new_str);
    		 }
    		 catch (std::out_of_range) {
			 _EXCPT(BackendsErrors::MalformedAnswerExImpl,impl,"SRTDBESMImpl::get_cfg()");
			 throw impl; }
			 
   	try {
   		
   	 	char *d = new char[new_str.length()+ 1];
       	std::copy(new_str.begin(), (new_str.end()), d);
       	d[new_str.length()] = '\0';
       	
    		if (b_addr == m_paddr_1->getDevIO()->read(timestamp)) 
   		 	{
    			 	m_pcfg_1->getDevIO()->write(d,timestamp3);
      	 	}
       
   	 	else if (b_addr == m_paddr_2->getDevIO()->read(timestamp)) 
   		 	{
    			 	m_pcfg_2->getDevIO()->write(d,timestamp3);
      	 	}
       
    		else if (b_addr == m_paddr_3->getDevIO()->read(timestamp)) 
   		 	{
    			 	m_pcfg_3->getDevIO()->write(d,timestamp3);
      	 	}
       
    		else if (b_addr == m_paddr_4->getDevIO()->read(timestamp)) 
   		 	{
    			 	m_pcfg_4->getDevIO()->write(d,timestamp3);
      	 	}  
      	     
    		}
		 	catch (ComponentErrors::ComponentErrorsExImpl& e) {
			e.log(LM_DEBUG);
			throw e.getComponentErrorsEx();
	 	 		}    
		 	catch (ACSErr::ACSbaseExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"SRTDBESMImpl::get_dbeatt()");
			throw impl;
				}		    
		}
    CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::get_cfg()",(LM_INFO,c));
}

void SRTDBESMImpl::set_dbeatt(const char * out_dbe, const char * att_val)
{
	 AUTO_TRACE("SRTDBESMImpl::set_dbeatt()");
	 string out_dbe_message;
	 try {
	 	
    	 out_dbe_message = m_commandSocket.set_dbeatt(out_dbe, att_val);
    	 
        }
		 catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	 }
 		 catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		 }	
		 catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::set_dbeatt()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
	 	 }
    
    char *c = new char[out_dbe_message.length()-2 + 1];  // discard \r\n
    std::copy(out_dbe_message.begin(), (out_dbe_message.end()-2), c);
    c[out_dbe_message.length()-2] = '\0';
    
    CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::set_dbeatt()",(LM_INFO,c));
}

void SRTDBESMImpl::get_dbeatt(const char * out_dbe)
{
	 AUTO_TRACE("SRTDBESMImpl::get_dbeatt()");
	 int newlines = 0;
	 string out_dbe_message;
	 try {
	 	
    	 out_dbe_message = m_commandSocket.get_dbeatt(out_dbe);
    	 
        }
		 catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	 }
 		 catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		 }	
		 catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::get_dbeatt()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
	 	 }
	 char *c = new char[out_dbe_message.length() + 1];  // here we keep \r\n
    std::copy(out_dbe_message.begin(), (out_dbe_message.end()), c);
    c[out_dbe_message.length()] = '\0';
    	 
    int i =0; 
    int v[5];
    v[0] = 0;
    while (c[i] != '\0'){
    		if ( c[i] == '\n' ){
    			 v[newlines+1] = i;
    		    newlines++;
    		    }
    		i++;    
    		 }
    
    for (int j=0; j< newlines; j++) {
	 	ACS::doubleSeq new_seq;
	 	ACS::Time timestamp, timestamp2, timestamp3;
	 	double b_addr=0, att_ch=0, new_att_val=0;
	 
    	try {
   	 	m_commandSocket.parse_double_response(out_dbe_message.substr(v[j],v[j+1]), "BOARD ", " ATT", &b_addr);
   	 	m_commandSocket.parse_double_response(out_dbe_message.substr(v[j],v[j+1]), "ATT ", " VALUE", &att_ch);
   	 	m_commandSocket.parse_double_response(out_dbe_message.substr(v[j],v[j+1]), "VALUE ", "\n", &new_att_val);
    		 }
    		catch (std::out_of_range) {
			_EXCPT(BackendsErrors::MalformedAnswerExImpl,impl,"SRTDBESMImpl::get_dbeatt()");
			throw impl; }
   	try {
   	 
    		if (b_addr == m_paddr_1->getDevIO()->read(timestamp)) 
   		 	{
    			 	new_seq = (*m_patts_1).getDevIO()->read(timestamp2);
    			 	new_seq[att_ch] = new_att_val;
    			 	m_patts_1->getDevIO()->write(new_seq,timestamp3);
      	 	}
       
   	 	else if (b_addr == m_paddr_2->getDevIO()->read(timestamp)) 
   		 	{
    			 	new_seq = (*m_patts_2).getDevIO()->read(timestamp2);
    			 	new_seq[att_ch] = new_att_val;
    			 	m_patts_2->getDevIO()->write(new_seq,timestamp3);
       		}
       
    		else if (b_addr == m_paddr_3->getDevIO()->read(timestamp)) 
   		 	{
    			 	new_seq = (*m_patts_3).getDevIO()->read(timestamp2);
    			 	new_seq[att_ch] = new_att_val;
    			 	m_patts_3->getDevIO()->write(new_seq,timestamp3);
      	 	}
       
    		else if (b_addr == m_paddr_4->getDevIO()->read(timestamp)) 
   		 	{
    			 	new_seq = (*m_patts_4).getDevIO()->read(timestamp2);
    			 	new_seq[att_ch] = new_att_val;
    			 	m_patts_4->getDevIO()->write(new_seq,timestamp3);
      	 	}  
      	     
    		}
		 	catch (ComponentErrors::ComponentErrorsExImpl& e) {
			e.log(LM_DEBUG);
			throw e.getComponentErrorsEx();
	 	 		}    
		 	catch (ACSErr::ACSbaseExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"SRTDBESMImpl::get_dbeatt()");
			throw impl;
				}		    
		}
		
    CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::get_dbeatt()",(LM_INFO,c));
}

void SRTDBESMImpl::get_firm(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
	 AUTO_TRACE("SRTDBESMImpl::get_firm()");
	 string output;
	 
	 try {
	 	
    	 output = m_commandSocket.get_firm(b_addr);
    	 
        }
		 catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
	 	 }
 		 catch (BackendsErrors::BackendsErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getBackendsErrorsEx();		
		 }	
		 catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::get_diag()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
	 	 }
    
    char *c = new char[output.length()-2 + 1];  // discard \r\n
    std::copy(output.begin(), (output.end()-2), c);
    c[output.length()-2] = '\0';

	 CUSTOM_LOG(LM_FULL_INFO,"SRTDBESMImpl::get_firm()",(LM_INFO,c));
	 
	 return c;
}


GET_PROPERTY_REFERENCE(ACS::ROlong,m_paddr_1,addr_1);
GET_PROPERTY_REFERENCE(ACS::ROlong,m_paddr_2,addr_2);
GET_PROPERTY_REFERENCE(ACS::ROlong,m_paddr_3,addr_3);
GET_PROPERTY_REFERENCE(ACS::ROlong,m_paddr_4,addr_4);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pregs_1,regs_1);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pregs_2,regs_2);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pregs_3,regs_3);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pregs_4,regs_4);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pamps_1,amps_1);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pamps_2,amps_2);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pamps_3,amps_3);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pamps_4,amps_4);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_peqs_1,eqs_1);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_peqs_2,eqs_2);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_peqs_3,eqs_3);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_peqs_4,eqs_4);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pbpfs_1,bpfs_1);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pbpfs_2,bpfs_2);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pbpfs_3,bpfs_3);
GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pbpfs_4,bpfs_4);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_patts_1,atts_1);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_patts_2,atts_2);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_patts_3,atts_3);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_patts_4,atts_4);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pvolts_1,volts_1);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pvolts_2,volts_2);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pvolts_3,volts_3);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pvolts_4,volts_4);
GET_PROPERTY_REFERENCE(ACS::ROdouble,m_ptemps_1,temps_1);
GET_PROPERTY_REFERENCE(ACS::ROdouble,m_ptemps_2,temps_2);
GET_PROPERTY_REFERENCE(ACS::ROdouble,m_ptemps_3,temps_3);
GET_PROPERTY_REFERENCE(ACS::ROdouble,m_ptemps_4,temps_4);
GET_PROPERTY_REFERENCE(ACS::ROstring,m_pcfg_1,cfg_1);
GET_PROPERTY_REFERENCE(ACS::ROstring,m_pcfg_2,cfg_2);
GET_PROPERTY_REFERENCE(ACS::ROstring,m_pcfg_3,cfg_3);
GET_PROPERTY_REFERENCE(ACS::ROstring,m_pcfg_4,cfg_4);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTDBESMImpl)

/*___oOo___*/
