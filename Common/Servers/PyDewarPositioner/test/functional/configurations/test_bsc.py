import unittest2
import time
from ComponentErrors import ComponentErrorsEx
from Acspy.Clients.SimpleClient import PySimpleClient


class BSCTest(unittest2.TestCase):
    """Test the BSC Configuration"""

    def setUp(self):
        client = PySimpleClient()
        self.dp = client.getComponent('RECEIVERS/DewarPositioner')
        self.dp.setup('KKG')
    
    def tearDown(self):
        self.dp.park()
        time.sleep(1)

    def test_setConfiguration(self):
        self.dp.setConfiguration('BSC')
        self.assertEqual(self.dp.getConfiguration(), 'BSC')

    def test_setPosition(self):
        """Verify we can not set the position"""
        self.dp.setConfiguration('BSC')
        self.assertRaises(ComponentErrorsEx, self.dp.setPosition, 1)


if __name__ == '__main__':
    unittest2.main()
