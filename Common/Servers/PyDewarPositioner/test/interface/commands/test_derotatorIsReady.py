import unittest
import mocker
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorIsReadyTest(unittest.TestCase):
    """Test the derotatorIsReady command"""

    def test_IsReady(self):
        dp = DewarPositionerImpl()
        success, answer = dp.command('derotatorIsReady')
        self.assertEqual((success, answer), (True, 'False'))
        success, answer = dp.command('derotatorSetup=KKG')
        success, answer = dp.command('derotatorIsReady')
        self.assertEqual((success, answer), (True, 'True'))

if __name__ == '__main__':
    unittest.main()
