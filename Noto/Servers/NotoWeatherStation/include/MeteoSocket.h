#ifndef _CMETEOSOCKET
#define _CMETEOSOCKET
#include <IRA>
#include "acstime.h" 
#include "MeteoData.h"
#include <ComponentErrors.h>
#include "CXMLDataParser.h"

using namespace IRA;


const static char WEATHERCMD[9] ="LOOP 1\n";


class MeteoSocket : public CSocket{


public:

	MeteoSocket(CString, unsigned int);
	MeteoSocket(CString, unsigned int,MeteoData*);
	virtual ~MeteoSocket();
	int sendCMD(CError&,CString);
	int receiveData(CError&,CString&);
        void connection() throw (ACSErr::ACSbaseExImpl);
        CError  init() ;
        
	void disconnection() throw (ACSErr::ACSbaseExImpl);;
	inline bool isConnected(){return m_isConnected;};
	void initParser(MeteoData *md);
// 	void releaseParser(){ 	XML_ParserFree(p);};;

     double getWindSpeed();  // Get Wind Speed
     double getWindDir();    // Get Wind Direction (disabled)
     double getTemperature();
     double getHumidity();
     double getPressure();
     double getWindspeedPeak();
     int updateParam();
     

private:
	
 	CString ADDRESS;//="192.167.8.4";  // to be put in the CDB
	unsigned int PORT ;//= 5000 
	CError m_error;
	
	bool m_isConnected;
static int Depth;
	char Buff[BUFFSIZE];
        char *m_received_data;
	MeteoData *m_meteodata;
	map<string,double> m_map;
	vector<string> split (string message, string delimiter=",");
	double m_windspeed;
	double m_winddir;
	double m_temperature;
	double m_pressure;
	double m_humidity;
        double m_windspeedPeak;
        int parse( const char* buff);   // weather data from station
        int fs_parse(const char* buff); // weather data from the field system

//	MeteoData *m_sensorData;

	
	



};
#endif //_CMETEOSOCKET

