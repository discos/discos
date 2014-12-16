import time
import unittest2
from ComponentErrors import ComponentErrorsEx, ComponentErrorsEx
from Acspy.Clients.SimpleClient import PySimpleClient

from DewarPositioner.cdbconf import CDBConf
from Management import MNG_WARNING, MNG_FAILURE, MNG_OK


class MngStatusTest(unittest2.TestCase):

    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')

    def tearDown(self):
        self.positioner.park()

    def test_setup(self):
        self.assertEqual(self.positioner.getManagementStatus(), MNG_WARNING)
        self.positioner.setup('KKG')
        time.sleep(1)
        self.assertEqual(self.positioner.getManagementStatus(), MNG_OK)

if __name__ == '__main__':
    unittest2.main()
