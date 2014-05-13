import unittest2
import time
from ComponentErrors import NotAllowedEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient


class GetPositionTest(unittest2.TestCase):
    """Test the DewarPositioner.getPosition() method"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.derotator = client.getComponent('RECEIVERS/SRTKBandDerotator')
        self.positioner.setup('KKG')

    def test_notReady(self):
        """The DewarPositioner must raise a NotAllowedEx when it's not ready"""
        self.positioner.park()
        self.assertRaises(NotAllowedEx, self.positioner.getPosition)

    def test_value(self):
        """Verify it returns the right value"""
        self.derotator.setPosition(1.5)
        time.sleep(2)
        self.assertAlmostEqual(self.positioner.getPosition(), 1.5, places=2)
        self.positioner.park()


if __name__ == '__main__':
    unittest2.main()
