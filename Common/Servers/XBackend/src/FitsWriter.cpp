#include "FitsWriter.h"

using namespace CCfits;

#define _FITSW_DATACOL_TIME "time"
#define _FITSW_DATACOL_RA "raj2000"
#define _FITSW_DATACOL_DEC "decj2000"
#define _FITSW_DATACOL_AZ "az"
#define _FITSW_DATACOL_EL "el"
#define _FITSW_DATACOL_PARANG "par_angle"
#define _FITSW_DATACOL_DEROT "derot_angle"
#define _FITSW_DATACOL_TSYS "tsys"
#define _FITSW_DATACOL_CAL "flag_cal"
#define _FITSW_DATACOL_TRACK "flag_track"
#define _FITSW_DATACOL_WEATHER "weather"

#define _FITSW_CHCOL_ID "id"
#define _FITSW_CHCOL_BINS "bins"
#define _FITSW_CHCOL_POL "polarization"
#define _FITSW_CHCOL_BW "bandWidth"
#define _FITSW_CHCOL_FREQ "frequency"
#define _FITSW_CHCOL_ATT "attenuation"
#define _FITSW_CHCOL_SR "sampleRate"
#define _FITSW_CHCOL_FEED "feedNumber"


CFitsWriter::CFitsWriter()
{
	m_filename = "out.fits";
	m_basepath = "~/";
	m_channels = NULL;
	pFits=NULL;
	m_mainHeaderSet=false;
	
	data_table=channel_table=feed_table=NULL;
	next_row = 1;
	data_column = _FW_DATAHEADER_ELEMENTS; //the first column containing channel data in data table
	feed_number = 1;	
	
	ChColName.push_back(_FITSW_CHCOL_ID);
	ChColName.push_back(_FITSW_CHCOL_BINS);
	ChColName.push_back(_FITSW_CHCOL_POL);
	ChColName.push_back(_FITSW_CHCOL_BW);
	ChColName.push_back(_FITSW_CHCOL_FREQ);
	ChColName.push_back(_FITSW_CHCOL_ATT);
	ChColName.push_back(_FITSW_CHCOL_SR);
	ChColName.push_back(_FITSW_CHCOL_FEED);

	ChColForm.push_back("J");
	ChColForm.push_back("J");
	ChColForm.push_back("J");
	ChColForm.push_back("D");
	ChColForm.push_back("D");
	ChColForm.push_back("D");
	ChColForm.push_back("D");
	ChColForm.push_back("J");

	ChColUnit.push_back("");
	ChColUnit.push_back("");
	ChColUnit.push_back("");
	ChColUnit.push_back("MHz");
	ChColUnit.push_back("MHz");
	ChColUnit.push_back("db");
	ChColUnit.push_back("MHz");
	ChColUnit.push_back("");

	DataColName.push_back(_FITSW_DATACOL_TIME);
	DataColName.push_back(_FITSW_DATACOL_RA);
	DataColName.push_back(_FITSW_DATACOL_DEC);
	DataColName.push_back(_FITSW_DATACOL_AZ);
	DataColName.push_back(_FITSW_DATACOL_EL);
	DataColName.push_back(_FITSW_DATACOL_PARANG);
	DataColName.push_back(_FITSW_DATACOL_DEROT);
	DataColName.push_back(_FITSW_DATACOL_TSYS);
	DataColName.push_back(_FITSW_DATACOL_CAL);
	DataColName.push_back(_FITSW_DATACOL_TRACK);
	DataColName.push_back(_FITSW_DATACOL_WEATHER);

	DataColForm.push_back("D");
	DataColForm.push_back("D");
	DataColForm.push_back("D");
	DataColForm.push_back("D");
	DataColForm.push_back("D");
	DataColForm.push_back("D");
	DataColForm.push_back("D");
	DataColForm.push_back("D");
	DataColForm.push_back("J");
	DataColForm.push_back("J");
	DataColForm.push_back("3D");

	DataColUnit.push_back("MJD");
	DataColUnit.push_back("radians");
	DataColUnit.push_back("radians");
	DataColUnit.push_back("radians");
	DataColUnit.push_back("radians");
	DataColUnit.push_back("radians");
	DataColUnit.push_back("radians");
	DataColUnit.push_back("K");
	DataColUnit.push_back("");
	DataColUnit.push_back("");
	DataColUnit.push_back("");
	
	FeedColName.push_back("id");
	FeedColName.push_back("xOffset");
	FeedColName.push_back("yOffset");
	FeedColName.push_back("relativePower");

	FeedColForm.push_back("J");
	FeedColForm.push_back("D");
	FeedColForm.push_back("D");
	FeedColForm.push_back("D");

	FeedColUnit.push_back("");
	FeedColUnit.push_back("");
	FeedColUnit.push_back("");
	FeedColUnit.push_back("");
};

CFitsWriter::~CFitsWriter()
{
	if (m_channels!=NULL) delete[] m_channels;
	if (pFits!=NULL) delete pFits;
	ChColName.clear();
	ChColForm.clear();
	ChColUnit.clear();
	DataColName.clear();
	DataColForm.clear();
	DataColUnit.clear();
	FeedColName.clear();
	FeedColForm.clear();
	FeedColUnit.clear();
};

bool CFitsWriter::flush()
{
	if (!pFits) {
		m_lastError = "fits file is not created";
		return false;
	}
	try{
		pFits->flush();
		return true;
	}
	catch(FitsException& fe){
		m_lastError = fe.message().c_str();
		return false;
	}
};

void CFitsWriter::setFileName(const char *name)
{
	m_filename = name;
};

void CFitsWriter::setBasePath(const char *path)
{
	m_basepath = path;
};

bool CFitsWriter::create()
{
	std::string fullName=m_basepath+m_filename;
	try {
		pFits = new FITS(fullName.c_str(), Write);
		setPrimaryHeaderComment(DEFAULT_COMMENT);
		pFits->pHDU().writeHistory(HISTORY1);
		pFits->pHDU().writeDate();
	}
	catch (CCfits::FitsException& ex) {
		m_lastError = ex.message().c_str();
		return false;
	}
	return true;
};

bool CFitsWriter::create(const char *name)
{
	setFileName(name);
	return create();
};

bool CFitsWriter::create(const char *name,const char *path)
{
	setBasePath(path);
	setFileName(name);
	return create();
};

bool CFitsWriter::setPrimaryHeaderComment(const std::string& comment)
{
	if (!pFits) {
		m_lastError= "fits file is not created";
		return false;
	}
	try {
		pFits->pHDU().writeComment(comment);
	}
	catch (CCfits::FitsException& ex) {
		m_lastError = ex.message().c_str();
		return false;
	}
	return true;
};

bool CFitsWriter::saveMainHeader(const Backends::TMainHeader& tmh)
{
	m_mainHeader = tmh;
	// creates the required number of slots to stores channels information
	m_channels = new Backends::TSectionHeader[tmh.sections];
	if (!setPrimaryHeaderKey("beams", tmh.beams, "number of beams")) return false; //last error set in setPrimaryHeaderKey
	if (!setPrimaryHeaderKey("integration", tmh.integration, "integration time in milliseconds")) return false;
	if (!setPrimaryHeaderKey("channels", tmh.sections, "total number of channels")) return false;
	if (!setPrimaryHeaderKey("sample size", tmh.sampleSize, "number of bytes of a single sample")) return false;
	//SETTING THE DATA TYPE FOR DATA TABLE
	switch(tmh.sampleSize){
		case 2:
			data_type = "J";
			break;
		case 4:
			data_type = "E";
			break;
		default:
			data_type = "D";
			break; 
	}
	m_mainHeaderSet=true;
	return true;
};

bool CFitsWriter::saveChannelHeader(const Backends::TSectionHeader* tch)
{
	if (!m_mainHeaderSet) {
		m_lastError="main header has not been saved";
		return false;
	}
	for (int i=0;i<m_mainHeader.sections;i++) {
		m_channels[i]=tch[i];
	}
	return true;
};

bool CFitsWriter::storeAuxData(TDataHeader& dh){
	try{
		if (data_table) {
			data_table->column(_FITSW_DATACOL_TIME).write(&(dh.time), 1, next_row);
			data_table->column(_FITSW_DATACOL_RA).write(&(dh.raj2000), 1, next_row);
			data_table->column(_FITSW_DATACOL_DEC).write(&(dh.decj2000), 1, next_row);
			data_table->column(_FITSW_DATACOL_AZ).write(&(dh.az), 1, next_row);
			data_table->column(_FITSW_DATACOL_EL).write(&(dh.el), 1, next_row);
			data_table->column(_FITSW_DATACOL_PARANG).write(&(dh.par_angle), 1, next_row);
			data_table->column(_FITSW_DATACOL_DEROT).write(&(dh.derot_angle), 1, next_row);
			data_table->column(_FITSW_DATACOL_TSYS).write(&(dh.tsys), 1, next_row);
			data_table->column(_FITSW_DATACOL_CAL).write(&(dh.flag_cal), 1, next_row);
			data_table->column(_FITSW_DATACOL_TRACK).write(&(dh.flag_track), 1, next_row);
			std::valarray<double> data(dh.weather, 3);
			std::vector<std::valarray<double> > vdata;
			vdata.push_back(data);
			data_table->column(_FITSW_DATACOL_WEATHER).writeArrays(vdata, next_row);
		}
		else {
			m_lastError= "data table not available";
			return false;
		}
	}
	catch(FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	return true;
};

bool CFitsWriter::saveFeedHeader(TFeedHeader& fh) {
	if (!feed_table) {
		m_lastError = "no feed table found";
		return false;
	}
	try {
		feed_table->column(FeedColName[0]).write(&(fh.id),1,feed_number);
		feed_table->column(FeedColName[1]).write(&(fh.xOffset),1,feed_number);
		feed_table->column(FeedColName[2]).write(&(fh.yOffset),1,feed_number);
		feed_table->column(FeedColName[3]).write(&(fh.relativePower),1,feed_number);
		feed_number++;
	}
	catch(FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	return true;
};

bool CFitsWriter::addDataTable(const char *name)
{
	if (!pFits) {
		m_lastError = "fits file is not created";
		return false;		
	}
	try{
		data_table = pFits->addTable(name,0,DataColName,DataColForm,DataColUnit);
		return true;	
	}
	catch(FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	catch(...) {
		m_lastError = "cannot create channels table";
		return false;
	}
};

bool CFitsWriter::addFeedTable(const char *name)
{
	if (!pFits) {
		m_lastError = "cannot create feed table: no main header found";
		return false;		
	}
	try{
		feed_table = pFits->addTable(name, 0, FeedColName, FeedColForm, FeedColUnit);
		return true;
	}
	catch(FitsException& fe){
		m_lastError = fe.message().c_str();
		return false;
	}
	catch(...) {
		m_lastError = "cannot create feed table";
		return false;
	}
};

bool CFitsWriter::addChannelTable(const char *name)
{
	std::vector<long> id;
	std::vector<long> bins;
	std::vector<long> polarization;
	std::vector<double> bandWidth;
	std::vector<double> frequency;
	std::vector<double> attenuation;
	std::vector<double> sampleRate;
	std::vector<long> feedNumber;
	
	if (!pFits) {
		m_lastError = "fits file is not created";
		return false;		
	}
	if(!m_channels){
		m_lastError = "no channels information found";
		return false;
	}
	for(int i = 0; i < m_mainHeader.sections; ++i) {
		std::stringstream colName, type;
//		std::string str[]={"xx","yy","Rexy","Imxy"};
		id.push_back(m_channels[i].id);
		bins.push_back(m_channels[i].bins);
		polarization.push_back(m_channels[i].polarization);
		bandWidth.push_back(m_channels[i].bandWidth);
		frequency.push_back(m_channels[i].frequency);
		attenuation.push_back(m_channels[i].attenuation[0]);
		sampleRate.push_back(m_channels[i].sampleRate);
		feedNumber.push_back(m_channels[i].feed);
//Sto sostituendo***************		
		colName << "Ch" << m_channels[i].id;
	    DataColName.push_back(colName.str());
		if((polarization[i] == 0)||(polarization[i] == 1))
			type <<  (1 * bins[i]) << data_type;
		else
			type << (4 * bins[i]) << data_type; //full stokes
		DataColForm.push_back(type.str());
		DataColUnit.push_back("");
//*************con		
//		type <<  (1 * bins[i]) << data_type;
//		colName << "Ch" << m_channels[i].id;
//		if((polarization[i] == 0)||(polarization[i] == 1)){
//		    DataColName.push_back(colName.str());
//		    DataColForm.push_back(type.str());
//			DataColUnit.push_back("");
//		}
//		else{
//			for (int j=0;j<4;j++){	
//				std::stringstream colNameT;
//				colNameT<<	"Ch" << m_channels[i].id << str[j];			
//				DataColName.push_back(colNameT.str());
//				DataColForm.push_back(type.str());
//				DataColUnit.push_back("");
//			}
//		}
//fino a qui**************************************+++++
	}
	try {
		channel_table = pFits->addTable(name,m_mainHeader.sections, ChColName, ChColForm, ChColUnit);
		if (!channel_table) {
			m_lastError = "channel table could not be created";
			return false;
		}
//		channel_table->column(_FITSW_CHCOL_ID).write<long>(id,0);
//		channel_table->column(_FITSW_CHCOL_BINS).write(bins, 1);
//		channel_table->column(_FITSW_CHCOL_POL).write(polarization, 1);
//		channel_table->column(_FITSW_CHCOL_BW).write(bandWidth, 1);
//		channel_table->column(_FITSW_CHCOL_FREQ).write(frequency, 1);
//		channel_table->column(_FITSW_CHCOL_ATT).write(attenuation, 1);
//		channel_table->column(_FITSW_CHCOL_SR).write(sampleRate, 1);
//		channel_table->column(_FITSW_CHCOL_FEED).write(feedNumber, 1);
		return true;
	}
	catch(FitsException& fe){
		m_lastError = fe.message().c_str();
		return false;
	}
	catch(...){
		m_lastError = "channel table could not be created";
		return false;
	}
};
