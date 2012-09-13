#ifndef _WEATHERSTATIONDATA
#define _WEATHERSTATIONDATA


#include <string>
#include <iostream>
#include <map>

using namespace std;

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
#define NSENSORS 21

enum { HEATINGTEMP, HEATINGVOLTAGE,SUPPLYVOLTAGE,RIFVOLTAGE,
	WINDDIRMIN,WINDDIRAVE,WINDDIRMAX,
	WINDSPEEDMIN,WINDSPEEDAVE,WINDSPEEDMAX,
	AIRTEMP,RELHUM,AIRPRESSURE,
	RAINACC,RAINDURATION,RAININTENSITY,RAINPEAKDURATION,
	HAILACC,HAILDURATION,HAILINTENSITY,HAILPEAK};


static const char *COMMANDS[NSENSORS]={ 

 "th01",
 "vh01",
 "vs01",
 "vr01",
 "dn01", 
 "dm01", 
 "dx01", 
 "sn01", 
 "sm01", 
 "sx01", 
 "ta01", 
 "ua01", 
 "pa01", 
 "rc01", 
 "rd01", 
 "ri01", 
 "rp01", 
 "hc01", 
"hd01", 
"hi01", 
"hp01"};  

     
class WeatherStationData
{    
     
public:	
	inline	void setTag(string tag){m_acttag=tag;};
	inline	void setVal(double val){m_actval=val;};
	inline	void setSensorName(string tag){m_sensorname=tag;};
	inline	void setSensorValue(double val){m_sensorvalue=val;};
     
     
	inline	void setDate(string date){m_date=date;};
	inline	string getTag(){ return m_acttag ;};
	inline	double getVal(){return m_actval;};
	inline	string getDate(){return m_date;};
	inline	string getSensorName(){ return m_sensorname ;};
	inline	double getSensorValue(){return m_sensorvalue;}
	inline map<string,double> getSensorMap() {return sensorMap;}
	map<string,double> sensorMap;
	map<string,string> sensorDate;


	
		
private:
	string m_acttag;
	double m_actval;
	string  m_date;
	string m_sensorname;
	double m_sensorvalue;
};

#endif
