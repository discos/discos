/*

    C++ Wrapper for Xephem libastro. 
    
    

*/




//#ifndef _LIBRWAPPER
//#define  _LIBRWAPPER

#include <iostream>
#include <cstring>
#include <string>
#include "astro.h"
#include "preferences.h"
#include <string>
#include <map>
#include <cctype>
#include <algorithm>
#include <locale>
#include <memory>


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
      
      
      SolarSystemBody(PLCode code)  ;
      ~SolarSystemBody(){
		std::cout << "destroyed"<<"\n";      
      
      //
      };
    
      static std::string getPlanetNameFromCode(PLCode code);
      static PLCode  getPlanetCodeFromName(std::string  name);
      void compute (Site* site);
      SolarSystemBody();
      void setObject(PLCode code);
       
 
      Obj* getObject();
      void report();
      static std::map<std::string,PLCode> planet;
      void getCoordinates(double& ra, double& dec,double& az,double& el, double& range);      
      private:
            
            double _ra,_dec,_range,_az,_el;
            PLCode _code;
            
    
    
protected:
        
            std::unique_ptr<Obj> obj; // Replaces Obj* obj
 
    
    
    
};

class Jupiter : public SolarSystemBody {

public:
    
     Jupiter() ;
     
private:
   
    
    
    
};

}

 




  



//#endif   /*_LIBRWAPPER*/



































