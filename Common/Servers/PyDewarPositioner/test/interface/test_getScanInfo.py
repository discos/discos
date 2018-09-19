import unittest
import Management
from ComponentErrors import ValidationErrorEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient


class TestGetScanInfo(unittest.TestCase):
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.positioner.setup('KKG')

    def test_notScanning(self):
        info = self.positioner.getScanInfo()
        self.assertEqual(info.axis, Management.MNG_NO_AXIS)


if __name__ == '__main__':
    unittest.main()
