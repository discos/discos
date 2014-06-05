import unittest2
from ComponentErrors import ValidationErrorEx
from Acspy.Clients.SimpleClient import PySimpleClient


class ModeTest(unittest2.TestCase):
    """Test the DewarPositioner.*Mode() methods"""

    def setUp(self):
        client = PySimpleClient()
        self.dp = client.getComponent('RECEIVERS/DewarPositioner')

    def test_wrong_mode(self):
        """The set* methods must raise a ValidationErrorEx in case of wrong mode"""
        self.assertRaises(ValidationErrorEx, self.dp.setRewindingMode, 'FOO')
        self.assertRaises(ValidationErrorEx, self.dp.setTrackingMode, 'FOO')

    def test_right_mode(self):
        """The argument of set* must be returned by get* in case of right mode"""
        self.dp.setRewindingMode('AUTO')
        self.assertEqual(self.dp.getRewindingMode(), 'AUTO')
        self.dp.setRewindingMode('MANUAL')
        self.assertEqual(self.dp.getRewindingMode(), 'MANUAL')
        self.dp.setTrackingMode('FIXED')
        self.assertEqual(self.dp.getTrackingMode(), 'FIXED')
        self.dp.setTrackingMode('OPTIMIZED')
        self.assertEqual(self.dp.getTrackingMode(), 'OPTIMIZED')


if __name__ == '__main__':
    unittest2.main()
