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
#include <valarray>

namespace FitsWriter_private {

class CFitsTools {
public:

	/**
	 * It allows to create a new file fits
	 * @param fits pointer to the fits file, it will be modified by the call
	 * @param fileName name of the file to be created
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	static bool createFile(
			CCfits::FITS *& fits,
			const IRA::CString& fileName,
			IRA::CString& errorMessage);
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
	 * It closes the fits file a frees the resources.
	 * @param fits pointer to the fits file
	 */
	static void closeFile(CCfits::FITS *& fits);

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
	 * @param val list of keywords values
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

	/**
	 * It allows to write a primary header key (multiple)
	 * @param fits pointer to the fits file
	 * @param name keyword name
	 * @param val vector of keywords values
	 * @param description allows to give the keyword description
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	template <class T>
	static bool setPrimaryHeaderKey(CCfits::FITS *const fits,
			const IRA::CString& name,
			const std::vector<T>& val,
			const IRA::CString& description,
			IRA::CString& errorMessage);


	/**
	 * It allows to write a table header key (multiple)
	 * @param table pointer to the fits table
	 * @param name keyword name
	 * @param val list of values of the keyword
	 * @param description allows to give the keyword description
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	template <class T>
	static bool setTableHeaderKey(
			CCfits::Table *const table,
			const IRA::CString& name,
			const std::list<T>& val,
			const IRA::CString& description,
			IRA::CString& errorMessage);
	/**
	 * It allows to write a table header key (multiple)
	 * @param table pointer to the fits table
	 * @param name keyword name
	 * @param val vector of values of the keyword
	 * @param description allows to give the keyword description
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	template <class T>
	static bool setTableHeaderKey(CCfits::Table *const table,
			const IRA::CString& name,
			const std::vector<T>& val,
			const IRA::CString& description,
			IRA::CString& errorMessage);

	/**
	 * It allows to write a table header key
	 * @param table pointer to the fits table
	 * @param name keyword name
	 * @param val value of the keyword
	 * @param description allows to give the keyword description
	 * @param errorMessage error message if a problem was found
	 * @return false if the operation fails
	 */
	template <class T>
	static bool setTableHeaderKey(
			CCfits::Table *const table,
			const IRA::CString& name,
			const T& val,
			const IRA::CString& description,
			IRA::CString& errorMessage);

	/**
	 * It allows to create a binary table inside the fits file
	 * @param fits pointer to the fits file
	 * @param name name of the table
	 * @param colName list of names of each column
	 * @param colForm list of columns data type
	 * @param colUnit list of column description or units
	 * @param errorMessage error message if a problem was found
	 * @return the pointer to the newly create table, NULL if an error occurred
	 */
	static CCfits::Table * createTable(CCfits::FITS *const fits,const IRA::CString& name,const std::vector<string>& Colname,
			const std::vector<string>& Colform,const std::vector<string>& Colunit,IRA::CString& errorMessage);


	/**
	 * It allows to write a scalar column in a binary table
	 * @param table pointer to the table
	 * @param column name of the column to be written
	 * @param val value to be written in the column
	 * @param row row number (first row is 1)
	 * @param errorMessage error message if a problem was found
	 * @return false in case of errors
	 */
	template <class T>
	static bool writeTable(CCfits::Table *const table,const IRA::CString& column,T* const val,const int& row,IRA::CString& errorMessage);

	/**
	 * It allows to write a scalar column in a binary table
	 * @param table pointer to the table
	 * @param column id of the column (first one is 1)
	 * @param val value to be written in the column
	 * @param row row number (first row is 1)
	 * @param errorMessage error message if a problem was found
	 * @return false in case of errors
	 */
	template <class T>
	static bool writeTable(CCfits::Table *const table,const int& column,T* const val,const int& row,IRA::CString& errorMessage);

	/**
	 * It allows to write a vector column in a binary table
	 * @param table pointer to the table
	 * @param column name of the column
	 * @param val vector of values to be written in the column
	 * @param row row number (first row is 1)
	 * @param errorMessage error message if a problem was found
	 * @return false in case of errors
	 */
	template <class T>
	static bool writeTable(CCfits::Table *const table,const IRA::CString& column,const std::vector<T>& val,const int& row,IRA::CString& errorMessage);

	/**
	 * It allows to write a vector column in a binary table
	 * @param table pointer to the table
	 * @param column id of the column (first one is 1)
	 * @param val vector of values to be written in the column
	 * @param row row number (first row is 1)
	 * @param errorMessage error message if a problem was found
	 * @return false in case of errors
	 */
	template <class T>
	static bool writeTable(CCfits::Table *const table,const int& column,const std::vector<T>& val,const int& row,IRA::CString& errorMessage);

	/**
	 * It allows to write a vector column in a binary table
	 * @param table pointer to the table
	 * @param column id of the column (first one is 1)
	 * @param val vector of values to be written in the column
	 * @param row row number (first row is 1)
	 * @param errorMessage error message if a problem was found
	 * @return false in case of errors
	 */
	template <class T>
	static bool writeTable(CCfits::Table *const table,const int& column,const std::vector< std::valarray< T > > & val,const int& row,IRA::CString& errorMessage);

	/**
	 * It allows to write a vector column in a binary table
	 * @param table pointer to the table
	 * @param column name of the column
	 * @param val vector of values to be written in the column
	 * @param row row number (first row is 1)
	 * @param errorMessage error message if a problem was found
	 * @return false in case of errors
	 */
	template <class T>
	static bool writeTable(CCfits::Table *const table,const IRA::CString& column,const std::vector< std::valarray< T > > & val,const int& row,IRA::CString& errorMessage);


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
bool CFitsTools::setPrimaryHeaderKey(CCfits::FITS *const fits,const IRA::CString& name,const std::list<T>& val,
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

template <class T>
bool CFitsTools::setPrimaryHeaderKey(CCfits::FITS *const fits,const IRA::CString& name,const std::vector<T>& val,
		const IRA::CString& description,IRA::CString& errorMessage)
{
	if (!fits) {
		errorMessage="fits file is not created";
		return false;
	}
	typename std::vector<T>::const_iterator it;
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

template <class T>
bool CFitsTools::setTableHeaderKey(CCfits::Table *const table,const IRA::CString& name,const std::list<T>& val,const IRA::CString& description,
		IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	typename std::list<T>::const_iterator it;
	IRA::CString out;
	unsigned counter=0;
	try {
		for (it=val.begin();it!=val.end();it++) {
			out.Format("%s%d",(const char *)name,counter+1);
			table->addKey((const char*)out,*it,(const char *)description);
			counter++;
		}
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}

template <class T>
bool CFitsTools::setTableHeaderKey(CCfits::Table *const table,const IRA::CString& name,const std::vector<T>& val,const IRA::CString& description,
		IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	typename std::vector<T>::const_iterator it;
	IRA::CString out;
	unsigned counter=0;
	try {
		for (it=val.begin();it!=val.end();it++) {
			out.Format("%s%d",(const char *)name,counter+1);
			table->addKey((const char*)out,*it,(const char *)description);
			counter++;
		}
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}

template <class T>
bool CFitsTools::setTableHeaderKey(CCfits::Table *const table,const IRA::CString& name,const T& val,const IRA::CString& description,
		IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	try {
		table->addKey((const char *)name,val,(const char *)description);
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}




/*template <class T>
bool CFitsTools::writeTable(CCfits::Table *const table,const IRA::CString& column,T* const val,const int& row,
		IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	try {
		table->column((const char *)column).write(val,1,row);
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}

template <class T>
bool CFitsTools::writeTable(CCfits::Table *const table,const int& column,T* const val,const int& row,IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	try {
		table->column(column).write(val,1,row);
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}*/

template <class T>
bool CFitsTools::writeTable(CCfits::Table *const table,const int& column,const std::vector<T>& val,const int& row,IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	try {
		table->column(column).write(val/*,val.size()*/,row);
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}

template <class T>
bool CFitsTools::writeTable(CCfits::Table *const table,const IRA::CString& column,const std::vector<T>& val,const int& row,IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	try {
		table->column((const char *)column).write(val/*,val.size()*/,row);
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}

template <class T>
bool CFitsTools::writeTable(CCfits::Table *const table,const int& column,const std::vector< std::valarray< T > > & val,const int& row,IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	try {
		table->column(column).writeArrays(val,row);
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}

template <class T>
bool CFitsTools::writeTable(CCfits::Table *const table,const IRA::CString& column,const std::vector< std::valarray< T > > & val,const int& row,IRA::CString& errorMessage)
{
	if(!table) {
		errorMessage="table not found";
		return false;
	}
	try {
		table->column((const char *)column).writeArrays(val,row);
	}
	catch(CCfits::FitsException& fe) {
		errorMessage=fe.message().c_str();
		return false;
	}
	return true;
}

};

#endif /* FITSTOOLS_H_ */
