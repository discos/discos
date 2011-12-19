/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) UNSPECIFIED - FILL IN, 2011 
* 
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* "@(#) $Id$"
*
* who       when      what
* --------  --------  ----------------------------------------------
* spoppi  2011-09-26  created 
*/

/************************************************************************
*   NAME
*   
* 
*   SYNOPSIS
*
*   
*   PARENT CLASS
*
* 
*   DESCRIPTION
*
*
*   PUBLIC METHODS
*
*
*   PUBLIC DATA MEMBERS
*
*
*   PROTECTED METHODS
*
*
*   PROTECTED DATA MEMBERS
*
*
*   PRIVATE METHODS
*
*
*   PRIVATE DATA MEMBERS
*
*
*   FILES
*
*   ENVIRONMENT
*
*   COMMANDS
*
*   RETURN VALUES
*
*   CAUTIONS 
*
*   EXAMPLES
*
*   SEE ALSO
*
*   BUGS   
* 
*------------------------------------------------------------------------
*/

#include "vltPort.h"
#include "CommandLine.h"
static char *rcsId="@(#) $Id$"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


static const int MAXLEN=255; // max len of the reply from a gpib quert


static const string FREQCMD="FREQ ";
static const string QUERYFREQ="FREQ? ";
static const string QUERYPOWER="POWER? ";
static const string POWERCMD="FREQ AMPL";
static const string RFONCMD="OUTP:STAT ON";
static const string RFOFFCMD="OUTP:STAT OFF";
static const string QUERYRF="OUTP:STAT?";
static const string FREQUNIT=" MHZ";
static const string POWERUNIT=" dBM";



CommandLine::CommandLine()
{
//	ACS_TRACE("CommandLine::CommandLine()");
	GPIB_errors[EDVR]="System error";
	GPIB_errors[ECIC]="Function requires GPIB board to be CIC";
    GPIB_errors[ENOL]="No Listeners on the GPIB";
    GPIB_errors[EADR]="GPIB board not addressed correctly";
    GPIB_errors[EARG]="Invalid argument to function call";
    GPIB_errors[ESAC]="GPIB board not System Controller as required";
    GPIB_errors[EABO]="I/O operation aborted (timeout)";
    GPIB_errors[ENEB]="Nonexistent GPIB board";
    GPIB_errors[EDMA]="DMA error";
    GPIB_errors[EOIP]="Asynchronous I/O in progress";
    GPIB_errors[ECAP]="No capability for operation";
    GPIB_errors[EFSO]="File system error";
    GPIB_errors[EBUS]="GPIB bus error";
    GPIB_errors[ESRQ]="SRQ stuck in ON position";
    GPIB_errors[ETAB]="Table problem";
    GPIB_errors[ELCK]="Interface is locked";
    GPIB_errors[EARM]="Ibnotify callback failed to rearm";
    GPIB_errors[EWIP]="Wait in progress on specified input handle";
    GPIB_errors[ERST]="Event notification was cancelled due to a reset of the interface";
    GPIB_errors[EPWR]="The interface lost power";

}

CommandLine::~CommandLine()
{


}


int CommandLine::configure(int bid,int pad,int sat,int tmo, int eot, int eos) throw(GPIBException)

{

	m_device = ibdev(                /* Create a unit descriptor handle         */
	                    bid,              /* Board Index (GPIB0 = 0, GPIB1 = 1, ...) */
	                    pad,          /* Device primary address                  */
	                    sat,        /* Device secondary address                */
	                    tmo,                    /* Timeout setting (T10s = 10 seconds)     */
	                    eot,                       /* Assert EOI line at end of write         */
	                    eos);

 	if (m_device==ERR) {
 						cout <<"err" <<endl;
					   throw GPIBException("ibdev:"+GPIB_errors[ThreadIberr()]);
					}
	if (m_device==TIMO) {
					   throw GPIBException("ibdev:timeout");
					}
	if (ThreadIbsta()& ERR )
		{
						   throw GPIBException("ibdev:"+GPIB_errors[ThreadIberr()]);
		}
	return 0;

}

int CommandLine::connect(string& reply)
{
	return 0;


}

int CommandLine::query(string cmd,string& reply) throw (GPIBException)
{

	char buff[MAXLEN];

	ibwrt(m_device,(char* )cmd.c_str(), cmd.size());
	if (ThreadIbsta()& ERR )
	{
 					   throw GPIBException("ibrwt:"+GPIB_errors[ThreadIberr()]);
	}
/* Send the identification query command   */
	ibrd(m_device, buff, MAXLEN);
	if (ThreadIbsta()& ERR )
		{
						   throw GPIBException("ibrd:"+GPIB_errors[ThreadIberr()]);
		}/* Read up to 100 bytes from the device
	*/
	buff[ThreadIbcnt()] = '\0';	     /* Null terminate the ASCII string 	*/
	reply = string(buff) ;

	return 0;


}
int CommandLine::disconnect() throw (GPIBException)
{
	ibonl(m_device,0);
	if (ThreadIbsta()& ERR )
		{
			throw GPIBException("ibonl:"+GPIB_errors[ThreadIberr()]);
		}

	return 0;

}

int CommandLine::init(string& reply) throw (GPIBException)
		{
			try {
				query("*IDN?",reply);
				sendCMD("*RST"); // reset
				sendCMD("POW:ALC:SOUR INT"); // enable internal leveling;
			}  	catch (GPIBException& ex)

			{
					throw ex;
			}
			return 0;

		}



int CommandLine::setPower(double pow) throw (GPIBException)
{

	try{
		string cmd;
		cmd="POW " + stringify(pow)+POWERUNIT;
		sendCMD(cmd);

	} catch (GPIBException& ex)

	{

		throw;

	}
	return 0;

}

int CommandLine::getFreq(double & freq) throw (GPIBException)

{
	try {
		string reply;
		query(QUERYFREQ,reply);
		cout << "commandline freq:" <<reply << endl;
		freq=atof(reply.c_str());
		return 0;

	}  	catch (GPIBException& ex)

	{


			throw ex;
			return -1;

	}
}


int CommandLine::setFreq(double freq) throw (GPIBException){

	try{

		string cmd;
		cmd=FREQCMD+ stringify(freq)+FREQUNIT;
		sendCMD(cmd);

	} catch (GPIBException& ex)

	{


		throw ex;

	}


	return 0;




}

int CommandLine::getPower(double& pow)  throw (GPIBException)
{
	try{
		        string reply;
				query(QUERYPOWER,reply);
				pow=atof(reply.c_str());
	}catch(GPIBException& ex){
		throw ex;
	}
	return 0;

}

int CommandLine::sendCMD(string cmd) throw(GPIBException)
{

 	ibwrt(m_device,const_cast<char*>(cmd.c_str()), cmd.size());
	int i;
	i=ThreadIbsta();

 	if (i& ERR )
	{
		cout << "Error:" <<endl;
		throw GPIBException("ibrwt:"+GPIB_errors[ThreadIberr()]);
	}

	return 0;

}


void CommandLine::queryErrors(string& reply) throw(GPIBException)
{

	try{
	  //      string reply;
			query("SYST:ERR?",reply);

 		}catch(GPIBException& ex){
			throw ex;
		}
 }

int CommandLine::parseSCPIErrors(string msg,string& reply)

{

	// the scpi errors, i.e. the internal errors queue, is
	// made of   code,"message" pair
	// e.g.   -113,"Undefined header"

	int code;

	size_t comma;
	comma=msg.find(",") ; //
	code =atoi( msg.substr(0,comma).c_str());
	reply=msg.substr(comma+1);
	return code;




}

int CommandLine::rfOn() throw (GPIBException)
{
	try{
		string reply;
		int iReply;

		query(QUERYRF,reply);
		iReply=atof(reply.c_str());
		if (iReply == 0){
			sendCMD(RFONCMD);

		} //  turn on only if is off.


	} catch (GPIBException& ex)
	{
		throw ex;

	}



}

int CommandLine::rfOff() throw (GPIBException)
{

	try{
			string reply;
			int iReply;
			query(QUERYRF,reply);
			iReply=atof(reply.c_str());

			if (iReply == 1){
				sendCMD(RFOFFCMD);

			} //  turn on only if is off.


		} catch (GPIBException& ex)
		{
			throw ex;

		}



}

/*___oOo___*/
