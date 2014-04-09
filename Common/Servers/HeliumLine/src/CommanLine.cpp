/*
 * CommanLine.cpp
 *
 *  Created on: Feb 14, 2014
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

int CommandLine::configure(string ip,int port,int nb,int start)
{
      m_mbhandler = modbus_new_tcp(ip.c_str(), port);
	  m_nbytes=nb;
	  m_address=start; // address of the supply
//      m_registers=new uint16_t[nb];  /

}

int CommandLine::connect()
{
	  modbus_connect(m_mbhandler);

}

int CommandLine::disconnect(){
	  modbus_close(m_mbhandler);


}

int CommandLine::getPressure(double& supply, double &ret){



	uint16_t temp_val[2]; // read registers
	modbus_read_registers(m_mbhandler, m_address,2,temp_val); // supply
	supply =modbus_get_float(temp_val);
	modbus_read_registers(m_mbhandler, m_address+2,2,temp_val); // return
	ret =modbus_get_float(temp_val);

	return 0;
}


