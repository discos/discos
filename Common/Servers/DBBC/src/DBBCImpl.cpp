#include "DBBCImpl.h"
#include <LogFilter.h>
#include <IRA>
#include <ComponentErrors.h>
#include <Definitions.h>
#include "Common.h"

//#include <baciDevIO.h>
//#include <baciROstring.h>
//#include "DevIOAttenuation.h"
//#include "DevIOBandWidth.h"
//#include "DevIOTime.h"
//#include "DevIOFrequency.h"
//#include "DevIOPolarization.h"
//#include "DevIOInputsNumber.h"
//#include "DevIOSampleRate.h"
//#include "DevIOBins.h"
//#include "DevIOFileName.h"
//#include "DevIOFeed.h"
//#include "DevIOIntegration.h"
//#include "DevIOStatus.h"
//#include "DevIOBusy.h"
//#include "DevIOMode8bit.h"
//#include "DevIOTsys.h"
//#include "DevIOSectionsNumber.h"
//#include "DevIOInputSection.h"
//#include "Timer.h"

static char *rcsId="@(#) $Id: DBBCImpl.cpp,v 1.1 2011-05-31 12:19:51 amelis Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

// SimpleParser è una classe, scritta da Orlati, che serve ad estrarre un comando da una stringa (il classico parser) 
using namespace SimpleParser;

// _IRA_LOGFILTER è una macro, scritta da Orlati, che serve a filtrare i messaggi di log. E' nel file LogFilter.h
_IRA_LOGFILTER_DECLARE;

// Sequenza di inizializzazione dei vari smartpointer ; serve per comunicare col writer sotto TCP/IP; apre una serie di flussi, io ne apro il 1° e quello è il canale di comunicazione
DBBCImpl::DBBCImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices):
BulkDataSenderDefaultImpl(CompName,containerServices),
        m_ptime(this),
	m_pbackendName(this),
	m_pbandWidth(this),
	m_pfrequency(this),
	m_psampleRate(this),
	m_pattenuation(this),
	m_ppolarization(this),
	m_pbins(this),
	m_pinputsNumber(this),
	m_pintegration(this),
	m_pstatus(this),
	m_pbusy(this),
	m_pfeed(this),
	m_ptsys(this),
	m_psectionsNumber(this),
	m_pinputSection(this)
{
  AUTO_TRACE("DBBCImpl::DBBCImpl");
  intf = new Cpld2IntfClient("192.167.8.179", 4444);
  ACS_LOG(LM_FULL_INFO,"DBBCImpl::DBBCImpl()",  (LM_INFO,"DBBCImpl::COMPSTATE_CONSTRUCTOR"));	
}

DBBCImpl::~DBBCImpl(){
  AUTO_TRACE("DBBCImpl::~DBBCImpl()");
  delete intf;	 
}

void DBBCImpl::LoadScan()
{  
   //Cpld2IntfClient intf; 
   AUTO_TRACE("DBBCImpl::LoadScan()");
   ACS_LOG(LM_FULL_INFO,"DBBCImpl::LoadScan()",(LM_INFO,"DBBCImpl::SPETTROMETRO_A_SCANSIONE"));
   char buffer[32];
   sprintf(buffer, "8 0 0 0 0");
   //sprintf(buffer, "8 0 0 0 0");
   if (intf->conn.PutLine(buffer)==0) return intf->conn.LastError();
   //delete intf;
   //int n = intf->GetNumber();	
}

void DBBCImpl::LoadVLBI()
{  //Cpld2IntfClient intf;
   AUTO_TRACE("DBBCImpl::LoadVLBI()");
   ACS_LOG(LM_FULL_INFO,"DBBCImpl::LoadVLBI()",(LM_INFO,"DBBCImpl::CONFIGURAZIONE_VLBI"));
   char buffer[32];
   sprintf(buffer, "7 0 0 0 0");
   if (intf->conn.PutLine(buffer)==0) return intf->conn.LastError();
   //delete intf;
   //int n = intf->GetNumber();
}

// E' l'inizializzazione: con il metodo initialize() faccio partire le strutture che devono governare l'hardware
void DBBCImpl::initialize() 
	throw (ACSErr::ACSbaseExImpl)
{
		
	AUTO_TRACE("DBBCImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"DBBCImpl::initialize()",(LM_INFO,"DBBCImpl::COMPSTATE_INITIALIZING"));
        
        //intf ("192.167.8.179", 4444);
        if ( ! intf->IsOpen()) {
	   cerr << "Error opening connection with " << "192.167.8.179" << endl;
	   exit (-1);
    }
}

// Il codice seguente va messo sempre, sono le properties ereditate dal Generic Backend. E' una macro, definita in Definitions.h (cvs/Common/Libraries/IRALibrary/include) che serve per creare la properties
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROuLongLong,m_ptime,time); 
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROstring,m_pbackendName,backendName); 
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth); 
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROdoubleSeq,m_pfrequency,frequency); 
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROdoubleSeq,m_psampleRate,sampleRate); 
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROdoubleSeq,m_pattenuation,attenuation); 
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROlongSeq,m_ppolarization,polarization); 
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROlongSeq,m_pbins,bins); 
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROlong,m_pinputsNumber,inputsNumber);
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROlong,m_pintegration,integration);
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROpattern,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(DBBCImpl,Management::ROTBoolean,m_pbusy,busy);
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROlongSeq,m_pfeed,feed);
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROdoubleSeq,m_ptsys,systemTemperature);
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROlong,m_psectionsNumber,sectionsNumber);
_PROPERTY_REFERENCE_CPP(DBBCImpl,ACS::ROlongSeq,m_pinputSection,inputSection);

// Queste due righe seguenti vanno sempre messe in fondo al file, servono per aggiungere le funzioni di supporto MACI DLL

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(DBBCImpl)
