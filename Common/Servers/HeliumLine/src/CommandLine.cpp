/*
 * CommanLine.cpp
 *
 *  Created on: Feb 10, 2014
 *      Author: spoppi
 */


#include "../include/CommandLine.h"

static char *rcsId="@(#) $Id$";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


CommandLine::CommandLine(void)
{
//	m_mbhandler=

}

CommandLine::~CommandLine(){
	  modbus_free(m_mbhandler);

	  delete(m_registers);
}

int CommandLine::configure(char* ip,int port,int nb,int start)
{
      m_mbhandler = modbus_new_tcp(ip, port);
	  m_nbytes=nb;
	  m_address=start; // address of the supply
//      m_registers=new uint16_t[nb];  /

}

int CommandLine::connect()
{
	  modbus_connect(m_mbhandler);
      return 0;

}

int CommandLine::disconnect(){
	  modbus_close(m_mbhandler);
      return 0;


}

int CommandLine::getPressure(double& supply, double &ret){

	getSupplyPressure(supply);
	getReturnPressure(ret);

	return 0;
}

int CommandLine::getSupplyPressure(double& supply)
{
    ReadValue(m_address,supply);
    return 0;


}

int CommandLine::getReturnPressure(double& ret)
{
	ReadValue(m_address+2,ret);
	return 0;
}

int CommandLine::ReadValue(int address,double& value)
{

	uint16_t temp_val[2]; // read registers
	modbus_read_registers(m_mbhandler, address,2,temp_val); // supply
	value=modbus_get_float(temp_val);
    return 0;


}



