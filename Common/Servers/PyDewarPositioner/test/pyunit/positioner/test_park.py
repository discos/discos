import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositioner.positioner import Positioner, NotAllowedError
from DewarPositioner.cdbconf import CDBConf


class PositionerParkTest(unittest2.TestCase):

    def setUp(self):
        self.cdbconf = CDBConf()
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('FIXED')

    def test_notConfigured(self):
        """Verify it does not raise an exception if not yet configured"""
        p = Positioner(self.cdbconf)
        p.park()

    def test_set_park_position(self):
        """Vefify the park() method sets the device position."""
        client = PySimpleClient()
        device = client.getComponent('RECEIVERS/SRTKBandDerotator')
        using_mock = False
        p = Positioner(self.cdbconf)
        p.setup(siteInfo={}, source=None, device=device)
        time.sleep(2)
        park_position=1
        p.park(parkPosition=park_position)
        time.sleep(3)
        self.assertAlmostEqual(
                park_position,
                device.getActPosition(), 
                places=1
        )



if __name__ == '__main__':
    unittest2.main()
