import unittest2
from ComponentErrors import ValidationErrorEx, NotAllowedEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
import time


class isConfiguredTest(unittest2.TestCase):
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.positioner.setup('KKG')

    def test_isConfiguredForX(self):
        """Test the DewarPositioner.isConfiguredFor* methods"""
        self.assertEqual(self.positioner.isConfiguredForRewinding(), True)
        self.assertEqual(self.positioner.isConfiguredForUpdating(), False)
        self.positioner.setUpdatingMode('OPTIMIZED')
        self.assertEqual(self.positioner.isConfiguredForUpdating(), True)
        self.positioner.park()
        self.assertEqual(self.positioner.isConfiguredForUpdating(), False)

if __name__ == '__main__':
    unittest2.main()
