import unittest2
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorGetCmdPositionTest(unittest2.TestCase):
    """Test the derotatorGetCmdPosition command"""

    def test_setupcmd(self):
        dp = DewarPositionerImpl()
        dp.command('derotatorSetup=KKG') # Default conf: FIXED
        success, answer = dp.command('derotatorGetCmdPosition')
        self.assertEqual(success, True)
        value = float(answer.rstrip('d'))
        # The position should be 0 after the setup. In case it is not, wait a bit...
        self.assertAlmostEqual(value, 0, places=2)
        success, answer = dp.command('derotatorSetPosition=2d')
        self.assertEqual(success, True)
        success, answer = dp.command('derotatorGetCmdPosition')
        value = float(answer.rstrip('d'))
        self.assertAlmostEqual(value, 2, places=2)
        dp.command('derotatorPark')

if __name__ == '__main__':
    unittest2.main()
