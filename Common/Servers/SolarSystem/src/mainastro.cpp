#include "libastrowrapper.h"
#include <iostream>
int main(int argc, char** argv)

{

   xephemlib::SolarSystemBody* p =   new xephemlib::SolarSystemBody(JUPITER);
   std::cout << "New SolarSystemBody done" << std::endl;
   xephemlib::Site *site = new xephemlib::Site();
   site -> setCoordinate(degrad(9.5),degrad(39.5),600);
   site -> setTime(59319.5) ;  
//   Site *site = new Site(59319.5,degrad(9.5),degrad(39.5),600);


   p->compute(site);
   p->report();
   std::cout <<xephemlib::SolarSystemBody::getPlanetNameFromCode(JUPITER)<<std::endl;
   std::cout << xephemlib::SolarSystemBody::getPlanetCodeFromName("saTurn")<<std::endl;
   return 0;

}
