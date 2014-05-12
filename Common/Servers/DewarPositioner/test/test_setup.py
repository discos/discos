import unittest2
from ComponentErrors import ValidationErrorEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient


class SetupTest(unittest2.TestCase):
    def setUp(self):
        self.client = PySimpleClient().getComponent('RECEIVERS/DewarPositioner')

    def test_wrongcode(self):
        """A setup with a wrong code raises a ValidationErrorEx"""
        self.assertRaises(ValidationErrorEx, self.client.setup, 'FOOCODE')

    def test_not_available(self):
        """When the derotator is not available, a CannotGetComponentEx must be raised"""
        self.assertRaises(CannotGetComponentEx, self.client.setup, 'TEST')

    def test_rightcode(self):
        """Verify after a setup the component is properly configured"""
        code = 'KKG'
        try:
            self.client.setup(code)
        except CannotGetComponentEx, ex:
            print "The derotator is not available"
        self.assertEqual(code, self.client.getActualSetup())


if __name__ == '__main__':
    unittest2.main()
