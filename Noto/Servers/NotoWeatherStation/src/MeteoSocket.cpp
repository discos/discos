#include  "MeteoSocket.h"
#define MAXSIZE 255
int MeteoSocket::Depth=0;

// #define SIMULATOR




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
	int n_sent;
        ACS_TRACE("MeteoSocket::sendCMD");
    	n_sent=Send(err,(const char *)cmd,cmd.GetLength());
        cout <<"Manda manda:" << (const char *)cmd << endl;
	ACS_DEBUG_PARAM("MeteoSocket::sendCMD(CError& err, CString cmd)","sent:  %s", (const char *) cmd);

	return n_sent;

}

int MeteoSocket:: updateParam(){
//
//
            
            
      try{
            
            int n_sent=0;
	    CError err;
	    CString rdata="";
/*            connect();*/
            n_sent=Send(err,WEATHERCMD,CString(WEATHERCMD).GetLength());
            
            cout <<"Manda manda:" << WEATHERCMD << endl;
            cout << "sent:"<<n_sent << endl;
             
            ACS_DEBUG_PARAM("MeteoSocket::updateParam(CError& err, CString cmd)","sent:  %s", (const char *) WEATHERCMD);

/* 		receiveData(err,rdata);*/
        int n_received,n_received_total=0;
        char buff[MAXSIZE];
        char receivedChar=0;
        int i=0;
  
        while (n_received_total !=100)
 
        {
        
        
                n_received=Receive(err,&receivedChar,1);
        //      receivedChar=buff[0];
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

          


                cout << "hex:";
                for (int j=0; j<100;j++)  cout<<hex <<  (int)buff[j]<<" ";
                cout << endl;


		ACS_DEBUG_PARAM("MeteoSocket::updateParam(CError& err, CString cmd)","received:  %s", (const char *) rdata)     
                
//             sendCMD(err,CString("noresp\n"));
	                
 		parse(buff);
                IRA::CIRATools::Wait(0,50000);

 		if (err.isNoError()) return 0;
 		else
                { 
                    cout <<"err:"<< err.getFullDescription()<<endl;

                    return -1;
                }
            
/*            disconnect();*/
            }catch (ComponentErrors::SocketErrorExImpl &x)
        
        {
                 ACS_LOG(LM_FULL_INFO,"MeteoSocket:: updateParam",
                       (LM_ERROR,"Can not connect  to WeatherStation @%s:%d  ",
                       (const char *) ADDRESS,PORT));

                _THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"MeteoSocket:: updateParam");
 
        }           
}



int  MeteoSocket::receiveData(CError& err, CString& rdata)
{

 
	rdata="";
	
	ACS_TRACE("MeteoSocket::receiveData");
	int n_received,n_received_total=0;

	 char buff[MAXSIZE];
	char receivedChar=0;
	int i=0;

 
 
 
	while (n_received_total !=100)
 
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
        m_received_data=buff;
        
	rdata=CString(buff);
                 cout << "data"<<(const char *) rdata << endl;
	ACS_DEBUG_PARAM("MeteoSocket::receiveData(CError& err, CString cmd)","received:  %s", (const char *) rdata);
	return n_received_total;
	
		
			
		
//		n_received=Receive(err,buff,MAXSIZE);
//		buff[n_received]=0;
//    	rdata =CString(buff);  bg
//		ACS_DEBUG_PARAM("MeteoSocket::sendCMD(CError& err, CString cmd)","received:  %s", (const char *) rdata);
 	


}

CError MeteoSocket::init() 
{
                  
          
}
          
          
          
          
CError MeteoSocket::connection() throw (ACSErr::ACSbaseExImpl)

{
       OperationResult err ;
        
	
     try{     
        err=Create(m_error,STREAM);  
        if (err==FAIL)
          {
                m_isConnected=false;
           } 
        setSockMode(m_error,BLOCKING); 
	
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
        } catch(...)
       {
        cout <<"Connect Error" << endl;
        return m_error;
       
       }
       

}

CError MeteoSocket::disconnection()throw (ACSErr::ACSbaseExImpl)
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
 	cout << "Disconnect: unknown exception" << endl; 
		ACS_LOG(LM_FULL_INFO,"MeteoSocket::Disconnect()",(LM_ERROR,"%s",(const char *) m_error.getDescription()));

	} 
		
	return m_error;
	
}

double MeteoSocket::getWindSpeed()
{
//	if((updateParam()==-1)) cout <<"Error Reading Param"<< endl;
	updateParam();

//	m_windspeed=-99; // windspeed disabled
	ACS_LOG(LM_FULL_INFO,"MeteoSocket::getWindSpeed()",(LM_TRACE,"Not yet implemented"));


	//to be impemented
	return m_windspeed;

}
double MeteoSocket::getWindDir()
{
//	if((updateParam()==-1)) cout <<"Error Reading Param"<< endl;
	updateParam();
	ACS_LOG(LM_FULL_INFO,"MeteoSocket::getWindDir()",(LM_TRACE,"Not yet implemented"));

#ifdef SIMULATOR

	m_winddir=-99;

#endif


	return m_winddir;

}
double MeteoSocket::getTemperature()
	{

		if((updateParam()==-1)){
			ACS_LOG(LM_FULL_INFO,"MeteoSocket::getTemperature()",(LM_ERROR,"Reading Temperature"));
		}
#ifdef SIMULATOR

	m_temperature=20;

#endif

		return m_temperature;

	}
double MeteoSocket::getHumidity()	{
		//to be impemented
	if((updateParam()==-1)){
		ACS_LOG(LM_FULL_INFO,"MeteoSocket::getHumidity()",(LM_ERROR,"Reading Humidity"));
	}

#ifdef SIMULATOR

	m_humidity=70;

#endif
	return m_humidity;
	}
double MeteoSocket::getPressure()	{
		//to be impemented
	if((updateParam()==-1)){
		ACS_LOG(LM_FULL_INFO,"MeteoSocket::getPressure()",(LM_ERROR,"Reading Pressure"));
	}
#ifdef SIMULATOR

	m_pressure=1010;

#endif
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

vector<string> MeteoSocket::split (string message, string delimiter)
{
  /**
   Do as the split string method as default the delimiter is comma         
          
          
  */
          
          
   vector<string> v;

   size_t pos = 0;
   std::string token;
   while ((pos = message.find(delimiter)) != std::string::npos) {
       token = message.substr(0, pos);
       std::cout << token << std::endl;
       v.push_back(token);
       message.erase(0, pos + delimiter.length());
   }
   v.push_back(message);
   return v;
 
}
          



int MeteoSocket::fs_parse(const char* recv)
{
	int len;
        string recvS;
        vector<string> vrecv;
     
	len=strlen(recv);
        recvS=recv;
        
//  	cout <<"received"<< len << endl;
        vrecv=split(recvS,"/");
        vrecv=split(vrecv[2],",");
     
	int ndata=vrecv.size();
	if (ndata > 3)
	
	{
	 	double temp,pres,hum,wind,winddir;
		temp = atof(vrecv[0].c_str());
		pres = atof(vrecv[1].c_str());
		hum  = atof(vrecv[2].c_str());
                wind  = atof(vrecv[3].c_str());
                winddir =atof(vrecv[4].c_str());
		m_temperature=temp;
		m_pressure=pres;
		m_humidity=hum;
		m_windspeed=wind;
                m_winddir=winddir;
                
		


		 ACS_LOG(LM_FULL_INFO,"MeteoSocket::parse()",(LM_DEBUG," Meteoparms  %f %f %f",temp,pres,hum ));
		  
	} else
	{
		 ACS_LOG(LM_FULL_INFO,"MeteoSocket::parse()",(LM_ERROR,"Not enough data from meteo server"));
		 return -1;

	}

	return 0;

 
}
 int MeteoSocket::parse(const  char* recv)
{
        int len;
        float temp,pres,hum;
        int temperature=0;
        long pressure=0;
        int humidity=0;
        int wind_dir=0;
        int wind_speed=0;

                temperature=recv[14];
                temperature=temperature<<8;
                temperature=temperature|(recv[13]&0xff);
                humidity=recv[34];
                pressure=recv[9];
                pressure=pressure<<8;
                pressure=pressure|(recv[8]&0xff);
                wind_speed=recv[15];
                wind_dir=recv[18];
                wind_dir=wind_dir<<8;
                wind_dir=wind_dir|(recv[17]&0xff);
                 
                temp=(float)(temperature-320)*5./9./10.;// tenth of fahrenight degree to celsius 
                hum=(float)humidity;
                pres= (float) pressure/1000*33.8639 ;
                 
                m_temperature=temp;
                m_pressure=pres;
                m_humidity=hum;
                m_windspeed=wind_speed;
                m_winddir=wind_dir;
                m_windspeedPeak=wind_speed;
                 ACS_LOG(LM_FULL_INFO,"MeteoSocket::parse()",(LM_DEBUG," Meteoparms  %f %f %f",temp,pres,hum ));
                  
                cout << temp << ","<<pres<<","<<hum<<endl;

        return 0;

 
}


