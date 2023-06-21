//#include <usdImpl.h>
//#include <usdDevIO.h>

	template <class T> T USDDevIO<T>::read( ACS::Time &timestamp)     throw (ASErrors::ASErrorsExImpl)
	{
		ACS_TRACE("::USDDevIO::read()");
		
		CORBA::Long value; 
		// T value; da errore in compilazione, anche se mi sembra piu' corretta questa
		
		if( m_rLen==0) return m_value;		// no hardware read for WO properties

		if (m_pusd->m_available) {	
			
			try {
				m_pLan->recUSDPar(m_cmd,m_addr,m_rLen,value);
			}
			catch (ASErrors::ASErrorsEx& ex) {
				m_pusd->exCheck(ex);
				throw ASErrors::USDErrorExImpl(ex,__FILE__,__LINE__,"USDDevIO::read()");
			}  
			
			catch (CORBA::SystemException& corbaEx) {
				ASErrors::corbaErrorExImpl exImpl(__FILE__,__LINE__,"USDDevIO::read()");
				_ADD_MEMBER(exImpl,m_addr);
				_ADD_MEMBER(exImpl,m_cmd);		
				throw exImpl;
			}
		
			timestamp=getTimeStamp();
			if(m_cmd==APOS) value>>=USxS;														// ustep to step position conversion
			else if(m_cmd==FMIN || m_cmd==FMAX) value=long(value);		// velocity 
			else if(m_cmd==STAT ) {
				m_pusd->m_calibrate ? value|=CAL : value&=~CAL;		// status CAL integration
				m_pusd->m_ploop ? value|=PAUT : value&=~PAUT;		// status  PAUT integration
			}
			m_value=value;
			return m_value;
		} else  {
			 throw ASErrors::USDUnavailableExImpl(__FILE__,__LINE__,"USDDevIO::read()");
		}
	
	}
	/**
	 * Used to send values to USD.
	*/ 
	template <class T> void USDDevIO<T>::write(const T& value,ACS::Time &timestamp)   throw (ASErrors::ASErrorsExImpl)
	{
		CORBA::Long par;
		ACS_TRACE("::USDDevIO::write()");
		
		if( m_wLen==0) {
			m_value=value;		// no hardware write for RO properties
			return;
		}
		
	  try  {
		  if (m_pusd->m_available) {
			
			  if(m_cmd==CPOS||m_cmd==RPOS) par=value<<USxS; // step to ustep position conversion
			  else if(m_cmd==FMIN || m_cmd==FMAX) par=long(value);	// velocity
			  else par=value;
			
			  CompletionImpl
              comp(m_pLan->sendUSDCmd(m_cmd,m_addr,par,m_wLen));
		
			  if(m_pusd->compCheck(comp))	throw ASErrors::USDErrorExImpl(comp,__FILE__,__LINE__,"USDDevIO::write()");  
			
			  timestamp=getTimeStamp();
			  m_value=value;
		  } else {
			  throw ASErrors::USDUnavailableExImpl(__FILE__,__LINE__,"USDDevIO::write()");
		  }
	  }
	  
	  catch (CORBA::SystemException& corbaEx) {
		ASErrors::corbaErrorExImpl exImpl(__FILE__,__LINE__,"USDDevIO::write()");
		_ADD_MEMBER(exImpl,m_addr);
		_ADD_MEMBER(exImpl,m_cmd);		
		throw exImpl;
	  }

	}
