import unittest
import time
from ComponentErrors import ValidationErrorEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient


class IsSlewingTest(unittest.TestCase):
    """Test all DewarPositioner.isSomething() methods"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.derotator = client.getComponent('RECEIVERS/SRTKBandDerotator')

    def tearDown(self):
        self.positioner.park()

    def test_isSlewing(self):
        """Verify the DewarPositioner.isSlewing() method"""
        self.positioner.setup('KKG')
        time.sleep(2)
        self.derotator.setPosition(5.5)
        time.sleep(0.5)
        self.assertEqual(self.derotator.isSlewing(), True)
        self.assertEqual(self.positioner.isSlewing(), self.derotator.isSlewing())
        time.sleep(3)
        self.assertEqual(self.derotator.isSlewing(), False)
        self.assertEqual(self.positioner.isSlewing(), self.derotator.isSlewing())

if __name__ == '__main__':
    unittest.main()
