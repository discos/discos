import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, PositionerError
from DewarPositioner.cdbconf import CDBConf
from Acspy.Clients.SimpleClient import PySimpleClient


class PositionerSetupTest(unittest2.TestCase):

    def setUp(self):
        self.cdbconf = CDBConf()
        self.cdbconf.attributes['RewindingTimeout'] = 2
        try:
            client = PySimpleClient()
            self.device = client.getComponent('RECEIVERS/SRTKBandDerotator')
            self.using_mock = False
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from DewarPositionerMockers.mock_components import MockDevice
            self.device = MockDevice()
            self.using_mock = True

    def test_starting_pos(self):
        """Verify it sets properly the starting position"""
        p = Positioner(self.cdbconf)
        p.setup(siteInfo={}, source=None, device=self.device, setupPosition=2)
        time.sleep(0.5) if self.using_mock else time.sleep(2)
        self.assertEqual(p.getPosition(), 2)

    def test_device_name(self):
        """Verify it sets properly the device_name"""
        p = Positioner(self.cdbconf)
        p.setup(siteInfo={}, source=None, device=self.device)
        self.assertEqual(p.getDeviceName(), self.device._get_name())


if __name__ == '__main__':
    unittest2.main()
