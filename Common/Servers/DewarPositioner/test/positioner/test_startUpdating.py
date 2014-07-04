from __future__ import with_statement
import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositioner.positioner import Positioner, NotAllowedError

class PositionerStartUpdatingTest(unittest2.TestCase):

    def test_notYetConfigured(self):
        try:
            client = PySimpleClient()
            device = client.getComponent('RECEIVERS/SRTKBandDerotator')
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from DewarPositionerTest.mock_components import MockDevice
            device = MockDevice()
        cdb_info = {
                'updating_time': 0.1,
                'rewinding_timeout': 1.5,
                'rewinding_sleep_time': 1
        }
        p = Positioner(cdb_info)
        self.assertRaises(NotAllowedError, p.startUpdating)
        p.setup(site_info={}, source=None, device=device)
        self.assertEqual(p.isConfiguredForUpdating(), False)
        self.assertRaises(NotAllowedError, p.startUpdating)
        p.setUpdatingMode('FIXED')
        self.assertEqual(p.isConfiguredForUpdating(), True)
        self.assertRaises(NotAllowedError, p.startUpdating)
        with self.assertRaisesRegexp(NotAllowedError, '^no site information'):
            p.startUpdating()
        p.setup(site_info={'foo': 'foo'}, source=None, device=device)
        with self.assertRaisesRegexp(NotAllowedError, '^not configured for updating'):
            p.startUpdating()
        p.setUpdatingMode('FIXED')
        with self.assertRaisesRegexp(NotAllowedError, 'no source available'):
            p.startUpdating()

if __name__ == '__main__':
    unittest2.main()
