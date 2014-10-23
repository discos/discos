from __future__ import with_statement
import unittest2
import random
import time
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

    def test_notYetConfigured(self):
        """Verify startUpdating()"""
        # startUpdating() raises NotAllowedError when the system is not configured
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector')
        self.assertEqual(self.p.isConfigured(), False)
        self.p.setup(siteInfo={}, source=None, device=self.device)
        # startUpdating() raises NotAllowedError when the system is not configured
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector')
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector')
        with self.assertRaisesRegexp(NotAllowedError, '^no site information available'):
            self.p.startUpdating('axis', 'sector')
        self.p.setup(siteInfo={'foo': 'foo'}, source=None, device=self.device)
        with self.assertRaisesRegexp(NotAllowedError, 'no source available'):
            self.p.startUpdating('axis', 'sector')
        self.p.setup(siteInfo={'foo': 'foo'}, source='self.source', device=self.device)
        self.cdbconf.setConfiguration('FIXED')
        with self.assertRaisesRegexp(NotAllowedError, '^dynamic'):
            self.p.startUpdating('axis', 'sector')
        self.cdbconf.setConfiguration('BSC')
        sector = 'WRONG_SECTOR'
        with self.assertRaisesRegexp(NotAllowedError, '^sector %s not in' %sector):
            self.p.startUpdating('axis', sector)
        axis = 'WRONG_AXIS'
        sector = 'ANT_NORTH'
        with self.assertRaisesRegexp(PositionerError, '^configuration problem:'):
            self.p.startUpdating(axis, sector)
        axis = 'MNG_TRACK'
        # Raise value error (wrong unpacking)
        self.cdbconf.UpdatingPosition['MNG_TRACK'] = [10] # expected [position, functionName]
        self.assertRaises(PositionerError, self.p.startUpdating, axis, sector)
        # Raise AttributeError: fooName does not exist
        self.cdbconf.UpdatingPosition['ANT_NORTH'] = [10, 'fooName'] # [position, functionName]
        self.assertRaises(PositionerError, self.p.startUpdating, axis, sector)

    def test_bsc(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        site_info = {'latitude': 50}
        self.p.setup(site_info, self.source, self.device)
        
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info)
        axisCode = 'MNG_TRACK'
        updatingConf = self.cdbconf.getUpdatingConfiguration(axisCode)
        initialPosition = float(updatingConf['initialPosition'])
        try:
            self.p.startUpdating(axisCode, 'ANT_NORTH')
            for i in range(5, 15):
                self.source.setAzimuth(i*10)
                self.source.setElevation(i*5)
                time.sleep(0.11)
                expected = initialPosition + gen.next()
                self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()

    def test_staticX(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        site_info = {'latitude': 50}
        self.p.setup(site_info, self.source, self.device)
        
        posgen = PosGenerator()
        axisCode = 'MNG_HOR_LON'
        gen = posgen.parallactic(self.source, site_info)
        updatingConf = self.cdbconf.getUpdatingConfiguration(axisCode)
        initialPosition = float(updatingConf['initialPosition'])
        functionName = updatingConf['functionName']
        staticValue = float(functionName.lstrip('static'))
        try:
            self.p.startUpdating(axisCode, 'ANT_NORTH')
            expected = initialPosition + staticValue
            self.assertEqual(self.device.getActPosition(), expected)
        finally:
            self.p.stopUpdating()



if __name__ == '__main__':
    unittest2.main()
