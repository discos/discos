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
   m_ptemps_4(this)
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
}

void SRTDBESMImpl::set_mode(short b_addr, const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
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
}

void SRTDBESMImpl::set_att(short b_addr, short out_ch, double att_val) //throw (BackendsErrors::BackendsErrorsEx)
{
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
}

void SRTDBESMImpl::store_allmode(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
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
}

void SRTDBESMImpl::clr_mode(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
	try {
		
    	m_commandSocket.clr_mode(cfg_name);
    	
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
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::clr_mode()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
			}
}

char * SRTDBESMImpl::get_status(short b_addr)// throw (BackendsErrors::BackendsErrorsEx)
{
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
			_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SRTDBESMImpl::clr_mode()");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();
		}
    
    char *c = new char[output.length()-2 + 1];  // discard \r\n
    std::copy(output.begin(), (output.end()-2), c);
    c[output.length()-2] = '\0';
    
    m_commandSocket.parse_longSeq_response(output, "REG=[ ", " ]\n\n", &new_reg_vals);    // parsing errors are thrown
    m_commandSocket.parse_doubleSeq_response(output, "ATT=[ ", " ]\r\n", &new_att_vals);  // by the commandSocket
 
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
					    		    		
    return c;
}

char * SRTDBESMImpl::get_comp(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
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
    
    m_commandSocket.parse_longSeq_response(output, "AMP=[ ", " ]\n", &new_amp_vals);    
 	 m_commandSocket.parse_longSeq_response(output, "EQ=[ ", " ]\nBPF", &new_eq_vals);
  	 m_commandSocket.parse_longSeq_response(output, "BPF=[ ", " ]\r\n", &new_bpf_vals);
    
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
	  	    		
    return c;
}

char * SRTDBESMImpl::get_diag(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
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

    m_commandSocket.parse_double_response(output, "5V ", " 3V3", &new_volt_val);
  	 new_volt_vals[0] = new_volt_val;  														
         
    m_commandSocket.parse_double_response(output, "3V3 ", "\nT0", &new_volt_val2);
    new_volt_vals[1] = new_volt_val2;
         
    m_commandSocket.parse_double_response(output, "T0 ", "\r\n", &new_temp_val);
    
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

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTDBESMImpl)

/*___oOo___*/
