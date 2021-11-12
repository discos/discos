#include "libastrowrapper.h"
#include <gtest/gtest.h>
#include <iostream>

class SolarSystemBodyTest : public ::testing::Test {
  

  protected:
  
 
      Site* site;
     
     SolarSystemBodyTest()
  {
  
  }   

  virtual void SetUp() {
      
    site = new Site(59393.5,degrad(9.5),degrad(39.5),600);

  }

  virtual void TearDown() {
  }
};



   

// TEST_F(EphemTest,versionTest){
//     std::string toolkit_version;
//      EXPECT_STREQ("CSPICE_N0065",solsys.spiceToolkitVersion().c_str());
//  
// }



TEST_F(SolarSystemBodyTest,JupiterPosition){
    
//      Site *site = new Site(59393.5,degrad(9.5),degrad(39.5),700);
     
     SolarSystemBody* p =   new SolarSystemBody(JUPITER);
     p->compute(this->site);
      
     EXPECT_NEAR( 334.167106906 ,raddeg(p->ra),2.7e-4);
     EXPECT_NEAR( -11.764766054 ,raddeg(p->dec),2.7e-4);
     EXPECT_NEAR( 4.39448666972074  ,p->range,2.7e-5);

   
      
     
}

TEST_F(SolarSystemBodyTest,Sun){
    
     Site *site = new Site();
     site -> setCoordinate(degrad(9.5),degrad(39.5),700);
     site -> setTime(59393.5)
     

     SolarSystemBody* p =   new SolarSystemBody(SUN);
     p->compute(site);
      
     EXPECT_NEAR( 96.800863628 ,raddeg(p->ra),2.7e-4);
     EXPECT_NEAR( 23.287056282 ,raddeg(p->dec),2.7e-4);
     EXPECT_NEAR( 1.01658809887528  ,p->range,2.7e-6);

      
        
     
}



// Google Test can be run manually from the main() function
// or, it can be linked to the gtest_main library for an already
// set-up main() function primed to accept Google Test test cases.
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

// Build command: g++ main.cpp -lgtest
