#include "libastrowrapper.h"

namespace xephemlib {
	


const char  *planetnames[]={"Mercury","Venus","Mars","Jupiter","Saturn",
                            "Uranus","Neptune","Pluto","Sun","Moon","NOBJ"};
                            



Site::Site(double mj, double lon, double lt, double height)
{
    this->n_mjd=mj-15020; // xephem uses mjd 0 as 1899 12 31 at 12:00
    this->n_lat=lt;
    this->n_lng=lon;
    this->n_epoch=J2000;
   // this->n_height=height;
    
}

Site::Site()

{
	
}

void Site::setCoordinate(double lon, double lt,double height)
{

    this->n_lat=lt;
    this->n_lng=lon;
  //  this->n_heigt=height;

}

void Site::setTime(double mj)
{

    this->n_mjd=mj-15020; // xephem uses mjd 0 as 1899 12 31 at 12:00



}
	



void Site::stampa(void){

    std::cout<< "mjd:"<< this->n_mjd <<std::endl;
    std::cout << "lon:"<< raddeg(this->n_lng) << std::endl;
    std::cout << "lat:"<< raddeg(this->n_lat) << std::endl;
    
    
}

double Site::getMjd()
{

    return this->n_mjd+15020;
    
}

SolarSystemBody::SolarSystemBody(PLCode code)
{
    obj=new Obj();

    
    obj->any.co_type=PLANET;
    strncpy(obj->any.co_name,planetnames[code],10);
    obj->
    pl.plo_code=code;
#ifdef DEBUG    
    std::cout << "Code: " << code << std::endl;
    std::cout << "Name: " << planetnames[code] << std::endl;
    std::cout << "Name: " << obj->any.co_name << std::endl;
#endif
    
    pref_set(PREF_EQUATORIAL,PREF_TOPO);

    
    
}

PLCode  SolarSystemBody::getPlanetCodeFromName(std::string  str)
{
     std::locale loc;
    for (std::string::size_type i=0; i<str.length(); ++i)
           str[i]=std::toupper(str[i],loc);
 

     return SolarSystemBody::plan[str];
     
		
		
}
	



std::string  SolarSystemBody::getPlanetNameFromCode(PLCode code ){
	




    std::string name;
    for (std::map<std::string,PLCode>::iterator it=SolarSystemBody::plan.begin(); it!=SolarSystemBody::plan.end(); ++it)
    {       
#ifdef DEBUG        
         //   std::cout << it->first << " => " << it->second << '\n';
#endif
            if (it->second == code)
            name=it->first;
            
    }


  
    return name;
    
    


}



void  SolarSystemBody::compute(Site* site){
        std::cout << "Compute " <<std::endl;
    obj_cir (site, obj);

    ra=obj->any.co_ra;
    dec=obj->any.co_dec;
    az=obj->any.co_az;
    el=obj->any.co_alt;
    range=obj->anyss.so_edist;
    
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



    std::cout << "Ra : " << ra << std::endl;
    std::cout << "Dec: " << dec << std::endl;
    std::cout << "Az : " << az << std::endl;
    std::cout << "El : " << el << std::endl;
    std::cout << "Range : " << range << std::endl;

    
    
    
}

std::map<std::string,PLCode>SolarSystemBody::plan;  //definition of static variable


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
   
         SolarSystemBody::plan["MERCURY"]=MERCURY;
         SolarSystemBody::plan["VENUS"]=VENUS ; 
         SolarSystemBody::plan["MARS"]=MARS ;          
         SolarSystemBody::plan["JUPITER"]=JUPITER;
         SolarSystemBody::plan["SATURN"]=SATURN ; 
         SolarSystemBody::plan["URANUS"]=URANUS ; 
         SolarSystemBody::plan["NEPTUNE"]=NEPTUNE ;        
         SolarSystemBody::plan["PLUTO"]=PLUTO ;   
         SolarSystemBody::plan["SUN"]=SUN ;                          
         SolarSystemBody::plan["MOON"]=MOON ;   
         SolarSystemBody::plan["NOBJ"]=NOBJ ;                                            
	      return 0;
}


static bool _tmp=createMap();  // call to the function

}

