import random
import time
import unittest2
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, PositionerError
from Acspy.Clients.SimpleClient import PySimpleClient


class PositionerOffsetTest(unittest2.TestCase):

    def test_set_get(self):
        """Verify the set and get methods"""
        p = Positioner()
        p.setOffset(2)
        self.assertEqual(p.getOffset(), 2)
        p.clearOffset()

    def test_set_new_pos(self):
        """Vefify the setOffset set a new position."""
        try:
            client = PySimpleClient()
            device = client.getComponent('RECEIVERS/SRTKBandDerotator')
            using_mock = False
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from mock_components import MockDevice
            device = MockDevice()
            using_mock = True

        p = Positioner()
        p.setup(site_info={}, source=None, device=device)
        time.sleep(0.5) if using_mock else time.sleep(1)
        act_position = device.getActPosition()
        offset = 1.5
        p.setOffset(offset)
        time.sleep(0.5) if using_mock else time.sleep(3)
        self.assertAlmostEqual(p.getPosition(), act_position + offset, places=2)
        self.assertAlmostEqual(act_position + offset, device.getActPosition(), places=2)
        p.park()


if __name__ == '__main__':
    unittest2.main()
