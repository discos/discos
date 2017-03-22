#ifndef _CXMLDPARSER
#define _CXMLDPARSER

#include <string>
#include <expat.h>
#include <string.h>
#include <iostream>
#include <map>
#include "MeteoData.h"

#define HEATINGTEMP th01
#define HEATINGVOLTAGE th01
#define SUPPLYVOLTAGE th01


/*
th01 heating temp
vh01 heating voltage
vs01 supply voltage
vr01 rif. voltage
dn01 wind dir min
dm01 wind dir ave
dx01 wind dir max
sn01 wind speed ave
sm01 wind speed max
sx01 wind speed max
ta01 air temp
ua01 rel. humidity
pa01 air pressure
rc01 rain acc.
rd01 rain duration
ri01 rain intensity
rp01 rain peak duration
hc01 hail acc.
hd01 hail duration
hi01 hail intensity
hp01 hail peak duration
*/


using namespace std;
 



class CXMLDataParser {

//<String><Name>date</Name><Val>20100305143937</Val></String><DBL><Name>ta01</Name><Val>22.600000</Val><Name>dm01</Name><Val>180.0</Val></DBL>



public:

	CXMLDataParser();
	CXMLDataParser(MeteoData *md);
	virtual ~CXMLDataParser();
	int Parse(char* buff);
	int Parse(std::string buff);
	
	

protected:
#define BUFFSIZE	8192
	static int Depth;
	char Buff[BUFFSIZE];
	static void start_hndl(void* data, const XML_Char* el, const XML_Char** attr);
	static void end_hndl(void *data, const XML_Char* el);

 	static void char_hndl(void *data, const XML_Char *s, int len);
//	static vodi addSensor(MeteoData *md,string
	static void checkSensor(MeteoData *md, string xmlvalue, const string  label);
 
	XML_Parser p;
	MeteoData *m_meteodata;
	map<string,double> m_map;
	string m_tagname; 
	string m_tagdate;
	double m_tagvalue;
	string m_acttag; // actual tag;
	MeteoData *m_sensorData;




};










#endif
