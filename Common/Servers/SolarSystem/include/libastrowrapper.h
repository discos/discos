/*

    C++ Wrapper for Xephem libastro. 
    
    

*/




#ifndef _LIBRWAPPER
#define  _LIBRWAPPER

#include <iostream>
#include <cstring>
#include "astro.h"
#include "preferences.h"
#include <string>
#include <map>
#include <cctype>
#include <algorithm>
#include <locale>

namespace xephemlib
{


class Site: public  Now {

public:  
      
    Site(double mj, double lon, double lt, double height);
    Site();

    void setCoordinate(double lon, double lt, double height);
    void setTime(double mj);
    
    void stampa(void);
    double getMjd();
    
    
private:
       double m_lon,m_lat,m_height;
       std::string name;
    
};

class SolarSystemBody   {

public:
      
      
     
      SolarSystemBody(PLCode code);
      ~SolarSystemBody(){ delete obj;}
      double ra,dec,range,az,el;
      static std::string getPlanetNameFromCode(PLCode code);
      static PLCode  getPlanetCodeFromName(std::string  name);
      void compute (Site* site);
      SolarSystemBody() {};
      Obj* getObject();
      void report();
      static std::map<std::string,PLCode> plan;
      
      private:
      
    
    
protected:
        
            Obj* obj;
    
    
    
};

class Jupiter : public SolarSystemBody {

public:
    
     Jupiter() ;
    
    
    
};

}

 




  



#endif   /*_LIBRWAPPER*/
