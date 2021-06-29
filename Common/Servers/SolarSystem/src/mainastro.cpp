#include "libastrowrapper.h"

int main(int argc, char** argv)

{

   SolarSystemBody* p =   new SolarSystemBody(JUPITER);
   std::cout << "New SolarSystemBody done" << std::endl;
   Site *site = new Site(59319.5,degrad(9.5),degrad(39.5),600);
   p->compute(site);
   p->report();
   
   return 0;

}
