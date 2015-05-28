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

namespace FitsWriter_private {

class CFitsTools {
public:
	/**
	 * It allows to add a user comment into the fits primary header
	 * @param fits pointer to the fits file	
	 * @param comment new comment to be added
	 * @param errorMessage error message if a problem was found
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
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	static bool primaryHeaderHistory(
			CCfits::FITS *const fits,
			const IRA::CString& history,
			IRA::CString& errorMessage);

	/**
	 * It flushes all pending writing in the fits file.
	 * @param fits pointer to the fits file
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	static bool flush(CCfits::FITS *const fits,IRA::CString& errorMessage);

	/**
	 * It allows to write a primary header key
	 * @param fits pointer to the fits file
	 * @param name keyword name
	 * @param val value of the keyword
	 * @param description allows to give the keyword description
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	template <class T>
	static bool setPrimaryHeaderKey(
			CCfits::FITS *const fits,
			const IRA::CString& name,
			const T& val,
			const IRA::CString& description,
			IRA::CString& errorMessage);

	/**
	 * It allows to write a primary header key (multiple)
	 * @param fits pointer to the fits file
	 * @param name keyword name
	 * @param val value of the keyword
	 * @param description allows to give the keyword description
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	template <class T>
	static bool setPrimaryHeaderKey(
			CCfits::FITS *const fits,
			const IRA::CString& name,
			const std::list<T>& val,
			const IRA::CString& description,
			IRA::CString& errorMessage);

};

template <class T>
bool CFitsTools::setPrimaryHeaderKey(CCfits::FITS *const fits,const IRA::CString& name,const T& val,
		const IRA::CString& description,IRA::CString& errorMessage)
{
	if (!fits) {
		errorMessage="fits file is not created";
		return false;
	}
	try {
		fits->pHDU().addKey((const char*)name,val,(const char *)description);
	}
	catch(CCfits::FitsException& ex) {
		errorMessage=ex.message().c_str();
		return false;
	}
	return true;
};

template <class T>
bool CFitsTools::setPrimaryHeaderKey(CCfits::FITS *const fits,const IRA::CString& name,const typename std::list<T>& val,
		const IRA::CString& description,IRA::CString& errorMessage)
{
	if (!fits) {
		errorMessage="fits file is not created";
		return false;
	}
	typename std::list<T>::const_iterator it;
	IRA::CString out;
	unsigned counter=0;
	try {
		for (it=val.begin();it!=val.end();it++) {
			out.Format("%s%d",(const char *)name,counter+1);
			fits->pHDU().addKey((const char*)out,*it,(const char *)description);
			counter++;
		}
	}
	catch(CCfits::FitsException& ex) {
		errorMessage=ex.message().c_str();
		return false;
	}
	return true;
}



};

#endif /* FITSTOOLS_H_ */
