#ifndef _CMETEOSOCKET
#define _CMETEOSOCKET
#include <IRA>
#include "acstime.h" 
#include "MeteoData.h"
#include <ComponentErrors.h>
#include "CXMLDataParser.h"

using namespace IRA;




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


	int parse(const char* buff);
	MeteoData *m_sensorData;
	
private:
	
 	CString ADDRESS;//="192.167.8.4";  // to be put in the CDB
	unsigned int PORT ;//= 5000 
	CError m_error;
	
	bool m_isConnected;
static int Depth;
	char Buff[BUFFSIZE];
/*	static void start_hndl(void* data, const XML_Char* el, const XML_Char** attr);
	static void end_hndl(void *data, const XML_Char* el);

 	static void char_hndl(void *data, const XML_Char *s, int len);
	static void checkSensor(MeteoData *md, string xmlvalue, const string  label);
 
	XML_Parser p;*/
	MeteoData *m_meteodata;
	map<string,double> m_map;
/*	string m_tagname; 
	string m_tagdate;
	double m_tagvalue;
	string m_acttag; // actual tag;*/
	
	
	



};
#endif //_CMETEOSOCKET

