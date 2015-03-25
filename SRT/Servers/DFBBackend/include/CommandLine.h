/*
 * CommandLine.h
 *
 *  Created on: Oct 23, 2013
 *      Author: spoppi
 */

#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include "Socket.h"
#include <IRA>
#include <ComponentErrors.h>
#include <BackendsErrors.h>
#include <TotalPowerS.h>
#include <string>
#include <sstream>
#include <algorithm>

namespace DFB {

class CCommandLine: public IRA::CSocket {
public:
	CCommandLine();
	virtual ~CCommandLine();
	int go(string time);
	int go();

	int stop(void);
	int cycle(string params);
	int fileOpen(string name);
	int fileClose();
    int configuration(string cfg);
    int observer(string obs);
    int obstype(string obstype);
    int source(string source);
    int  frequency(double freq);
    int  restFrequency(double rfreq);
    int  frequencyInvert(bool finv);

private:
    string m_prefix;
    string zeroFill(int n); // fill the string length of the command with zero filled values.
    string composeCmd(string cmd);



};




}

#endif /* COMMANDLINE_H_ */
