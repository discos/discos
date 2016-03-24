import unittest2
import time
from ComponentErrors import ComponentErrorsEx
from Acspy.Clients.SimpleClient import PySimpleClient


class BSC_OPTTest(unittest2.TestCase):
    """Test the BSC_OPT Configuration"""

    def setUp(self):
        client = PySimpleClient()
        self.dp = client.getComponent('RECEIVERS/DewarPositioner')
        self.dp.setup('KKG')
        self.dp.setConfiguration('BSC_OPT')
    
    def tearDown(self):
        self.dp.park()
        time.sleep(1)

    def test_setConfiguration(self):
        self.assertEqual(self.dp.getConfiguration(), 'BSC_OPT')

    def test_setPosition(self):
        """Verify we can not set the position"""
        self.assertRaises(ComponentErrorsEx, self.dp.setPosition, 1)


if __name__ == '__main__':
    unittest2.main()
