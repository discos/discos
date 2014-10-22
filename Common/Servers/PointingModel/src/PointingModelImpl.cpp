// $Id: PointingModelImpl.cpp,v 1.3 2008-02-01 11:24:13 c.migoni Exp $

#include <PointingModelImpl.h>
#include <ComponentErrors.h>
#include <Definitions.h>

using namespace IRA;
using namespace ComponentErrors;

static char *rcsId="@(#) $Id: PointingModelImpl.cpp,v 1.3 2008-02-01 11:24:13 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

PointingModelImpl::PointingModelImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices) : 
	ACSComponentImpl(CompName,containerServices)
{	
	AUTO_TRACE("PointingModelImpl::PointingModelImpl()");
	m_componentName = CString(CompName);
	m_receiverCode = "";
}

PointingModelImpl::~PointingModelImpl()
{
	AUTO_TRACE("PointingModelImpl::~PointingModelImpl()");
}

void PointingModelImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("PointingModelImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"PointingModelImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		m_receiverTable = (CDBTable *)new CDBTable(getContainerServices(),"Receiver","DataBlock/PointingModel");
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"PointingModelImpl::initialize()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO, "PointingModelImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void PointingModelImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("PointingModelImpl::execute()");
	ACS_LOG(LM_FULL_INFO,"PointingModelImpl::execute()",(LM_INFO,"PointingModel::READING_RECEIVER_CONFIGURATION"));
	CError error;
	error.Reset();
	if (!m_receiverTable->addField(error,"receiverCode",CDataField::STRING)) {
		error.setExtra("receiverCode field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"phi",CDataField::DOUBLE)) {
		error.setExtra("phi field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum00",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum00 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal00",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal00 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum01",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum01 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal01",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal01 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum02",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum02 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal02",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal02 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum03",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum03 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal03",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal03 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum04",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum04 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal04",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal04 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum05",CDataField::LONGLONG)) {
	error.setExtra("coefficientNum05 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal05",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal05 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum06",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum06 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal06",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal06 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum07",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum07 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal07",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal07 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum08",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum08 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal08",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal08 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum09",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum09 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal09",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal09 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum10",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum10 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal10",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal10 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum11",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum11 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal11",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal11 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum12",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum12 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal12",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal12 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum13",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum13 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal13",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal13 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum14",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum14 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal14",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal14 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum15",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum15 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal15",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal15 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum16",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum16 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal16",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal16 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum17",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum17 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal17",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal17 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum18",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum18 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal18",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal18 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum19",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum19 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal19",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal19 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum20",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum20 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal20",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal20 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum21",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum21 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal21",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal21 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum22",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum22 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal22",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal22 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum23",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum23 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal23",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal23 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum24",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum24 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal24",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal24 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum25",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum25 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal25",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal25 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientNum26",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum26 field not found", 0);
 	}
	else if (!m_receiverTable->addField(error,"coefficientVal26",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal26 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum27",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum27 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal27",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal27 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum28",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum28 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal28",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal28 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientNum29",CDataField::LONGLONG)) {
		error.setExtra("coefficientNum29 field not found", 0);
	}
	else if (!m_receiverTable->addField(error,"coefficientVal29",CDataField::DOUBLE)) {
		error.setExtra("coefficientVal29 field not found", 0);
	}
	if (!error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		throw dummy;
	}
	if (!m_receiverTable->openTable(error)) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
		throw dummy;
	}

	receiverActivated = false; // DA VERIFICARE SE VA BENE MESSO QUI !!!!!!!!!!!

	ACS_LOG(LM_FULL_INFO,"PointingModelImpl::execute()",(LM_INFO,"PointingModel::RECEIVER_TABLE_OPENED"));
	ACS_LOG(LM_FULL_INFO,"PointingModelImpl::execute()",(LM_INFO,"PointingModel::COMPSTATE_OPERATIONAL"));
}

void PointingModelImpl::cleanUp()
{
	AUTO_TRACE("PointingModelImpl::cleanUp()");
	ACSComponentImpl::cleanUp();
}

void PointingModelImpl::aboutToAbort()
{
	AUTO_TRACE("PointingModelImpl::aboutToAbort()");
}

void PointingModelImpl::getAzElOffsets (CORBA::Double azimuth, CORBA::Double elevation, CORBA::Double_out azOffset, CORBA::Double_out elOffset)  throw (CORBA::SystemException, AntennaErrors::AntennaErrorsEx)
{
	AUTO_TRACE("PointingModelImpl::getAzElOffsets()");
	if (receiverActivated) {
		getAzOff(azimuth, elevation, azOffset);
		getElOff(azimuth, elevation, elOffset);
	}
	else {
		azOffset = 0.0;
		elOffset = 0.0;
		_EXCPT(AntennaErrors::ReceiverNotFoundExImpl, __dummy,"PointingModelImpl::setReceiver()");
		__dummy.log(LM_DEBUG);
		throw __dummy.getAntennaErrorsEx();
	}
}

void PointingModelImpl::setReceiver (const char* receiverCode) throw (CORBA::SystemException, AntennaErrors::AntennaErrorsEx)
{
	AUTO_TRACE("PointingModelImpl::setReceiver()");
	bool done = false;

	m_receiverTable->First();
	CString name(receiverCode);
	for (int i = 0; i < m_receiverTable->recordCount(); i++) {
		CString rxCode((*m_receiverTable)["receiverCode"]->asString());
		if (name == rxCode) {
			done = true;
			break;
		}
		m_receiverTable->Next();
	}
	if (done) {
		PM.phi = (*m_receiverTable)["phi"]->asDouble();
		PM.coefficientNum[0] = (*m_receiverTable)["coefficientNum00"]->asLongLong();
		PM.coefficientVal[0] = (*m_receiverTable)["coefficientVal00"]->asDouble();
		PM.coefficientNum[1] = (*m_receiverTable)["coefficientNum01"]->asLongLong();
		PM.coefficientVal[1] = (*m_receiverTable)["coefficientVal01"]->asDouble();
		PM.coefficientNum[2] = (*m_receiverTable)["coefficientNum02"]->asLongLong();
		PM.coefficientVal[2] = (*m_receiverTable)["coefficientVal02"]->asDouble();
		PM.coefficientNum[3] = (*m_receiverTable)["coefficientNum03"]->asLongLong();
		PM.coefficientVal[3] = (*m_receiverTable)["coefficientVal03"]->asDouble();
		PM.coefficientNum[4] = (*m_receiverTable)["coefficientNum04"]->asLongLong();
		PM.coefficientVal[4] = (*m_receiverTable)["coefficientVal04"]->asDouble();
		PM.coefficientNum[5] = (*m_receiverTable)["coefficientNum05"]->asLongLong();
		PM.coefficientVal[5] = (*m_receiverTable)["coefficientVal05"]->asDouble();
		PM.coefficientNum[6] = (*m_receiverTable)["coefficientNum06"]->asLongLong();
		PM.coefficientVal[6] = (*m_receiverTable)["coefficientVal06"]->asDouble();
		PM.coefficientNum[7] = (*m_receiverTable)["coefficientNum07"]->asLongLong();
		PM.coefficientVal[7] = (*m_receiverTable)["coefficientVal07"]->asDouble();
		PM.coefficientNum[8] = (*m_receiverTable)["coefficientNum08"]->asLongLong();
		PM.coefficientVal[8] = (*m_receiverTable)["coefficientVal08"]->asDouble();
		PM.coefficientNum[9] = (*m_receiverTable)["coefficientNum09"]->asLongLong();
		PM.coefficientVal[9] = (*m_receiverTable)["coefficientVal09"]->asDouble();
		PM.coefficientNum[10] = (*m_receiverTable)["coefficientNum10"]->asLongLong();
		PM.coefficientVal[10] = (*m_receiverTable)["coefficientVal10"]->asDouble();
		PM.coefficientNum[11] = (*m_receiverTable)["coefficientNum11"]->asLongLong();
		PM.coefficientVal[11] = (*m_receiverTable)["coefficientVal11"]->asDouble();
		PM.coefficientNum[12] = (*m_receiverTable)["coefficientNum12"]->asLongLong();
		PM.coefficientVal[12] = (*m_receiverTable)["coefficientVal12"]->asDouble();
		PM.coefficientNum[13] = (*m_receiverTable)["coefficientNum13"]->asLongLong();
		PM.coefficientVal[13] = (*m_receiverTable)["coefficientVal13"]->asDouble();
		PM.coefficientNum[14] = (*m_receiverTable)["coefficientNum14"]->asLongLong();
		PM.coefficientVal[14] = (*m_receiverTable)["coefficientVal14"]->asDouble();
		PM.coefficientNum[15] = (*m_receiverTable)["coefficientNum15"]->asLongLong();
		PM.coefficientVal[15] = (*m_receiverTable)["coefficientVal15"]->asDouble();
		PM.coefficientNum[16] = (*m_receiverTable)["coefficientNum16"]->asLongLong();
		PM.coefficientVal[16] = (*m_receiverTable)["coefficientVal16"]->asDouble();
		PM.coefficientNum[17] = (*m_receiverTable)["coefficientNum17"]->asLongLong();
		PM.coefficientVal[17] = (*m_receiverTable)["coefficientVal17"]->asDouble();
		PM.coefficientNum[18] = (*m_receiverTable)["coefficientNum18"]->asLongLong();
		PM.coefficientVal[18] = (*m_receiverTable)["coefficientVal18"]->asDouble();
		PM.coefficientNum[19] = (*m_receiverTable)["coefficientNum19"]->asLongLong();
		PM.coefficientVal[19] = (*m_receiverTable)["coefficientVal19"]->asDouble();
		PM.coefficientNum[20] = (*m_receiverTable)["coefficientNum20"]->asLongLong();
		PM.coefficientVal[20] = (*m_receiverTable)["coefficientVal20"]->asDouble();
		PM.coefficientNum[21] = (*m_receiverTable)["coefficientNum21"]->asLongLong();
		PM.coefficientVal[21] = (*m_receiverTable)["coefficientVal21"]->asDouble();
		PM.coefficientNum[22] = (*m_receiverTable)["coefficientNum22"]->asLongLong();
		PM.coefficientVal[22] = (*m_receiverTable)["coefficientVal22"]->asDouble();
		PM.coefficientNum[23] = (*m_receiverTable)["coefficientNum23"]->asLongLong();
		PM.coefficientVal[23] = (*m_receiverTable)["coefficientVal23"]->asDouble();
		PM.coefficientNum[24] = (*m_receiverTable)["coefficientNum24"]->asLongLong();
		PM.coefficientVal[24] = (*m_receiverTable)["coefficientVal24"]->asDouble();
		PM.coefficientNum[25] = (*m_receiverTable)["coefficientNum25"]->asLongLong();
		PM.coefficientVal[25] = (*m_receiverTable)["coefficientVal25"]->asDouble();
		PM.coefficientNum[26] = (*m_receiverTable)["coefficientNum26"]->asLongLong();
		PM.coefficientVal[26] = (*m_receiverTable)["coefficientVal26"]->asDouble();
		PM.coefficientNum[27] = (*m_receiverTable)["coefficientNum27"]->asLongLong();
		PM.coefficientVal[27] = (*m_receiverTable)["coefficientVal27"]->asDouble();
		PM.coefficientNum[28] = (*m_receiverTable)["coefficientNum28"]->asLongLong();
		PM.coefficientVal[28] = (*m_receiverTable)["coefficientVal28"]->asDouble();
		PM.coefficientNum[29] = (*m_receiverTable)["coefficientNum29"]->asLongLong();
		PM.coefficientVal[29] = (*m_receiverTable)["coefficientVal29"]->asDouble();

		receiverActivated = true;
	}
	else {
		receiverActivated = false;
		_EXCPT(AntennaErrors::ReceiverNotFoundExImpl, __dummy,"PointingModelImpl::setReceiver()");
		__dummy.log(LM_DEBUG);
		throw __dummy.getAntennaErrorsEx();
	}
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(PointingModelImpl)

/*___oOo___*/
