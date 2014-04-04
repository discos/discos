#ifndef _SERIALPORT_H
#define _SERIALPORT_H
/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.cnr.it)  08/07/2004      Creation                                          */
/* Andrea Orlati(aorlati@ira.cnr.it)  24/01/2005		Added parameters to get errors from methods 	     */

// $Id: SerialPort.h,v 1.1.1.1 2007-05-03 15:15:47 a.orlati Exp $

#include <termios.h>
#include <sys/poll.h>
#include <baciThread.h>

namespace IRA {

/**
 * This code implements a method to comunicate to any device attached to serial port (RS232, 485....). 
 * The comunication type is "semi-asynchronous": read and write are blocking calls but a timeout is    
 * used. There is also a non-blocking function (isRxReady) that can be used as flag for incoming data. 
 * This implementation is based on timeouts. SIGIO handlers was discarded because you can have just one
 * signal handler even if there are more than one serial port. Thread was discarded because they are   
 * resource consumers.                                                                                 
 * In order to start comunication, after calling constructor use OpenPort to configure and open serial 
 * port. Functions Rx and Tx are used to transmit and receive data. If error conditions are found      
 * ACS exceptions are thrown (please see SerialPortError.xml). A HardwareFail error will be sent   
 * if an error occurs during a call to a system API, in this case additional information are appended: 
 * 'Code' and 'Message'. Table below resumes other returned error codes that developers should         
 * dealt with.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 * <b>ERROR CODE TABLE</b><br>  
 * Error type <i>CError::SerialType=0x0100</i><br>
 * <table border=2>
 *	<tr><th>#</th><th>Memonic</th><th>ShortMessage</th></tr>
 * <tr><td><a name="Excp1">1</a></td><td>SrlPortConfig</td><td>"Port could not be configured"</td></tr>
 * <tr><td><a name="Excp2">2</a></td><td>SrlPortNOpened</td><td>"Port is not opened"</td></tr>
 * <tr><td><a name="Excp3">3</a></td><td>HardwareFail</td><td>"Harware failure"</td></tr>
 * <tr><td><a name="Excp4">4</a></td><td>ReadTimeout</td><td>"Timeout during read operation"</td></tr>
 * <tr><td><a name="Excp5">5</a></td><td>WriteTimeout</td><td>"Timeout during write operation"</td></tr>
 * <tr><td><a name="Excp6">6</a></td><td>MemoryError</td><td>"Buffer could not be allocated"</td></tr>
 * </table>
 */
class CSerialPort
{
public:
	/**
	 * List of baudrates 
	*/
	enum BaudRates {BR50=50,BR75=75,BR110=110,BR134=134,BR150=150,BR200=200,BR300=300,BR600=600,BR1200=1200,BR2400=2400,
	  BR4800=4800,BR9600=9600,BR19200=19200,BR38400=38400,BR57600=57600,BR115200=115200};
	/**
	 * List of possible parity checks 
	*/	  
	enum Parities {ODD,EVEN,NOPARITY};
	/**
	 * List of possible numeber of bits per datum
	*/ 
	enum DataBits {D5=5,D6=6,D7=7,D8=8};
	/**
	 * number of stop bits
	*/
	enum StopBits {STOP1=1,STOP2=2};
	/**
	 * Possible handshakes
	*/
	enum Handshakes {NONE,HARDWARE,XONXOFF};
	/**
	 * Constructor
	 * @param Port device name, es: /dev/ttyS1
	*/
	CSerialPort(CString Port);
	/** 
	 * Destructor
	 * It also tries to close the port 
	*/
	virtual ~CSerialPort();
	/**
	 * Call this member function to open and configure the port.
 	 * @param Rate baudrate
	 * @param DataB number of data bits
	 * @param Par type of parity check
	 * @param Stop number of stop bits
	 * @param HandS handshake
	 * @param Timeout time allotted for an I/O operation, in milliseconds
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 1 <a href="#Excp1">SrlPortConfig</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 6 <a href="#Excp6">MemoryError</a>
	*/
	void OpenPort(CError& Err,BaudRates Rate=BR9600,DataBits DataB=D8,Parities Par=NOPARITY,StopBits Stop=STOP1,Handshakes HandS=NONE,int Timeout=200);
	/**
	 * Read some data from the serial port.
 	 * @param Buff buffer used to return read data 
	 * @param ReadBytes max number of bytes to be read 
	 * @param Err eventually used to return an error code:
 	 * 		 @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * 		 @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 * 		 @arg \c 4 <a href="#Excp4">ReadTimeout</a>
	 * @return the number of read bytes if no error was encountered
   */
	int Rx(BYTE *Buff,WORD ReadBytes,CError& Err);
	/**
	 * Transmit some data toward the serial port.
	 * @param Buff buffer containing data
	 * @param WriteBytes buffer length
	 * @param Err eventually used to return an error code:
	 *        @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 *        @arg \c 3 <a href="#Excp3">HardwareFail</a>
	 *        @arg \c 5 <a href="#Excp5">WriteTimeout</a>
	 * @return the number of bytes effectively written 
	*/
	int Tx(const BYTE *Buff,WORD WriteBytes,CError& Err);
	/**
	 * This function checks if there is some data to be read
	 * @param Err eventually used to return an error code:
	 *        @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * @return true if there is data ready
	*/
	bool isRxReady(CError& Err);
	/**
	 * This function checks if data can be trensmitted without blocking
	 * @param Err eventually used to return an error code:
	 *        @arg \c 2 <a href="#Excp2">SrlPortNOpened</a>
	 * @return true if data can be written
	*/
	bool isTxReady(CError& Err);
	/** 
	 * Used to close port. No need to call it directly, this is done by the destructor 
	*/ 
	void ClosePort();
	/**
	 * @return device name, es: /dev/ttyS1
	*/
	inline CString getPort() { return m_sPortName; }
	/**
	 * @return configured baudrate
	*/ 
	inline BaudRates getBaudRate() { return m_Rate;}
	/**
	 * @return configured number of data bits
	*/ 
	inline DataBits getDataBit() { return m_DataB; }
	/**
	 * @return configured parity check
	*/ 
	inline Parities getParity() { return m_Par; }
	/**
	 * @return configured nmber of stop bits
	*/ 
	inline StopBits getStopBit() { return m_Stop; }
	/**
	 * @return configured handshake
	*/ 
	inline Handshakes getHandShake() { return m_HandS; }
	/**
	 * @return configured timeout
	*/ 
	inline int getTimeout() { return m_Timeout; }
protected:
	CString m_sPortName;
	int m_iPortD;
	bool m_bOpened;
	BaudRates m_Rate;
	DataBits m_DataB;
	Parities m_Par;
	StopBits m_Stop;
	Handshakes m_HandS;
	int m_Timeout;
	// critical section mutex: if the serial line is used by several thread concurrently, lock this mutex.
	BACIMutex m_SerialMutex;
private:
	struct termios m_stOldConfig;
	struct pollfd m_stPolling;
};

}

#endif
