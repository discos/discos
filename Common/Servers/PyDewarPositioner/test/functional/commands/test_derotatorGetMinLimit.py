import unittest
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorGetMinLimitTest(unittest.TestCase):
    """Test the derotatorGetMinLimit command"""

    def test_get_min_limit(self):
        dp = DewarPositionerImpl()
        dp.command('derotatorSetup=KKG') # Default conf: FIXED
        success, answer = dp.command('derotatorGetMinLimit')
        self.assertEqual(success, True)
        self.assertTrue(answer.endswith('d'))

if __name__ == '__main__':
    unittest.main()
