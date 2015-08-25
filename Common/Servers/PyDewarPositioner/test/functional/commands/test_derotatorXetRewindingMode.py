import unittest2
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorXetRewindingModeTest(unittest2.TestCase):
    """Test the derotoatr[Set/Get]RewindingMode commands"""

    def test_xetRewindingMode(self):
        dp = DewarPositionerImpl()
        # Verify the case of wrong mode in input
        success, answer = dp.command('derotatorSetRewindingMode=FOO') 
        self.assertEqual(success, False)
        # Verify the AUTO and MANUAL rewinding mode
        for mode in ('AUTO', 'MANUAL'):
            success, answer = dp.command('derotatorSetRewindingMode=%s' %mode) 
            self.assertEqual(success, True) 
            success, answer = dp.command('derotatorGetRewindingMode') 
            self.assertEqual(answer, mode)


if __name__ == '__main__':
    unittest2.main()
