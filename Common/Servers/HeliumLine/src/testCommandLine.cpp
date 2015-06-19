/*
 * testCommandLine.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: spoppi
 */

#include "../include/CommandLine.h"
#include <iostream>

#define IP "192.168.200.64"
#define PORT 502

int main (int argc,char** argv)
{
	double in,out;

    CommandLine* cl= new CommandLine();
    cl->configure(IP,PORT,0,4);
    cl->connect();
    cl->getPressure(in,out);
    std::cout << in << "-" <<out<<std::endl;
    return 0;


}
