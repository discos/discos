#ifndef FITSTOOLS_H_
#define FITSTOOLS_H_

/* ************************************************************************************************************* */
/* DISCOS project                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                       */
/* Andrea Orlati (a.orlati@ira.inaf.it)   09/04/2015
*/

#include <CCfits>
#include <IRA>

class CFitsTools {
public:
	/**
	 * It allows to add a user comment into the fits primary header
	 * @param fits pointer to the fits file	
	 * @param comment new comment to be added
	 * @param errorMessage errore message if a problem was found
	 * @return false if the operation fails
	 */
	static bool primaryHeaderComment(
			CCfits::FITS *const fits,
			const IRA::CString& comment,
			IRA::CString& errorMessage);
	/**
	 * It allows to add an history entry into the fits primary header
	 * @param fits pointer to the fits file	
	 * @param comment new history entry
	 * @param errorMessage errore message if a problem was found
	 * @return false if the operation fails
	 */
	static bool primaryHeaderHistory(
			CCfits::FITS *const fits,
			const IRA::CString& history,
			IRA::CString& errorMessage);

};


#endif /* FITSTOOLS_H_ */
