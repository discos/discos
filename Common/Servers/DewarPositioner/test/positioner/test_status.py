from __future__ import with_statement
import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError, PositionerError
from mock_components import MockDevice, Property, Completion

import mocker

class PositionerStartUpdatingTest(unittest2.TestCase):

    def setUp(self):
        self.p = Positioner()
        self.m = mocker.Mocker()
        self.device = self.m.patch(MockDevice())

    def tearDown(self):
        self.m.restore()
        self.m.verify()

    def test_not_configured(self):
        """Raise a NotAllowedError if the positioner is not configured"""
        self.assertRaises(NotAllowedError, self.p.status)

    def test_cannot_get_device_status_property(self):
        """Raise PositionerError if it cannot get the device status property"""
        self.device._get_status()
        self.m.throw(RuntimeError)
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        with self.assertRaisesRegexp(PositionerError, '^cannot .*status property'):
            self.p.status()

    def test_cannot_get_device_status_value(self):
        """Raise PositionerError if it cannot get the device status value"""
        self.device._get_status()
        self.m.result('foo')
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        with self.assertRaisesRegexp(PositionerError, '^cannot .*status value'):
            self.p.status()

    def test_completion_error(self):
        """Raise PositionerError if the device status has a completion error"""
        self.device._get_status()
        self.m.result(Property(1, Completion(1)))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        with self.assertRaisesRegexp(PositionerError, '.*status value is not valid'):
            self.p.status()

    def test_ok(self):
        """Return a '00111' in case of slewing, tracking and updating"""
        self.device._get_status()
        self.m.result(Property(0, Completion()))
        self.p = self.m.patch(self.p)
        mocker.expect(self.p.isTracking()).result(True)
        self.m.count(1, None)
        mocker.expect(self.p.isUpdating()).result(True)
        self.m.count(1, None)
        mocker.expect(self.p.isSlewing()).result(True)
        self.m.count(1, None)
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.status(), '00111')

    def test_device_power_off(self):
        """Return a '10000' (failure) in case the device is power off"""
        self.device._get_status()
        self.m.result(Property(1, Completion()))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.status(), '10000')

    def test_device_failure(self):
        """Return a '10000' (failure) in case the device is in failure"""
        self.device._get_status()
        self.m.result(Property(2, Completion()))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.status(), '10000')

    def test_device_communication_error(self):
        """Return a '10000' (failure) in case the device status is CE"""
        self.device._get_status()
        self.m.result(Property(4, Completion()))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.status(), '10000')

    def test_device_not_ready(self):
        """Return a '01000' (warning) in case the device is not ready"""
        self.device._get_status()
        self.m.result(Property(8, Completion()))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.status(), '01000')


if __name__ == '__main__':
    unittest2.main()
