import unittest2
from ComponentErrors import ValidationErrorEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient


class IsMethodsTest(unittest2.TestCase):
    """Test all DewarPositioner.isSomething() methods"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.derotator = client.getComponent('RECEIVERS/SRTKBandDerotator')

    def test_ready(self):
        """Verify the DewarPositioner.isReady() method"""
        self.positioner.setup('KKG')
        self.assertEqual(self.positioner.isReady(), self.derotator.isReady())

if __name__ == '__main__':
    unittest2.main()
