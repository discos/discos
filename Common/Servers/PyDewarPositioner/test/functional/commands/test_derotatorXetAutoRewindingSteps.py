import unittest
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorXetAutoRewindingStepsTest(unittest.TestCase):
    """Test the derotoatr[Set/Get]AutoRewindingSteps commands"""

    def test_xetAutoRewindingSteps(self):
        dp = DewarPositionerImpl()
        success, answer = dp.command('derotatorSetAutoRewindingSteps=3') 
        self.assertEqual(success, False)
        dp.command('derotatorSetup=KKG')
        time.sleep(2)
        success, answer = dp.command('derotatorSetAutoRewindingSteps=4') 
        self.assertEqual(success, False) # Out of range
        success, answer = dp.command('derotatorGetAutoRewindingSteps') 
        self.assertEqual(int(answer), 0)
        steps='3'
        success, answer = dp.command('derotatorSetAutoRewindingSteps=' + steps) 
        self.assertEqual(success, True)
        success, answer = dp.command('derotatorGetAutoRewindingSteps') 
        self.assertEqual(answer, steps)

if __name__ == '__main__':
    unittest.main()
