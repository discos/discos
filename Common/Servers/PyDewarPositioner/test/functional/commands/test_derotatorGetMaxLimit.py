import unittest2
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorGetMaxLimitTest(unittest2.TestCase):
    """Test the derotatorGetMaxLimit command"""

    def test_get_max_limit(self):
        dp = DewarPositionerImpl()
        dp.command('derotatorSetup=KKG') # Default conf: FIXED
        success, answer = dp.command('derotatorGetMaxLimit')
        self.assertEqual(success, True)
        self.assertTrue(answer.endswith('d'))

if __name__ == '__main__':
    unittest2.main()
