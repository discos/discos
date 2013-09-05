#ifndef _FITSWRITER_
#define _FITSWRITER_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: FitsWriter.h,v 1.6 2010/06/25 11:26:38 bliliana Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  when           What                                                       */
/* Marco Bartolini		  							 24/01/2009      Creation                                                  */
/* Andrea Orlati (a.orlati@ira.inaf.it) 			 12/03/2009      Reviewed some parts                           */

//Standard Imports
#include <sstream>
#include <memory>
#include <valarray>

//External Library Imports
#include <CCfits>
#include <string.h>

//Project Imports
#include <BackendsDefinitionsC.h>

#define FITS_VERSION "V. 0.8"

#define DEFAULT_COMMENT FITS_VERSION" Created by  S. Righini, M. Bartolini  & A. Orlati"

#define HISTORY1 FITS_VERSION" First output standard for Italian radiotelescopes"

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
	    double tsys;
	    int flag_cal;
	    int flag_track;
	    double weather[3];
	};
	// This number must reflect the exact number of fields in the TDataHeader structure
#define _FW_DATAHEADER_ELEMENTS 12 
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
	void setFileName(const char *name);
	
	/**
	 * Allows to set the base path (excluded the file name) of the fits file
	 * @param path string that containes the 
	 */	
	void setBasePath(const char *path);
	
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
	bool create(const char *name);

	/**
	 * It creates the file in the disk, using the given name and path. The fully qualified file name is path+name.
	 * @param name it allows to provide a new file name
	 * @param path it allows to provide the base path
	 * @return false if the creation fails.
	 */
	bool create(const char *name,const char *path);
	
	/**
	 * It allows to store information coming from the main header of the data flow
	 * @param tmh reference to the main header structure
	 * @return false if the operation fails
	 */ 
	bool saveMainHeader(const Backends::TMainHeader& tmh);
	
	/**
	 * It allows to store the data regarding the channels. It expects exactly the number of channels that was declared in the main header structure
	 * @param tch reference to the channel header structure
	 * @return false if the operation fails
	 */
	bool saveChannelHeader(const Backends::TSectionHeader* tch);
	
	/**
	 * It allows to save the feed header and store them into the fits file
	 * @param fh reference to one  feed header
	 * @return false if the operation fails
	 */
	bool saveFeedHeader(TFeedHeader& fh);
	
	/**
	 * It allows to add the channel table to the fits file
	 * @param name name of the table
	 * @return false if the operation is not succesful
	 */
	bool addChannelTable(const char *name="CHANNEL TABLE");
	
	/**
	 * It allows to add data table to the fits file
	 * @param name name of the table
	 * @return false if the operation fails
	 */
	bool addDataTable(const char *name="DATA TABLE");
	
	/**
	 * It allows to add the feed table to the fits file
	 * @param name name of the table
	 * @return false if the operation could not be completed
	 */
	bool addFeedTable(const char *name="FEED TABLE");
	
	/**
	 * It allows to add a pair (key,value) to the data header
	 * @param name keyword to be added
	 * @param val value to be assigne to the keyword
	 * @param description it will permit to give an extra description to the key
	 * @return false if the function fails
	 */
	template <typename T>
		bool setDataHeaderKey(const std::string& name, T val, const std::string& description); 
	
	/**
	 * It allows to add a pair (key,value) to the primary header
	 * @param name keyword to be added
	 * @param val value to be assigne to the keyword
	 * @param description it will permit to give an extra description to the key
	 * @return false if the function fails
	 */
	template <typename T>
		bool setPrimaryHeaderKey(const std::string& name, T val, const std::string& description="Description");

	/**
	 * It allows to add a pair (key,value) to the channel header
	 * @param name keyword to be added
	 * @param val value to be assigne to the keyword
	 * @param description it will permit to give an extra description to the key
	 * @return false if the function fails
	 */	
	template <typename T>
		bool setChannelHeaderKey(const std::string& name, T val, const std::string& description="Description");
	
	/**
	 * It allows to add a pair (key,value) to the feed table header
	 * @param name keyword to be added
	 * @param val value to be assigne to the keyword
	 * @param description it will permit to give an extra description to the key
	 * @return false if the function fails
	 */
	template <typename T>
		bool setFeedHeaderKey(const std::string& name, T val, const std::string& description="Description");
	
	/**
	 * It allows to add a user comment into the fits primary header
	 * @param comment new comment to be added
	 * @return false if the operation fails
	 */
	bool setPrimaryHeaderComment(const std::string& comment);
	
	/**
	 * This method stores the auxiliary data into  the fits file.
	 * @param data reference to the structure that contains the data (corresponding to one row of the table) to be saved
	 * @return false if the operation failed
	 */ 
	bool storeAuxData(TDataHeader& data);
	
	/**
	 * This method stores the raw data into the fits file. This data should be the raw data directly coming from the backend.
	 * @param p vector containing the raw data
	 * @param size number of elments of the provided vector
	 * @param index identifies the column  (starting from the first column of the data table that contains raw data) the provided vector refers to
	 */
	template <typename T> bool storeData(const T* p, int size, int index);

	/**
	 * It moves the write pointer to the next row of the data table. The next call to <i>storeData()</i> will affect the next row. 
	 */
	void add_row(){ next_row++; }
	
	/**
	 * It can be used to get a description of the last occured error
	 * @return the string containing the description
	 */
	 const char *getLastError() { return m_lastError.c_str(); }
	
	private:
		std::string m_lastError;
		std::string m_filename; //default is  'out.fits'
		std::string m_basepath; //must end with '/' defaults to '~/'
		
		std::string data_type;
		CCfits::FITS *pFits;
		CCfits::Table *data_table, *channel_table, *feed_table;
		
		Backends::TMainHeader m_mainHeader; // main header data structure
		bool m_mainHeaderSet; //flags used to check if the main header has been saved
		Backends::TSectionHeader *m_channels;
		
		std::vector<string> ChColName;
		std::vector<string> ChColForm;
		std::vector<string> ChColUnit;
		std::vector<string> DataColName;
		std::vector<string> DataColForm;
		std::vector<string> DataColUnit;
		std::vector<string> FeedColName;
		std::vector<string> FeedColForm;
		std::vector<string> FeedColUnit;
		int next_row, data_column, feed_number;
};

template <typename T>
bool CFitsWriter::setDataHeaderKey(const std::string& name, T val, const std::string& description)
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
bool CFitsWriter::setFeedHeaderKey(const std::string& name, T val, const std::string& description)
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
bool CFitsWriter::setChannelHeaderKey(const std::string& name, T val, const std::string& description)
{
	if(!channel_table)
	{
		m_lastError = "no channel table found";
		return false;
	}
	try {
		channel_table->addKey(name, val,description);
	}
	catch(CCfits::FitsException& fe) {
		m_lastError = fe.message().c_str();
		return false;
	}
	return true;
};

template <typename T>
bool CFitsWriter::setPrimaryHeaderKey(const std::string& name, T val, const std::string& description)
{
	if (!pFits) {
		m_lastError = "fits file is not created";
		return false;
	}
	try {
		pFits->pHDU().addKey(name, val, description);
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
