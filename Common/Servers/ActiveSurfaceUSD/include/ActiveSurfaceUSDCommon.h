#ifndef USDCommon_h
#define USDCommon_h

#include <IRA>
#include <ASErrors.h>
#include <ComponentErrors.h>
//CORBA stubs
#include <usdS.h>
#include <lanS.h>
#include <ActiveSurfaceCommonS.h>
#include "ActiveSurfaceProxy.h"

/// ustep to step exp. 2 factor (2^7=128)
#define USxS    7

// mask pattern for status
#define UNAV    0xFF000000
#define MRUN    0x000080
#define CAMM    0x000100
#define ENBL    0x002000
#define PAUT    0x000800
#define CAL     0x008000

// USD driver command
#define RESET   0x01
#define STOP    0x11
#define FMIN    0x20
#define FMAX    0x21
#define ACCL    0x22
#define LCNTR   0x23
#define UBIT    0x25
#define RESL    0x26
#define DELAY   0x28
#define HSTOP   0x2A
#define APOS    0x12
#define STAT    0x13
#define SVER    0x10
#define USDT    0x14
#define CPOS    0x30
#define RPOS    0x31
#define GO      0x32

#define ACK     0x06

#define MM2STEP 1400 //(42000 STEP / 30 MM)

#define MAX_FAILURES 5
#define MIN_BACKOFF ACS::TimeInterval(100000000) // 10 seconds
#define MAX_BACKOFF ACS::TimeInterval(600000000) // 1 minute

// specific macros

/** @#define _ADD_MEMBER(OBJ, MEMB)
 *  This macro add extra data to an error/compl. object. MEMB must be real variable or constant
 *  @param OBJ error or completion object
 *  @param MEMB costant or variable added as (name,value) pair
 */
#define _ADD_MEMBER(OBJ, MEMB)\
{\
    CString mName(#MEMB);\
    CString mVal;\
    mVal.Format("%02x", MEMB);\
    _ADD_EXTRA(OBJ, mName, mVal);\
}

/** @#define _THROW_EX(ERR, ROUTINE, VAR)
 *  Create and launch a remote (CORBA) exception, adding USD address and VAR as addedd data
 *  @param ERR error
 *  @param ROUTINE the calling routine
 *  @param VAR costant or variable added as (name,value) pair
 */
#define _THROW_EX(ERR, ROUTINE, VAR)\
{\
    ASErrors::ERR##ExImpl exImpl(__FILE__, __LINE__, ROUTINE);\
    _ADD_MEMBER(exImpl, m_addr);\
    _ADD_MEMBER(exImpl, VAR);\
    throw exImpl.getASErrorsEx();\
}

/** @#define _VAR_CHK_EXIMPL(VAR, ERR, ROUTINE)
 *  Launch a local exception if VAR is not true.
 *  @param VAR variable to be checked
 *  @param ERR error
 *  @param ROUTINE the calling routine
 */
#define _VAR_CHK_EXIMPL(VAR, ERR, ROUTINE)\
{\
    if(!VAR)\
    {\
        throw ASErrors::ERR##ExImpl(__FILE__, __LINE__, ROUTINE);\
    }\
}

/** @#define _VAR_CHK_EX(VAR, ERR, ROUTINE)
 *  Launch a remote(CORBA) exception if VAR is not true.
 *  @param VAR variable to be checked
 *  @param ERR error
 *  @param ROUTINE the calling routine
 */
#define _VAR_CHK_EX(VAR, ERR, ROUTINE) \
{\
    if(!VAR)\
    {\
        throw ASErrors::ERR##ExImpl(__FILE__, __LINE__, ROUTINE).getASErrorsEx();\
    }\
}

/** @#define _COMPL_CHK_THROW_EX(CP_SP, OUT_TYPE, ROUTINE)
 *  Check a remote(CORBA) completion_var. Launch an remotem OUT_TYPE CORBA exception
 *  if it is not error free.
 *  @param CP_SP Complition_var smart pointer
 *  @param OUT_TYPE out type exception
 *  @param ROUTINE the calling routine
 */
#define _COMPL_CHK_THROW_EX(CP_SP, OUT_TYPE, ROUTINE)\
{\
    ACSErr::CompletionImpl comp(CP_SP.in());\
    if(compCheck(comp))\
    {\
        throw ASErrors::OUT_TYPE##ExImpl(comp, __FILE__, __LINE__, ROUTINE).getASErrorsEx();\
    }\
}

/** @#define _SET_CDB(PROP, LVAL, ROUTINE) throw (ASErrors::CDBAccessErrorExImpl)
 *  Set a long CDB property or attribute.
 *  @param PROP property to be setted
 *  @param LVAL the value
 *  @param ROUTINE the calling routine
 */
#define _SET_CDB(PROP, LVAL, ROUTINE)\
{\
    maci::ContainerServices* cs = getContainerServices();\
    if(!CIRATools::setDBValue(cs, #PROP, (const long&)LVAL))\
    {\
        ASErrors::CDBAccessErrorExImpl exImpl(__FILE__, __LINE__, ROUTINE);\
        exImpl.setFieldName(#PROP);\
        throw exImpl;\
    }\
}

/** @#define _SET_CDB_D(PROP, LVAL, ROUTINE) throw (ASErrors::CDBAccessErrorExImpl)
 *  Set a double CDB property or attribute.
 *  @param PROP property to be setted
 *  @param LVAL the value
 *  @param ROUTINE the calling routine
 */
#define _SET_CDB_D(PROP, LVAL, ROUTINE)\
{\
    maci::ContainerServices* cs = getContainerServices();\
    if(!CIRATools::setDBValue(cs, #PROP, (const double&)LVAL))\
    {\
        ASErrors::CDBAccessErrorExImpl exImpl(__FILE__, __LINE__, ROUTINE);\
        exImpl.setFieldName(#PROP);\
        throw exImpl;\
    }\
}

/** @#define _GET_PROP(PROP, RETVAR, ROUTINE)
 *  Get a property value.
 *  @param PROP property to be read
 *  @param RETVAR the value
 *  @param ROUTINE the calling routine
 *  @throw a DevIOError remote exception if return completion is not error free
 */
#define _GET_PROP(PROP, RETVAR, ROUTINE)\
{\
    ACS::Time now = getTimeStamp();\
    if(!m_available && now < m_next_retry_time)\
    {\
        throw ASErrors::USDUnavailableExImpl(__FILE__, __LINE__, ROUTINE).getASErrorsEx();\
    }\
    ACS::Time timestamp;\
    try\
    {\
        RETVAR = m_##PROP##_sp->getDevIO()->read(timestamp);\
        if(!m_available && m_failures == MAX_FAILURES)\
        {\
            ACS_LOG(LM_FULL_INFO, ROUTINE, (LM_NOTICE, "USD available again."));\
            m_available = true;\
            m_usdStatus.available = true;\
            m_lanStatus->write(m_usdStatus);\
            m_backoff_time = MIN_BACKOFF;\
        }\
        m_failures = 0;\
    }\
    catch(ASErrors::ASErrorsExImpl& exImpl)\
    {\
        exImplCheck(exImpl, ROUTINE);\
        if(!m_available)\
        {\
            m_next_retry_time = getTimeStamp() + m_backoff_time;\
            m_backoff_time = std::min(m_backoff_time * 2, MAX_BACKOFF);\
        }\
        ASErrors::DevIOErrorExImpl ex(exImpl, __FILE__, __LINE__, ROUTINE);\
        throw ex.getDevIOErrorEx();\
    }\
}

/** @#define _SET_PROP(PROP, RETVAR, ROUTINE)
 *  Set a property value.
 *  @param PROP property to be read
 *  @param VALUE property value to be changed
 *  @param ROUTINE the calling routine
 *  @throw a DevIOError remote exception if return completion is not error free
 */
#define _SET_PROP(PROP, VALUE, ROUTINE)\
{\
    ACS::Time timestamp;\
    try\
    {\
        m_##PROP##_sp->getDevIO()->write(VALUE, timestamp);\
        m_failures = 0;\
    }\
    catch(ASErrors::ASErrorsExImpl& exImpl)\
    {\
        exImplCheck(exImpl, ROUTINE);\
        if(!m_available)\
        {\
            m_next_retry_time = getTimeStamp() + m_backoff_time;\
            m_backoff_time = std::min(m_backoff_time * 2, MAX_BACKOFF);\
        }\
        ASErrors::DevIOErrorExImpl ex(exImpl, __FILE__, __LINE__, ROUTINE);\
        throw ex.getDevIOErrorEx();\
    }\
}

/** @#define _SET_LDEF(PROP, ROUTINE)
 *  Set the default value for given property. All type except double.
 *  Launch a remote exception if set_sync return completion is not error free.
 *  @param PROP property
 *  @param ROUTINE the calling routine
 */
#define _SET_LDEF(PROP, ROUTINE)\
{\
    long tmpd;\
    ACSErr::CompletionImpl comp;\
    maci::ContainerServices* cs = getContainerServices();\
    if (!CIRATools::getDBValue(cs, #PROP"/default_value", tmpd)) \
    {\
        ASErrors::CDBAccessErrorExImpl ex = ASErrors::CDBAccessErrorExImpl(__FILE__, __LINE__, ROUTINE);\
        ex.setFieldName(#PROP"/default_value");\
        throw ex.getASErrorsEx();\
    }\
    else\
    {\
        comp = m_##PROP##_sp->set_sync(tmpd);\
    }\
    if (compCheck(comp))\
    {\
        throw ASErrors::USDErrorExImpl(__FILE__, __LINE__, ROUTINE).getASErrorsEx();\
    }\
}

/** @#define _CATCH_ACS_EXCP_THROW_EXIMPL(IN_TYPE,OUT_TYPE,ROUTINE,VAR)
 *  Catch an ACS(with error trace) exception and launch a local one (ExImpl).
 *  @param IN_TYPE the exception to catch
 *  @param OUT_TYPE the exception to be launched
 *  @param VAR variable name and value added as addiotional data
 *  @param ROUTINE the calling routine
 */
#define _CATCH_ACS_EXCP_THROW_EXIMPL(IN_TYPE, OUT_TYPE, ROUTINE,VAR)\
catch(IN_TYPE& Ex)\
{\
    OUT_TYPE exImpl(Ex, __FILE__, __LINE__, ROUTINE);\
    _ADD_MEMBER(exImpl, m_addr);\
    _ADD_MEMBER(exImpl, VAR);\
    throw exImpl;\
}

/** @#define _CATCH_EXCP_THROW_EXIMPL(IN_TYPE,OUT_TYPE,ROUTINE,VAR)
 *  Catch an CORBA(w/o error trace) exception and launch a local one (ExImpl).
 *  @param IN_TYPE the exception to catch
 *  @param OUT_TYPE the exception to be launched
 *  @param VAR variable name and value added as addiotional data
 *  @param ROUTINE the calling routine
 */
#define _CATCH_EXCP_THROW_EXIMPL(IN_TYPE, OUT_TYPE, ROUTINE, VAR)\
catch(IN_TYPE& Ex)\
{\
    OUT_TYPE exImpl(__FILE__, __LINE__, ROUTINE);\
    _ADD_MEMBER(exImpl, m_addr);\
    _ADD_MEMBER(exImpl, VAR);\
    throw exImpl;\
}

/** @#define _CATCH_ACS_EXCP_THROW_EX(IN_TYPE,OUT_TYPE,ROUTINE,VAR)
 *  Catch an ACS(with error trace) exception and launch a remote one (Ex).
 *  @param IN_TYPE the exception to catch
 *  @param OUT_TYPE the exception to be launched
 *  @param VAR variable name and value added as additional data
 *  @param ROUTINE the calling routine
 */
#define _CATCH_ACS_EXCP_THROW_EX(IN_TYPE, _ERR, ROUTINE, VAR)\
catch(IN_TYPE& Ex)\
{\
    ASErrors::_ERR##ExImpl exImpl(Ex, __FILE__, __LINE__, ROUTINE);\
    _ADD_MEMBER(exImpl, m_addr);\
    _ADD_MEMBER(exImpl, VAR);\
    throw exImpl.getASErrorsEx();\
}

/** @#define _CATCH_EXCP_THROW_EX(IN_TYPE,OUT_TYPE,ROUTINE,VAR)
 *  Catch an CORBA(w/o error trace) exception and launch a remote one (Ex).
 *  @param IN_TYPE the exception to catch
 *  @param OUT_TYPE the exception to be launched
 *  @param VAR variable name and value added as addiotional data
 *  @param ROUTINE the calling routine
 */
#define _CATCH_EXCP_THROW_EX(IN_TYPE, _ERR, ROUTINE, VAR)\
catch(IN_TYPE& Ex)\
{\
    ASErrors::_ERR##ExImpl exImpl(__FILE__, __LINE__, ROUTINE);\
    _ADD_MEMBER(exImpl, m_addr);\
    _ADD_MEMBER(exImpl, VAR);\
    throw exImpl.getASErrorsEx();\
}

#endif /* USDCommon_h */
