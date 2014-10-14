from __future__ import with_statement
import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositioner.positioner import Positioner, NotAllowedError
from DewarPositioner.cdbconf import CDBConf

class PositionerStartUpdatingTest(unittest2.TestCase):

    def test_notYetConfigured(self):
        """Verify startUpdating()"""
        try:
            client = PySimpleClient()
            device = client.getComponent('RECEIVERS/SRTKBandDerotator')
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from DewarPositionerMockers.mock_components import MockDevice
            device = MockDevice()
        cdbconf = CDBConf()
        cdbconf.attributes['RewindingTimeout'] = 2
        p = Positioner(cdbconf)
        # startUpdating() raises NotAllowedError when the system is not configured
        self.assertRaises(NotAllowedError, p.startUpdating)
        self.assertEqual(p.isConfigured(), False)
        p.setup(siteInfo={}, source=None, device=device)
        # startUpdating() raises NotAllowedError when the system is not configured
        self.assertRaises(NotAllowedError, p.startUpdating)
        cdbconf.setup('KKG')
        cdbconf.setConfiguration('BSC')
        time.sleep(0.1)
        self.assertRaises(NotAllowedError, p.startUpdating)
        with self.assertRaisesRegexp(NotAllowedError, '^no site information available'):
            p.startUpdating()
        p.setup(siteInfo={'foo': 'foo'}, source=None, device=device)
        with self.assertRaisesRegexp(NotAllowedError, 'no source available'):
            p.startUpdating()
        time.sleep(0.1)
        cdbconf.setConfiguration('FIXED')
        with self.assertRaisesRegexp(NotAllowedError, '^dynamic'):
            p.startUpdating()

if __name__ == '__main__':
    unittest2.main()
