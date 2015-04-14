from __future__ import with_statement
import unittest2
import random
import math
import time

import Management
import Antenna

from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError, PositionerError
from DewarPositionerMockers.mock_components import MockDevice, MockSource
from DewarPositioner.cdbconf import CDBConf
from DewarPositioner.posgenerator import PosGenerator

class PositionerStartUpdatingTest(unittest2.TestCase):

    def setUp(self):
        self.device = MockDevice()
        self.source = MockSource()
        self.cdbconf = CDBConf()
        self.cdbconf.attributes['RewindingTimeout'] = 2
        self.p = Positioner(self.cdbconf)

    def tearDown(self):
        self.p.park()
 
    def test_notUpdating(self):
        """Verify the default values"""
        info = self.p.getScanInfo()
        axis = info['axis']
        self.assertEqual(axis, Management.MNG_NO_AXIS)

    def test_updating(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude = az = el = math.radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info)
        self.assertEqual(self.p.getScanInfo()['axis'], Management.MNG_NO_AXIS)
        self.p.startUpdating(Management.MNG_TRACK, Antenna.ANT_NORTH, az, el)
        self.assertEqual(self.p.getScanInfo()['axis'], Management.MNG_TRACK)
        self.p.stopUpdating()
        self.assertEqual(self.p.getScanInfo()['axis'], Management.MNG_NO_AXIS)

    def test_Pis(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('FIXED')
        self.p.setup({}, self.source, self.device)
        self.p.setPosition(2)
        self.assertEqual(self.p.getScanInfo()['iStaticPos'], 2)

    def test_Pip(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        latitude = math.radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        az = el = math.radians(60)
        Pip = PosGenerator.getParallacticAngle(latitude, az, el)
        self.p.startUpdating(Management.MNG_TRACK, Antenna.ANT_NORTH, az, el)
        self.assertEqual(self.p.getScanInfo()['iStaticPos'], -19.2)
        self.assertEqual(self.p.getScanInfo()['iParallacticPos'], Pip)

    def test_Pdp(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude = az = el = math.radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info)
        self.p.startUpdating(Management.MNG_TRACK, Antenna.ANT_NORTH, az, el)
        self.assertEqual(self.p.getScanInfo()['axis'], Management.MNG_TRACK)
        self.p.stopUpdating()
        self.assertEqual(self.p.getScanInfo()['axis'], Management.MNG_NO_AXIS)


if __name__ == '__main__':
    unittest2.main()
