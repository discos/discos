from __future__ import with_statement
import unittest2
import mocker
import random
import time
from math import radians
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError, PositionerError
from DewarPositionerMockers.mock_components import MockDevice, MockSource
from DewarPositioner.cdbconf import CDBConf
from DewarPositioner.posgenerator import PosGenerator

class PositionerStartUpdatingTest(unittest2.TestCase):

    def setUp(self):
        self.m = mocker.Mocker()
        self.device = MockDevice()
        self.source = MockSource()
        self.cdbconf = CDBConf()
        self.cdbconf.attributes['RewindingTimeout'] = 2
        self.cdbconf.attributes['UpdatingTime'] = 0.1
        self.p = Positioner(self.cdbconf)

    def tearDown(self):
        self.p.park()
 
    def test_alreadyUpdating(self):
        """The call to startUpdating() have to stop and start again a new updating"""
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        latitude, az, el = [radians(50)] * 3
        site_info = {'latitude': latitude}
        
        self.p.setup(site_info, self.source, self.device)
        try:
            self.p.startUpdating('MNG_TRACK', 'ANT_NORTH', az, el)
            time.sleep(0.2)
            self.assertEqual(self.p.isUpdating(), True)
            self.p.startUpdating('MNG_TRACK', 'ANT_NORTH', az, el)
            time.sleep(0.2)
            self.assertEqual(self.p.isUpdating(), True)
        finally:
            self.p.stopUpdating()
 
    def test_cannotSetPosition(self):
        """Cannot set position during an updating"""
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude, az, el = [radians(50)] * 3
        site_info = {'latitude': latitude}

        self.p.setup(site_info, self.source, self.device)
        try:
            self.p.startUpdating('MNG_TRACK', 'ANT_NORTH', az, el)
            time.sleep(0.2)
            self.assertRaises(NotAllowedError, self.p.setPosition, 0)
        finally:
            self.p.stopUpdating()

    def test_notYetConfigured(self):
        """Verify startUpdating()"""
        # startUpdating() raises NotAllowedError when the system is not configured
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector', 1, 1)
        self.assertEqual(self.p.isConfigured(), False)
        self.p.setup(siteInfo={}, source=None, device=self.device)
        # startUpdating() raises NotAllowedError when the system is not configured
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector', 1, 1)
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector', 1, 1)
        with self.assertRaisesRegexp(NotAllowedError, '^no site information available'):
            self.p.startUpdating('axis', 'sector', 1, 1)
        self.p.setup(siteInfo={'foo': 'foo'}, source=None, device=self.device)
        self.cdbconf.setConfiguration('BSC')
        with self.assertRaisesRegexp(NotAllowedError, "no source available"):
            self.p.startUpdating('axis', 'sector', 1, 1)
        self.p.setup(siteInfo={'foo': 'foo'}, source='source', device=self.device)
        self.cdbconf.setConfiguration('FIXED')
        self.p.startUpdating('axis', 'sector', 1, 1) # Do not raise an exception
        self.p.stopUpdating() # Do not raise exception
        self.cdbconf.setConfiguration('BSC')
        sector = 'WRONG_SECTOR'
        with self.assertRaisesRegexp(NotAllowedError, '^sector %s not in' %sector):
            self.p.startUpdating('axis', sector, 1, 1)
        axis = 'WRONG_AXIS'
        sector = 'ANT_NORTH'
        with self.assertRaisesRegexp(PositionerError, '^configuration problem:'):
            self.p.startUpdating(axis, sector, 1, 1)
        axis = 'MNG_TRACK'
        # Raise value error (wrong unpacking)
        self.cdbconf.UpdatingPosition['MNG_TRACK'] = [10] # expected [position, functionName]
        self.assertRaises(PositionerError, self.p.startUpdating, axis, sector, 1, 1)
        # Raise AttributeError: fooName does not exist
        self.cdbconf.UpdatingPosition['ANT_NORTH'] = [10, 'fooName'] # [position, functionName]
        self.assertRaises(PositionerError, self.p.startUpdating, axis, sector, 1, 1)

    def test_custom(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude = radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        
        Pis = 2.0
        self.cdbconf.updateInitialPositions(Pis)
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info)
        try:
            begin_idx = 5
            for i in range(begin_idx, 10):
                az, el = radians(i*10), radians(i*5)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    self.p.startUpdating('MNG_TRACK', 'ANT_NORTH', az, el)
                time.sleep(0.11)
                expected = Pis + gen.next()
                self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()

    def test_custom_opt(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM_OPT')
        latitude = radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        
        Pis = 2.0
        self.cdbconf.updateInitialPositions(Pis)
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info)
        try:
            begin_idx = 5
            for i in range(begin_idx, 10):
                az, el = radians(i*10), radians(i*5)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    Pip = PosGenerator.getParallacticAngle(latitude, az, el)
                    self.p.startUpdating('MNG_TRACK', 'ANT_NORTH', az, el)
                time.sleep(0.11)
                expected = Pis + gen.next() - Pip # Only the delta
                self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()

    def test_bsc(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        latitude = radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info)
        axisCode = 'MNG_TRACK'
        updatingConf = self.cdbconf.getUpdatingConfiguration(axisCode)
        initialPosition = float(updatingConf['initialPosition'])
        try:
            begin_idx = 5
            for i in range(begin_idx, 10):
                az, el = radians(i*10), radians(i*5)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    self.p.startUpdating(axisCode, 'ANT_NORTH', az, el)
                time.sleep(0.11)
            expected = initialPosition + gen.next()
            self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()

    def test_BSC_staticX(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        latitude = radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        
        axisCode = 'MNG_HOR_LON'
        updatingConf = self.cdbconf.getUpdatingConfiguration(axisCode)
        initialPosition = float(updatingConf['initialPosition'])
        functionName = updatingConf['functionName']
        staticValue = float(functionName.lstrip('static'))
        try:
            begin_idx = 5
            for i in range(begin_idx, 10):
                az, el = radians(i*10), radians(i*5)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    self.p.startUpdating(axisCode, 'ANT_NORTH', az, el)
                time.sleep(0.11)
            expected = initialPosition + staticValue
            self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()


    def test_CUSTOM_staticX(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude = radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        
        axisCode = 'MNG_HOR_LON'
        initialPosition = 2
        self.p.setPosition(initialPosition)
        updatingConf = self.cdbconf.getUpdatingConfiguration(axisCode)
        functionName = updatingConf['functionName']
        staticValue = float(functionName.lstrip('static'))
        try:
            begin_idx = 5
            for i in range(begin_idx, 10):
                az, el = radians(i*10), radians(i*5)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    self.p.startUpdating(axisCode, 'ANT_NORTH', az, el)
                time.sleep(0.11)
            expected = initialPosition + staticValue
            self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()


if __name__ == '__main__':
    unittest2.main()
