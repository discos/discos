#ifndef _CMETEOSOCKET
#define _CMETEOSOCKET
#include <IRA>
#include "acstime.h" 
#include "WeatherStationData.h"
#include <ComponentErrors.h>
#include "CXMLDataParser.h"

using namespace IRA;




class SRTWeatherSocket : public CSocket{


public:

	SRTWeatherSocket(CString, unsigned int);
	SRTWeatherSocket(CString, unsigned int,WeatherStationData*);
	virtual ~SRTWeatherSocket();
	CError  connect() throw (ACSErr::ACSbaseExImpl);
	CError  disconnect() throw (ACSErr::ACSbaseExImpl);;
	inline bool isConnected(){return m_isConnected;};
	int parse(CString);


	int Parse(char* buff);
	WeatherStationData *m_sensorData;
	double getWind();
	double getTemperature();
	double getHumidity();
	double getPressure();
	double getWinDir();
	double getWindSpeedPeak();


private:
	struct dateval {
		CString date;
		double val;

	} dateval;

	int sendCMD(CError&,CString);
	int receiveData(CError&,CString&);
	void initParser(WeatherStationData *md);
	void releaseParser(){ 	XML_ParserFree(p);};;
	
 	CString ADDRESS;//="192.167.8.4";  // to be put in the CDB
	unsigned int PORT ;//= 5000 
	CError m_error;


	
	bool m_isConnected;
static int Depth;
	char Buff[BUFFSIZE];
	static void start_hndl(void* data, const XML_Char* el, const XML_Char** attr);
	static void end_hndl(void *data, const XML_Char* el);

 	static void char_hndl(void *data, const XML_Char *s, int len);
	static void checkSensor(WeatherStationData *md, string xmlvalue, const string  label);

	static void checkSensor(WeatherStationData *md, string xmlvalue, const string  label,double& val);

 
	XML_Parser p;
	WeatherStationData *m_meteodata;
	map<string,double> m_map;
	string m_tagname; 
	string m_tagdate;
	double m_tagvalue;
	string m_acttag; // actual tag;
	
	
	



};
#endif //_CMETEOSOCKET

