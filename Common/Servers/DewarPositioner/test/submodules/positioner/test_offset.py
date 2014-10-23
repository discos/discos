import random
import time
import unittest2
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError
from DewarPositioner.cdbconf import CDBConf
from Acspy.Clients.SimpleClient import PySimpleClient


class PositionerOffsetTest(unittest2.TestCase):

    def setUp(self):
        cdbconf = CDBConf()
        self.p = Positioner(cdbconf)
        try:
            client = PySimpleClient()
            self.device = client.getComponent('RECEIVERS/SRTKBandDerotator')
            self.using_mock = False
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from DewarPositionerMockers.mock_components import MockDevice
            self.device = MockDevice()
            self.using_mock = True

    def tearDown(self):
        self.p.park()
        time.sleep(0.2)

    def test_set_get(self):
        """Verify the set and get methods"""
        # Not allowed when the system is not yet configured
        self.assertRaises(NotAllowedError, self.p.setOffset, 2)
        self.p.setup(siteInfo={}, source=None, device=self.device)
        self.p.setOffset(2)
        self.assertEqual(self.p.getOffset(), 2)
        self.p.clearOffset()
        self.assertEqual(self.p.getOffset(), 0)

    def test_set_new_pos(self):
        """Vefify the setOffset set a new position."""
        self.p.setup(siteInfo={}, source=None, device=self.device)
        time.sleep(0.3) if self.using_mock else time.sleep(3)
        act_position = self.device.getActPosition()
        offset = 0.5
        self.p.setOffset(offset)
        time.sleep(0.3) if self.using_mock else time.sleep(3)
        self.assertAlmostEqual(
                self.p.getPosition(), 
                act_position + offset, 
                places=1
        )
        self.assertAlmostEqual(
                act_position + offset, 
                self.device.getActPosition(), 
                places=1
        )

    def _test_out_of_range(self):
        """Cause a rewind in case the offset is out of range"""
        # TODO: this test fails
        self.p.setup(siteInfo={}, source=None, device=self.device)
        self.p.setRewindingMode('AUTO')
        actual_pos = 100
        self.device.setPosition(actual_pos)
        offset = 50
        max_rewinding_feeds = 180 
        expected = actual_pos - max_rewinding_feeds + offset
        self.p.setOffset(offset)
        time.sleep(0.5) # Wait for the offset
        self.assertEqual(self.device.getActPosition(), expected)


if __name__ == '__main__':
    unittest2.main()
