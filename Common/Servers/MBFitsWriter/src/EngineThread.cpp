#include "EngineThread.h"
#include "EngineThread.h"

#include "CommonTools.h"
#include "Configuration.h"

#include "MBFitsManager.h"

#include "Scan.h"

#include <DateTime.h>
#include <ManagementErrors.h>
#include <SkySource.h>

using MBFitsWriter_private::EngineThread;

using std::bad_alloc;

#define PI 3.141592653589793238462643383279502884197169399375105820974944592

#define _GET_DOUBLE_ATTRIBUTE_E(ATTRIB,DESCR,FIELD,NAME) { \
	double tmpd; \
	if (!IRA::CIRATools::getDBValue(m_containerServices_p, ATTRIB, tmpd, "alma/", NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
		throw dummy; \
	} \
	else { \
		FIELD=tmpd; \
		ACS_DEBUG_PARAM("CConfiguration::init()",DESCR" %lf",tmpd); \
	} \
}

/*	// Not implemented
EngineThread::EngineThread() : ACS::Thread(),
															 m_dataWrapper_p(NULL),
															 m_configuration_p(NULL),
															 m_containerServices_p(NULL),
															 m_collectThread_p(NULL),
															 m_antennaBoss_p(Antenna::AntennaBoss::_nil()),
															 m_receiversBoss_p(Receivers::ReceiversBoss::_nil()),
															 m_timeSlice(0),
															 m_scanName(string()),
															 m_mbFitsManager_p(NULL),
															 m_layoutCollection(LayoutCollection()) {
}
*/

/*	// Not implemented
EngineThread::EngineThread( const EngineThread& engineThread_ ) : ACS::Thread(engineThread_),
																																	m_dataWrapper_p(engineThread_.m_dataWrapper_p),
																																	m_configuration_p(engineThread_.m_configuration_p),
																																	m_containerServices_p(engineThread_.m_containerServices_p),
																																	m_collectThread_p(engineThread_.m_collectThread_p),
																																	m_antennaBoss_p(Antenna::AntennaBoss::_nil()),
																																	m_receiversBoss_p(Receivers::ReceiversBoss::_nil()),
																																	m_timeSlice(engineThread_.m_timeSlice),
																																	m_scanName(string()),
																																	m_mbFitsManager_p(NULL),
																																	m_layoutCollection(LayoutCollection()) {
}
*/

EngineThread::EngineThread( const ACE_CString& name_,
														CSecureArea<CDataCollection>* dataWrapper_p_,
														const ACS::TimeInterval& responseTime_,
														const ACS::TimeInterval& sleepTime_ ) : ACS::Thread(name_, responseTime_, sleepTime_),
																																		m_dataWrapper_p(dataWrapper_p_),
																																		m_configuration_p(NULL),
																																		m_containerServices_p(NULL),
																																		m_collectThread_p(NULL),
																																		m_antennaBoss_p(Antenna::AntennaBoss::_nil()),
																																		m_receiversBoss_p(Receivers::ReceiversBoss::_nil()),
																																		m_timeSlice(0),
																																		m_scanName(string()),
																																		m_mbFitsManager_p(NULL),
																																		m_layoutCollection(LayoutCollection()) {
}

EngineThread::~EngineThread() {
	if ( m_mbFitsManager_p ) { delete m_mbFitsManager_p; m_mbFitsManager_p = NULL; }

	try {
		CCommonTools::unloadReceiversBoss(m_receiversBoss_p, m_containerServices_p);
		CCommonTools::unloadAntennaBoss(m_antennaBoss_p, m_containerServices_p);
	} catch( ACSErr::ACSbaseExImpl& exception_ ) {
		exception_.log(LM_WARNING);
	}

	m_receiversBoss_p = Receivers::ReceiversBoss::_nil();
	m_antennaBoss_p		= Antenna::AntennaBoss::_nil();

	m_collectThread_p			= NULL;
	m_containerServices_p	= NULL;
	m_configuration_p			= NULL;
}

/*	// Not implemented
bool EngineThread::operator==( const EngineThread& engineThread_ ) const {
	return ( ACS::Thread::operator==(engineThread_) &&
					 (((!m_dataWrapper_p				&& !engineThread_.m_dataWrapper_p				) ||
						 ( m_dataWrapper_p				&&  engineThread_.m_dataWrapper_p				&& (*m_dataWrapper_p				== *engineThread_.m_dataWrapper_p				)))) &&
					 (((!m_configuration_p			&& !engineThread_.m_configuration_p			) ||
						 ( m_configuration_p			&&  engineThread_.m_configuration_p			&& (*m_configuration_p			== *engineThread_.m_configuration_p			)))) &&
					 (((!m_containerServices_p	&& !engineThread_.m_containerServices_p	) ||
						 ( m_containerServices_p	&&  engineThread_.m_containerServices_p && (*m_containerServices_p	== *engineThread_.m_containerServices_p	)))) &&

					 (((!m_collectThread_p			&& !engineThread_.m_collectThread_p			) ||
						 ( m_collectThread_p			&&  engineThread_.m_collectThread_p			&& (*m_collectThread_p			== *engineThread_.m_collectThread_p			)))) &&

					 (m_antennaBoss_p		== engineThread_.m_antennaBoss_p	) &&
					 (m_receiversBoss_p == engineThread_.m_receiversBoss_p) &&
					 (m_timeSlice				== engineThread_.m_timeSlice			) &&
					 (m_scanName				== engineThread_.m_scanName				) &&
					 (((!m_mbFitsManager_p			&& !engineThread_.m_mbFitsManager_p			) ||
						 ( m_mbFitsManager_p			&&  engineThread_.m_mbFitsManager_p			&& (*m_mbFitsManager_p			== *engineThread_.m_mbFitsManager_p			)))  ) &&

					 (m_layoutCollection == engineThread_.m_layoutCollection) );
}
*/

/*	// Not implemented
bool EngineThread::operator!=( const EngineThread& engineThread_ ) const {
	return !(*this == engineThread_);
}
*/

/*	// Not implemented
EngineThread& EngineThread::operator=( const EngineThread& engineThread_ ) {
	if ( *this == engineThread_ ) {	// handle self assignment
		ACS::Thread::operator=(engineThread_);

		m_dataWrapper_p				= engineThread_.m_dataWrapper_p;
		m_configuration_p			= engineThread_.m_configuration_p;
		m_containerServices_p	= engineThread_.m_containerServices_p;
		m_collectThread_p			= engineThread_.m_collectThread_p;

		m_antennaBoss_p				= engineThread_.m_antennaBoss_p;
		m_receiversBoss_p			= engineThread_.m_receiversBoss_p;

		m_timeSlice						= engineThread_.m_timeSlice;

		m_scanName						= engineThread_.m_scanName;

		if ( m_mbFitsManager_p ) { delete m_mbFitsManager_p; m_mbFitsManager_p = NULL; }
		m_mbFitsManager_p = new CMBFitsWriter(engineThread_.m_mbFitsManager_p);

		m_layoutCollection = engineThread_.m_layoutCollection;
}

	return *this;
}
*/

void EngineThread::onStart() {
	AUTO_TRACE("EngineThread::onStart()");
}

void EngineThread::onStop() {
	AUTO_TRACE("EngineThread::onStop()");
}

void EngineThread::runLoop() {
  static MBFitsManager::FeBe_v_t	frontendBackendNames;
	static bool											subScanStop = true;
	static Baseband::Baseband_s_t		baseBands;
	static string										basisFrameCType;
	static string										nativeFrameCType;
	static bool		moveFram	= false;
	static double longObj		= 0.0;
	static double latObj		= 0.0;

  // Blocking call - CDataCollection instance is owned until released
	CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();

	TIMEVALUE currentUTC;
	IRA::CIRATools::getTime(currentUTC);		// it marks the start of the activity job

	if ( data_p->isReset() ) {
	  //ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_NOTICE, "FILE_CLOSED"));

	  // chiudere i file se risultano aperti
	  if ( m_mbFitsManager_p ) { delete m_mbFitsManager_p; m_mbFitsManager_p = NULL; }

		data_p->haltResetStage();
		return;
	}

  if ( data_p->isReady() ) {							// main headers are already saved
    if ( data_p->isStart() ) {
      if ( !m_mbFitsManager_p || !m_mbFitsManager_p->isScanStarted() || !data_p->isSubScanHeaderReady() ) { // file has to be opened
        data_p->setStatus(Management::MNG_OK);

				IRA::CDBTable* timeData_p										= NULL;
				IRA::CDBTable* bandsParameters_p						= NULL;
				IRA::CDBTable* pointingModelCoefficients_p	= NULL;

				Baseband::Section::Section_v_t	sections;

				MBFitsManager::Double_v_t				restFreqencies;
				MBFitsManager::String_v_t				lines;
				MBFitsManager::Double_v_t				bandwidths;

				MBFitsManager::String_v_t				phases;

				MBFitsManager::Long_v_t					useBand;

				MBFitsManager::Long_v_t					nUseFeed;
				MBFitsManager::Long_v_t					useFeed;
				MBFitsManager::Long_v_t					beSects;
				MBFitsManager::Long_v_t					feedType;

				MBFitsManager::Double_v_t				feedOffX;
				MBFitsManager::Double_v_t				feedOffY;

				MBFitsManager::Float_v_t				polA;

				MBFitsManager::Float_v_t				aperEff;
				MBFitsManager::Float_v_t				beamEff;
				MBFitsManager::Float_v_t				etafss;
				MBFitsManager::Float_v_t				hpbws;
				MBFitsManager::Float_v_t				antGain;
				MBFitsManager::Float_v_t				tCal;
				MBFitsManager::Float_v_t				bolCalFc;
				MBFitsManager::Float_v_t				beGain;
				MBFitsManager::Float_v_t				flatFiel;
				MBFitsManager::Float_v_t				boldCoff;
				MBFitsManager::Float_v_t				gainImag;
				MBFitsManager::Float_v_t				gainEle1;
				MBFitsManager::Float_v_t				gainEle2;

				try {
					// get data from receivers boss
					collectReceiversData();

					// get the data from the antenna boss
					collectAntennaData();
/*
					CString	sourceName;
					double	sourceRightAscension	= 0.0;
					double	sourceDeclination			= 0.0;
					double	sourceVlsr						= 0.0;
					data_p->getSource(sourceName, sourceRightAscension, sourceDeclination, sourceVlsr);
*/
					const string receiverCode(data_p->getReceiverCode());

					frontendBackendNames.clear();

					stringstream frontendBackendName;
					frontendBackendName << receiverCode
															<< '-'
															<< string("Backend");
					frontendBackendNames.push_back(frontendBackendName.str());
					frontendBackendName.str(string());

					m_mbFitsManager_p = new MBFitsManager(MBFitsManager::getLastVersion());

					string path(static_cast<const char*>(data_p->getFileName()));
					path = path.substr(0, path.length() - string(".fits").length()) + string("/");

					m_mbFitsManager_p->setPath(path);

//					Backends::TMainHeader						mainHeader		= data_p->getMainHeader();
					Backends::TSectionHeader const*	sectionHeader	= data_p->getSectionHeader();

					IRA::CSite			site;
					double					dut1									= 0.0;
					CString					siteName;

					ACS::doubleSeq	localOscillators;
					ACS::doubleSeq	receiversBandwidth;
					ACS::doubleSeq	receiversInitialFrequency;
					ACS::doubleSeq	calibrationMarks;
					ACS::longSeq		polarizations;
					ACS::doubleSeq	skyBandwidths;
					ACS::doubleSeq	skyFrequencies;

					CString	sourceName;
					double	sourceRightAscension	= 0.0;
					double	sourceDeclination			= 0.0;
					double	sourceVlsr						= 0.0;

					double	azimuthOffset					= 0.0;
					double	elevationOffset				= 0.0;
					double	rightAscensionOffset	= 0.0;
					double	declinationOffset			= 0.0;
					double	longitudeOffset				= 0.0;
					double	latitutdeOff					= 0.0;

					data_p->getSite(site, dut1, siteName);
					data_p->getLocalOscillator(localOscillators);
					data_p->getReceiverBandWidth(receiversBandwidth);
					data_p->getReceiverInitialFrequency(receiversInitialFrequency);
					data_p->getCalibrationMarks(calibrationMarks);
					data_p->getReceiverPolarization(polarizations);
					data_p->getSkyBandwidth(skyBandwidths);
					data_p->getSkyFrequency(skyFrequencies);
					data_p->getSource(sourceName, sourceRightAscension, sourceDeclination, sourceVlsr);
					data_p->getAntennaOffsets(azimuthOffset, elevationOffset,
																		rightAscensionOffset, declinationOffset,
																		longitudeOffset, latitutdeOff);

					const long scanNum		= data_p->getScanID();

					IRA::CDateTime	currentDatetime(currentUTC, dut1);

					stringstream dateTime;
					dateTime	<< setw(4) << setfill('0') << currentDatetime.getYear()
										<< string("-")
										<< setw(2) << setfill('0') << currentDatetime.getMonth()
										<< string("-")
										<< setw(2) << setfill('0') << currentDatetime.getDay()
										<< string("T")
										<< setw(2) << setfill('0') << currentDatetime.getHours()
										<< string(":")
										<< setw(2) << setfill('0') << currentDatetime.getMinutes()
										<< string(":")
										<< setw(2) << setfill('0') << currentDatetime.getSeconds()
										<< string(".")
										<< setw(3) << setfill('0') << currentDatetime.getMilliSeconds();

					const string dateObs(dateTime.str());

					TIMEDIFFERENCE currentLST;
					currentDatetime.LST(site).getDateTime(currentLST);  // get the current LST time

					baseBands.clear();

					for ( int indexSection = 0; indexSection < data_p->getSectionsNumber(); ++indexSection ) {
						for ( long indexInput = 0; indexInput < sectionHeader[indexSection].inputs; ++indexInput ) {
							Baseband::Section section(sectionHeader[indexSection], indexInput,
																				skyBandwidths[indexSection], skyFrequencies[indexSection] * 1000000);

							Baseband baseBand(sectionHeader[indexSection].bandWidth, sectionHeader[indexSection].frequency,
																skyBandwidths[indexSection], skyFrequencies[indexSection] * 1000000);

							Baseband::Baseband_i_s_t baseBand_i = baseBands.begin();
							for ( ; baseBand_i != baseBands.end(); ++baseBand_i ) {
								if ( baseBand_i->isSimilar(baseBand) ) {
									baseBand_i->insert(section);	// warning: passing ‘const Baseband’ as ‘this’ argument of ‘void Baseband::insert(const Baseband::Section&)’ discards qualifiers
									break;
								}
							}

							if ( baseBand_i == baseBands.end() ) {
								baseBand.insert(section);
								baseBands.insert(baseBand);
							}
						}
					}

					const int nUseBand = baseBands.size();	// Nusbd: number of bands in use

					timeData_p = loadTimeData();
					timeData_p->First();

					const string	timeSys((*timeData_p)["timeSys"]->asString());

					const double	mjd = currentDatetime.getMJD();

					const string	telescop(siteName);
					const string	origin("IRA- INAF");
					const string	creator("ESCS 0.3");
					const string	instrume("MEDHET");

for ( MBFitsManager::String_ci_v_t frontendBackendName_ci = frontendBackendNames.begin();
			frontendBackendName_ci != frontendBackendNames.end(); ++frontendBackendName_ci ) {
	restFreqencies.push_back(0.0);
	lines.push_back(string());
}

					const string  object(sourceName);
					const double	ra	= sourceRightAscension;
					const double	dec	= sourceDeclination;

					const string	radeSys("FK5");
					const float		equinox(2000.0);
					const double	exptime(0.0);

					string origFile(static_cast<const char*>(data_p->getFileName()));
					size_t posSep = origFile.rfind('/');
					if ( string::npos != posSep ) {
						origFile = origFile.substr(posSep + 1, origFile.length() - posSep - 1);
					}

					const string	projID(data_p->getProjectName());

					const double	siteLong	= site.getLongitude();
					const double	siteLat		= site.getLatitude();
					const float		siteElev	= site.getHeight();
					float	diameter = 0.0;
					_GET_DOUBLE_ATTRIBUTE_E("diameter", "Telescope dish diameter (meters):", diameter, "DataBlock/Mount");
					const string	obsID(data_p->getObserverName());
					const double	lst = currentLST.toSeconds();

					const double	utc2Ut1 = dut1;
					const double	tai2Utc	= (*timeData_p)["tai2utc"]->asDouble();
					const double	etUtc		= tai2Utc + (*timeData_p)["et2utc"]->asDouble();
					const double	gpsTai	= (*timeData_p)["gps2tai"]->asDouble();

					const Layout* const layout_p = m_layoutCollection.find("SCAN");

					string layoutValue;

					layoutValue = layout_p->find("CTYPE");
					basisFrameCType = layoutValue;
					const string basisFrameCType1(basisFrameCType.substr(0, basisFrameCType.find('/')));
					const string basisFrameCType2(basisFrameCType.substr(basisFrameCType.find('/') + 1, basisFrameCType.length() - basisFrameCType.find('/')));

					layoutValue = layout_p->find("BASISPROJECTION");
					const string basisProjection(layoutValue);
					const string basisProjectionCType1(basisProjection.substr(0, basisProjection.find('/')));
					const string basisProjectionCType2(basisProjection.substr(basisProjection.find('/') + 1, basisProjection.length() - basisProjection.find('/')));

					string	cType1(8, '-');
					string	cType2(8, '-');
					for ( unsigned int index = 0; (index < 4) && (index < basisFrameCType1.size()); ++index ) {
						cType1[index] = basisFrameCType1[index];
					}
					for ( unsigned int index = 0; (index < 3) && (index < basisProjectionCType1.size()); ++index ) {
						cType1[5 + index] = basisProjectionCType1[index];
					}
					for ( unsigned int index = 0; (index < 4) && (index < basisFrameCType2.size()); ++index ) {
						cType2[index] = basisFrameCType2[index];
					}
					for ( unsigned int index = 0; (index < 3) && (index < basisProjectionCType2.size()); ++index ) {
						cType2[5 + index] = basisProjectionCType2[index];
					}

					layoutValue = layout_p->find("CTYPEN");
					nativeFrameCType = layoutValue;
					const string nativeFrameCType1(nativeFrameCType.substr(0, nativeFrameCType.find('/')));
					const string nativeFrameCType2(nativeFrameCType.substr(nativeFrameCType.find('/') + 1, nativeFrameCType.length() - nativeFrameCType.find('/')));

					layoutValue = layout_p->find("NATIVEPROJECTION");
					const string nativeProjection(layoutValue);
					const string nativeProjectionCType1(nativeProjection.substr(0, nativeProjection.find('/')));
					const string nativeProjectionCType2(nativeProjection.substr(nativeProjection.find('/') + 1, nativeProjection.length() - nativeProjection.find('/')));

					string	cType1n(8, '-');
					string	cType2n(8, '-');
					for ( unsigned int index = 0; (index < 4) && (index < nativeFrameCType1.size()); ++index ) {
						cType1n[index] = nativeFrameCType1[index];
					}
					for ( unsigned int index = 0; (index < 3) && (index < nativeProjectionCType1.size()); ++index ) {
						cType1n[5 + index] = nativeProjectionCType1[index];
					}
					for ( unsigned int index = 0; (index < 4) && (index < nativeFrameCType2.size()); ++index ) {
						cType2n[index] = nativeFrameCType2[index];
					}
					for ( unsigned int index = 0; (index < 3) && (index < nativeProjectionCType2.size()); ++index ) {
						cType2n[5 + index] = nativeProjectionCType2[index];
					}

					layoutValue = layout_p->find("WCSNAME");
					const string	wcsName(layoutValue);

double crVal1	= 0.0;
double crVal2	= 0.0;
/*
					CSkySource skySource;

					if ( string("RA/DEC") == nativeFrameCType ) {
						skySource.setInputEquatorial(0.0, 0.0, equinox_e);
					} else if ( string("GLON/GLAT") == nativeFrameCType ) {
						skySource.setInputGalactic(0.0, 0.0);
					} else if ( string("ALON/ALAT") == nativeFrameCType ) {
						skySource.setInputHorizontal(0.0, 0.0, site);
					} else {
						//... non gestito ...
					}

					skySource.process(currentDatetime, site);

					double crVal1	= 0.0;
					double crVal2	= 0.0;

					if ( string("RA/DEC") == basisFrameCType ) {
						double eph						= 0.0;	// epoch which the coordinates refer to
						double dra						= 0.0;	// proper motion in right ascension (milli arcseconds per tropical year if FK4 is used, per julian year if Fk5 is used)
						double ddec						= 0.0;	// proper motion in declination (milli arcseconds per tropical year if FK4 is used, per julian year if Fk5 is used)
						double parallax				= 0.0;	// parallax in milli arcseconds.
						double radialVelocity	= 0.0;	// radial velocity in Km/sec ( positive value means the source is moving away)

						skySource.getInputEquatorial(crVal1, crVal2, eph, dra, ddec, parallax, rvel);
					} else if ( string("GLON/GLAT") == basisFrameCType ) {
						skySource.getInputGalactic(crVal1, crVal2);
					} else if ( string("ALON/ALAT") == basisFrameCType ) {
						// If basisFrame is Az/El then only Az/El with (0.0, 0.0) origin is allowed for NativeFrame
						crVal1	= 0.0;
						crVal2	= 0.0;
					} else {
						//... non gestito ...
					}
*/
/*
					double lonPole	= 0.0;
					double latPole	= 0.0;
					double tempCoord = 0.0;

					CSkySource skySourceCelestialPole;

					if ( string("RA/DEC") == basisFrame ) {
						skySourceCelestialPole.setInputEquatorial(0.0, 90.0 / 180.0 * PI, equinox_e);
					} else if ( string("GLON/GLAT") == basisFrame ) {
						skySourceCelestialPole.setInputGalactic(0.0, 90.0 / 180.0 * PI);
					} else if ( string("ALON/ALAT") == basisFrame ) {
						skySourceCelestialPole.setInputHorizontal(PI, site.getLatitude(), site);
					} else {
						//... non gestito ...
					}

					skySourceCelestialPole.process(currentDatetime, site);

					if ( string("RA/DEC") == nativeFrame ) {
						double epochLonPol = 0.0;
						skySourceCelestialPole.getApparentEquatorial(lonPole, tempCoord, epochLonPol);
					} else if ( string("GLON/GLAT") == nativeFrame ) {
						skySourceCelestialPole.getApparentHorizontal(lonPole, tempCoord);
					} else if ( string("ALON/ALAT") == nativeFrame ) {
						skySourceCelestialPole.getApparentGalactic(lonPole, tempCoord);
					} else {
						//... non gestito ...
					}
ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "lonPole e tempCoord: %3.2f %3.2f", lonPole, tempCoord));

					CSkySource skySourceNativePole;

					if ( string("RA/DEC") == nativeFrame ) {
						skySourceNativePole.setInputEquatorial(0.0, 90.0 / 180.0 * PI, equinox_e);
					} else if ( string("GLON/GLAT") == nativeFrame ) {
						skySourceNativePole.setInputGalactic(0.0, 90.0 / 180.0 * PI);
					} else if ( string("ALON/ALAT") == nativeFrame ) {
						skySourceNativePole.setInputHorizontal(PI, site.getLatitude(), site);
					} else {
						//... non gestito ...
					}

					skySourceNativePole.process(currentDatetime, site);

					if ( string("ALON/ALAT") == basisFrame ) {
						skySourceCelestialPole.getApparentHorizontal(latPole, tempCoord);
					} else {
						//... non gestito ...
					}
ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "tempCoord e latPole: %3.2f %3.2f", tempCoord, latPole));

ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "lonPole e latPole: %3.2f %3.2f", lonPole, latPole));
*/
					layoutValue = layout_p->find("SCANROT");
					const double scanRot = MBFitsManager::deg2Rad(atof(layoutValue.c_str()));
					const double lonPole = atan2(sin(scanRot) * cos(crVal2), sin(crVal2));
					const double latPole = asin(cos(scanRot) * cos(crVal2));

					double bLongObj	= 0.0;
					double bLatObj	= 0.0;

					if ( string("RA/DEC") == basisFrameCType ) {
						bLongObj	= sourceRightAscension;
						bLatObj		= sourceDeclination;
					} else if ( string("GLON/GLAT") == basisFrameCType ) {
						CSkySource::equatorialToGalactic(sourceRightAscension, sourceDeclination, bLongObj, bLatObj);
					} else if ( string("ALON/ALAT") == basisFrameCType ) {
						layoutValue = layout_p->find("BLONGOBJ");
						bLongObj		= atof(layoutValue.c_str());
						layoutValue = layout_p->find("BLATOBJ");
						bLatObj			= atof(layoutValue.c_str());
					} else {
						//... non gestito ...
					}

					// per avere le coordinate nel sistema native non sarebbe corretto sottrarre le coordinate CRVAL1,CRVAL2?
					if ( string("RA/DEC") == nativeFrameCType ) {
						longObj	= sourceRightAscension;
						latObj	= sourceDeclination;
					} else if ( string("GLON/GLAT") == nativeFrameCType ) {
						CSkySource::equatorialToGalactic(sourceRightAscension, sourceDeclination, longObj, latObj);
					} else if ( string("ALON/ALAT") == nativeFrameCType ) {
						if ( string("ALON/ALAT") == basisFrameCType ) {
							moveFram		= false;

							layoutValue = layout_p->find("BLONGOBJ");
							longObj			= atof(layoutValue.c_str());
							layoutValue = layout_p->find("BLATOBJ");
							latObj			= atof(layoutValue.c_str());
						} else {
							moveFram		= true;

							longObj			= sourceRightAscension;
							latObj			= sourceDeclination;
						}
					} else {
						//... non gestito ...
					}

const double	patLong		= 0.0;
const double	patLat		= 0.0;

const string	calCode;

const double	periDate	= 0.0;
const double	periDist	= 0.0;
const double	longAsc		= 0.0;
const double	omega			= 0.0;
const double	inclinat	= 0.0;
const double	eccentr		= 0.0;
const double	orbEpoch	= 0.0;
const double	orbEqnox	= 0.0;
const double	distance	= 0.0;

					Scan scan;
					scan.setLayout(*layout_p);

const double	tranDist	= 0.0;
const double	tranFreq	= 0.0;
const double	tranFocu	= 0.0;

					layoutValue = layout_p->find("WOBUSED");
					const bool		wobUsed		= (string("1") == layoutValue);
					layoutValue = layout_p->find("WOBTHROW");
					const double	wobThrow	= atof(layoutValue.c_str());
					layoutValue = layout_p->find("WOBDIR");
					const string	wobDir		= layoutValue;
					layoutValue = layout_p->find("WOBCYCLE");
					const float		wobCycle	= atof(layoutValue.c_str());
					layoutValue = layout_p->find("WOBMODE");
					const string	wobMode		= layoutValue;
					layoutValue = layout_p->find("WOBPATT");
					const string	wobPatt		= layoutValue;

const int			nPhases		= 1;
for ( int indexPhase = 0; indexPhase < nPhases; ++indexPhase ) {
	phases.push_back(string("NONE"));
}
					const int			nFebe = frontendBackendNames.size();

const float		pDeltaIA	= 0.0;
const float		pDeltaCA	= 0.0;
const float		pDeltaIE	= 0.0;
const float		fDeltaIA	= 0.0;
const float		fDeltaCA	= 0.0;
const float		fDeltaIE	= 0.0;

					pointingModelCoefficients_p = loadPointingModelCoefficients();
					pointingModelCoefficients_p->First();

					const float		ia				= (*pointingModelCoefficients_p)["ia"]->asDouble();
					const float		ie				= (*pointingModelCoefficients_p)["ie"]->asDouble();
					const float		hasa			= (*pointingModelCoefficients_p)["hasa"]->asDouble();
					const float		haca			= (*pointingModelCoefficients_p)["haca"]->asDouble();
					const float		hese			= (*pointingModelCoefficients_p)["hese"]->asDouble();
					const float		hece			= (*pointingModelCoefficients_p)["hece"]->asDouble();
					const float		hesa			= (*pointingModelCoefficients_p)["hesa"]->asDouble();
					const float		hasa2			= (*pointingModelCoefficients_p)["hasa2"]->asDouble();
					const float		haca2			= (*pointingModelCoefficients_p)["haca2"]->asDouble();
					const float		hesa2			= (*pointingModelCoefficients_p)["hesa2"]->asDouble();
					const float		heca2			= (*pointingModelCoefficients_p)["heca2"]->asDouble();
					const float		haca3			= (*pointingModelCoefficients_p)["haca3"]->asDouble();
					const float		heca3			= (*pointingModelCoefficients_p)["heca3"]->asDouble();
					const float		hesa3			= (*pointingModelCoefficients_p)["hesa3"]->asDouble();
					const float		npae			= (*pointingModelCoefficients_p)["npae"]->asDouble();
					const float		ca				= (*pointingModelCoefficients_p)["ca"]->asDouble();
					const float		an				= (*pointingModelCoefficients_p)["an"]->asDouble();
					const float		aw				= (*pointingModelCoefficients_p)["aw"]->asDouble();

					const float		hece2			= (*pointingModelCoefficients_p)["hece2"]->asDouble();
					const float		hece6			= (*pointingModelCoefficients_p)["hece6"]->asDouble();
					const float		hesa4			= (*pointingModelCoefficients_p)["hesa4"]->asDouble();
					const float		hesa5			= (*pointingModelCoefficients_p)["hesa5"]->asDouble();
					const float		hsca			= (*pointingModelCoefficients_p)["hsca"]->asDouble();
					const float		hsca2			= (*pointingModelCoefficients_p)["hsca2"]->asDouble();
					const float		hssa3			= (*pointingModelCoefficients_p)["hssa3"]->asDouble();
					const float		hsca5			= (*pointingModelCoefficients_p)["hsca5"]->asDouble();
					const float		nrx				= (*pointingModelCoefficients_p)["nrx"]->asDouble();
					const float		nry				= (*pointingModelCoefficients_p)["nry"]->asDouble();
					const float		hysa			= (*pointingModelCoefficients_p)["hysa"]->asDouble();
					const float		hyse			= (*pointingModelCoefficients_p)["hyse"]->asDouble();

					const double	setLinX		= (*pointingModelCoefficients_p)["setLinX"]->asDouble();
					const double	setLinY		= (*pointingModelCoefficients_p)["setLinY"]->asDouble();
					const double	setLinZ		= (*pointingModelCoefficients_p)["setLinZ"]->asDouble();

					const double	setRotX		= (*pointingModelCoefficients_p)["setRotX"]->asDouble();
					const double	setRotY		= (*pointingModelCoefficients_p)["setRotY"]->asDouble();
					const double	setRotZ		= (*pointingModelCoefficients_p)["setRotZ"]->asDouble();

					const bool		moveFoc		= (1 == (*pointingModelCoefficients_p)["moveFoc"]->asLongLong());
					const double	focAmp		= (*pointingModelCoefficients_p)["focAmp"]->asDouble();
					const double	focFreq		= (*pointingModelCoefficients_p)["focFreq"]->asDouble();
					const double	focPhase	= (*pointingModelCoefficients_p)["focPhase"]->asDouble();

const string	dewCabin;
const string	dewRtMod;
const float		dewUser		= 0.0;
const float		dewZero		= 0.0;
const string	location;
const string	optPath;
const int			nOptRefl	= 0;
const int			febeBand	= 2;
const int			febeFeed	= 2;

const string	fdTypCod("3: UNKNOWN");
const float		feGain		= 0.0;
					layoutValue = layout_p->find("SWTCHMOD");	
					const string	swtchMod(layoutValue);

const float		frThrwLo	= 0.0;
const float		frThrwHi	= 0.0;
const float		tBlank		= 0.0;
const float		tSync			= 0.0;

					const float iaRx		= (*pointingModelCoefficients_p)["iaRx"]->asDouble();
					const float ieRx		= (*pointingModelCoefficients_p)["ieRx"]->asDouble();
					const float hasaRx	= (*pointingModelCoefficients_p)["hasaRx"]->asDouble();
					const float hacaRx	= (*pointingModelCoefficients_p)["hacaRx"]->asDouble();
					const float heseRx	= (*pointingModelCoefficients_p)["heseRx"]->asDouble();
					const float heceRx	= (*pointingModelCoefficients_p)["heceRx"]->asDouble();
					const float hesaRx	= (*pointingModelCoefficients_p)["hesaRx"]->asDouble();
					const float hasa2Rx	= (*pointingModelCoefficients_p)["hasa2Rx"]->asDouble();
					const float haca2Rx	= (*pointingModelCoefficients_p)["haca2Rx"]->asDouble();
					const float hesa2Rx	= (*pointingModelCoefficients_p)["hesa2Rx"]->asDouble();
					const float heca2Rx	= (*pointingModelCoefficients_p)["heca2Rx"]->asDouble();
					const float haca3Rx	= (*pointingModelCoefficients_p)["haca3Rx"]->asDouble();
					const float heca3Rx	= (*pointingModelCoefficients_p)["heca3Rx"]->asDouble();
					const float hesa3Rx	= (*pointingModelCoefficients_p)["hesa3Rx"]->asDouble();
					const float npaeRx	= (*pointingModelCoefficients_p)["npaeRx"]->asDouble();
					const float caRx		= (*pointingModelCoefficients_p)["caRx"]->asDouble();
					const float anRx		= (*pointingModelCoefficients_p)["anRx"]->asDouble();
					const float awRx		= (*pointingModelCoefficients_p)["awRx"]->asDouble();

					const float hece2Rx	= (*pointingModelCoefficients_p)["hece2Rx"]->asDouble();
					const float hece6Rx	= (*pointingModelCoefficients_p)["hece6Rx"]->asDouble();
					const float hesa4Rx	= (*pointingModelCoefficients_p)["hesa4Rx"]->asDouble();
					const float hesa5Rx	= (*pointingModelCoefficients_p)["hesa5Rx"]->asDouble();
					const float hscaRx	= (*pointingModelCoefficients_p)["hscaRx"]->asDouble();
					const float hsca2Rx	= (*pointingModelCoefficients_p)["hsca2Rx"]->asDouble();
					const float hssa3Rx	= (*pointingModelCoefficients_p)["hssa3Rx"]->asDouble();
					const float hsca5Rx	= (*pointingModelCoefficients_p)["hsca5Rx"]->asDouble();
					const float nRxRx		= (*pointingModelCoefficients_p)["nRxRx"]->asDouble();
					const float nRyRx		= (*pointingModelCoefficients_p)["nRyRx"]->asDouble();

const string	sigOnln;
const string	refOnln;
const string	sigPol;
const string	refPol;

					for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
						useBand.push_back(indexUseBand + 1);
					}

					EngineThread::IntFeedHeader_m_t		idsFeeds;
					EngineThread::IntFeedHeader_m_t		indicesFeeds;
					EngineThread::IntPolarization_m_t	idsPolarizations;

					const WORD								feeds_n = data_p->getFeedNumber();
					CFitsWriter::TFeedHeader*	feeds_p = data_p->getFeedHeader();
					for ( int indexFeed = 0; indexFeed < feeds_n; ++indexFeed ) {
						idsFeeds.insert(EngineThread::IntFeedHeader_m_t::value_type(feeds_p[indexFeed].id, feeds_p[indexFeed]));
					}

					int indexFeed = 0;
					for ( int indexSection = 0; indexSection < data_p->getSectionsNumber(); ++indexSection ) {
						for ( long indexInput = 0; indexInput < sectionHeader[indexSection].inputs; ++indexInput ) {
							IntFeedHeader_ci_m_t idFeedHeader_ci = idsFeeds.find(sectionHeader[indexSection].feed);
							if ( idFeedHeader_ci == idsFeeds.end() ) {
								ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_NOTICE, "feed Header not found! Index section: %d, feed: %d", indexSection, sectionHeader[indexSection].feed));
								throw exception();
							}

							++indexFeed;
							indicesFeeds.insert(EngineThread::IntFeedHeader_m_t::value_type(indexFeed, idFeedHeader_ci->second));
							idsPolarizations.insert(EngineThread::IntPolarization_m_t::value_type(indexFeed, sectionHeader[indexSection].polarization));
						}
					}

int indexBaseband = 0;
for ( Baseband::Baseband_ci_s_t baseband_ci = baseBands.begin();
			baseband_ci != baseBands.end(); ++baseband_ci ) {
	++indexBaseband;

	int inputs_n = 0;

	baseband_ci->getSections(sections);
	for ( Baseband::Section::Section_ci_v_t section_ci = sections.begin();
				section_ci != sections.end(); ++section_ci ) {
		inputs_n += section_ci->getInputs();

		for ( int indexInput = 0; indexInput != section_ci->getInputs(); ++indexInput ) {
			//useFeed.push_back(section_ci->getIDFeed());
			long value = 1;
			useFeed.push_back(value);

			beSects.push_back(indexBaseband);
			feedType.push_back(3);
		}
	}

	sections.clear();

	nUseFeed.push_back(inputs_n);
}

const int									refFeed	= 1;
					string										polTy;

					for ( EngineThread::IntFeedHeader_ci_m_t indexFeed_ci = indicesFeeds.begin();
								indexFeed_ci != indicesFeeds.end(); ++indexFeed_ci ) {
						feedOffX.push_back(indexFeed_ci->second.xOffset);
						feedOffY.push_back(indexFeed_ci->second.yOffset);

						EngineThread::IntPolarization_ci_m_t	idPolarization_ci = idsPolarizations.find(indexFeed_ci->first);
						if ( idPolarization_ci == idsPolarizations.end() ) throw exception();

						switch( idPolarization_ci->second ) {
							case Backends::BKND_LCP:
								polTy += "L";
								break;
							case Backends::BKND_RCP:
								polTy += "R";
								break;
							case Backends::BKND_FULL_STOKES:
								polTy += "S";
								break;
							default:
								throw exception();
								break;
						}
					}

for ( int	indexFebeFeed = 0; indexFebeFeed < febeFeed; ++indexFebeFeed ) {
	polA.push_back(0.0);
}

					bandsParameters_p = loadAntennaParameters();

					bool bandParametersFound = false;

					bandsParameters_p->First();
					for ( int index = 0; index < bandsParameters_p->recordCount(); ++index ) {
						CString id((*bandsParameters_p)["band"]->asString());

						// potrebbe esistere più di un elemento associato allo stesso ricevitore
						// dovrei considerare anche la frequenza e prendere l'elemento più vicino
						if ( receiverCode == string(id) ) {
							bandParametersFound = true;
							break;
						}

						bandsParameters_p->Next();
					}

					for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
						for ( MBFitsManager::Long_ci_v_t useFeed_ci = useFeed.begin(); useFeed_ci != useFeed.end(); ++useFeed_ci ) {
							aperEff.push_back(bandParametersFound ? (*bandsParameters_p)["apertureEfficency"]->asDouble() : -1.0);
							beamEff.push_back(bandParametersFound ? (*bandsParameters_p)["beamEfficency"]->asDouble() : -1.0);
							etafss.push_back(bandParametersFound ? (*bandsParameters_p)["forwardEfficency"]->asDouble() : -1.0);
						}
					}

					try {
						const CString antennaBossComponentName(m_configuration_p->getAntennaBossComponent());

						ACS::ROdouble_var hpbwReference;
						ACSErr::Completion_var completion;
						CORBA::Double hpbw = 0.0;

						// capire l'utilita' di memorizzare valori per ogni feed per ogni baseband
						for ( EngineThread::IntFeedHeader_ci_m_t indexFeed_ci = indicesFeeds.begin();
									indexFeed_ci != indicesFeeds.end(); ++indexFeed_ci ) {
							for ( Baseband::Baseband_ci_s_t baseband_ci = baseBands.begin();
										baseband_ci != baseBands.end(); ++baseband_ci ) {
								// 20110106: esiste un unico valore, mentre sembra piu' appropriato avere un valore legato al setup dei feed

								hpbwReference = m_antennaBoss_p->FWHM();

								hpbw = hpbwReference->get_sync(completion.out());
								if ( !isCompletionErrorFree(completion, string("EngineThread::runLoop()"),
																						string("hpbw"), antennaBossComponentName) ) {
									hpbw = 0.0;
								}

								hpbws.push_back(hpbw);
							}
						}
					} catch( CORBA::SystemException& exception_ ) {
						_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectAntennaData()");
						impl.setName(exception_._name());
						impl.setMinor(exception_.minor());
						impl.log(LM_ERROR);

						data_p->setStatus(Management::MNG_WARNING);
					}

					for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
						for ( MBFitsManager::Long_ci_v_t useFeed_ci = useFeed.begin(); useFeed_ci != useFeed.end(); ++useFeed_ci ) {
							antGain.push_back(bandParametersFound ? (*bandsParameters_p)["antennaGain"]->asDouble() : -1.0);
						}
					}

for ( MBFitsManager::Long_ci_v_t useFeed_ci = useFeed.begin(); useFeed_ci != useFeed.end(); ++useFeed_ci ) {
	for ( int	indexFeed = 0; indexFeed < *useFeed_ci; ++indexFeed ) {
		if ( bandParametersFound ) {
			tCal.push_back(0.0);
		//tCal.push_back((*bandsParameters_p)[""]->asDouble());
		} else {
			tCal.push_back(0.0);
		}
	}
}

for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
	bolCalFc.push_back(0.0);
}

					try{
						ACS::doubleSeq			frequencies;
						ACS::doubleSeq			bandwidths;
						ACS::doubleSeq			attenuations;
						ACS::longSeq				feeds;
						ACS::longSeq				initialFrequencies;

						data_p->getInputsConfiguration(feeds, initialFrequencies, frequencies, bandwidths, attenuations);

						for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
							beGain.push_back(attenuations[indexUseBand]);
						}
					} catch( CORBA::SystemException& exception_ ) {
						_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectReceiversData()");
						impl.setName(exception_._name());
						impl.setMinor(exception_.minor());
						impl.log(LM_ERROR);

						data_p->setStatus(Management::MNG_WARNING);
					}

for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
	for ( MBFitsManager::Long_ci_v_t useFeed_ci = useFeed.begin(); useFeed_ci != useFeed.end(); ++useFeed_ci ) {
		flatFiel.push_back(1.0);
	}
}
for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
	for ( MBFitsManager::Long_ci_v_t useFeed_ci = useFeed.begin(); useFeed_ci != useFeed.end(); ++useFeed_ci ) {
		boldCoff.push_back(1.0);
	}
}

					for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
						for ( MBFitsManager::Long_ci_v_t useFeed_ci = useFeed.begin(); useFeed_ci != useFeed.end(); ++useFeed_ci ) {
							gainImag.push_back(bandParametersFound ? (*bandsParameters_p)["dsbImageRatio"]->asDouble() : -1.0);
						}
					}

for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
	gainEle1.push_back(1.0);
}
for ( int	indexUseBand = 0; indexUseBand < nUseBand; ++indexUseBand ) {
	gainEle2.push_back(1.0);
}

string rxBa(80, '-');
string rxCh(80, '-');
string rxHor(80, '-');

int indexRx = 0;

for ( EngineThread::IntFeedHeader_ci_m_t indexFeed_ci = indicesFeeds.begin();
			indexFeed_ci != indicesFeeds.end(); ++indexFeed_ci ) {
	feedOffX.push_back(indexFeed_ci->second.xOffset);
	feedOffY.push_back(indexFeed_ci->second.yOffset);

	EngineThread::IntPolarization_ci_m_t	idPolarization_ci = idsPolarizations.find(indexFeed_ci->second.id);
	if ( idPolarization_ci == idsPolarizations.end() ) throw exception();

	rxBa[indexRx]		= 'P';

	switch( idPolarization_ci->second ) {
		case Backends::BKND_LCP:
			rxCh[indexRx]		= 'L';
			break;
		case Backends::BKND_RCP:
			rxCh[indexRx]		= 'R';
			break;
		case Backends::BKND_FULL_STOKES:
			rxCh[indexRx]		= 'T';
			break;
		default:
			throw exception();
			break;
	}

	rxHor[indexRx]	= indexFeed_ci->first;

	++indexRx;
}

idsPolarizations.clear();
indicesFeeds.clear();
idsFeeds.clear();

string rxBa_40(rxBa.substr( 0, 40));
string rxBa_80(rxBa.substr(40, 80));
string rxCh_40(rxCh.substr( 0, 40));
string rxCh_80(rxCh.substr(40, 80));
string rxHor_40(rxHor.substr( 0, 40));
string rxHor_80(rxHor.substr(40, 80));

					if ( bandsParameters_p ) bandsParameters_p->closeTable();
					if ( bandsParameters_p ) { delete bandsParameters_p; bandsParameters_p = NULL; }

					m_mbFitsManager_p->startScan(telescop,
																			 origin,
																			 creator,
																			 instrume,
																			 frontendBackendNames,
																			 restFreqencies,
																			 lines,
																			 bandwidths,
																			 object,
																			 timeSys,
																			 mjd,
																			 dateObs,
																			 mjd,
																			 ra,
																			 dec,
																			 radeSys,
																			 equinox,
																			 exptime,
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
																			 nUseBand,
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
																			 hpbws,
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
																			 rxHor_80);

					sections.clear();
					baseBands.clear();

					restFreqencies.clear();
					lines.clear();
					bandwidths.clear();

					phases.clear();

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
					hpbws.clear();
					antGain.clear();
					tCal.clear();
					bolCalFc.clear();
					beGain.clear();
					flatFiel.clear();
					boldCoff.clear();
					gainImag.clear();
					gainEle1.clear();
					gainEle2.clear();
				} catch( bad_alloc& exception_ ) {
					data_p->setStatus(Management::MNG_FAILURE);

					if ( bandsParameters_p ) { bandsParameters_p->closeTable(); }
					if ( bandsParameters_p ) { delete bandsParameters_p; bandsParameters_p = NULL; }

					sections.clear();
					baseBands.clear();

					restFreqencies.clear();
					lines.clear();
					bandwidths.clear();

					phases.clear();

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
					hpbws.clear();
					antGain.clear();
					tCal.clear();
					bolCalFc.clear();
					beGain.clear();
					flatFiel.clear();
					boldCoff.clear();
					gainImag.clear();
					gainEle1.clear();
					gainEle2.clear();

					m_collectThread_p->setMBFitsManager(NULL);
					if ( m_mbFitsManager_p ) { delete m_mbFitsManager_p; m_mbFitsManager_p = NULL; }

					ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "Memory allocation failure while creating or writing to MBFits file!"));

					return;
				} catch( ... ) {
					data_p->setStatus(Management::MNG_FAILURE);

					if ( bandsParameters_p ) { bandsParameters_p->closeTable(); }
					if ( bandsParameters_p ) { delete bandsParameters_p; bandsParameters_p = NULL; }

					sections.clear();
					baseBands.clear();

					restFreqencies.clear();
					lines.clear();
					bandwidths.clear();

					phases.clear();

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
					hpbws.clear();
					antGain.clear();
					tCal.clear();
					bolCalFc.clear();
					beGain.clear();
					flatFiel.clear();
					boldCoff.clear();
					gainImag.clear();
					gainEle1.clear();
					gainEle2.clear();

					m_collectThread_p->setMBFitsManager(NULL);
					if ( m_mbFitsManager_p ) { delete m_mbFitsManager_p; m_mbFitsManager_p = NULL; }

					ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "Unexpected exception while writing to MBFits file! Trying to startScan"));

					return;
				}

				ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_NOTICE, "FILE_OPENED %s", (const char *)data_p->getFileName()));
      }

      if ( m_mbFitsManager_p && m_mbFitsManager_p->isScanStarted() && data_p->isSubScanHeaderReady() ) {
				MBFitsManager::String_v_t	phaseN;

				try {
					Backends::TSectionHeader const*	sectionHeader	= data_p->getSectionHeader();

					const long scanNum		= data_p->getScanID();
					const long subScanNum	= data_p->getSubScanID();

					baseBands.clear();

					ACS::doubleSeq	skyBandwidths;
					ACS::doubleSeq	skyFrequencies;

					// verificare la necessità di questo aggiornamento
					// - risulta necessario per poter fare data_p->getSkyBandwidth, valutare se serve anche ad altro
					// verificare la necessità di aggiornamento anche dei dati Antenna
					// get data from receivers boss
					collectReceiversData();

					data_p->getSkyBandwidth(skyBandwidths);
					data_p->getSkyFrequency(skyFrequencies);

					for ( int indexSection = 0; indexSection < data_p->getSectionsNumber(); ++indexSection ) {
						for ( long indexInput = 0; indexInput < sectionHeader[indexSection].inputs; ++indexInput ) {
							Baseband::Section section(sectionHeader[indexSection], indexInput,
																				skyBandwidths[indexSection], skyFrequencies[indexSection] * 1000000);

							Baseband baseBand(sectionHeader[indexSection].bandWidth, sectionHeader[indexSection].frequency,
																skyBandwidths[indexSection], skyFrequencies[indexSection] * 1000000);

							Baseband::Baseband_i_s_t baseBand_i = baseBands.begin();
							for ( ; baseBand_i != baseBands.end(); ++baseBand_i ) {
								if ( baseBand_i->isSimilar(baseBand) ) {
									baseBand_i->insert(section);	// warning: passing ‘const Baseband’ as ‘this’ argument of ‘void Baseband::insert(const Baseband::Section&)’ discards qualifiers
									break;
								}
							}

							if ( baseBand_i == baseBands.end() ) {
								baseBand.insert(section);
								baseBands.insert(baseBand);
							}
						}
					}

					const double dut1 = data_p->getDut1();
					IRA::CDateTime	currentDatetime(currentUTC, dut1);

					stringstream dateTime;
					dateTime	<< setw(4) << setfill('0') << currentDatetime.getYear()
										<< string("-")
										<< setw(2) << setfill('0') << currentDatetime.getMonth()
										<< string("-")
										<< setw(2) << setfill('0') << currentDatetime.getDay()
										<< string("T")
										<< setw(2) << setfill('0') << currentDatetime.getHours()
										<< string(":")
										<< setw(2) << setfill('0') << currentDatetime.getMinutes()
										<< string(":")
										<< setw(2) << setfill('0') << currentDatetime.getSeconds()
										<< string(".")
										<< setw(3) << setfill('0') << currentDatetime.getMilliSeconds();

					IRA::CSite site = data_p->getSite();

					TIMEDIFFERENCE currentLST;
					currentDatetime.LST(site).getDateTime(currentLST);  // get the current LST time

const string		subsType;

					const Layout* const layoutScan_p = m_layoutCollection.find("SCAN");

					Scan scan;
					scan.setLayout(*layoutScan_p);

					stringstream layoutId;
					layoutId << scanNum << "_" << subScanNum;

					const Layout* const layout_p = m_layoutCollection.find(layoutId.str());
					if ( layout_p ) {
						scan.setLayout(*layout_p);
					}

					stringstream	usrFrame;

					string layoutValue;

					layoutValue = layoutScan_p->find("CTYPE");
					const string basisFrameCType(layoutValue);
					const string basisFrameCType1(basisFrameCType.substr(0, basisFrameCType.find('/')));
					const string basisFrameCType2(basisFrameCType.substr(basisFrameCType.find('/') + 1, basisFrameCType.length() - basisFrameCType.find('/')));

					layoutValue = layoutScan_p->find("CTYPEN");
					if ( layout_p && (string() != layout_p->find("CTYPEN")) ) {
						layoutValue = layout_p->find("CTYPEN");
					}
					const string nativeFrameCType(layoutValue);
					const string nativeFrameCType1(nativeFrameCType.substr(0, nativeFrameCType.find('/')));
					const string nativeFrameCType2(nativeFrameCType.substr(nativeFrameCType.find('/') + 1, nativeFrameCType.length() - nativeFrameCType.find('/')));

					layoutValue = layoutScan_p->find("CTYPEOFF");
					if ( layout_p && (string() != layout_p->find("CTYPEOFF")) ) {
						layoutValue = layout_p->find("CTYPEOFF");
					}
					const string offsetFrameCType(layoutValue);
					const string offsetFrameCType1(offsetFrameCType.substr(0, offsetFrameCType.find('/')));
					const string offsetFrameCType2(offsetFrameCType.substr(offsetFrameCType.find('/') + 1, offsetFrameCType.length() - offsetFrameCType.find('/')));

					if ( string("RA/DEC") == basisFrameCType ) {
						usrFrame << "EQ";
					} else if ( string("GLON/GLAT") == basisFrameCType ) {
						usrFrame << "GA";
					} else if ( string("ALON/ALAT") == basisFrameCType ) {
						usrFrame << "HO";
					} else {
						usrFrame << "--";
					}

					if ( string("RA/DEC") == nativeFrameCType ) {
						usrFrame << "EQ";
					} else if ( string("GLON/GLAT") == nativeFrameCType ) {
						usrFrame << "GA";
					} else if ( string("ALON/ALAT") == nativeFrameCType ) {
						usrFrame << "HO";
					} else {
						usrFrame << "--";
					}

					if ( string("RA/DEC") == offsetFrameCType ) {
						usrFrame << "EQ";
					} else if ( string("GLON/GLAT") == offsetFrameCType ) {
						usrFrame << "GA";
					} else if ( string("ALON/ALAT") == offsetFrameCType ) {
						usrFrame << "HO";
					} else {
						usrFrame << "--";
					}

const bool			dpBlock		= false;
const int 			nInts			= 1;
const bool			wobCoord	= false;
const float			dewAng		= 0.0;
const float			dewExtra	= 0.0;

const int				channels	= 1;			// Nch: number of spectral channels for this baseband. This is reserved for spectral channels - in continuum data it is set to 1

const double		freqRes		= 0.0;
const string		molecule;	
const string		transiti;

					double		restFreq	= 0.0;
					layoutValue = layoutScan_p->find("RESTFREQ");
					if ( !layoutValue.empty() ) restFreq = atof(layoutValue.c_str());

					const string		sideBand("USB");
const double		sbSep			= 0.0;

					const string		_2ctyp2("PIX-INDX");
					const int				_2crpx2		= 1;
					const int				_2crvl2		= 1;
					const int				_21cd2a		= 1;

SpectralAxis spectralAxisRestFrameMainSideband(string(),
																							 string("FREQ    "),
																							 static_cast<float>(1.0),
																							 0.0,	
																							 0.0,	
																							 string("Hz"),
																							 string(),
																							 string("TOPOCENT"));

SpectralAxis spectralAxisRestFrameImageSideband(string(),
																								string("FREQ    "),
																								static_cast<float>(1.0),
																								0.0,
																								0.0,
																								string("Hz"),
																								string(),
																								string("TOPOCENT"));

SpectralAxis spectralAxisSkyFrameMainSideband(string(),
																							string("FREQ    "),
																							static_cast<float>(1.0),
																							0.0,
																							0.0,
																							string("Hz"),
																							string(),
																							string("TOPOCENT"));

SpectralAxis spectralAxisSkyFrameImageSideband(string(),
																							 string("FREQ    "),
																							 static_cast<float>(1.0),
																							 0.0,
																							 0.0,
																							 string("Hz"),
																							 string(),
																							 string("TOPOCENT"));

SpectralAxis spectralAxisRestFrameVelocity(string(),
																					 string("VRAD    "),
																					 static_cast<float>(1.0),
																					 0.0,
																					 0.0,
																					 string("km/s"),
																					 string(),
																					 string("TOPOCENT"));

const float			_1vsou2r	= 0.0;
const float			_1vsys2r	= 0.0;

					m_mbFitsManager_p->startSubScan(frontendBackendNames,
																					baseBands,
																					scanNum,
																					subScanNum,
																					dateTime.str(),
																					usrFrame.str(),
																					currentLST.toSeconds(),
																					subsType,
																					scan,

																					dpBlock,
																					nInts,
																					wobCoord,
																					phaseN,
																					dewAng,
																					dewExtra,

																					channels,
																					freqRes,
																					molecule,
																					transiti,
																					restFreq,
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
																					_1vsys2r);

					phaseN.clear();

					subScanStop = false;
				} catch( bad_alloc& exception_ ) {
					data_p->setStatus(Management::MNG_FAILURE);

					phaseN.clear();

					m_collectThread_p->setMBFitsManager(NULL);
					if ( m_mbFitsManager_p ) { delete m_mbFitsManager_p; m_mbFitsManager_p = NULL; }

					ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "Memory allocation failure while creating or writing to MBFits file!"));

					return;
				} catch( ... ) {
					data_p->setStatus(Management::MNG_FAILURE);

					phaseN.clear();

					m_collectThread_p->setMBFitsManager(NULL);
					if ( m_mbFitsManager_p ) { delete m_mbFitsManager_p; m_mbFitsManager_p = NULL; }

					ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "Unexpected exception while writing to MBFits file! Trying to startSubScan"));

					return;
				}

				m_collectThread_p->setMBFitsManager(m_mbFitsManager_p);

				m_collectThread_p->collectMeteo();
				m_collectThread_p->collectTracking();
				m_collectThread_p->processData();

				data_p->startRunningStage();

        ACS_LOG(LM_FULL_INFO, "CEngineThread::runLoop()", (LM_NOTICE, "SUBSCAN STARTED"));
      }
    } else if ( data_p->isRunning() ) {		// file was already created... then saves the data into it until there is somthing to process and there is still time available...
			try {
				if ( !subScanStop ) {
					if ( m_mbFitsManager_p ) {
						while ( checkTime(currentUTC.value().value) &&
										checkTimeSlot(currentUTC.value().value) &&
										processData(frontendBackendNames, baseBands, basisFrameCType, nativeFrameCType, moveFram, longObj, latObj) );
					}

					if ( data_p->isStop() ) {
						// save all the data in the buffer an then finalize the file
						if ( m_mbFitsManager_p ) {
							while( processData(frontendBackendNames, baseBands, basisFrameCType, nativeFrameCType, moveFram, longObj, latObj) );
						}

						if ( m_mbFitsManager_p ) {
							m_collectThread_p->collectMeteo();
							m_collectThread_p->collectTracking();
							m_collectThread_p->processData();
						}

						m_collectThread_p->setMBFitsManager(NULL);

						if ( m_mbFitsManager_p ) {
							m_mbFitsManager_p->endSubScan(frontendBackendNames);
						}

						baseBands.clear();
						subScanStop = true;

						ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_NOTICE, "SUBSCAN_FILES_FINALIZED"));

						data_p->haltStopStage();
					}
				}
			} catch( ... ) {
				ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "Unexpected exception trying to stopSubScan!"));
			}
    }
  } else {
		try {
			if ( m_mbFitsManager_p ) {
				m_mbFitsManager_p->endScan();
				if ( m_mbFitsManager_p ) { delete m_mbFitsManager_p; m_mbFitsManager_p = NULL; }

				// data_p->haltStopStage();

				ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_NOTICE, "SCAN_FILES_FINALIZED"));
			}
		} catch( ... ) {
			ACS_LOG(LM_FULL_INFO, "EngineThread::runLoop()", (LM_ERROR, "Unexpected exception trying to stopScan!"));
		}
	}
}

bool EngineThread::checkTime( const ACS::Time& currentTime_ ) const {
	CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();

	// gives the cache time to fill a little bit
	return (currentTime_ > (data_p->getFirstDumpTime() + getSleepTime() + m_timeSlice));
}

bool EngineThread::checkTimeSlot( const ACS::Time& slotStart_ ) const {
/*
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::TimeInterval slot=m_timeSlice; // gives the slot of time to finalize the job
	ACS::TimeInterval duration=now.value().value-slotStart_;
	if (duration<slot) return true;
	else return false;
*/

	return true;
}

bool EngineThread::isCompletionErrorFree( ACSErr::Completion_var& completion_,
																					const string methodName_,
																					const string attributeName_,
																					const CString& componentName_ ) const {
	if ( !ACSErr::CompletionImpl(completion_).isErrorFree() ) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl, impl, ACSErr::CompletionImpl(completion_), methodName_.c_str());
		impl.setAttributeName(attributeName_.c_str());
		impl.setComponentName(static_cast<const char *>(componentName_));
		impl.log(LM_ERROR);

		CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
		data_p->setStatus(Management::MNG_WARNING);

		return false;
	}

	return true;
}

void EngineThread::collectAntennaData() {
	CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();

	const CString antennaBossComponentName(m_configuration_p->getAntennaBossComponent());

	try {
		bool antennaBossError	= false;

		CCommonTools::getAntennaBoss(m_antennaBoss_p, m_containerServices_p, antennaBossComponentName, antennaBossError);
	} catch( ComponentErrors::CouldntGetComponentExImpl& exception_ ) {
		data_p->setStatus(Management::MNG_WARNING);

		exception_.log(LM_ERROR);

		try {
			CCommonTools::unloadAntennaBoss(m_antennaBoss_p, m_containerServices_p);
		} catch( ACSErr::ACSbaseExImpl& exception_ ) {
			exception_.log(LM_WARNING);
		}

		m_antennaBoss_p = Antenna::AntennaBoss::_nil();

		throw exception_;
	}

	try {
		ACS::ROstring_var targetReference;

		ACS::ROdouble_var rightAscensionReference;
		ACS::ROdouble_var declinationReference;

		ACS::ROdouble_var vlsrReference;

		ACS::ROdouble_var azimuthOffsetReference;
		ACS::ROdouble_var elevationOffsetReference;

		ACS::ROdouble_var rightAscensionOffsetReference;
		ACS::ROdouble_var declinationOffsetReference;

		ACS::ROdouble_var longitudeOffsetReference;
		ACS::ROdouble_var latitudeOffsetReference;

		// let's take the references to the attributes
		targetReference								= m_antennaBoss_p->target();

		rightAscensionReference				= m_antennaBoss_p->targetRightAscension();
		declinationReference					= m_antennaBoss_p->targetDeclination();

		vlsrReference									= m_antennaBoss_p->targetVlsr();

		azimuthOffsetReference				= m_antennaBoss_p->azimuthOffset();
		elevationOffsetReference			= m_antennaBoss_p->elevationOffset();

		rightAscensionOffsetReference	= m_antennaBoss_p->rightAscensionOffset();
		declinationOffsetReference		= m_antennaBoss_p->declinationOffset();

		longitudeOffsetReference			= m_antennaBoss_p->longitudeOffset();
		latitudeOffsetReference				= m_antennaBoss_p->latitudeOffset();

		ACSErr::Completion_var completion;

		CORBA::String_var target;
		CString						sourceName						= "";

		CORBA::Double 		rightAscension				= 0.0;
		CORBA::Double 		declination						= 0.0;

		CORBA::Double 		vlsr									= 0.0;

		CORBA::Double 		azimuthOffset					= 0.0;
		CORBA::Double 		elevationOffset				= 0.0;

		CORBA::Double 		rightAscensionOffset	= 0.0;
		CORBA::Double 		declinationOffset			= 0.0;

		CORBA::Double 		longitudeOffset				= 0.0;
		CORBA::Double 		latitudeOffset				= 0.0;

		// let's take the values of the attributes
		target								= targetReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("target"), antennaBossComponentName) ) {
			sourceName						= "";
		} else {
			sourceName						= static_cast<const char *>(target);
		}

		rightAscension				= rightAscensionReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetRightAscension"), antennaBossComponentName) ) {
			rightAscension				= 0.0;
		}

		declination						= declinationReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetDeclination"), antennaBossComponentName) ) {
			declination						= 0.0;
		}

		vlsr									= vlsrReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetVLsr"), antennaBossComponentName) ) {
			vlsr									= 0.0;
		}

		azimuthOffset					= azimuthOffsetReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetAzimuthOffset"), antennaBossComponentName) ) {
			azimuthOffset					= 0.0;
		}

		elevationOffset				= elevationOffsetReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetElevationOffset"), antennaBossComponentName) ) {
			elevationOffset				= 0.0;
		}

		rightAscensionOffset	= rightAscensionOffsetReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetRightAscensionOffset"), antennaBossComponentName) ) {
			rightAscensionOffset	= 0.0;
		}

		declinationOffset			= declinationOffsetReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetDeclinationOffset"), antennaBossComponentName) ) {
			declinationOffset			= 0.0;
		}

		longitudeOffset				= longitudeOffsetReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetLongitudeOffset"), antennaBossComponentName) ) {
			longitudeOffset				= 0.0;
		}

		latitudeOffset				= latitudeOffsetReference->get_sync(completion.out());
		if ( !isCompletionErrorFree(completion, string("EngineThread::collectAntennaData()"),
																string("targetLatitudeOffset"), antennaBossComponentName) ) {
			latitudeOffset				= 0.0;
		}

		data_p->setSource(sourceName, rightAscension, declination, vlsr);
		data_p->setAntennaOffsets(azimuthOffset, elevationOffset,
															rightAscensionOffset, declinationOffset,
															longitudeOffset, latitudeOffset);
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectAntennaData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);
	}
}

void EngineThread::collectReceiversData() {
	CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();

	const CString receiversBossComponentName(m_configuration_p->getReceiversBossComponent());

	try {
		bool receiversBossError = false;

		CCommonTools::getReceiversBoss(m_receiversBoss_p, m_containerServices_p, receiversBossComponentName, receiversBossError);
	} catch( ComponentErrors::CouldntGetComponentExImpl& exception_ ) {
		data_p->setStatus(Management::MNG_WARNING);

		exception_.log(LM_ERROR);

		try {
			CCommonTools::unloadReceiversBoss(m_receiversBoss_p, m_containerServices_p);
		} catch( ACSErr::ACSbaseExImpl& exception_ ) {
			exception_.log(LM_WARNING);
		}

		m_receiversBoss_p = Receivers::ReceiversBoss::_nil();

		throw exception_;
	}

	ACSErr::Completion_var	completion;

	try {	// get the receiver code
		ACS::ROstring_var			receiverCodeReference;
		CORBA::String_var			receiverCode;

		receiverCodeReference												= m_receiversBoss_p->actualSetup();
		receiverCode																= receiverCodeReference->get_sync(completion.out());

		if ( isCompletionErrorFree(completion, string("EngineThread::collectReceiversData()"),
															 string("receiverCode"), receiversBossComponentName) ) {
			data_p->setReceiverCode(static_cast<const char *>(receiverCode));
		} else {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl, impl, ACSErr::CompletionImpl(completion), "EngineThread::collectReceiversData()");

			impl.setAttributeName("receiverCode");
			impl.setComponentName(static_cast<const char *>(m_configuration_p->getReceiversBossComponent()));
			impl.log(LM_ERROR);

			data_p->setStatus(Management::MNG_WARNING);
			data_p->setReceiverCode("");
		}
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectReceiversData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->setReceiverCode("");
	}

	try {	// get the local oscillator
		ACS::ROdoubleSeq_var	localOscillatorsReference;
		ACS::doubleSeq_var		localOscillators;

		localOscillatorsReference										= m_receiversBoss_p->LO();
		localOscillators														= localOscillatorsReference->get_sync(completion.out());

		if ( isCompletionErrorFree(completion, string("EngineThread::collectReceiversData()"),
															 string("localOscillator"), receiversBossComponentName) ) {
			data_p->setLocalOscillator(localOscillators.in());
		} else {
			data_p->setLocalOscillator();
		}
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectReceiversData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->setLocalOscillator();
	}

	try {	// get the bandwidth for each intermediate frequency
		ACS::ROdoubleSeq_var	bandwidthsIntermediateFrequenciesReference;
		ACS::doubleSeq_var		bandwidthsIntermediateFrequencies;

		bandwidthsIntermediateFrequenciesReference	= m_receiversBoss_p->bandWidth();
		bandwidthsIntermediateFrequencies						= bandwidthsIntermediateFrequenciesReference->get_sync(completion.out());

		if ( isCompletionErrorFree(completion, string("EngineThread::collectReceiversData()"),
															 string("bandwidthIntermediateFrequency"), receiversBossComponentName) ) {
			data_p->setReceiverBandWidth(bandwidthsIntermediateFrequencies.in());
		} else {
			data_p->setReceiverBandWidth();
		}
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectReceiversData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->setReceiverBandWidth();
	}

	try {	// get the band initial frequency for each intemediate frequency of the receiver
		ACS::ROdoubleSeq_var	initialFrequenciesReference;
		ACS::doubleSeq_var		initialFrequencies;

		initialFrequenciesReference									= m_receiversBoss_p->initialFrequency();
		initialFrequencies													= initialFrequenciesReference->get_sync(completion.out());

		if ( isCompletionErrorFree(completion, string("EngineThread::collectReceiversData()"),
																string("initialFrequency"), receiversBossComponentName) ) {
			data_p->setReceiverInitialFrequency(initialFrequencies.in());
		} else {
			data_p->setReceiverInitialFrequency();
		}
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectReceiversData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->setReceiverInitialFrequency();
	}

	try {	// get the local polarization for each intemediate frequency of the receiver
		ACS::ROlongSeq_var		polarizationsReference;
		ACS::longSeq_var			polarizations;

		polarizationsReference											= m_receiversBoss_p->polarization();
		polarizations																= polarizationsReference->get_sync(completion.out());

		if ( isCompletionErrorFree(completion, string("EngineThread::collectReceiversData()"),
															 string("polarization"), receiversBossComponentName) ) {
			data_p->setReceiverPolarization(polarizations.in());
		} else {
			data_p->setReceiverPolarization();
		}
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectReceiversData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->setReceiverPolarization();
	}

	CFitsWriter::TFeedHeader* feedHeaders_p = NULL;
	try {	// get the feeds geometry
		ACS::doubleSeq_var	xOffsets;
		ACS::doubleSeq_var	yOffsets;
		ACS::doubleSeq_var	relativePowers;

		long feeds_n = static_cast<long>(m_receiversBoss_p->getFeeds(xOffsets.out(), yOffsets.out(), relativePowers.out()));
		feedHeaders_p = new CFitsWriter::TFeedHeader[feeds_n];

		for ( long indexFeed = 0; indexFeed < feeds_n; ++indexFeed ) {
			feedHeaders_p[indexFeed].id							= indexFeed;
			feedHeaders_p[indexFeed].xOffset				= xOffsets[indexFeed];
			feedHeaders_p[indexFeed].yOffset				= yOffsets[indexFeed];
			feedHeaders_p[indexFeed].relativePower	= relativePowers[indexFeed];
		}

		data_p->saveFeedHeader(feedHeaders_p, feeds_n);
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectReceiversData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->saveFeedHeader(NULL, 0);
	} catch ( ComponentErrors::ComponentErrorsEx& exception_ ) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl, impl, exception_, "EngineThread::collectReceiversData()");
		impl.setOperationName("getFeeds()");
		impl.setComponentName(static_cast<const char *>(receiversBossComponentName));
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->saveFeedHeader(NULL,0);
	}

	try {	// get the calibration marks values
		ACS::doubleSeq_var	calibrationMarks;
		ACS::doubleSeq			frequencies;
		ACS::doubleSeq			bandwidths;
		ACS::doubleSeq			attenuations;
		ACS::longSeq				feeds;
		ACS::longSeq				initialFrequencies;
		ACS::doubleSeq_var	skyFrequencies;
		ACS::doubleSeq_var	skyBandwidths;

		data_p->getInputsConfiguration(feeds, initialFrequencies, frequencies, bandwidths, attenuations);

		calibrationMarks = m_receiversBoss_p->getCalibrationMark(frequencies, bandwidths, feeds, initialFrequencies, skyFrequencies.out(), skyBandwidths.out());

		data_p->setCalibrationMarks(calibrationMarks.in());
		data_p->setSkyFrequency(skyFrequencies.in());
		data_p->setSkyBandwidth(skyBandwidths.in());
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::collectReceiversData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->setCalibrationMarks();
		data_p->setSkyFrequency();
		data_p->setSkyBandwidth();
	} catch ( ComponentErrors::ComponentErrorsEx& exception_ ) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl, impl, exception_, "EngineThread::collectReceiversData()");
		impl.setOperationName("getCalibrationMark()");
		impl.setComponentName(static_cast<const char *>(receiversBossComponentName));
		impl.log(LM_ERROR);

		data_p->setStatus(Management::MNG_WARNING);

		data_p->setCalibrationMarks();
		data_p->setSkyFrequency();
		data_p->setSkyBandwidth();
	}
}

bool EngineThread::processData( const MBFitsManager::FeBe_v_t& frontendBackendNames_,
																const Baseband::Baseband_s_t& baseBands_,
																const string& basisFrameCType_,
																const string& nativeFrameCType_,
																const bool	 moveFram_,
																double longObj_,
																double latObj_ ) {
	CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();

	// verificare la necessità di questo aggiornamento
	// verificare la necessità di aggiornamento anche dei dati Antenna
	// get data from receivers boss
	collectReceiversData();

	// verificare chi pulisce i buffer seguenti
	ACS::Time time;
	bool			calOn					= false;
	char*			bufferCopy_p	= NULL;		// pointer to the memory that has to be freed
	char*			buffer_p			= NULL;		// pointer to the buffer that contains the real data
	bool			tracking			= false;
	long			bufferSize		= 0;
	if ( !data_p->getDump(time, calOn, bufferCopy_p, buffer_p, tracking, bufferSize) ) return false;
//return true;

	TIMEVALUE timeValue;
	timeValue.value(time);
//	CFitsWriter::TDataHeader dataHeader;
//	dataHeader.time = CDateTime(timeValue, data_p->getDut1()).getMJD();
	IRA::CDateTime dateTime(timeValue, data_p->getDut1());
	const double	 mjdData = dateTime.getMJD();

	const long inputs_n = data_p->getInputsNumber();
	double		 tsys[inputs_n];
	MBFitsWriter_private::getTsysFromBuffer(buffer_p, inputs_n, tsys);

	double				apparentAz		= 0.0;
	double				apparentEl		= 0.0;
	double				apparentRa		= 0.0;
	double				apparentDec		= 0.0;
	double				apparentEpoch	= 0.0;
	double				apparentLon		= 0.0;
	double				apparentLat		= 0.0;

	const int			phase			= 1;
	double				longOff		= 0.0;
	double				latOff		= 0.0;
	double				azimuth		= 0.0;
	double				elevatio	= 0.0;
	double				ra				= 0.0;
	double				dec				= 0.0;
	double				lon				= 0.0;
	double				lat				= 0.0;

	double				cBasLong	= 0.0;
	double				cBasLat		= 0.0;

	const double	basLong		= 0.0;
	const double	basLat		= 0.0;
	const double	rotAngle	= 0.0;
	const double	mCRVal1		= 0.0;
	const double	mCRVal2		= 0.0;
	const double	mLonPole	= 0.0;
	const double	mLatPole	= 0.0;
	const double	dFocus_x	= 0.0;
	const double	dFocus_y	= 0.0;
	const double	dFocus_z	= 0.0;
	const double	dPhi_x		= 0.0;
	const double	dPhi_y		= 0.0;
	const double	dPhi_z		= 0.0;
	const double	wobDisLN	= 0.0;
	const double	wobDisLT	= 0.0;

	try {
		bool antennaBossError = false;
		CCommonTools::getAntennaBoss(m_antennaBoss_p, m_containerServices_p, m_configuration_p->getAntennaBossComponent(), antennaBossError);

		m_antennaBoss_p->getApparentCoordinates(time, apparentAz, apparentEl, apparentRa, apparentDec, apparentEpoch, apparentLon, apparentLat);

		// integration is multiplied by 10000 because internally we have the value in millesec while the methods requires 100ns.
		m_antennaBoss_p->getObservedEquatorial(time,	data_p->getIntegrationTime() * 10000, ra,				dec);
		m_antennaBoss_p->getObservedGalactic(time,		data_p->getIntegrationTime() * 10000, lon,			lat);
		m_antennaBoss_p->getObservedHorizontal(time,	data_p->getIntegrationTime() * 10000, azimuth,	elevatio);
	} catch( ComponentErrors::CouldntGetComponentExImpl& exception_ ) {
//		_IRA_LOGFILTER_LOG_EXCEPTION(exception_, LM_ERROR);

		data_p->setStatus(Management::MNG_FAILURE);
	} catch( CORBA::SystemException& exception_ ) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "EngineThread::processData()");
		impl.setName(exception_._name());
		impl.setMinor(exception_.minor());
//		_IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);

		data_p->setStatus(Management::MNG_FAILURE);
	} catch( ... ) {
		_EXCPT(ComponentErrors::UnexpectedExImpl, impl, "EngineThread::processData()");
//		_IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);

		data_p->setStatus(Management::MNG_FAILURE);
	}

	if ( string("RA/DEC") == basisFrameCType_ ) {
		double raJ2000	= 0.0;
		double decJ2000	= 0.0;
		CSkySource::apparentToJ2000(apparentRa, apparentDec, dateTime, raJ2000, decJ2000);

		cBasLong	= raJ2000;
		cBasLat		= decJ2000;
	} else if ( string("GLON/GLAT") == basisFrameCType_ ) {
		cBasLong	= apparentLon;
		cBasLat		= apparentLat;
	} else if ( string("ALON/ALAT") == basisFrameCType_ ) {
		cBasLong	= apparentAz;
		cBasLat		= apparentEl;
	} else {
		//... non gestito ...
	}

	if ( string("RA/DEC") == nativeFrameCType_ ) {
		longOff		= ra	- longObj_;
		latOff		= dec	- latObj_;
	} else if ( string("GLON/GLAT") == nativeFrameCType_ ) {
		longOff		= lon	- longObj_;
		latOff		= lat	- latObj_;
	} else if ( string("ALON/ALAT") == nativeFrameCType_ ) {
		if ( moveFram_ ) {
			IRA::CDateTime dateTime(timeValue, data_p->getDut1());

			IRA::CSkySource skySource;
			skySource.setInputEquatorial(longObj_, latObj_, IRA::CSkySource::SS_J2000);
			skySource.process(dateTime, data_p->getSite());
			skySource.getApparentHorizontal(longObj_, latObj_);
		}

		longOff		= azimuth		- longObj_;
		latOff		= elevatio	- latObj_;
	} else {
		//... non gestito ...
	}

	longOff = longOff * cos(latObj_);

	const double parAngle = IRA::CSkySource::paralacticAngle(dateTime, data_p->getSite(), azimuth, elevatio);

	TIMEDIFFERENCE lstData;
	IRA::CDateTime(timeValue, data_p->getDut1()).LST(data_p->getSite()).getDateTime(lstData);  // get the LST time

	for ( MBFitsManager::FeBe_ci_v_t frontendBackendName_ci = frontendBackendNames_.begin();
				frontendBackendName_ci != frontendBackendNames_.end(); ++frontendBackendName_ci ) {
		try {
			m_mbFitsManager_p->integrationParameters(*frontendBackendName_ci,
																							 mjdData,
																							 lstData.toSeconds(),
																							 data_p->getIntegrationTime() / 1000,	// internally we have the value of integration in millesec
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
																							 wobDisLT);
		} catch( ... ) {
			_EXCPT(ManagementErrors::FitsCreationErrorExImpl, impl, "EngineThread::processData()");
			impl.setFileName(static_cast<const char *>(data_p->getFileName()));
//		impl.setError(m_file_p->getLastError());
			impl.setError("MBFitsWriter error!");

//		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);

			data_p->setStatus(Management::MNG_FAILURE);

			return false;
		}

		long indexBaseband = 0;
		for ( Baseband::Baseband_ci_s_t baseBand_ci = baseBands_.begin();
					baseBand_ci != baseBands_.end(); ++baseBand_ci ) {
			++indexBaseband;

			Baseband::Section::Section_v_t sections;
			baseBand_ci->getSections(sections);

			vector<float> basebandData;

			for ( Baseband::Section::Section_ci_v_t section_ci = sections.begin(); section_ci != sections.end(); ++section_ci ) {
				const long sectionID				= section_ci->getID();

				const long polarizations_n	= data_p->SectionPolNumber(sectionID);
				const long bins_n						= data_p->getSectionBins(sectionID);
				const long channels_n				= polarizations_n * bins_n;

				switch( data_p->getSampleSize() ) {
					case sizeof(BYTE2_TYPE): {
							BYTE2_TYPE channel[channels_n];
							MBFitsWriter_private::getChannelFromBuffer<BYTE2_TYPE>(sectionID, polarizations_n, bins_n, buffer_p, channel);
							for ( long indexChannel = 0; indexChannel < channels_n; ++indexChannel ) { basebandData.push_back(channel[indexChannel]); }
						}
						break;
					case sizeof(BYTE4_TYPE): {
							BYTE4_TYPE channel[channels_n];
							MBFitsWriter_private::getChannelFromBuffer<BYTE4_TYPE>(sectionID, polarizations_n, bins_n, buffer_p, channel);
							for ( long indexChannel = 0; indexChannel < channels_n; ++indexChannel ) { basebandData.push_back(channel[indexChannel]); }
						}
						break;
					case sizeof(BYTE8_TYPE): {
							BYTE8_TYPE channel[channels_n];
							MBFitsWriter_private::getChannelFromBuffer<BYTE8_TYPE>(sectionID, polarizations_n, bins_n, buffer_p, channel);
							for ( long indexChannel = 0; indexChannel < channels_n; ++indexChannel ) { basebandData.push_back(channel[indexChannel]); }
						}
						break;
				}
			}

			sections.clear();

			try {
				m_mbFitsManager_p->integration(*frontendBackendName_ci,
																			 indexBaseband,
																			 mjdData,
																			 basebandData);
			} catch( ... ) {
				_EXCPT(ManagementErrors::FitsCreationErrorExImpl, impl, "EngineThread::processData()");
				impl.setFileName(static_cast<const char *>(data_p->getFileName()));
	//		impl.setError(m_file_p->getLastError());
				impl.setError("MBFitsWriter error!");

	//		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);

				data_p->setStatus(Management::MNG_FAILURE);

				basebandData.clear();

				return false;
			}

			basebandData.clear();

			stringstream tsysKeyword;
			tsysKeyword << "TSYS" << "_" << *frontendBackendName_ci << "_" << indexBaseband;

			MBFitsManager::Double_v_t tsysFeBeBaseband;
			for ( Baseband::Section::Section_ci_v_t section_ci = sections.begin(); section_ci != sections.end(); ++section_ci ) {
				tsysFeBeBaseband.push_back(tsys[section_ci->getID()]);
			}

			m_mbFitsManager_p->monitor(mjdData,
																 tsysKeyword.str(),
																 tsysFeBeBaseband);

			tsysFeBeBaseband.clear();
			tsysKeyword.str(string());
		}
	}

	if ( bufferCopy_p ) { delete[] bufferCopy_p; bufferCopy_p = NULL; }

	return true;
}

CDBTable* EngineThread::loadTimeData() const {
	MBFitsManager::String_v_t					fields;
	EngineThread::StringFieldType_m_t	fieldsTypes;

	if ( fields.empty() || fieldsTypes.empty() ) {
		fields.clear();
		fieldsTypes.clear();

		string fieldName;

		fieldName = string("timeSys");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::STRING));
		fieldName = string("tai2utc");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("et2utc");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("gps2tai");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
	}

	IRA::CDBTable* timeData_p = NULL;

	try {
		IRA::CError error;
		error.Reset();

		timeData_p = new IRA::CDBTable(m_containerServices_p, "TimeParameters", "DataBlock/TimeData");

		for ( MBFitsManager::String_ci_v_t field_ci = fields.begin(); field_ci != fields.end(); ++field_ci ) {
			EngineThread::StringFieldType_ci_m_t fieldType_ci = fieldsTypes.find(*field_ci);
			if ( fieldType_ci == fieldsTypes.end() ) {
				throw exception();
			}

			if ( !timeData_p->addField(error, fieldType_ci->first.c_str(), fieldType_ci->second) ) {
				string errorMessage("Error adding field " + fieldType_ci->first);
				error.setExtra(errorMessage.c_str(), 0);
			}
		}

		fields.clear();
		fieldsTypes.clear();

		timeData_p->openTable(error);
	} catch( std::bad_alloc& exception_ ) {
		if ( timeData_p ) timeData_p->closeTable();
		if ( timeData_p ) { delete timeData_p; timeData_p = NULL; }

		fields.clear();
		fieldsTypes.clear();

		_EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "EngineThread::runLoop()");
		throw dummy;
	}

	return timeData_p;
}

CDBTable* EngineThread::loadAntennaParameters() const {
	MBFitsManager::String_v_t					fields;
	EngineThread::StringFieldType_m_t	fieldsTypes;

	if ( fields.empty() || fieldsTypes.empty() ) {
		fields.clear();
		fieldsTypes.clear();

		string fieldName;

		fieldName = string("band");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::STRING));
		fieldName = string("apertureEfficency");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("beamEfficency");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("forwardEfficency");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("HPBW");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("antennaGain");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("calibrationTemperature_LCP");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("calibrationTemperature_RCP");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("dsbImageRatio");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("gainPolynomParameters_a");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("gainPolynomParameters_b");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("gainPolynomParameters_c");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
	}

	IRA::CDBTable* bandsParameters_p = NULL;

	try {
		IRA::CError error;
		error.Reset();

		bandsParameters_p = new IRA::CDBTable(m_containerServices_p, "BandParameters", "DataBlock/AntennaParameters");

		for ( MBFitsManager::String_ci_v_t field_ci = fields.begin(); field_ci != fields.end(); ++field_ci ) {
			EngineThread::StringFieldType_ci_m_t fieldType_ci = fieldsTypes.find(*field_ci);
			if ( fieldType_ci == fieldsTypes.end() ) {
				throw exception();
			}

			if ( !bandsParameters_p->addField(error, fieldType_ci->first.c_str(), fieldType_ci->second) ) {
				string errorMessage("Error adding field " + fieldType_ci->first);
				error.setExtra(errorMessage.c_str(), 0);
			}
		}

		fields.clear();
		fieldsTypes.clear();

		bandsParameters_p->openTable(error);
	} catch( std::bad_alloc& exception_ ) {
		if ( bandsParameters_p ) bandsParameters_p->closeTable();
		if ( bandsParameters_p ) { delete bandsParameters_p; bandsParameters_p = NULL; }

		fields.clear();
		fieldsTypes.clear();

		_EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "EngineThread::runLoop()");
		throw dummy;
	}

	return bandsParameters_p;
}

CDBTable* EngineThread::loadPointingModelCoefficients() const {
	MBFitsManager::String_v_t					fields;
	EngineThread::StringFieldType_m_t	fieldsTypes;

	if ( fields.empty() || fieldsTypes.empty() ) {
		fields.clear();
		fieldsTypes.clear();

		string fieldName;

		fieldName = string("ia");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("ie");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hasa");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("haca");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hese");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hece");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hasa2");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("haca2");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa2");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("heca2");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("haca3");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("heca3");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa3");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("npae");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("ca");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("an");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("aw");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("hece2");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hece6");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa4");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa5");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hsca");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hsca2");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hssa3");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hsca5");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("nrx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("nry");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("hysa");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hyse");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("setLinX");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("setLinY");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("setLinZ");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("setRotX");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("setRotY");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("setRotZ");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("moveFoc");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::LONGLONG));
		fieldName = string("focAmp");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("focFreq");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("focPhase");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("iaRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("ieRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hasaRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hacaRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("heseRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("heceRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesaRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hasa2Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("haca2Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa2Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("heca2Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("haca3Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("heca3Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa3Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("npaeRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("caRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("anRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("awRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));

		fieldName = string("hece2Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hece6Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa4Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hesa5Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hscaRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hsca2Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hssa3Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("hsca5Rx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("nRxRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
		fieldName = string("nRyRx");
		fields.push_back(fieldName);
		fieldsTypes.insert(map<string, CDataField::TFieldType>::value_type(fieldName, CDataField::DOUBLE));
	}

	IRA::CDBTable* pointingModelCoefficients = NULL;

	try {
		IRA::CError error;
		error.Reset();

		pointingModelCoefficients = new IRA::CDBTable(m_containerServices_p, "PointingCoefficients", "DataBlock/PointingParameters");

		for ( MBFitsManager::String_ci_v_t field_ci = fields.begin(); field_ci != fields.end(); ++field_ci ) {
			EngineThread::StringFieldType_ci_m_t fieldType_ci = fieldsTypes.find(*field_ci);
			if ( fieldType_ci == fieldsTypes.end() ) {
				throw exception();
			}

			if ( !pointingModelCoefficients->addField(error, fieldType_ci->first.c_str(), fieldType_ci->second) ) {
				string errorMessage("Error adding field " + fieldType_ci->first);
				error.setExtra(errorMessage.c_str(), 0);
			}
		}

		fields.clear();
		fieldsTypes.clear();

		pointingModelCoefficients->openTable(error);
	} catch( std::bad_alloc& exception_ ) {
		if ( pointingModelCoefficients ) pointingModelCoefficients->closeTable();
		if ( pointingModelCoefficients ) { delete pointingModelCoefficients; pointingModelCoefficients = NULL; }

		fields.clear();
		fieldsTypes.clear();

		_EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "EngineThread::runLoop()");
		throw dummy;
	}

	return pointingModelCoefficients;
}

void EngineThread::setConfiguration( CConfiguration* const configuration_p_ ) {
	m_configuration_p			= configuration_p_;
}

void EngineThread::setServices( maci::ContainerServices* const containerServices_p_ ) {
	m_containerServices_p	= containerServices_p_;
}

void EngineThread::setCollectorThread( MBFitsWriter_private::CCollectorThread* const collectThread_p_ ) {
	m_collectThread_p = collectThread_p_;
}

void EngineThread::setTimeSlice( const long timeSlice_ ) {
	m_timeSlice						= timeSlice_;
}

void EngineThread::setLayoutCollection( const LayoutCollection& layoutCollection_ ) {
	m_layoutCollection = layoutCollection_;
}
