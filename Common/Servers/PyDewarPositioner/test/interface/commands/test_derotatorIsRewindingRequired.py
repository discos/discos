import unittest2
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorIsRewindingRequiredTest(unittest2.TestCase):
    """Test the derotatorIsRewindingRequired command"""


    def test_isRewindingRequired(self):
        dp = DewarPositionerImpl()
        dp.command('derotatorSetup=KKG')
        success, answer = dp.command('derotatorIsRewindingRequired')
        self.assertEqual((success, answer), (True, 'False'))


if __name__ == '__main__':
    unittest2.main()
