#ifndef _FITSWRITER_
#define _FITSWRITER_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: FitsWriter.h,v 1.10 2011-02-28 17:46:12 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  when                       What                                                       */
/* Marco Bartolini		  						  24/01/2009      Creation                                                  */
/* Andrea Orlati (a.orlati@ira.inaf.it)   12/03/2009      Reviewed some parts                           */
/* Andrea Orlati (a.orlati@ira.inaf.it)   15/11/2009      added the tys information for each channel    */
/* Andrea Orlati (a.orlati@ira.inaf.it)   12/09/2010      version 0.9 ready, many keyword added, salient tables rearranged and splitted into different tables    */
/* Andrea Orlati (a.orlati@ira.inaf.it)   28/02/2011      the "RF INPUTS" table now reports the observed frequencies  */

//Standard Imports
#include <sstream>
#include <memory>
#include <valarray>

//External Library Imports
#include <CCfits>
#include <String.h>

//Project Imports
#include <BackendsDefinitionsC.h>
#include <ReceiversDefinitionsC.h>

#define FITS_VERSION1 "V. 0.8"
#define FITS_VERSION2 "V.0.82"
#define FITS_VERSION3 "V.0.9"
#define FITS_VERSION4 "V.0.91"
#define FITS_VERSION5 "V.0.92"
#define FITS_VERSION6 "V.1.0"

#define CURRENT_VERSION FITS_VERSION6

#define DEFAULT_COMMENT CURRENT_VERSION" Created by  S. Righini, M. Bartolini  & A. Orlati"

#define HISTORY1 FITS_VERSION1" First output standard for Italian radiotelescopes"
#define HISTORY2 FITS_VERSION2" The tsys column in data table raplaced with the Tant table, it reports the tsys measurement for each input of each section" 
#define HISTORY3 FITS_VERSION3" The section table has been splitted into two tables: sections and rf inputs table"
#define HISTORY4 FITS_VERSION4" Added the flux column in section table"
#define HISTORY5 FITS_VERSION5" SubScanType added as primary header keyword"
#define HISTORY6 FITS_VERSION6" Added new table to store position of subriflector e primary focus receivers: SERVO TaBLE"


/**
 * This class is very specific and it used to create a fits file from the data coming from backends that validates the <i>Backends::GenericBackends</i> interface
 */
class CFitsWriter{
public:
	/**
	 * structure that contains the auxiliary data that will compose one single row of the data table
	 */
	struct TDataHeader {
		double time;
		double raj2000;
	    double decj2000;
	    double az;
	    double el;
	    double par_angle;
	    double derot_angle;
	    //double tsys;
	    int flag_cal;
	    int flag_track;
	    double weather[3];
	};
	// This number must reflect the exact number of fields (column) in the data table before the single channel raw data
#define _FW_DATAHEADER_ELEMENTS 11 
	/**
	 * structure used to pass to the class the informations regading the feeds
	 */
	struct TFeedHeader {
		int id;
		double xOffset;
		double yOffset;
		double relativePower;
	};

public:
	/**
	 * Constructor.
	 */
	CFitsWriter();
	/**
	 * Destructor.
	 */
	virtual ~CFitsWriter();
	
	/**
	 * Allows to set the name of the fits file.
	 * @param name string that containes the file name
	 */
	void setFileName(const char* name);
	
	/**
	 * Allows to set the base path (excluded the file name) of the fits file
	 * @param path string that containes the 
	 */	
	void setBasePath(const char* path);
	
	/**
	 * It flushes all pending and unsaved data
	 * @return false if the operation could not be completed
	 */
	bool flush();
	
	/**
	 * It creates the file in the disk, using current name and path
	 * @return false if the creation fails.
	 */
	bool create();
	/**
	 * It creates the file in the disk, using current name and path
	 * @param name it allows to provide a new file name
	 * @return false if the creation fails.
	 */
	bool create(const char* name);

	/**
	 * It creates the file in the disk, using the given name and path. The fully qualified file name is path+name.
	 * @param name it allows to provide a new file name
	 * @param path it allows to provide the base path
	 * @return false if the creation fails.
	 */
	bool create(const char* name,const char* path);
	
	/**
	 * It allows to store information coming from the main header of the data flow
	 * @param tmh reference to the main header structure
	 * @return false if the operation fails
	 */ 
	bool saveMainHeader(const Backends::TMainHeader& tmh);
	
	/**
	 * It allows to store the data regarding the sections. It expects exactly the number of channels that was declared in the main header structure
	 * @param tch reference to the channel header structure
	 * @return false if the operation fails
	 */
	bool saveSectionHeader(const Backends::TSectionHeader* tch);
	
	/**
	 * It allows to save the feed header and store them into the fits file
	 * @param fh reference to one  feed header
	 * @return false if the operation fails
	 */
	bool saveFeedHeader(TFeedHeader& fh);
	
	/**
	 * It allows to add the sections and rfInput table to the fits file
	 * @param sectionID identifier of the backend sections
	 * @param feedsIF identifiers of the receivers feeds
	 * @param ifsIF identifiers of the receivers IF
	 * @param pols for each receiver IF gives its polarization
	 * @param los for IF gives its local oscillator value in MHz
	 * @param skyFreq for each backend section it gives the resulting observed sky frequency (MHz)
	 * @param skyBandWidth for each backend section it gives the value of the band width (MHz)
	 * @param marks for each RF input gives the value of the noise calibration mark. (°K) 
	 * @param sourceFlux for each backend section it reports the estimated source flux computed with section parameters
	 * @param atts attenuation, one for each input of the backend
	 * @param name name of the sections table
	 * @param rfName name of the RF inputs table
	 * @return false if the operation is not successful
	 */
	bool addSectionTable(const ACS::longSeq &sectionID, const ACS::longSeq& feedsID, const ACS::longSeq& ifsID,const ACS::longSeq& pols,const ACS::doubleSeq& los,
			const ACS::doubleSeq& skyFreq,const ACS::doubleSeq& skyBandWidth,const ACS::doubleSeq& marks,
			const ACS::doubleSeq& sourceFlux,const ACS::doubleSeq& atts,const IRA::CString& name="SECTION TABLE",const IRA::CString& rfName="RF INPUTS");

	/**
	 * It allows to add the servo table to the file
	 * @param axisName list of the names of the involved axis
	 * @param axisUnit list of the corresponding measurements units adopted for each axis, its dimension
	 *        must reflect the size of the previous argument
	 * @param name name of the table
	*/
	bool addServoTable(const ACS::stringSeq &axisName,const ACS::stringSeq& axisUnit,const IRA::CString name="SERVO TABLE");
	
	/**
	 * It allows to add data table to the fits file
	 * @param dataName name of the data table
	 * @param tsysName name of the tsys table
	 * @return false if the operation fails
	 */
	bool addDataTable(const IRA::CString& dataName="DATA TABLE",const IRA::CString tsysName="ANTENNA TEMP TABLE");
	
	/**
	 * It allows to add the feed table to the fits file
	 * @param name name of the table
	 * @return false if the operation could not be completed
	 */
	bool addFeedTable(const IRA::CString& namne="FEED TABLE");
	
	/**
	 * It allows to add a pair (key,value) to the data header
	 * @param name keyword to be added
	 * @param val value to be assigne to the keyword
	 * @param description it will permit to give an extra description to the key
	 * @return false if the function fails
	 */
	template <typename T>
		bool setDataHeaderKey(const IRA::CString& name, T val, const std::string& description); 
	
	/**
	 * It allows to add a pair (key,value) to the primary header
	 * @param name keyword to be added
	 * @param val value to be assigne to the keyword
	 * @param description it will permit to give an extra description to the key
	 * @return false if the function fails
	 */
	template <typename T>
		bool setPrimaryHeaderKey(const IRA::CString& name, T val, const IRA::CString& description="Description");

	/**
	 * It allows to add a pair (key,value) to the channel header
	 * @param name keyword to be added
	 * @param val value to be assigne to the keyword
	 * @param description it will permit to give an extra description to the key
	 * @return false if the function fails
	 */	
	template <typename T>
		bool setSectionHeaderKey(const IRA::CString& name, T val, const IRA::CString& description="Description");
	
	/**
	 * It allows to add a pair (key,value) to the feed table header
	 * @param name keyword to be added
	 * @param val value to be assigne to the keyword
	 * @param description it will permit to give an extra description to the key
	 * @return false if the function fails
	 */
	template <typename T>
		bool setFeedHeaderKey(const IRA::CString& name, T val, const IRA::CString& description="Description");
	
	/**
	 * It allows to add a user comment into the fits primary header
	 * @param comment new comment to be added
	 * @return false if the operation fails
	 */
	bool setPrimaryHeaderComment(const IRA::CString& comment);

	/**
	 * This method stores the position of the servo system in the file into the dedicated table
	 * @param time reference time in mjd
	 * @param pos sequence of positions, one for each involved axis
	 * @return false if the operation failed
	 */
	bool storeServoData(const double& time, const ACS::doubleSeq& pos);
	
	/**
	 * This method stores the auxiliary data into the fits file.
	 * @param data reference to the structure that contains the data (corresponding to one row of the table) to be saved
	 * @pointer to the vector that contains the tsys values for the channels. It must be a value for each channel.
	 * @return false if the operation failed
	 */ 
	bool storeAuxData(TDataHeader& data,double * tsys);
	
	/**
	 * This method stores the raw data into the fits file. This data should be the raw data directly coming from the backend.
	 * @param p vector containing the raw data
	 * @param size number of elements of the provided vector
	 * @param index identifies the column  (starting from the first column of the data table that contains raw data) the provided vector refers to
	 */
	template <typename T>
		bool storeData(const T* p, int size, int index);

	/**
	 * It moves the write pointer to the next row of the data table. The next call to <i>storeData()</i> will affect the next row. 
	 */
	void add_row(){ next_row++; }
	
	/**
	 * It can be used to get a description of the last occured error
	 * @return the string containing the description
	 */
	 const char *getLastError() { return (const char *)m_lastError; }
	
	private:
		IRA::CString m_lastError;
		IRA::CString m_filename; //default is  'out.fits'
		IRA::CString m_basepath; //must end with '/' defaults to '~/'
		
		std::string data_type;
		CCfits::FITS *pFits;
		CCfits::Table *data_table, *section_table, *rfInput_table, *feed_table, *tsys_table;
		CCfits::Table *servo_table;
		
		Backends::TMainHeader m_mainHeader; // main header data structure
		bool m_mainHeaderSet; //flags used to check if the main header has been saved
		Backends::TSectionHeader *m_channels;
		
		std::vector<string> SectColName;
		std::vector<string> SectColForm;
		std::vector<string> SectColUnit;
		std::vector<string> rfColName;
		std::vector<string> rfColForm;
		std::vector<string> rfColUnit;
		std::vector<string> DataColName;
		std::vector<string> DataColForm;
		std::vector<string> DataColUnit;
		std::vector<string> FeedColName;
		std::vector<string> FeedColForm;
		std::vector<string> FeedColUnit;
		std::vector<string> tsysColName;
		std::vector<string> tsysColUnit;
		std::vector<string> tsysColForm;
		std::vector<string> servoColName;
		std::vector<string> servoColUnit;
		std::vector<string> servoColForm;
		int next_row, data_column, feed_number;
};

template <typename T>
bool CFitsWriter::setDataHeaderKey(const IRA::CString& name, T val, const std::string& description)
{
	if(!data_table) {
		m_lastError = "no data table found";
		return false;
	}
	try {
		data_table->addKey((const char *)name,val,description);
	}
	catch(CCfits::FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	return true;
};

template <typename T>
bool CFitsWriter::setFeedHeaderKey(const IRA::CString& name, T val, const IRA::CString& description)
{
	if(!feed_table)
	{
		m_lastError = "no feed table found";
		return false;
	}
	try {
		feed_table->addKey(name, val, description);
	}
	catch(CCfits::FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	return true;
};

template <typename T>
bool CFitsWriter::setSectionHeaderKey(const IRA::CString& name, T val, const IRA::CString& description)
{
	if(!section_table)
	{
		m_lastError = "no section table found";
		return false;
	}
	try {
		section_table->addKey((const char *)name, val,(const char *)description);
	}
	catch(CCfits::FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	return true;
};

template <typename T>
bool CFitsWriter::setPrimaryHeaderKey(const IRA::CString& name, T val, const IRA::CString& description)
{
	if (!pFits) {
		m_lastError = "fits file is not created";
		return false;
	}
	try {
		pFits->pHDU().addKey((const char*)name, val,(const char *) description);
	}
	catch(CCfits::FitsException& fe)
	{
		m_lastError = fe.message().c_str();
		return false;
	}
	return true;
};

template <typename T>
bool CFitsWriter::storeData(const T* p, int size, int index){
	if (!data_table) {
		m_lastError="data table not available";
		return false;
	}
	if(size > 1){
		std::valarray<T> data(p, size);
		std::vector<std::valarray<T> > vdata;
		vdata.push_back(data);
		try {
			data_table->column(index + data_column).writeArrays(vdata, next_row);
		}
		catch(CCfits::FitsException& fe)
		{
			m_lastError = fe.message().c_str();
			return false;
		}
	}else{
		T data = *p;
		std::vector<T> vdata;
		vdata.push_back(data);
		try {
			data_table->column(index + data_column).write(vdata, next_row);
		}
		catch(CCfits::FitsException& fe)
		{
			m_lastError = fe.message().c_str();
			return false;
		}
	}
	return true;	
};
#endif
