import unittest2
import time

from ComponentErrors import ValidationErrorEx, OperationErrorEx
from Acspy.Clients.SimpleClient import PySimpleClient

from DewarPositioner.cdbconf import CDBConf


class SetupTest(unittest2.TestCase):
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.derotator = client.getComponent('RECEIVERS/SRTKBandDerotator')

    def test_wrongcode(self):
        """A setup with a wrong code raises a ValidationErrorEx"""
        self.assertRaises(ValidationErrorEx, self.positioner.setup, 'FOOCODE')

    def test_rightcode(self):
        """Verify after a setup the component is properly configured"""
        code = 'KKG'
        try:
            self.positioner.setup(code)
        except OperationErrorEx, ex:
            print "The derotator is not available"
        self.assertEqual(code, self.positioner.getActualSetup())

    def test_default_configuration(self):
        self.positioner.setup('KKG')
        self.assertEqual(self.positioner.getConfiguration(), 'FIXED')

    def test_offset(self):
        """Verify the setup clears the offset."""
        self.positioner.setup('KKG')
        self.positioner.setOffset(1.5)
        self.positioner.setup('KKG')
        self.assertEqual(self.positioner.getOffset(), 0)


if __name__ == '__main__':
    unittest2.main()
