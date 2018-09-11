import unittest
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorClearAutoRewindingStepsTest(unittest.TestCase):
    """Test the derotoatrClearAutoRewindingSteps command"""

    def test_clearAutoRewindingSteps(self):
        dp = DewarPositionerImpl()
        success, answer = dp.command('derotatorSetAutoRewindingSteps=3') 
        self.assertEqual(success, False)
        dp.command('derotatorSetup=KKG')
        time.sleep(2)
        dp.command('derotatorSetAutoRewindingSteps=3') 
        success, answer = dp.command('derotatorGetAutoRewindingSteps') 
        self.assertEqual(answer, '3')
        dp.command('derotatorClearAutoRewindingSteps') 
        success, answer = dp.command('derotatorGetAutoRewindingSteps') 
        self.assertEqual(answer, '0')


if __name__ == '__main__':
    unittest.main()
