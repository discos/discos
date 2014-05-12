import unittest2
from DewarPositioner.configuration import CDBConf
from ComponentErrors import ValidationErrorEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient


class CDBConfTest(unittest2.TestCase):
    def setUp(self):
        self.cdbconf = CDBConf()

    def test_wrongcode(self):
        """With a wrong code return None"""
        self.assertEqual(self.cdbconf.derotator_name('FOO'), None)

    def test_wrongcode(self):
        """With a wrong code return the derotator name"""
        self.assertEqual(self.cdbconf.derotator_name('KKG'), 'SRTKBandDerotator')

if __name__ == '__main__':
    unittest2.main()
