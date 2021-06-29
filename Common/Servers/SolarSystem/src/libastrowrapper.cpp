#include "libastrowrapper.h"




const char  *planetnames[]={"Mercury","Venus","Mars","Jupiter","Saturn",
                            "Uranus","Neptune","Pluto","Sun","Moon","NOBJ"};
                            

Site::Site(double mj, double lon, double lt, double height)
{
    this->n_mjd=mj-15020; // xephem uses mjd 0 as 1899 12 31 at 12:00
    this->n_lat=lt;
    this->n_lng=lon;
    this->n_epoch=J2000;
//     this->n_heigt=height;
    
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

    std::cout << "Ra : " << ra << std::endl;
    std::cout << "Dec: " << dec << std::endl;
    std::cout << "Az : " << az << std::endl;
    std::cout << "El : " << el << std::endl;
    std::cout << "Range : " << range << std::endl;

    
    
    
}
