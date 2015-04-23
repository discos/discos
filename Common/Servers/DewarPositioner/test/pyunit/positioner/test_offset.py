import random
import time
import unittest2
from math import radians
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError
from DewarPositioner.cdbconf import CDBConf
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositionerMockers.mock_components import MockDevice, MockSource


class PositionerOffsetTest(unittest2.TestCase):

    def setUp(self):
        self.cdbconf = CDBConf()
        self.p = Positioner(self.cdbconf)
        self.source = MockSource()
        try:
            client = PySimpleClient()
            self.device = client.getComponent('RECEIVERS/SRTKBandDerotator')
            self.using_mock = False
            print '\nWARNING -> using the real component'
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            self.device = MockDevice()
            self.using_mock = True

    def tearDown(self):
        self.p.park()
        time.sleep(0.2)

    def _test_set_get(self):
        """Verify the set and get methods"""
        # Not allowed when the system is not yet configured
        self.assertRaises(NotAllowedError, self.p.setOffset, 2)
        self.p.setup(siteInfo={}, source=None, device=self.device)
        self.p.setOffset(2)
        self.assertEqual(self.p.getOffset(), 2)
        self.assertEqual(self.p.getPosition(), 2)
        self.p.clearOffset()
        self.assertEqual(self.p.getOffset(), 0)

    def _test_set_new_pos(self):
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

    def test_out_of_range(self):
        """Cause a rewind in case the offset is out of range"""
        self.cdbconf.setup('KKG')
        self.cdbconf.setConfiguration('CUSTOM_OPT')
        az, el, latitude = [radians(50)] * 3
        site_info = {'latitude': latitude}
        self.p.setup(site_info, self.source, self.device)
        self.p.setRewindingMode('AUTO')
        offset = 20
        max_limit = self.device.getMaxLimit() 
        min_limit = self.device.getMinLimit()
        Pis = max_limit - offset/2
        time.sleep(0.2) if self.using_mock else time.sleep(3)
        self.p.setPosition(Pis)
        time.sleep(0.2) # Wait a bit for the setup
        max_rewinding_steps = (max_limit - min_limit) // self.device.getStep()
        expected = Pis - max_rewinding_steps*self.device.getStep() + offset
        self.source.setAzimuth(az)
        self.source.setElevation(el)
        self.p.startUpdating('MNG_TRACK', 'ANT_NORTH', az, el, None, None)
        time.sleep(0.2) if self.using_mock else time.sleep(3)
        self.p.setOffset(offset)
        time.sleep(0.2) if self.using_mock else time.sleep(3)
        self.assertEqual(self.device.getActPosition(), expected)


if __name__ == '__main__':
    unittest2.main()
