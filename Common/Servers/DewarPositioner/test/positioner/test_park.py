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
        time.sleep(0.5) if using_mock else time.sleep(2)
        offset = 2
        p.setOffset(offset)
        time.sleep(0.5) if using_mock else time.sleep(2)
        p.park()
        time.sleep(0.5) if using_mock else time.sleep(3)
        self.assertAlmostEqual(
                p.getStartingPosition(), 
                device.getActPosition(), 
                places=2
        )



if __name__ == '__main__':
    unittest2.main()
