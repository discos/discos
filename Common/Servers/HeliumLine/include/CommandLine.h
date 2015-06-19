/*
 * CommandLine.h
 *
 *  Created on: Feb 14, 2014
 *      Author: spoppi
 */


#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <modbus/modbus.h>
#include <iostream>
#include <string>
using namespace std;

class CommandLine {
   public:
     CommandLine();
     ~CommandLine();
     int configure(char* ip,int port,int nb,int start);

     int connect();
     int disconnect();
     int getPressure(double& in,double& out);
     int getSupplyPressure(double&);
     int getReturnPressure(double&);


  private:
     int ReadValue(int address,double& value);
     modbus_t *m_mbhandler;
     int m_address;
     int m_nbytes;
     uint16_t *m_registers;

};


#endif /* COMMANDLINE_H_ */
