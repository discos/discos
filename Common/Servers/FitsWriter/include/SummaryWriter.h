

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
	*/
	bool create();
private:
	IRA::CString m_lastError;
	IRA::CString m_filename;
	IRA::CString m_basepath;
	CCfits::FITS *m_pFits;
};

#endif
