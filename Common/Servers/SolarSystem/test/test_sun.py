import unittest
# Import command-line arguments
from sys import argv

# Import the SimpleClient class
from Acspy.Clients.SimpleClient import PySimpleClient

import Antenna
from math import radians,degrees

import Acspy.Common.TimeHelper
import time



class SunPositionTest(unittest.TestCase):
    def setUp(self):
         epoch = Acspy.Common.TimeHelper.TimeUtil()
         struct_time = time.strptime("2022-Jan-27 13:30", "%Y-%b-%d %H:%M")
         tm=time.mktime(struct_time)
         self.acstime=epoch.py2epoch(tm)
         print(tm)
         self.simpleClient = PySimpleClient()
         self.hwRef = self.simpleClient.getDynamicComponent(None,"IDL:alma/Antenna/SolarSystemBody:1.0","SolarSystemBodyImpl",None)
         self.hwRef.setBodyName('Sun')



    def test_equatorialJ2000(self):
         ra,dec=self.hwRef.getJ2000EquatorialCoordinate(self.acstime.value)
         print(ra,dec)	 
             
    
         self.assertAlmostEqual(degrees(ra), 309.681353589,  delta=1e-4)
         self.assertAlmostEqual(degrees(dec),   -18.451737235,  delta=1e-4)

    def test_equatorialApparent(self):
        for i in range(10):	
            az,el,ra,dec,l,lon,lat =self.hwRef.getAllCoordinates(self.acstime.value)
#          print('type az',type(az))
            self.assertAlmostEqual(degrees(ra),309.986231168 , delta=1e-4)   	 
            self.assertAlmostEqual(degrees(dec),-18.376456126 , delta=1e-4)  
#    def test_horizontalApparent(self):
#         az,el,ra,dec,l,lon,lat =self.hwRef.getAllCoordinates(self.acstime.value)
            print (f'{i}:az,el=i{degrees(az)},{degrees(el)}') 
            self.assertAlmostEqual(degrees(az),   210.403368369   , delta=5e-4)   	 
            self.assertAlmostEqual(degrees(el),  26.276295555  , delta=5e-4)  
    
    def test_equatorialApparent(self):
        for i in range(10):	
            az,el,ra,dec,l,lon,lat =self.hwRef.getAllCoordinates(self.acstime.value)
#          print('type az',type(az))
            self.assertAlmostEqual(degrees(ra),309.986231168 , delta=1e-4)   	 
            self.assertAlmostEqual(degrees(dec),-18.376456126 , delta=1e-4)  
#    def test_horizontalApparent(self):
#         az,el,ra,dec,l,lon,lat =self.hwRef.getAllCoordinates(self.acstime.value)
            print (f'{i}:az,el=i{degrees(az)},{degrees(el)}') 
            self.assertAlmostEqual(degrees(az),   210.403368369   , delta=5e-4)   	 
            self.assertAlmostEqual(degrees(el),  26.276295555  , delta=5e-4)  
       	 
    def tearDown(self):
    	   print('closing')
    	   self.simpleClient.disconnect()
if __name__ == '__main__':
    
        unittest.main() 
 
