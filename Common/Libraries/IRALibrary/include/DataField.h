#ifndef _DATAFIELD_H
#define _DATAFIELD_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DataField.h,v 1.2 2007-10-24 15:09:48 a.orlati Exp $	    												  */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 07/06/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 26/01/2018      Rewritten in order to bypass an issue with 64bit ACS2017 */

#include <vector>
#include <baciValue.h>
#include "Definitions.h"
#include "String.h"

namespace IRA {

class CDBTable;	
	
/**
 * This class is a column of each database table (<i>CDBTable</i> object). 
 * Each field is identified by its title or column name (it matches to the name attribute of element tag of and XML document) and its data type. 
 * @sa <i>CDBTable</i> for more information about XML documents and schemas and the ACS configuration data base.
 * This class allows to access the value of the field of the current record inside the database table. This value can be treated as longlong, a double double word,
 * floating point double or a string. At present the fields are read-only, i.e. they cannot be modified using this class.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CDataField {	
public:
	friend class CDBTable;
	
	/**
	 * List of all supported field types
	*/		
	enum TFieldType {
		LONGLONG, 			    /*!< signed 64 bits integer */
		DOUBLEDOUBLEWORD,		 /*!< unsigned 64 bits integer */
		DOUBLE,			       /*!< 64 bits floating point number */
		STRING			       /*!< string */
	};
	
	/**
	 * Constructor.
	 * @param name the XML tag that identifies the column inside the table
	 * @param type field type
	*/
	CDataField(const CString& name,const TFieldType& type);
	
	/**
	 * Destructor.
	*/
	virtual ~CDataField();
	
	/**
	 * This method can be used to check if the field in the current dataset is undefined or not.
	 * @return true if null (not defined).
	*/
	bool isNull() const;
	
	/**
	 * This method returns the name of the field in the current recrodset. 
	 * @return a string that is the column name of the current table.
	*/
	IRA::CString getColumnName() const;
	
	/** 
	 * Call this method to retrieve the current field value as a string.
	 * @return the string containing the field value, or an empty string if the field is null
	*/
	IRA::CString asString() const;
	
	/** 
	 * Call this method to retrieve the current field value as a long long integer.
	 * @return the field value, or zero if the field is null or it is not a long long.
	*/
	long long asLongLong() const;
	
	/** 
	 * Call this method to retrieve the current field value as a 64 bits unsigned integer.
	 * @return the field value, or zero if the field is null or it is not an unsigned long long.
	*/
	DDWORD asDoubleDoubleWord() const;

	/** 
	 * Call this method to retrieve the current field value as a floating point number.
	 * @return the field value, or zero if the field is null or it is not a double.
	*/
	double asDouble() const;
	
protected:
	
	/**
	 * This method is used by the XML parser to allocate a new field value.
    */ 	
	void addValue();

	/**
	 * This method is used by the XML parser to set the current field value as a string.
	 * @param value new value as string
    */ 
	void setValue(const CString& value);

	/**
	 * Sets the current field pointer to the first position.
	*/
	void First();

	/**
	 * Sets the current field pointer to the last position.
	*/
	void Last();

	/**
	 * Sets the current field pointer to the next available position. If the pointer already points the last one, nothing happens.
	*/
	void Next();

	/**
	 * Sets the current field pointer to the previous available position. If the pointer already points the first one, nothing happens.
	*/
	void Prev();
	
	/**
	 * Sets the current filed pointer to an absolute value. If the commanded position is out of the bounds nothing is done.
	 * @param pos new absolute position
	*/
	void setPointer(const WORD& pos);
	
private:
	CString m_title;
	TFieldType m_type;
	int m_listPointer;
	//std::vector<baci::BACIValue *> m_valueList;	
	std::vector<IRA::CString *> m_valueList;

  	CDataField(const CDataField& rSrc);    // no implementation given
   void operator=(const CDataField& rSrc);  // no implementation given
};

}

#endif
