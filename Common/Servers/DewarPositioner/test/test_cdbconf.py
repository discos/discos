import unittest2
from DewarPositioner.configuration import CDBConf
from ComponentErrors import ValidationErrorEx
from Acspy.Clients.SimpleClient import PySimpleClient


class CDBConfTest(unittest2.TestCase):
    def setUp(self):
        self.cdbconf = CDBConf()
        self.cdbconf.setup('KKG')

    def test_wrongcode(self):
        """With a wrong code raise a ValidationErrorEx"""
        self.assertRaises(ValidationErrorEx, self.cdbconf.get_entry, 'foo')

    def test_rightcode(self):
        """With a wrong code return the derotator name"""
        self.assertEqual(self.cdbconf.get_entry('derotator_name'), 'SRTKBandDerotator')

if __name__ == '__main__':
    unittest2.main()
