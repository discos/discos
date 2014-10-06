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
        """Verify isConfiguredForUpdating() and startUpdating()"""
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
        p.setup(siteInfo={}, source=None, device=device)
        # isConfiguredForUpdating() returns False when the system is not configured
        self.assertEqual(p.isConfiguredForUpdating(), False)
        # startUpdating() raises NotAllowedError when updating mode is not yet selected
        self.assertRaises(NotAllowedError, p.startUpdating)
        p.setUpdatingMode('FIXED')
        # isConfiguredForUpdating() returns True after a setUpdatingMode()
        self.assertEqual(p.isConfiguredForUpdating(), True)
        self.assertRaises(NotAllowedError, p.startUpdating)
        with self.assertRaisesRegexp(NotAllowedError, '^no site information'):
            p.startUpdating()
        p.setup(siteInfo={'foo': 'foo'}, source=None, device=device)
        with self.assertRaisesRegexp(NotAllowedError, '^not configured for updating'):
            p.startUpdating()
        p.setUpdatingMode('FIXED')
        with self.assertRaisesRegexp(NotAllowedError, 'no source available'):
            p.startUpdating()

if __name__ == '__main__':
    unittest2.main()
