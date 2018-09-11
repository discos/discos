import unittest
import time
from ComponentErrors import ValidationErrorEx, NotAllowedEx
from Acspy.Clients.SimpleClient import PySimpleClient


class CustomOPTTest(unittest.TestCase):
    """Test the CUSTOM_OPT Configuration"""

    def setUp(self):
        client = PySimpleClient()
        self.dp = client.getComponent('RECEIVERS/DewarPositioner')
        self.dp.setup('KKG')
        self.dp.setConfiguration('CUSTOM_OPT')
    
    def tearDown(self):
        self.dp.park()
        time.sleep(0.5)

    def test_setConfiguration(self):
        self.assertEqual(self.dp.getConfiguration(), 'CUSTOM_OPT')

    def test_setPosition(self):
        """Verify we can set the position"""
        self.dp.setPosition(1)


if __name__ == '__main__':
    unittest.main()
