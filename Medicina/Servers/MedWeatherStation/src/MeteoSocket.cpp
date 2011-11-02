#include  "MeteoSocket.h"
#define MAXSIZE 255
int MeteoSocket::Depth=0;


MeteoSocket::MeteoSocket(CString addr, unsigned int port )
{



	ACS_TRACE("MeteoSocket::MeteoSocket");
	m_isConnected=false;
	ADDRESS=addr;
	PORT=port;

}
// 

MeteoSocket::MeteoSocket(CString addr, unsigned int port,MeteoData *md)
{
	ACS_TRACE("MeteoSocket::MeteoSocket");

	m_meteodata=md;
	m_isConnected=false;
	ADDRESS=addr;
	PORT=port;
// 	ACS_LOG(LM_FULL_INFO,"MeteoSocket::MeteoSocket()",(LM_INFO,"Creating xlm parser and handlers"));


}



MeteoSocket::~MeteoSocket()
{
	
	ACS_TRACE("MeteoSocket::~MeteoSocket");


}
int MeteoSocket::sendCMD(CError& err, CString cmd)
{
	ACS_TRACE("MeteoSocket::sendCMD");
    	Send(err,(const char *)cmd,cmd.GetLength());

	ACS_DEBUG_PARAM("MeteoSocket::sendCMD(CError& err, CString cmd)","sent:  %s", (const char *) cmd);

	return 0;



}

int MeteoSocket:: updateParam(){
	    CError err;
	    CString rdata="";
	    sendCMD(err,CString(WEATHERCMD));
		ACS_DEBUG_PARAM("MeteoSocket::updateParam(CError& err, CString cmd)","sent:  %s", (const char *) WEATHERCMD);

 		IRA::CIRATools::Wait(0,100000);

		receiveData(err,rdata);
		ACS_DEBUG_PARAM("MeteoSocket::updateParam(CError& err, CString cmd)","received:  %s", (const char *) rdata);

 		parse(rdata);
 		if (err.isNoError()) return 0;
 		else return -1;


}

int  MeteoSocket::receiveData(CError& err, CString& rdata)
{

 
	//rdata="";
	
	ACS_TRACE("MeteoSocket::receiveData");
	int n_received,n_received_total=0;

	char buff[MAXSIZE];
	char receivedChar=0;
	int i=0;
	while (receivedChar!='\n')
	{
	
	
		n_received=Receive(err,&receivedChar,1);
	//	receivedChar=buff[0];
		if (i >= MAXSIZE ) // avoid pointer overflow
		{
			buff[i]=0;
			rdata=CString(buff);
			return n_received_total;
					
		}
		
		
		buff[i++]=receivedChar;
		n_received_total = i;
		
	}
	buff[n_received_total]=0;
	rdata=CString(buff);
	ACS_DEBUG_PARAM("MeteoSocket::receiveData(CError& err, CString cmd)","received:  %s", (const char *) rdata);
	return n_received_total;
	
		
			
		
//		n_received=Receive(err,buff,MAXSIZE);
//		buff[n_received]=0;
//    	rdata =CString(buff);  
//		ACS_DEBUG_PARAM("MeteoSocket::sendCMD(CError& err, CString cmd)","received:  %s", (const char *) rdata);
 	
 		return 0;

}

CError MeteoSocket::connect() throw (ACSErr::ACSbaseExImpl)

{
	OperationResult err ;
	err=Create(m_error,STREAM);  
	if (err==FAIL)
	{
		_EXCPT(ComponentErrors::SocketErrorExImpl,ex,"MeteoSocket::connect()- Create Socket");
		ex.log(LM_DEBUG);
		throw ex;
		m_isConnected=false;
		return m_error;
	} 

	err=Connect(m_error,ADDRESS,PORT);
	if (err==FAIL)
	{

		CError error; // CError object only for Closing the Socket)		
		_EXCPT(ComponentErrors::SocketErrorExImpl,ex,"MeteoSocket::connect()- Connect to Socket");
		ex.log(LM_DEBUG);
		throw ex;
		Close (error);
		m_isConnected=false;
		return m_error;
	} 
	m_isConnected=true;
// 
	return m_error;


}

CError MeteoSocket::disconnect()throw (ACSErr::ACSbaseExImpl)
{

	 ACS_LOG(LM_FULL_INFO,"MeteoSocket::Disconnect()",(LM_INFO,"  disconnecting Socket"));
	try {
		if (m_isConnected)
  		{	Close(m_error);
			if (m_error.isNoError())
			{
	
			 ACS_LOG(LM_FULL_INFO,"MeteoSocket::Disconnected()",(LM_DEBUG,"Disconnecting Socket  ",(const char *) ADDRESS,PORT));
 			} else
			{
		  	ACS_LOG(LM_FULL_INFO,"MeteoSocket::Disconnect()",(LM_ERROR,"%s",(const char *) m_error.getDescription()));
	     		_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"MeteoStation::disconnect()");	

			}
		}
	} catch (...)
	{
// 	cout << "unknown exception" << endl; 
		ACS_LOG(LM_FULL_INFO,"MeteoSocket::Disconnect()",(LM_ERROR,"%s",(const char *) m_error.getDescription()));

	}
		
	return m_error;
	
}

double MeteoSocket::getWindSpeed()
{
//	if((updateParam()==-1)) cout <<"Error Reading Param"<< endl;
	updateParam();

	m_windspeed=-99; // windspeed disabled
	ACS_LOG(LM_FULL_INFO,"MeteoSocket::getWindSpeed()",(LM_TRACE,"Not yet implemented"));


	//to be impemented
	return m_windspeed;

}
double MeteoSocket::getWindDir()
{
//	if((updateParam()==-1)) cout <<"Error Reading Param"<< endl;
//	updateParam();
	ACS_LOG(LM_FULL_INFO,"MeteoSocket::getWindDir()",(LM_TRACE,"Not yet implemented"));

	m_winddir=-99;
	return m_winddir;

}
double MeteoSocket::getTemperature()
	{

		if((updateParam()==-1)){
			ACS_LOG(LM_FULL_INFO,"MeteoSocket::getTemperature()",(LM_ERROR,"Reading Temperature"));
		}
		return m_temperature;

	}
double MeteoSocket::getHumidity()	{
		//to be impemented
	if((updateParam()==-1)){
		ACS_LOG(LM_FULL_INFO,"MeteoSocket::getHumidity()",(LM_ERROR,"Reading Humidity"));
	}

	return m_humidity;
	}
double MeteoSocket::getPressure()	{
		//to be impemented
	if((updateParam()==-1)){
		ACS_LOG(LM_FULL_INFO,"MeteoSocket::getPressure()",(LM_ERROR,"Reading Pressure"));
	}
	return m_pressure;

	}

//double MeteoSocket::getWinDir()	{
//		// to be implemented
//		return -99;
//
//
//}




void MeteoSocket::initParser(MeteoData *md)
{

	 m_meteodata=md;


}





int MeteoSocket::parse(const char* recv)
{
	int len;
	len=strlen(recv);

//  	cout <<"received"<< len << endl;
	string ss;
	string srecv;
	srecv=string(recv);
	vector<string> vrecv;

	istringstream  ist(srecv); // string stream
	while (ist >> ss) vrecv.push_back(ss) ;// split the string
	int ndata=vrecv.size();
	if (ndata > 3)
	
	{
	 	double temp,pres,hum,wind;
		temp = atof(vrecv[ndata-3].c_str());
		pres = atof(vrecv[ndata-2].c_str());
		hum  = atof(vrecv[ndata-1].c_str());
		wind  = -99.;//  atof(vrecv[ndata-1].c_str());

		m_temperature=temp;
		m_pressure=pres;
		m_humidity=hum;
		m_windspeed=wind;
		


		 ACS_LOG(LM_FULL_INFO,"MeteoSocket::parse()",(LM_DEBUG," Meteoparms  %f %f %f",temp,pres,hum ));
		  
	} else
	{
		 ACS_LOG(LM_FULL_INFO,"MeteoSocket::parse()",(LM_ERROR,"Not enough data from meteo server"));
		 return -1;

	}

	return 0;

 
}
 
