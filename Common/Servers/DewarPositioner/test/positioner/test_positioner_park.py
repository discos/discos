import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositioner.positioner import Positioner, NotAllowedError


class PositionerParkTest(unittest2.TestCase):

    def test_notConfigured(self):
        """Verify it raises an exception if not yet configured"""
        p = Positioner()
        self.assertRaises(NotAllowedError, p.park)

    def test_set_starting_pos(self):
        """Vefify the park() method set the device position."""
        p = Positioner()
        p.setup('RECEIVERS/SRTKBandDerotator')
        time.sleep(3)
        offset = 2
        p.setOffset(offset)
        time.sleep(1)
        p.park()
        time.sleep(3)
        derotator = PySimpleClient().getComponent('RECEIVERS/SRTKBandDerotator')
        self.assertAlmostEqual(
                p.getStartingPosition(), 
                derotator.getActPosition(), 
                places=2
        )



if __name__ == '__main__':
    unittest2.main()
