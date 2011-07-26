#include  "SRTWeatherSocket.h"
#define MAXSIZE 255
int SRTWeatherSocket::Depth=0;


SRTWeatherSocket::SRTWeatherSocket(CString addr, unsigned int port )
{



	ACS_TRACE("SRTWeatherSocket::SRTWeatherSocket");
	m_isConnected=false;
	ADDRESS=addr;
	PORT=port;



}
// 

SRTWeatherSocket::SRTWeatherSocket(CString addr, unsigned int port,WeatherStationData *md)
{
	ACS_TRACE("SRTWeatherSocket::SRTWeatherSocket");

	m_meteodata=md;
	m_isConnected=false;
	ADDRESS=addr;
	PORT=port;
        p= XML_ParserCreate(NULL);
         XML_SetElementHandler(p, start_hndl, end_hndl);
    	 XML_SetCharacterDataHandler(p,char_hndl);
	ACS_LOG(LM_FULL_INFO,"SRTWeatherSocket::SRTWeatherSocket()",(LM_INFO,"Creating xlm parser and handlers"));
	 m_sensorData=md;
   	 XML_SetUserData(p,md);


}



SRTWeatherSocket::~SRTWeatherSocket()
{
	
	ACS_TRACE("SRTWeatherSocket::~SRTWeatherSocket");


}
int SRTWeatherSocket::sendCMD(CError& err, CString cmd)
{
//	CError err;
	ACS_TRACE("SRTWeatherSocket::sendCMD");
    	Send(err,(const char *)cmd,cmd.GetLength());

	ACS_DEBUG_PARAM("SRTWeatherSocket::sendCMD(CError& err, CString cmd)","sent:  %s", (const char *) cmd);

	return 0;



}


int  SRTWeatherSocket::receiveData(CError& err, CString& rdata)
{

	ACS_TRACE("SRTWeatherSocket::receiveData");
	int n_received;
	bool done=false;
	char buff[MAXSIZE];
	n_received=Receive(err,buff,MAXSIZE);
	buff[n_received]=0;
    	rdata =CString(buff);  
	ACS_DEBUG_PARAM("SRTWeatherSocket::sendCMD(CError& err, CString cmd)","received:  %s", (const char *) rdata);
 	return 0;

}

CError SRTWeatherSocket::connect() throw (ACSErr::ACSbaseExImpl)

{
	OperationResult err ;
	err=Create(m_error,DGRAM);  
	if (err==FAIL)
	{
		_EXCPT(ComponentErrors::SocketErrorExImpl,ex,"SRTWeatherSocket::connect()- Create Socket");
		ex.log(LM_DEBUG);
		throw ex;
		m_isConnected=false;
		return m_error;
	} 

	err=Connect(m_error,ADDRESS,PORT);
	if (err==FAIL)
	{

		CError error; // CError object only for Closing the Socket)		
		_EXCPT(ComponentErrors::SocketErrorExImpl,ex,"SRTWeatherSocket::connect()- Connect to Socket");
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

CError SRTWeatherSocket::disconnect()throw (ACSErr::ACSbaseExImpl)
{

	 ACS_LOG(LM_FULL_INFO,"SRTWeatherSocket::Disconnect()",(LM_INFO,"  disconnecting Socked"));
	try {
		if (m_isConnected)
  		{	Close(m_error);
			if (m_error.isNoError())
			{
	
			 ACS_LOG(LM_FULL_INFO,"SRTWeatherSocket::Disconnect()",(LM_DEBUG,"Disconnecting Socket  ",(const char *) ADDRESS,PORT));
 			} else
			{
		  	ACS_LOG(LM_FULL_INFO,"SRTWeatherSocket::Disconnect()",(LM_ERROR,"%s",(const char *) m_error.getDescription()));
	     		_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"MeteoStation::disconnect()");	

			}
		}
	} catch (...)
	{
// 	cout << "unknown exception" << endl; 
		ACS_LOG(LM_FULL_INFO,"SRTWeatherSocket::Disconnect()",(LM_ERROR,"%s",(const char *) m_error.getDescription()));

	}
		
	return m_error;
	
}
int SRTWeatherSocket::parse(CString meteo_string)
{
	int i;

	int len;
	int done=true;
	len=strlen((const char*) meteo_string);
// 	cout << "LEN:" <<len <<endl;

 
 	if (! XML_Parse(p, (const char*) meteo_string, len, done)) {
       fprintf(stderr, "Parse error at line %d:\n%s\n",
 	      XML_GetCurrentLineNumber(p),
 	      XML_ErrorString(XML_GetErrorCode(p)));
	}

// 	cout << "temp:" <<i<<":"<< (const char *) meteo_string << endl;

	return 0;

}

void SRTWeatherSocket::initParser(WeatherStationData *md)
{

	 m_meteodata=md;
 	 p= XML_ParserCreate(NULL);
         XML_SetElementHandler(p, start_hndl, end_hndl);
    	 XML_SetCharacterDataHandler(p,char_hndl);
//	cout << "Creating xlm parser and handlers" << endl;
   	 XML_SetUserData(p,m_meteodata);


}

void SRTWeatherSocket::start_hndl(void* data, const XML_Char* el, const XML_Char** attr)
{
int i;
    WeatherStationData* md=(WeatherStationData*) data;
//    cout << "start handlers" << endl;

  for (i = 0; i < Depth; i++)
//     printf("  ");

//   printf("%s", el);
   md->setTag(string(el));
//   for (i = 0; attr[i]; i += 2) {
//      printf(" attributo %s='%s'", attr[i], attr[i + 1]);
//   }

  printf("\n");
  Depth++;


}
void SRTWeatherSocket::end_hndl(void *data, const XML_Char* el)
{
    WeatherStationData* md=(WeatherStationData*) data;
//    cout << "END handlers" << endl;


  md->setTag("");
  Depth--;
//   if (Depth==0)  cout << "Fine" <<endl;  
 }

void  SRTWeatherSocket::char_hndl(void *data, const XML_Char *s, int len)

{

//    / cout  << "CHAR handlers" << endl;

    WeatherStationData* md=(WeatherStationData*) data;

    string xmlvalue; // sensor name from the xml string

    char temp[512];
    strncpy(temp,s,len);
    temp[len]=0;
    xmlvalue=string(temp);  //

//      cout <<"char_hndl:"<< xmlvalue.c_str() << endl;

    if (xmlvalue=="date") {
	md->setSensorName(string(temp));
	md->setTag(xmlvalue);
       
    }
    if ((md->getSensorName()=="date") &&(md->getTag()=="Val")) {
        md->setDate(xmlvalue);
       md->setTag("");

    }
  
    for (int i=0; i < NSENSORS;i++) 
	{ checkSensor(md,xmlvalue,COMMANDS[i]);
  	}


}

void SRTWeatherSocket::checkSensor(WeatherStationData *md,string xmlvalue,const string sensorlabel)
{
    if ((md->getTag()=="Name") && (xmlvalue==sensorlabel))
    {
	md->setSensorName(xmlvalue);
	md->setTag("");
		

    }

    if ((md->getTag()=="Val") && (md->getSensorName()==sensorlabel))
    {
	double value;
	string name;
	name=md->getSensorName();
	value=atof(xmlvalue.c_str());
	//md->setSensorName(string(sSensor));
	md->setTag("");
	md->sensorMap[name]=value;
// 	cout <<name <<":" <<value<<endl;
    }


}

int SRTWeatherSocket::Parse( char* buff)
{
int len;
int done=true;
len=strlen(buff);
// cout << "LEN:" <<len <<endl;

 
 if (! XML_Parse(p, buff, len, done)) {
       fprintf(stderr, "Parse error at line %d:\n%s\n",
 	      XML_GetCurrentLineNumber(p),
 	      XML_ErrorString(XML_GetErrorCode(p)));
       return -1;

	}
   return 0;

 
}
 
