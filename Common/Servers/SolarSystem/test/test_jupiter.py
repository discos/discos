import unittest
# Import command-line arguments
from sys import argv

# Import the SimpleClient class
from Acspy.Clients.SimpleClient import PySimpleClient

import Antenna
from math import radians,degrees

import Acspy.Common.TimeHelper
import time


class JupiterPositionTest(unittest.TestCase):
    def setUp(self):
         epoch = Acspy.Common.TimeHelper.TimeUtil()
         struct_time = time.strptime("2021-Apr-15 00:00", "%Y-%b-%d %H:%M")
         tm=time.mktime(struct_time)
         self.acstime=epoch.py2epoch(tm)
         print(tm)
         simpleClient = PySimpleClient()
         self.hwRef = simpleClient.getDynamicComponent(None,"IDL:alma/Antenna/SolarSystemBody:1.0","SolarSystemBodyImpl",None)
         self.hwRef.setBodyName('Jupiter')



    def test_equatorialJ2000(self):
         ra,dec=self.hwRef.getJ2000EquatorialCoordinate(self.acstime.value)
         time.sleep(1)
         print(ra,dec)	 
             
          
         self.assertAlmostEqual(degrees(ra),  328.035447280, places=4)
         self.assertAlmostEqual(degrees(dec),    -13.662097261, places=4)

    def test_equatorialApparent(self):
    	
    	 az,el,ra,dec,l,lon,lat =self.hwRef.getAllCoordinates(self.acstime.value)
         self.assertAlmostEqual(degrees(ra), 328.315829715 , delta=1e-4)   	 
         self.assertAlmostEqual(degrees(dec), -13.564437853  , delta=1e-4)  
    def test_horizontalApparent(self):
    	 az,el,ra,dec,l,lon,lat =self.hwRef.getAllCoordinates(self.acstime.value)
         self.assertAlmostEqual(degrees(az), 84.320777342  , delta=5e-4)   	 
         self.assertAlmostEqual(degrees(el), -28.325680692  , delta=5e-4)  
    
     	 

if __name__ == '__main__':
    
        unittest.main() 
 
