#ifndef _ERROR_H
#define _ERROR_H
/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.cnr.it)  24/01/2005      Creation                                          */
/* Andrea Orlati(aorlati@ira.cnr.it)  26/01/2005      Added the extra information feature               */
/*																																		  */

// $Id: Error.h,v 1.2 2010-04-16 08:28:38 a.orlati Exp $

namespace IRA {

/**
 * This class is meant as an error descriptor for the IRALibrary and is used by functions and class members as a way to return errors to the caller. 
 * Errors have been devided into ErrorTypes which allows the programmer to
 * localize the orgin of the error. Errors are also described by a code which identifies and explain with
 * more details the erros itself. The combination of type and code gives the CodeNumber which is an identifier 	
 * which uniquely identifies the error in the system. This class also stores some additional information such
 * as the file, the routine and the line of the piece of code that originated the error; the local time the error
 * occured and set of pair (Name,Value) that can be used to report the cause of the error.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CError 
{
public:
	/**
	 * List of all possible error types.
	*/  		
	enum ErrorTypes { 
		NoError=0x0000,     /*!< No error found */
		SerialType=0x0100,  /*!< Serial bus errors */
		SocketType=0x0200,  /*!< Socket errors */
		DBType=0x300		/*!< Data base errors */
	};
	/**
	 * List of all possible serial bus errors 
	*/  		
	enum SerialErrors {
		SrlPortConfig=0x00,		
		SrlPortNOpened=0x01,
		HardwareFail=0x02,
		ReadTimeout=0x03,
		WriteTimeout=0x04,
		MemoryError=0x05,
		BadAnswer=0x06,
		ParityError=0x07,
		BadParameter=0x08
	};
	/**
	 * List of all possible socket errors 
	*/  
	enum SocketErrors {
		InvalidIPAddr=0x00,		
		SockCreationError=0x01,
		SockBindError=0x02,
		ListenError=0x03,
		ConnectError=0x04,
		SocketNCreated=0x05,
		AcceptError=0x06,
		SendError=0x07,
		ReceiveError=0x08,
		UnspecifiedHost=0x09,
		SockConfiguration=0x0a,
		OperationNPermitted=0x0b
	};
	/**
	 * List of all possible dataset errors 
	*/  
	enum DBErrors {
		Memory4Parser=0x00,
		TableLocation=0x01,
		ParserError=0x02,
		IllegalOpened=0x03,
		DalLocation=0x04
	};
	/** 
	 * Constructor.
   */
	CError();
	/** 
	 * Copy constructor.
   */
	CError(const CError& rSrc);
	/**
	 * Destructor.
   */
	virtual ~CError();
	/**
	 * This member function is used to set error type, code and other information
	 * @param Type error type
	 * @param Code error code
	 * @param File file name of the source code that generates the error
	 * @param Routine routine where the error occurs
	 * @param Line line number of the instruction that generates the error
   */
	void setError(ErrorTypes Type,WORD Code,CString File="",CString Routine="",DWORD Line=0);
	/**
	 * Use this member function to set couple of data that represent extra information about the error.
	 * @param Name extra information name
	 * @param Value extra information value
   */
	void setExtra(CString Name,DWORD Value);
	/**
	 * This function reset the error to its original values, that means no errors
	 * reported.
	*/
	void Reset();
	/**
	 * @return return true if the error is a NoError type
	*/
	inline bool isNoError() { return (m_Type==NoError); }
	/**
	 * @return the error type
	*/
	inline ErrorTypes getType() { return m_Type; }
	/**
    * This member function is used to retrieve the error code, given by the
	 * composition of error code and type.
	 * @return the error code
	*/
	DWORD getErrorCode();
	/**
    * This member function is used to retrieve the error message related to the
	 * code.
	 * @return the string that describes the error in a more human readable fashion
	*/
	CString getDescription();
	/**
	 * @return the file name.
   */
	inline CString getFile() { return m_sFile; }	
	/**
	 * @return the routine name.
   */
	inline CString getRoutine() { return m_sRoutine; }	
	/**
	 * @return the line of the instuction that identify the error.
   */
	inline DWORD getLine() { return m_dwLine; }	
	/**
	 * Use this function to know when the error occured.
	 * @param errTime reference to the time the error occured
	*/
	void getTime(TIMEVALUE& errTime);
	/**
	 * Use this function if you have to retrive extra information about the error.
	 * @param Name used to return the field Name of the extra information pair
	 * @param Value used to return the field Value of the extra information pair
	 * @return true if a pair (Name,Value) of extra information has been retrived.
   */
	bool getExtraInfo(CString& Name,DWORD& Value);
	/**
    * This member function is used to retrieve a full explanation of the error.
	 * @return the string that reports the error, the code and some additional information
	*/
	CString getFullDescription();
	/**
	 * Copy operator.
	*/
   void operator=(const CError& rSrc);  
private:
	static const BYTE DataSize=10;
	typedef struct {
		CString Name;
		DWORD Value;
	} T_ErrorData;
	BYTE m_Code;
	ErrorTypes m_Type;
	T_ErrorData m_pData[DataSize];
	WORD m_wDataPointer;
	WORD m_wDataReader;
	CString m_sRoutine;
	CString m_sFile;
	DWORD m_dwLine;
	TIMEVALUE m_ErrorTime;
};

}

#endif
