import unittest
import time
from ComponentErrors import ComponentErrorsEx
from Acspy.Clients.SimpleClient import PySimpleClient


class GenericTest(unittest.TestCase):
    """Test the generic configuration behavior"""

    def setUp(self):
        client = PySimpleClient()
        self.dp = client.getComponent('RECEIVERS/DewarPositioner')
        self.dp.setup('KKG')
    
    def tearDown(self):
        self.dp.park()
        time.sleep(1)

    def test_wrongConfiguration(self):
        """The actual configuration does not have to change"""
        conf = 'BSC'
        self.dp.setConfiguration('BSC')
        self.assertEqual(self.dp.getConfiguration(), 'BSC')
        self.assertRaises(ComponentErrorsEx, self.dp.setConfiguration, 'WRONG')
        self.assertEqual(self.dp.getConfiguration(), 'BSC')


if __name__ == '__main__':
    unittest.main()
