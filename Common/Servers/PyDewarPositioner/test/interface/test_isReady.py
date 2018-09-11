import unittest
from ComponentErrors import ValidationErrorEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient


class IsReadyTest(unittest.TestCase):
    """Test all DewarPositioner.isSomething() methods"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.derotator = client.getComponent('RECEIVERS/SRTKBandDerotator')
        self.positioner.setup('KKG')

    def test_ready(self):
        """Verify the DewarPositioner.isReady() method"""
        self.assertEqual(self.positioner.isReady(), self.derotator.isReady())

    def test_after_park(self):
        """Verify the DewarPositioner.isReady() returns false after a park()"""
        self.positioner.park()
        self.assertEqual(self.positioner.isReady(), False)


if __name__ == '__main__':
    unittest.main()
