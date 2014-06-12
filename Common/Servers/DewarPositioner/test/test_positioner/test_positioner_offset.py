import unittest2
import random
import time
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
        device_name = 'RECEIVERS/SRTKBandDerotator'
        client = PySimpleClient()
        derotator = client.getComponent(device_name)
        p = Positioner()
        p.setup(device_name)
        time.sleep(3)
        act_position = derotator.getActPosition()
        offset = 1.5
        p.setOffset(offset)
        time.sleep(3)
        self.assertAlmostEqual(p.getPosition(), act_position + offset, places=2)
        self.assertAlmostEqual(act_position + offset, derotator.getActPosition(), places=2)
        p.park()


if __name__ == '__main__':
    unittest2.main()
