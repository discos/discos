#include "MBFitsWriter.h"

#include <dirent.h>
#include <errno.h>
#include <sstream>
#include <sys/stat.h>

using CCfits::BinaryTbl;
using CCfits::ExtHDU;
using CCfits::HduType;
using CCfits::Write;

using std::endl;
using std::pair;
using std::stringstream;
using std::string;

CMBFitsWriter::TableName_m_t								CMBFitsWriter::m_tablesNames						= CMBFitsWriter::TableName_m_t();
CMBFitsWriter::TableKeywordFormat_m_t				CMBFitsWriter::m_tablesKeywordsFormats	= CMBFitsWriter::TableKeywordFormat_m_t();
CMBFitsWriter::Keyword::KeywordKeyword_m_t	CMBFitsWriter::m_keywords								= CMBFitsWriter::Keyword::KeywordKeyword_m_t();

void CMBFitsWriter::initialize() {
	if ( CMBFitsWriter::m_tablesNames.empty() ) {
		CMBFitsWriter::m_tablesNames.insert(CMBFitsWriter::TableName_m_t::value_type(Undefined,	string("")								));
		CMBFitsWriter::m_tablesNames.insert(CMBFitsWriter::TableName_m_t::value_type(Primary,		string("Primary")			    ));
		CMBFitsWriter::m_tablesNames.insert(CMBFitsWriter::TableName_m_t::value_type(Scan,			string("SCAN-MBFITS")			));
		CMBFitsWriter::m_tablesNames.insert(CMBFitsWriter::TableName_m_t::value_type(FebePar,		string("FEBEPAR-MBFITS")	));
		CMBFitsWriter::m_tablesNames.insert(CMBFitsWriter::TableName_m_t::value_type(ArrayData,	string("ARRAYDATA-MBFITS")));
		CMBFitsWriter::m_tablesNames.insert(CMBFitsWriter::TableName_m_t::value_type(Monitor,		string("MONITOR-MBFITS")	));
		CMBFitsWriter::m_tablesNames.insert(CMBFitsWriter::TableName_m_t::value_type(DataPar,		string("DATAPAR-MBFITS")	));
		CMBFitsWriter::m_tablesNames.insert(CMBFitsWriter::TableName_m_t::value_type(Grouping,	string("GROUPING")				));
	}

	if ( CMBFitsWriter::m_tablesKeywordsFormats.empty() ) {
		pair<CMBFitsWriter::TableKeywordFormat_i_m_t, bool> tablesKeywordsFormats_insert;
		CMBFitsWriter::TableKeywordFormat_i_m_t							tablesKeywordsFormats_i;

		tablesKeywordsFormats_insert = CMBFitsWriter::m_tablesKeywordsFormats.insert(CMBFitsWriter::TableKeywordFormat_m_t::value_type(Undefined,	CMBFitsWriter::KeywordFormat_m_t()));

		tablesKeywordsFormats_insert = CMBFitsWriter::m_tablesKeywordsFormats.insert(CMBFitsWriter::TableKeywordFormat_m_t::value_type(Primary,		CMBFitsWriter::KeywordFormat_m_t()));
		tablesKeywordsFormats_i			 = tablesKeywordsFormats_insert.first;
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NAXIS"),												string("1I") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SIMPLE"),												string("1L") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("BITPIX"),												string("1I") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EXTEND"),												string("1L") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TELESCOP"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("ORIGIN"),												string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CREATOR"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MBFTSVER"),											string("10A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("COMMENT"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO DPR CATG"), 				string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO DPR TYPE"), 				string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO DPR TECH"), 				string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO OBS PROG ID"),			string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO OBS ID"),					string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO TEL AIRM START"),	string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO TEL AIRM END"),		string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO TEL ALT"),					string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HIERARCH ESO TEL AZ"),					string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("INSTRUME"),											string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FEBEn"),												string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FREQn"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LINEn"),												string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("BWIDn"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OBJECT"),												string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TIMESYS"),											string("4A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MJD-OBS"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DATE-OBS"),											string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RA"),														string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DEC"),													string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RADECSYS"),											string("30A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EQUINOX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EXPTIME"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("ORIGFILE"),											string("40A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PROJID"),												string("18A")));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MBFITS"),												string("40A")));

		tablesKeywordsFormats_insert = CMBFitsWriter::m_tablesKeywordsFormats.insert(CMBFitsWriter::TableKeywordFormat_m_t::value_type(Scan,			CMBFitsWriter::KeywordFormat_m_t()));
		tablesKeywordsFormats_i			 = tablesKeywordsFormats_insert.first;
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EXTNAME"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TELESCOP"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SITELONG"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SITELAT"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SITEELEV"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DIAMETER"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PROJID"),												string("18A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OBSID"),												string("24A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TIMESYS"),											string("4A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DATE-OBS"),											string("23A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MJD"),													string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LST"),													string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NOBS"),													string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NSUBS"),												string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("UTC2UT1"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TAI2UTC"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("ETUTC"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("GPSTAI"),												string("1D") ));
//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CTYPE1B"),											string("8A") ));	// Removed in 1.63
//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CTYPE2B"),											string("8A") ));	// Removed in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WCSNAME"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RADESYS"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EQUINOX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CTYPE1"),												string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CTYPE2"),												string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CRVAL1"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CRVAL2"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CTYPE1N"),											string("8A") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CTYPE2N"),											string("8A") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LONPOLE"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LATPOLE"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OBJECT"),												string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("BLONGOBJ"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("BLATOBJ"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LONGOBJ"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LATOBJ"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PATLONG"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PATLAT"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CALCODE"),											string("4A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MOVEFRAM"),											string("1L") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PERIDATE"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PERIDIST"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LONGASC"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OMEGA"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("INCLINAT"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("ECCENTR"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("ORBEPOCH"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("ORBEQNOX"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DISTANCE"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANTYPE"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANMODE"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANGEOM"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANDIR"),											string("4A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANLINE"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANRPTS"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANLEN"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANXVEL"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANTIME"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANXSPC"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANYSPC"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANSKEW"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANPAR1"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANPAR2"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CROCYCLE"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("ZIGZAG"),												string("1L") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TRANDIST"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TRANFREQ"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TRANFOCU"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WOBUSED"),											string("1L") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WOBTHROW"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WOBDIR"),												string("4A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WOBCYCLE"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WOBMODE"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WOBPATT"),											string("10A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PHASEn"),												string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NFEBE"),												string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PDELTAIA"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PDELTACA"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PDELTAIE"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FDELTAIA"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FDELTACA"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FDELTAIE"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("IA"),														string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("IE"),														string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HASA"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HACA"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESE"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECE"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HASA2"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HACA2"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA2"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECA2"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HACA3"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECA3"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA3"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NPAE"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CA"),														string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("AN"),														string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("AW"),														string("1E") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECE2"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECE6"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA4"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA5"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HSCA"),													string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HSCA2"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HSSA3"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HSCA5"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NRX"),													string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NRY"),													string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HYSA"),													string("1E") ));	// New in 1.63

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HYSA"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HYSE"),													string("1E") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SETLINX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SETLINY"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SETLINZ"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SETROTX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SETROTY"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SETROTZ"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MOVEFOC"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FOCAMP"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FOCFREQ"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FOCPHASE"),											string("1E") ));	// New in 1.63

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MBFITS"),												string("40A")));

		tablesKeywordsFormats_insert = CMBFitsWriter::m_tablesKeywordsFormats.insert(CMBFitsWriter::TableKeywordFormat_m_t::value_type(FebePar,		CMBFitsWriter::KeywordFormat_m_t()));
		tablesKeywordsFormats_i			 = tablesKeywordsFormats_insert.first;
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EXTNAME"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FEBE"),													string("17A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DATE-OBS"),											string("23A")));

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DEWCABIN"),											string("10A")));	// Modified in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DEWCABIN"),											string("70A")));	// Modified in 1.63

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DEWRTMOD"),											string("5A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DEWUSER"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DEWZERO"),											string("1E") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LOCATION"),											string("70A")));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OPTPATH"),											string("70A")));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NOPTREFL"),											string("1J") ));	// New in 1.63

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FEBEBAND"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FEBEFEED"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NUSEBAND"),											string("1J") ));

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FDTYPCOD"),											string("80A")));	// Modified in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FDTYPCOD"),											string("70A")));	// Modified in 1.63

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FEGAIN"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SWTCHMOD"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NPHASES"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FRTHRWLO"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FRTHRWHI"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TBLANK"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TSYNC"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("IARX"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("IERX"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HASARX"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HACARX"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESERX"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECERX"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESARX"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HASA2RX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HACA2RX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA2RX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECA2RX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HACA3RX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECA3RX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA3RX"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NPAERX"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CARX"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("ANRX"),													string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("AWRX"),													string("1E") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECE2RX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HECE6RX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA4RX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HESA5RX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HSCARX"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HSCA2RX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HSSA3RX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("HSCA5RX"),											string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NRXRX"),												string("1E") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NRYRX"),												string("1E") ));	// New in 1.63

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SIG_ONLN"),											string("80A")));	// Modified in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SIG_ONLN"),											string("70A")));	// Modified in 1.63
//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("REF_ONLN"),											string("80A")));	// Modified in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("REF_ONLN"),											string("70A")));	// Modified in 1.63
//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SIG_POL"),											string("80A")));	// Modified in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SIG_POL"),											string("70A")));	// Modified in 1.63
//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("REF_POL"),											string("80A")));	// Modified in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("REF_POL"),											string("70A")));	// Modified in 1.63

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RXBA_40"),											string("40A")));	// For Effelsberg Toolbox
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RXBA_80"),											string("40A")));	// For Effelsberg Toolbox

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RXCH_40"),											string("40A")));	// For Effelsberg Toolbox
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RXCH_80"),											string("40A")));	// For Effelsberg Toolbox

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RXHOR_40"),											string("40A")));	// For Effelsberg Toolbox
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RXHOR_80"),											string("40A")));	// For Effelsberg Toolbox

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MBFITS"),												string("40A")));

		tablesKeywordsFormats_insert = CMBFitsWriter::m_tablesKeywordsFormats.insert(CMBFitsWriter::TableKeywordFormat_m_t::value_type(ArrayData,	CMBFitsWriter::KeywordFormat_m_t()));
		tablesKeywordsFormats_i			 = tablesKeywordsFormats_insert.first;
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EXTNAME"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FEBE"),													string("17A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("BASEBAND"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SUBSNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OBSNUM"),												string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DATE-OBS"),											string("23A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CHANNELS"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NUSEFEED"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FREQRES"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("BANDWID"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MOLECULE"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("TRANSITI"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("RESTFREQ"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SKYFREQ"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SIDEBAND"),											string("3A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SBSEP"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("2CTYP2"),												string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("2CRPX2"),												string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("2CRVL2"),												string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("21CD2A"),												string("1J") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WCSNM2F"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CTYP2F"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRPX2F"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRVL2F"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("11CD2F"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CUNI2F"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SPEC2F"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SOBS2F"),											string("8A") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WCSNM2I"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CTYP2I"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRPX2I"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRVL2I"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("11CD2I"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CUNI2I"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SPEC2I"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SOBS2I"),											string("8A") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WCSNM2S"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CTYP2S"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRPX2S"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRVL2S"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("11CD2S"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CUNI2S"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SPEC2S"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SOBS2S"),											string("8A") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WCSNM2J"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CTYP2J"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRPX2J"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRVL2J"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("11CD2J"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CUNI2J"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SPEC2J"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SOBS2J"),											string("8A") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WCSNM2R"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CTYP2R"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRPX2R"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CRVL2R"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("11CD2R"),												string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1CUNI2R"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SPEC2R"),											string("8A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1SOBS2R"),											string("8A") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1VSOU2R"),											string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("1VSYS2R"),											string("1E") ));

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MBFITS"),												string("40A")));

		tablesKeywordsFormats_insert = CMBFitsWriter::m_tablesKeywordsFormats.insert(CMBFitsWriter::TableKeywordFormat_m_t::value_type(Monitor,		CMBFitsWriter::KeywordFormat_m_t()));
		tablesKeywordsFormats_i			 = tablesKeywordsFormats_insert.first;
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EXTNAME"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SUBSNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OBSNUM"),												string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DATE-OBS"),											string("23A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("USRFRAME"),											string("20A")));

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MBFITS"),												string("40A")));

		tablesKeywordsFormats_insert = CMBFitsWriter::m_tablesKeywordsFormats.insert(CMBFitsWriter::TableKeywordFormat_m_t::value_type(DataPar,		CMBFitsWriter::KeywordFormat_m_t()));
		tablesKeywordsFormats_i			 = tablesKeywordsFormats_insert.first;
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EXTNAME"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SUBSNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OBSNUM"),												string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DATE-OBS"),											string("23A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("FEBE"),													string("17A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("LST"),													string("1D") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SUBSTYPE"),											string("4A") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OBSTYPE"),											string("4A") ));	// Deprecated in 1.63

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANTYPE"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANMODE"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANGEOM"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANDIR"),											string("4A") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANLINE"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANRPTS"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANLEN"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANXVEL"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANTIME"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANXSPC"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANYSPC"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANSKEW"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANPAR1"),											string("1D") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANPAR2"),											string("1D") ));

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CTYPE1N"),											string("8A") ));	// New in 1.63
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("CTYPE2N"),											string("8A") ));	// New in 1.63

		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DPBLOCK"),											string("1L") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("NINTS"),												string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("OBSTATUS"),											string("10A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("WOBCOORD"),											string("1L") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("PHASEn"),												string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DEWANG"),												string("1E") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DEWEXTRA"),											string("1E") ));

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MBFITS"),												string("40A")));

		tablesKeywordsFormats_insert = CMBFitsWriter::m_tablesKeywordsFormats.insert(CMBFitsWriter::TableKeywordFormat_m_t::value_type(Grouping,	CMBFitsWriter::KeywordFormat_m_t()));
		tablesKeywordsFormats_i			 = tablesKeywordsFormats_insert.first;
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("EXTNAME"),											string("20A")));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("SCANNUM"),											string("1J") ));
		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("DATE-OBS"),											string("23A")));

//		tablesKeywordsFormats_i->second.insert(CMBFitsWriter::KeywordFormat_m_t::value_type(string("MBFITS"),												string("40A")));
	}

	if ( CMBFitsWriter::m_keywords.empty() ) {
		Keyword keyword;

		// Anticipated monitor points
		keyword = Keyword(string("ENCODER_AZ_EL"),					string("2D"),	string("deg;deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("TRACKING_AZ_EL"),					string("2D"),	string("deg;deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("INCLINOMETER_1_2"),				string("2D"),	string("deg;deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("FOCUS_X_Y_Z"),						string("3D"),	string("mm;mm;mm"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("DFOCUS_X_Y_Z"),						string("3D"),	string("mm;mm;mm"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("PHI_X_Y_Z"),							string("3D"),	string("deg;deg;deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("DPHI_X_Y_Z"),							string("3D"),	string("deg;deg;deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("ANTENNA_AZ_EL"),					string("2D"),	string("deg;deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("WOBDISPL"),								string("1D"),	string("deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		// Frequencies labelled by frontend
		keyword = Keyword(string("GUNF_<fe>"),							string("1D"),	string("Hz"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("PLLF_<fe>"),							string("1D"), string("Hz"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

//		columnNames_.push_back(string("LOFREQ_<febe>"));
//		columnFormats_.push_back(string(""));				// NLOD
//		columnUnits_.push_back(string("Hz"));

//		columnNames_.push_back(string("LOSIDEBAND_<febe>"));
//		columnFormats_.push_back(string(""));				// NLOD
//		columnUnits_.push_back(string("Hz"));

		keyword = Keyword(string("NUMIFCONV_<febe>"),				string("1J"), string());
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		// Environmental parameters
//		columnNames_.push_back(string("TSTRUCT"));
//		columnFormats_.push_back(string(""));				// nD
//		columnUnits_.push_back(string("K"));

		keyword = Keyword(string("WIND_SPEED_DIR"),					string("2D"),	string("m/s;deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("TAMB_P_HUMID"),						string("3D"),	string("C;hPa;%"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("TCABIN"),									string("1D"),	string("K"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("TDEWAR"),									string("1D"),	string("K"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		// Calibration
//		columnNames_.push_back(string("TP_<febe>"));
//		columnFormats_.push_back(string(""));				// NUSBDD
//		columnUnits_.push_back(string("Counts"));

		keyword = Keyword(string("THOTCOLD_<cabin>"),				string("2D"),	string("K;K"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("THOTCOLD_<febe>_<band>"),	string("2D"),	string("K;K"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

//		columnNames_.push_back(string("PHOT_<febe>_<band>"));
//		columnFormats_.push_back(string(""));				// NUSFDD
//		columnUnits_.push_back(string("counts"));
//
//		columnNames_.push_back(string("PCOLD_<febe>_<band>"));
//		columnFormats_.push_back(string(""));				// NUSFDD
//		columnUnits_.push_back(string("counts"));
//
//		columnNames_.push_back(string("PSKY_<febe>_<band>"));
//		columnFormats_.push_back(string(""));				// NUSFDD
//		columnUnits_.push_back(string("counts"));
//
//		columnNames_.push_back(string("TRX_<febe>_<band>"));
//		columnFormats_.push_back(string(""));				// NUSFDD
//		columnUnits_.push_back(string("K"));
//
//		columnNames_.push_back(string("TSYS_<febe>_<band>"));
//		columnFormats_.push_back(string(""));				// NUSFDD
//		columnUnits_.push_back(string("K"));
//
//		columnNames_.push_back(string("TSYSIMAG_<febe>_<band>"));
//		columnFormats_.push_back(string(""));				// NUSFDD
//		columnUnits_.push_back(string("K"));
//
//		columnNames_.push_back(string("TCAL_<febe>_<band>"));
//		columnFormats_.push_back(string(""));				// NUSFDD
//		columnUnits_.push_back(string("K"));

		keyword = Keyword(string("TAUZEN_<febe>_<band>"),		string("1D"),	string());
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("TAU_<febe>_<band>"),			string("1D"),	string());
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("TAUIMAGE_<febe>_<band>"),	string("1D"),	string());
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

//		columnNames_.push_back(string("GAINARRAY_<febe>_<band>"));
//		columnFormats_.push_back(string(""));				// NUSFD × NCHD
//		columnUnits_.push_back(string(""));

		keyword = Keyword(string("OBSFL_<febe>_<band>"),		string("3D"),	string("counts;counts;counts"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("CALFL_<febe>_<band>"),		string("3D"),	string("Jy;Jy;Jy"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("CALFAC_<febe>_<band>"),		string("3D"),	string("Jy/counts;Jy/counts;Jy/counts"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		// Water vapour radiometer
		keyword = Keyword(string("TSYS_TRX_RD<freq>"),			string("2D"),	string("K;K"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("CORCF_VALID_RD<freq>"),		string("2D"),	string());
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("TAU_WPATH_RD<freq>"),			string("2D"),	string("-;mm"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		// Observer Pointing
		keyword = Keyword(string("IAOBS_CAOBS_IEOBS"),			string("3D"),	string("arcsec;arcsec;arcsec"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("FOCOBS_X_Y_Z"),						string("3D"),	string("mm;mm;mm"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("PHIOBS_X_Y_Z"),						string("3D"),	string("deg;deg;deg"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));

		keyword = Keyword(string("REFRACTIO"),							string("3D"),	string("arcsec;arcsec;arcsec"));
		CMBFitsWriter::m_keywords.insert(Keyword::KeywordKeyword_m_t::value_type(keyword.getKeyword(), keyword));
	}
}

void CMBFitsWriter::terminate() {
	CMBFitsWriter::m_keywords.clear();

	for ( CMBFitsWriter::TableKeywordFormat_i_m_t tableKeywordFormat_i = CMBFitsWriter::m_tablesKeywordsFormats.begin();
				tableKeywordFormat_i != CMBFitsWriter::m_tablesKeywordsFormats.end(); ++tableKeywordFormat_i ) {
		tableKeywordFormat_i->second.clear();
	}
	CMBFitsWriter::m_tablesKeywordsFormats.clear();

	CMBFitsWriter::m_tablesNames.clear();
}

CMBFitsWriter::CMBFitsWriter() : m_mbFits_p(NULL),
																 m_fileName(string()),
																 m_path(string()),
																 m_tablesTables(CMBFitsWriter::TableTable_m_p_t()),
																 m_tablesRowIndices(CMBFitsWriter::TableRowIndex_m_t()) {
	CMBFitsWriter::m_tablesTables.insert(CMBFitsWriter::TableTable_m_p_t::value_type(Undefined,	NULL));
	CMBFitsWriter::m_tablesTables.insert(CMBFitsWriter::TableTable_m_p_t::value_type(Primary,		NULL));
	CMBFitsWriter::m_tablesTables.insert(CMBFitsWriter::TableTable_m_p_t::value_type(Scan,			NULL));
	CMBFitsWriter::m_tablesTables.insert(CMBFitsWriter::TableTable_m_p_t::value_type(FebePar,		NULL));
	CMBFitsWriter::m_tablesTables.insert(CMBFitsWriter::TableTable_m_p_t::value_type(ArrayData,	NULL));
	CMBFitsWriter::m_tablesTables.insert(CMBFitsWriter::TableTable_m_p_t::value_type(Monitor,		NULL));
	CMBFitsWriter::m_tablesTables.insert(CMBFitsWriter::TableTable_m_p_t::value_type(DataPar,		NULL));
	CMBFitsWriter::m_tablesTables.insert(CMBFitsWriter::TableTable_m_p_t::value_type(Grouping,	NULL));

	CMBFitsWriter::m_tablesRowIndices.insert(CMBFitsWriter::TableRowIndex_m_t::value_type(Undefined,	1));
	CMBFitsWriter::m_tablesRowIndices.insert(CMBFitsWriter::TableRowIndex_m_t::value_type(Primary,		1));
	CMBFitsWriter::m_tablesRowIndices.insert(CMBFitsWriter::TableRowIndex_m_t::value_type(Scan,				1));
	CMBFitsWriter::m_tablesRowIndices.insert(CMBFitsWriter::TableRowIndex_m_t::value_type(FebePar,		1));
	CMBFitsWriter::m_tablesRowIndices.insert(CMBFitsWriter::TableRowIndex_m_t::value_type(ArrayData,	1));
	CMBFitsWriter::m_tablesRowIndices.insert(CMBFitsWriter::TableRowIndex_m_t::value_type(Monitor,		1));
	CMBFitsWriter::m_tablesRowIndices.insert(CMBFitsWriter::TableRowIndex_m_t::value_type(DataPar,		1));
	CMBFitsWriter::m_tablesRowIndices.insert(CMBFitsWriter::TableRowIndex_m_t::value_type(Grouping,		1));
}

/*	// Not implemented
//  warning: passing ‘const std::auto_ptr<CCfits::FITS>’ as ‘this’ argument of ‘std::auto_ptr<_Tp>::operator std::auto_ptr_ref<_Tp1>() [with _Tp1 = CCfits::FITS, _Tp = CCfits::FITS]’ discards qualifiers
CMBFitsWriter::CMBFitsWriter( const CMBFitsWriter& mbFitsWriter_ ) : m_mbFits_p(mbFitsWriter_.m_mbFits_p),
																																		 m_fileName(mbFitsWriter_.m_fileName),
																																		 m_path(mbFitsWriter_.m_path),
																																		 m_tablesTables(mbFitsWriter_.m_tablesTables),
																																		 m_tablesRowIndices(mbFitsWriter_.m_tablesRowIndices) {
}
*/

CMBFitsWriter::~CMBFitsWriter() {
	m_tablesRowIndices.clear();
	m_tablesTables.clear();

	close();
}

bool CMBFitsWriter::operator==( const CMBFitsWriter& mbFitsWriter_ ) const {
	return false;

/*	// Not implemented
	return ( ((!m_mbFits_p.get() && !mbFitsWriter_.m_mbFits_p.get()) ||
						( m_mbFits_p.get() &&  mbFitsWriter_.m_mbFits_p.get() && (*m_mbFits_p == *mbFitsWriter_.m_mbFits_p))) &&
					 (m_fileName == mbFitsWriter_.m_fileName) &&
					 (m_path		 == mbFitsWriter_.m_path		) &&
					 (m_tablesTables		 == mbFitsWriter_.m_tablesTables		 ) &&
					 (m_tablesRowIndices == mbFitsWriter_.m_tablesRowIndices) );
*/
}

bool CMBFitsWriter::operator!=( const CMBFitsWriter& mbFitsWriter_ ) const {
	return !(*this == mbFitsWriter_);
}

/*	// Not implemented
//  warning: passing ‘const std::auto_ptr<CCfits::FITS>’ as ‘this’ argument of ‘std::auto_ptr<_Tp>::operator std::auto_ptr_ref<_Tp1>() [with _Tp1 = CCfits::FITS, _Tp = CCfits::FITS]’ discards qualifiers
CMBFitsWriter& CMBFitsWriter::operator=( const CMBFitsWriter& mbFitsWriter_ ) {
	// handle self assignment
	if ( *this != mbFitsWriter_ ) {
		m_mbFits_p				 = mbFitsWriter_.m_mbFits_p;
		m_fileName				 = mbFitsWriter_.m_fileName;
		m_path						 = mbFitsWriter_.m_path;
		m_tablesTables		 = mbFitsWriter_.m_tablesTables;
		m_tablesRowIndices = mbFitsWriter_.m_tablesRowIndices;
	}

	//assignment operator
	return *this;
}
*/

string CMBFitsWriter::getExtName( const CMBFitsWriter::Table_e table_e_ ) {
	const CMBFitsWriter::TableName_ci_m_t tableName_ci = CMBFitsWriter::m_tablesNames.find(table_e_);
	if ( tableName_ci == CMBFitsWriter::m_tablesNames.end() ) {
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

	return tableName_ci->second;
}

string CMBFitsWriter::getFileName() const {
	return m_fileName;
}

void CMBFitsWriter::setFileName( const string& fileName_ ) {
	string::size_type positionLastPathSeparator = fileName_.find_last_of("/\\");
	if ( string::npos != positionLastPathSeparator ) {
		setPath(fileName_.substr(0, positionLastPathSeparator + 1));
		m_fileName = fileName_.substr(positionLastPathSeparator + 1, fileName_.length() - positionLastPathSeparator);
	} else {
		m_fileName = fileName_;
	}
}

string CMBFitsWriter::getPath() const {
	return m_path;
}

void CMBFitsWriter::setPath( const string& path_ ) {
	string path(path_);
	if ( path.find_last_of("/\\") != (path.length() - 1) ) path += "/";

	m_path = path;
}

string CMBFitsWriter::getFullName() const {
	return (m_path + m_fileName);
}

bool CMBFitsWriter::directoryExists( const string& path_ ) {
	if ( path_.empty() ) return true;

	DIR*			 directory			 = opendir(path_.c_str());
	const bool directoryExists = (directory != NULL);
	if ( directory ) closedir(directory);

	return directoryExists;
}

void CMBFitsWriter::makeDirectory( const string& path_ ) {
	const int resultMkDir = mkdir(path_.c_str(), 0777);

	// mkdir() returns zero on success, or -1 if an error occurred (in which case, errno is set appropriately).
	// errno = EEXIST: File exists
	if ( (0 != resultMkDir) && (EEXIST != errno) ) {
/*
cout << "CMBFitsWriter::makeDirectory()" << " - " << resultMkDir << endl;
cout << "CMBFitsWriter::makeDirectory()" << " - " << errno << endl;

cout << "EACCES: " << EACCES << endl;
cout << "EEXIST: " << EEXIST << endl;
cout << "EFAULT: " << EFAULT << endl;
cout << "ELOOP: " << ELOOP << endl;
cout << "ENAMETOOLONG: " << ENAMETOOLONG << endl;
cout << "ENOENT: " << ENOENT << endl;
cout << "ENOMEM: " << ENOMEM << endl;
cout << "ENOSPC: " << ENOSPC << endl;
cout << "ENOTDIR: " << ENOTDIR << endl;
cout << "EPERM: " << EPERM << endl;
cout << "EROFS: " << EROFS << endl;
*/
/*
	EACCES 				The parent directory does not allow write permission to the process, or one of the directories in pathname did not allow search permission.  (See also path_resolution(2).)
	EEXIST				pathname already exists (not necessarily as a directory).  This includes the case where pathname is a symbolic link, dangling or not.
	EFAULT				pathname points outside your accessible address space.
	ELOOP					Too many symbolic links were encountered in resolving pathname.
	ENAMETOOLONG	pathname was too long.
	ENOENT				A directory component in pathname does not exist or is a dangling symbolic link.
	ENOMEM				Insufficient kernel memory was available.
	ENOSPC				The device containing pathname has no room for the new directory.
	ENOSPC				The new directory cannot be created because the user’s disk quota is exhausted.
	ENOTDIR				A component used as a directory in pathname is not, in fact, a directory.
	EPERM					The filesystem containing pathname does not support the creation of directories.
	EROFS					pathname refers to a file on a read-only filesystem.
*/
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}
}

bool CMBFitsWriter::directoryExists() const {
	return CMBFitsWriter::directoryExists(getPath());
}

void CMBFitsWriter::makeDirectory() const {
	if ( getPath().empty() ) {
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

	string path = getPath();
	const string separators("\\/");

	typedef vector<string>							String_v_t;
	typedef String_v_t::iterator				String_i_v_t;
	typedef String_v_t::const_iterator	String_ci_v_t;

	String_v_t pathParts;

	string::size_type positionBegin = 0;
	string::size_type positionEnd		= path.find_first_of(separators, positionBegin);
	if ( string::npos == positionEnd ) positionEnd = path.length();
	pathParts.push_back(path.substr(positionBegin, positionEnd - positionBegin));

	while ( path.length() > positionEnd ) {
		positionBegin = positionEnd + separators.length() - 1;
		positionEnd		= path.find_first_of(separators, positionBegin);
		if ( string::npos == positionEnd ) positionEnd = path.length();
		pathParts.push_back(path.substr(positionBegin, positionEnd - positionBegin));
	}

	path = string();
	for ( String_ci_v_t pathPart_ci = pathParts.begin(); pathPart_ci != pathParts.end(); ++pathPart_ci ) {
		path += string("/") + *pathPart_ci;
		if ( !CMBFitsWriter::directoryExists(path) ) {
			CMBFitsWriter::makeDirectory(path);
		}
	}
}

void CMBFitsWriter::open() {
	if ( !directoryExists() ) {
		makeDirectory();
	}

	// the datatype of the primary image.
	// bitpix may be one of the following CFITSIO constants: BYTE_IMG, SHORT_IMG, LONG_IMG, FLOAT_IMG, DOUBLE_IMG, USHORT_IMG, ULONG_IMG.
	// Note that if you send in a bitpix of USHORT_IMG or ULONG_IMG, CCfits will set HDU::bitpix() to its signed equivalent (SHORT_IMG or LONG_IMG), and then set BZERO to 2^15 or 2^31.
	// va impostato il valore di bitpix nel file FITS? Serve per alcuni costruttori, non per tutti
	int  	m_bitpix	= BYTE_IMG;		// si riesce ad ottenere da qualche componente di ESCS?

	vector<String> primaryKeys;

	try {
		m_mbFits_p = new FITS(getFullName(), Write, false, primaryKeys);
	} catch( FITS::CantCreate& exception_ ) {
		primaryKeys.clear();
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception_;
	}

	primaryKeys.clear();
}

void CMBFitsWriter::close() {
	if ( m_mbFits_p ) { m_mbFits_p->destroy(); m_mbFits_p = NULL; }
}

void CMBFitsWriter::flush() {
	if ( m_mbFits_p ) { m_mbFits_p->flush(); }
}

void CMBFitsWriter::addTable( const CMBFitsWriter::Table_e table_e_ ) {
	if ( !m_mbFits_p ) {
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

  if ( CMBFitsWriter::Primary == table_e_ ) {
	  printf("ex : %s %d\n",__FILE__,__LINE__);
	  throw exception();
  }

	if ( CMBFitsWriter::Undefined != table_e_ ) {
		const string extName(CMBFitsWriter::getExtName(table_e_));

		int									rows_n	= 1;					// The number of rows in the table to be created.
//		std::vector<string> columnNames;					// A vector containing the table column names
//		std::vector<string> columnFormats;				// A vector containing the table column formats
//		std::vector<string> columnUnits;					// (Optional) a vector giving the units of the columns.
//		HduType							type		= BinaryTbl;	// The table type - AsciiTbl or BinaryTbl (defaults to BinaryTbl) the lists of columns are optional - one can create an empty table extension but if supplied, colType, columnName and colFmt must have equal dimensions.
//		int									version = 1;
//
//		CMBFitsWriter::getColumns(table_e_, columnNames, columnFormats, columnUnits);

		try {
			Table* table_p = m_mbFits_p->addTable(extName, rows_n);
			m_tablesTables.find(table_e_)->second = table_p;
		} catch( FitsException& exception_ ) {
//			columnNames.clear();
//			columnFormats.clear();
//			columnUnits.clear();
			printf("ex : %s %d\n",__FILE__,__LINE__);
			throw exception_;
		}

//		columnNames.clear();
//		columnFormats.clear();
//		columnUnits.clear();
	}
}

//#include <iostream>
//using std::cout;
//using std::endl;
void CMBFitsWriter::addTableColumns( const CMBFitsWriter::Table_e table_e_,
																		 const int iteration_ ) {
	if ( !m_mbFits_p ) {
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

  if ( CMBFitsWriter::Primary == table_e_ ) {
	  printf("ex : %s %d\n",__FILE__,__LINE__);
	  throw exception();
  }

	if ( CMBFitsWriter::Undefined != table_e_ ) {
		const string extName(CMBFitsWriter::getExtName(table_e_));

		std::vector<string> columnNames;					// A vector containing the table column names
		std::vector<string> columnFormats;				// A vector containing the table column formats
		std::vector<string> columnUnits;					// (Optional) a vector giving the units of the columns.

		CMBFitsWriter::getColumns(table_e_, columnNames, columnFormats, columnUnits, iteration_);

		try {
			ExtHDU& table    = m_mbFits_p->extension(extName);

      int     typeCode = 0;
      long    repeat   = 0;
      long    width    = 0;
      int     status   = 0;

			for ( std::vector<string>::const_iterator columnName_ci = columnNames.begin(), columnFormat_ci = columnFormats.begin(), columnUnit_ci = columnUnits.begin();
            ((columnName_ci != columnNames.end()) && (columnFormat_ci != columnFormats.end()) && (columnUnit_ci != columnUnits.end()));
            ++columnName_ci, ++columnFormat_ci, ++columnUnit_ci ) {
        fits_binary_tform(const_cast<char*>(columnFormat_ci->c_str()), &typeCode, &repeat, &width, &status);
        table.addColumn(static_cast<CCfits::ValueType>(typeCode), *columnName_ci, repeat, columnUnit_ci->c_str());
      }
		} catch( FitsException& exception_ ) {
			columnNames.clear();
			columnFormats.clear();
			columnUnits.clear();
			printf("ex : %s %d\n",__FILE__,__LINE__);
			throw exception_;
		}

		columnNames.clear();
		columnFormats.clear();
		columnUnits.clear();
	}
}

unsigned int CMBFitsWriter::getRowIndex( const CMBFitsWriter::Table_e table_e_ ) const {
  if ( CMBFitsWriter::Primary == table_e_ ) {
	  printf("ex : %s %d\n",__FILE__,__LINE__);
	  throw exception();
  }

	CMBFitsWriter::TableRowIndex_ci_m_t tableRowIndex_ci = m_tablesRowIndices.find(table_e_);
	if ( tableRowIndex_ci == m_tablesRowIndices.end()	 ) {
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

	return tableRowIndex_ci->second;
}

void CMBFitsWriter::setRowIndex( const CMBFitsWriter::Table_e table_e_,
																 const unsigned int rowIndex_ ) {
  if ( CMBFitsWriter::Primary == table_e_ ) {
	  printf("ex : %s %d\n",__FILE__,__LINE__);
	  throw exception();
  }

  // il numero memorizzato potrebbe non essere sincronizzato con il valore in tabella - attenzione ai punti di modifica del valore in tabella e i memoria
	CMBFitsWriter::TableRowIndex_i_m_t	 tableRowIndex_i	 = m_tablesRowIndices.find(table_e_);
	if ( tableRowIndex_i == m_tablesRowIndices.end()	 ) {
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

	tableRowIndex_i->second = rowIndex_;
}

void CMBFitsWriter::insertRows( const CMBFitsWriter::Table_e table_e_,
																const unsigned int rowIndexStart_,
																const unsigned int rows_n_ ) {
  if ( CMBFitsWriter::Primary == table_e_ ) {
	  printf("ex : %s %d\n",__FILE__,__LINE__);
	  throw exception();
  }

  // il numero memorizzato potrebbe non essere sincronizzato con il valore in tabella - attenzione ai punti di modifica del valore in tabella e i memoria
	CMBFitsWriter::TableRowIndex_i_m_t	 tableRowIndex_i	 = m_tablesRowIndices.find(table_e_);
	if ( tableRowIndex_i == m_tablesRowIndices.end()	 ){
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

	// verificare che il valore di rowIndexStart_ sia compatibile con il numero di righe esistente in tabella - potrebbe farlo CCFits e sollevare eventualmente eccezioni

  try {
		Table* table_p = m_tablesTables.find(table_e_)->second;
//cout << "Cast done!" << endl;
//cout << ((NULL != table_p) ? "Cast succeded!" : "Cast failed!") << endl;
//    table_p->insertRows(rowIndexStart_, rows_n_);
//    dynamic_cast<Table*>(table)->insertRows(rowIndexStart_, rows_n_);
//    table_p->insertRows(rowIndexStart_, rows_n_);
//    table_p->insertRows(1);
//cout << "I did it!" << endl;
  } catch( FitsException& exception_ ) {
	  printf("ex : %s %d\n",__FILE__,__LINE__);
    throw exception_;
  }
}

string CMBFitsWriter::getKeywordFormat( const CMBFitsWriter::Table_e table_e_,
																				const string& keyword_ ) {
	const CMBFitsWriter::TableKeywordFormat_ci_m_t tableKeywordFormat_ci = CMBFitsWriter::m_tablesKeywordsFormats.find(table_e_);
	if ( tableKeywordFormat_ci == m_tablesKeywordsFormats.end()				) {
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

	const CMBFitsWriter::KeywordFormat_ci_m_t			 keywordFormat_ci			 = tableKeywordFormat_ci->second.find(keyword_);
	if ( keywordFormat_ci			 == tableKeywordFormat_ci->second.end() ) {
		printf("ex : %s %d\n",__FILE__,__LINE__);
		throw exception();
	}

	return keywordFormat_ci->second;
}

void CMBFitsWriter::getColumns( const CMBFitsWriter::Table_e table_e_,
															  vector<string>& columnNames_,
															  vector<string>& columnFormats_,
															  vector<string>& columnUnits_,
															  const unsigned int iteration_ ) {
	switch( table_e_ ) {
		case CMBFitsWriter::Undefined:
			printf("ex : %s %d\n",__FILE__,__LINE__);
			throw exception();
			break;

		case CMBFitsWriter::Primary:
			printf("ex : %s %d\n",__FILE__,__LINE__);
			throw exception();
			break;

		case CMBFitsWriter::Scan:
			columnNames_.push_back(string("FEBE"));
			columnFormats_.push_back(string("17A"));
			columnUnits_.push_back(string(""));
			break;

		case CMBFitsWriter::FebePar: {
			int nuseband	= 0;
			getKeywordValue(CMBFitsWriter::FebePar, string("NUSEBAND"),	nuseband);

			switch ( iteration_ ) {
				case 0: {
						stringstream nuseband_Long;
						nuseband_Long << nuseband << string("J");

						columnNames_.push_back(string("USEBAND"));
						columnFormats_.push_back(nuseband_Long.str());		// NUSBDJ
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("NUSEFEED"));
						columnFormats_.push_back(nuseband_Long.str());		// NUSBDJ
						columnUnits_.push_back(string(""));

						nuseband_Long.str(string());
					}
					break;
				case 1: {
						int	febefeed	= 0;
						int	nuseband	= 0;
						getKeywordValue(CMBFitsWriter::FebePar, string("FEBEFEED"),	febefeed);
						getKeywordValue(CMBFitsWriter::FebePar, string("NUSEBAND"),	nuseband);

//						valarray<long> nusefeed_valarray;
//						getColumnValue(CMBFitsWriter::FebePar, string("NUSEFEED"), getRowIndex(CMBFitsWriter::FebePar), nusefeed_valarray);
//						const long nusefeed = nusefeed_valarray.max();
const long nusefeed = 2;

						stringstream nusefeedNusebandPLong;
						nusefeedNusebandPLong << string("1PJ(") << (nusefeed * nuseband) << string(")");

						stringstream febefeedASCII;
						febefeedASCII					<< febefeed								<< string("A");

						stringstream febefeedReal;
						febefeedReal  				<< febefeed								<< string("E");

						stringstream febefeedDouble;
						febefeedDouble				<< febefeed								<< string("D");

						stringstream febefeedNusebandReal;
						febefeedNusebandReal	<< (febefeed * nuseband)	<< string("E");

						stringstream nusebandReal;
						nusebandReal					<< nuseband								<< string("E");

						columnNames_.push_back(string("USEFEED"));
						columnFormats_.push_back(nusefeedNusebandPLong.str());	// 1PJ(NUSFD × NUSBD)
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("BESECTS"));
						columnFormats_.push_back(nusefeedNusebandPLong.str());	// 1PJ(NUSFD × NUSBD)
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("FEEDTYPE"));
						columnFormats_.push_back(nusefeedNusebandPLong.str());	// 1PJ(NUSFD × NUSBD)
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("FEEDOFFX"));
						columnFormats_.push_back(febefeedDouble.str());					// NFDD
						columnUnits_.push_back(string("deg"));

						columnNames_.push_back(string("FEEDOFFY"));
						columnFormats_.push_back(febefeedDouble.str());					// NFDD
						columnUnits_.push_back(string("deg"));

						columnNames_.push_back(string("REFFEED"));
						columnFormats_.push_back(string("1J"));
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("POLTY"));
						columnFormats_.push_back(febefeedASCII.str());					// NFDA
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("POLA"));
						columnFormats_.push_back(febefeedReal.str());						// NFDE
						columnUnits_.push_back(string("deg"));

						columnNames_.push_back(string("APEREFF"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("BEAMEFF"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("ETAFSS"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("HPBW"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string("deg"));

						columnNames_.push_back(string("ANTGAIN"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string("K/Jy"));

						columnNames_.push_back(string("TCAL"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string("mK"));

						columnNames_.push_back(string("BOLCALFC"));
						columnFormats_.push_back(nusebandReal.str());						// NUSBDE
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("BEGAIN"));
						columnFormats_.push_back(nusebandReal.str());						// NUSBDE
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("FLATFIEL"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("BOLDCOFF"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("GAINIMAG"));
						columnFormats_.push_back(febefeedNusebandReal.str());		// NFD × NUSBDE
						columnUnits_.push_back(string(""));

						columnNames_.push_back(string("GAINELE1"));
						columnFormats_.push_back(nusebandReal.str());						// NUSBDE
						columnUnits_.push_back(string("deg"));

						columnNames_.push_back(string("GAINELE2"));
						columnFormats_.push_back(nusebandReal.str());						// NUSBDE
						columnUnits_.push_back(string(""));

						nusebandReal.str(string());
						febefeedNusebandReal.str(string());
						febefeedDouble.str(string());
						febefeedReal.str(string());
						febefeedASCII.str(string());
						nusefeedNusebandPLong.str(string());
					}
					break;
				}
      }
			break;

		case CMBFitsWriter::ArrayData: {
        int useFeeds_n = 0;
        int channels_n = 0;
        getKeywordValue(CMBFitsWriter::ArrayData,	string("NUSEFEED"), useFeeds_n);
        getKeywordValue(CMBFitsWriter::ArrayData, string("CHANNELS"), channels_n);
        stringstream useFeedsChannels;
        useFeedsChannels << (useFeeds_n * channels_n) << string("E");

				columnNames_.push_back(string("MJD"));
				columnFormats_.push_back(string("1D"));
				columnUnits_.push_back(string("day"));

				columnNames_.push_back(string("DATA"));
				columnFormats_.push_back(useFeedsChannels.str());				// NCH × NUSFDE
				columnUnits_.push_back(string(""));

				useFeedsChannels.str(string());
			}
			break;

		case CMBFitsWriter::Monitor:
			columnNames_.push_back(string("MJD"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("day"));

			columnNames_.push_back(string("MONPOINT"));
			columnFormats_.push_back(string("30A"));
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("MONVALUE"));
			columnFormats_.push_back(string("1PD"));				// 1PD(maxelem)
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("MONUNITS"));
//			columnFormats_.push_back(string("1PA(32)"));				// 1PA(maxelem)
			columnFormats_.push_back(string("32A"));				// CCFits non supporta 1PA(...) - vedi BinTable.css righe 299-300
			columnUnits_.push_back(string(""));
			break;

		case CMBFitsWriter::DataPar:
			columnNames_.push_back(string("MJD"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("Julian day"));

			columnNames_.push_back(string("LST"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("s"));

			columnNames_.push_back(string("INTEGTIM"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("s"));

			columnNames_.push_back(string("PHASE"));
			columnFormats_.push_back(string("1J"));
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("LONGOFF"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("LATOFF"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("AZIMUTH"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("ELEVATIO"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("RA"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("DEC"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("PARANGLE"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("CBASLONG"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("CBASLAT"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("BASLONG"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("BASLAT"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("ROTANGLE"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("MCRVAL1"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("MCRVAL2"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("MLONPOLE"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("MLATPOLE"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("DFOCUS_X"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("mm"));

			columnNames_.push_back(string("DFOCUS_Y"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("mm"));

			columnNames_.push_back(string("DFOCUS_Z"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("mm"));

			columnNames_.push_back(string("DPHI_X"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("DPHI_Y"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("DPHI_Z"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("WOBDISLN"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));

			columnNames_.push_back(string("WOBDISLT"));
			columnFormats_.push_back(string("1D"));
			columnUnits_.push_back(string("deg"));
			break;

		case CMBFitsWriter::Grouping:
			columnNames_.push_back(string("MEMBER_POSITION"));
			columnFormats_.push_back(string("1J"));
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("MEMBER_LOCATION"));
			columnFormats_.push_back(string("256A"));
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("MEMBER_URI_TYPE"));
			columnFormats_.push_back(string("3A"));
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("EXTNAME"));
			columnFormats_.push_back(string("30A"));
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("SUBSNUM"));
			columnFormats_.push_back(string("1J"));
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("FEBE"));
			columnFormats_.push_back(string("30A"));
			columnUnits_.push_back(string(""));

			columnNames_.push_back(string("BASEBAND"));
			columnFormats_.push_back(string("1J"));
			columnUnits_.push_back(string(""));
			break;
	}
}

void CMBFitsWriter::setColumnUnits( const CMBFitsWriter::Table_e table_e_,
																		const string& keyword_ ) {
	if ( CMBFitsWriter::Monitor == table_e_ ) {
		CMBFitsWriter::Keyword::KeywordKeyword_ci_m_t keywordKeyword_ci = CMBFitsWriter::m_keywords.find(keyword_);
		if ( keywordKeyword_ci == CMBFitsWriter::m_keywords.end() ) {
			printf("ex : setColumnUnits, keyword %s, %s %d\n",keyword_.c_str(),__FILE__,__LINE__);
			throw exception();
		}

		setColumnValue(table_e_, string("MONUNITS"), keywordKeyword_ci->second.getUnits());
	}

/*
// Following code is to be tested as Fits library will support 1PA(...) column type
// See comments near the creation of MONUNITS

	static map<string, unsigned int> keywordRowIndex;

	if ( CMBFitsWriter::Monitor == table_e_ ) {
		CMBFitsWriter::Keyword::KeywordKeyword_ci_m_t keywordKeyword_ci = CMBFitsWriter::m_keywords.find(keyword_);
		if ( keywordKeyword_ci == CMBFitsWriter::m_keywords.end() ) throw exception();

		map<string, unsigned int>::const_iterator keywordRowIndex_ci = keywordRowIndex.find(keyword_);
		if ( keywordRowIndex_ci != keywordRowIndex.end() ) {
			setColumnValue(table_e_, string("MONUNITS"), keywordRowIndex_ci->second);
		} else {
			setColumnValue(table_e_, string("MONUNITS"), keywordKeyword_ci->second.getUnits());
			keywordRowIndex.insert(map<string, unsigned int>::value_type(keyword_, getRowIndex(table_e_)));
		}
	}
*/
}

CMBFitsWriter::Keyword::Keyword() : m_keyword(string()),
																		m_format(string()),
																		m_units(string()) {
}

CMBFitsWriter::Keyword::Keyword( const CMBFitsWriter::Keyword& keyword_ ) : m_keyword(keyword_.m_keyword),
																																						m_format(keyword_.m_format),
																																						m_units(keyword_.m_units) {
}

CMBFitsWriter::Keyword::Keyword( const string& keyword_,
																 const string& format_,
																 const string& units_) : m_keyword(keyword_),
																												 m_format(format_),
																												 m_units(units_) {
}

CMBFitsWriter::Keyword::~Keyword() {
}

bool CMBFitsWriter::Keyword::operator==( const CMBFitsWriter::Keyword& keyword_ ) const {
	return ( (m_keyword == keyword_.m_keyword	) &&
					 (m_format	== keyword_.m_format	) &&
					 (m_units		== keyword_.m_units		) );
}

bool CMBFitsWriter::Keyword::operator!=( const CMBFitsWriter::Keyword& keyword_ ) const {
	return !(*this == keyword_);
}

CMBFitsWriter::Keyword& CMBFitsWriter::Keyword::operator=( const CMBFitsWriter::Keyword& keyword_ ) {
	if ( *this != keyword_ ) {
		m_keyword = keyword_.m_keyword;
		m_format	= keyword_.m_format;
		m_units		= keyword_.m_units;
	}

	return *this;
}

string CMBFitsWriter::Keyword::getKeyword() const {
	return m_keyword;
}

string CMBFitsWriter::Keyword::getFormat()	const {
	return m_format;
}

string CMBFitsWriter::Keyword::getUnits()		const {
	return m_units;
}
