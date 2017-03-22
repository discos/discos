import unittest2
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorGetRewindingStepTest(unittest2.TestCase):
    """Test the derotoatrClearAutoRewindingSteps command"""

    def test_clearAutoRewindingSteps(self):
        dp = DewarPositionerImpl()
        success, answer = dp.command('derotatorGetRewindingStep') 
        self.assertEqual(success, False)
        dp.command('derotatorSetup=KKG')
        time.sleep(2)
        success, answer = dp.command('derotatorGetRewindingStep') 
        self.assertEqual(success, True)
        self.assertEqual(answer, '60.0d')


if __name__ == '__main__':
    unittest2.main()
