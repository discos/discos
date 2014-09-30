import unittest2
from maciErrType import CannotGetComponentEx

from ComponentErrors import NotAllowedEx
from Acspy.Clients.SimpleClient import PySimpleClient

from DewarPositioner.configuration import CDBConf


class StartUpdatingTest(unittest2.TestCase):
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')

    def test_NotAllowedEx(self):
        """Verify it raises a NotAllowedEx"""
        # Raised because no updating mode is selected
        self.assertRaises(NotAllowedEx, self.positioner.startUpdating)
        self.positioner.setUpdatingMode('FIXED')
        # Raised because the system is not yet configured (a setup is required)
        self.assertRaises(NotAllowedEx, self.positioner.startUpdating)
   
if __name__ == '__main__':
    unittest2.main()
