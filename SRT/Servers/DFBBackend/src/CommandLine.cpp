/*
 * CommandLine.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: spoppi
 */

#include "CommandLine.h"

namespace DFB {

CCommandLine::CCommandLine() {
	// TODO Auto-generated constructor stub



}

CCommandLine::~CCommandLine() {
	// TODO Auto-generated destructor stub
}

string CCommandLine::zerofill(int n)
{


	    stringstream stream("");
	    stream.width(4);
	    stream.fill('0');
	    stream << n;
	    return stream.str();

}

string CCommandLine::composeCmd(string cmd)
{
    string composed_cmd;
    transform(cmd.begin(), cmd.end(),cmd.begin(), ::toupper);// capitalize
    composed_cmd=m_prefix+" "+zerofill(cmd.size())+cmd;
    return composed_cmd;

}


int  CCommandLine::configuration(string cfg)
{


	return 0;


}

int CCommandLine::cycle(string)
{


	return 0;
}

int CCommandLine::fileClose()
{

	return 0;
}

int CCommandLine::fileOpen(string name)
{

	return 0;

}

int CCommandLine::frequency(double freq)
{

	return 0;
}

int CCommandLine::frequencyInvert(bool finv)
{

	return 0;
}

int CCommandLine::go(string time){

	return 0;

}

int CCommandLine::go(){

	return 0;

}
int CCommandLine::observer(string obs)
{


	return 0;

}

int CCommandLine::obstype(string obstype)
{

	return 0;
}

int CCommandLine::restFrequency(double freq)
{


	return 0;
}

int CCommandLine::source(string source)
{


	return 0;

}

int CCommandLine::stop()
{

}



}


