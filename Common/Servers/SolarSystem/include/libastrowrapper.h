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




class Site: public  Now {

public:  
      
    Site(double mj, double lon, double lt, double height);
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

      void compute (Site* site);
      SolarSystemBody() {};
      Obj* getObject();
      void report();
      
      private:
      
    
    
protected:
        
            Obj* obj;
    
    
    
};

class Jupiter : public SolarSystemBody {

public:
    
     Jupiter() ;
    
    
    
};






#endif   /*_LIBRWAPPER*/
