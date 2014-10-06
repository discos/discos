import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositioner.positioner import Positioner, NotAllowedError


class PositionerParkTest(unittest2.TestCase):

    def setUp(self):
        self.cdbInfo = {
                'UpdatingTime': 0.1,
                'RewindingTimeout': 1.5,
                'RewindingSleepTime': 1
        }

    def test_notConfigured(self):
        """Verify it raises an exception if not yet configured"""
        p = Positioner(self.cdbInfo)
        self.assertRaises(NotAllowedError, p.park)

    def test_set_park_position(self):
        """Vefify the park() method sets the device position."""
        try:
            client = PySimpleClient()
            device = client.getComponent('RECEIVERS/SRTKBandDerotator')
            using_mock = False
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from DewarPositionerMockers.mock_components import MockDevice
            device = MockDevice()
            using_mock = True

        p = Positioner(self.cdbInfo)
        p.setup(siteInfo={}, source=None, device=device)
        time.sleep(0.5) if using_mock else time.sleep(2)
        offset = 2
        p.setOffset(offset)
        time.sleep(0.5) if using_mock else time.sleep(2)
        park_position=1
        p.park(park_position=park_position)
        time.sleep(0.5) if using_mock else time.sleep(3)
        self.assertAlmostEqual(
                park_position,
                device.getActPosition(), 
                places=2
        )



if __name__ == '__main__':
    unittest2.main()
