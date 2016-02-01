#include <iostream>

#include "MBFitsManager.h"

#include <DateTime.h>
#include <SkySource.h>

#define PI 3.141592653589793238462643383279502884197169399375105820974944592

double rad2Deg( const double value_ ) {
	return (value_ * 180.0 / PI);
}

double deg2Rad( const double value_ ) {
	return (value_ * PI / 180.0);
}

void startScan( MBFitsManager& mbFitsManager_,
								MBFitsManager::String_v_t& febes_,
								int& nUseBand_ ) {
	const string											telescop("Medicina");
	const string											origin("IRA- INAF");
	const string											creator("ESCS 0.2");

	//										const string											comment;

	//										const string											hierarchEsoDprCatg;
	//										const string											hierarchEsoDprType;
	//										const string											hierarchEsoDprTech;
	//										const string											hierarchEsoObsProgID;
	//										const int													hierarchEsoObsID;
	//										const double											hierarchEsoTelAirmStart;
	//										const double											hierarchEsoTelAirmEnd;
	//										const double											hierarchEsoTelAlt;
	//										const double											hierarchEsoTelAz;

	const string											instrume("MEDHET");

	febes_.push_back("     CCC-Backend");

	MBFitsManager::Double_v_t					restFreqencies;
	restFreqencies.push_back(0.0);
	MBFitsManager::String_v_t					lines;
	lines.push_back(string());
	MBFitsManager::Double_v_t					bandwidths;
	bandwidths.push_back(150.0);
	bandwidths.push_back(150.0);
	const string											object("3C286");
	const string											timeSys("UTC");
	const double											mjdObs		= 56167.6176303127;
	const string											dateObs("2012-08-28T14:49:23.259");
	const double											mjd				= 56167.6176303127;
	const double											ra				= 3.53925778693173;
	const double											dec				= 0.532485216525134;
	const string											radeSys("FK5");
	const float												equinox		= 2000.0;
	const double											exptime		= 0.0;
	const string											origFile;
	const string											projID("libardi");
	const int													scanNum		= 1;

	const double											siteLong	= deg2Rad(11.64693056);
	const double											siteLat		= deg2Rad(44.52048889);
	const float												siteElev	= 28.0;
	const float												diameter	= 32.0;
	const string											obsID("libardi");
	const double											lst				= 20959493.355;
	const double											utc2Ut1		= 0.0;
	const double											tai2Utc		= 0.0;
	const double											etUtc			= 32.184;
	const double											gpsTai		= -19.0;
	const string											wcsName("ABSOLUTE EQUATORIAL");
	const string											cType1("RA---SFL");
	const string											cType2("DEC---SFL");
	const double											crVal1		= 0.0;
	const double											crVal2		= 0.0;
	const string											cType1n("RA---SFL");
	const string											cType2n("DEC---SFL");
	const double											lonPole		= 0.0;
	const double											latPole		= 90.0;
	const double											bLongObj	= 3.53925778693173;
	const double											bLatObj		= 0.532485216525134;
	const double											longObj		= 3.53925778693173;
	const double											latObj		= 0.532485216525134;
	const double											patLong		= 0.0;
	const double											patLat		= 0.0;
	const string											calCode;
	const bool												moveFram	= false;
	const double											periDate	= 0.0;
	const double											periDist	= 0.0;
	const double											longAsc		= 0.0;
	const double											omega			= 0.0;
	const double											inclinat	= 0.0;
	const double											eccentr		= 0.0;
	const double											orbEpoch	= 0.0;
	const double											orbEqnox	= 0.0;
	const double											distance	= 0.0;

	const Scan												scan(Scan::typePoint,
																				 Scan::modeOTF,
																				 Scan::geometryLine,
																				 Scan::directionEquatorialLongitude,
																				 1,
																				 1,
																				 0.625,
																				 1.0,
																				 0.05,
																				 12.5,
																				 0.0,
																				 0.0,
																				 0.0,
																				 0.0,
																				 string("O"),
																				 true,
																				 ("RA---SFL"),
																				 ("DEC--SFL"));

	const double											tranDist	= 0.0;
	const double											tranFreq	= 0.0;
	const double											tranFocu	= 0.0;
	const bool												wobUsed		= false;
	const double											wobThrow	= 0.0;
	const string											wobDir;
	const float												wobCycle	= 0.0;
	const string											wobMode;
	const string											wobPatt;

	MBFitsManager::String_v_t					phases;
	phases.push_back("NONE");

	const int													nFebe			= 1;
	const float												pDeltaIA	= 0.0;
	const float												pDeltaCA	= 0.0;
	const float												pDeltaIE	= 0.0;
	const float												fDeltaIA	= 0.0;
	const float												fDeltaCA	= 0.0;
	const float												fDeltaIE	= 0.0;
	const float												ia				= 0.0;
	const float												ie				= 0.0;
	const float												hasa			= 0.0;
	const float												haca			= 0.0;
	const float												hese			= 0.0;
	const float												hece			= 0.0;
	const float												hesa			= 0.0;
	const float												hasa2			= 0.0;
	const float												haca2			= 0.0;
	const float												hesa2			= 0.0;
	const float												heca2			= 0.0;
	const float												haca3			= 0.0;
	const float												heca3			= 0.0;
	const float												hesa3			= 0.0;
	const float												npae			= 0.0;
	const float												ca				= 0.0;
	const float												an				= 0.0;
	const float												aw				= 0.0;
	const float												hece2			= 0.0;
	const float												hece6			= 0.0;
	const float												hesa4			= 0.0;
	const float												hesa5			= 0.0;
	const float												hsca			= 0.0;
	const float												hsca2			= 0.0;
	const float												hssa3			= 0.0;
	const float												hsca5			= 0.0;
	const float												nrx				= 0.0;
	const float												nry				= 0.0;
	const float												hysa			= 0.0;
	const float												hyse			= 0.0;
	const double											setLinX		= 0.0;
	const double											setLinY		= 0.0;
	const double											setLinZ		= 0.0;
	const double											setRotX		= 0.0;
	const double											setRotY		= 0.0;
	const double											setRotZ		= 0.0;
	const double											moveFoc		= 0.0;
	const double											focAmp		= 0.0;
	const double											focFreq		= 0.0;
	const double											focPhase	= 0.0;

	const string											dewCabin;
	const string											dewRtMod;
	const float												dewUser		= 0.0;
	const float												dewZero		= 0.0;
	const string											location;
	const string											optPath;
	const int													nOptRefl	= 0.0;
	const int													febeBand	= 2;
	const int													febeFeed	= 2;
	nUseBand_	= 2;
	const string											fdTypCod("3: UNKNOWN");
	const float												feGain		= 0.0;
	const string											swtchMod;
	const int													nPhases		= 1;
	const float												frThrwLo	= 0.0;
	const float												frThrwHi	= 0.0;
	const float												tBlank		= 0.0;
	const float												tSync			= 0.0;
	const float												iaRx			= 0.0;
	const float												ieRx			= 0.0;
	const float												hasaRx		= 0.0;
	const float												hacaRx		= 0.0;
	const float												heseRx		= 0.0;
	const float												heceRx		= 0.0;
	const float												hesaRx		= 0.0;
	const float												hasa2Rx		= 0.0;
	const float												haca2Rx		= 0.0;
	const float												hesa2Rx		= 0.0;
	const float												heca2Rx		= 0.0;
	const float												haca3Rx		= 0.0;
	const float												heca3Rx		= 0.0;
	const float												hesa3Rx		= 0.0;
	const float												npaeRx		= 0.0;
	const float												caRx			= 0.0;
	const float												anRx			= 0.0;
	const float												awRx			= 0.0;
	const float												hece2Rx		= 0.0;
	const float												hece6Rx		= 0.0;
	const float												hesa4Rx		= 0.0;
	const float												hesa5Rx		= 0.0;
	const float												hscaRx		= 0.0;
	const float												hsca2Rx		= 0.0;
	const float												hssa3Rx		= 0.0;
	const float												hsca5Rx		= 0.0;
	const float												nRxRx			= 0.0;
	const float												nRyRx			= 0.0;
	const string											sigOnln;
	const string											refOnln;
	const string											sigPol;
	const string											refPol;

	MBFitsManager::Long_v_t						nUseFeed;
	MBFitsManager::Long_v_t						useFeed;
	MBFitsManager::Long_v_t						useBand;
	for ( int indexUseBand = 0; indexUseBand < nUseBand_; ++indexUseBand ) {
		useBand.push_back(indexUseBand);
		nUseFeed.push_back(1);
		for ( int indexUseFeed = 0; indexUseFeed < nUseFeed.at(indexUseBand); ++indexUseFeed ) {
			useFeed.push_back(indexUseFeed + 1);
		}
	}

	MBFitsManager::Long_v_t						beSects;
	beSects.push_back(1);
	beSects.push_back(2);

	MBFitsManager::Long_v_t						feedType;
	for ( int indexUseBand = 0; indexUseBand < nUseBand_; ++indexUseBand ) {
		for ( int indexUseFeed = 0; indexUseFeed < nUseFeed.at(indexUseBand); ++indexUseFeed ) {
			feedType.push_back(3);
		}
	}

	MBFitsManager::Double_v_t					feedOffX;
	MBFitsManager::Double_v_t					feedOffY;
	for ( int indexFebeFeed = 0; indexFebeFeed < febeFeed; ++indexFebeFeed ) {
		feedOffX.push_back(0.0);
		feedOffY.push_back(0.0);
	}

	const int													refFeed		= 1;
	const string											polTy("LR");

	MBFitsManager::Float_v_t					polA;
	for ( int indexFebeFeed = 0; indexFebeFeed < febeFeed; ++indexFebeFeed ) {
		polA.push_back(0.0);
	}

	MBFitsManager::Float_v_t					aperEff;
	MBFitsManager::Float_v_t					beamEff;
	MBFitsManager::Float_v_t					etafss;
	MBFitsManager::Float_v_t					hpbw;
	MBFitsManager::Float_v_t					antGain;
	MBFitsManager::Float_v_t					tCal;
	MBFitsManager::Float_v_t					bolCalFc;
	MBFitsManager::Float_v_t					beGain;
	MBFitsManager::Float_v_t					flatFiel;
	MBFitsManager::Float_v_t					boldCoff;
	MBFitsManager::Float_v_t					gainImag;
	MBFitsManager::Float_v_t					gainEle1;
	MBFitsManager::Float_v_t					gainEle2;
	for ( int indexUseBand = 0; indexUseBand < nUseBand_; ++indexUseBand ) {
		for ( int indexFebeFeed = 0; indexFebeFeed < febeFeed; ++indexFebeFeed ) {
			aperEff.push_back(0.58);
			beamEff.push_back(0.71);
			etafss.push_back(0.88);
			hpbw.push_back(0.1251138);
			antGain.push_back(0.16);
			tCal.push_back(0.0);
			flatFiel.push_back(1.0);
			boldCoff.push_back(1.0);
			gainImag.push_back(-18.0);
		}

		bolCalFc.push_back(0.0);
		beGain.push_back(1.0);

		gainEle1.push_back(1.0);
		gainEle2.push_back(1.0);
	}

	const string rxBa_40;
	const string rxBa_80;
	const string rxCh_40;
	const string rxCh_80;
	const string rxHor_40;
	const string rxHor_80;

	mbFitsManager_.startScan(	telescop,
														origin,
														creator,

//														comment,
//
//														hierarchEsoDprCatg,
//														hierarchEsoDprType,
//														hierarchEsoDprTech,
//														hierarchEsoObsProgID,
//														hierarchEsoObsID,
//														hierarchEsoTelAirmStart,
//														hierarchEsoTelAirmEnd,
//														hierarchEsoTelAlt,
//														hierarchEsoTelAz,

														instrume,
														febes_,
														restFreqencies,
														lines,
														bandwidths,
														object,
														timeSys,
														mjdObs,
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
														nUseBand_,
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
														rxHor_80);
}

void startSubScan( MBFitsManager& mbFitsManager_,
									 const MBFitsManager::String_v_t& febes_,
									 const int nUseBand_ ) {
	Baseband::Baseband_s_t		baseBands;
	for ( int indexBaseband = 0; indexBaseband < nUseBand_; ++indexBaseband ) {
		const double bandwidth				= 300.0;
		const double frequency				= 4900.0;
//		const long	 IFPolarization	= indexBaseband;

		const double skyBandwidth			= 150.0;
		const double skyFrequency			= 5000.0;

		Baseband baseband(bandwidth, frequency * 1000000, skyBandwidth, skyFrequency * 1000000);

		baseBands.insert(baseband);
	}

	const int							scanNum		= 1;
	const int							subSNum		= 1;
//	const int							obsNum;
	const string					dateObs("2012-08-28T14:49:23.259");
	const string					usrFrame("EQEQEQ");

	const double					lst				= 20959493.355;
	const string					subsType("O");
//	const string					obsType;

	const Scan						scan(Scan::typePoint,
														 Scan::modeOTF,
														 Scan::geometryLine,
														 Scan::directionEquatorialLongitude,
														 1,
														 1,
														 0.625,
														 1.0,
														 0.05,
														 12.5,
														 0.0,
														 0.0,
														 0.0,
														 0.0,
														 string("O"),
														 true,
														 ("RA---SFL"),
														 ("DEC--SFL"));

	const bool						dpBlock		= false;
	const int 						nInts			= 1;
	const bool						wobCoord	= true;

	MBFitsManager::String_v_t	phaseN;
	phaseN.push_back("NONE");

	const float						dewAng		= 0.0;
	const float						dewExtra	= 0.0;

	const int							channels	= 1;
	const double					freqRes		= 0.0;

	const string					molecule;
	const string					transiti;
	const double					restFreq	= 5000000000.0;

	const string					sideBand("USB");
	const double					sbSep			= 0.0;
	const string					_2ctyp2("PIX-INDX");
	const int							_2crpx2		= 1;
	const int							_2crvl2		= 1;
	const int							_21cd2a		= 1;

	const SpectralAxis spectralAxisRestFrameMainSideband(string(),
																											 string("FREQ    "),
																											 static_cast<float>(1.0),
																											 0.0,
																											 0.0,
																											 string("Hz"),
																											 string(),
																											 string("TOPOCENT"));

const SpectralAxis spectralAxisRestFrameImageSideband(string(),
																											string("FREQ    "),
																											static_cast<float>(1.0),
																											0.0,
																											0.0,
																											string("Hz"),
																											string(),
																											string("TOPOCENT"));

const SpectralAxis spectralAxisSkyFrameMainSideband(string(),
																										string("FREQ    "),
																										static_cast<float>(1.0),
																										0.0,
																										0.0,
																										string("Hz"),
																										string(),
																										string("TOPOCENT"));

const SpectralAxis spectralAxisSkyFrameImageSideband(string(),
																										 string("FREQ    "),
																										 static_cast<float>(1.0),
																										 0.0,
																										 0.0,
																										 string("Hz"),
																										 string(),
																										 string("TOPOCENT"));

const SpectralAxis spectralAxisRestFrameVelocity(string(),
																								 string("VRAD    "),
																								 static_cast<float>(1.0),
																								 0.0,
																								 0.0,
																								 string("km/s"),
																								 string(),
																								 string("TOPOCENT"));

	const float						_1vsou2r	= 0.0;
	const float						_1vsys2r	= 0.0;

	mbFitsManager_.startSubScan(febes_,
															baseBands,
															scanNum,
															subSNum,
//															obsNum,
															dateObs,
															usrFrame,

															lst,
															subsType,
//															obsType,

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
}

int main( int argc, char** argv ) {
	CMBFitsWriter::initialize();
	Scan::initialize();

	const string path("/archive/data/20120907/20120907-1500000-143000-libardi_3C286/");

	MBFitsManager::String_v_t febes;

	MBFitsManager mbFitsManager(MBFitsManager::getLastVersion());
	mbFitsManager.setPath(path);

	try {
		int												nUseBand = 0;
/*
		startScan(mbFitsManager, febes, nUseBand);

		startSubScan(mbFitsManager, febes, nUseBand);
*/
		febes.clear();

		double mjd								= atof(argv[1]);
		IRA::CDateTime dateTime(mjd + 2400000.5);

		Antenna::TSiteInformation siteInformation;
		siteInformation.longitude = deg2Rad(11.64693056);
		siteInformation.latitude	= deg2Rad(44.52048889);
		siteInformation.height		= 28.0;
		siteInformation.xP				= 0.0;
		siteInformation.yP				= 0.0;
		siteInformation.elipsoid	= 0;

		IRA::CSite site(siteInformation);

//		double azimuth						= deg2Rad(55.4527361762891);
//		double elevation					= deg2Rad(46.9777207996893);
		double azimuth						= 0.0;
		double elevation					= 0.0;

		double rightAscension			= deg2Rad(atof(argv[2]));
		double declination				= deg2Rad(atof(argv[3]));

		IRA::CSkySource skySource;
		skySource.setInputEquatorial(rightAscension, declination, IRA::CSkySource::SS_J2000);
		skySource.process(dateTime, site);
		skySource.getApparentHorizontal(azimuth, elevation);
		printf("%f %f\n", azimuth, elevation);
//		printf("%f %f\n", rad2Deg(azimuth), rad2Deg(elevation));

/*
		double ra, dec, eph, dra, ddec, parallax, rvel;
		skySource.getJ2000Equatorial(ra, dec, eph, dra, ddec, parallax, rvel);
		printf("%f %f\n", rad2Deg(ra), rad2Deg(dec));
*/
/*
		double parallatticAngle = deg2Rad(-72.7110306728735);
		IRA::CSkySource::horizontalToEquatorial(dateTime, site, azimuth, elevation, rightAscension, declination, parallatticAngle);
		printf("%f %f\n", rad2Deg(rightAscension), rad2Deg(declination));
		IRA::CSkySource::apparentToJ2000(rightAscension, declination, dateTime, rightAscension, declination);
		printf("%f %f\n", rad2Deg(rightAscension), rad2Deg(declination));
*/
	} catch(...) {
		febes.clear();

		std::cerr << "Exception!" << std::endl;
	}

	Scan::terminate();
	CMBFitsWriter::terminate();
}
