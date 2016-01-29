#include "ExternalClientsSocketServer.h"

#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	CString tmps; \
	if (!CIRATools::getDBValue(m_services,ATTRIB,tmps)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CRefractionCore::initialize()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmps; \
		ACS_DEBUG_PARAM("CRefractionCore::initialize()",DESCR" %s",(const char*)tmps); \
	} \
}

using namespace ComponentErrors;
using namespace ManagementErrors;

CExternalClientsSocketServer::CExternalClientsSocketServer(ContainerServices *service) : CSocket(), 
	m_services(service)
{
	AUTO_TRACE("CExternalClientsSocketServer::CExternalClientsSocketServer()");
	setExternalClientSocketStatus(ExternalClientSocketStatus_NOTCNTD);
    m_byebye = false;
    m_accept = false;
}

CExternalClientsSocketServer::~CExternalClientsSocketServer()
{
	AUTO_TRACE("CExternalClientsSocketServer::~CExternalClientsSocketServer()");
	setExternalClientSocketStatus(ExternalClientSocketStatus_NOTCNTD);
	Close(m_Error);
}

void CExternalClientsSocketServer::initialize(CConfiguration *config) throw (SocketErrorExImpl)
{
	AUTO_TRACE("CExternalClientsSocketServer::Init()");
	m_configuration = config;
	// Create server socket in blocking mode
	CString ipAdd;
	ipAdd = m_configuration->ipAddress();
	if (Create(m_Error,STREAM,m_configuration->Port(),&ipAdd) == FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CExternalClientsSocketServer::Init()");
	}
    // Listen
	if (Listen(m_Error) == FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CExternalClientsSocketServer::Init()");
	}	
	else {
		setExternalClientSocketStatus(ExternalClientSocketStatus_CNTD);
	}
	// TO BE VERIFIED
	int Val=1000;
	if (setSockOption(m_Error,SO_RCVBUF,&Val,sizeof(int)) == FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CExternalClientsSocketServer::Init()");
	}
}

void CExternalClientsSocketServer::execute() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl)
{
	if (getStatus() == ExternalClientSocketStatus_CNTD) {
		m_Scheduler = Management::Scheduler::_nil();
		CString sVisor = m_configuration->superVisor();
		if (sVisor.Compare("Gavino") == 0)
			strcpy (superVisorName, GAVINO);
		if (sVisor.Compare("Palmiro") == 0)
			strcpy (superVisorName, PALMIRO);
		if (sVisor.Compare("Ducezio") == 0)
			strcpy (superVisorName, DUCEZIO);
		try {
			m_Scheduler = m_services->getComponent<Management::Scheduler>(superVisorName);
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CExternalClientsSocketServer::execute()");
			Impl.setComponentName(superVisorName);
			throw Impl;
		}
		ACS_LOG(LM_FULL_INFO,"CExternalClientsSocketServer::execute()",(LM_INFO,"CExternalClientsSocketServer::SCHEDULER_LOCATED"));
		m_antennaBoss = Antenna::AntennaBoss::_nil();
		try {
			m_antennaBoss = m_services->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CExternalClientsSocketServer::execute()");
			Impl.setComponentName("ANTENNA/Boss");
			throw Impl;
		}
		m_receiversBoss = Receivers::ReceiversBoss::_nil();
		try {
			m_receiversBoss = m_services->getComponent<Receivers::ReceiversBoss>("RECEIVERS/Boss");
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CExternalClientsSocketServer::execute()");
			Impl.setComponentName("RECEIVERS/Boss");
			throw Impl;
		}
        	m_mount = Antenna::Mount::_nil();
		try {
			m_mount = m_services->getComponent<Antenna::Mount>("ANTENNA/Mount");
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CExternalClientsSocketServer::execute()");
			Impl.setComponentName("ANTENNA/Mount");
			throw Impl;
		}
	}
}

void CExternalClientsSocketServer::cleanUp()
{
	try {
		m_services->releaseComponent((const char*)m_Scheduler->name());
	}
	catch (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CExternalClientsSocketServer::cleanUp()");
		Impl.setComponentName((const char *)m_Scheduler->name());
		Impl.log(LM_DEBUG);
	}
	try {
		m_services->releaseComponent((const char*)m_antennaBoss->name());
	}
	catch (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CExternalClientsSocketServer::cleanUp()");
		Impl.setComponentName((const char *)m_antennaBoss->name());
		Impl.log(LM_DEBUG);
	}
	try {
		m_services->releaseComponent((const char*)m_receiversBoss->name());
	}
	catch (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CExternalClientsSocketServer::cleanUp()");
		Impl.setComponentName((const char *)m_receiversBoss->name());
		Impl.log(LM_DEBUG);
	}
	try {
		m_services->releaseComponent((const char*)m_mount->name());
	}
	catch (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CExternalClientsSocketServer::cleanUp()");
		Impl.setComponentName((const char *)m_mount->name());
		Impl.log(LM_DEBUG);
	}
	newExternalClientsSocketServer.Close(m_Error);
       	m_accept = false;
}

void CExternalClientsSocketServer::cmdToScheduler()
{
	BYTE inBuffer[BUFFERSIZE];
    	BYTE outBuffer[BUFFERSIZE];
    	char * ret_val;
    	IRA::CString out;
	int rBytes;
    	WORD Len;
    	OperationResult Res;
	bool ans = false;

	if (m_accept == false) {
        // Accept
    		OperationResult Res;
    		Res = Accept(m_Error, newExternalClientsSocketServer);
    		if (Res == FAIL) {
			_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
			dummy.setCode(m_Error.getErrorCode());
			dummy.setDescription((const char*)m_Error.getDescription());
			m_Error.Reset();
        		_ADD_BACKTRACE(SocketErrorExImpl,_dummy,dummy,"CExternalClientsSocketServer::cmdToScheduler()");
        		_dummy.log(LM_ERROR);
		}
    		if (Res == WOULDBLOCK)
        		setSockMode(m_Error,BLOCKING);
    	}
    	m_accept = true;

    	rBytes = receiveBuffer(inBuffer,BUFFERSIZE);

	if (rBytes > 0 ) {
		inBuffer[rBytes] = '\0';
		//printf("Command received = %s\n", inBuffer);
		if (strncmp ((const char*)inBuffer, "antennaParameters",18)== 0) {
			TIMEVALUE now;
         		const char answerDelimiter='\\';
            		const char fieldDelimiter=',';
            		IRA::CString m_answerDelimiter;
            		IRA::CString m_fieldDelimiter;
            		ACSErr::Completion_var completion;
            		Management::ROTSystemStatus_var antennaBossStatus_var;
            		ACS::pattern antennaBossStatus_val;
            		Management::ROTBoolean_var tracking_var;
            		bool tracking_val;
            		double ra, dec;
            		double az, el;
            		double lon, lat;
            		IRA::CString azStr;
            		IRA::CString elStr;
            		IRA::CString azCommStr;
            		IRA::CString elCommStr;
            		IRA::CString raStr;
            		IRA::CString decStr;
            		IRA::CString lonStr;
            		IRA::CString latStr;
            		IRA::CString trackingStr;
            		char * status_val;
            		ACS::ROstring_var targetRef;
            		CORBA::String_var target;
            		ACS::ROstring_var receiverRef;
            		CORBA::String_var receiver;
			ACS::doubleSeq_var lo_var;
			ACS::ROdoubleSeq_var loRef;
			double lo[2];
            		ACS::ROdouble_var azErr_var, elErr_var;
            		double azErr, elErr;
            		ACS::ROdouble_var pointingAzCorr_var, pointingElCorr_var, refractionCorr_var;
            		double pointingAzCorr, pointingElCorr, refractionCorr;
            		double azOff, elOff, raOff, decOff, lonOff, latOff;
			double azComm, elComm;

			IRA::CIRATools::getTime(now);
            		ret_val = "antennaParameters";
            		out = IRA::CString (ret_val);
            		m_answerDelimiter=IRA::CString(answerDelimiter);
            		out+=m_answerDelimiter;
            		IRA::CString outTime;
            		IRA::CIRATools::timeToStr(now.value().value,outTime);
            		out.Concat(outTime);
            		m_fieldDelimiter=IRA::CString(fieldDelimiter);
            		out+=m_fieldDelimiter;
                    	try {
                        	antennaBossStatus_var = m_antennaBoss->status();
                        	antennaBossStatus_val = antennaBossStatus_var->get_sync(completion.out());
			        if (antennaBossStatus_val == Management::MNG_OK)
					status_val = "OK";
			        else if (antennaBossStatus_val == Management::MNG_WARNING)
					status_val = "WARNING";
			        else if (antennaBossStatus_val == Management::MNG_FAILURE)
					status_val = "FAILURE";
			        else
					status_val = "UNKNOWN";
                    	}
                    	catch (...) {
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
		    		impl.log(LM_ERROR);
            			status_val = "NA";
	    		}
			out.Concat(status_val);
			out+=m_fieldDelimiter;
            		try {
                		targetRef = m_antennaBoss->target ();
                		target = targetRef->get_sync (completion.out ());
            		}
            		catch (...) {
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
		    		impl.log(LM_ERROR);
            			target = "NA";
	    		}
			out.Concat(target);
			out+=m_fieldDelimiter;
            		try {
                		m_antennaBoss->getObservedHorizontal(now.value().value,1,az,el);
                m_antennaBoss->getObservedEquatorial(now.value().value,1,ra,dec);
                m_antennaBoss->getObservedGalactic(now.value().value,1,lon,lat);
                IRA::CIRATools::radToAngle(az,azStr);
                IRA::CIRATools::radToAngle(el,elStr);
                IRA::CIRATools::radToHourAngle(ra,raStr);
                IRA::CIRATools::radToSexagesimalAngle(dec,decStr);
                IRA::CIRATools::radToSexagesimalAngle(lon,lonStr);
                IRA::CIRATools::radToSexagesimalAngle(lat,latStr);
		m_antennaBoss->getRawCoordinates(now.value().value,azComm,elComm);
                IRA::CIRATools::radToAngle(azComm,azCommStr);
                IRA::CIRATools::radToAngle(elComm,elCommStr);
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                azStr = "NA";
                elStr = "NA";
                raStr = "NA";
                decStr = "NA";
                lonStr = "NA";
                latStr = "NA";
                azCommStr = "NA";
                elCommStr = "NA";
            }
			out.Concat(azStr);
			out+=m_fieldDelimiter;
			out.Concat(elStr);
			out+=m_fieldDelimiter;
			out.Concat(raStr);
			out+=m_fieldDelimiter;
			out.Concat(decStr);
			out+=m_fieldDelimiter;
			out.Concat(lonStr);
			out+=m_fieldDelimiter;
			out.Concat(latStr);
			out+=m_fieldDelimiter;
			out.Concat(azCommStr);
			out+=m_fieldDelimiter;
			out.Concat(elCommStr);
			out+=m_fieldDelimiter;
            try {
                azErr_var = m_mount->azimuthError();
                azErr = azErr_var->get_sync( completion.out());
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                azErr = 0.0;
            }
            out.Concat(azErr);
			out+=m_fieldDelimiter;
            try {
                elErr_var = m_mount->elevationError();
                elErr = elErr_var->get_sync( completion.out());
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                elErr = 0.0;
            }
            out.Concat(elErr);
			out+=m_fieldDelimiter;
            try {
                pointingAzCorr_var = m_antennaBoss->pointingAzimuthCorrection();
                pointingAzCorr = pointingAzCorr_var->get_sync( completion.out());
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                pointingAzCorr = 0.0;
            }
            out.Concat(pointingAzCorr);
			out+=m_fieldDelimiter;
            try {
                pointingElCorr_var = m_antennaBoss->pointingElevationCorrection();
                pointingElCorr = pointingElCorr_var->get_sync( completion.out());
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                pointingElCorr = 0.0;
            }
            out.Concat(pointingElCorr);
			out+=m_fieldDelimiter;
            try {
                refractionCorr_var = m_antennaBoss->refractionCorrection();
                refractionCorr = refractionCorr_var->get_sync( completion.out());
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                refractionCorr = 0.0;
            }
            out.Concat(refractionCorr);
			out+=m_fieldDelimiter;
            try {
                m_antennaBoss->getAllOffsets(azOff,elOff,raOff,decOff,lonOff,latOff);
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                azOff = 0.0;
                elOff = 0.0;
                raOff = 0.0;
                decOff = 0.0;
                lonOff = 0.0;
                latOff = 0.0;
            }
			out.Concat(azOff);
			out+=m_fieldDelimiter;
			out.Concat(elOff);
			out+=m_fieldDelimiter;
			out.Concat(raOff);
			out+=m_fieldDelimiter;
			out.Concat(decOff);
			out+=m_fieldDelimiter;
			out.Concat(lonOff);
			out+=m_fieldDelimiter;
			out.Concat(latOff);
			out+=m_fieldDelimiter;
            try {
                receiverRef = m_receiversBoss->actualSetup ();
                receiver = receiverRef->get_sync (completion.out ());
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                receiver = "NA";
            }
            out.Concat(receiver);
			out+=m_fieldDelimiter;
            try {
                loRef = m_receiversBoss->LO();
                lo_var = loRef->get_sync(completion.out());
                lo[0] = lo_var[0];
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                lo[0] = 0.0;
            }
			out.Concat(lo[0]);
			out+=m_fieldDelimiter;
            try {
                tracking_var = m_Scheduler->tracking();
                tracking_val = tracking_var->get_sync(completion.out());
                if (tracking_val == Management::MNG_TRUE) {
                    trackingStr = "TRACKING";
                }
                else {
                    trackingStr = "SLEWING";
                }
            }
            catch (...) {
                _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                impl.log(LM_ERROR);
                trackingStr = "NA";
            }
			out.Concat(trackingStr);
		}
		else {
			try {
				ans = m_Scheduler->command((const char*)inBuffer, ret_val);
                		out = IRA::CString(ret_val);
        		}
        		catch (CORBA::SystemException& ex) {
                		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
                		impl.setName(ex._name());
		    		impl.setMinor(ex.minor());
		    		impl.log(LM_ERROR);
            			ret_val = "CORBA Error";
                		out = IRA::CString(ret_val);
        		}
        		catch (...) {
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
		    		impl.log(LM_ERROR);
            			ret_val = "Unexpected Error";
                		out = IRA::CString(ret_val);
	    		}
			    if (ans == false) {
				    //ret_val = "Nuraghe Error"; // TBD!!
                	out = IRA::CString(ret_val);
			    }
        	}
        	Len = out.GetLength();
        	int i;
        	for (i = 0; i < Len; i++) {
            		outBuffer[i] = out.CharAt(i);
       		}
        	outBuffer[Len]='\n';
        	//printf("Command returned = %s", outBuffer);
        	Res = sendBuffer(outBuffer,Len+1);
        	if (Res == WOULDBLOCK || Res == FAIL) {
            		_EXCPT(SocketErrorExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
         		impl.log(LM_ERROR);
        	}
     	}
     	else { 
     		newExternalClientsSocketServer.Close(m_Error);
        	m_accept = false;
    	}
}

// private methods

CExternalClientsSocketServer::OperationResult CExternalClientsSocketServer::sendBuffer(BYTE *Msg,WORD Len)
{
	int NWrite;
	int BytesSent;
	BytesSent=0;
	while (BytesSent<Len) {
		if ((NWrite = newExternalClientsSocketServer.Send(m_Error,(const void *)(Msg+BytesSent),Len-BytesSent))<0) {
			if (NWrite==WOULDBLOCK) {
				setExternalClientSocketStatus(ExternalClientSocketStatus_NOTCNTD);
				return WOULDBLOCK;
			}
			else {
				setExternalClientSocketStatus(ExternalClientSocketStatus_NOTCNTD);
				return FAIL;
			}
		}
		else { // success
			BytesSent+=NWrite;
		}
	}
	if (BytesSent==Len) {
		return SUCCESS;
   }
	else {
		_SET_ERROR(m_Error,CError::SocketType,CError::SendError,"CExternalClientsSocketServer::SendBuffer()");
		return FAIL;
	}
}

int CExternalClientsSocketServer::receiveBuffer(BYTE *Msg,WORD Len)
{
	int nRead;
	TIMEVALUE Now;
	TIMEVALUE Start;
	CIRATools::getTime(Start);
	while (true) {
		nRead = newExternalClientsSocketServer.Receive(m_Error,(void *)Msg,Len);
		if (nRead == WOULDBLOCK) {
			CIRATools::getTime(Now);
			if (CIRATools::timeDifference(Start,Now)>m_configuration->receiveTimeout()) {
				return WOULDBLOCK;
			}
			else {
				CIRATools::Wait(0,5000);
				continue;
			}
		}
		else if (nRead == FAIL) { 
			return nRead;
		}
		else if (nRead == 0) {
			return nRead;
		}
		else
            return nRead;
	}
}

void CExternalClientsSocketServer::setExternalClientSocketStatus(ExternalClientSocketStatus status)
{ 
	m_Status = status;
	if (m_Status == ExternalClientSocketStatus_BSY) m_bBusy=true;
	else if (m_Status == ExternalClientSocketStatus_CNTD) m_bBusy=false;
}

bool CExternalClientsSocketServer::isBusy() 
{
	return (m_bBusy);
}

CExternalClientsSocketServer::ExternalClientSocketStatus CExternalClientsSocketServer:: getStatus() const
{
    return m_Status;
}
