import unittest2
from ComponentErrors import ValidationErrorEx, NotAllowedEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
import time


class ParkTest(unittest2.TestCase):
    """Test the DewarPositioner.park() method"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.derotator = client.getComponent('RECEIVERS/SRTKBandDerotator')
        self.positioner.setup('KKG')

    def test_park_position(self):
        """Verify after a park() the derotator is in the zero position"""
        self.derotator.setPosition(2)
        time.sleep(2)
        self.positioner.park()
        time.sleep(3)
        self.assertAlmostEqual(self.derotator.getActPosition(), 0, places=2)

    def test_configuration(self):
        """Verify after a park() the DewarPositioner is not configured"""
        self.positioner.park()
        self.assertEqual(self.positioner.isConfigured(), False)
        self.assertRaises(NotAllowedEx, self.positioner.park)


if __name__ == '__main__':
    unittest2.main()
