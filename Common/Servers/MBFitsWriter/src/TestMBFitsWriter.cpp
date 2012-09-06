#include "MBFitsManager.h"

#include <ComponentErrors.h>
#include <DataField.h>
#include <DBTable.h>
#include <Definitions.h>
#include <IRA>
#include <IRATools.h>

using maci::ContainerServices;
using IRA::CDataField;

#define PI 3.141592653589793238462643383279502884197169399375105820974944592

#define TEST_12 1

#if TEST_12
const string path("/archive/data/20120111_12/");

const string frequency5GHz("5GHz");
const string frequency8GHz("8GHz");
const string frequency22GHz("22GHz");
const string frequency(frequency5GHz);
#endif

int main( int argc, char** argv ) {

vector<string>											fields;
map<string, CDataField::TFieldType> fieldsTypes;

fields.push_back(string("band"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("band"), CDataField::STRING));
/*
fields.push_back(string("apertureEfficency"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("apertureEfficency"), CDataField::DOUBLE));
fields.push_back(string("beamEfficency"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("beamEfficency"), CDataField::DOUBLE));
fields.push_back(string("forwardEfficency"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("forwardEfficency"), CDataField::DOUBLE));
fields.push_back(string("HPBW"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("HPBW"), CDataField::DOUBLE));
fields.push_back(string("antennaGain"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("antennaGain"), CDataField::DOUBLE));

//		<calibrationTemperature>
//			<LPC12.7</LPC>
//			<RPC15.5</RPC>
//		</calibrationTemperature>
//fields.push_back(string(""));
//fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string(""), CDataField::));
fields.push_back(string("LCP"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("LCP"), CDataField::DOUBLE));
fields.push_back(string("RCP"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("RCP"), CDataField::DOUBLE));

fields.push_back(string("dsbImageRatio"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("dsbImageRatio"), CDataField::DOUBLE));

//		<gainPolynomParameters>
//			<a-6.8310687E-5</a>
//			<b7.285044E-3</b>
//			<c8.0577027E-1</c>
//		</gainPolynomParameters>
//fields.push_back(string(""));
//fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string(""), CDataField::));
fields.push_back(string("a"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("a"), CDataField::DOUBLE));
fields.push_back(string("b"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("b"), CDataField::DOUBLE));
fields.push_back(string("c"));
fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(string("c"), CDataField::DOUBLE));
*/

IRA::CDBTable* bandsParameters_p = NULL;

try {
	IRA::CError error;
	error.Reset();

//	ContainerServices containerServices(ACS_CString("MBFitsZillaContainer"), NULL);
	maci::SimpleClient client;
	client.init(argc, argv);
	client.login();

//	bandsParameters_p = new IRA::CDBTable(&containerServices, "BandsParameters", "MANAGEMENT/BandsParameters");
//	bandsParameters_p = new IRA::CDBTable(&containerServices, "BandsParameters_2", "MANAGEMENT/BandsParameters");
	bandsParameters_p = new IRA::CDBTable(&client, "BandParameters_2", "MANAGEMENT/AntennaParameters");

	for ( vector<string>::const_iterator field_ci = fields.begin(); field_ci != fields.end(); ++field_ci ) {
		map<string, CDataField::TFieldType>::const_iterator fieldType_ci = fieldsTypes.find(*field_ci);
		if ( fieldType_ci == fieldsTypes.end() ) {
			// TODO - add exception messag
			throw exception();
		}

		if ( !bandsParameters_p->addField(error, fieldType_ci->first.c_str(), CDataField::STRING) ) {
			string errorMessage("Error adding field " + fieldType_ci->first);
			error.setExtra(errorMessage.c_str(), 0);
		}
	}

	bool open = bandsParameters_p->openTable(error);
printf("Open: %d\n", open ? 1 : 0);
printf("Error: %s\n", string(error.getDescription()).c_str());
} catch( std::bad_alloc& exception_ ) {
	if ( bandsParameters_p ) bandsParameters_p->closeTable();
	if ( bandsParameters_p ) { delete bandsParameters_p; bandsParameters_p = NULL; }

	_EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "EngineThread::runLoop()");
	throw dummy;
}

fields.clear();
fieldsTypes.clear();
}

int main2( int argc_, char** argv_ ) {
  MBFitsManager::FeBe_v_t		frontendBackendNames;
  MBFitsManager::Double_v_t	restFrequencies;
  MBFitsManager::String_v_t	lines;
  MBFitsManager::Double_v_t	bwids;
	MBFitsManager::String_v_t	phases;

	MBFitsManager::Long_v_t		useBand;
	MBFitsManager::Long_v_t		nUseFeed;
	MBFitsManager::Long_v_t		useFeed;
	MBFitsManager::Long_v_t		beSects;
	MBFitsManager::Long_v_t		feedType;
	MBFitsManager::Double_v_t	feedOffX;
	MBFitsManager::Double_v_t	feedOffY;
	MBFitsManager::Float_v_t	polA;
	MBFitsManager::Float_v_t	aperEff;
	MBFitsManager::Float_v_t	beamEff;
	MBFitsManager::Float_v_t	etafss;
	MBFitsManager::Float_v_t	hpbw;
	MBFitsManager::Float_v_t	antGain;
	MBFitsManager::Float_v_t	tCal;
	MBFitsManager::Float_v_t	bolCalFc;
	MBFitsManager::Float_v_t	beGain;
	MBFitsManager::Float_v_t	flatFiel;
	MBFitsManager::Float_v_t	boldCoff;
	MBFitsManager::Float_v_t	gainImag;
	MBFitsManager::Float_v_t	gainEle1;
	MBFitsManager::Float_v_t	gainEle2;

	Baseband::Baseband_s_t		baseBands;
	MBFitsManager::String_v_t	phaseN;
	MBFitsManager::Double_v_t	bandwidths;
	MBFitsManager::Double_v_t	tAmb_p_humid;
	MBFitsManager::Float_v_t	data;

cout << "TestMBFitsWriter" << " - " << "ctrl 01" << endl;
	CMBFitsWriter::initialize();
cout << "TestMBFitsWriter" << " - " << "ctrl 02" << endl;
	Scan::initialize();
cout << "TestMBFitsWriter" << " - " << "ctrl 03" << endl;

  try {
cout << "TestMBFitsWriter" << " - " << "ctrl 04" << endl;
    MBFitsManager mbFitsManager(MBFitsManager::getLastVersion());
cout << "TestMBFitsWriter" << " - " << "ctrl 05" << endl;

    mbFitsManager.setPath(string(path));
cout << "TestMBFitsWriter" << " - " << "ctrl 06" << endl;

const string  telescop("Medicina"); // Telescope Name                               // TODO - CDB
const string	origin("IRA- INAF");	// Organisation or Institution                  // TODO - CDB
const string	creator("ESCS 0.2");	// Software (including version)                 // TODO - CDB
const string	instrume("MEDHET");		// Instrument type (heterodyne/bolometer/test)  // TODO - CDB

stringstream frontendBackendName;
if ( frequency == frequency5GHz ) {
  frontendBackendName << setw(8) << setfill(' ') << string("CCCL");
} else if ( frequency == frequency5GHz ) {
  frontendBackendName << setw(8) << setfill(' ') << string("CCCL");
} else if ( frequency == frequency5GHz ) {
  frontendBackendName << setw(8) << setfill(' ') << string("CCCL");
}
frontendBackendName << '-'
                    << string("Backend");
frontendBackendNames.push_back(frontendBackendName.str());
frontendBackendName.str(string());

for ( MBFitsManager::String_ci_v_t frontendBackendName_ci = frontendBackendNames.begin();
			frontendBackendName_ci != frontendBackendNames.end(); ++frontendBackendName_ci ) {
	restFrequencies.push_back(0.0);		// Centre frequency for FEBEn       // TODO - Get
	lines.push_back(string());				// Line name corresponding to FREQn // TODO - Get
	bwids.push_back(0.0);				// Bandwidth for FEBEn              // TODO - Get
}
cout << "TestMBFitsWriter" << " - " << "ctrl 07" << endl;

const string  object("3C286");       								// Object observed                  			// TODO - Get
const string	timeSys("UTC");												// Time system (TAI, UTC, UT1, etc ...) for MJD and DATE-OBS  // TODO - CDB
const double	mjdObs = 55917.6792884492;						// MJD start in TIMESYS system						// TODO - Get
const string	dateObs("2011-12-22T16:18:10.522");		// Date of observation in TIMESYS system	// TODO - Get

const double	ra = 0.0;		// TODO - Get
const double	dec = 0.0;	// TODO - Get

const string	radeSys("FK5");				// Coordinate reference frame.	// TODO - CDB
const float		equinox(2000.0);			// Equinox.											// TODO - CDB
const double	expTime(0.0);					// Total integration time.
const string	origFile;							// Original file name.
const string	projID;								// Full Project ID.							// TODO - File

const int	scanNum = 1;
cout << "TestMBFitsWriter" << " - " << "ctrl 08" << endl;

const double	siteLong	= 11.64693056;			// Observatory longitude					// TODO - CDB
const double	siteLat		= 44.52048889;			// Observatory latitude						// TODO - CDB
const float		siteElev	= 28.0;							// Observatory elevation					// TODO - CDB
const float		diameter	= 32.0;							// dish diameter									// TODO - CDB
const string	obsID("Paolo Libardi - PL");	// Observer and operator initials	// TODO - File
const double	lst = 30928096.387;						// Local apparent sidereal time (scan start)	// TODO - Get

const double	utc2Ut1 = 1.0;								// UT1-UTC time translation										// TODO - CDB
const double	tai2Utc = 0.0;								// UTC-TAI time translation										// TODO - CDB
const double	etUtc		= tai2Utc + 32.184;		// Ephemeris Time - UTC time translation			// TODO - CDB
const double	gpsTai	= -19.0;							// GPS time - TAI translation									// TODO - CDB
cout << "TestMBFitsWriter" << " - " << "ctrl 09" << endl;

const string	wcsName("DESCRIP EQUATORIAL");	// TODO - Schedule + Sw
const string	cType1("RA---SFL");							// TODO - Schedule + File + Sw
const string	cType2("DEC--SFL");							// TODO - Schedule + File + Sw
const double	crVal1		= 0.0;								// TODO - Schedule
const double	crVal2		= 0.0;								// TODO - Schedule
const string	cType1n("RA---SFL");						// TODO - Schedule + File + Sw
const string	cType2n("RA---SFL");						// TODO - Schedule + File + Sw
const double	lonPole		= 0.0;								// TODO - verificare correttezza
const double	latPole 	= PI - crVal2;				// TODO - verificare correttezza
const double	bLongObj	= crVal1;							// TODO - verificare correttezza
const double	bLatObj		= crVal2;							// TODO - verificare correttezza
const double	longObj		= 0.0;								// TODO - verificare correttezza
const double	latObj		= 0.0;								// TODO - verificare correttezza
const double	patLong		= 0.0;								// TODO - verificare correttezza
const double	patLat		= 0.0;								// TODO - verificare correttezza
cout << "TestMBFitsWriter" << " - " << "ctrl 10" << endl;

const string	calCode;												// TODO - File

const bool		moveFram	= false;							// TODO - File
const double	periDate	= 0.0;								// TODO - File
const double	periDist	= 0.0;								// TODO - File
const double	longAsc		= 0.0;								// TODO - File
const double	omega			= 0.0;								// TODO - File
const double	inclinat	= 0.0;								// TODO - File
const double	eccentr		= 0.0;								// TODO - File
const double	orbEpoch	= 0.0;								// TODO - File
const double	orbEqnox	= 0.0;								// TODO - File
const double	distance	= 0.0;								// TODO - File
cout << "TestMBFitsWriter" << " - " << "ctrl 11" << endl;

const Scan::Type_e			scanType(Scan::typeUnknown);				// TODO - File + Sw
const Scan::Mode_e			scanMode(Scan::modeUnknown);				// TODO - File + Sw
const Scan::Geometry_e	scanGeom(Scan::geometryUnknown);		// TODO - File + Sw
const Scan::Direction_e	scanDir(Scan::directionUndefined);	// TODO - Schedule + Sw
const int								scanLine	= 1;				// TODO - File
const int								scanRpts	= 1;				// TODO - File
const double						scanLen		= 0.0;			// TODO - Schedule + Sw
const double						scanXVel	= 0.0;			// TODO - Schedule + Sw
const double						scanTime	= 0.0;			// TODO - Schedule + Sw
const double						scanXSpc	= 0.0;			// TODO - Schedule + Sw
const double						scanYSpc	= 0.0;			// TODO - Schedule + Sw
const double						scanSkew	= 0.0;			// TODO - Schedule + Sw
const double						scanPar1	= 0.0;			// TODO - Schedule + Sw
const double						scanPar2	= 0.0;			// TODO - Schedule + Sw
const string						croCycle;							// TODO - File
const bool							zigZag		= false;		// TODO - File
const string						cType1N;							// TODO - File
const string						cType2N;							// TODO - File
const Scan		scan(scanType,
									 scanMode,
									 scanGeom,
									 scanDir,
									 scanLine,
									 scanRpts,
									 scanLen,
									 scanXVel,
									 scanTime,
									 scanXSpc,
									 scanYSpc,
									 scanSkew,
									 scanPar1,
									 scanPar2,
									 croCycle,
									 zigZag,
									 cType1N,
									 cType2N);
cout << "TestMBFitsWriter" << " - " << "ctrl 12" << endl;

const double	tranDist	= 0.0;								// TODO - File
const double	tranFreq	= 0.0;								// TODO - File
const double	tranFocu	= 0.0;								// TODO - File

const bool		wobUsed		= false;							// TODO - File
const double	wobThrow	= 0.0;								// TODO - File
const string	wobDir;													// TODO - File
const float		wobCycle	= 0.0;								// TODO - File
const string	wobMode;												// TODO - File
const string	wobPatt;												// TODO - File

const int			nPhases		= 1;									// TODO - File
for ( int indexPhase = 1; indexPhase <= nPhases; ++indexPhase ) {
	stringstream phase;
	phase << indexPhase;
	phases.push_back(phase.str());		// TODO - CDB
	phase.str(string());
}

const int			nFebe = frontendBackendNames.size();
cout << "TestMBFitsWriter" << " - " << "ctrl 13" << endl;

const float		pDeltaIA	= 0.0;
const float		pDeltaCA	= 0.0;
const float		pDeltaIE	= 0.0;
const float		fDeltaIA	= 0.0;
const float		fDeltaCA	= 0.0;
const float		fDeltaIE	= 0.0;
const float		ia				= 0.0;
const float		ie				= 0.0;
const float		hasa			= 0.0;
const float		haca			= 0.0;
const float		hese			= 0.0;
const float		hece			= 0.0;
const float		hesa			= 0.0;
const float		hasa2			= 0.0;
const float		haca2			= 0.0;
const float		hesa2			= 0.0;
const float		heca2			= 0.0;
const float		haca3			= 0.0;
const float		heca3			= 0.0;
const float		hesa3			= 0.0;
const float		npae			= 0.0;
const float		ca				= 0.0;
const float		an				= 0.0;
const float		aw				= 0.0;
const float		hece2			= 0.0;
const float		hece6			= 0.0;
const float		hesa4			= 0.0;
const float		hesa5			= 0.0;
const float		hsca			= 0.0;
const float		hsca2			= 0.0;
const float		hssa3			= 0.0;
const float		hsca5			= 0.0;
const float		nrx				= 0.0;
const float		nry				= 0.0;
const float		hysa			= 0.0;
const float		hyse			= 0.0;
const double	setLinX		= 0.0;
const double	setLinY		= 0.0;
const double	setLinZ		= 0.0;
const double	setRotX		= 0.0;
const double	setRotY		= 0.0;
const double	setRotZ		= 0.0;
const double	moveFoc		= 0.0;
const double	focAmp		= 0.0;
const double	focFreq		= 0.0;
const double	focPhase	= 0.0;
cout << "TestMBFitsWriter" << " - " << "ctrl 14" << endl;

const string	dewCabin;					// TODO - Get
const string	dewRtMod;					// TODO - Get
const float		dewUser		= 0.0;	// TODO - Get
const float		dewZero		= 0.0;	// TODO - Get
const string	location;					// TODO - Get
const string	optPath;					// TODO - Get
const int			nOptRefl	= 0;		// TODO - Get
const int			febeBand	= 2;		// TODO - Get
const int			febeFeed	= 2;		// TODO - Get

//const string	fdTypCod	= 0;		// TODO - Get
const string	fdTypCod;		      // TODO - Get
const float		feGain		= 0.0;	// TODO - Get
const string	swtchMod;	// TODO - File
//	const int			nPhases;
const float		frThrwLo	= 0.0;	// TODO - Get
const float		frThrwHi	= 0.0;	// TODO - Get
const float		tBlank		= 0.0;	// TODO - Get
const float		tSync			= 0.0;	// TODO - Get
const float		iaRx			= 0.0;	// TODO - ???
const float		ieRx			= 0.0;	// TODO - ???
const float		hasaRx		= 0.0;	// TODO - ???
const float		hacaRx		= 0.0;	// TODO - ???
const float		heseRx		= 0.0;	// TODO - ???
const float		heceRx		= 0.0;	// TODO - ???
const float		hesaRx		= 0.0;	// TODO - ???
const float		hasa2Rx		= 0.0;	// TODO - ???
const float		haca2Rx		= 0.0;	// TODO - ???
const float		hesa2Rx		= 0.0;	// TODO - ???
const float		heca2Rx		= 0.0;	// TODO - ???
const float		haca3Rx		= 0.0;	// TODO - ???
const float		heca3Rx		= 0.0;	// TODO - ???
const float		hesa3Rx		= 0.0;	// TODO - ???
const float		npaeRx		= 0.0;	// TODO - ???
const float		caRx			= 0.0;	// TODO - ???
const float		anRx			= 0.0;	// TODO - ???
const float		awRx			= 0.0;	// TODO - ???
const float		hece2Rx		= 0.0;	// TODO - ???
const float		hece6Rx		= 0.0;	// TODO - ???
const float		hesa4Rx		= 0.0;	// TODO - ???
const float		hesa5Rx		= 0.0;	// TODO - ???
const float		hscaRx		= 0.0;	// TODO - ???
const float		hsca2Rx		= 0.0;	// TODO - ???
const float		hssa3Rx		= 0.0;	// TODO - ???
const float		hsca5Rx		= 0.0;	// TODO - ???
const float		nRxRx			= 0.0;	// TODO - ???
const float		nRyRx			= 0.0;	// TODO - ???
const string	sigOnln;					// TODO - ???
const string	refOnln;					// TODO - ???
const string	sigPol;						// TODO - ???
const string	refPol;						// TODO - ???
cout << "TestMBFitsWriter" << " - " << "ctrl 15" << endl;

const int nUseBand = 2;
for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
	useBand.push_back(indexUseBand + 1);
}

nUseFeed.push_back(1);
nUseFeed.push_back(1);
useFeed.push_back(1);
useFeed.push_back(2);
beSects.push_back(1);
beSects.push_back(2);
feedType.push_back(3);
feedType.push_back(3);
feedOffX.push_back(0.0);
feedOffX.push_back(0.0);
feedOffY.push_back(0.0);
feedOffY.push_back(0.0);

const int			refFeed = 1;
const string	polTy;

polA.push_back(0.0);
polA.push_back(0.0);
aperEff.push_back(1.0);
aperEff.push_back(1.0);
beamEff.push_back(1.0);
beamEff.push_back(1.0);
etafss.push_back(1.0);
etafss.push_back(1.0);
hpbw.push_back(1.0);
hpbw.push_back(1.0);
antGain.push_back(1.0);
antGain.push_back(1.0);
tCal.push_back(1.0);
tCal.push_back(1.0);
bolCalFc.push_back(1.0);
bolCalFc.push_back(1.0);
beGain.push_back(1.0);
beGain.push_back(1.0);
flatFiel.push_back(1.0);
flatFiel.push_back(1.0);
boldCoff.push_back(1.0);
boldCoff.push_back(1.0);
gainImag.push_back(1.0);
gainImag.push_back(1.0);
gainEle1.push_back(1.0);
gainEle1.push_back(1.0);
gainEle2.push_back(1.0);
gainEle2.push_back(1.0);

string rxBa(80, '-');
string rxCh(80, '-');
string rxHor(80, '-');

string rxBa_40(rxBa.substr( 0, 40));
string rxBa_80(rxBa.substr(40, 80));
string rxCh_40(rxCh.substr( 0, 40));
string rxCh_80(rxCh.substr(40, 80));
string rxHor_40(rxHor.substr( 0, 40));
string rxHor_80(rxHor.substr(40, 80));

cout << "TestMBFitsWriter" << " - " << "ctrl 16" << endl;
    mbFitsManager.startScan(telescop,
														origin,
														creator,
														instrume,
														frontendBackendNames,
														restFrequencies,
														lines,
														bandwidths,
														object,
														timeSys,
														mjdObs,
														dateObs,
														mjdObs,
														ra,
														dec,
														radeSys,
														equinox,
														expTime,
														origFile,
														projID,
														scanNum,
														siteLong,
														siteLat,
														siteElev,
														diameter,
														obsID,
														lst,
														utc2Ut1,
														tai2Utc,
														etUtc,
														gpsTai,
														wcsName,
														cType1,
														cType2,
														crVal1,
														crVal2,
														cType1n,
														cType2n,
														lonPole,
														latPole,
														bLongObj,
														bLatObj,
														longObj,
														latObj,
														patLong,
														patLat,
														calCode,
														moveFram,
														periDate,
														periDist,
														longAsc,
														omega,
														inclinat,
														eccentr,
														orbEpoch,
														orbEqnox,
														distance,
														scan,
														tranDist,
														tranFreq,
														tranFocu,
														wobUsed,
														wobThrow,
														wobDir,
														wobCycle,
														wobMode,
														wobPatt,
														phases,
														nFebe,
														pDeltaIA,
														pDeltaCA,
														pDeltaIE,
														fDeltaIA,
														fDeltaCA,
														fDeltaIE,
														ia,
														ie,
														hasa,
														haca,
														hese,
														hece,
														hesa,
														hasa2,
														haca2,
														hesa2,
														heca2,
														haca3,
														heca3,
														hesa3,
														npae,
														ca,
														an,
														aw,
														hece2,
														hece6,
														hesa4,
														hesa5,
														hsca,
														hsca2,
														hssa3,
														hsca5,
														nrx,
														nry,
														hysa,
														hyse,
														setLinX,
														setLinY,
														setLinZ,
														setRotX,
														setRotY,
														setRotZ,
														moveFoc,
														focAmp,
														focFreq,
														focPhase,

														dewCabin,
														dewRtMod,
														dewUser,
														dewZero,
														location,
														optPath,
														nOptRefl,
														febeBand,
														febeFeed,
														useBand.size(),
														fdTypCod,
														feGain,
														swtchMod,
														nPhases,
														frThrwLo,
														frThrwHi,
														tBlank,
														tSync,
														iaRx,
														ieRx,
														hasaRx,
														hacaRx,
														heseRx,
														heceRx,
														hesaRx,
														hasa2Rx,
														haca2Rx,
														hesa2Rx,
														heca2Rx,
														haca3Rx,
														heca3Rx,
														hesa3Rx,
														npaeRx,
														caRx,
														anRx,
														awRx,
														hece2Rx,
														hece6Rx,
														hesa4Rx,
														hesa5Rx,
														hscaRx,
														hsca2Rx,
														hssa3Rx,
														hsca5Rx,
														nRxRx,
														nRyRx,
														sigOnln,
														refOnln,
														sigPol,
														refPol,

														useBand,
														nUseFeed,
														useFeed,
														beSects,
														feedType,
														feedOffX,
														feedOffY,
														refFeed,
														polTy,
														polA,
														aperEff,
														beamEff,
														etafss,
														hpbw,
														antGain,
														tCal,
														bolCalFc,
														beGain,
														flatFiel,
														boldCoff,
														gainImag,
														gainEle1,
														gainEle2,
														rxBa_40,
														rxBa_80,
														rxCh_40,
														rxCh_80,
														rxHor_40,
														rxHor_80
														);

baseBands.insert(Baseband(50.0, 300.0, 0.0));
baseBands.insert(Baseband(50.0, 300.0, 1.0));

const int			subScanNum = 1;
const string	usrFrame("EQEQEQ");
const string	subsType("ON");
const bool		dpBlock = false;
const int 		nInts	= 1;
const bool		wobCoord = false;
phaseN.push_back("1");
const float		dewAng = 0.0f;
const float		dewExtra = 0.0f;

const int			channels = 1;
const double	freqRes = 0.0;
for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
	bandwidths.push_back(150.0);
}
const string	molecule;
const string	transiti;
const double	restFreq = 0.0;
const double	skyFreq = 0.0;
const string	sideBand("USB");
const double	sbSep = 0.0;
const string	_2ctyp2("PIX-INDX");
const int			_2crpx2 = 0;
const int			_2crvl2 = 0;
const int			_21cd2a = 0;

SpectralAxis spectralAxisRestFrameMainSideband(string(),
																							 string(),
																							 static_cast<float>(1.0),
																							 0.0,
																							 0.0,
																							 string(),
																							 string(),
																							 string());

SpectralAxis spectralAxisRestFrameImageSideband(string(),
																								string(),
																								static_cast<float>(1.0),
																								0.0,
																								0.0,
																								string(),
																								string(),
																								string());

SpectralAxis spectralAxisSkyFrameMainSideband(string(),
																							string(),
																							static_cast<float>(1.0),
																							0.0,
																							0.0,
																							string(),
																							string(),
																							string());

SpectralAxis spectralAxisSkyFrameImageSideband(string(),
																							 string(),
																							 static_cast<float>(1.0),
																							 0.0,
																							 0.0,
																							 string(),
																							 string(),
																							 string());

SpectralAxis spectralAxisRestFrameVelocity(string(),
																					 string(),
																					 static_cast<float>(1.0),
																					 0.0,
																					 0.0,
																					 string(),
																					 string(),
																					 string());

const float			_1vsou2r	= 0.0;
const float			_1vsys2r	= 0.0;
cout << "TestMBFitsWriter" << " - " << "ctrl 17" << endl;

		mbFitsManager.startSubScan(frontendBackendNames,
															 baseBands,

															 scanNum,
															 subScanNum,

															 dateObs,
															 usrFrame,
															 lst,
															 subsType,

															 scan,

//															 cType1n,
//															 cType2n,

															 dpBlock,
															 nInts,

															 wobCoord,

															 phaseN,

															 dewAng,
															 dewExtra,

															 channels,
															 freqRes,
															 bandwidths,
															 molecule,
															 transiti,
															 restFreq,
															 skyFreq,
															 sideBand,
															 sbSep,
															 _2ctyp2,
															 _2crpx2,
															 _2crvl2,
															 _21cd2a,

											 				 spectralAxisRestFrameMainSideband,
											 				 spectralAxisRestFrameImageSideband,
											 				 spectralAxisSkyFrameMainSideband,
											 				 spectralAxisSkyFrameImageSideband,
											 				 spectralAxisRestFrameVelocity,

											 				 _1vsou2r,
											 				 _1vsys2r
											 				);
cout << "TestMBFitsWriter" << " - " << "ctrl 18" << endl;

double	mjd = mjdObs;
const string referenceKeyword("TAMB_P_HUMID");
tAmb_p_humid.push_back(13.7);
tAmb_p_humid.push_back(1021.23);
tAmb_p_humid.push_back(89.7);
cout << "TestMBFitsWriter" << " - " << "ctrl 19" << endl;

		mbFitsManager.monitor(mjd,
													referenceKeyword,
													tAmb_p_humid
												 );
cout << "TestMBFitsWriter" << " - " << "ctrl 20" << endl;

		tAmb_p_humid.clear();

		for ( int indexLoop = 0; indexLoop < 100; ++indexLoop ) {
cout << "TestMBFitsWriter" << " - " << "ctrl 21" << endl;

			for ( MBFitsManager::FeBe_ci_v_t frontendBackendName_ci = frontendBackendNames.begin();
						frontendBackendName_ci != frontendBackendNames.end(); ++frontendBackendName_ci ) {
cout << "TestMBFitsWriter" << " - " << "ctrl 22" << endl;
const double	integTim = 0.0;
const int			phase = 1;
const double	longOff = 0.0;
const double	latOff = 0.0;
const double	azimuth = 0.0;
const double	elevatio = 0.0;
const double	ra = 0.0;
const double	dec = 0.0;
const double	parAngle = 0.0;
const double	cBasLong = 0.0;
const double	cBasLat = 0.0;
const double	basLong = 0.0;
const double	basLat = 0.0;
const double	rotAngle = 0.0;
const double	mCRVal1 = 0.0;
const double	mCRVal2 = 0.0;
const double	mLonPole = 0.0;
const double	mLatPole = 0.0;
const double	dFocus_x = 0.0;
const double	dFocus_y = 0.0;
const double	dFocus_z = 0.0;
const double	dPhi_x = 0.0;
const double	dPhi_y = 0.0;
const double	dPhi_z = 0.0;
const double	wobDisLN = 0.0;
const double	wobDisLT = 0.0;
cout << "TestMBFitsWriter" << " - " << "ctrl 23" << endl;

				mbFitsManager.integrationParameters(*frontendBackendName_ci,
																						mjd,
																						lst,
																						integTim,
																						phase,
																						longOff,
																						latOff,
																						azimuth,
																						elevatio,
																						ra,
																						dec,
																						parAngle,
																						cBasLong,
																						cBasLat,
																						basLong,
																						basLat,
																						rotAngle,
																						mCRVal1,
																						mCRVal2,
																						mLonPole,
																						mLatPole,
																						dFocus_x,
																						dFocus_y,
																						dFocus_z,
																						dPhi_x,
																						dPhi_y,
																						dPhi_z,
																						wobDisLN,
																						wobDisLT
																						);
cout << "TestMBFitsWriter" << " - " << "ctrl 24" << endl;
cout << "TestMBFitsWriter" << " - " << "ctrl 25" << endl;

				int indexBaseband = 0;
				for ( Baseband::Baseband_ci_s_t baseband_ci = baseBands.begin();
							baseband_ci != baseBands.end(); ++baseband_ci ) {
cout << "TestMBFitsWriter" << " - " << "ctrl 26" << endl;
data.push_back(978.3);

cout << "TestMBFitsWriter" << " - " << "ctrl 27" << endl;
					mbFitsManager.integration(*frontendBackendName_ci,
																		indexBaseband + 1,
																		mjd,
																		data
																	 );
cout << "TestMBFitsWriter" << " - " << "ctrl 28" << endl;

					data.clear();

					++indexBaseband;
				}
			}
cout << "TestMBFitsWriter" << " - " << "ctrl 29" << endl;

			if ( 0 == (indexLoop % 7) ) {
cout << "TestMBFitsWriter" << " - " << "ctrl 30" << endl;

tAmb_p_humid.push_back(13.7);
tAmb_p_humid.push_back(1021.23);
tAmb_p_humid.push_back(89.7);
cout << "TestMBFitsWriter" << " - " << "ctrl 31" << endl;

				mbFitsManager.monitor(mjd,
															referenceKeyword,
															tAmb_p_humid
														 );
cout << "TestMBFitsWriter" << " - " << "ctrl 32" << endl;

				tAmb_p_humid.clear();
cout << "TestMBFitsWriter" << " - " << "ctrl 33" << endl;
			}
cout << "TestMBFitsWriter" << " - " << "ctrl 34" << endl;
		}
cout << "TestMBFitsWriter" << " - " << "ctrl 35" << endl;

tAmb_p_humid.push_back(13.7);
tAmb_p_humid.push_back(1021.23);
tAmb_p_humid.push_back(89.7);
cout << "TestMBFitsWriter" << " - " << "ctrl 36" << endl;

		mbFitsManager.monitor(mjd,
													referenceKeyword,
													tAmb_p_humid
												 );
cout << "TestMBFitsWriter" << " - " << "ctrl 37" << endl;

		tAmb_p_humid.clear();

		mbFitsManager.endSubScan(frontendBackendNames);
cout << "TestMBFitsWriter" << " - " << "ctrl 38" << endl;

		mbFitsManager.endScan();
cout << "TestMBFitsWriter" << " - " << "ctrl 39" << endl;

		data.clear();
		tAmb_p_humid.clear();
		phaseN.clear();
    baseBands.clear();

		useBand.clear();
		nUseFeed.clear();
		useFeed.clear();
		beSects.clear();
		feedType.clear();
		feedOffX.clear();
		feedOffY.clear();
		polA.clear();
		aperEff.clear();
		beamEff.clear();
		etafss.clear();
		hpbw.clear();
		antGain.clear();
		tCal.clear();
		bolCalFc.clear();
		beGain.clear();
		flatFiel.clear();
		boldCoff.clear();
		gainImag.clear();
		gainEle1.clear();
		gainEle2.clear();

		phases.clear();
    bandwidths.clear();
    lines.clear();
    restFrequencies.clear();
    frontendBackendNames.clear();
cout << "TestMBFitsWriter" << " - " << "ctrl 40" << endl;
  } catch( FITS::CantCreate& exception_ ) {
    cerr << "Error: " << exception_.message() << endl;

		data.clear();
		tAmb_p_humid.clear();
		phaseN.clear();
    baseBands.clear();

		useBand.clear();
		nUseFeed.clear();
		useFeed.clear();
		beSects.clear();
		feedType.clear();
		feedOffX.clear();
		feedOffY.clear();
		polA.clear();
		aperEff.clear();
		beamEff.clear();
		etafss.clear();
		hpbw.clear();
		antGain.clear();
		tCal.clear();
		bolCalFc.clear();
		beGain.clear();
		flatFiel.clear();
		boldCoff.clear();
		gainImag.clear();
		gainEle1.clear();
		gainEle2.clear();

		phases.clear();
    bandwidths.clear();
    lines.clear();
    restFrequencies.clear();
    frontendBackendNames.clear();
  } catch( FitsException& exception_ ) {
    cerr << "Error: " << exception_.message() << endl;

		data.clear();
		tAmb_p_humid.clear();
		phaseN.clear();
    baseBands.clear();

		useBand.clear();
		nUseFeed.clear();
		useFeed.clear();
		beSects.clear();
		feedType.clear();
		feedOffX.clear();
		feedOffY.clear();
		polA.clear();
		aperEff.clear();
		beamEff.clear();
		etafss.clear();
		hpbw.clear();
		antGain.clear();
		tCal.clear();
		bolCalFc.clear();
		beGain.clear();
		flatFiel.clear();
		boldCoff.clear();
		gainImag.clear();
		gainEle1.clear();
		gainEle2.clear();

		phases.clear();
    bandwidths.clear();
    lines.clear();
    restFrequencies.clear();
    frontendBackendNames.clear();
  }

cout << "TestMBFitsWriter" << " - " << "ctrl 41" << endl;
	Scan::terminate();
cout << "TestMBFitsWriter" << " - " << "ctrl 42" << endl;
	CMBFitsWriter::terminate();
cout << "TestMBFitsWriter" << " - " << "ctrl 43" << endl;

cout << "In 'MBFitsManager::startSubScan' viene calcolato il valore di 'nUseFeed' sulla base dei valori di 'baseBands'." << endl
		 << "Devo quindi inizializzare tale insieme in modo corretto!" << endl;

	return 0;
}
