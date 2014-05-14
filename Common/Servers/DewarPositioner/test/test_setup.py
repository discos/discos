import unittest2
import time

from ComponentErrors import ValidationErrorEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient

from DewarPositioner.configuration import CDBConf


class SetupTest(unittest2.TestCase):
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.derotator = client.getComponent('RECEIVERS/SRTKBandDerotator')

    def test_wrongcode(self):
        """A setup with a wrong code raises a ValidationErrorEx"""
        self.assertRaises(ValidationErrorEx, self.positioner.setup, 'FOOCODE')

    def test_not_available(self):
        """When the derotator is not available, a CannotGetComponentEx must be raised"""
        self.assertRaises(CannotGetComponentEx, self.positioner.setup, 'TEST')

    def test_rightcode(self):
        """Verify after a setup the component is properly configured"""
        code = 'KKG'
        try:
            self.positioner.setup(code)
        except CannotGetComponentEx, ex:
            print "The derotator is not available"
        self.assertEqual(code, self.positioner.getActualSetup())

    def test_starting_position(self):
        """Verify the setup puts the derotator in its starting position."""
        self.derotator.setup()
        self.derotator.setPosition(1.5)
        time.sleep(2)

        setup_code = 'KKG'
        cdbconf = CDBConf()
        cdbconf.setup(setup_code)
        self.positioner.setup(setup_code)
        time.sleep(2)

        self.assertAlmostEqual(
                cdbconf.get_entry('starting_position'), 
                self.derotator.getActPosition()
        )


if __name__ == '__main__':
    unittest2.main()
