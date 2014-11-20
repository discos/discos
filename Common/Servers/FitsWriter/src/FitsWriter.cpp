#include "FitsWriter.h"
#include <Definitions.h>
#include <IRATools.h>

using namespace CCfits;
using namespace Backends;

#define _FITSW_DATACOL_TIME "time"
#define _FITSW_DATACOL_RA "raj2000"
#define _FITSW_DATACOL_DEC "decj2000"
#define _FITSW_DATACOL_AZ "az"
#define _FITSW_DATACOL_EL "el"
#define _FITSW_DATACOL_PARANG "par_angle"
#define _FITSW_DATACOL_DEROT "derot_angle"
//#define _FITSW_DATACOL_TSYS "tsys"
#define _FITSW_DATACOL_CAL "flag_cal"
#define _FITSW_DATACOL_TRACK "flag_track"
#define _FITSW_DATACOL_WEATHER "weather"

#define _FITSW_SECTCOL_ID "id"
#define _FITSW_SECTCOL_TYPE "type"
#define _FITSW_SECTCOL_SR "sampleRate"
#define _FITSW_SECTCOL_BINS "bins"
#define _FITSW_SECTCOL_FLUX "flux"

#define _FITSW_RFCOL_FEED "feed"
#define _FITSW_RFCOL_IF "ifChain"
#define _FITSW_RFCOL_POL "polarization"
#define _FITSW_RFCOL_FREQ "frequency"
#define _FITSW_RFCOL_BW "bandWidth"
#define _FITSW_RFCOL_LO "localOscillator"
#define _FITSW_RFCOL_ATT "attenuation"
#define _FITSW_RFCOL_CAL "calibratonMark"
#define _FTISW_RFCOL_SECT "section"



CFitsWriter::CFitsWriter()
{
	m_filename = "out.fits";
	m_basepath = "~/";
	m_channels = NULL;
	pFits=NULL;
	m_mainHeaderSet=false;
	
	data_table=section_table=rfInput_table=feed_table=tsys_table=servo_table=NULL;
	next_row = 1;
	next_servo_table_row = 1;
	data_column = _FW_DATAHEADER_ELEMENTS; //the first column containing channel data in data table
	feed_number = 1;	
	
	SectColName.push_back(_FITSW_SECTCOL_ID);
	SectColName.push_back(_FITSW_SECTCOL_TYPE);
	SectColName.push_back(_FITSW_SECTCOL_SR);
	SectColName.push_back(_FITSW_SECTCOL_BINS);
	SectColName.push_back(_FITSW_SECTCOL_FLUX);

	SectColForm.push_back("J");
	SectColForm.push_back("6A");
	SectColForm.push_back("D");
	SectColForm.push_back("J");
	SectColForm.push_back("D");

	SectColUnit.push_back("");
	SectColUnit.push_back("");
	SectColUnit.push_back("MHz");
	SectColUnit.push_back("");
	SectColForm.push_back("Jy");
	
	rfColName.push_back(_FITSW_RFCOL_FEED);
	rfColName.push_back(_FITSW_RFCOL_IF);
	rfColName.push_back(_FITSW_RFCOL_POL);
	rfColName.push_back(_FITSW_RFCOL_FREQ);
	rfColName.push_back(_FITSW_RFCOL_BW);
	rfColName.push_back(_FITSW_RFCOL_LO);
	rfColName.push_back(_FITSW_RFCOL_ATT);
	rfColName.push_back(_FITSW_RFCOL_CAL);
	rfColName.push_back(_FTISW_RFCOL_SECT);
	
	rfColForm.push_back("J");
	rfColForm.push_back("J");
	rfColForm.push_back("8A");
	rfColForm.push_back("D");
	rfColForm.push_back("D");
	rfColForm.push_back("D");
	rfColForm.push_back("D");
	rfColForm.push_back("D");
	rfColForm.push_back("J");
	
	rfColUnit.push_back("");
	rfColUnit.push_back("");
	rfColUnit.push_back("");
	rfColUnit.push_back("MHz");
	rfColUnit.push_back("MHz");
	rfColUnit.push_back("MHz");
	rfColUnit.push_back("db");
	rfColUnit.push_back("K");
	rfColUnit.push_back("");

	DataColName.push_back(_FITSW_DATACOL_TIME);
	DataColName.push_back(_FITSW_DATACOL_RA);
	DataColName.push_back(_FITSW_DATACOL_DEC);
	DataColName.push_back(_FITSW_DATACOL_AZ);
	DataColName.push_back(_FITSW_DATACOL_EL);
	DataColName.push_back(_FITSW_DATACOL_PARANG);
	DataColName.push_back(_FITSW_DATACOL_DEROT);
	//DataColName.push_back(_FITSW_DATACOL_TSYS);
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
	//DataColForm.push_back("D");
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
	//DataColUnit.push_back("K");
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
	SectColName.clear();
	SectColForm.clear();
	SectColUnit.clear();
	rfColName.clear();
	rfColForm.clear();
	rfColUnit.clear();
	DataColName.clear();
	DataColForm.clear();
	DataColUnit.clear();
	FeedColName.clear();
	FeedColForm.clear();
	FeedColUnit.clear();
	tsysColName.clear();
	tsysColForm.clear();
	tsysColUnit.clear();
	servoColName.clear();
	servoColForm.clear();
	servoColUnit.clear();
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

void CFitsWriter::setFileName(const char* name)
{
	m_filename = name;
};

void CFitsWriter::setBasePath(const char* path)
{
	m_basepath = path;
};

bool CFitsWriter::create()
{
	IRA::CString fullName=m_basepath+m_filename;
	try {
		pFits = new FITS((const char *)fullName, Write);
		setPrimaryHeaderComment(DEFAULT_COMMENT);
		pFits->pHDU().writeHistory(HISTORY1);
		pFits->pHDU().writeHistory(HISTORY2);
		pFits->pHDU().writeHistory(HISTORY3);
		pFits->pHDU().writeHistory(HISTORY4);
		pFits->pHDU().writeHistory(HISTORY5);
		pFits->pHDU().writeHistory(HISTORY6);
		pFits->pHDU().writeHistory(HISTORY7);
		pFits->pHDU().writeDate();
	}
	catch (CCfits::FitsException& ex) {
		m_lastError = ex.message().c_str();
		return false;
	}
	return true;
};

bool CFitsWriter::create(const char* name)
{
	setFileName(name);
	return create();
};

bool CFitsWriter::create(const char* name, const char* path)
{
	setBasePath(path);
	setFileName(name);
	return create();
};

bool CFitsWriter::setPrimaryHeaderComment(const IRA::CString& comment)
{
	if (!pFits) {
		m_lastError= "fits file is not created";
		return false;
	}
	try {
		pFits->pHDU().writeComment((const char *)comment);
	}
	catch (CCfits::FitsException& ex) {
		m_lastError = ex.message().c_str();
		return false;
	}
	return true;
};

bool CFitsWriter::saveMainHeader(const TMainHeader& tmh)
{
	m_mainHeader = tmh;
	// creates the required number of slots to stores channels information
	m_channels = new TSectionHeader[tmh.sections];
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

bool CFitsWriter::saveSectionHeader(const TSectionHeader* tch)
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

bool CFitsWriter::storeServoData(const double& time,const ACS::doubleSeq& pos)
{
	if (servo_table) {
		double app;
		try {
			app=time;
			servo_table->column(1).write(&app,1,next_servo_table_row);
			for (unsigned k=0;k<pos.length();k++) {
				app=pos[k];
				servo_table->column(2+k).write(&app,1,next_servo_table_row);
			}
		}
		catch(FitsException& fe) {
			m_lastError = fe.message().c_str();
			return false;
		}
		return true;
	}
	else {
		m_lastError= "minor servo data table not available";
		return false;
	}
}

bool CFitsWriter::storeAuxData(TDataHeader& dh,double * tsys) {
	try{
		if (data_table) {
			std::valarray<double> wData(dh.weather,3);
			//std::valarray<double> tsysData(tsys,m_mainHeader.sections);
			std::vector<std::valarray<double> > wDataVector;
			//std::vector<std::valarray<double> > tsysDataVector;
			data_table->column(_FITSW_DATACOL_TIME).write(&(dh.time), 1, next_row);
			data_table->column(_FITSW_DATACOL_RA).write(&(dh.raj2000), 1, next_row);
			data_table->column(_FITSW_DATACOL_DEC).write(&(dh.decj2000), 1, next_row);
			data_table->column(_FITSW_DATACOL_AZ).write(&(dh.az), 1, next_row);
			data_table->column(_FITSW_DATACOL_EL).write(&(dh.el), 1, next_row);
			data_table->column(_FITSW_DATACOL_PARANG).write(&(dh.par_angle), 1, next_row);
			data_table->column(_FITSW_DATACOL_DEROT).write(&(dh.derot_angle), 1, next_row);
			data_table->column(_FITSW_DATACOL_CAL).write(&(dh.flag_cal), 1, next_row);
			data_table->column(_FITSW_DATACOL_TRACK).write(&(dh.flag_track), 1, next_row);
			wDataVector.push_back(wData);
			//tsysDataVector.push_back(tsysData);
			data_table->column(_FITSW_DATACOL_WEATHER).writeArrays(wDataVector, next_row);
			//data_table->column(_FITSW_DATACOL_TSYS).writeArrays(tsysDataVector,next_row);
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
	try {
		if (tsys_table) {
			double *temp=tsys;
			std::vector<std::valarray<double> > tsysDataVector;
			
			for (int i=0;i<m_mainHeader.sections;i++) {
				if (m_channels[i].inputs>1) {
					tsysDataVector.clear();
					std::valarray<double> tsysData(temp,m_channels[i].inputs);
					temp+=m_channels[i].inputs;
					tsysDataVector.push_back(tsysData);
					tsys_table->column(1+i).writeArrays(tsysDataVector,next_row);  // column index start from 1
				}
				else {
					tsys_table->column(1+i).write(temp,1,next_row);  // column index start from 1
					temp++;
				}
			}
		}
		else {
			m_lastError= "tsys table not available";
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

bool CFitsWriter::addDataTable(const IRA::CString& dataName,const IRA::CString tsysName)
{
	if (!pFits) {
		m_lastError = "fits file is not created";
		return false;		
	}
	try{
		data_table = pFits->addTable((const char *)dataName,0,DataColName,DataColForm,DataColUnit);	
	}
	catch(FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	catch(...) {
		m_lastError = "cannot create data table";
		return false;
	}
	try{
		tsys_table = pFits->addTable((const char *)tsysName,0,tsysColName,tsysColForm,tsysColUnit);	
	}
	catch(FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	catch(...) {
		m_lastError = "cannot create tsys table";
		return false;
	}	
	return true;
};

bool CFitsWriter::addServoTable(const ACS::stringSeq &axisName,const ACS::stringSeq& axisUnit,const IRA::CString name)
{
	if (!pFits) {
		m_lastError = "cannot create servo table: no main header found";
		return false;
	}
	if (!data_table) {
		m_lastError= "data table not available";
		return false;
	}
	unsigned dim=axisName.length();
	servoColName.clear();
	servoColUnit.clear();
	servoColForm.clear();
	servoColName.push_back(_FITSW_DATACOL_TIME);
	servoColUnit.push_back("MJD");
	servoColForm.push_back("D");
	for (unsigned k=0;k<dim;k++) {
		servoColName.push_back((const char *)axisName[k]);
		servoColUnit.push_back((const char *)axisUnit[k]);
		servoColForm.push_back("D");
	}
	try{
		servo_table=pFits->addTable((const char *)name,0,servoColName,servoColForm,servoColUnit);
		return true;
	}
	catch(FitsException& fe){
		m_lastError = fe.message().c_str();
		return false;
	}
	catch(...) {
		m_lastError = "cannot create servo table";
		return false;
	}
}


bool CFitsWriter::addFeedTable(const IRA::CString& name)
{
	if (!pFits) {
		m_lastError = "cannot create feed table: no main header found";
		return false;		
	}
	try{
		feed_table = pFits->addTable((const char *)name, 0, FeedColName, FeedColForm, FeedColUnit);
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

bool CFitsWriter::addSectionTable(const ACS::longSeq &sectionID, const ACS::longSeq& feedsID, const ACS::longSeq& ifsID,const ACS::longSeq& pols,const ACS::doubleSeq& los,
		const ACS::doubleSeq& skyFreq,const ACS::doubleSeq& skyBandWidth,const ACS::doubleSeq& marks,const ACS::doubleSeq& sourceFlux,const ACS::doubleSeq& atts,
		const IRA::CString& name,const IRA::CString& rfName)
{
	std::vector<long> id;
	std::vector<long> bins;
	std::vector<double> sampleRate;
	std::vector<double> flux;
	std::vector<string> sect_type;	
	std::vector<long> feed;
	std::vector<long> ifChain;
	std::vector<string> polarization;
	std::vector<double> bandWidth;
	std::vector<double> frequency;
	std::vector<double> localOscillator;
	std::vector<double> attenuation;
	std::vector<double> calib;
	std::vector<long> section;
	long inputsNumber=0;
	long fluxIterator=0;
	//double skyFreq,skyBw;
	
	if (!pFits) {
		m_lastError = "fits file is not created";
		return false;		
	}
	if(!m_channels){
		m_lastError = "no channels information found";
		return false;
	}
	for(int i = 0; i < m_mainHeader.sections; ++i) {
		std::stringstream colName, type, tsysType;
		id.push_back(m_channels[i].id);  // Section ID
		bins.push_back(m_channels[i].bins); //Section Bins
		sampleRate.push_back(m_channels[i].sampleRate); // section S.R.
		if (fluxIterator<(long)sourceFlux.length()) {
			flux.push_back(sourceFlux[fluxIterator]);// estimated source flux associated to the section parameters
		}
		fluxIterator+=m_channels[i].inputs;
		// creation of the channels columns for the data table......
		colName << "Ch" << m_channels[i].id;
	    DataColName.push_back(colName.str());		
		if (m_channels[i].polarization==Backends::BKND_FULL_STOKES) {
			sect_type.push_back("stokes");
			type << (4 * bins[i]) << data_type; //full stokes
		}
		else {
			sect_type.push_back("simple");
			type <<  (1 * bins[i]) << data_type;
		}
		DataColForm.push_back(type.str());
		DataColUnit.push_back("");

		if (m_channels[i].inputs>1) {
			tsysType << (m_channels[i].inputs) << "D";;
			tsysColForm.push_back(tsysType.str());
		}
		else {
			tsysColForm.push_back("D");
		}
		tsysColName.push_back(colName.str());
		tsysColUnit.push_back("K");
	}
	inputsNumber=sectionID.length();
	for (unsigned j=0;j<sectionID.length();j++) {
		if (j<feedsID.length()) {
			feed.push_back(feedsID[j]);
		}
		else {
			feed.push_back(0);
		}
		if (j<ifsID.length()) {
			ifChain.push_back(ifsID[j]);
		}
		else {
			ifChain.push_back(0);
		}
		if (j<pols.length()) {
			if (pols[j]==Receivers::RCV_LCP) polarization.push_back("LCP");
			else if (pols[j]==Receivers::RCV_RCP) polarization.push_back("RCP");
			else if (pols[j]==Receivers::RCV_VLP) polarization.push_back("VLP");
			else if (pols[j]==Receivers::RCV_HLP) polarization.push_back("HLP");
			else polarization.push_back("");
		}
		else {
			polarization.push_back("");
		}
		if (j<skyFreq.length()) {
			frequency.push_back(skyFreq[j]);
		}
		else {
			frequency.push_back(DOUBLENULLVALUE);
		}
		if (j<skyBandWidth.length()) {
			bandWidth.push_back(skyBandWidth[j]);
		}
		else {
			bandWidth.push_back(DOUBLENULLVALUE);
		}
		if (j<los.length()) {
			localOscillator.push_back(los[j]);
		}
		else {
			localOscillator.push_back(DOUBLENULLVALUE);
		}
		if (j<marks.length()) { // in this case the noise cal values are given one for each backend input
			calib.push_back(marks[j]);
		}
		else {
			calib.push_back(DOUBLENULLVALUE);
		}
		if (j<atts.length()) {
			attenuation.push_back(atts[j]);
		}
		else {
			attenuation.push_back(DOUBLENULLVALUE);
		}
		section.push_back(sectionID[j]);
	}

		/*for (long j=0;((j<m_channels[i].inputs) && (j<2));j++) {
			inputsNumber++;
			unsigned ifNumber;
			ifNumber=(unsigned) m_channels[i].IF[j];
			ifChain.push_back(ifNumber);
			feed.push_back(m_channels[i].feed);
			if (ifNumber<pols.length()) { // in this case the polarizations are given one for each receiver IF
				if (pols[ifNumber]==Receivers::RCV_LCP) polarization.push_back("LCP");
				else if (pols[ifNumber]==Receivers::RCV_RCP) polarization.push_back("RCP");
				else if (pols[ifNumber]==Receivers::RCV_VLP) polarization.push_back("VLP");
				else if (pols[ifNumber]==Receivers::RCV_HLP) polarization.push_back("HLP");
				else polarization.push_back("");
			}
			else {
				polarization.push_back("");
			}
			if (i<(long)skyFreq.length()) {
				frequency.push_back(skyFreq[i]);
			}
			else {
				frequency.push_back(DOUBLENULLVALUE);
			}
			if (i<(long)skyBandWidth.length()) {
				bandWidth.push_back(skyBandWidth[i]);
			}
			else {
				bandWidth.push_back(DOUBLENULLVALUE);
			}
			if (ifNumber<los.length()) {
				localOscillator.push_back(los[ifNumber]);
			}
			else {
				localOscillator.push_back(DOUBLENULLVALUE);
			}
			if ((unsigned long)inputsNumber<=marks.length()) { // in this case the noise cal values are given one for each backend input
				calib.push_back(marks[inputsNumber-1]);
			}
			else {
				calib.push_back(DOUBLENULLVALUE);
			}
			attenuation.push_back(m_channels[i].attenuation[j]);
			section.push_back(m_channels[i].id);
		}*/
		/*if (m_channels[i].inputs>1) {
			tsysType << (m_channels[i].inputs) << "D";;
			tsysColForm.push_back(tsysType.str());
		}
		else {
			tsysColForm.push_back("D");
		}
		tsysColName.push_back(colName.str());
		tsysColUnit.push_back("K");*/
	/*}*/
	try {
		section_table = pFits->addTable((const char *)name,m_mainHeader.sections, SectColName, SectColForm, SectColUnit);
		if (!section_table) {
			m_lastError = "section table could not be created";
			return false;
		}		
		section_table->column(_FITSW_SECTCOL_ID).write<long>(id,0);
		section_table->column(_FITSW_SECTCOL_TYPE).write(sect_type,1);
		section_table->column(_FITSW_SECTCOL_SR).write(sampleRate,1);		
		section_table->column(_FITSW_SECTCOL_BINS).write(bins,1);
		section_table->column(_FITSW_SECTCOL_FLUX).write(flux,1);
	}
	catch(FitsException& fe){
		m_lastError = fe.message().c_str();
		return false;
	}
	catch(...){
		m_lastError = "section table could not be created";
		return false;
	}
	try {
		rfInput_table = pFits->addTable((const char *)rfName,inputsNumber,rfColName,rfColForm,rfColUnit);
		if (!rfInput_table) {
			m_lastError = "RF inputs table could not be created";
			return false;
		}
		rfInput_table->column(_FITSW_RFCOL_FEED).write(feed,1);
		rfInput_table->column(_FITSW_RFCOL_IF).write(ifChain,1);
		rfInput_table->column(_FITSW_RFCOL_POL).write(polarization,1);
		rfInput_table->column(_FITSW_RFCOL_FREQ).write(frequency,1);
		rfInput_table->column(_FITSW_RFCOL_BW).write(bandWidth,1);
		rfInput_table->column(_FITSW_RFCOL_LO).write(localOscillator,1);
		rfInput_table->column(_FITSW_RFCOL_ATT).write(attenuation,1);		
		rfInput_table->column(_FITSW_RFCOL_CAL).write(calib, 1);
		rfInput_table->column(_FTISW_RFCOL_SECT).write(section,1);
	}
	catch(FitsException& fe){
		m_lastError = fe.message().c_str();
		return false;
	}
	catch(...){
		m_lastError = "RF inputs table could not be created";
		return false;
	}
	return true;
};

