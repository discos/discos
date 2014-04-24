import unittest
from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import ComponentErrorsEx


class SetupTest(unittest.TestCase):
    def setUp(self):
        self.client = PySimpleClient().getComponent('RECEIVERS/DewarPositioner')

    def test_rightcode(self):
        """Verify after a setup the component is properly configured"""
        code = 'KKG'
        try:
            self.client.setup(code)
        except ComponentErrorsEx, ex:
            pass # It's OK, the derotator is not available
        self.assertEqual(code, self.client.getActualSetup())

    def test_wrongcode(self):
        """Verify a setup with a wrong code raises a ComponentErrorEx"""
        self.assertRaises(ComponentErrorsEx, self.client.setup, 'FOO')


if __name__ == '__main__':
    unittest.main()
