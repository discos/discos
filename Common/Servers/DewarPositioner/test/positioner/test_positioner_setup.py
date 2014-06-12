import unittest2
import random
import time
from DewarPositioner.positioner import Positioner, PositionerError


class PositionerSetupTest(unittest2.TestCase):

    def test_wrongname(self):
        """Verify the it raiese a exception in case of wrong name"""
        p = Positioner()
        self.assertRaises(PositionerError, p.setup, 'foo')

    def test_starting_pos(self):
        """Verify it sets properly the starting position"""
        device_name = 'RECEIVERS/SRTKBandDerotator'
        p = Positioner()
        p.setup(device_name, 2)
        self.assertEqual(p.getStartingPosition(), 2)

    def test_device_name(self):
        """Verify it sets properly the starting position"""
        device_name = 'RECEIVERS/SRTKBandDerotator'
        p = Positioner()
        p.setup(device_name, 2)
        self.assertEqual(p.getDeviceName(), device_name)


if __name__ == '__main__':
    unittest2.main()
