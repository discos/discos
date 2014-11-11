from __future__ import with_statement
import threading
import time
import unittest2
import mocker

from DewarPositioner.positioner import Positioner, PositionerError, NotAllowedError
from DewarPositioner.cdbconf import CDBConf
from DewarPositionerMockers.mock_components import MockDevice

class RewindTest(unittest2.TestCase):

    def setUp(self):
        self.device = MockDevice()
        self.m = mocker.Mocker()
        self.cdbconf = CDBConf()
        self.cdbconf.attributes['RewindingTimeout'] = '2.0'
        self.p = Positioner(self.cdbconf)
        self.p.setup(siteInfo={}, source=None, device=self.device)

    def tearDown(self):
        self.m.restore()
        self.m.verify()

    def test_number_of_feeds_oor(self):
        """Raise PositionerError when the number of feeds is out of range"""
        with self.assertRaisesRegexp(PositionerError, 'actual pos: {0.0}'):
            self.p.rewind(numberOfFeeds=3)

    def test_not_positive_number_of_feeds(self):
        """Raise PositionerError when the number of feeds is not positive"""
        with self.assertRaisesRegexp(PositionerError, 'feeds must be positive$'):
            self.p.rewind(numberOfFeeds=0)

    def test_cannot_get_actual_position(self):
        """Raise PositionerError when the number of feeds is not positive"""
        self.device = self.m.patch(self.device)
        self.device.getActPosition()
        self.m.throw(RuntimeError)
        self.m.replay()
        with self.assertRaisesRegexp(PositionerError, 'cannot get the device'):
            self.p.rewind(numberOfFeeds=2)

    def test_timeout_exceeded(self):
        """Raise PositionerError when the timeout is exceeded"""
        timeout = float(self.cdbconf.getAttribute('RewindingTimeout'))
        setattr(self.device, 'is_tracking', False)
        with self.assertRaisesRegexp(PositionerError, '%ss exceeded' %timeout):
            self.p.rewind(numberOfFeeds=2)

    def test_is_rewinding(self):
        """Verify the isRewinding(), isRewindingRequired() and isTracking()"""
        n = 2 # Number of feeds
        t = threading.Thread(
                   name='isRewinding() test',
                   target=self.p.rewind,
                   args=(n,)
        )
        self.assertEqual(self.p.isRewinding(), False)
        timeout = float(self.cdbconf.getAttribute('RewindingTimeout'))
        sleep_time = float(self.cdbconf.getAttribute('RewindingSleepTime'))
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
        time.sleep(timeout / 2.0)
        self.assertEqual(self.p.isRewinding(), True)
        self.assertEqual(self.p.isRewindingRequired(), False)
        self.assertEqual(self.p.isTracking(), False)
        time.sleep(timeout)
        self.assertEqual(self.p.isRewinding(), False)

    def test_tracking_when_rewinding_required(self):
        """Verify the tracking flag is False when a rewinding is required"""
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
        self.p.rewind(numberOfFeeds=n)
        self.assertEqual(self.device.getActPosition(), expected)

    def test_position_after_rewinding_from100(self):
        """Verify the value of the position after the rewinding (from 100)"""
        time.sleep(0.5) # Wait until the setup is completed
        n = 2
        self.device.setPosition(100)
        expected = sum(self.p.getRewindingParameters(n))
        self.p.rewind(numberOfFeeds=n)
        self.assertEqual(self.device.getActPosition(), expected)

    def test_position_after_auto_rewinding_from100(self):
        """Verify the value of the position after auto rewinding (from 0)"""
        time.sleep(0.5) # Wait until the setup is completed
        self.device.setPosition(100)
        expected = sum(self.p.getRewindingParameters())
        self.p.rewind()
        self.assertEqual(self.device.getActPosition(), expected)

    def test_xxxAutoRewindingFeeds(self):
        """Verify setAutoRewindingFeeds() and clearAutoRewindingFeeds()"""
        time.sleep(0.5) # Wait until the setup is completed
        n = 1
        # NotAllowedError is raised in case of a number of feeds too large
        with self.assertRaisesRegexp(NotAllowedError, 'max number of feeds'):
            self.p.setAutoRewindingFeeds(5)
        # NotAllowedError is raised in case of a number of feeds not positive
        with self.assertRaisesRegexp(NotAllowedError, 'must be positive'):
            self.p.setAutoRewindingFeeds(0)
        self.device.setPosition(100)
        self.p.setAutoRewindingFeeds(n)
        expected = sum(self.p.getRewindingParameters(n))
        self.p.rewind(self.p.getAutoRewindingFeeds())
        self.assertEqual(self.device.getActPosition(), expected)
        self.device.setPosition(100)
        self.p.clearAutoRewindingFeeds()
        expected = sum(self.p.getRewindingParameters())
        self.p.rewind(self.p.getAutoRewindingFeeds())
        self.assertEqual(self.device.getActPosition(), expected)
        self.p.park()
        # A NotAllowedError is raised in case the system is not configured
        with self.assertRaisesRegexp(NotAllowedError, 'not configured'):
            self.p.setAutoRewindingFeeds(n)

    def test_wrong_auto_rewinding_feeds(self):
        """must_update is True in case of wrong auto rewinding_feeds during rewinding"""
        time.sleep(0.5) # Wait until the setup is completed
        n = 4
        self.p.setAutoRewindingFeeds(n)
        self.device.setPosition(50)
        self.assertEqual(self.p.isRewindingRequired(), False)
        with self.assertRaisesRegexp(PositionerError, 'actual pos: {50.0}'):
            self.p.rewind()
        time.sleep(0.5) # Wait until the rewind returns
        self.assertEqual(self.p.isRewindingRequired(), True)


if __name__ == '__main__':
    unittest2.main()
