// $Id: ObservatoryImpl.cpp,v 1.6 2011-06-21 16:39:05 a.orlati Exp $

#include "ObservatoryImpl.h"
#include "ObservatoryDevIOs.h"

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr ObservatoryImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}

#define _GET_DOUBLE_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
	double tmpd; \
	if (!IRA::CIRATools::getDBValue(getContainerServices(),ATTRIB,tmpd,"alma/",NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"ObservatoryImpl::initialize()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpd; \
		ACS_DEBUG_PARAM("OTFImpl::initialize()",DESCR" %lf",tmpd); \
	} \
}

#define _GET_LONG_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
	long tmpd; \
	if (!IRA::CIRATools::getDBValue(getContainerServices(),ATTRIB,tmpd,"alma/",NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"ObservatoryImpl::initialize()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpd; \
		ACS_DEBUG_PARAM("OTFImpl::initialize()",DESCR" %lf",tmpd); \
	} \
}

#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
	IRA::CString tmps; \
	if (!IRA::CIRATools::getDBValue(getContainerServices(),ATTRIB,tmps,"alma/",NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"ObservatoryImpl::initialize()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmps; \
		ACS_DEBUG_PARAM("OTFImpl::initialize()",DESCR" %s",(const char *)tmps); \
	} \
}

using namespace ComponentErrors;
using namespace IRA;

static char *rcsId="@(#) $Id: ObservatoryImpl.cpp,v 1.6 2011-06-21 16:39:05 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

ObservatoryImpl::ObservatoryImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_observatoryName(this),
  	m_puniversalTime(this),
	m_pjulianDay(this),
	m_pGAST(this),
	m_pLST(this),
	m_pDUT1(this),
	m_platitude(this),m_plongitude(this),m_pheight(this),
	m_pxGeod(this),m_pyGeod(this),m_pzGeod(this),
	m_pyPolarMotion(this),m_pxPolarMotion(this),
	m_pgeodeticModel(this),
	m_data(NULL)
{	
	AUTO_TRACE("ObservatoryImpl::ObservatoryImpl()");
}

ObservatoryImpl::~ObservatoryImpl()
{
	AUTO_TRACE("ObservatoryImpl::~ObservatoryImpl()");
}

void ObservatoryImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	double dut1,longitude,latitude,height,yPolar,xPolar;
	IRA::CString obsName;
	long model;
	ACS::Time timeStamp;
	AUTO_TRACE("ObservatoryImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::initialize()",(LM_INFO,"ObservatoryImpl::COMPSTATE_INITIALIZING"));
	try {
		m_data=new CSecureArea<T_DevDataBlock>(new T_DevDataBlock);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"ObservatoryImpl::initialize()");
		throw dummy;
	}
	try {		 
		m_pDUT1=new ROdouble(getContainerServices()->getName()+":DUT1",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::DUT1,static_cast<void *>(m_data)),true);
		
		m_observatoryName=new ROstring(getContainerServices()->getName()+":observatoryName",getComponent());
		
		m_puniversalTime=new ROuLongLong(getContainerServices()->getName()+":universalTime",getComponent(),
		  new ObsDevIO<CORBA::ULongLong>(ObsDevIO<CORBA::ULongLong>::UNIVERSALTIME,static_cast<void *>(m_data)),true);
		  
		m_pjulianDay=new ROdouble(getContainerServices()->getName()+":julianDay",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::JULIANDAY,static_cast<void *>(m_data)),true);
		  
		m_pGAST=new ROuLongLong(getContainerServices()->getName()+":GAST",getComponent(),
		  new ObsDevIO<CORBA::ULongLong>(ObsDevIO<CORBA::ULongLong>::GAST,static_cast<void *>(m_data)),true);

		m_pLST=new ROuLongLong(getContainerServices()->getName()+":LST",getComponent(),
		  new ObsDevIO<CORBA::ULongLong>(ObsDevIO<CORBA::ULongLong>::LST,static_cast<void *>(m_data)),true);
		  
		m_platitude=new ROdouble(getContainerServices()->getName()+":latitude",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::LATITUDE,static_cast<void *>(m_data)),true);
		
		m_plongitude=new ROdouble(getContainerServices()->getName()+":longitude",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::LONGITUDE,static_cast<void *>(m_data)),true);
		
		m_pheight=new ROdouble(getContainerServices()->getName()+":height",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::HEIGHT,static_cast<void *>(m_data)),true);
		  
		m_pyPolarMotion=new ROdouble(getContainerServices()->getName()+":yPolarMotion",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::YPOLAR,static_cast<void *>(m_data)),true);

		m_pxPolarMotion=new ROdouble(getContainerServices()->getName()+":xPolarMotion",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::XPOLAR,static_cast<void *>(m_data)),true);
		  		  		  
		m_pxGeod=new ROdouble(getContainerServices()->getName()+":xGeod",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::XGEOD,static_cast<void *>(m_data)),true);

		m_pyGeod=new ROdouble(getContainerServices()->getName()+":yGeod",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::YGEOD,static_cast<void *>(m_data)),true);
		  
		m_pzGeod=new ROdouble(getContainerServices()->getName()+":zGeod",getComponent(),
		  new ObsDevIO<CORBA::Double>(ObsDevIO<CORBA::Double>::ZGEOD,static_cast<void *>(m_data)),true);
		  		
		m_pgeodeticModel=new ROEnumImpl<ACS_ENUM_T(Antenna::TGeodeticModel),POA_Antenna::ROTGeodeticModel>
		  (getContainerServices()->getName()+":geodeticModel",getComponent(),
  		  new ObsDevIO<Antenna::TGeodeticModel>(ObsDevIO<Antenna::TGeodeticModel>::GEOMODEL,static_cast<void *>(m_data)),true);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"ObservatoryImpl::initialize()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::initialize()",(LM_INFO,"READING_CONFIGURATION"));
	_GET_DOUBLE_ATTRIBUTE("DUT1","Dut1 (milliseconds):",dut1,"DataBlock/Station");  // could throw an exception
	_GET_DOUBLE_ATTRIBUTE("longitude","longitude (degrees):",longitude,"DataBlock/Station");
	_GET_DOUBLE_ATTRIBUTE("latitude","latitude (degrees):",latitude,"DataBlock/Station");
	_GET_DOUBLE_ATTRIBUTE("height","height (meters):",height,"DataBlock/Station");
	_GET_DOUBLE_ATTRIBUTE("yPolarMotion","yPolarMotion (mas):",yPolar,"DataBlock/Station");
	_GET_DOUBLE_ATTRIBUTE("xPolarMotion","xPolarMotion (mas):",xPolar,"DataBlock/Station");
	_GET_LONG_ATTRIBUTE("geodeticModel","GeodeticModel:",model,"DataBlock/Station");
	_GET_STRING_ATTRIBUTE("stationName","Station name:",obsName,"DataBlock/Station"); 
	
	m_observatoryName->getDevIO()->write((const char *)obsName,timeStamp);
	m_pDUT1->getDevIO()->write(dut1,timeStamp);  // could throw an exception
	m_platitude->getDevIO()->write(latitude,timeStamp);
	m_plongitude->getDevIO()->write(longitude,timeStamp);
	m_pheight->getDevIO()->write(height,timeStamp);
	m_pyPolarMotion->getDevIO()->write(yPolar,timeStamp);
	m_pxPolarMotion->getDevIO()->write(xPolar,timeStamp);
	m_pgeodeticModel->getDevIO()->write( (Antenna::TGeodeticModel)model,timeStamp);
	ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::initialize()",(LM_INFO,"ObservatoryImpl::COMPSTATE_INITIALIZED"));	
}

void ObservatoryImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("ObservatoryImpl::execute()");	
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ThreadErrorExImpl,__dummy,E,"ObservatoryImpl::execute");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ThreadErrorExImpl,__dummy,E,"ObservatoryImpl::execute");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::execute()",(LM_INFO,"ObservatoryImpl::COMPSTATE_OPERATIONAL"));
}

void ObservatoryImpl::cleanUp()
{
	AUTO_TRACE("ObservatoryImpl::cleanUp()");
	stopPropertiesMonitoring();
	if (m_data) {
		delete m_data;
		m_data=NULL;
	}
	CharacteristicComponentImpl::cleanUp();
}

void ObservatoryImpl::aboutToAbort()
{
	AUTO_TRACE("ObservatoryImpl::aboutToAbort()");
	if (m_data) {
		delete m_data;
		m_data=NULL;
	}
}

GET_PROPERTY_REFERENCE(ACS::ROstring,m_observatoryName,observatoryName);

GET_PROPERTY_REFERENCE(ACS::ROuLongLong,m_puniversalTime,universalTime);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pjulianDay,julianDay);

GET_PROPERTY_REFERENCE(ACS::ROuLongLong,m_pGAST,GAST);

GET_PROPERTY_REFERENCE(ACS::ROuLongLong,m_pLST,LST);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pDUT1,DUT1);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_platitude,latitude);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_plongitude,longitude);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pheight,height);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pyPolarMotion,yPolarMotion);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pxPolarMotion,xPolarMotion);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pxGeod,xGeod);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pyGeod,yGeod);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pzGeod,zGeod);

GET_PROPERTY_REFERENCE(Antenna::ROTGeodeticModel,m_pgeodeticModel,geodeticModel);

Antenna::TSiteInformation ObservatoryImpl::getSiteSummary() throw (CORBA::SystemException)
{
	AUTO_TRACE("ObservatoryImpl::getSiteSummary()");
	CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();	
	Antenna::TSiteInformation tmp=data->site.getSummary();
	tmp.DUT1=data->dut1;
	return tmp;	
}

void ObservatoryImpl::setDUT1(CORBA::Double val,CORBA::Boolean save) throw (CORBA::SystemException,ComponentErrorsEx)
{
    ACS::Time timestamp;
	AUTO_TRACE("ObservatoryImpl::setDUT1()");
	try {
		m_pDUT1->getDevIO()->write(val,timestamp);
		ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::setDUT1()",(LM_NOTICE,"DUT1_CHANGED"));
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"ObservatoryImpl::setDUT1");
		dummy.setPropertyName("DUT1");
		dummy.setReason("Property could not be written");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
	if (save) {
		if (!CIRATools::setDBValue(getContainerServices(),"DUT1",val,"alma/","DataBlock/Station")) {
			_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"ObservatoryImpl::setDUT1()");
			dummy.setFieldName("DUT1/default_value");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();		
		}
		ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::setDUT1()",(LM_NOTICE,"NEW_DUT1_STORED_IN_CDB"));
	}	
}

void ObservatoryImpl::setGeodeticModel(Antenna::TGeodeticModel model,CORBA::Boolean save) throw (CORBA::SystemException,ComponentErrorsEx)
{
    ACS::Time timestamp;
	AUTO_TRACE("ObservatoryImpl::setGeodeticModel()");
	try {
		m_pgeodeticModel->getDevIO()->write(model,timestamp);
		ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::setGeodeticModel()",(LM_NOTICE,"GEOID_MODEL_CHANGED"));		
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"ObservatoryImpl::setGeodeticModel");
		dummy.setPropertyName("geodeticModel");
		dummy.setReason("Property could not be written");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
	if (save) {
		if (!CIRATools::setDBValue(getContainerServices(),"geodeticModel",(long)model,"alma/","DataBlock/Station")) {
			_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"ObservatoryImpl::setGeodeticModel()");
			dummy.setFieldName("geodeticModel/default_value");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();		
		}
		ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::setGeodeticModel()",(LM_NOTICE,"NEW_GEOID_MODEL_STORED_IN_CDB"));
	}		
}

void ObservatoryImpl::setPoleMotion(CORBA::Double xP,CORBA::Double yP,CORBA::Boolean save) 
  throw (CORBA::SystemException,ComponentErrorsEx)
{
    ACS::Time timestamp;
	AUTO_TRACE("ObservatoryImpl::setPoleMotion()");
	try {
		m_pxPolarMotion->getDevIO()->write(xP,timestamp);
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"ObservatoryImpl::setPoleMotion()");
		dummy.setPropertyName("xPolarMotion");
		dummy.setReason("Property could not be written");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
	try {
		m_pyPolarMotion->getDevIO()->write(yP,timestamp);
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"ObservatoryImpl::setPoleMotion()");
		dummy.setPropertyName("yPolarMotion");
		dummy.setReason("Property could not be written");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
	ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::setPoleMotion()",(LM_NOTICE,"POLEMOTION_CHANGED"));
	if (save) {
		if (!CIRATools::setDBValue(getContainerServices(),"xPolarMotion",xP,"alma/","DataBlock/Station")) {
			_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"ObservatoryImpl::setPoleMotion()");
			dummy.setFieldName("xPolarMotion/default_value");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();		
		}
		if (!CIRATools::setDBValue(getContainerServices(),"yPolarMotion",yP,"alma/","DataBlock/Station")) {
			_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"ObservatoryImpl::setPoleMotion()");
			dummy.setFieldName("yPolarMotion/default_value");
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();		
		}
		ACS_LOG(LM_FULL_INFO,"ObservatoryImpl::setPoleMotion()",(LM_NOTICE,"NEW_POLEMOTION_STORED_IN_CDB"));
	}		
}

Antenna::TGeodeticModel ObservatoryImpl::GeodeticModel2IDL(const CSite::TGeodeticEllipsoid& model)
{
	switch (model) {
		case CSite::WGS84: { return Antenna::GEOID_WGS84; }
		case CSite::GRS80: { return Antenna::GEOID_GRS80; }
		case CSite::MERIT83: { return Antenna::GEOID_MERIT83; }
		case CSite::OSU91A: { return Antenna::GEOID_OSU91A; }
		default : { return Antenna::GEOID_SOVIET85; } // CSite::SOVIET85		
	}
}
	
CSite::TGeodeticEllipsoid ObservatoryImpl::IDL2GeodeticModel(const Antenna::TGeodeticModel& model)
{
	switch (model) {
		case Antenna::GEOID_WGS84: { return CSite::WGS84; }
		case Antenna::GEOID_GRS80: { return CSite::GRS80; }
		case Antenna::GEOID_MERIT83: { return CSite::MERIT83; }
		case Antenna::GEOID_OSU91A: { return CSite::OSU91A; }
		default : { return CSite::SOVIET85; } //OBSERV::OBSERV_SOVIET85
	}	
} 

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(ObservatoryImpl)

/*___oOo___*/
