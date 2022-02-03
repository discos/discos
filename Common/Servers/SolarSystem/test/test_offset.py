import unittest
# Import command-line arguments
from sys import argv

# Import the SimpleClient class
from Acspy.Clients.SimpleClient import PySimpleClient

import Antenna
from math import radians,degrees,cos

import Acspy.Common.TimeHelper
import time


class SetOffsetTest(unittest.TestCase):
    def setUp(self):
         epoch = Acspy.Common.TimeHelper.TimeUtil()
         struct_time = time.strptime("2021-Apr-15 00:00", "%Y-%b-%d %H:%M")
         tm=time.mktime(struct_time)
         self.acstime=epoch.py2epoch(tm)
         print(tm)
         simpleClient = PySimpleClient()
         self.hwRef = simpleClient.getDynamicComponent(None,"IDL:alma/Antenna/SolarSystemBody:1.0","SolarSystemBodyImpl",None)
         self.hwRef.setBodyName('Jupiter')




    def test_equatorialOffset(self):
         ra_ref=radians(328.315829715)
         dec_ref=radians(-13.564437853)
         ra_offs=radians(1)
         dec_offs=radians(1)
         
         self.hwRef.setOffsets(ra_offs,dec_offs,Antenna.ANT_EQUATORIAL) 
         az,el,ra,dec,l,lon,lat =self.hwRef.getAllCoordinates(self.acstime.value)
         self.assertAlmostEqual(degrees(ra), degrees(ra_ref+ra_offs/cos(dec_ref+dec_offs)) , delta=1e-4)   	 
         self.assertAlmostEqual(degrees(dec), degrees(dec_ref+dec_offs)  , delta=1e-4)  
    def test_horizontalOffset(self):
         az_ref=radians(84.320777342)
         el_ref=radians(-28.325680692 )
         az_offs=radians(1)
         el_offs=radians(0)
         self.hwRef.setOffsets(az_offs,el_offs,Antenna.ANT_HORIZONTAL) 
         az,el,ra,dec,l,lon,lat =self.hwRef.getAllCoordinates(self.acstime.value)
         self.assertAlmostEqual(degrees(az),degrees( az_ref+az_offs/cos(el_ref+el_offs))  , delta=5e-4)   	 
         self.assertAlmostEqual(degrees(el), degrees( el_ref) , delta=5e-4)  
    
     	 

if __name__ == '__main__':
    
        unittest.main() 
 
