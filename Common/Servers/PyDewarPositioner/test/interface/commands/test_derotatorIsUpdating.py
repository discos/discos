import unittest
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorIsUpdatingTest(unittest.TestCase):
    """Test the derotatorIsUpdating command"""

    def test_isUpdating(self):
        dp = DewarPositionerImpl()
        success, answer = dp.command('derotatorIsUpdating')
        self.assertEqual((success, answer), (True, 'False'))


if __name__ == '__main__':
    unittest.main()
