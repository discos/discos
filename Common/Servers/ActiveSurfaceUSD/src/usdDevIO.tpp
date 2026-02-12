template <class T> T USDDevIO<T>::read(ACS::Time &timestamp) throw (ASErrors::ASErrorsExImpl)
{
    ACS_TRACE("::USDDevIO::read()");

    ACS::Time now = getTimeStamp();
    if(!m_pusd.m_available && now < m_pusd.m_next_retry_time)
    {
        if(m_cmd != STAT)
        {
            throw ASErrors::USDUnavailableExImpl(__FILE__, __LINE__, "USDDevIO::read()").getASErrorsEx();
        }
    }

    CORBA::Long value;

    if(m_rLen == 0)
    {
        return m_value;
    }

    try
    {
        m_pLan->recUSDPar(m_cmd, m_addr, m_rLen, value);
    }
    catch (ASErrors::ASErrorsEx& ex)
    {
        ASErrors::ASErrorsExImpl impl(ex);
        throw impl;
    }
    catch (CORBA::SystemException& corbaEx)
    {
        ASErrors::corbaErrorExImpl exImpl(__FILE__, __LINE__, "USDDevIO::read()");
        _ADD_MEMBER(exImpl, m_addr);
        _ADD_MEMBER(exImpl, m_cmd);
        throw exImpl;
    }

    timestamp = getTimeStamp();
    if(m_cmd == APOS)
    {
        value >>= USxS;                                     // ustep to step position conversion
    }
    else if(m_cmd == FMIN || m_cmd == FMAX)
    {
        value = long(value);                                // velocity
    }
    else if(m_cmd == STAT)
    {
        m_pusd.m_calibrate ? value |= CAL : value &=~ CAL;  // status CAL integration
        m_pusd.m_ploop ? value |= PAUT : value &=~ PAUT;    // status  PAUT integration
    }
    m_value = value;
    return m_value;
}

/**
 * Used to send values to USD.
*/
template <class T> void USDDevIO<T>::write(const T& value, ACS::Time &timestamp) throw (ASErrors::ASErrorsExImpl)
{
    CORBA::Long par;
    ACS_TRACE("::USDDevIO::write()");

    ACS::Time now = getTimeStamp();
    if(!m_pusd.m_available && now < m_pusd.m_next_retry_time)
    {
        throw ASErrors::USDUnavailableExImpl(__FILE__, __LINE__, "USDDevIO::read()").getASErrorsEx();
    }

    if(m_wLen == 0)
    {
        m_value = value;                                    // no hardware write for RO properties
        return;
    }

    try
    {
        if(m_cmd == CPOS || m_cmd == RPOS)
        {
            par = value << USxS;                            // step to ustep position conversion
        }
        else if(m_cmd == FMIN || m_cmd == FMAX)
        {
            par = long(value);                              // velocity
        }
        else
        {
            par = value;
        }

        CompletionImpl
        comp(m_pLan->sendUSDCmd(m_cmd, m_addr, par, m_wLen));

        if(m_pusd.compCheck(comp))
        {
            throw ASErrors::USDErrorExImpl(comp, __FILE__, __LINE__, "USDDevIO::write()");
        }

        timestamp = getTimeStamp();
        m_value = value;
    }
    catch (CORBA::SystemException& corbaEx)
    {
        ASErrors::corbaErrorExImpl exImpl(__FILE__, __LINE__, "USDDevIO::write()");
        _ADD_MEMBER(exImpl, m_addr);
        _ADD_MEMBER(exImpl, m_cmd);
        throw exImpl;
    }
}
