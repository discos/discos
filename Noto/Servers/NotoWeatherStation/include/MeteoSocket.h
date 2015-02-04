#ifndef _CMETEOSOCKET
#define _CMETEOSOCKET
#include <IRA>
#include "acstime.h" 
#include "MeteoData.h"
#include <ComponentErrors.h>
#include "CXMLDataParser.h"

using namespace IRA;


const static char WEATHERCMD[10] ="LOOP\n";


class MeteoSocket : public CSocket{


public:

	MeteoSocket(CString, unsigned int);
	MeteoSocket(CString, unsigned int,MeteoData*);
	virtual ~MeteoSocket();
	int sendCMD(CError&,CString);
	int receiveData(CError&,CString&);
	CError  connect() throw (ACSErr::ACSbaseExImpl);
	CError  disconnect() throw (ACSErr::ACSbaseExImpl);;
	inline bool isConnected(){return m_isConnected;};
	void initParser(MeteoData *md);
// 	void releaseParser(){ 	XML_ParserFree(p);};;

     double getWindSpeed();  // Get Wind Speed
     double getWindDir();    // Get Wind Direction (disabled)
     double getTemperature();
	 double getHumidity();
     double getPressure();

private:
	
 	CString ADDRESS;//="192.167.8.4";  // to be put in the CDB
	unsigned int PORT ;//= 5000 
	CError m_error;
	
	bool m_isConnected;
static int Depth;
	char Buff[BUFFSIZE];
	MeteoData *m_meteodata;
	map<string,double> m_map;
	vector<string> split (string message, string delimiter=",");
	double m_windspeed;
	double m_winddir;
	double m_temperature;
	double m_pressure;
	double m_humidity;
        int parse(const char* buff);   // weather data from station
        int fs_parse(const char* buff); // weather data from the field system

//	MeteoData *m_sensorData;
	 int updateParam();

	
	



};
#endif //_CMETEOSOCKET

