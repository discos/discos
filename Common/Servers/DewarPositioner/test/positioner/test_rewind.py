from __future__ import with_statement
import threading
import time
import unittest2
import mocker

from DewarPositioner.positioner import Positioner, PositionerError
from DewarPositionerTest.mock_components import MockDevice

class RewindTest(unittest2.TestCase):

    def setUp(self):
        self.device = MockDevice()
        self.m = mocker.Mocker()
        self.cdb_info = {
                'updating_time': 0.1,
                'rewinding_timeout': 1.5,
                'rewinding_sleep_time': 1
        }
        self.p = Positioner(self.cdb_info)
        self.p.setup(site_info={}, source=None, device=self.device)

    def tearDown(self):
        self.m.restore()
        self.m.verify()

    def test_number_of_feeds_oor(self):
        """Raise PositionerError when the number of feeds is out of range"""
        with self.assertRaisesRegexp(PositionerError, 'actual pos: {0.0}'):
            self.p.rewind(number_of_feeds=3)

    def test_not_positive_number_of_feeds(self):
        """Raise PositionerError when the number of feeds is not positive"""
        with self.assertRaisesRegexp(PositionerError, 'feeds must be positive$'):
            self.p.rewind(number_of_feeds=0)

    def test_cannot_get_actual_position(self):
        """Raise PositionerError when the number of feeds is not positive"""
        self.device = self.m.patch(self.device)
        self.device.getActPosition()
        self.m.throw(RuntimeError)
        self.m.replay()
        with self.assertRaisesRegexp(PositionerError, 'cannot get the device'):
            self.p.rewind(number_of_feeds=2)

    def test_timeout_exceeded(self):
        """Raise PositionerError when the timeout is exceeded"""
        timeout = self.cdb_info['rewinding_timeout']
        setattr(self.device, 'is_tracking', False)
        with self.assertRaisesRegexp(PositionerError, '%ss exceeded' %timeout):
            self.p.rewind(number_of_feeds=2)

    def test_is_rewinding(self):
        """Verify the isRewinding(), isRewindingRequired() and isTracking()"""
        n = 2 # Number of feeds
        t = threading.Thread(
                   name='isRewinding() test',
                   target=self.p.rewind,
                   args=(n,)
        )
        self.assertEqual(self.p.isRewinding(), False)
        timeout = self.cdb_info['rewinding_timeout'] 
        sleep_time = self.cdb_info['rewinding_sleep_time']
        requests = timeout / sleep_time
        self.device = self.m.patch(self.device)
        self.device.isTracking()
        for i in range(int(requests//2)):
            self.device.isTracking()
            self.m.result(False)
        self.device.isTracking()
        self.m.result(True)
        self.m.count(1, None)
        self.m.replay()
        t.start()
        time.sleep(self.cdb_info['rewinding_timeout'] / 2.0)
        self.assertEqual(self.p.isRewinding(), True)
        self.assertEqual(self.p.isRewindingRequired(), False)
        self.assertEqual(self.p.isTracking(), False)
        time.sleep(self.cdb_info['rewinding_timeout'])
        self.assertEqual(self.p.isRewinding(), False)

    def test_tracking_when_rewinding_required(self):
        """Verify the traking is False when the rewinding is required"""
        self.p = self.m.patch(self.p)
        self.p.isRewindingRequired()
        self.m.result(True)
        self.m.count(2)
        self.m.replay()
        self.assertEqual(self.p.isRewindingRequired(), True)
        self.assertEqual(self.p.isRewinding(), False)
        self.assertEqual(self.p.isTracking(), False)

    def test_position_after_rewinding_from0(self):
        """Verify the value of the position after the rewinding (from 0)"""
        n = 2
        self.device = self.m.patch(self.device)
        mocker.expect(self.device.isTracking()).result(True)
        self.m.replay()
        expected = sum(self.p.getRewindingParameters(n))
        self.p.rewind(number_of_feeds=n)
        self.assertEqual(self.device.getActPosition(), expected)

    def test_position_after_rewinding_from100(self):
        """Verify the value of the position after the rewinding (from 100)"""
        time.sleep(0.5) # Wait until the setup is completed
        n = 2
        self.device.setPosition(100)
        expected = sum(self.p.getRewindingParameters(n))
        self.p.rewind(number_of_feeds=n)
        self.assertEqual(self.device.getActPosition(), expected)

    def test_position_after_auto_rewinding_from100(self):
        """Verify the value of the position after auto rewinding (from 0)"""
        time.sleep(0.5) # Wait until the setup is completed
        self.device.setPosition(100)
        expected = sum(self.p.getRewindingParameters(None))
        self.p.rewind(None)
        self.assertEqual(self.device.getActPosition(), expected)


if __name__ == '__main__':
    unittest2.main()
