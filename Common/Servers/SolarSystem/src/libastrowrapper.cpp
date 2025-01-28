#include "libastrowrapper.h"

namespace xephemlib {
	


const char  *planetnames[]={"Mercury","Venus","Mars","Jupiter","Saturn",
                            "Uranus","Neptune","Pluto","Sun","Moon","NOBJ"};
                            



Site::Site(double mj, double lon, double lt, double height)
{
    this->n_mjd=mj-15019.5; // xephem uses mjd 0 as 1899 12 31 at 12:00
    this->n_lat=lt;
    this->n_lng=lon;
    this->n_epoch=J2000;
   // this->n_height=height;
    
}

Site::Site()

{
	    this->n_epoch=J2000;
}

void Site::setCoordinate(double lon, double lt,double height)
{

    this->n_lat=lt;
    this->n_lng=lon;
  //  this->n_heigt=height;

}

void Site::setTime(double mj)
{

    this->n_mjd=mj-15019.5; // xephem uses mjd 0 as 1899 12 31 at 12:00



}
	



void Site::stampa(void){

    std::cout<< "mjd:"<< this->n_mjd <<std::endl;
    std::cout << "lon:"<< raddeg(this->n_lng) << std::endl;
    std::cout << "lat:"<< raddeg(this->n_lat) << std::endl;
    
    
}

double Site::getMjd()
{

    return this->n_mjd+15019.5;
    
}

SolarSystemBody::SolarSystemBody()
{
   obj=new Obj();
   

};

void SolarSystemBody::setObject(PLCode code){
    
        
    
//    obj=new Obj();   
      _code=code;
       
    
  //  obj->any.co_type=PLANET;
    strncpy(obj->any.co_name,planetnames[_code],10);
//    obj->
 //   pl.plo_code=code;
#ifdef DEBUG    
//    std::cout << "Code: " << code << std::endl;
//    std::cout << "Name: " << planetnames[code] << std::endl;
 //   std::cout << "Name: " << obj->any.co_name << std::endl;
#endif
    
//    pref_set(PREF_EQUATORIAL,PREF_TOPO);

  //      std::cout << "exit after pref_set: " << obj->any.co_name << std::endl;

};
  
SolarSystemBody::SolarSystemBody(PLCode code)
{
    //obj=new Obj();

    _code=code;
    
//    obj->any.co_type=PLANET;
  //  strncpy(obj->any.co_name,planetnames[_code],10);
  //  obj->pl.plo_code=code;
#ifdef DEBUG    
//    std::cout << "Code: " << code << std::endl;
//    std::cout << "Name: " << planetnames[code] << std::endl;
//    std::cout << "Name: " << obj->any.co_name << std::endl;
#endif
    
  //  pref_set(PREF_EQUATORIAL,PREF_TOPO);

    
    
}

PLCode  SolarSystemBody::getPlanetCodeFromName(std::string  str)
{
     std::locale loc;
    for (std::string::size_type i=0; i<str.length(); ++i){
           str[i]=std::toupper(str[i],loc);
     }
      if (planet.find(str) != planet.end()) 
      { 
      //     std::cout <<str << " is in the map." << std::endl;
           return SolarSystemBody::planet[str]; 
      } 
      else 
      { 
      
      //std::cout << str << " is not in the map." << std::endl;
            return NOBJ;
      }

      

     
     
		
		
}
	



std::string  SolarSystemBody::getPlanetNameFromCode(PLCode code ){
	



    std::string name;
    for (std::map<std::string,PLCode>::iterator it=SolarSystemBody::planet.begin(); it!=SolarSystemBody::planet.end(); ++it)
    {       
#ifdef DEBUG        
         //   std::cout << it->first << " => " << it->second << '\n';
#endif
            if (it->second == code)
            name=it->first;
            
    }


  
    return name;
    
    


}


void SolarSystemBody::getCoordinates(double& ra, double& dec,double& az,double& el, double& range)
{
   ra=_ra;
   dec=_dec;
   az=_az;
   el=_el;
   range=_range;


};


void  SolarSystemBody::compute(Site* site){
    
    Obj* obj=new Obj();
    
    
    
    obj->any.co_type=PLANET;
    strncpy(obj->any.co_name,planetnames[_code],10);
    obj->
    pl.plo_code=_code;
#ifdef DEBUG    
//    std::cout << "Code: " << _code << std::endl;
//    std::cout << "Name: " << planetnames[_code] << std::endl;
//    std::cout << "Name: " << obj->any.co_name << std::endl;
#endif    
     pref_set(PREF_EQUATORIAL,PREF_TOPO);

    
//    std::cout << "Compute " <<std::endl;
    obj_cir (site, obj);

    _ra=obj->any.co_ra;
    _dec=obj->any.co_dec;
    _az=obj->any.co_az;
    _el=obj->any.co_alt;
    _range=obj->anyss.so_edist;
    delete obj;
    
}


Obj* SolarSystemBody::getObject()
{
    return  obj;
    
    
}

void SolarSystemBody::report()
{
/* Astrometric right ascension and declination of the target center with
respect to the observing site (coordinate origin) in the reference frame of
the planetary ephemeris (ICRF). Compensated for down-leg light-time delay
aberration.*/



    std::cout << "Ra : " << _ra << std::endl;
    std::cout << "Dec: " << _dec << std::endl;
    std::cout << "Az : " << _az << std::endl;
    std::cout << "El : " << _el << std::endl;
    std::cout << "Range : " << _range << std::endl;

    
    
    
}

std::map<std::string,PLCode>SolarSystemBody::planet;  //definition of static variable


 bool  createMap() // function to initialize static map 
{        /*
typedef enum {
    MERCURY,
    VENUS,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE,
    PLUTO,
    SUN,
    MOON,
    NOBJ	// total number of basic objects 
} PLCode;
*/
   
         SolarSystemBody::planet["MERCURY"]=MERCURY;
         SolarSystemBody::planet["VENUS"]=VENUS ; 
         SolarSystemBody::planet["MARS"]=MARS ;          
         SolarSystemBody::planet["JUPITER"]=JUPITER;
         SolarSystemBody::planet["SATURN"]=SATURN ; 
         SolarSystemBody::planet["URANUS"]=URANUS ; 
         SolarSystemBody::planet["NEPTUNE"]=NEPTUNE ;        
         SolarSystemBody::planet["PLUTO"]=PLUTO ;   
         SolarSystemBody::planet["SUN"]=SUN ;                          
         SolarSystemBody::planet["MOON"]=MOON ;   
         SolarSystemBody::planet["NOBJ"]=NOBJ ;                                            
	      return 0;
}


static bool _tmp=createMap();  // call to the function

}

