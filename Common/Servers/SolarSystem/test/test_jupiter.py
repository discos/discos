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
#      def setUp(self):



    def test_planet_position(self):
         epoch = Acspy.Common.TimeHelper.TimeUtil()
         struct_time = time.strptime("2021-Apr-15 00:00", "%Y-%b-%d %H:%M")
         tm=time.mktime(struct_time)
         self.acstime=epoch.py2epoch(tm)
         print(tm)
         simpleClient = PySimpleClient()
         hwRef = simpleClient.getDynamicComponent(None,"IDL:alma/Antenna/SolarSystemBody:1.0","SolarSystemBodyImpl",None)
         try:
             print ('wwww')
             ra,dec=hwRef.getJ2000EquatorialCoordinate(self.acstime.value)
             time.sleep(1)
             print(ra,dec)	 
             
             
             #self.assertAlmostEqual(ra, 328.035447923, places=1)


         except:
         	 print('except')


if __name__ == '__main__':
    
        unittest.main() # Real test using the antenna CDB
 
