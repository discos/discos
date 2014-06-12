import unittest2
from maciErrType import CannotGetComponentEx

from ComponentErrors import NotAllowedEx
from Acspy.Clients.SimpleClient import PySimpleClient

from DewarPositioner.configuration import CDBConf


class StartTrackingTest(unittest2.TestCase):
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')

    def test_NotAllowedEx(self):
        """Verify it raises a NotAllowedEx"""
        # Raised because no tracking mode is selected
        self.assertRaises(NotAllowedEx, self.positioner.startTracking)
        self.positioner.setTrackingMode('FIXED')
        # Raised because the system is not yet configured (a setup is required)
        self.assertRaises(NotAllowedEx, self.positioner.startTracking)
   
if __name__ == '__main__':
    unittest2.main()
