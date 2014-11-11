/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.cnr.it)  21/07/2004      Creation                                          */
/* Andrea Orlati(aorlati@ira.cnr.it)  13/08/2004      Errors handled with ACS exceptions                */
/*                                                                                                      */

// $Id: Lecom.h,v 1.1.1.1 2007-05-03 15:15:47 a.orlati Exp $

#ifndef _LECOMPROT_H
#define _LECOMPROT_H

namespace IRA {

/** 
 * This code implements the LECOM -A/B protocol to comunicate via serial ports to the LENZE  
 * EVS9300EP drive. Code numbers are coded with standard addressing or with extended addressing.
 * Methods taking Code and SubCode as parameters implement extended addressing.
 * The base class is CSerialPort, also all errors code are inherited.
 * After creation, base class method OpenPort must be called before any other operation.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 * <b>ERROR CODE TABLE</b> (All erros from base class are inherited.<br>  
 * Error type <i>CErros::SerialType=0x0100</i><br>
 * <table border=2>
 *	<tr><th>#</th><th>Memonic</th><th>ShortMessage</th></tr>
 * <tr><td><a name="Excp7">7</a></td><td>BadAnswer</td><td>"Unrecognized answer format"</td></tr>
 * <tr><td><a name="Excp8">8</a></td><td>ParityError</td><td>"Unrecognized answer format"</td></tr>
 * <tr><td><a name="Excp9">9</a></td><td>BadParameter</td><td>"Parameter doesn't exist in this controller"</td></tr>
 * <tr><td><a name="Excp1">1</a></td><td>SrlPortConfig</td><td>"Port could not be configured"</td></tr>
 * <tr><td><a name="Excp2">2</a></td><td>SrlPortNOpened</td><td>"Port is not opened"</td></tr>
 * <tr><td><a name="Excp3">3</a></td><td>HardwareFail</td><td>"Harware failure"</td></tr>
 * <tr><td><a name="Excp4">4</a></td><td>ReadTimeout</td><td>"Timeout during read operation"</td></tr>
 * <tr><td><a name="Excp5">5</a></td><td>WriteTimeout</td><td>"Timeout during write operation"</td></tr>
 * <tr><td><a name="Excp6">6</a></td><td>MemoryError</td><td>"Buffer could not be allocated"</td></tr>
 * </table>
 */
class CLecom : public CSerialPort
{
public:
	/**
	 * List of possible device adresses
	*/
	enum Addrs {BROADCAST};
	/** 
	 * Constructor
	 * @param Port device name es: /dev/ttyS1
	 * @param Err eventually used to return an error code:
	 *        @arg \c 6 <a href="#Excp6">MemoryError</a>	 
	*/
	CLecom(CString Port,CError& Err);
	/**
	 * Destructor
	*/
	virtual ~CLecom();
	/**
	 * The receive command is used to request parameter values from controllers.
	 * This functions prepare and send the telegram
	 * @param Addr address of the slave(device)
	 * @param Code code number assigned to the parameter to be read
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>
	*/
	void ReceiveCommand(BYTE Addr,WORD Code,CError& Err);
	/**
	 * The receive command is used to request parameter values from controllers.
	 * This functions prepare and send the telegram
	 * @param Addr address of the slave(device)
	 * @param Code code number assigned to the parameter to be read
	 * @param SubCode subcode number, in this case the extended notation is used
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>
	*/	
	void ReceiveCommand(BYTE Addr,WORD Code,BYTE SubCode,CError& Err);
	/**
	 * The send command is used to set parameter values inside the controllers.
	 * This function prepares and sends the telegram
	 * @param Addr address of the slave(device)
	 * @param Code code number assigned to the parameter to be set
	 * @param Val new value of the parameter
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>
	*/ 
	void SendCommand(BYTE Addr,WORD Code,double Val,CError& Err);
	/**
	 * The send command is used to set parameter values inside the controllers.
	 * This function prepares and sends the telegram
	 * @param Addr address of the slave(device)
	 * @param Code code number assigned to the parameter to be set
	 * @param Val new value of the parameter
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	*/ 	
	void SendCommand(BYTE Addr,WORD Code,int Val,CError& Err);
	/**
	 * The send command is used to set parameter values inside the controllers.
	 * This function prepares and sends the telegram
	 * @param Addr address of the slave(device)
	 * @param Code code number assigned to the parameter to be set
	 * @param SubCode subcode number, in this case the extended notation is used.
	 * @param Val new value of the parameter
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	*/ 	
	void SendCommand(BYTE Addr,WORD Code,BYTE SubCode,double Val,CError& Err);
	/**
	 * The send command is used to set parameter values inside the controllers.
	 * This function prepares and sends the telegram
	 * @param Addr address of the slave(device)
	 * @param Code code number assigned to the parameter to be set
	 * @param SubCode subcode number, in this case the extended notation is used.	 
	 * @param Val new value of the parameter
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	*/ 	
	void SendCommand(BYTE Addr,WORD Code,BYTE SubCode,int Val,CError& Err);
	/** 
	 * Answer read and decodes the answer from the controller. This function follows
	 * a send command and will return if the controller has understood the request or not.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a> 
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 * 		 @arg \c 9 <a href="#Excp9">BadParameter</a>
	 * @return true if the controller has acknowledged the request.
	*/
	bool Answer(CError& Err);
	/** 
	 * Answer read and decodes the answer from the controller. This function follows
	 * a receive command and will return the value reported by the controller.
	 * @param Code code number of the parameter to be read
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 * 		 @arg \c 9 <a href="#Excp9">BadParameter</a>	 
	 * @return the value of the requested parameter
	*/
	double Answer(WORD Code,CError& Err);
	/** 
	 * Answer read and decodes the answer from the controller. This function follows
	 * a receive command and will return the value reported by the controller. This version
	 * is used for extended notation.
	 * @param Code code number of the parameter to be read
	 * @param SubCode subcode number, in this case the extended notation is used.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 * 		 @arg \c 9 <a href="#Excp9">BadParameter</a>	 
	 * @return the value of the requested parameter
	*/	
	double Answer(WORD Code,BYTE SubCode,CError& Err);
	/**
	 * This is a macro. It is the same as a call to SendCommand and then a call to Answer.
	 * This functions will keep busy the line until the controller answers to the request or timeout expires.
	 * @param Addr address of the slave(device) 
	 * @param Code code number of the parameter to be read
	 * @param Val new value of the parameter
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 * 		 @arg \c 9 <a href="#Excp9">BadParameter</a>	 
	 * @return true if the controller has acknowledged the request
	*/
	bool Set(BYTE Addr,WORD Code,double Val,CError& Err);
	/**
	 * This is a macro. It is the same as a call to SendCommand and then a call to Answer.
	 * This functions will keep busy the line until the controller answers to the request or timeout expires.
    * @param Addr address of the slave(device)
	 * @param Code code number of the parameter to be read
	 * @param Val new value of the parameter
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 *			 @arg \c 9 <a href="#Excp9">BadParameter</a>	 
	 * @return true if the controller has acknowledged the request
	*/	
	bool Set(BYTE Addr,WORD Code,int Val,CError& Err);
	/**
	 * This is a macro. It is the same as a call to SendCommand and then a call to Answer.
	 * This functions will keep busy the line until the controller answers to the request or timeout expires.
	 * @param Addr address of the slave(device)
	 * @param Code code number of the parameter to be read
	 * @param SubCode subcode number, in this case the extended notation is used.
	 * @param Val new value of the parameter
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 * 		 @arg \c 9 <a href="#Excp9">BadParameter</a>	 
	 * @return true if the controller has acknowledged the request
	*/	
	bool Set(BYTE Addr,WORD Code,BYTE SubCode,double Val,CError& Err);
	/**
	 * This is a macro. It is the same as a call to SendCommand and then a call to Answer.
	 * This functions will keep busy the line until the controller answers to the request or timeout expires.
	 * @param Addr address of the slave(device)
	 * @param Code code number of the parameter to be read
	 * @param SubCode subcode number, in this case the extended notation is used.
	 * @param Val new value of the parameter
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 * 		 @arg \c 9 <a href="#Excp9">BadParameter</a>	 	 
	 * @return true if the controller has acknowledged the request
	*/	
	bool Set(BYTE Addr,WORD Code,BYTE SubCode,int Val,CError& Err);
	/** 
	 * This is a macro. It is the same as a call to ReceiveCommand and then a call to Answer.
	 * These functions will keep busy the line until the controller answer to the request or timeout expires.
	 * @param Addr address of the slave(device)
	 * @param Code code number of the parameter to be read 
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 * 		 @arg \c 9 <a href="#Excp9">BadParameter</a>	 	 
	 * @return the double value the controller has answered to the request.  
	*/
	double Get(BYTE Addr,WORD Code,CError& Err);
	/** 
	 * This is a macro. It is the same as a call to ReceiveCommand and then a call to Answer.
	 * These functions will keep busy the line until the controller answer to the request or timeout expires.
	 * @param Addr address of the slave(device)
	 * @param Code code number of the parameter to be read
	 * @param SubCode subcode number, in this case the extended notation is used. 
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 5 <a href="#Excp5">WriteTimeout</a>	 
	 * 		 @arg \c 7 <a href="#Excp7">BadAnswer</a>
	 * 		 @arg \c 8 <a href="#Excp8">ParityError</a>
	 * 		 @arg \c 9 <a href="#Excp9">BadParameter</a>	 	 
	 * @return the double value the controller has answered to the request.  
	*/	
	double Get(BYTE Addr,WORD Code,BYTE SubCode,CError& Err);
	/**
	 * This implements Singleton pattern. This function will be necessary if two or more modules needs to use the same 
	 * serial line. In order to avoid to create the same object (that means open the same serial port and then a
	 * "protection error") a module must call direcly this function.
	 * @param Err eventually used to return an error code:
 	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * 		 @arg \c 6 <a href="#Excp6">MemoryError</a>	 
	 * @param Port device name, es: /dev/ttyS1
	 * @param Rate baudrate
	 * @param DataB number of data bits
	 * @param Par type of parity check
	 * @param Stop number of stop bits
	 * @param HandS handshake
	 * @param Timeout time allotted for an I/O operation, in milliseconds
	 * @return pointer to a CLecom object, NULL if it fails 
	*/
	static CLecom *Create(CError& Err,CString Port,BaudRates Rate=BR9600,DataBits DataB=D8,Parities Par=NOPARITY,StopBits Stop=STOP1,Handshakes HandS=NONE,
	  int Timeout=200);
	/**
	 * If a CLecom object has been allocated through the create function, it must be cleared by calling this function
	 * @param Object CLecom object to clean up
	*/ 
	static void Destroy(CLecom *Object);  
protected:
	BYTE *m_ucBuffer;
	void CodeNumber(const WORD Code,BYTE &C1,BYTE &C2);
	void Address(const BYTE Addr,BYTE &A1,BYTE &A2);
	//return the CRC number.
	BYTE CheckNumber(BYTE *Buff,WORD Len);
	// Compose a telegram for the transmission. Return the length of the packed buffer.
	WORD Pack(BYTE *Buff,BYTE Addr,WORD Code,CString Val);
	// Compose a telegram for the transmission using extened notation. Return the length of the packed buffer.
	WORD Pack(BYTE *Buff,BYTE Addr,WORD Code,BYTE SubCode,CString Val);
	// this method decode the answer from the controller.
	bool UnpackValue(BYTE *Buff,WORD Len,double &Val);
	// Convert a character buffer into an integer value. True if conversione can be performed.
	// Base indicates the notation used. If an overflow occurred MAX_INT is returned. MIN_INT
	// in case of underflow.
	bool StrToInteger(const BYTE *Buff,int Base,int &Val);
	// Convert a character buffer into a double value. True if conversione can be performed.
	// if an overflow (plud or minus) occurred HUGE_VALF and HUGE_VALL are returned rispectvely.
	bool StrToDouble(const BYTE *Buff,double &Val);
};

}
#endif
