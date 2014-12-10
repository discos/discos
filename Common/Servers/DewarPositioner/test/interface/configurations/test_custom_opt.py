import unittest2
import time
from ComponentErrors import ValidationErrorEx, NotAllowedEx
from Acspy.Clients.SimpleClient import PySimpleClient


class CustomTest(unittest2.TestCase):
    """Test the CUSTOM Configuration"""

    def setUp(self):
        client = PySimpleClient()
        self.dp = client.getComponent('RECEIVERS/DewarPositioner')
        self.dp.setup('KKG')
    
    def tearDown(self):
        self.dp.park()
        time.sleep(0.5)

    def test_setConfiguration(self):
        self.dp.setConfiguration('CUSTOM_OPT')
        self.assertEqual(self.dp.getConfiguration(), 'CUSTOM_OPT')

    def test_setPosition(self):
        """Verify we can set the position"""
        self.dp.setConfiguration('CUSTOM_OPT')
        self.dp.setPosition(1)


if __name__ == '__main__':
    unittest2.main()
