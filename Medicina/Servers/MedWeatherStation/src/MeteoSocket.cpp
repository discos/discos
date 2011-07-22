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


int  MeteoSocket::receiveData(CError& err, CString& rdata)
{

	ACS_TRACE("MeteoSocket::receiveData");
	int n_received;
	char buff[MAXSIZE];
	n_received=Receive(err,buff,MAXSIZE);
	buff[n_received]=0;
    	rdata =CString(buff);  
	ACS_DEBUG_PARAM("MeteoSocket::sendCMD(CError& err, CString cmd)","received:  %s", (const char *) rdata);
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

		m_meteodata->sensorMap[COMMANDS[AIRTEMP]]=temp;
		m_meteodata->sensorMap[COMMANDS[AIRPRESSURE]]=pres;
		m_meteodata->sensorMap[COMMANDS[RELHUM]]=hum;
		m_meteodata->sensorMap[COMMANDS[WINDSPEEDAVE]]=wind;
		
		cout <<temp <<":"<<pres<<":"<<hum <<endl;

		 ACS_LOG(LM_FULL_INFO,"MeteoSocket::parse()",(LM_DEBUG," Meteoparms  %f %f %f",temp,pres,hum ));
		  
	} else
	{
		 ACS_LOG(LM_FULL_INFO,"MeteoSocket::update()",(LM_ERROR,"Not enough data from meteo server"));
	}

	return 0;

 
}
 
