#include "MBFitsManager.h"

#define PI 3.141592653589793238462643383279502884197169399375105820974944592

#include "Scan.h"

string	MBFitsManager::m_lastVersion("1.63");

string MBFitsManager::getLastVersion() {
	return MBFitsManager::m_lastVersion;
}

/*
MBFitsManager::MBFitsManager() : m_isGrouping(true),
																 m_scanStarted(false),
																 m_path(string()),
																 m_mbFitsVersion(string()),
																 m_datasetName(string()),
																 m_scan(Scan()),
																 m_primaryHeaderGroupingTable_p(NULL),
																 m_scanTable_p(NULL),
																 m_febeParTables(MBFitsManager::FeBe_MBFitsWriter_p_m_t()),
																 m_monitorTable_p(NULL),
																 m_dataParTables(MBFitsManager::FeBe_MBFitsWriter_p_m_t()),
																 m_arrayDataTables(MBFitsManager::FeBe_Baseband_MBFitsWriter_p_m_t()) {
}
*/

/*	// Not implemented
MBFitsManager::MBFitsManager( const MBFitsManager& mbFitsManager_ ) : m_isGrouping(true),
																																			m_scanStarted(false),
																																			m_path(mbFitsManager_.m_path),
																																			m_datasetName(string()),
																																			m_scan(mbFitsManager_.m_scan),
																																			... {
}
*/

MBFitsManager::MBFitsManager( const string& mbFitsVersion_ ) : m_isGrouping(true),
																															 m_scanStarted(false),
																															 m_path(string()),
																															 m_mbFitsVersion(mbFitsVersion_),
																															 m_datasetName(string()),
																															 m_scan(Scan()),
																															 m_primaryHeaderGroupingTable_p(NULL),
																															 m_scanTable_p(NULL),
																															 m_febeParTables(MBFitsManager::FeBe_MBFitsWriter_p_m_t()),
																															 m_monitorTable_p(NULL),
																															 m_dataParTables(MBFitsManager::FeBe_MBFitsWriter_p_m_t()),
																															 m_arrayDataTables(MBFitsManager::FeBe_Baseband_MBFitsWriter_p_m_t()) {
}

MBFitsManager::~MBFitsManager() {
	for (	MBFitsManager::FeBe_Baseband_MBFitsWriter_p_i_m_t febeArrayDataTable_i = m_arrayDataTables.begin();
				febeArrayDataTable_i != m_arrayDataTables.end(); ++febeArrayDataTable_i ) {
		for ( MBFitsManager::Baseband_MBFitsWriter_p_i_m_t arrayDataTable_i = febeArrayDataTable_i->second.begin();
					arrayDataTable_i != febeArrayDataTable_i->second.end(); ++arrayDataTable_i ) {
			if ( arrayDataTable_i->second ) arrayDataTable_i->second->close();
			if ( arrayDataTable_i->second ) { delete arrayDataTable_i->second; arrayDataTable_i->second = NULL; }
		}
		febeArrayDataTable_i->second.clear();
	}
	m_arrayDataTables.clear();

	for (	MBFitsManager::FeBe_MBFitsWriter_p_i_m_t dataParTable_i = m_dataParTables.begin();
				dataParTable_i != m_dataParTables.end(); ++dataParTable_i ) {
		if ( dataParTable_i->second ) dataParTable_i->second->close();
		if ( dataParTable_i->second ) { delete dataParTable_i->second; dataParTable_i->second = NULL; }
	}
	m_dataParTables.clear();

	if ( m_monitorTable_p								) m_monitorTable_p->close();
	if ( m_monitorTable_p								) { delete m_monitorTable_p;								m_monitorTable_p								= NULL; }

	for ( MBFitsManager::FeBe_MBFitsWriter_p_i_m_t febeParTable_i = m_febeParTables.begin();
				febeParTable_i != m_febeParTables.end(); ++febeParTable_i ) {
		if ( febeParTable_i->second ) febeParTable_i->second->close();
		if ( febeParTable_i->second ) { delete febeParTable_i->second; febeParTable_i->second = NULL; }
	}
	m_febeParTables.clear();

	if ( m_scanTable_p									) m_scanTable_p->close();
	if ( m_scanTable_p									) { delete m_scanTable_p;										m_scanTable_p										= NULL; }

	if ( m_primaryHeaderGroupingTable_p ) m_primaryHeaderGroupingTable_p->close();
	if ( m_primaryHeaderGroupingTable_p ) { delete m_primaryHeaderGroupingTable_p;	m_primaryHeaderGroupingTable_p	= NULL; }

	m_scanStarted = false;
}

bool MBFitsManager::operator==( const MBFitsManager& mbFitsManager_ ) const {
	return false;

/*	// Not implemented
	return ( (m_isGrouping	== mbFitsManager_.m_isGrouping ) &&
					 (m_scanStarted == mbFitsManager_.m_scanStarted) &&
           (m_path				== mbFitsManager_.m_path			 ) &&
					 (m_datasetName	== mbFitsManager_.m_datasetName) &&
					 (m_scan				== mbFitsManager_.m_scan			 ) &&
...
);
*/
}

bool MBFitsManager::operator!=( const MBFitsManager& mbFitsManager_ ) const {
	return !(*this == mbFitsManager_);
}

/*	// Not implemented
MBFitsManager& MBFitsManager::operator=( const MBFitsManager& mbFitsManager_ ) {
		// handle self assignment
	if ( *this != mbFitsWriterTable_ ) {
		m_isGrouping	= mbFitsManager_.m_isGrouping;
		m_scanStarted = mbFitsManager_.m_scanStarted;
		m_path				= mbFitsManager_.m_path;
		m_datasetName	= mbFitsManager_.m_datasetName;
		m_scan				= mbFitsManager_.m_scan;

		...
	}

	//assignment operator
	return *this;
}
*/

bool MBFitsManager::isScanStarted() const {
  return m_scanStarted;
}

string MBFitsManager::getPath() const {
	return m_path;
}

void MBFitsManager::setPath( const string& path_ ) {
	m_path				= path_;

	m_datasetName	= m_path;
	size_t posSep = m_datasetName.rfind('/', m_datasetName.length() - 2);
	if ( string::npos != posSep ) {
		m_datasetName = m_datasetName.substr(posSep + 1, m_datasetName.length() - posSep - 2);
	}
}

void MBFitsManager::startScan( const string&										telescop_,
															 const string&										origin_,
															 const string&										creator_,

//															 const string&										comment_,

//															 const string&										hierarchEsoDprCatg_,
//															 const string&										hierarchEsoDprType_,
//															 const string&										hierarchEsoDprTech_,
//															 const string&										hierarchEsoObsProgID_,
//															 const int												hierarchEsoObsID_,
//															 const double											hierarchEsoTelAirmStart_,
//															 const double											hierarchEsoTelAirmEnd_,
//															 const double											hierarchEsoTelAlt_,
//															 const double											hierarchEsoTelAz_,

															 const string&										instrume_,
															 const MBFitsManager::String_v_t&	febes_,
															 const MBFitsManager::Double_v_t&	restFreqencies_,
															 const MBFitsManager::String_v_t&	lines_,
															 const MBFitsManager::Double_v_t&	bandwidths_,
															 const string&										object_,
															 const string&										timeSys_,
															 const double											mjdObs_,
															 const string&										dateObs_,
															 const double											mjd_,
															 const double											ra_,
															 const double											dec_,
															 const string&										radeSys_,
															 const float											equinox_,
															 const double											exptime_,
															 const string&										origFile_,
															 const string&										projID_,
															 const int												scanNum_,

															 const double											siteLong_,
															 const double											siteLat_,
															 const float											siteElev_,
															 const float											diameter_,
															 const string&										obsID_,
															 const double											lst_,
															 const double											utc2Ut1_,
															 const double											tai2Utc_,
															 const double											etUtc_,
															 const double											gpsTai_,
															 const string&										wcsName_,
															 const string&										cType1_,
															 const string&										cType2_,
															 const double											crVal1_,
															 const double											crVal2_,
															 const string&										cType1n_,
															 const string&										cType2n_,
															 const double											lonPole_,
															 const double											latPole_,
															 const double											bLongObj_,
															 const double											bLatObj_,
															 const double											longObj_,
															 const double											latObj_,
															 const double											patLong_,
															 const double											patLat_,
															 const string&										calCode_,
															 const bool												moveFram_,
															 const double											periDate_,
															 const double											periDist_,
															 const double											longAsc_,
															 const double											omega_,
															 const double											inclinat_,
															 const double											eccentr_,
															 const double											orbEpoch_,
															 const double											orbEqnox_,
															 const double											distance_,
															 const Scan&											scan_,
															 const double											tranDist_,
															 const double											tranFreq_,
															 const double											tranFocu_,
															 const bool												wobUsed_,
															 const double											wobThrow_,
															 const string&										wobDir_,
															 const float											wobCycle_,
															 const string&										wobMode_,
															 const string&										wobPatt_,
															 const MBFitsManager::String_v_t&	phases_,
															 const int												nFebe_,
															 const float											pDeltaIA_,
															 const float											pDeltaCA_,
															 const float											pDeltaIE_,
															 const float											fDeltaIA_,
															 const float											fDeltaCA_,
															 const float											fDeltaIE_,
															 const float											ia_,
															 const float											ie_,
															 const float											hasa_,
															 const float											haca_,
															 const float											hese_,
															 const float											hece_,
															 const float											hesa_,
															 const float											hasa2_,
															 const float											haca2_,
															 const float											hesa2_,
															 const float											heca2_,
															 const float											haca3_,
															 const float											heca3_,
															 const float											hesa3_,
															 const float											npae_,
															 const float											ca_,
															 const float											an_,
															 const float											aw_,
															 const float											hece2_,
															 const float											hece6_,
															 const float											hesa4_,
															 const float											hesa5_,
															 const float											hsca_,
															 const float											hsca2_,
															 const float											hssa3_,
															 const float											hsca5_,
															 const float											nrx_,
															 const float											nry_,
															 const float											hysa_,
															 const float											hyse_,
															 const double											setLinX_,
															 const double											setLinY_,
															 const double											setLinZ_,
															 const double											setRotX_,
															 const double											setRotY_,
															 const double											setRotZ_,
															 const double											moveFoc_,
															 const double											focAmp_,
															 const double											focFreq_,
															 const double											focPhase_,

															 const string&											dewCabin_,
															 const string&											dewRtMod_,
															 const float												dewUser_,
															 const float												dewZero_,
															 const string&											location_,
															 const string&											optPath_,
															 const int													nOptRefl_,
															 const int													febeBand_,
															 const int													febeFeed_,
															 const int													nUseBand_,
															 const string&											fdTypCod_,
															 const float												feGain_,
															 const string&											swtchMod_,
															 const int													nPhases_,
															 const float												frThrwLo_,
															 const float												frThrwHi_,
															 const float												tBlank_,
															 const float												tSync_,
															 const float												iaRx_,
															 const float												ieRx_,
															 const float												hasaRx_,
															 const float												hacaRx_,
															 const float												heseRx_,
															 const float												heceRx_,
															 const float												hesaRx_,
															 const float												hasa2Rx_,
															 const float												haca2Rx_,
															 const float												hesa2Rx_,
															 const float												heca2Rx_,
															 const float												haca3Rx_,
															 const float												heca3Rx_,
															 const float												hesa3Rx_,
															 const float												npaeRx_,
															 const float												caRx_,
															 const float												anRx_,
															 const float												awRx_,
															 const float												hece2Rx_,
															 const float												hece6Rx_,
															 const float												hesa4Rx_,
															 const float												hesa5Rx_,
															 const float												hscaRx_,
															 const float												hsca2Rx_,
															 const float												hssa3Rx_,
															 const float												hsca5Rx_,
															 const float												nRxRx_,
															 const float												nRyRx_,
															 const string&											sigOnln_,
															 const string&											refOnln_,
															 const string&											sigPol_,
															 const string&											refPol_,

															 const MBFitsManager::Long_v_t&			useBand_,
															 const MBFitsManager::Long_v_t&			nUseFeed_,
															 const MBFitsManager::Long_v_t&			useFeed_,
															 const MBFitsManager::Long_v_t&			beSects_,
															 const MBFitsManager::Long_v_t&			feedType_,
															 const MBFitsManager::Double_v_t&		feedOffX_,
															 const MBFitsManager::Double_v_t&		feedOffY_,
															 const int													refFeed_,
															 const string&											polTy_,
															 const MBFitsManager::Float_v_t&		polA_,
															 const MBFitsManager::Float_v_t&		aperEff_,
															 const MBFitsManager::Float_v_t&		beamEff_,
															 const MBFitsManager::Float_v_t&		etafss_,
															 const MBFitsManager::Float_v_t&		hpbw_,
															 const MBFitsManager::Float_v_t&		antGain_,
															 const MBFitsManager::Float_v_t&		tCal_,
															 const MBFitsManager::Float_v_t&		bolCalFc_,
															 const MBFitsManager::Float_v_t&		beGain_,
															 const MBFitsManager::Float_v_t&		flatFiel_,
															 const MBFitsManager::Float_v_t&		boldCoff_,
															 const MBFitsManager::Float_v_t&		gainImag_,
															 const MBFitsManager::Float_v_t&		gainEle1_,
															 const MBFitsManager::Float_v_t&		gainEle2_,
															 const string& rxBa_40_,
															 const string& rxBa_80_,
															 const string& rxCh_40_,
															 const string& rxCh_80_,
															 const string& rxHor_40_,
															 const string& rxHor_80_ ) {

	m_scan = scan_;

	m_primaryHeaderGroupingTable_p = new CMBFitsWriterTable(NULL, CMBFitsWriter::Grouping);
	m_primaryHeaderGroupingTable_p->setFileName(getPath() + string("GROUPING") + string(".fits"));
	m_primaryHeaderGroupingTable_p->open();

	createPrimaryHeader(telescop_,
											origin_,
											creator_,
											m_mbFitsVersion,

//											comment_,

//											hierarchEsoDprCatg_,
//											hierarchEsoDprType_,
//											hierarchEsoDprTech_,
//											hierarchEsoObsProgID_,
//											hierarchEsoObsID_,
//											hierarchEsoTelAirmStart_,
//											hierarchEsoTelAirmEnd_,
//											hierarchEsoTelAlt_,
//											hierarchEsoTelAz_,

											instrume_,
											febes_,
											restFreqencies_,
											lines_,
											bandwidths_,
											object_,
											timeSys_,
											mjdObs_,
											dateObs_,
											mjd_,
											ra_,
											dec_,
											radeSys_,
											equinox_,
											exptime_,
											origFile_,
											projID_,

											scanNum_);

	m_primaryHeaderGroupingTable_p->addTableColumns();

	m_scanTable_p = new CMBFitsWriterTable(m_primaryHeaderGroupingTable_p, CMBFitsWriter::Scan);
	m_scanTable_p->setFileName(getPath() + string("SCAN") + string(".fits"));
	m_scanTable_p->open();

	saveGroupingBinTableData(m_scanTable_p->getFullName(),
													 string("URL"),
													 m_scanTable_p->getExtName(),
													 -999,
													 string(),
													 -999);

	const long	nObs	= 0;
	const long	nSubs	= 0;

	createScanHeader(telescop_,
									 siteLong_,
									 siteLat_,
									 siteElev_,
									 diameter_,
									 projID_,
									 obsID_,
									 scanNum_,
									 timeSys_,
									 dateObs_,
									 mjd_,
									 lst_,
									 nObs,
									 nSubs,
									 utc2Ut1_,
									 tai2Utc_,
									 etUtc_,
									 gpsTai_,
									 wcsName_,
									 radeSys_,
									 equinox_,
									 cType1_,
									 cType2_,
									 crVal1_,
									 crVal2_,
									 cType1n_,
									 cType2n_,
									 lonPole_,
									 latPole_,
									 object_,
									 bLongObj_,
									 bLatObj_,
									 longObj_,
									 latObj_,
									 patLong_,
									 patLat_,
									 calCode_,
									 moveFram_,
									 periDate_,
									 periDist_,
									 longAsc_,
									 omega_,
									 inclinat_,
									 eccentr_,
									 orbEpoch_,
									 orbEqnox_,
									 distance_,
									 scan_,
									 tranDist_,
									 tranFreq_,
									 tranFocu_,
									 wobUsed_,
									 wobThrow_,
									 wobDir_,
									 wobCycle_,
									 wobMode_,
									 wobPatt_,
									 phases_,
									 nFebe_,
									 pDeltaIA_,
									 pDeltaCA_,
									 pDeltaIE_,
									 fDeltaIA_,
									 fDeltaCA_,
									 fDeltaIE_,
									 ia_,
									 ie_,
									 hasa_,
									 haca_,
									 hese_,
									 hece_,
									 hesa_,
									 hasa2_,
									 haca2_,
									 hesa2_,
									 heca2_,
									 haca3_,
									 heca3_,
									 hesa3_,
									 npae_,
									 ca_,
									 an_,
									 aw_,
									 hece2_,
									 hece6_,
									 hesa4_,
									 hesa5_,
									 hsca_,
									 hsca2_,
									 hssa3_,
									 hsca5_,
									 nrx_,
									 nry_,
									 hysa_,
									 hyse_,
									 setLinX_,
									 setLinY_,
									 setLinZ_,
									 setRotX_,
									 setRotY_,
									 setRotZ_,
									 moveFoc_,
									 focAmp_,
									 focFreq_,
									 focPhase_);

	m_scanTable_p->addTableColumns();

	CMBFitsWriterTable* febeParTable_p = NULL;
	for ( MBFitsManager::String_ci_v_t febe_ci = febes_.begin(); febe_ci != febes_.end(); ++febe_ci ) {
		febeParTable_p = new CMBFitsWriterTable(m_primaryHeaderGroupingTable_p, CMBFitsWriter::FebePar);
		febeParTable_p->setFileName(getPath() + *febe_ci + string("-") + string("FEBEPAR") + string(".fits"));
		febeParTable_p->open();

		saveGroupingBinTableData(febeParTable_p->getFullName(),
														 string("URL"),
														 febeParTable_p->getExtName(),
														 -999,
														 *febe_ci,
														 -999);

		m_febeParTables.insert(MBFitsManager::FeBe_MBFitsWriter_p_m_t::value_type(*febe_ci, febeParTable_p));

		saveScanBinTableData(*febe_ci);

		createFebeParHeader(febeParTable_p,
											  *febe_ci,
												scanNum_,
												dateObs_,
												dewCabin_,
												dewRtMod_,
												dewUser_,
												dewZero_,
												location_,
												optPath_,
												nOptRefl_,
												febeBand_,
												febeFeed_,
												nUseBand_,
												fdTypCod_,
												feGain_,
												swtchMod_,
												nPhases_,
												frThrwLo_,
												frThrwHi_,
												tBlank_,
												tSync_,
												iaRx_,
												ieRx_,
												hasaRx_,
												hacaRx_,
												heseRx_,
												heceRx_,
												hesaRx_,
												hasa2Rx_,
												haca2Rx_,
												hesa2Rx_,
												heca2Rx_,
												haca3Rx_,
												heca3Rx_,
												hesa3Rx_,
												npaeRx_,
												caRx_,
												anRx_,
												awRx_,
												hece2Rx_,
												hece6Rx_,
												hesa4Rx_,
												hesa5Rx_,
												hscaRx_,
												hsca2Rx_,
												hssa3Rx_,
												hsca5Rx_,
												nRxRx_,
												nRyRx_,
												sigOnln_,
												refOnln_,
												sigPol_,
												refPol_,
												rxBa_40_,
												rxBa_80_,
												rxCh_40_,
												rxCh_80_,
												rxHor_40_,
												rxHor_80_);

		febeParTable_p->addTableColumns();

		saveFebeParBinTableData(febeParTable_p,
														useBand_,
														nUseFeed_,
														useFeed_,
														beSects_,
														feedType_,
														feedOffX_,
														feedOffY_,
														refFeed_,
														polTy_,
														polA_,
														aperEff_,
														beamEff_,
														etafss_,
														hpbw_,
														antGain_,
														tCal_,
														bolCalFc_,
														beGain_,
														flatFiel_,
														boldCoff_,
														gainImag_,
														gainEle1_,
														gainEle2_);
	}

	m_scanStarted = true;
}

void MBFitsManager::endScan() {
	for ( MBFitsManager::FeBe_MBFitsWriter_p_i_m_t febeParTable_i = m_febeParTables.begin();
				febeParTable_i != m_febeParTables.end(); ++febeParTable_i ) {
		febeParTable_i->second->close();
	}
	m_febeParTables.clear();

	if ( m_scanTable_p									) m_scanTable_p->close();
	if ( m_scanTable_p									) { delete m_scanTable_p;										m_scanTable_p										= NULL; }

	if ( m_primaryHeaderGroupingTable_p ) m_primaryHeaderGroupingTable_p->close();
	if ( m_primaryHeaderGroupingTable_p ) { delete m_primaryHeaderGroupingTable_p;	m_primaryHeaderGroupingTable_p	= NULL; }

	m_scanStarted = false;
}

void MBFitsManager::startSubScan( const MBFitsManager::FeBe_v_t&	febeNames_,
																	const Baseband::Baseband_s_t&		baseBands_,
																	const int							scanNum_,
																	const int							subsNum_,
//																	const int							obsNum_,
																	const string&					dateObs_,
																	const string&					usrFrame_,

																	const double					lst_,
																	const string&					subsType_,
//																	const string&					obsType_,

																	const Scan&						scan_,

//																	const string&					cType1n_,
//																	const string&					cType2n_,

																	const bool						dpBlock_,
																	const int 						nInts_,
																	const bool						wobCoord_,
																	const vector<string>&	phaseN_,
																	const float						dewAng_,
																	const float						dewExtra_,

																	const int							channels_,
																	const double					freqRes_,
																	const string&					molecule_,
																	const string&					transiti_,
																	const double					restFreq_,
																	const string&					sideBand_,
																	const double					sbSep_,
																	const string&					_2ctyp2_,
																	const int							_2crpx2_,
																	const int							_2crvl2_,
																	const int							_21cd2a_,

																	// Next we assign to the specral axis a frequency and velocity description relative to the rest frame
																	// But other alternatives axes descriptions could also be given
																	const SpectralAxis& spectralAxisRestFrameMainSideband_,
																	const SpectralAxis& spectralAxisRestFrameImageSideband_,
																	const SpectralAxis& spectralAxisSkyFrameMainSideband_,
																	const SpectralAxis& spectralAxisSkyFrameImageSideband_,
																	const SpectralAxis& spectralAxisRestFrameVelocity_,

																	const float						_1vsou2r_,
																	const float						_1vsys2r_ ) {
	stringstream subPath;

	subPath << getPath() << setw(4) << setfill('0') << subsNum_ << "/";

	m_monitorTable_p = new CMBFitsWriterTable(m_primaryHeaderGroupingTable_p, CMBFitsWriter::Monitor);
	m_monitorTable_p->setFileName(subPath.str() + string("MONITOR") + string(".fits"));
	m_monitorTable_p->open();

	saveGroupingBinTableData(m_monitorTable_p->getFullName(),
													 string("URL"),
													 m_monitorTable_p->getExtName(),
													 subsNum_,
													 string(),
													 -999);

	createMonitorHeader(scanNum_,
											subsNum_,
											dateObs_,
											usrFrame_);

  m_monitorTable_p->addTableColumns();

	const string obStatus("ABORT");

	CMBFitsWriterTable* dataParTable_p		= NULL;
	CMBFitsWriterTable* arrayDataTable_p	= NULL;

	for ( MBFitsManager::FeBe_ci_v_t febeName_ci = febeNames_.begin(); febeName_ci != febeNames_.end(); ++febeName_ci ) {
		dataParTable_p = new CMBFitsWriterTable(m_primaryHeaderGroupingTable_p, CMBFitsWriter::DataPar);
		dataParTable_p->setFileName(subPath.str() + *febeName_ci + string("-") + string("DATAPAR") + string(".fits"));
		dataParTable_p->open();

		saveGroupingBinTableData(dataParTable_p->getFullName(),
														 string("URL"),
														 dataParTable_p->getExtName(),
														 subsNum_,
														 *febeName_ci,
														 -999);

		m_dataParTables.insert(MBFitsManager::FeBe_MBFitsWriter_p_m_t::value_type(*febeName_ci, dataParTable_p));

		createDataParHeader(dataParTable_p,
												scanNum_,
												subsNum_,
//												obsNum_,
												dateObs_,
												*febeName_ci,
												lst_,
												subsType_,
//												obsType_,

												m_scan,
												scan_,

//												cType1n_,
//												cType2n_,

												dpBlock_,
												nInts_,
												obStatus,
												wobCoord_,
												phaseN_,
												dewAng_,
												dewExtra_);

		dataParTable_p->addTableColumns();

		MBFitsManager::FeBe_Baseband_MBFitsWriter_p_insert_m_t  arrayDataTable_insert = m_arrayDataTables.insert(MBFitsManager::FeBe_Baseband_MBFitsWriter_p_m_t::value_type(*febeName_ci, MBFitsManager::Baseband_MBFitsWriter_p_m_t()));
		MBFitsManager::FeBe_Baseband_MBFitsWriter_p_i_m_t       arrayDataTable_i			= arrayDataTable_insert.first;

		int indexBaseband = 0;
		for ( Baseband::Baseband_ci_s_t baseBand_ci = baseBands_.begin(); baseBand_ci != baseBands_.end(); ++baseBand_ci ) {
			int nUseFeed = 0;
			Baseband::Section::Section_v_t sections;
			baseBand_ci->getSections(sections);
			for ( Baseband::Section::Section_ci_v_t section_ci = sections.begin();
						section_ci != sections.end(); ++section_ci ) {
				nUseFeed += section_ci->getInputs();
			}
			sections.clear();

			stringstream basebandName;
			basebandName << setw(2) << setfill('0') << (indexBaseband + 1);

			arrayDataTable_p = new CMBFitsWriterTable(m_primaryHeaderGroupingTable_p, CMBFitsWriter::ArrayData);
			arrayDataTable_p->setFileName(subPath.str() + *febeName_ci + string("-") + string("ARRAYDATA") + string("-") + basebandName.str() + string(".fits"));
			arrayDataTable_p->open();

			saveGroupingBinTableData(arrayDataTable_p->getFullName(),
															 string("URL"),
															 arrayDataTable_p->getExtName(),
															 subsNum_,
															 *febeName_ci,
														 	 indexBaseband + 1);

			arrayDataTable_i->second.insert(MBFitsManager::Baseband_MBFitsWriter_p_m_t::value_type(indexBaseband + 1, arrayDataTable_p));

			createArrayDataHeader(arrayDataTable_p,
														*febeName_ci,
														indexBaseband + 1,
														scanNum_,
														subsNum_,
//														obsNum_,
														dateObs_,
														channels_,
														nUseFeed,
														freqRes_,
														baseBand_ci->getSkyBandwidth(),

														molecule_,
														transiti_,
														restFreq_,
														baseBand_ci->getSkyFrequency(),
														sideBand_,
														sbSep_,
														_2ctyp2_,
														_2crpx2_,
														_2crvl2_,
														_21cd2a_,

														// Next we assign to the specral axis a frequency and velocity description relative to the rest frame
														// But other alternatives axes descriptions could also be given
														spectralAxisRestFrameMainSideband_,
														spectralAxisRestFrameImageSideband_,
														spectralAxisSkyFrameMainSideband_,
														spectralAxisSkyFrameImageSideband_,
														spectralAxisRestFrameVelocity_,

														_1vsou2r_,
														_1vsys2r_);

			arrayDataTable_p->addTableColumns();

			++indexBaseband;
		}
	}
}

void MBFitsManager::endSubScan( MBFitsManager::FeBe_v_t frontendBackendNames_ ) {
	// 20110920 - ipotesi che tutti i feed concludano positivamente il subscan
	//						considero che vengano impostati tutti come ABORTED solo nel caso di stopSchedule explicito
	//						DA IMPLEMENTARE
	const string obStatus("OK");

	for ( MBFitsManager::FeBe_ci_v_t frontendBackendName_ci = frontendBackendNames_.begin();
				frontendBackendName_ci != frontendBackendNames_.end(); ++frontendBackendName_ci ) {
		MBFitsManager::FeBe_MBFitsWriter_p_i_m_t dataParTable_i = m_dataParTables.find(*frontendBackendName_ci);
		if ( m_dataParTables.end() == dataParTable_i ) throw exception();

		updateDataParHeader(dataParTable_i->second,
												obStatus);
	}

	updateScanHeader(1, 1);

	for (	MBFitsManager::FeBe_Baseband_MBFitsWriter_p_i_m_t febeArrayDataTable_i = m_arrayDataTables.begin();
				febeArrayDataTable_i != m_arrayDataTables.end(); ++febeArrayDataTable_i ) {
		for ( MBFitsManager::Baseband_MBFitsWriter_p_i_m_t arrayDataTable_i = febeArrayDataTable_i->second.begin();
					arrayDataTable_i != febeArrayDataTable_i->second.end(); ++arrayDataTable_i ) {
			if ( arrayDataTable_i->second ) arrayDataTable_i->second->close();
			if ( arrayDataTable_i->second ) { delete arrayDataTable_i->second; arrayDataTable_i->second = NULL; }
		}
		febeArrayDataTable_i->second.clear();
	}
	m_arrayDataTables.clear();

	for (	MBFitsManager::FeBe_MBFitsWriter_p_i_m_t dataParTable_i = m_dataParTables.begin();
				dataParTable_i != m_dataParTables.end(); ++dataParTable_i ) {
		if ( dataParTable_i->second ) dataParTable_i->second->close();
		if ( dataParTable_i->second ) { delete dataParTable_i->second; dataParTable_i->second = NULL; }
	}
	m_dataParTables.clear();

	if ( m_monitorTable_p ) m_monitorTable_p->close();
	if ( m_monitorTable_p	) { delete m_monitorTable_p; m_monitorTable_p = NULL; }
}

void MBFitsManager::integration( const string&										febeName_,
																 const long												basebandID_,
																 const double											mjd_,
																 const MBFitsManager::Float_v_t&	data_ ) {
	MBFitsManager::FeBe_Baseband_MBFitsWriter_p_i_m_t	febeArrayDataTable_i	= m_arrayDataTables.find(febeName_);
	if ( m_arrayDataTables.end()						== febeArrayDataTable_i ) throw exception();

	MBFitsManager::Baseband_MBFitsWriter_p_i_m_t				arrayDataTable_i			= febeArrayDataTable_i->second.find(basebandID_);
	if ( febeArrayDataTable_i->second.end() == arrayDataTable_i			) throw exception();

	saveArrayDataBinTableData(arrayDataTable_i->second,
														mjd_,
														data_);
}

void MBFitsManager::integrationParameters( const string&	febeName_,
																					 const double		mjd_,
																					 const double		lst_,
																					 const double		integTim_,
																					 const int			phase_,
																					 const double		longOff_,
																					 const double		latOff_,
																					 const double		azimuth_,
																					 const double		elevatio_,
																					 const double		ra_,
																					 const double		dec_,
																					 const double		parAngle_,
																					 const double		cBasLong_,
																					 const double		cBasLat_,
																					 const double		basLong_,
																					 const double		basLat_,
																					 const double		rotAngle_,
																					 const double		mCRVal1_,
																					 const double		mCRVal2_,
																					 const double		mLonPole_,
																					 const double		mLatPole_,
																					 const double		dFocus_x_,
																					 const double		dFocus_y_,
																					 const double		dFocus_z_,
																					 const double		dPhi_x_,
																					 const double		dPhi_y_,
																					 const double		dPhi_z_,
																					 const double		wobDisLN_,
																					 const double		wobDisLT_ ) {
	MBFitsManager::FeBe_MBFitsWriter_p_i_m_t dataParTable_i = m_dataParTables.find(febeName_);
	if ( m_dataParTables.end() == dataParTable_i ) throw exception();

	saveDataParBinTableData(dataParTable_i->second,
													mjd_,
													lst_,
													integTim_,
													phase_,
													longOff_,
													latOff_,
													azimuth_,
													elevatio_,
													ra_,
													dec_,
													parAngle_,
													cBasLong_,
													cBasLat_,
													basLong_,
													basLat_,
													rotAngle_,
													mCRVal1_,
													mCRVal2_,
													mLonPole_,
													mLatPole_,
													dFocus_x_,
													dFocus_y_,
													dFocus_z_,
													dPhi_x_,
													dPhi_y_,
													dPhi_z_,
													wobDisLN_,
													wobDisLT_);

	updatePrimaryHeader(integTim_);
}

void MBFitsManager::monitor( const double	mjd_,
														 const string& referenceKeyword_,
														 const MBFitsManager::Double_v_t& value_,
														 const string& keyword_ ) {
	m_monitorTable_p->setColumnValue(string("MJD"),				mjd_																							);
	m_monitorTable_p->setColumnValue(string("MONPOINT"),	(!keyword_.empty() ? keyword_ : referenceKeyword_));
	m_monitorTable_p->setColumnValue(string("MONVALUE"),	value_																						);

	CMBFitsWriter* const monitorTable_p = static_cast<CMBFitsWriter*>(m_monitorTable_p);
	monitorTable_p->setColumnUnits(CMBFitsWriter::Monitor, referenceKeyword_);

	m_monitorTable_p->setRowIndex(m_monitorTable_p->getRowIndex() + 1);
}

double MBFitsManager::deg2Rad( const double value_ ) {
	return (value_ * PI / 180.0);
}

double MBFitsManager::rad2Deg( const double value_ ) {
	return (value_ * 180.0 / PI);
}

void MBFitsManager::createPrimaryHeader( const string&										telescop_,
																				 const string&										origin_,
																				 const string&										creator_,
																				 const string&										mbftsVer_,

//																				 const string&										comment_,

//																				 const string&										hierarchEsoDprCatg_,
//																				 const string&										hierarchEsoDprType_,
//																				 const string&										hierarchEsoDprTech_,
//																				 const string&										hierarchEsoObsProgID_,
//																				 const long												hierarchEsoObsID_,
//																				 const double											hierarchEsoTelAirmStart_,
//																				 const double											hierarchEsoTelAirmEnd_,
//																				 const double											hierarchEsoTelAlt_,
//																				 const double											hierarchEsoTelAz_,

																				 const string&										instrume_,
																				 const MBFitsManager::String_v_t&	febes_,
																				 const MBFitsManager::Double_v_t&	restFreqencies_,
																				 const MBFitsManager::String_v_t&	lines_,
																				 const MBFitsManager::Double_v_t&	bandwidths_,
																				 const string&										object_,
																				 const string&										timeSys_,
																				 const double											mjdObs_,
																				 const string&										dateObs_,
																				 const double											mjd_,
																				 const double											ra_,
																				 const double											dec_,
																				 const string&										radeSys_,
																				 const float											equinox_,
																				 const double											exptime_,
																				 const string&										origFile_,
																				 const string&										projID_,

																				 const long												scanNum_ ) {
	CMBFitsWriter* const primaryHeaderGroupingTable_p = static_cast<CMBFitsWriter*>(m_primaryHeaderGroupingTable_p);

	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("TELESCOP"),											telescop_								);

	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("ORIGIN"),												origin_									);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("CREATOR"),											creator_								);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("MBFTSVER"),											mbftsVer_								);

//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("COMMENT"),											comment_								);

//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO DPR CATG"),				hierarchEsoDprCatg_			);
//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO DPR TYPE"),				hierarchEsoDprType_			);
//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO DPR TECH"),				hierarchEsoDprTech_			);
//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO OBS PROG ID"),			hierarchEsoObsProgID_		);
//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO OBS ID"),					hierarchEsoObsID_				);
//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO TEL AIRM START"),	hierarchEsoTelAirmStart_);
//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO TEL AIRM END"),		hierarchEsoTelAirmEnd_	);
//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO TEL ALT"),					hierarchEsoTelAlt_			);
//	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("HIERARCH ESO TEL AZ"),					hierarchEsoTelAz_				);

	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("INSTRUME"),											instrume_								);

	long indexFebe = 0;
	for ( MBFitsManager::String_ci_v_t febe_ci = febes_.begin(); febe_ci != febes_.end(); ++febe_ci ) {
		stringstream keyword;
		keyword << "FEBE" << ++indexFebe;
		primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, keyword.str(), *febe_ci, string(), string("FEBEn"));
		keyword.str(string());
	}

	long indexRestFreq = 0;
	for ( MBFitsManager::Double_ci_v_t restFreq_ci = restFreqencies_.begin(); restFreq_ci != restFreqencies_.end(); ++restFreq_ci ) {
		stringstream keyword;
		keyword << "FREQ" << ++indexRestFreq;
		primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, keyword.str(), *restFreq_ci, string(), string("FREQn"));
		keyword.str(string());
	}

	long indexLine = 0;
	for ( MBFitsManager::String_ci_v_t line_ci = lines_.begin(); line_ci != lines_.end(); ++line_ci ) {
		stringstream keyword;
		keyword << "LINE" << ++indexLine;
		primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, keyword.str(), *line_ci, string(), string("LINEn"));
		keyword.str(string());
	}

	long indexBWid = 0;
	for ( MBFitsManager::Double_ci_v_t bWid_ci = bandwidths_.begin(); bWid_ci != bandwidths_.end(); ++bWid_ci ) {
		stringstream keyword;
		keyword << "BWID" << ++indexBWid;
		primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, keyword.str(), *bWid_ci, string(), string("BWIDn"));
		keyword.str(string());
	}

	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("OBJECT"),												object_											);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("TIMESYS"),											timeSys_										);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("MJD-OBS"),											mjdObs_											);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("DATE-OBS"),											dateObs_										);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("RA"),														MBFitsManager::rad2Deg(ra_)	);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("DEC"),													MBFitsManager::rad2Deg(dec_));
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("RADECSYS"),											radeSys_										);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("EQUINOX"),											equinox_										);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("EXPTIME"),											exptime_										);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("ORIGFILE"),											origFile_										);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("PROJID"),												projID_											);

	if ( m_isGrouping ) {
		m_primaryHeaderGroupingTable_p->addTable();

		m_primaryHeaderGroupingTable_p->setKeywordValue(string("SCANNUM"),	scanNum_);
		m_primaryHeaderGroupingTable_p->setKeywordValue(string("DATE-OBS"), dateObs_);

		primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary,	string("MBFITS"),	m_datasetName);
	}

	m_primaryHeaderGroupingTable_p->flush();
}

void MBFitsManager::updatePrimaryHeader( const double integTim_ ) {
	CMBFitsWriter* const primaryHeaderGroupingTable_p = static_cast<CMBFitsWriter*>(m_primaryHeaderGroupingTable_p);

	double expTime = 0.0;
	primaryHeaderGroupingTable_p->getKeywordValue(CMBFitsWriter::Primary, string("EXPTIME"), expTime);
	primaryHeaderGroupingTable_p->setKeywordValue(CMBFitsWriter::Primary, string("EXPTIME"), expTime + integTim_);

	m_primaryHeaderGroupingTable_p->flush();
}

void MBFitsManager::saveGroupingBinTableData( // const long		member_position_,
																							const string&	member_location_,
																							const string&	member_uri_type_,
																							const string&	extName_,
																							const long		subsNum_,
																							const string&	febe_,
																							const long		baseband_ ) {
	if ( !m_isGrouping ) return;

//	m_primaryHeaderGroupingTable_p->setColumnValue(string("MEMBER_POSITION"), member_position_);

	const string memberLocation(member_location_.substr(getPath().length(), member_location_.length() - getPath().length()));
	m_primaryHeaderGroupingTable_p->setColumnValue(string("MEMBER_LOCATION"), memberLocation	);

	m_primaryHeaderGroupingTable_p->setColumnValue(string("MEMBER_URI_TYPE"), member_uri_type_);
	m_primaryHeaderGroupingTable_p->setColumnValue(string("EXTNAME"),					extName_				);
	m_primaryHeaderGroupingTable_p->setColumnValue(string("SUBSNUM"),					subsNum_				);
	m_primaryHeaderGroupingTable_p->setColumnValue(string("FEBE"),						febe_						);
	m_primaryHeaderGroupingTable_p->setColumnValue(string("BASEBAND"),				baseband_				);

	m_primaryHeaderGroupingTable_p->setRowIndex(m_primaryHeaderGroupingTable_p->getRowIndex() + 1);

	m_primaryHeaderGroupingTable_p->flush();
}

void MBFitsManager::createScanHeader( const string&											telescop_,
																			const double											siteLong_,
																			const double											siteLat_,
																			const float												siteElev_,
																			const float												diameter_,
																			const string&											projID_,
																			const string&											obsID_,
																			const long												scanNum_,
																			const string&											timeSys_,
																			const string&											dateObs_,
																			const double											mjd_,
																			const double											lst_,
																			const long												nObs_,
																			const long												nSubs_,
																			const double											utc2Ut1_,
																			const double											tai2Utc_,
																			const double											etUtc_,
																			const double											gpsTai_,
																			const string&											wcsName_,
																			const string&											radeSys_,
																			const float												equinox_,
																			const string&											cType1_,
																			const string&											cType2_,
																			const double											crVal1_,
																			const double											crVal2_,
																			const string&											cType1n_,
																			const string&											cType2n_,
																			const double											lonPole_,
																			const double											latPole_,
																			const string&											object_,
																			const double											bLongObj_,
																			const double											bLatObj_,
																			const double											longObj_,
																			const double											latObj_,
																			const double											patLong_,
																			const double											patLat_,
																			const string&											calCode_,
																			const bool												moveFram_,
																			const double											periDate_,
																			const double											periDist_,
																			const double											longAsc_,
																			const double											omega_,
																			const double											inclinat_,
																			const double											eccentr_,
																			const double											orbEpoch_,
																			const double											orbEqnox_,
																			const double											distance_,
																			const Scan&												scan_,
																			const double											tranDist_,
																			const double											tranFreq_,
																			const double											tranFocu_,
																			const bool												wobUsed_,
																			const double											wobThrow_,
																			const string&											wobDir_,
																			const float												wobCycle_,
																			const string&											wobMode_,
																			const string&											wobPatt_,
																			const MBFitsManager::String_v_t&	phases_,
																			const int													nFebe_,
																			const float												pDeltaIA_,
																			const float												pDeltaCA_,
																			const float												pDeltaIE_,
																			const float												fDeltaIA_,
																			const float												fDeltaCA_,
																			const float												fDeltaIE_,
																			const float												ia_,
																			const float												ie_,
																			const float												hasa_,
																			const float												haca_,
																			const float												hese_,
																			const float												hece_,
																			const float												hesa_,
																			const float												hasa2_,
																			const float												haca2_,
																			const float												hesa2_,
																			const float												heca2_,
																			const float												haca3_,
																			const float												heca3_,
																			const float												hesa3_,
																			const float												npae_,
																			const float												ca_,
																			const float												an_,
																			const float												aw_,
																			const float												hece2_,
																			const float												hece6_,
																			const float												hesa4_,
																			const float												hesa5_,
																			const float												hsca_,
																			const float												hsca2_,
																			const float												hssa3_,
																			const float												hsca5_,
																			const float												nrx_,
																			const float												nry_,
																			const float												hysa_,
																			const float												hyse_,
																			const double											setLinX_,
																			const double											setLinY_,
																			const double											setLinZ_,
																			const double											setRotX_,
																			const double											setRotY_,
																			const double											setRotZ_,
																			const double											moveFoc_,
																			const double											focAmp_,
																			const double											focFreq_,
																			const double											focPhase_ ) {
	CMBFitsWriter* const scanTable_p = static_cast<CMBFitsWriter*>(m_scanTable_p);

  m_scanTable_p->addTable();

  m_scanTable_p->setKeywordValue(string("TELESCOP"),	telescop_																);
  m_scanTable_p->setKeywordValue(string("SITELONG"),	MBFitsManager::rad2Deg(siteLong_)				);
  m_scanTable_p->setKeywordValue(string("SITELAT"),		MBFitsManager::rad2Deg(siteLat_)				);
  m_scanTable_p->setKeywordValue(string("SITEELEV"),	siteElev_																);
  m_scanTable_p->setKeywordValue(string("DIAMETER"),	diameter_																);
  m_scanTable_p->setKeywordValue(string("PROJID"),		projID_																	);
  m_scanTable_p->setKeywordValue(string("OBSID"),			obsID_																	);
  m_scanTable_p->setKeywordValue(string("SCANNUM"),		scanNum_																);
  m_scanTable_p->setKeywordValue(string("TIMESYS"),		timeSys_																);
  m_scanTable_p->setKeywordValue(string("DATE-OBS"),	dateObs_																);
  m_scanTable_p->setKeywordValue(string("MJD"),				mjd_																		);
  m_scanTable_p->setKeywordValue(string("LST"),				lst_																		);
  m_scanTable_p->setKeywordValue(string("NOBS"),			nObs_																		);
  m_scanTable_p->setKeywordValue(string("NSUBS"),			nSubs_																	);
  m_scanTable_p->setKeywordValue(string("UTC2UT1"),		utc2Ut1_																);
  m_scanTable_p->setKeywordValue(string("TAI2UTC"),		tai2Utc_																);
  m_scanTable_p->setKeywordValue(string("ETUTC"),			etUtc_																	);
  m_scanTable_p->setKeywordValue(string("GPSTAI"),		gpsTai_																	);
  m_scanTable_p->setKeywordValue(string("WCSNAME"),		wcsName_																);
  m_scanTable_p->setKeywordValue(string("RADESYS"),		radeSys_																);
  m_scanTable_p->setKeywordValue(string("EQUINOX"),		equinox_																);
  m_scanTable_p->setKeywordValue(string("CTYPE1"),		cType1_																	);
  m_scanTable_p->setKeywordValue(string("CTYPE2"),		cType2_																	);
  m_scanTable_p->setKeywordValue(string("CRVAL1"),		MBFitsManager::rad2Deg(crVal1_)					);
  m_scanTable_p->setKeywordValue(string("CRVAL2"),		MBFitsManager::rad2Deg(crVal2_)					);
  m_scanTable_p->setKeywordValue(string("CTYPE1N"),		cType1n_																);
  m_scanTable_p->setKeywordValue(string("CTYPE2N"),		cType2n_																);
  m_scanTable_p->setKeywordValue(string("LONPOLE"),		MBFitsManager::rad2Deg(lonPole_)				);
  m_scanTable_p->setKeywordValue(string("LATPOLE"),		MBFitsManager::rad2Deg(latPole_)				);
  m_scanTable_p->setKeywordValue(string("OBJECT"),		object_																	);
  m_scanTable_p->setKeywordValue(string("BLONGOBJ"),	MBFitsManager::rad2Deg(bLongObj_)				);
  m_scanTable_p->setKeywordValue(string("BLATOBJ"),		MBFitsManager::rad2Deg(bLatObj_)				);
  m_scanTable_p->setKeywordValue(string("LONGOBJ"),		MBFitsManager::rad2Deg(longObj_)				);
  m_scanTable_p->setKeywordValue(string("LATOBJ"),		MBFitsManager::rad2Deg(latObj_)					);
  m_scanTable_p->setKeywordValue(string("PATLONG"),		MBFitsManager::rad2Deg(patLong_)				);
  m_scanTable_p->setKeywordValue(string("PATLAT"),		MBFitsManager::rad2Deg(patLat_)					);
  m_scanTable_p->setKeywordValue(string("CALCODE"),		calCode_																);
  m_scanTable_p->setKeywordValue(string("MOVEFRAM"),	moveFram_																);
  m_scanTable_p->setKeywordValue(string("PERIDATE"),	periDate_																);
  m_scanTable_p->setKeywordValue(string("PERIDIST"),	periDist_																);
  m_scanTable_p->setKeywordValue(string("LONGASC"),		MBFitsManager::rad2Deg(longAsc_)				);
  m_scanTable_p->setKeywordValue(string("OMEGA"),			MBFitsManager::rad2Deg(omega_)					);
  m_scanTable_p->setKeywordValue(string("INCLINAT"),	MBFitsManager::rad2Deg(inclinat_)				);
  m_scanTable_p->setKeywordValue(string("ECCENTR"),		eccentr_																);
  m_scanTable_p->setKeywordValue(string("ORBEPOCH"),	orbEpoch_																);
  m_scanTable_p->setKeywordValue(string("ORBEQNOX"),	orbEqnox_																);
  m_scanTable_p->setKeywordValue(string("DISTANCE"),	distance_																);

  m_scanTable_p->setKeywordValue(string("SCANTYPE"),	Scan::getType(scan_.getType())					);
  m_scanTable_p->setKeywordValue(string("SCANMODE"),	Scan::getMode(scan_.getMode())					);
  m_scanTable_p->setKeywordValue(string("SCANGEOM"),	Scan::getGeometry(scan_.getGeometry())	);
  m_scanTable_p->setKeywordValue(string("SCANDIR"),		Scan::getDirection(scan_.getDirection()));
  m_scanTable_p->setKeywordValue(string("SCANLINE"),	scan_.getLine()													);
  m_scanTable_p->setKeywordValue(string("SCANRPTS"),	scan_.getRepeats()											);
  m_scanTable_p->setKeywordValue(string("SCANLEN"),		scan_.getLength()												);
  m_scanTable_p->setKeywordValue(string("SCANXVEL"),	scan_.getXVelocity()										);
  m_scanTable_p->setKeywordValue(string("SCANTIME"),	scan_.getTime()													);
  m_scanTable_p->setKeywordValue(string("SCANXSPC"),	scan_.getXSpace()												);
  m_scanTable_p->setKeywordValue(string("SCANYSPC"),	scan_.getYSpace()												);
  m_scanTable_p->setKeywordValue(string("SCANSKEW"),	scan_.getSkew()													);
  m_scanTable_p->setKeywordValue(string("SCANPAR1"),	scan_.getParameter1()										);
  m_scanTable_p->setKeywordValue(string("SCANPAR2"),	scan_.getParameter2()										);
  m_scanTable_p->setKeywordValue(string("CROCYCLE"),	scan_.getCROCycle()											);
  m_scanTable_p->setKeywordValue(string("ZIGZAG"),		scan_.getZigZag()												);

  m_scanTable_p->setKeywordValue(string("TRANDIST"),	tranDist_																);
  m_scanTable_p->setKeywordValue(string("TRANFREQ"),	tranFreq_																);
  m_scanTable_p->setKeywordValue(string("TRANFOCU"),	MBFitsManager::rad2Deg(tranFocu_)				);
  m_scanTable_p->setKeywordValue(string("WOBUSED"),		wobUsed_																);
  m_scanTable_p->setKeywordValue(string("WOBTHROW"),	MBFitsManager::rad2Deg(wobThrow_)				);
  m_scanTable_p->setKeywordValue(string("WOBDIR"),		wobDir_																	);
  m_scanTable_p->setKeywordValue(string("WOBCYCLE"),	wobCycle_																);
  m_scanTable_p->setKeywordValue(string("WOBMODE"),		wobMode_																);
  m_scanTable_p->setKeywordValue(string("WOBPATT"),		wobPatt_																);

	long indexPhase = 0;
	for ( vector<string>::const_iterator phase_ci = phases_.begin(); phase_ci != phases_.end(); ++phase_ci ) {
		stringstream keyword;
		keyword << "PHASE" << ++indexPhase;
		scanTable_p->setKeywordValue(CMBFitsWriter::Scan, keyword.str(), *phase_ci, string(), string("PHASEn"));
		keyword.str(string());
	}

  m_scanTable_p->setKeywordValue(string("NFEBE"),			nFebe_																	);
  m_scanTable_p->setKeywordValue(string("PDELTAIA"),	MBFitsManager::rad2Deg(pDeltaIA_)				);
  m_scanTable_p->setKeywordValue(string("PDELTACA"),	MBFitsManager::rad2Deg(pDeltaCA_)				);
  m_scanTable_p->setKeywordValue(string("PDELTAIE"),	MBFitsManager::rad2Deg(pDeltaIE_)				);
  m_scanTable_p->setKeywordValue(string("FDELTAIA"),	MBFitsManager::rad2Deg(fDeltaIA_)				);
  m_scanTable_p->setKeywordValue(string("FDELTACA"),	MBFitsManager::rad2Deg(fDeltaCA_)				);
  m_scanTable_p->setKeywordValue(string("FDELTAIE"),	MBFitsManager::rad2Deg(fDeltaIE_)				);
  m_scanTable_p->setKeywordValue(string("IA"),				MBFitsManager::rad2Deg(ia_)							);
  m_scanTable_p->setKeywordValue(string("IE"),				MBFitsManager::rad2Deg(ie_)							);
  m_scanTable_p->setKeywordValue(string("HASA"),			MBFitsManager::rad2Deg(hasa_)						);
  m_scanTable_p->setKeywordValue(string("HACA"),			MBFitsManager::rad2Deg(haca_)						);
  m_scanTable_p->setKeywordValue(string("HESE"),			MBFitsManager::rad2Deg(hese_)						);
  m_scanTable_p->setKeywordValue(string("HECE"),			MBFitsManager::rad2Deg(hece_)						);
  m_scanTable_p->setKeywordValue(string("HESA"),			MBFitsManager::rad2Deg(hesa_)						);
  m_scanTable_p->setKeywordValue(string("HASA2"),			MBFitsManager::rad2Deg(hasa2_)					);
  m_scanTable_p->setKeywordValue(string("HACA2"),			MBFitsManager::rad2Deg(haca2_)					);
  m_scanTable_p->setKeywordValue(string("HESA2"),			MBFitsManager::rad2Deg(hesa2_)					);
  m_scanTable_p->setKeywordValue(string("HECA2"),			MBFitsManager::rad2Deg(heca2_)					);
  m_scanTable_p->setKeywordValue(string("HACA3"),			MBFitsManager::rad2Deg(haca3_)					);
  m_scanTable_p->setKeywordValue(string("HECA3"),			MBFitsManager::rad2Deg(heca3_)					);
  m_scanTable_p->setKeywordValue(string("HESA3"),			MBFitsManager::rad2Deg(hesa3_)					);
  m_scanTable_p->setKeywordValue(string("NPAE"),			MBFitsManager::rad2Deg(npae_)						);
  m_scanTable_p->setKeywordValue(string("CA"),				MBFitsManager::rad2Deg(ca_)							);
  m_scanTable_p->setKeywordValue(string("AN"),				MBFitsManager::rad2Deg(an_)							);
  m_scanTable_p->setKeywordValue(string("AW"),				MBFitsManager::rad2Deg(aw_)							);
  m_scanTable_p->setKeywordValue(string("HECE2"),			MBFitsManager::rad2Deg(hece2_)					);
  m_scanTable_p->setKeywordValue(string("HECE6"),			MBFitsManager::rad2Deg(hece6_)					);
  m_scanTable_p->setKeywordValue(string("HESA4"),			MBFitsManager::rad2Deg(hesa4_)					);
  m_scanTable_p->setKeywordValue(string("HESA5"),			MBFitsManager::rad2Deg(hesa5_)					);
  m_scanTable_p->setKeywordValue(string("HSCA"),			MBFitsManager::rad2Deg(hsca_)						);
  m_scanTable_p->setKeywordValue(string("HSCA2"),			MBFitsManager::rad2Deg(hsca2_)					);
  m_scanTable_p->setKeywordValue(string("HSSA3"),			MBFitsManager::rad2Deg(hssa3_)					);
  m_scanTable_p->setKeywordValue(string("HSCA5"),			MBFitsManager::rad2Deg(hsca5_)					);
  m_scanTable_p->setKeywordValue(string("NRX"),				MBFitsManager::rad2Deg(nrx_)						);
  m_scanTable_p->setKeywordValue(string("NRY"),				MBFitsManager::rad2Deg(nry_)						);
  m_scanTable_p->setKeywordValue(string("HYSA"),			MBFitsManager::rad2Deg(hysa_)						);
  m_scanTable_p->setKeywordValue(string("HYSE"),			MBFitsManager::rad2Deg(hyse_)						);
  m_scanTable_p->setKeywordValue(string("SETLINX"),		setLinX_																);
  m_scanTable_p->setKeywordValue(string("SETLINY"),		setLinY_																);
  m_scanTable_p->setKeywordValue(string("SETLINZ"),		setLinZ_																);
  m_scanTable_p->setKeywordValue(string("SETROTX"),		MBFitsManager::rad2Deg(setRotX_)				);
  m_scanTable_p->setKeywordValue(string("SETROTY"),		MBFitsManager::rad2Deg(setRotY_)				);
  m_scanTable_p->setKeywordValue(string("SETROTZ"),		MBFitsManager::rad2Deg(setRotZ_)				);
  m_scanTable_p->setKeywordValue(string("MOVEFOC"),		moveFoc_																);
  m_scanTable_p->setKeywordValue(string("FOCAMP"),		focAmp_																	);
  m_scanTable_p->setKeywordValue(string("FOCFREQ"),		focFreq_																);
  m_scanTable_p->setKeywordValue(string("FOCPHASE"),	MBFitsManager::rad2Deg(focPhase_)				);

	if ( m_isGrouping ) {
		CMBFitsWriter* const primaryHeaderScanTable_p = static_cast<CMBFitsWriter*>(m_scanTable_p);
		primaryHeaderScanTable_p->setKeywordValue(CMBFitsWriter::Primary,	string("MBFITS"),	m_datasetName);
	}

	m_scanTable_p->flush();
}

void MBFitsManager::updateScanHeader( const long	nObs_,
																			const long	nSubs_ ) {
	CMBFitsWriter* const scanTable_p = static_cast<CMBFitsWriter*>(m_scanTable_p);

	long nObs		= 0;
	long nSubs	= 0;
	scanTable_p->getKeywordValue(CMBFitsWriter::Scan, string("NOBS"),		nObs					);
	scanTable_p->getKeywordValue(CMBFitsWriter::Scan, string("NSUBS"),	nSubs					);

	scanTable_p->setKeywordValue(CMBFitsWriter::Scan, string("NOBS"),		nObs	+ nObs_	);
	scanTable_p->setKeywordValue(CMBFitsWriter::Scan, string("NSUBS"),	nSubs	+ nSubs_);

	m_scanTable_p->flush();
}

void MBFitsManager::updateDataParHeader( CMBFitsWriterTable* const dataParTable_p_,
																				 const string& obStatus_ ) {
	dataParTable_p_->setKeywordValue(string("OBSTATUS"),	obStatus_	);

	dataParTable_p_->flush();
}

void MBFitsManager::saveScanBinTableData( const string& febe_ ) {
	m_scanTable_p->setColumnValue(string("FEBE"),				febe_									);

	m_scanTable_p->setRowIndex(m_scanTable_p->getRowIndex() + 1);

	m_scanTable_p->flush();
}

void MBFitsManager::createFebeParHeader( CMBFitsWriterTable* const febeParTable_p_,
																				 const string&	febe_,
																				 const int			scanNum_,
																				 const string&	dateObs_,
																				 const string&	dewCabin_,
																				 const string&	dewRtMod_,
																				 const float		dewUser_,
																				 const float		dewZero_,
																				 const string&	location_,
																				 const string&	optPath_,
																				 const int			nOptRefl_,
																				 const int			febeBand_,
																				 const int			febeFeed_,
																				 const int			nUseBand_,
																				 const string&	fdTypCod_,
																				 const float		feGain_,
																				 const string&	swtchMod_,
																				 const int			nPhases_,
																				 const float		frThrwLo_,
																				 const float		frThrwHi_,
																				 const float		tBlank_,
																				 const float		tSync_,
																				 const float		iaRx_,
																				 const float		ieRx_,
																				 const float		hasaRx_,
																				 const float		hacaRx_,
																				 const float		heseRx_,
																				 const float		heceRx_,
																				 const float		hesaRx_,
																				 const float		hasa2Rx_,
																				 const float		haca2Rx_,
																				 const float		hesa2Rx_,
																				 const float		heca2Rx_,
																				 const float		haca3Rx_,
																				 const float		heca3Rx_,
																				 const float		hesa3Rx_,
																				 const float		npaeRx_,
																				 const float		caRx_,
																				 const float		anRx_,
																				 const float		awRx_,
																				 const float		hece2Rx_,
																				 const float		hece6Rx_,
																				 const float		hesa4Rx_,
																				 const float		hesa5Rx_,
																				 const float		hscaRx_,
																				 const float		hsca2Rx_,
																				 const float		hssa3Rx_,
																				 const float		hsca5Rx_,
																				 const float		nRxRx_,
																				 const float		nRyRx_,
																				 const string&	sigOnln_,
																				 const string&	refOnln_,
																				 const string&	sigPol_,
																				 const string&	refPol_,
																				 const string&	rxBa_40_,
																				 const string&	rxBa_80_,
																				 const string&	rxCh_40_,
																				 const string&	rxCh_80_,
																				 const string&	rxHor_40_,
																				 const string&	rxHor_80_ ) {
  febeParTable_p_->addTable();

  febeParTable_p_->setKeywordValue(string("FEBE"),			febe_														);
  febeParTable_p_->setKeywordValue(string("SCANNUM"),		scanNum_												);
  febeParTable_p_->setKeywordValue(string("DATE-OBS"),	dateObs_												);
  febeParTable_p_->setKeywordValue(string("DEWCABIN"),	dewCabin_												);
  febeParTable_p_->setKeywordValue(string("DEWRTMOD"),	dewRtMod_												);
  febeParTable_p_->setKeywordValue(string("DEWUSER"),		MBFitsManager::rad2Deg(dewUser_));
  febeParTable_p_->setKeywordValue(string("DEWZERO"),		MBFitsManager::rad2Deg(dewZero_));
  febeParTable_p_->setKeywordValue(string("LOCATION"),	location_												);
  febeParTable_p_->setKeywordValue(string("OPTPATH"),		optPath_												);
  febeParTable_p_->setKeywordValue(string("NOPTREFL"),	nOptRefl_												);
  febeParTable_p_->setKeywordValue(string("FEBEBAND"),	febeBand_												);
  febeParTable_p_->setKeywordValue(string("FEBEFEED"),	febeFeed_												);
  febeParTable_p_->setKeywordValue(string("NUSEBAND"),	nUseBand_												);
  febeParTable_p_->setKeywordValue(string("FDTYPCOD"),	fdTypCod_												);
  febeParTable_p_->setKeywordValue(string("FEGAIN"),		feGain_													);
  febeParTable_p_->setKeywordValue(string("SWTCHMOD"),	swtchMod_												);
  febeParTable_p_->setKeywordValue(string("NPHASES"),		nPhases_												);
  febeParTable_p_->setKeywordValue(string("FRTHRWLO"),	frThrwLo_												);
  febeParTable_p_->setKeywordValue(string("FRTHRWHI"),	frThrwHi_												);
  febeParTable_p_->setKeywordValue(string("TBLANK"),		tBlank_													);
  febeParTable_p_->setKeywordValue(string("TSYNC"),			tSync_													);
  febeParTable_p_->setKeywordValue(string("IARX"),			MBFitsManager::rad2Deg(iaRx_)		);
  febeParTable_p_->setKeywordValue(string("IERX"),			MBFitsManager::rad2Deg(ieRx_)		);
  febeParTable_p_->setKeywordValue(string("HASARX"),		MBFitsManager::rad2Deg(hasa2Rx_));
  febeParTable_p_->setKeywordValue(string("HACARX"),		MBFitsManager::rad2Deg(hacaRx_)	);
  febeParTable_p_->setKeywordValue(string("HESERX"),		MBFitsManager::rad2Deg(heseRx_)	);
  febeParTable_p_->setKeywordValue(string("HECERX"),		MBFitsManager::rad2Deg(heceRx_)	);
  febeParTable_p_->setKeywordValue(string("HESARX"),		MBFitsManager::rad2Deg(hesaRx_)	);
  febeParTable_p_->setKeywordValue(string("HASA2RX"),		MBFitsManager::rad2Deg(hasa2Rx_));
  febeParTable_p_->setKeywordValue(string("HACA2RX"),		MBFitsManager::rad2Deg(haca2Rx_));
  febeParTable_p_->setKeywordValue(string("HESA2RX"),		MBFitsManager::rad2Deg(hesa2Rx_));
  febeParTable_p_->setKeywordValue(string("HECA2RX"),		MBFitsManager::rad2Deg(heca2Rx_));
  febeParTable_p_->setKeywordValue(string("HACA3RX"),		MBFitsManager::rad2Deg(haca3Rx_));
  febeParTable_p_->setKeywordValue(string("HECA3RX"),		MBFitsManager::rad2Deg(heca3Rx_));
  febeParTable_p_->setKeywordValue(string("HESA3RX"),		MBFitsManager::rad2Deg(hesa3Rx_));
  febeParTable_p_->setKeywordValue(string("NPAERX"),		MBFitsManager::rad2Deg(npaeRx_)	);
  febeParTable_p_->setKeywordValue(string("CARX"),			MBFitsManager::rad2Deg(caRx_)		);
  febeParTable_p_->setKeywordValue(string("ANRX"),			MBFitsManager::rad2Deg(anRx_)		);
  febeParTable_p_->setKeywordValue(string("AWRX"),			MBFitsManager::rad2Deg(awRx_)		);
	febeParTable_p_->setKeywordValue(string("HECE2RX"),		MBFitsManager::rad2Deg(hece2Rx_));
	febeParTable_p_->setKeywordValue(string("HECE6RX"),		MBFitsManager::rad2Deg(hece6Rx_));
	febeParTable_p_->setKeywordValue(string("HESA4RX"),		MBFitsManager::rad2Deg(hesa4Rx_));
	febeParTable_p_->setKeywordValue(string("HESA5RX"),		MBFitsManager::rad2Deg(hesa5Rx_));
	febeParTable_p_->setKeywordValue(string("HSCARX"),		MBFitsManager::rad2Deg(hscaRx_)	);
	febeParTable_p_->setKeywordValue(string("HSCA2RX"),		MBFitsManager::rad2Deg(hsca2Rx_));
	febeParTable_p_->setKeywordValue(string("HSSA3RX"),		MBFitsManager::rad2Deg(hssa3Rx_));
	febeParTable_p_->setKeywordValue(string("HSCA5RX"),		MBFitsManager::rad2Deg(hsca5Rx_));
	febeParTable_p_->setKeywordValue(string("NRXRX"),			MBFitsManager::rad2Deg(nRxRx_)	);
	febeParTable_p_->setKeywordValue(string("NRYRX"),			MBFitsManager::rad2Deg(nRyRx_)	);
  febeParTable_p_->setKeywordValue(string("SIG_ONLN"),	sigOnln_												);
  febeParTable_p_->setKeywordValue(string("REF_ONLN"),	refOnln_												);
  febeParTable_p_->setKeywordValue(string("SIG_POL"),		sigPol_													);
  febeParTable_p_->setKeywordValue(string("REF_POL"),		refPol_													);

  febeParTable_p_->setKeywordValue(string("RXBA_40"),		rxBa_40_												);
  febeParTable_p_->setKeywordValue(string("RXBA_80"),		rxBa_80_												);
  febeParTable_p_->setKeywordValue(string("RXCH_40"),		rxCh_40_												);
  febeParTable_p_->setKeywordValue(string("RXCH_80"),		rxCh_80_												);
  febeParTable_p_->setKeywordValue(string("RXHOR_40"),	rxHor_40_												);
  febeParTable_p_->setKeywordValue(string("RXHOR_80"),	rxHor_80_												);

	if ( m_isGrouping ) {
		CMBFitsWriter* const primaryHeaderFebeParTable_p = static_cast<CMBFitsWriter*>(febeParTable_p_);
		primaryHeaderFebeParTable_p->setKeywordValue(CMBFitsWriter::Primary,	string("MBFITS"),	m_datasetName);
	}

	febeParTable_p_->flush();
}

void MBFitsManager::saveFebeParBinTableData( CMBFitsWriterTable* const	febeParTable_p_,
																						 const MBFitsManager::Long_v_t&		useBand_,
																						 const MBFitsManager::Long_v_t&		nUseFeed_,
																						 const MBFitsManager::Long_v_t&		useFeed_,
																						 const MBFitsManager::Long_v_t&		beSects_,
																						 const MBFitsManager::Long_v_t&		feedType_,
																						 const MBFitsManager::Double_v_t&	feedOffX_,
																						 const MBFitsManager::Double_v_t&	feedOffY_,
																						 const int												refFeed_,
																						 const string&										polTy_,
																						 const MBFitsManager::Float_v_t&	polA_,
																						 const MBFitsManager::Float_v_t&	aperEff_,
																						 const MBFitsManager::Float_v_t&	beamEff_,
																						 const MBFitsManager::Float_v_t&	etafss_,
																						 const MBFitsManager::Float_v_t&	hpbw_,
																						 const MBFitsManager::Float_v_t&	antGain_,
																						 const MBFitsManager::Float_v_t&	tCal_,
																						 const MBFitsManager::Float_v_t&	bolCalFc_,
																						 const MBFitsManager::Float_v_t&	beGain_,
																						 const MBFitsManager::Float_v_t&	flatFiel_,
																						 const MBFitsManager::Float_v_t&	boldCoff_,
																						 const MBFitsManager::Float_v_t&	gainImag_,
																						 const MBFitsManager::Float_v_t&	gainEle1_,
																						 const MBFitsManager::Float_v_t&	gainEle2_ ) {
  febeParTable_p_->setColumnValue(string("USEBAND"),	useBand_	);
  febeParTable_p_->setColumnValue(string("NUSEFEED"),	nUseFeed_	);

  febeParTable_p_->addTableColumns(1);

  febeParTable_p_->setColumnValue(string("USEFEED"),	useFeed_	);
  febeParTable_p_->setColumnValue(string("BESECTS"),	beSects_	);
  febeParTable_p_->setColumnValue(string("FEEDTYPE"),	feedType_	);
  febeParTable_p_->setColumnValue(string("FEEDOFFX"),	feedOffX_	);
  febeParTable_p_->setColumnValue(string("FEEDOFFY"),	feedOffY_	);
  febeParTable_p_->setColumnValue(string("REFFEED"),	refFeed_	);
	febeParTable_p_->setColumnValue(string("POLTY"),		polTy_		);
  febeParTable_p_->setColumnValue(string("POLA"),			polA_			);
  febeParTable_p_->setColumnValue(string("APEREFF"),	aperEff_	);
  febeParTable_p_->setColumnValue(string("BEAMEFF"),	beamEff_	);
  febeParTable_p_->setColumnValue(string("ETAFSS"),		etafss_		);

  MBFitsManager::Float_v_t	hpbw;
  for ( MBFitsManager::Float_ci_v_t	hpbw_ci = hpbw_.begin(); hpbw_ci != hpbw_.end(); ++hpbw_ci ) {
		hpbw.push_back(MBFitsManager::rad2Deg(*hpbw_ci));
  }
  febeParTable_p_->setColumnValue(string("HPBW"),			hpbw			);
  hpbw.clear();

  febeParTable_p_->setColumnValue(string("ANTGAIN"),	antGain_	);
  febeParTable_p_->setColumnValue(string("TCAL"),			tCal_			);
  febeParTable_p_->setColumnValue(string("BOLCALFC"),	bolCalFc_	);
  febeParTable_p_->setColumnValue(string("BEGAIN"),		beGain_		);
  febeParTable_p_->setColumnValue(string("FLATFIEL"),	flatFiel_	);
  febeParTable_p_->setColumnValue(string("BOLDCOFF"),	boldCoff_	);
  febeParTable_p_->setColumnValue(string("GAINIMAG"),	gainImag_	);
  febeParTable_p_->setColumnValue(string("GAINELE1"),	gainEle1_	);
  febeParTable_p_->setColumnValue(string("GAINELE2"),	gainEle2_	);

	febeParTable_p_->setRowIndex(febeParTable_p_->getRowIndex() + 1);

	febeParTable_p_->flush();
}

void MBFitsManager::createMonitorHeader( const int			scanNum_,
																				 const int			subSNum_,
//																				 const int			obsNum_,
																				 const string&	dateObs_,
																				 const string&	usrFrame_ ) {
  m_monitorTable_p->addTable();

  m_monitorTable_p->setKeywordValue(string("SCANNUM"),	scanNum_	);
  m_monitorTable_p->setKeywordValue(string("SUBSNUM"),	subSNum_	);
//  m_monitorTable_p->setKeywordValue(string("OBSNUM"),		obsNum_		);
  m_monitorTable_p->setKeywordValue(string("DATE-OBS"),	dateObs_	);
  m_monitorTable_p->setKeywordValue(string("USRFRAME"),	usrFrame_	);

	m_monitorTable_p->flush();
}

void MBFitsManager::createDataParHeader( CMBFitsWriterTable* const dataParTable_p_,
																				 const int 							scanNum_,
																				 const int 							subsNum_,
//																				 const int 							obsNum_,
																				 const string&					dateObs_,
																				 const string&					febe_,
																				 const double						lst_,
																				 const string&					subsType_,
//																				 const string&					obsType_,

																				 const Scan&						scanReference_,
																				 const Scan&						scan_,

//																				 const string&					cType1n_,
//																				 const string&					cType2n_,

																				 const bool							dpBlock_,
																				 const int 							nInts_,
																				 const string&					obStatus_,
																				 const bool							wobCoord_,
																				 const vector<string>&	phaseN_,
																				 const float						dewAng_,
																				 const float						dewExtra_ ) {
	CMBFitsWriter* const dataParTable_p = static_cast<CMBFitsWriter*>(dataParTable_p_);

  dataParTable_p_->addTable();

  dataParTable_p_->setKeywordValue(string("SCANNUM"),		scanNum_	);
  dataParTable_p_->setKeywordValue(string("SUBSNUM"),		subsNum_	);
//  dataParTable_p_->setKeywordValue(string("OBSNUM"),		obsNum_		);
  dataParTable_p_->setKeywordValue(string("DATE-OBS"),	dateObs_	);
  dataParTable_p_->setKeywordValue(string("FEBE"),			febe_			);
  dataParTable_p_->setKeywordValue(string("LST"),				lst_			);
  dataParTable_p_->setKeywordValue(string("SUBSTYPE"),	subsType_	);
//  dataParTable_p_->setKeywordValue(string("OBSTYPE"),		obsType_	);

	if ( scan_.getType()				!= scanReference_.getType()				) {
		dataParTable_p_->setKeywordValue(string("SCANTYPE"),	Scan::getType(scan_.getType())				);
	}
	if ( scan_.getMode()				!= scanReference_.getMode()				) {
		dataParTable_p_->setKeywordValue(string("SCANMODE"),	Scan::getMode(scan_.getMode())				);
	}
	if ( scan_.getGeometry()		!= scanReference_.getGeometry() 	) {
		dataParTable_p_->setKeywordValue(string("SCANGEOM"),	Scan::getGeometry(scan_.getGeometry()));
	}
	if ( scan_.getDirection()		!= scanReference_.getDirection()	) {
		dataParTable_p_->setKeywordValue(string("SCANDIR"),	Scan::getDirection(scan_.getDirection()));
	}
	if ( scan_.getLine()				!= scanReference_.getLine()				) {
		dataParTable_p_->setKeywordValue(string("SCANLINE"),	scan_.getLine()												);
	}
	if ( scan_.getRepeats()			!= scanReference_.getRepeats()		) {
		dataParTable_p_->setKeywordValue(string("SCANRPTS"),	scan_.getRepeats()										);
	}
	if ( scan_.getLength()			!= scanReference_.getLength()			) {
		dataParTable_p_->setKeywordValue(string("SCANLEN"),	scan_.getLength()												);
	}
	if ( scan_.getXVelocity()		!= scanReference_.getXVelocity()	) {
		dataParTable_p_->setKeywordValue(string("SCANXVEL"),	scan_.getXVelocity()									);
	}
	if ( scan_.getTime()				!= scanReference_.getTime()				) {
		dataParTable_p_->setKeywordValue(string("SCANTIME"),	scan_.getTime()												);
	}
	if ( scan_.getXSpace()			!= scanReference_.getXSpace()			) {
		dataParTable_p_->setKeywordValue(string("SCANXSPC"),	scan_.getXSpace()											);
	}
	if ( scan_.getYSpace()			!= scanReference_.getYSpace()			) {
		dataParTable_p_->setKeywordValue(string("SCANYSPC"),	scan_.getYSpace()											);
	}
	if ( scan_.getSkew()				!= scanReference_.getSkew()				) {
		dataParTable_p_->setKeywordValue(string("SCANSKEW"),	scan_.getSkew()												);
	}
	if ( scan_.getParameter1()	!= scanReference_.getParameter1()	) {
		dataParTable_p_->setKeywordValue(string("SCANPAR1"),	scan_.getParameter1()									);
	}
	if ( scan_.getParameter2()	!= scanReference_.getParameter2()	) {
		dataParTable_p_->setKeywordValue(string("SCANPAR2"),	scan_.getParameter2()									);
	}

	if ( scan_.getCType1N()			!= scanReference_.getCType1N()		) {
		dataParTable_p_->setKeywordValue(string("CTYPE1N"),		scan_.getCType1N()										);
	}
	if ( scan_.getCType2N()			!= scanReference_.getCType2N()		) {
		dataParTable_p_->setKeywordValue(string("CTYPE2N"),		scan_.getCType2N()										);
	}

  dataParTable_p_->setKeywordValue(string("DPBLOCK"),		dpBlock_	);
  dataParTable_p_->setKeywordValue(string("NINTS"),			nInts_		);
  dataParTable_p_->setKeywordValue(string("OBSTATUS"),	obStatus_	);
  dataParTable_p_->setKeywordValue(string("WOBCOORD"),	wobCoord_	);

	long indexPhase = 1;
	for ( vector<string>::const_iterator phase_ci = phaseN_.begin(); phase_ci != phaseN_.end(); ++phase_ci ) {
		stringstream keyword;
		keyword << "PHASE" << indexPhase;
		dataParTable_p->setKeywordValue(CMBFitsWriter::DataPar, keyword.str(), *phase_ci, string(), string("PHASEn"));
		keyword.str(string());

		++indexPhase;
	}

  dataParTable_p_->setKeywordValue(string("DEWANG"),		dewAng_		);
  dataParTable_p_->setKeywordValue(string("DEWEXTRA"),	dewExtra_	);

	if ( m_isGrouping ) {
		CMBFitsWriter* const primaryHeaderDataParTable_p = static_cast<CMBFitsWriter*>(dataParTable_p_);
		primaryHeaderDataParTable_p->setKeywordValue(CMBFitsWriter::Primary,	string("MBFITS"),	m_datasetName);
	}

	dataParTable_p_->flush();
}

void MBFitsManager::saveDataParBinTableData( CMBFitsWriterTable* const dataParTable_p_,
																						 const double	mjd_,
																						 const double	lst_,
																						 const double	integTim_,
																						 const int		phase_,
																						 const double	longOff_,
																						 const double	latOff_,
																						 const double	azimuth_,
																						 const double	elevatio_,
																						 const double	ra_,
																						 const double	dec_,
																						 const double	parAngle_,
																						 const double	cBasLong_,
																						 const double	cBasLat_,
																						 const double	basLong_,
																						 const double	basLat_,
																						 const double	rotAngle_,
																						 const double	mcrVal1_,
																						 const double	mcrVal2_,
																						 const double	mLonPole_,
																						 const double	mLatPole_,
																						 const double	dFocus_x_,
																						 const double	dFocus_y_,
																						 const double	dFocus_z_,
																						 const double	dPhi_x_,
																						 const double	dPhi_y_,
																						 const double	dPhi_z_,
																						 const double	wobDisLn_,
																						 const double	wobDisLt_ ) {
  dataParTable_p_->setColumnValue(string("MJD"),				mjd_			);
  dataParTable_p_->setColumnValue(string("LST"),				lst_			);
  dataParTable_p_->setColumnValue(string("INTEGTIM"),		integTim_	);
  dataParTable_p_->setColumnValue(string("PHASE"),			phase_		);
  dataParTable_p_->setColumnValue(string("LONGOFF"),		MBFitsManager::rad2Deg(longOff_)	);
  dataParTable_p_->setColumnValue(string("LATOFF"),			MBFitsManager::rad2Deg(latOff_)		);
  dataParTable_p_->setColumnValue(string("AZIMUTH"),		MBFitsManager::rad2Deg(azimuth_)	);
  dataParTable_p_->setColumnValue(string("ELEVATIO"),		MBFitsManager::rad2Deg(elevatio_)	);
  dataParTable_p_->setColumnValue(string("RA"),					MBFitsManager::rad2Deg(ra_)				);
  dataParTable_p_->setColumnValue(string("DEC"),				MBFitsManager::rad2Deg(dec_)			);
  dataParTable_p_->setColumnValue(string("PARANGLE"),		MBFitsManager::rad2Deg(parAngle_)	);
  dataParTable_p_->setColumnValue(string("CBASLONG"),		MBFitsManager::rad2Deg(cBasLong_)	);
  dataParTable_p_->setColumnValue(string("CBASLAT"),		MBFitsManager::rad2Deg(cBasLat_)	);
  dataParTable_p_->setColumnValue(string("BASLONG"),		MBFitsManager::rad2Deg(basLong_)	);
  dataParTable_p_->setColumnValue(string("BASLAT"),			MBFitsManager::rad2Deg(basLat_)		);
  dataParTable_p_->setColumnValue(string("ROTANGLE"),		MBFitsManager::rad2Deg(rotAngle_)	);
  dataParTable_p_->setColumnValue(string("MCRVAL1"),		MBFitsManager::rad2Deg(mcrVal1_)	);
  dataParTable_p_->setColumnValue(string("MCRVAL2"),		MBFitsManager::rad2Deg(mcrVal2_)	);
  dataParTable_p_->setColumnValue(string("MLONPOLE"),		MBFitsManager::rad2Deg(mLonPole_)	);
  dataParTable_p_->setColumnValue(string("MLATPOLE"),		MBFitsManager::rad2Deg(mLatPole_)	);
  dataParTable_p_->setColumnValue(string("DFOCUS_X"),		dFocus_x_	);
  dataParTable_p_->setColumnValue(string("DFOCUS_Y"),		dFocus_y_	);
  dataParTable_p_->setColumnValue(string("DFOCUS_Z"),		dFocus_z_	);
  dataParTable_p_->setColumnValue(string("DPHI_X"),			MBFitsManager::rad2Deg(dPhi_x_)		);
  dataParTable_p_->setColumnValue(string("DPHI_Y"),			MBFitsManager::rad2Deg(dPhi_y_)		);
  dataParTable_p_->setColumnValue(string("DPHI_Z"),			MBFitsManager::rad2Deg(dPhi_z_)		);
  dataParTable_p_->setColumnValue(string("WOBDISLN"),		MBFitsManager::rad2Deg(wobDisLn_)	);
  dataParTable_p_->setColumnValue(string("WOBDISLT"),		MBFitsManager::rad2Deg(wobDisLt_)	);

	dataParTable_p_->setRowIndex(dataParTable_p_->getRowIndex() + 1);

	dataParTable_p_->flush();
}

void MBFitsManager::createArrayDataHeader( CMBFitsWriterTable* const arrayDataTable_p_,
																					 const string&	febe_,
																					 const int			baseBand_,
																					 const int			scanNum_,
																					 const int			subsNum_,
//																					 const int			obsNum_,
																					 const string&	dateObs_,
																					 const int			channels_,
																					 const int			nUseFeed_,
																					 const double		freqRes_,
																					 const double		bandWid_,
																					 const string&	molecule_,
																					 const string&	transiti_,
																					 const double		restFreq_,
																					 const double		skyFreq_,
																					 const string&	sideBand_,
																					 const double		sbSep_,
																					 const string&	_2ctyp2_,
																					 const int			_2crpx2_,
																					 const int			_2crvl2_,
																					 const int			_21cd2a_,

																					 // Next we assign to the specral axis a frequency and velocity description relative to the rest frame
																					 // But other alternatives axes descriptions could also be given
																					 const SpectralAxis& spectralAxisRestFrameMainSideband_,
																					 const SpectralAxis& spectralAxisRestFrameImageSideband_,
																					 const SpectralAxis& spectralAxisSkyFrameMainSideband_,
																					 const SpectralAxis& spectralAxisSkyFrameImageSideband_,
																					 const SpectralAxis& spectralAxisRestFrameVelocity_,

																					 const float		_1vsou2r_,
																					 const float		_1vsys2r_ ) {
  arrayDataTable_p_->addTable();

	arrayDataTable_p_->setKeywordValue(string("FEBE"),			febe_			);
	arrayDataTable_p_->setKeywordValue(string("BASEBAND"),	baseBand_	);
	arrayDataTable_p_->setKeywordValue(string("SCANNUM"),		scanNum_	);
	arrayDataTable_p_->setKeywordValue(string("SUBSNUM"),		subsNum_	);
//	arrayDataTable_p_->setKeywordValue(string("OBSNUM"),		obsNum_		);
	arrayDataTable_p_->setKeywordValue(string("DATE-OBS"),	dateObs_	);
	arrayDataTable_p_->setKeywordValue(string("CHANNELS"),	channels_	);
	arrayDataTable_p_->setKeywordValue(string("NUSEFEED"),	nUseFeed_	);
	arrayDataTable_p_->setKeywordValue(string("FREQRES"),		freqRes_	);
	arrayDataTable_p_->setKeywordValue(string("BANDWID"),		bandWid_	);
	arrayDataTable_p_->setKeywordValue(string("MOLECULE"),	molecule_	);
	arrayDataTable_p_->setKeywordValue(string("TRANSITI"),	transiti_	);
	arrayDataTable_p_->setKeywordValue(string("RESTFREQ"),	restFreq_	);
	arrayDataTable_p_->setKeywordValue(string("SKYFREQ"),		skyFreq_	);
	arrayDataTable_p_->setKeywordValue(string("SIDEBAND"),	sideBand_	);
	arrayDataTable_p_->setKeywordValue(string("SBSEP"),			sbSep_		);
	arrayDataTable_p_->setKeywordValue(string("2CTYP2"),		_2ctyp2_	);
	arrayDataTable_p_->setKeywordValue(string("2CRPX2"),		_2crpx2_	);
	arrayDataTable_p_->setKeywordValue(string("2CRVL2"),		_2crvl2_	);
	arrayDataTable_p_->setKeywordValue(string("21CD2A"),		_21cd2a_	);

	// Next we assign to the specral axis a frequency and velocity description relative to the rest frame
	// But other alternatives axes descriptions could also be given
	arrayDataTable_p_->setKeywordValue(string("WCSNM2F"),		spectralAxisRestFrameMainSideband_.getAxisName()								);
	arrayDataTable_p_->setKeywordValue(string("1CTYP2F"),		spectralAxisRestFrameMainSideband_.getAxisCol2()								);
	arrayDataTable_p_->setKeywordValue(string("1CRPX2F"),		spectralAxisRestFrameMainSideband_.getReferenceChannel()				);
	arrayDataTable_p_->setKeywordValue(string("1CRVL2F"),		spectralAxisRestFrameMainSideband_.getReferenceChannelValue()		);
	arrayDataTable_p_->setKeywordValue(string("11CD2F"),		spectralAxisRestFrameMainSideband_.getChannelSeparation()				);
	arrayDataTable_p_->setKeywordValue(string("1CUNI2F"),		spectralAxisRestFrameMainSideband_.getUnit()										);
	arrayDataTable_p_->setKeywordValue(string("1SPEC2F"),		spectralAxisRestFrameMainSideband_.getStandardOfRest()					);
	arrayDataTable_p_->setKeywordValue(string("1SOBS2F"),		spectralAxisRestFrameMainSideband_.getObservingFrame()					);

	arrayDataTable_p_->setKeywordValue(string("WCSNM2I"),		spectralAxisRestFrameImageSideband_.getAxisName()								);
	arrayDataTable_p_->setKeywordValue(string("1CTYP2I"),		spectralAxisRestFrameImageSideband_.getAxisCol2()								);
	arrayDataTable_p_->setKeywordValue(string("1CRPX2I"),		spectralAxisRestFrameImageSideband_.getReferenceChannel()				);
	arrayDataTable_p_->setKeywordValue(string("1CRVL2I"),		spectralAxisRestFrameImageSideband_.getReferenceChannelValue()	);
	arrayDataTable_p_->setKeywordValue(string("11CD2I"),		spectralAxisRestFrameImageSideband_.getChannelSeparation()			);
	arrayDataTable_p_->setKeywordValue(string("1CUNI2I"),		spectralAxisRestFrameImageSideband_.getUnit()										);
	arrayDataTable_p_->setKeywordValue(string("1SPEC2I"),		spectralAxisRestFrameImageSideband_.getStandardOfRest()					);
	arrayDataTable_p_->setKeywordValue(string("1SOBS2I"),		spectralAxisRestFrameImageSideband_.getObservingFrame()					);

	arrayDataTable_p_->setKeywordValue(string("WCSNM2S"),		spectralAxisSkyFrameMainSideband_.getAxisName()									);
	arrayDataTable_p_->setKeywordValue(string("1CTYP2S"),		spectralAxisSkyFrameMainSideband_.getAxisCol2()									);
	arrayDataTable_p_->setKeywordValue(string("1CRPX2S"),		spectralAxisSkyFrameMainSideband_.getReferenceChannel()					);
	arrayDataTable_p_->setKeywordValue(string("1CRVL2S"),		spectralAxisSkyFrameMainSideband_.getReferenceChannelValue()		);
	arrayDataTable_p_->setKeywordValue(string("11CD2S"),		spectralAxisSkyFrameMainSideband_.getChannelSeparation()				);
	arrayDataTable_p_->setKeywordValue(string("1CUNI2S"),		spectralAxisSkyFrameMainSideband_.getUnit()											);
	arrayDataTable_p_->setKeywordValue(string("1SPEC2S"),		spectralAxisSkyFrameMainSideband_.getStandardOfRest()						);
	arrayDataTable_p_->setKeywordValue(string("1SOBS2S"),		spectralAxisSkyFrameMainSideband_.getObservingFrame()						);

	arrayDataTable_p_->setKeywordValue(string("WCSNM2J"),		spectralAxisSkyFrameImageSideband_.getAxisName()								);
	arrayDataTable_p_->setKeywordValue(string("1CTYP2J"),		spectralAxisSkyFrameImageSideband_.getAxisCol2()								);
	arrayDataTable_p_->setKeywordValue(string("1CRPX2J"),		spectralAxisSkyFrameImageSideband_.getReferenceChannel()				);
	arrayDataTable_p_->setKeywordValue(string("1CRVL2J"),		spectralAxisSkyFrameImageSideband_.getReferenceChannelValue()		);
	arrayDataTable_p_->setKeywordValue(string("11CD2J"),		spectralAxisSkyFrameImageSideband_.getChannelSeparation()				);
	arrayDataTable_p_->setKeywordValue(string("1CUNI2J"),		spectralAxisSkyFrameImageSideband_.getUnit()										);
	arrayDataTable_p_->setKeywordValue(string("1SPEC2J"),		spectralAxisSkyFrameImageSideband_.getStandardOfRest()					);
	arrayDataTable_p_->setKeywordValue(string("1SOBS2J"),		spectralAxisSkyFrameImageSideband_.getObservingFrame()					);

	arrayDataTable_p_->setKeywordValue(string("WCSNM2R"),		spectralAxisRestFrameVelocity_.getAxisName()										);
	arrayDataTable_p_->setKeywordValue(string("1CTYP2R"),		spectralAxisRestFrameVelocity_.getAxisCol2()										);
	arrayDataTable_p_->setKeywordValue(string("1CRPX2R"),		spectralAxisRestFrameVelocity_.getReferenceChannel()						);
	arrayDataTable_p_->setKeywordValue(string("1CRVL2R"),		spectralAxisRestFrameVelocity_.getReferenceChannelValue()				);
	arrayDataTable_p_->setKeywordValue(string("11CD2R"),		spectralAxisRestFrameVelocity_.getChannelSeparation()						);
	arrayDataTable_p_->setKeywordValue(string("1CUNI2R"),		spectralAxisRestFrameVelocity_.getUnit()												);
	arrayDataTable_p_->setKeywordValue(string("1SPEC2R"),		spectralAxisRestFrameVelocity_.getStandardOfRest()							);
	arrayDataTable_p_->setKeywordValue(string("1SOBS2R"),		spectralAxisRestFrameVelocity_.getObservingFrame()							);

	arrayDataTable_p_->setKeywordValue(string("1VSOU2R"),		_1vsou2r_	);
	arrayDataTable_p_->setKeywordValue(string("1VSYS2R"),		_1vsys2r_	);

	arrayDataTable_p_->flush();
}

void MBFitsManager::saveArrayDataBinTableData( CMBFitsWriterTable* const				arrayDataTable_p_,
																							 const double											mjd_,
																							 const MBFitsManager::Float_v_t&	data_ ) {
	arrayDataTable_p_->setColumnValue(string("MJD"), 	mjd_	);
	arrayDataTable_p_->setColumnValue(string("DATA"), data_	);

	arrayDataTable_p_->setRowIndex(arrayDataTable_p_->getRowIndex() + 1);

	if ( m_isGrouping ) {
		CMBFitsWriter* const primaryHeaderArrayDataTable_p = static_cast<CMBFitsWriter*>(arrayDataTable_p_);
		primaryHeaderArrayDataTable_p->setKeywordValue(CMBFitsWriter::Primary,	string("MBFITS"),	m_datasetName);
	}

	arrayDataTable_p_->flush();
}
