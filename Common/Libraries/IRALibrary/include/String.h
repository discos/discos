#ifndef _CSTRING_H__
#define _CSTRING_H__

/* *************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                     */
/*                                                                                                     */
/* This code is under GNU General Public Licence (GPL).                                                */
/*                                                                                                     */
/* Who                                when            What                                             */
/* Andrea Orlati(aorlati@ira.cnr.it)  09/07/2004	  Creation                                          */
/* Andrea Orlati                      14/07/2004      Added Format Method                              */
/* Andrea Orlati                      29/08/2006      Fixed a bug in Format method                     */
/* Andrea Orlati                      27/11/2006      Added a new constructor that behaves like the format function  */
/* Andrea Orlati                      21/10/2008     Added modifiers 'const' to ToLong, ToDouble,  */
/* Andrea Orlati                      20/09/2008     fixed a bug in RTrim and LTrim function  */

// $Id: String.h,v 1.4 2010-09-20 11:48:20 a.orlati Exp $

#include <String_Base.h>
#include <SStringfwd.h>

namespace IRA {

/** 
 * This class allow to manage strings easier than standard C/CPP methods
 * If there are memory allocations errors, application is aborted (assert).
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class CString
{
public:
	/**
	 * Constructor
	 * Create an empty string
	*/
	CString();
	/**
	 * Constructor
	 * Create a CString from a classic C array of chars
	 * @param src pointer to the characters array
	*/
	CString(const char *src);
	/**
	 * Constructor
	 * Copy constructor 
	 * @param src CString object to copy
	*/	
	CString(const CString & src);
	/**
	 * Constructor
	 * Create a string containing a sigle character 
	 * @param ch included character 
	*/		
	CString(char ch);
	/**
	 * Constructor
	 * Create a string of blanks 
	 * @param num number of blanks
	*/	
	explicit CString(int num);
	/**
	 * Constructor.
	 * Create a string from an ACE string
	 * @param str ACE string object
	*/
	CString(ACE_CString str);
	
	/**
	 * Constructor.
	 * It creates a new string according to a specific format function as printf does. If the string can't be
	 * produced (format string and parameters do not match, for example) the resulting string will be empty.
	 * @param max indicates the limit of the number of characters (included the string terminator) that will be added to the string,
    *        if a negative or a zero is passed it will be used the default limit.	 
	 * @param format string containing normal character and/or conversion specifiers	 
	*/
	CString(const int& max,const char *format,...);
	/**
	 * Destructor.
	*/
	~CString();
public: // Operations
	/**
	 * Clear the string
	*/ 
	void Empty();
	/**
	 * Checks if the string is empty (zero length)
	 * @return true if the string is empty
	*/
	bool IsEmpty() const;
	/**
	 * @return the length of the string
	*/
	int GetLength() const;
	/**
	 * Perform a string concatenation
	 * @param add string to concatenate 
	 * @return the object
	*/ 
	CString & Concat(const CString & add);
	/**
	 * Perform a string concatenation
	 * @param add C string to concatenate
	 * @return the object
	*/ 
	CString & Concat(const char *add);
	/**
	 * Perform a string concatenation
	 * @param add character to be added to the string
	 * @return the object
	*/ 
	CString & Concat(char add);
	/**
	 * Concatenate the alfanumeric rapresentation of an integer value
	 * @param nValue integer value
	 * @return the object
	*/ 
	CString & Concat(int nValue);
	/**
	 * Concatenate the alfanumeric rapresentation of a floating point (max number of digits)
	 * @param fValue floating point number
	 * @return the object
	*/ 
	CString & Concat(float fValue);
	/**
	 * Concatenate the alfanumeric rapresentation of a floating point
	 * @param fValue floating point number
	 * @param digits number of decimal digits to include
	 * @return the object
	 * @return the object
	*/ 
	CString & Concat(float fValue,int digits);
	/**
	 * Concatenate the alfanumeric rapresentation of a double (max number of digits)
	 * @param dValue double precision floating point number
	 * @return the object
	*/ 
	CString & Concat(double dValue);
	/**
	 * Concatenate the alfanumeric rapresentation of a double
	 * @param dValue double precision floating point number
	 * @param digits number of decimal digits to include
	 * @return the object
	*/ 
	CString & Concat(double dValue,int digits);
	/**
	 * Concatenate the alfanumeric rapresentation of a long double (max number of digits)
	 * @param dValue double precision floating point number
	 * @return the object
	*/ 
	CString & Concat(long double dValue);
	/**
	 * Concatenate the alfanumeric rapresentation of a long double
	 * @param dValue double precision floating point number
	 * @param digits number of decimal digits to include
	 * @return the object
	*/ 
	CString & Concat(long double dValue,int digits);
	/**
	 * Format the string according to the specified Format string as printf does. If the string can't be
	 * produced (format string and parameters doesn't match, for example) the string will be left unchanged.
	 * @param Frm string containing normal character and/or conversion specifiers
	*/
	void Format(const char *Frm,...);
	/**
	 * @param index position of a character inside the string		
	 * @return the indexed character
	*/
	char CharAt(int index) const;
	/**
	 * Set a character inside the string
	 * @param index a position inside the string
	 * @param ch character inserted into the specified position
	*/
	void SetAt(int index, char ch);
	/** 
	 * Compares the string with another one matching the case
	 * @param str string used for the comparison 
	 * @return an integer less than, equal to, or greater than zero if the string (or the first n bytes thereof) is found,
    * respectively, to be less than, to match, or be greater than the other one.
	*/
	int Compare(const char *str) const;
	/** Compares the string with another one without matching the case
	 * @param str string used for the comparison 
	 * @return an integer less than, equal to, or greater than zero if the string (or the first n bytes thereof) is found,
    * respectively, to be less than, to match, or be greater than the other one.
	*/
	int CompareNoCase(const char *str) const;
	/**
	 * Gets the first characters of the string from the left.
	 * @param count number of characters from the left
	 * @return the string composed by the selected characters
	*/
	CString Left(int count) const;
	/**
	 * Gets the first characters of the string from the right.
	 * @param count number of characters from the right
	 * @return the string composed by the selected characters
	*/
	CString Right(int count) const;
	/**
	 * Gets the characters within a range.
	 * @param start index of the first character
	 * @param count number of characters to add
	 * @return the string composed by the selected characters
	*/
	CString Mid(int start, int count) const;
	/**
	 * Makes all letter of the string capital letter 
	*/
	void MakeUpper();
	/**
	 * Makes lower all letter of the string
	*/	
	void MakeLower();
	/**
	 * Looks for a substring inside the string.
	 * @param ch character to match
	 * @return the position of the first letter of the matching pattern
	*/ 
	int Find(char ch) const;
	/**
	 * Looks for a substring inside the string.
	 * @param str string to match
	 * @return the position of the first letter of the matching pattern
	*/ 	
	int Find(const char *str) const;
	/**
	 * Looks for a substring inside the string starting from a position.
	 * @param ch character to match
	 * @param start start from this position
	 * @return the position of the first letter of the matching pattern
	*/ 	
	int Find(char ch,int start) const;
	/**
	 * Looks for a substring inside the string starting from a position.
	 * @param str string to match
	 * @param start start from this position
	 * @return the position of the first letter of the matching pattern
	*/ 		
	int Find(const char *str, int start) const;
	/**
	 * Replace all occurences of a character with another one, starting from a position.
	 * @param ch1 charatecter to replace
	 * @param ch2 replacing character
	 * @param start index of the first character
	 * @return the number of replaced characters
	*/
	int Replace(char ch1,char ch2,int start=0);
	/**
	 * Removes all the leading spaces of the string.
	 */
	void LTrim();
	/**
	 * Removes all the trailing spaces of the string.
	 */
	void RTrim();
	/**
	 * Tries to convert the string to an integer.
	 * @return the converted integer
	*/
	int ToInt() const;
	/**
	 * Tries to convert the string to a long integer. For 32 bits compilers and Linux OS behaves as <i>ToInt()</i>.
	 * @return the converted integer
	*/	
	long ToLong() const;
	/**
	 * Tries to convert the string to an long long integer.
	 * @return the converted integer
	*/	
	long long ToLongLong() const;
	/**
	 * Tries to convert the string to a floating point number.
	 * @return the converted floating point
	*/	
	float ToFloat() const;
	/**
	 * Tries to convert the string to a double.
	 * @return the converted double
	*/	
	double ToDouble() const;
	/**
	 * Tries to convert the string to a long double.
	 * @return the converted long double
	*/		
	long double ToLongDouble() const;
	// operators
	char operator [] (int index) const;
	// cast
	operator const char *() const;
	const CString & operator =(const CString& src);
	const CString & operator =(char ch);
	const CString & operator =(const char* str);
	// have to be friend so we can use char and char* as first arguments
	/**
	 * Friend operator, used to concatenate.
	*/ 	
	friend CString operator +(const CString& s1,const CString & s2);
	/**
	 * Friend operator, used to concatenate.
	*/ 		
	friend CString operator +(const CString & s1, char ch);
	/**
	 * Friend operator, used to concatenate.
	*/ 		
	friend CString operator +(const CString & s1, const char *str);
	/**
	 * Friend operator, used to concatenate.
	*/ 		
	friend CString operator +(char ch, const CString & s2);
	/**
	 * Friend operator, used to concatenate.
	*/ 		
	friend CString operator +(const char *str, const CString & s2);
	/**
	 * Operator used to concatenate the string with another one.
	*/ 	
	const CString & operator +=(const CString & src);
	/**
	 * Operator used to concatenate the string with another one.
	*/ 		
	const CString & operator +=(char ch);
	/**
	 * Operator used to concatenate the string with another one.
	*/ 		
	const CString & operator +=(const char *str);
	// compare
	/**
	 * Frind operator used to compare strings
	*/ 	
	friend bool operator ==(const CString & s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator ==(const CString & s1, const char *s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator ==(const char *s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator !=(const CString & s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator !=(const CString & s1, const char *s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator !=(const char *s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator < (const CString & s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator < (const CString & s1, const char *s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator < (const char *s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator > (const CString & s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator > (const CString & s1, const char *s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator > (const char *s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator <=(const CString & s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator <=(const CString & s1, const char *s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator <=(const char *s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator >=(const CString & s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator >=(const CString & s1, const char *s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
	friend bool operator >=(const char *s1, const CString & s2);
	/**
	 * Frind operator used to compare strings
	*/ 		
protected:
	int m_iSize;
	char *m_cpString;
};

}

#endif
