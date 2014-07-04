import random
import time
import unittest2
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError
from Acspy.Clients.SimpleClient import PySimpleClient


class PositionerOffsetTest(unittest2.TestCase):

    def setUp(self):
        cdb_info = {
                'updating_time': 0.1,
                'rewinding_timeout': 1.5,
                'rewinding_sleep_time': 1
        }
        self.p = Positioner(cdb_info)
        try:
            client = PySimpleClient()
            self.device = client.getComponent('RECEIVERS/SRTKBandDerotator')
            self.using_mock = False
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from DewarPositionerTest.mock_components import MockDevice
            self.device = MockDevice()
            self.using_mock = True

    def tearDown(self):
        self.p.park()
        time.sleep(0.2)

    def test_set_get(self):
        """Verify the set and get methods"""
        self.assertRaises(NotAllowedError, self.p.setOffset, 2)
        self.p.setup(site_info={}, source=None, device=self.device)
        self.p.setOffset(2)
        self.assertEqual(self.p.getOffset(), 2)
        self.p.clearOffset()
        self.assertEqual(self.p.getOffset(), 0)

    def test_set_new_pos(self):
        """Vefify the setOffset set a new position."""
        self.p.setup(site_info={}, source=None, device=self.device)
        time.sleep(0.5) if self.using_mock else time.sleep(1)
        act_position = self.device.getActPosition()
        offset = 1.5
        self.p.setOffset(offset)
        time.sleep(0.5) if self.using_mock else time.sleep(3)
        self.assertAlmostEqual(
                self.p.getPosition(), 
                act_position + offset, 
                places=2
        )
        self.assertAlmostEqual(
                act_position + offset, 
                self.device.getActPosition(), 
                places=2
        )

if __name__ == '__main__':
    unittest2.main()
