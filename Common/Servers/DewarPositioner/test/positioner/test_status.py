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
        """Return a not ready pattern if the positioner is not configured"""
        self.assertEqual(self.p.getStatus(), '000000')

    def test_device_not_ready(self):
        """Return a not ready pattern if the device is not ready"""
        self.device.isReady()
        self.m.result(False)
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        self.assertEqual(self.p.getStatus(), '000000')

    def test_cannot_get_device_status_property(self):
        """Return a failure pattern if it cannot get the device status property"""
        self.device._get_status()
        self.m.throw(RuntimeError)
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        self.assertEqual(self.p.getStatus(), '100000')

    def test_cannot_get_device_status_value(self):
        """Return a failure pattern if it cannot get the device status value"""
        self.device._get_status()
        self.m.result('foo')
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        self.assertEqual(self.p.getStatus(), '100000')

    def test_completion_error(self):
        """Return a failure pattern if the device status has a completion error"""
        self.device._get_status()
        self.m.result(Property(1, Completion(1)))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        self.assertEqual(self.p.getStatus(), '100000')

    def test_ok(self):
        """Return a '001111' in case of ready, slewing, tracking and updating"""
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
        self.assertEqual(self.p.getStatus(), '001111')

    def test_device_power_off(self):
        """Return a '100000' (failure) in case the device is not ready and power off"""
        self.device._get_status()
        self.m.result(Property(1, Completion()))
        self.device.isReady()
        self.m.result(False)
        self.m.count(1, None)
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.getStatus(), '100000')

    def test_device_failure(self):
        """Return a '100000' (failure) in case the device is ready and in failure"""
        self.device._get_status()
        self.m.result(Property(2, Completion()))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.getStatus(), '100001')

    def test_device_communication_error(self):
        """Return a '100001' (ready and failure) in case the device status is CE"""
        self.device._get_status()
        self.m.result(Property(4, Completion()))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.getStatus(), '100001')

    def test_device_not_ready(self):
        """Return a '000001' in case the device is not ready"""
        self.device._get_status()
        self.m.result(Property(8, Completion()))
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.getStatus(), '000001')

    def test_unexpected_exception(self):
        """Return a failure pattern if case of unexpected exception"""
        self.device.isSlewing()
        self.m.throw(RuntimeError)
        self.m.replay()
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.2)
        self.assertEqual(self.p.getStatus(), '100000')


if __name__ == '__main__':
    unittest2.main()
