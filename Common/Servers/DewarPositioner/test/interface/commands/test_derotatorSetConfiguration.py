import unittest2
import mocker
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorSetConfigurationTest(unittest2.TestCase):
    """Test the derotatorSet[Get]Configuration commands"""

    def test_SetConfiguration(self):
        dp = DewarPositionerImpl()
        success, answer = dp.command('derotatorGetConfiguration')
        self.assertEqual((success, answer), (True, ''))
        dp.command('derotatorSetup=KKG')
        success, answer = dp.command('derotatorSetConfiguration=BSC')
        self.assertEqual(success, True)
        success, answer = dp.command('derotatorGetConfiguration')
        self.assertEqual((success, answer), (True, 'BSC'))
        success, answer = dp.command('derotatorSetConfiguration=FIXED')
        self.assertEqual(success, True)
        success, answer = dp.command('derotatorGetConfiguration')
        self.assertEqual((success, answer), (True, 'FIXED'))

if __name__ == '__main__':
    unittest2.main()
