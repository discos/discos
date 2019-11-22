from __future__ import with_statement, division
import unittest
import mocker
import random
import time
from Antenna import ANT_SOUTH, ANT_NORTH
from Management import MNG_TRACK, MNG_BEAMPARK, MNG_HOR_LON
from math import radians
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError, PositionerError
from DewarPositionerMockers.mock_components import MockDevice, MockSource
from DewarPositioner.cdbconf import CDBConf
from DewarPositioner.posgenerator import PosGenerator

class PositionerStartUpdatingTest(unittest.TestCase):

    def setUp(self):
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
            self.p.startUpdating(MNG_TRACK, ANT_NORTH, az, el, None, None)
            time.sleep(0.2)
            self.assertEqual(self.p.isUpdating(), True)
            self.p.startUpdating(MNG_TRACK, ANT_NORTH, az, el, None, None)
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
            self.p.startUpdating(MNG_TRACK, ANT_NORTH, az, el, None, None)
            time.sleep(0.2)
            self.assertRaises(NotAllowedError, self.p.setPosition, 0)
        finally:
            self.p.stopUpdating()
 
    def test_do_nothing_with_axis_MNG_BEAMPARK(self):
        """Do nothing in case the axis is MNG_BEAMPARK"""
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude, az, el = [radians(50)] * 3
        site_info = {'latitude': latitude}

        self.p.setup(site_info, self.source, self.device)
        try:
            self.p.startUpdating(MNG_BEAMPARK, ANT_NORTH, az, el, None, None)
            time.sleep(0.2)
            self.assertFalse(self.p.isUpdating())
        finally:
            self.p.stopUpdating()

    def test_notYetConfigured(self):
        """Verify startUpdating()"""
        # startUpdating() raises NotAllowedError when the system is not configured
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector', 1, 1, None, None)
        self.assertEqual(self.p.isConfigured(), False)
        self.p.setup(siteInfo={}, source=None, device=self.device)
        # startUpdating() raises NotAllowedError when the system is not configured
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector', 1, 1, None, None)
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        self.assertRaises(NotAllowedError, self.p.startUpdating, 'axis', 'sector', 1, 1, None, None)
        with self.assertRaisesRegexp(NotAllowedError, '^no site information available'):
            self.p.startUpdating('axis', 'sector', 1, 1, None, None)
        self.p.setup(siteInfo={'foo': 'foo'}, source=None, device=self.device)
        self.cdbconf.setConfiguration('BSC')
        with self.assertRaisesRegexp(NotAllowedError, "no source available"):
            self.p.startUpdating('axis', 'sector', 1, 1, None, None)
        self.p.setup(siteInfo={'foo': 'foo'}, source='source', device=self.device)
        self.cdbconf.setConfiguration('FIXED')
        self.p.startUpdating('axis', 'sector', 1, 1, None, None) # Do not raise an exception
        self.p.stopUpdating() # Do not raise exception
        self.cdbconf.setConfiguration('BSC')
        sector = 'WRONG_SECTOR'
        with self.assertRaisesRegexp(NotAllowedError, '^sector %s not in' %sector):
            self.p.startUpdating('axis', sector, 1, 1, None, None)
        axis = 'WRONG_AXIS'
        sector = ANT_NORTH
        with self.assertRaisesRegexp(PositionerError, '^configuration problem:'):
            self.p.startUpdating(axis, sector, 1, 1, None, None)
        axis = MNG_TRACK
        # Raise value error (wrong unpacking)
        self.cdbconf.UpdatingPosition['MNG_TRACK'] = [10] # expected [position, functionName]
        self.assertRaises(PositionerError, self.p.startUpdating, axis, sector, 1, 1, None, None)
        # Raise AttributeError: fooName does not exist
        self.cdbconf.UpdatingPosition['ANT_NORTH'] = [10, 'fooName'] # [position, functionName]
        self.assertRaises(PositionerError, self.p.startUpdating, axis, sector, 1, 1, None, None)

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
                az, el = radians(180), radians(45 + i)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    self.p.startUpdating(MNG_TRACK, ANT_NORTH, az, el, None, None)
                time.sleep(0.11)
                expected = Pis + gen.next()
                self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()

    def test_change_of_sign_negative_to_positive(self):
        "The parallactic angle do not have to change from -180 to 180 degrees"
        site_info = {'latitude': radians(39.49)}
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info, initial_sign=-1)
        azs = [radians(i) for i in (40, 20, 0, 360, 340, 320)]
        els = [radians(i) for i in (81, 83, 85, 85, 83, 81)]
        p0 = None
        for az, el in zip(azs, els):
            self.source.setAzimuth(az)
            self.source.setElevation(el)
            angle = gen.next()
            self.assertIsNotNone(angle)
            if p0:
                delta = abs(angle - p0)
                self.assertLess(delta, 180)
            p0 = angle

    def test_change_of_sign_positive_to_negative(self):
        "The parallactic angle do not have to change from 180 to -180 degrees"
        site_info = {'latitude': radians(39.49)}
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info, initial_sign=+1)
        azs = reversed([radians(i) for i in (40, 20, 0, 360, 340, 320)])
        els = reversed([radians(i) for i in (81, 83, 85, 85, 83, 81)])
        p0 = None
        for az, el in zip(azs, els):
            self.source.setAzimuth(az)
            self.source.setElevation(el)
            angle = gen.next()
            self.assertIsNotNone(angle)
            if p0:
                delta = abs(angle - p0)
                self.assertLess(delta, 180)
            p0 = angle


    def test_clearSource(self):
        "Put the parallactic angle sign to None"
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude = radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        az = radians(45)
        el  = radians(45)
        self.source.setAzimuth(az)
        self.source.setElevation(el)
        self.assertIsNone(self.p.sign)
        self.p.startUpdating(MNG_TRACK, ANT_NORTH, az, el, None, None)
        time.sleep(0.5)
        self.assertIsNotNone(self.p.sign)
        self.p._clearSign()
        self.assertIsNone(self.p.sign)


    def test_custom_auto_rewinding(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude = radians(39.49)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        self.p.setRewindingMode('AUTO')
        
        Pis = -23
        self.cdbconf.updateInitialPositions(Pis)
        try:
            # Az and el in radians; the related parallactic angle is -63
            az, el = 1.2217, 0.6109
            parallactic = PosGenerator.getParallacticAngle(latitude, az, el)
            target = Pis + parallactic
            min_limit = self.device.getMinLimit()
            # The final angle is -86, lower than the min limit (-85.77)
            self.assertLess(target, min_limit)

            self.source.setAzimuth(az)
            self.source.setElevation(el)
            self.p.startUpdating(MNG_TRACK, ANT_NORTH, az, el, None, None)

            # For the K Band, we expect a rewind of 180 degrees
            # rewind_angle = self.p.getAutoRewindingSteps() * self.device.getStep()
            rewind_angle = 180 # The maximum, in case getAutoRewindingSteps is 0
            expected = target + rewind_angle
            time.sleep(0.11)
            self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()


    def test_startUpdating_during_rewind(self):
        """When you call startUpdating during a rewind, the updating must
        continue with the new parameters"""
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM')
        latitude = radians(39.49)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        self.p.setRewindingMode('AUTO')
        
        Pis = -23
        self.cdbconf.updateInitialPositions(Pis)
        try:
            # Az and el in radians; the related parallactic angle is -63
            az, el = 1.2217, 0.6109
            parallactic = PosGenerator.getParallacticAngle(latitude, az, el)
            target = Pis + parallactic
            min_limit = self.device.getMinLimit()
            # The final angle is -86, lower than the min limit (-85.77)
            self.assertLess(target, min_limit)

            self.source.setAzimuth(az)
            self.source.setElevation(el)
            sleep_time = 5.0
            try:
                setPosition = self.device.setPosition
                def mockSetPosition(value):
                    setPosition(value)
                    time.sleep(sleep_time)
                self.device.setPosition = mockSetPosition
                self.p.startUpdating(MNG_TRACK, ANT_NORTH, az, el, None, None)
                time.sleep(sleep_time/2)
                self.cdbconf.updateInitialPositions(0)
                az, el = 0.6109, 0.6109
                self.p.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, None, None)
                time.sleep(sleep_time/2 + 1)
            finally:
                self.device.setPosition = setPosition
            ppos = PosGenerator.getParallacticAngle(latitude, az, el)
            self.assertEqual(self.p.control.scanInfo['iParallacticPos'], ppos)
            self.assertEqual(self.p.control.scanInfo['sector'], ANT_SOUTH)
            self.assertTrue(self.p.isUpdating())
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
                    self.p.startUpdating(MNG_TRACK, ANT_NORTH, az, el, None, None)
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
        axisCode = MNG_TRACK
        updatingConf = self.cdbconf.getUpdatingConfiguration(str(axisCode))
        Pis = float(updatingConf['initialPosition'])
        try:
            begin_idx = 5
            for i in range(begin_idx, 10):
                az, el = radians(i*10), radians(i*5)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    self.p.startUpdating(axisCode, ANT_NORTH, az, el, None, None)
                time.sleep(0.11)
                expected = Pis + gen.next()
                self.assertAlmostEqual(expected, self.device.getActPosition(), places=2)
        finally:
            self.p.stopUpdating()

    def test_bsc_opt(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC_OPT')
        latitude = radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        
        posgen = PosGenerator()
        gen = posgen.parallactic(self.source, site_info)
        axisCode = MNG_TRACK
        updatingConf = self.cdbconf.getUpdatingConfiguration(str(axisCode))
        Pis = float(updatingConf['initialPosition'])
        try:
            begin_idx = 5
            for i in range(begin_idx, 10):
                az, el = radians(i*10), radians(i*5)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    Pip = PosGenerator.getParallacticAngle(latitude, az, el)
                    self.p.startUpdating(axisCode, ANT_NORTH, az, el, None, None)
                time.sleep(0.11)
                expected = Pis + gen.next() - Pip # Only the delta
                self.assertAlmostEqual(expected, self.device.getActPosition(), places=2)
        finally:
            self.p.stopUpdating()

    def test_BSC_staticX(self):
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('BSC')
        latitude = radians(50)
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        
        axisCode = MNG_HOR_LON
        updatingConf = self.cdbconf.getUpdatingConfiguration(str(axisCode))
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
                    self.p.startUpdating(axisCode, ANT_NORTH, az, el, None, None)
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
        
        axisCode = MNG_HOR_LON
        initialPosition = 2
        self.p.setPosition(initialPosition)
        updatingConf = self.cdbconf.getUpdatingConfiguration(str(axisCode))
        functionName = updatingConf['functionName']
        staticValue = float(functionName.lstrip('static'))
        try:
            begin_idx = 5
            for i in range(begin_idx, 10):
                az, el = radians(i*10), radians(i*5)
                self.source.setAzimuth(az)
                self.source.setElevation(el)
                if i == begin_idx:
                    self.p.startUpdating(axisCode, ANT_NORTH, az, el, None, None)
                time.sleep(0.11)
            expected = initialPosition + staticValue
            self.assertEqual(expected, self.device.getActPosition())
        finally:
            self.p.stopUpdating()


if __name__ == '__main__':
    unittest.main()
