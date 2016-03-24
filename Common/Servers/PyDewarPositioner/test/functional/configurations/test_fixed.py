import unittest2
import time
from ComponentErrors import ValidationErrorEx
from Acspy.Clients.SimpleClient import PySimpleClient


class FixedTest(unittest2.TestCase):
    """Test the FIXED Configuration"""

    def setUp(self):
        client = PySimpleClient()
        self.dp = client.getComponent('RECEIVERS/DewarPositioner')
        self.dp.setup('KKG')
    
    def tearDown(self):
        self.dp.park()
        time.sleep(1)

    def test_setPosition(self):
        # Verify we can set the position
        self.dp.setPosition(1.5)
        time.sleep(2)
        self.assertAlmostEqual(self.dp.getPosition(), 1.5, places=1)
        # Vefiry the position does not change aftere a re-configuration
        self.dp.setConfiguration('FIXED')
        self.assertAlmostEqual(self.dp.getPosition(), 1.5, places=1)


if __name__ == '__main__':
    unittest2.main()
