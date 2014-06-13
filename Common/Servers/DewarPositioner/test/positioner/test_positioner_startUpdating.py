import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError


class PositionerStartUpdatingTest(unittest2.TestCase):

    def test_notYetConfigured(self):
        p = Positioner()
        self.assertRaises(NotAllowedError, p.startUpdating)
        device_name = 'RECEIVERS/SRTKBandDerotator'
        p.setup(device_name)
        self.assertEqual(p.isConfiguredForUpdating(), False)
        self.assertRaises(NotAllowedError, p.startUpdating)
        p.setUpdatingMode('FIXED')
        self.assertEqual(p.isConfiguredForUpdating(), True)
        self.assertEqual(None, p.startUpdating())

if __name__ == '__main__':
    unittest2.main()
