import unittest
# Import command-line arguments
from sys import argv

# Import the SimpleClient class
from Acspy.Clients.SimpleClient import PySimpleClient

import Antenna
from math import radians,degrees

import Acspy.Common.TimeHelper
import time


class JupiterDistanceTest(unittest.TestCase):
    def setUp(self):
         epoch = Acspy.Common.TimeHelper.TimeUtil()
         struct_time = time.strptime("2021-Apr-15 00:00", "%Y-%b-%d %H:%M")
         tm=time.mktime(struct_time)
         self.acstime=epoch.py2epoch(tm)
         print(tm)
         simpleClient = PySimpleClient()
         time.sleep(1.)
         self.hwRef = simpleClient.getDynamicComponent(None,"IDL:alma/Antenna/SolarSystemBody:1.0","SolarSystemBodyImpl",None)
         self.hwRef.setBodyName('Jupiter')



    def test_jupiter_distance(self):
         distance=self.hwRef.getDistance(self.acstime.value)
         time.sleep(1)

         print('distance',distance)
         
          
         self.assertAlmostEqual(distance,  5.49776341649024, delta=5e-5)

  
    
     	 

if __name__ == '__main__':
    
        unittest.main() 
 
