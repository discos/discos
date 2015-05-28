

#ifndef SUMMARYWRITER_H_
#define SUMMARYWRITER_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                       */
/* Andrea Orlati (a.orlati@ira.inaf.it)   09/04/2015      created                         */

#include <CCfits>
#include <IRA>
#include "File.h"

/**
 * This class is in charge of writing a fits file containing the summary of the scan when
 * the scan come to end.
 */
class CSummaryWriter {
public:
	/**
	 * Constructor
	 */
	CSummaryWriter();
	/**
	 * Destructor
	 */
	~CSummaryWriter();

	/**
	 * Allows to set the name of the fits file.
	 * @param name string that contains the file name
	 */
	void setFileName(const char* name);

	/**
	 * Allows to set the base path (excluded the file name) of the fits file
	 * @param path string that contains the
	 */
	void setBasePath(const char* path);

	/**
	 * This method creates the summary FITS file in the disk. It also add preliminary informations
	 * such as comments, date, history and so on.
	 * @return false if the operation could not be executed, true otherwise
	*/
	bool create();

	/**
	 * @return the reference to the internal file. The reference could be used to save the data into it.
	 */
	FitsWriter_private::CFile& getFile() { return (*m_file); }

	/**
	 * @return the pointer to the internal file. The reference could be used to save the data into it.
	 */
	FitsWriter_private::CFile* getFilePointer() { return m_file; }


	/**
	 * This method is in charge of writing the data (keyword, tables and whatever) into the FITS file. The data are taken from the
	 * internal file
	 * @return false if the operation could not be executed, true otherwise
	 */
	bool write();

	/**
	 * This method closes the present summary file.
	 */
	bool close();

	/**
	 * It allows to collect the error message of the current active error
	 */
	const IRA::CString& getLastError() const { return m_lastError; }
private:
	IRA::CString m_lastError;
	IRA::CString m_filename;
	IRA::CString m_basepath;
	CCfits::FITS *m_pFits;
	FitsWriter_private::CFile *m_file;
	void init();
};

#endif
