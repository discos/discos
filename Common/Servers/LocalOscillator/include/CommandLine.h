#ifndef __COMMANDLINE__
#define __COMMANDLINE__

#include <ni488.h>
#include <cstring>
#include <string>
#include <LogFilter.h>
#include <exception>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

class BadConversion : public runtime_error {
 public:
   BadConversion(string const& s)
     : runtime_error(s)
     { }
 };

 inline string stringify(double x)
 {
   std::ostringstream o;
   if (!(o << x))
     throw BadConversion("stringify(double)");
   return o.str();
 }



/**
 * Command line forcommunication with the GPIB instrument
 *
 *
 *
 */
class GPIBException : public exception {

public:

	GPIBException();
	GPIBException(string msg){m_msg=msg;}
	virtual ~GPIBException() throw(){};
	const char* what() const throw(){
		return m_msg.c_str();	}
private:
	string m_msg;

};






class CommandLine {

public:

	CommandLine();
	virtual ~CommandLine();
	/**
	 * @param boardID = Board id;
	 * @param pad     = Primary addres
	 * @param sad    = Secondary address
	 * @param tmo	  = Timeout;
	 * @param eot     =
	 * @param eos     =
	 */
	 map<int, string> GPIB_errors;

	int configure(int boardID, int pad, int sad, int tmo, int eot, int eos) throw (GPIBException);

	/**d
	 *
	 *
	 */
	int  connect(string& reply) ;
	int disconnect();

	int setFreq(double) throw (GPIBException);
	int setPower(double) throw (GPIBException);
	int getPower (double&) throw (GPIBException);
	int getFreq (double&) throw (GPIBException);


	inline int getDevice() {return m_device;}


	int sendCMD(string cmd) throw (GPIBException);
	int receiveData(string cmd)  throw (GPIBException);
	int query(string cmd,string& reply) throw (GPIBException);
	int checkError()throw (GPIBException){
		if (ThreadIbsta()==ERR) {
		   throw GPIBException(GPIB_errors[ThreadIberr()]);
		}
;
	}
private:

	int m_device; // GPIB device

};




#endif
